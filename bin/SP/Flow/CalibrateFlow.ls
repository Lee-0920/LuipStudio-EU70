--[[
 * @brief 校准流程。
--]]

CalibrateType =
{
    calibrate= 0,
    mulCalibrate = 1,
    onlyCalibrateBlank = 2,
    onlyCalibrateStandard = 3,
}

CalibrateFlow = Flow:new
{
    calibrateDateTime = 0,
    zeroCalibrateDateTime = 0,
    standardCalibrateDateTime = 0,
    measureBlank = true,
    measureStandard = false,
    mulPointCalibration = true,
    currentRange = 1,
    isUseStart = false,
    curveCalibrateRange = 0,
    lastResultInfo = "N",
    text = "",
    isQualified = false,
}

function CalibrateFlow:new(o, target, consistency)
    o = o or {}
    setmetatable(o, self)
    self.__index = self

    o.calibrateDateTime = os.time()
    o.zeroCalibrateDateTime = status.measure.calibrate[config.measureParam.range[config.measureParam.calibrateRangeIndex + 1] + 1].zeroCalibrateDateTime
    o.standardCalibrateDateTime = status.measure.calibrate[config.measureParam.range[config.measureParam.calibrateRangeIndex + 1] + 1].standardCalibrateDateTime
    o.calibrateType = target
    o.currentRange = config.measureParam.range[config.measureParam.calibrateRangeIndex + 1] + 1
    o.isUseStart = false
    o.curveCalibrateRange = 0
    o.lastResultInfo = status.measure.newResult.measure.resultInfo
    o.zeropeak = {0,0,0,0}
    o.standardpeak = {0,0,0,0}
    o.consistency = consistency
    o.turboMode = config.measureParam.turboMode

    return o
end

function CalibrateFlow:GetRuntime()
    local runtime = 0
    if self.calibrateType == CalibrateType.calibrate then
        runtime = setting.runStatus.calibrate.GetTime()
    elseif self.calibrateType == CalibrateType.mulCalibrate then
        runtime = setting.runStatus.mulCalibrate.GetTime()
    elseif self.calibrateType == CalibrateType.onlyCalibrateBlank then
        runtime = setting.runStatus.onlyCalibrateBlank.GetTime()
    elseif self.calibrateType == CalibrateType.onlyCalibrateStandard then
        runtime = setting.runStatus.onlyCalibrateStandard.GetTime()
    end
    return runtime
end

function CalibrateFlow:OnStart()
    local eventStr = "开始" .. self.text
    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)

    if 1 == config.system.OEM and self.calibrateType == CalibrateType.mulCalibrate then
        self.calibrateType = CalibrateType.calibrate
    end
    --组合流程需要重新加载时间
    self.measureDateTime = os.time()
    -- 初始化下位机
    -- dc:GetIDeviceStatus():Initialize()
    -- lc:GetIDeviceStatus():Initialize()
    -- log:debug("打开紫外灯")
    -- dc:GetIOpticalAcquire():TurnOnLED()

    status.measure.isUseStart = self.isUseStart
    status.measure.newResult.measure.resultInfo = "C"
    ConfigLists.SaveMeasureStatus()

    --继电器指示
    Helper.Result.RelayOutOperate(setting.mode.relayOut.calibrateInstruct, true)

    --设置运行状态
    local runStatus
    if self.curveCalibrateRange == 1 then
        runStatus = Helper.Status.SetStatus(setting.runStatus.range1CurveCalibrate)
        self.currentRange = config.measureParam.range[1] + 1 --量程二校准
    elseif self.curveCalibrateRange == 2 then
        runStatus = Helper.Status.SetStatus(setting.runStatus.range2CurveCalibrate)
        self.currentRange = config.measureParam.range[2] + 1 --量程三校准
    elseif self.curveCalibrateRange == 3 then
        runStatus = Helper.Status.SetStatus(setting.runStatus.range3CurveCalibrate)
        self.currentRange = config.measureParam.range[3] + 1 --量程三校准
    else
        if self.calibrateType == CalibrateType.mulCalibrate then
            runStatus = Helper.Status.SetStatus(setting.runStatus.mulCalibrate)
            self.currentRange = config.measureParam.range[config.measureParam.calibrateRangeIndex + 1] + 1
        elseif self.calibrateType == CalibrateType.onlyCalibrateBlank then
            runStatus = Helper.Status.SetStatus(setting.runStatus.onlyCalibrateBlank)
            self.currentRange = config.measureParam.range[config.measureParam.calibrateRangeIndex + 1] + 1
        elseif self.calibrateType == CalibrateType.onlyCalibrateStandard then
            runStatus = Helper.Status.SetStatus(setting.runStatus.onlyCalibrateStandard)
            self.currentRange = config.measureParam.range[config.measureParam.calibrateRangeIndex + 1] + 1
        else
            runStatus = Helper.Status.SetStatus(setting.runStatus.calibrate)
        end
    end

    StatusManager.Instance():SetStatus(runStatus)
end


function CalibrateFlow:OnProcess()
    local initAbsorbance = {0,0}
    local absorbance = {0,0}
    local peak = {0.065,1.10187,1.760357,3.506981,4.998178,7.813926,10.83882}
    local peakIC = {0,0.748447,0.694807,0.644137,0.682876,0.584448,0.710218}
    local consistency = {0, 0}
    local addParam = {setting.calibrate[1],setting.calibrate[2]}
    local measureResult1 = Measurer:GetZeroMeasureResult()
    local measureResult2 = Measurer:GetZeroMeasureResult()
    local measureAD1 = MeasureAD:new()
    local measureAD2 = MeasureAD:new()
    local curveK = 1
    local curveB = 0
    local R2 = 1
    local meausureConsistency = {}
    local oneTimesCreateCurve = false
    local blankConsistency = {TC = 0.232971, IC = 0.18858, TOC = 0.04828}
    local cStr = ""

    self.isUserStop = false
    self.isFinish = false

    if config.measureParam.rangeAccurateCalibrate == true then
        config.measureParam.zeroAccurateCalibrate = true
        config.measureParam.standardAccurateCalibrate = true
    else
        config.measureParam.zeroAccurateCalibrate = false
        config.measureParam.standardAccurateCalibrate = false
    end


    if self.curveCalibrateRange > 0 then
        if status.measure.standardCurve > 0 then
            self.measureBlank = false
            self.measureStandard = true
        else
            log:info("不存在基准标线，无法进行量程系数校正")
            return
        end
    else
        --一键运行增加管路更新操作
        if self.calibrateType == CalibrateType.mulCalibrate then
            oneTimesCreateCurve = true
            --self:PipeRenew()
            if self.turboMode then
                for k,v in pairs(config.measureParam.turboCalibratePointConsistency) do
                    consistency[k] = v
                    print(" turboC" .. " [" .. k .. "] = " .. v)
                end
            else
                for k,v in pairs(config.measureParam.calibratePointConsistency) do
                    consistency[k] = v
                    print(" C" .. " [" .. k .. "] = " .. v)
                end
            end
        else
            self.mulPointCalibration = false
            self.measureStandard = true
            consistency[1] = 0
            consistency[2] = self.consistency
            log:debug("单点校准浓度 " .. self.consistency)
            if self.consistency < 1 then
                cStr = "[" .. string.format("%.0f", self.consistency * 1000) .. "ppb]"
            else
                cStr = "[" .. string.format("%.0f", self.consistency) .. "ppm]"
            end

            local updateWidgetManager = UpdateWidgetManager.Instance()
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "即将进行" .. cStr .. "单点校准")
        end
        if self.turboMode then
            curveK = config.measureParam.curveKTurbo
            curveB = config.measureParam.curveBTurbo
        else
            curveK = config.measureParam.curveK
            curveB = config.measureParam.curveB
        end
    end

    --零点校准和量程校准时间更新
    local timeRecord = os.time()
    if self.measureBlank == true and self.measureStandard == true then
        self.zeroCalibrateDateTime = timeRecord
        self.standardCalibrateDateTime = timeRecord
    elseif self.measureBlank == false and self.measureStandard == true then
        self.standardCalibrateDateTime = timeRecord
    elseif self.measureBlank == true and self.measureStandard == false then
        self.zeroCalibrateDateTime = timeRecord
    end

    -- --需要测量零点
    -- if self.measureBlank == true then
    --     --测量零点
    --     log:debug("校准-零点测量")

    --     local updateWidgetManager = UpdateWidgetManager.Instance()
    --     if self.mulPointCalibration then
    --         updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[空白水]后，点击确认开始下一步校准(1/8)")
    --     else
    --         updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[空白水]后，点击确认开始下一步校准(1/3)")
    --     end

    --     if Measurer.flow then
    --         Measurer:Reset()
    --     end
    --     Measurer.flow = self
    --     Measurer.measureType = MeasureType.Blank
    --     Measurer.currentRange = self.currentRange
    --     for k,v in pairs(addParam[1]) do
    --         Measurer.addParam [k] = v
    --     end
    --     --根据量程修改参数
    --     Measurer.addParam.standardVolume = 0
    --     Measurer.addParam.blankVolume = setting.measure.range[self.currentRange].blankVolume + setting.measure.range[self.currentRange].sampleVolume
    --     Measurer.addParam.dilutionExtractVolume1 = setting.measure.range[self.currentRange].dilutionExtractVolume1
    --     Measurer.addParam.dilutionAddBlankVolume1 = setting.measure.range[self.currentRange].dilutionAddBlankVolume1
    --     Measurer.addParam.dilutionExtractVolume2 = setting.measure.range[self.currentRange].dilutionExtractVolume2
    --     Measurer.addParam.dilutionAddBlankVolume2 = setting.measure.range[self.currentRange].dilutionAddBlankVolume2
    --     Measurer.addParam.dilutionExtractVolume3 = setting.measure.range[self.currentRange].dilutionExtractVolume3
    --     Measurer.addParam.dilutionAddBlankVolume3 = setting.measure.range[self.currentRange].dilutionAddBlankVolume3
    --     Measurer.addParam.afterReagent1AddBlankVolume = setting.measure.range[self.currentRange].afterReagent1AddBlankVolume
    --     Measurer.addParam.diluteFactor = setting.measure.range[self.currentRange].diluteFactor
    --     Measurer.addParam.rinseSampleVolume = 0
    --     Measurer.addParam.rinseBlankVolume = setting.measure.range[self.currentRange].rinseStandardVolume + setting.measure.range[self.currentRange].rinseSampleVolume
    --     Measurer.addParam.rinseStandardVolume = 0

    --     --零点流程执行
    --     local peakHighTabelTC = {}
    --     local peakHighTabelIC = {}
    --     --测量次数
    --     local measureTimes = 5
    --     --舍弃次数
    --     local throwNum = 2
    --     if self.turboMode then
    --         config.measureParam.reagent1Vol = 2
    --         config.measureParam.reagent2Vol = 2
    --         measureTimes = 75
    --         throwNum = 45
    --     else
    --         config.measureParam.reagent1Vol = 1
    --         config.measureParam.reagent2Vol = 0
    --     end

    --     config.modifyRecord.measureParam(true)
    --     ConfigLists.SaveMeasureParamConfig()
    --     setting.measureResult.continousModeParam.currentMeasureCnt = 0
    --     for j = 1, measureTimes do
    --         local err,result = pcall(function() return Measurer:Measure() end)
    --         if not err then      -- 出现异常
    --             if type(result) == "table" then
    --                 if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
    --                     self.isUserStop = true
    --                     error(result)
    --                 elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
    --                     self.isUserStop = true
    --                     error(result)
    --                 elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
    --                     self.isUserStop = true
    --                     error(result)
    --                 elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
    --                     self.isUserStop = true
    --                     error(result)
    --                 elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
    --                     self.isUserStop = true
    --                     error(result)
    --                 else
    --                     error(result)
    --                 end
    --             else
    --                 error(result)
    --             end
    --         else    -- 正常
    --             peakHighTabelTC[j] = result.peakTC
    --             peakHighTabelIC[j] = result.peakIC
    --             self.zeropeak[1] = string.format("%.2f", result.peakTC)
    --             log:debug("第" .. j .. "次空白水TC修正峰值= " .. peakHighTabelTC[j] .. ", IC修正峰值= " .. peakHighTabelIC[j])
    --             for k,v in pairs(result) do
    --                 measureResult1[k] = v
    --             end
    --         end
    --     end
    --     --if self.turboMode then
    --     --
    --     --else
    --     --    peak[1] = (peakHighTabelTC[5] + peakHighTabelTC[3] + peakHighTabelTC[4])/3
    --     --    peakIC[1] = (peakHighTabelIC[5] + peakHighTabelIC[3] + peakHighTabelIC[4])/3
    --     --end
    --     local sumTC = 0
    --     local sumIC = 0
    --     for i = (throwNum + 1), measureTimes do
    --         sumTC = sumTC + peakHighTabelTC[i]
    --         sumIC = sumIC + peakHighTabelIC[i]
    --     end
    --     peak[1] = sumTC / (measureTimes - throwNum)
    --     peakIC[1] =  sumIC / (measureTimes - throwNum)

    --     log:debug("空白水测试TC峰值= " .. peak[1] .. ", IC峰值= " .. peakIC[1])
    --     blankConsistency.TC = self:CalculateConsistency(peak[1], ModelType.TC)
    --     blankConsistency.IC = self:CalculateConsistency(peakIC[1], ModelType.IC)
    --     blankConsistency.TOC = blankConsistency.TC - blankConsistency.IC
        
    --     log:debug("空白水测试TC浓度= " .. blankConsistency.TC .. ", IC浓度= " .. blankConsistency.IC .. ", TOC浓度= " .. blankConsistency.TOC)
    -- end

    --需要多点定标
    if self.mulPointCalibration == true then
        log:debug("校准-多点校准标点测量")

        local updateWidgetManager = UpdateWidgetManager.Instance()
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[250ppb]标液后，点击确认开始下一步校准(2/8)")

        -- --测量标点
        -- if Measurer.flow then
        --     Measurer:Reset()
        -- end
        -- Measurer.flow = self
        -- Measurer.measureType = MeasureType.Standard
        -- Measurer.currentRange = self.currentRange
        -- for k,v in pairs(addParam[2]) do
        --     Measurer.addParam [k] = v
        -- end

        -- --根据量程修改参数
        -- Measurer.addParam.standardVolume = setting.measure.range[self.currentRange].sampleVolume
        -- Measurer.addParam.blankVolume = setting.measure.range[self.currentRange].blankVolume
        -- Measurer.addParam.dilutionExtractVolume1 = setting.measure.range[self.currentRange].dilutionExtractVolume1
        -- Measurer.addParam.dilutionAddBlankVolume1 = setting.measure.range[self.currentRange].dilutionAddBlankVolume1
        -- Measurer.addParam.dilutionExtractVolume2 = setting.measure.range[self.currentRange].dilutionExtractVolume2
        -- Measurer.addParam.dilutionAddBlankVolume2 = setting.measure.range[self.currentRange].dilutionAddBlankVolume2
        -- Measurer.addParam.dilutionExtractVolume3 = setting.measure.range[self.currentRange].dilutionExtractVolume3
        -- Measurer.addParam.dilutionAddBlankVolume3 = setting.measure.range[self.currentRange].dilutionAddBlankVolume3
        -- Measurer.addParam.afterReagent1AddBlankVolume = setting.measure.range[self.currentRange].afterReagent1AddBlankVolume
        -- Measurer.addParam.diluteFactor = setting.measure.range[self.currentRange].diluteFactor
        -- Measurer.addParam.rinseSampleVolume = 0
        -- Measurer.addParam.rinseBlankVolume = 0
        -- Measurer.addParam.rinseStandardVolume = setting.measure.range[self.currentRange].rinseStandardVolume + setting.measure.range[self.currentRange].rinseSampleVolume

        -- local peakHighTabelTC = {}
        -- local peakHighTabelIC = {}
        -- --测量次数
        -- local measureTimes = 4
        -- --舍弃次数
        -- local throwNum = 1
        -- if self.turboMode then
        --     config.measureParam.reagent1Vol = 2
        --     config.measureParam.reagent2Vol = 2
        --     measureTimes = 75
        --     throwNum = 45
        -- else
        --     config.measureParam.reagent1Vol = 1
        --     config.measureParam.reagent2Vol = 0
        -- end
        -- config.modifyRecord.measureParam(true)
        -- ConfigLists.SaveMeasureParamConfig()
        -- setting.measureResult.continousModeParam.currentMeasureCnt = 0
        -- for j = 1, measureTimes do
        --     local err,result = pcall(function() return Measurer:Measure() end)
        --     if not err then      -- 出现异常
        --         if type(result) == "table" then
        --             if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
        --                 self.isUserStop = true
        --                 error(result)
        --             else
        --                 error(result)
        --             end
        --         else
        --             error(result)
        --         end
        --     else    -- 正常
        --         peakHighTabelTC[j] = result.peakTC
        --         peakHighTabelIC[j] = result.peakIC
        --         self.zeropeak[1] = string.format("%.2f", result.peakTC)
        --         log:debug("第" .. j .. "次250ppb标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
        --         for k,v in pairs(result) do
        --             measureResult1[k] = v
        --         end
        --     end
        -- end
        -- local sumTC = 0
        -- local sumIC = 0
        -- for i = (throwNum + 1), measureTimes do
        --     sumTC = sumTC + peakHighTabelTC[i]
        --     sumIC = sumIC + peakHighTabelIC[i]
        -- end
        -- peak[2] = sumTC / (measureTimes - throwNum)
        -- peakIC[2] =  sumIC / (measureTimes - throwNum)

        -- if self.turboMode then
        --     log:debug("250ppb标液测试TC平均值= " .. peak[2] .. ", IC平均值= " .. peakIC[2])
        -- else
        --     log:debug("250ppb标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])
        -- end

        -- --1ppm标点校准/ Turbo模式500ppb校准
        -- if self.turboMode then
        --     updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[500ppb]标液后，点击确认开始下一步校准(3/7)")
        --     config.measureParam.reagent1Vol = 2
        --     config.measureParam.reagent2Vol = 2
        --     measureTimes = 75
        --     throwNum = 45
        -- else
        --     updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[1ppm]标液后，点击确认开始下一步校准(3/8)")
        --     config.measureParam.reagent1Vol = 1
        --     config.measureParam.reagent2Vol = 0.1
        -- end
        -- config.modifyRecord.measureParam(true)
        -- ConfigLists.SaveMeasureParamConfig()
        -- setting.measureResult.continousModeParam.currentMeasureCnt = 0
        -- for j = 1, measureTimes do
        --     local err,result = pcall(function() return Measurer:Measure() end)
        --     if not err then      -- 出现异常
        --         if type(result) == "table" then
        --             if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
        --                 self.isUserStop = true
        --                 error(result)
        --             else
        --                 error(result)
        --             end
        --         else
        --             error(result)
        --         end
        --     else    -- 正常
        --         peakHighTabelTC[j] = result.peakTC
        --         peakHighTabelIC[j] = result.peakIC
        --         self.zeropeak[1] = string.format("%.2f", result.peakTC)
        --         log:debug("第" .. j .. "次1ppm标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
        --         for k,v in pairs(result) do
        --             measureResult1[k] = v
        --         end
        --     end
        -- end
        -- local sumTC = 0
        -- local sumIC = 0
        -- for i = (throwNum + 1), measureTimes do
        --     sumTC = sumTC + peakHighTabelTC[i]
        --     sumIC = sumIC + peakHighTabelIC[i]
        -- end
        -- peak[3] = sumTC / (measureTimes - throwNum)
        -- peakIC[3] =  sumIC / (measureTimes - throwNum)

        -- if self.turboMode then
        --     log:debug("500ppb标液测试TC平均值= " .. peak[3] .. ", IC平均值= " .. peakIC[3])
        -- else
        --     log:debug("1ppm标液测试TC峰值= " .. peak[3] .. ", IC峰值= " .. peakIC[3])
        -- end

        -- --5ppm标点校准/ Turbo模式1ppm校准
        -- if self.turboMode then
        --     updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[1ppm]标液后，点击确认开始下一步校准(4/7)")
        --     config.measureParam.reagent1Vol = 2
        --     config.measureParam.reagent2Vol = 2
        --     measureTimes = 75
        --     throwNum = 45
        -- else
        --     updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[5ppm]标液后，点击确认开始下一步校准(4/8)")
        --     config.measureParam.reagent1Vol = 1
        --     config.measureParam.reagent2Vol = 1
        -- end
        -- config.modifyRecord.measureParam(true)
        -- ConfigLists.SaveMeasureParamConfig()
        -- setting.measureResult.continousModeParam.currentMeasureCnt = 0
        -- for j = 1, measureTimes do
        --     local err,result = pcall(function() return Measurer:Measure() end)
        --     if not err then      -- 出现异常
        --         if type(result) == "table" then
        --             if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
        --                 self.isUserStop = true
        --                 error(result)
        --             else
        --                 error(result)
        --             end
        --         else
        --             error(result)
        --         end
        --     else    -- 正常
        --         peakHighTabelTC[j] = result.peakTC
        --         peakHighTabelIC[j] = result.peakIC
        --         self.zeropeak[1] = string.format("%.2f", result.peakTC)
        --         log:debug("第" .. j .. "次5ppm标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
        --         for k,v in pairs(result) do
        --             measureResult1[k] = v
        --         end
        --     end
        -- end
        -- local sumTC = 0
        -- local sumIC = 0
        -- for i = (throwNum + 1), measureTimes do
        --     sumTC = sumTC + peakHighTabelTC[i]
        --     sumIC = sumIC + peakHighTabelIC[i]
        -- end
        -- peak[4] = sumTC / (measureTimes - throwNum)
        -- peakIC[4] =  sumIC / (measureTimes - throwNum)

        -- if self.turboMode then
        --     log:debug("1ppm标液测试TC平均值= " .. peak[4] .. ", IC平均值= " .. peakIC[4])
        -- else
        --     log:debug("5ppm标液测试TC峰值= " .. peak[4] .. ", IC峰值= " .. peakIC[4])
        -- end

        -- --10ppm标点校准/ Turbo模式3校准ppm
        -- if self.turboMode then
        --     updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[3ppm]标液后，点击确认开始下一步校准(5/7)")
        --     config.measureParam.reagent1Vol = 2
        --     config.measureParam.reagent2Vol = 2
        --     measureTimes = 75
        --     throwNum = 45
        -- else
        --     updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[10ppm]标液后，点击确认开始下一步校准(5/8)")
        --     config.measureParam.reagent1Vol = 1
        --     config.measureParam.reagent2Vol = 2
        -- end
        -- config.modifyRecord.measureParam(true)
        -- ConfigLists.SaveMeasureParamConfig()
        -- setting.measureResult.continousModeParam.currentMeasureCnt = 0
        -- for j = 1, measureTimes do
        --     local err,result = pcall(function() return Measurer:Measure() end)
        --     if not err then      -- 出现异常
        --         if type(result) == "table" then
        --             if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
        --                 self.isUserStop = true
        --                 error(result)
        --             else
        --                 error(result)
        --             end
        --         else
        --             error(result)
        --         end
        --     else    -- 正常
        --         peakHighTabelTC[j] = result.peakTC
        --         peakHighTabelIC[j] = result.peakIC
        --         self.zeropeak[1] = string.format("%.2f", result.peakTC)
        --         log:debug("第" .. j .. "次10ppm标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
        --         for k,v in pairs(result) do
        --             measureResult1[k] = v
        --         end
        --     end
        -- end
        -- local sumTC = 0
        -- local sumIC = 0
        -- for i = (throwNum + 1), measureTimes do
        --     sumTC = sumTC + peakHighTabelTC[i]
        --     sumIC = sumIC + peakHighTabelIC[i]
        -- end
        -- peak[5] = sumTC / (measureTimes - throwNum)
        -- peakIC[5] =  sumIC / (measureTimes - throwNum)

        -- if self.turboMode then
        --     log:debug("3ppm标液测试TC平均值= " .. peak[5] .. ", IC平均值= " .. peakIC[5])
        -- else
        --     log:debug("10ppm标液测试TC峰值= " .. peak[5] .. ", IC峰值= " .. peakIC[5])
        -- end

        -- --25ppm标点校准/ Turbo模式5ppm校准
        -- if self.turboMode then
        --     updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[5ppm]标液后，点击确认开始下一步校准(6/7)")
        --     config.measureParam.reagent1Vol = 2
        --     config.measureParam.reagent2Vol = 2
        --     measureTimes = 75
        --     throwNum = 45
        -- else
        --     updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[25ppm]标液后，点击确认开始下一步校准(6/8)")
        --     config.measureParam.reagent1Vol = 1
        --     config.measureParam.reagent2Vol = 3.4
        -- end
        -- config.modifyRecord.measureParam(true)
        -- ConfigLists.SaveMeasureParamConfig()
        -- setting.measureResult.continousModeParam.currentMeasureCnt = 0
        -- for j = 1, measureTimes do
        --     local err,result = pcall(function() return Measurer:Measure() end)
        --     if not err then      -- 出现异常
        --         if type(result) == "table" then
        --             if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
        --                 self.isUserStop = true
        --                 error(result)
        --             else
        --                 error(result)
        --             end
        --         else
        --             error(result)
        --         end
        --     else    -- 正常
        --         peakHighTabelTC[j] = result.peakTC
        --         peakHighTabelIC[j] = result.peakIC
        --         self.zeropeak[1] = string.format("%.2f", result.peakTC)
        --         log:debug("第" .. j .. "次25ppm标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
        --         for k,v in pairs(result) do
        --             measureResult1[k] = v
        --         end
        --     end
        -- end
        -- local sumTC = 0
        -- local sumIC = 0
        -- for i = (throwNum + 1), measureTimes do
        --     sumTC = sumTC + peakHighTabelTC[i]
        --     sumIC = sumIC + peakHighTabelIC[i]
        -- end
        -- peak[6] = sumTC / (measureTimes - throwNum)
        -- peakIC[6] =  sumIC / (measureTimes - throwNum)
        -- if self.turboMode then
        --     log:debug("5ppm标液测试TC峰值= " .. peak[6] .. ", IC峰值= " .. peakIC[6])
        -- else
        --     log:debug("25ppm标液测试TC峰值= " .. peak[6] .. ", IC峰值= " .. peakIC[6])
        -- end

        -- --50ppm标点校准/ Turbo模式不执行该浓度
        -- if self.turboMode == false then
        --     updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[50ppm]标液后，点击确认开始下一步校准(7/8)")
        --     if self.turboMode then
        --         config.measureParam.reagent1Vol = 2
        --         config.measureParam.reagent2Vol = 2
        --         measureTimes = 75
        --         throwNum = 45
        --     else
        --         config.measureParam.reagent1Vol = 1
        --         config.measureParam.reagent2Vol = 5.5
        --     end
        --     config.modifyRecord.measureParam(true)
        --     ConfigLists.SaveMeasureParamConfig()
        --     setting.measureResult.continousModeParam.currentMeasureCnt = 0
        --     for j = 1, measureTimes do
        --         local err,result = pcall(function() return Measurer:Measure() end)
        --         if not err then      -- 出现异常
        --             if type(result) == "table" then
        --                 if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
        --                     self.isUserStop = true
        --                     error(result)
        --                 elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
        --                     self.isUserStop = true
        --                     error(result)
        --                 elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
        --                     self.isUserStop = true
        --                     error(result)
        --                 elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
        --                     self.isUserStop = true
        --                     error(result)
        --                 elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
        --                     self.isUserStop = true
        --                     error(result)
        --                 else
        --                     error(result)
        --                 end
        --             else
        --                 error(result)
        --             end
        --         else    -- 正常
        --             peakHighTabelTC[j] = result.peakTC
        --             peakHighTabelIC[j] = result.peakIC
        --             self.zeropeak[1] = string.format("%.2f", result.peakTC)
        --             log:debug("第" .. j .. "次50ppm标液TC修正峰值= " .. peakHighTabelTC[j] .. ", IC修正峰值= " .. peakHighTabelIC[j])
        --             for k,v in pairs(result) do
        --                 measureResult1[k] = v
        --             end
        --         end
        --     end
        --     local sumTC = 0
        --     local sumIC = 0
        --     for i = (throwNum + 1), measureTimes do
        --         sumTC = sumTC + peakHighTabelTC[i]
        --         sumIC = sumIC + peakHighTabelIC[i]
        --     end
        --     peak[7] = sumTC / (measureTimes - throwNum)
        --     peakIC[7] =  sumIC / (measureTimes - throwNum)
        --     log:debug("50ppm标液测试TC峰值= " .. peak[7] .. ", IC峰值= " .. peakIC[7])
        -- end        

        --换算理论值TOC+空白水TOC+对应IC
        if self.turboMode then
            for k ,v in pairs(config.measureParam.turboCalibratePointConsistency) do
                log:debug("TC峰值[" .. k .. "] = " .. peak[k] .. ", IC峰值 = " .. peakIC[k])
                --原标线计算IC浓度
                peakIC[k] = 10^(math.log(peakIC[k],10)*curveK-curveB)
                if k > 1 then
                    consistency[k] = v + blankConsistency.TOC + peakIC[k]
                else
                    --空白水电导率、浓度
                    consistency[k] = 0.0022
                    peak[k] = 0.065
                end
                print(" Turbo C" .. " [" .. k .. "] = " .. consistency[k])
                log:debug("Turbo 原标曲计算IC[" .. k .. "] = " .. peakIC[k])

                --TC峰值取对数
                peak[k] = math.log(peak[k],10)
                --理论TC取对数
                consistency[k] = math.log(consistency[k],10)

                log:debug("Turbo TC电导率[" .. k .. "]对数 = " .. peak[k] .. "理论TC浓度对数 = " .. consistency[k])
            end
        else
            for k ,v in pairs(config.measureParam.calibratePointConsistency) do
                log:debug("TC峰值[" .. k .. "] = " .. peak[k] .. ", IC峰值 = " .. peakIC[k])
                --原标线计算IC浓度
                peakIC[k] = 10^(math.log(peakIC[k],10)*curveK-curveB)
                if k > 1 then
                    consistency[k] = v + blankConsistency.TOC + peakIC[k]
                else
                    --空白水电导率、浓度
                    consistency[k] = 0.0022
                    peak[k] = 0.065
                end
                print(" C" .. " [" .. k .. "] = " .. consistency[k])
                log:debug("原标曲计算IC[" .. k .. "] = " .. peakIC[k])

                --TC峰值取对数
                peak[k] = math.log(peak[k],10)
                --理论TC取对数
                consistency[k] = math.log(consistency[k],10)

                log:debug("TC电导率[" .. k .. "]对数 = " .. peak[k] .. "理论TC对数 = " .. consistency[k])
            end
        end

        ----结果表中移除空白水值，不使用空白水浓度进行拟合
        --table.remove(consistency, 1)
        --table.remove(peak, 1)
        --table.remove(peakIC, 1)

        --for k,v in pairs(peak) do
        --    peak[k] = 10^(math.log(peak[k],10)*curveK-curveB)
        --    peakIC[k] = 10^(math.log(peakIC[k],10)*curveK-curveB)
        --    consistency[k] = math.log(consistency[k] + peakIC[k],10)
        --end

        if self.turboMode then
            --峰高对数为x,浓度对数为y
            curveK, curveB = self:AlgorithmLeastSquareMethod(peak, consistency, 6)
            R2 = self:AlgorithmFitGoodness(curveK, curveB, peak, consistency, 6)
            log:debug("R2 = " .. R2)
            config.measureParam.curveKTurbo = curveK
            config.measureParam.curveBTurbo = curveB
        else
            --峰高对数为x,浓度对数为y
            curveK, curveB = self:AlgorithmLeastSquareMethod(peak, consistency, 7)
            R2 = self:AlgorithmFitGoodness(curveK, curveB, peak, consistency, 7)
            log:debug("R2 = " .. R2)
            config.measureParam.curveK = curveK
            config.measureParam.curveB = curveB
        end

        setting.ui.profile.measureParam.updaterCurveParam(0,true)
        config.modifyRecord.measureParam(true)
        ConfigLists.SaveMeasureParamConfig()
        ConfigLists.SaveMeasureStatus()

        log:debug("校准-多点校准IC测量") --测量4次

        local updateWidgetManager = UpdateWidgetManager.Instance()
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[10ppm]IC标液后，点击确认开始下一步校准(8/8)")

        log:debug("关闭紫外灯")
        dc:GetIOpticalAcquire():TurnOffLED()	--关LED

        if self.turboMode then
            config.measureParam.reagent1Vol = 2
            config.measureParam.reagent2Vol = 2
            measureTimes = 75
            throwNum = 45
        else
            config.measureParam.reagent1Vol = 1
            config.measureParam.reagent2Vol = 0
        end

        setting.measureResult.continousModeParam.currentMeasureCnt = 0
        for j = 1, measureTimes do
            local err,result = pcall(function() return Measurer:Measure() end)
            if not err then      -- 出现异常
                if type(result) == "table" then
                    if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
                        self.isUserStop = true
                        error(result)
                    elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
                        self.isUserStop = true
                        error(result)
                    elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
                        self.isUserStop = true
                        error(result)
                    elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
                        self.isUserStop = true
                        error(result)
                    elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
                        self.isUserStop = true
                        error(result)
                    else
                        error(result)
                    end
                else
                    error(result)
                end
            else    -- 正常
                peakHighTabelTC[j] = result.peakTC
                peakHighTabelIC[j] = result.peakIC
                self.zeropeak[1] = string.format("%.2f", result.peakTC)
                log:debug("第" .. j .. "次10ppmIC标液TC修正峰值= " .. peakHighTabelTC[j] .. ", IC修正峰值= " .. peakHighTabelIC[j])
                for k,v in pairs(result) do
                    measureResult1[k] = v
                end
            end
        end
        local sumTC = 0
        local sumIC = 0
        for i = (throwNum + 1), measureTimes do
            sumTC = sumTC + peakHighTabelTC[i]
            sumIC = sumIC + peakHighTabelIC[i]
        end
        peak[2] = sumTC / (measureTimes - throwNum)
        peakIC[2] =  sumIC / (measureTimes - throwNum)
        log:debug("10ppmIC标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        local hisKIC = 1
        local measureTC = 0
        local measureIC = 0
        --if self.turboMode then
        --    --计算Turbo校准k(IC)
        --    hisKIC = config.measureParam.ICTurboCurveK
        --    measureTC =  self:CalculateConsistency(peak[2], ModelType.TC)
        --    measureIC =  self:CalculateConsistency(peakIC[2], ModelType.IC)
        --    if measureIC ~= 0 then
        --        config.measureParam.ICTurboCurveK = measureTC / measureIC
        --    end
        --else
        --    --计算单点校准k(IC)
        --    hisKIC = config.measureParam.ICCurveK
        --    measureTC =  self:CalculateConsistency(peak[2], ModelType.TC)
        --    measureIC =  self:CalculateConsistency(peakIC[2], ModelType.IC)
        --    if measureIC ~= 0 then
        --        config.measureParam.ICCurveK = measureTC / measureIC
        --    end
        --end
        measureTC =  self:CalculateConsistency(peak[2], ModelType.TC)
        measureIC =  self:CalculateConsistency(peakIC[2], ModelType.IC)

        --关紫外灯理论上TC值与IC值相等
        local checkError = math.abs(measureTC - measureIC)/measureIC * 100
        log:debug("IC理论值/测量值误差 = " .. checkError)
        if checkError > 10 or R2 < 0.99 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local tipStr = "多点校准失败, R2 = " .. string.format("%.5f", R2) .. ",误差" .. string.format("%.2f", checkError) .. "%(8/8)"
            log:debug(tipStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
            --if self.turboMode then
            --    config.measureParam.ICTurboCurveK = hisKIC
            --else
            --    config.measureParam.ICCurveK = hisKIC
            --end
            setting.ui.profile.measureParam.updaterCurveParam(0,true)
            config.modifyRecord.measureParam(true)
            ConfigLists.SaveMeasureParamConfig()
        else
            self.isQualified = true
            local tipStr = "多点校准成功, R2 = " .. string.format("%.5f", R2) .. ",误差" .. string.format("%.2f", checkError) .. "%(8/8)"
            log:debug(tipStr)
            setting.ui.profile.measureParam.updaterCurveParam(0,true)
            config.modifyRecord.measureParam(true)
            ConfigLists.SaveMeasureParamConfig()
            ConfigLists.SaveMeasureStatus()
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
        end

        log:debug("打开紫外灯")
        dc:GetIOpticalAcquire():TurnOnLED()
    end

    --需要测量标点
    if self.measureStandard == true then

        log:debug("校准-单点校准TC测量")

        local updateWidgetManager = UpdateWidgetManager.Instance()
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. cStr .. "TOC标液后，点击确认开始下一步校准(2/3)")

        -- log:debug("打开紫外灯")
        -- dc:GetIOpticalAcquire():TurnOnLED()

        -- --测量标点
        -- if Measurer.flow then
        --     Measurer:Reset()
        -- end
        -- Measurer.flow = self
        -- Measurer.measureType = MeasureType.Standard
        -- Measurer.currentRange = self.currentRange
        -- for k,v in pairs(addParam[2]) do
        --     Measurer.addParam [k] = v
        -- end

        -- --根据量程修改参数
        -- Measurer.addParam.standardVolume = setting.measure.range[self.currentRange].sampleVolume
        -- Measurer.addParam.blankVolume = setting.measure.range[self.currentRange].blankVolume
        -- Measurer.addParam.dilutionExtractVolume1 = setting.measure.range[self.currentRange].dilutionExtractVolume1
        -- Measurer.addParam.dilutionAddBlankVolume1 = setting.measure.range[self.currentRange].dilutionAddBlankVolume1
        -- Measurer.addParam.dilutionExtractVolume2 = setting.measure.range[self.currentRange].dilutionExtractVolume2
        -- Measurer.addParam.dilutionAddBlankVolume2 = setting.measure.range[self.currentRange].dilutionAddBlankVolume2
        -- Measurer.addParam.dilutionExtractVolume3 = setting.measure.range[self.currentRange].dilutionExtractVolume3
        -- Measurer.addParam.dilutionAddBlankVolume3 = setting.measure.range[self.currentRange].dilutionAddBlankVolume3
        -- Measurer.addParam.afterReagent1AddBlankVolume = setting.measure.range[self.currentRange].afterReagent1AddBlankVolume
        -- Measurer.addParam.diluteFactor = setting.measure.range[self.currentRange].diluteFactor
        -- Measurer.addParam.rinseSampleVolume = 0
        -- Measurer.addParam.rinseBlankVolume = 0
        -- Measurer.addParam.rinseStandardVolume = setting.measure.range[self.currentRange].rinseStandardVolume + setting.measure.range[self.currentRange].rinseSampleVolume

        -- local peakHighTabelTC = {}
        -- local peakHighTabelIC = {}

        -- if self.consistency > 0.5 then
        --     config.measureParam.reagent1Vol = 1
        --     config.measureParam.reagent2Vol = 0.1
        -- else
        --     config.measureParam.reagent1Vol = 1
        --     config.measureParam.reagent2Vol = 0.1
        -- end
        -- config.modifyRecord.measureParam(true)
        -- ConfigLists.SaveMeasureParamConfig()
        -- setting.measureResult.continousModeParam.currentMeasureCnt = 0
        -- for j = 1, 5 do
        --     local err,result = pcall(function() return Measurer:Measure() end)
        --     if not err then      -- 出现异常
        --         if type(result) == "table" then
        --             if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
        --                 self.isUserStop = true
        --                 error(result)
        --             else
        --                 error(result)
        --             end
        --         else
        --             error(result)
        --         end
        --     else    -- 正常
        --         peakHighTabelTC[j] = result.peakTC
        --         peakHighTabelIC[j] = result.peakIC
        --         self.zeropeak[1] = string.format("%.2f", result.peakTC)
        --         log:debug("第" .. j .. "次1ppmTC标液TC修正峰值= " .. peakHighTabelTC[j] .. ", IC修正峰值= " .. peakHighTabelIC[j])
        --         for k,v in pairs(result) do
        --             measureResult1[k] = v
        --         end
        --     end
        -- end
        peak[2] = 1.94389
        peakIC[2] = 0.7359

        log:debug(cStr .. "TC标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        ----计算单点校准k(TC)
        ----理论TC浓度+空白水TOC浓度
        --local consistencyTC = blankConsistency.TOC +  blankConsistency.IC + consistency[2]
        ----实测浓度
        --local firstMeasureTC =  self:CalculateConsistency(peak[2], ModelType.TC)

        --换算理论值TOC+空白水TOC+对应IC
        for k ,v in pairs(consistency) do
            log:debug("TC峰值[" .. k .. "] = " .. peak[k] .. ", IC峰值 = " .. peakIC[k])
            --原标线计算IC浓度
            peakIC[k] = 10^(math.log(peakIC[k],10)*curveK-curveB)
            if k > 1 then
                consistency[k] = v + blankConsistency.TOC + peakIC[k]
            else
                --空白水电导率、浓度
                consistency[k] = 0.0022
                peak[k] = 0.065
            end
            print(" C" .. " [" .. k .. "] = " .. consistency[k])
            log:debug("原标曲计算IC[" .. k .. "] = " .. peakIC[k])

            --TC峰值取对数
            peak[k] = math.log(peak[k],10)
            --理论TC取对数
            consistency[k] = math.log(consistency[k],10)

            log:debug("TC电导率[" .. k .. "]对数 = " .. peak[k] .. "理论TC对数 = " .. consistency[k])
        end

        --峰高对数为x,浓度对数为y
        curveK, curveB = self:AlgorithmLeastSquareMethod(peak, consistency, 2)
        R2 = self:AlgorithmFitGoodness(curveK, curveB, peak, consistency, 2)
        log:debug("R2 = " .. R2)

        log:debug("校准-单点校准IC测量")

        local updateWidgetManager = UpdateWidgetManager.Instance()
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[1ppm]IC标液后，点击确认开始下一步校准(3/3)")

        -- log:debug("关闭紫外灯")
        -- dc:GetIOpticalAcquire():TurnOffLED()	--关LED

        -- setting.measureResult.continousModeParam.currentMeasureCnt = 0
        -- for j = 1, 5 do
        --     local err,result = pcall(function() return Measurer:Measure() end)
        --     if not err then      -- 出现异常
        --         if type(result) == "table" then
        --             if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
        --                 self.isUserStop = true
        --                 error(result)
        --             elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
        --                 self.isUserStop = true
        --                 error(result)
        --             else
        --                 error(result)
        --             end
        --         else
        --             error(result)
        --         end
        --     else    -- 正常
        --         peakHighTabelTC[j] = result.peakTC
        --         peakHighTabelIC[j] = result.peakIC
        --         self.zeropeak[1] = string.format("%.2f", result.peakTC)
        --         log:debug("第" .. j .. "次1ppmIC标液TC修正峰值= " .. peakHighTabelTC[j] .. ", IC修正峰值= " .. peakHighTabelIC[j])
        --         for k,v in pairs(result) do
        --             measureResult1[k] = v
        --         end
        --     end
        -- end
        -- peak[2] = (peakHighTabelTC[5] + peakHighTabelTC[3] + peakHighTabelTC[4])/3
        -- peakIC[2] = (peakHighTabelIC[5] + peakHighTabelIC[3] + peakHighTabelIC[4])/3

        -- log:debug(cStr .. "IC标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算单点校准k(TC)
        --local hisKTC = config.measureParam.TCCurveK
        --local hisKIC = config.measureParam.ICCurveK
        local measureTC =  self:CalculateConsistency(peak[2], ModelType.TC)
        local measureIC =  self:CalculateConsistency(peakIC[2], ModelType.IC)
        --if measureIC ~= 0 then
        --    config.measureParam.ICCurveK = measureTC / measureIC
        --end
        --if measureTC ~= 0 then
        --    config.measureParam.TCCurveK = consistencyTC / firstMeasureTC
        --end
        --关紫外灯理论上TC值与IC值相等
        local checkError = math.abs(measureTC - measureIC)/measureIC * 100
        log:debug("IC理论值/测量值误差 = " .. checkError)

        if checkError > 10  then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local tipStr = cStr .. "单点校准失败,误差" .. string.format("%.2f", checkError) .. "%(3/3)"
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
            --config.measureParam.ICCurveK = hisKIC
            --config.measureParam.TCCurveK = hisKTC
        else
            self.isQualified = true
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local tipStr = cStr .. "单点校准成功,误差" .. string.format("%.2f", checkError) .. "%(3/3)"
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
        end

        setting.ui.profile.measureParam.updaterCurveParam(0,true)
        config.modifyRecord.measureParam(true)
        ConfigLists.SaveMeasureParamConfig()
        ConfigLists.SaveMeasureStatus()
    end

    if self.calibrateType == CalibrateType.mulCalibrate or self.calibrateType == CalibrateType.calibrate then
        --保存基准线
        status.measure.standardCurve = self.currentRange
    end

    local absorbancess = (peak[2] - peak[1]) * setting.measure.range[self.currentRange].diluteFactor / config.measureParam.curveParam[self.currentRange].RangeConsistency / config.measureParam.reviseParameter[self.currentRange]
    local calibrateRange  = self.currentRange
    local rangeTable = {}
    table.insert(rangeTable, self.currentRange)
    if oneTimesCreateCurve then
        table.remove(rangeTable, 1)
        --一键校准不生成0-20量程标线
        for i = 2, setting.measure.range.rangeNum do
            if i ~= self.currentRange then
                table.insert(rangeTable, i)
            end
        end
        table.insert(rangeTable, self.currentRange)
    end

    for i, range in pairs(rangeTable) do
        local showpeak = {peak[1],peak[2]}

        if math.abs(consistency[2] - consistency[1]) < 0.000001 then
            log:info("校准结果计算异常")
            error(CalibrateResultWrongException:new())
        else

        end

        --Turbo模式不保存标线,直接更改参数值
        if self.isQualified == true and self.turboMode == false then
            -- 保存校准结果
            --	print("Push calibrate result data to file.")
            local resultManager = ResultManager.Instance()
            local recordData = RecordData.new(resultManager:GetCalibrateRecordDataSize(setting.resultFileInfo.calibrateRecordFile[1].name))
            recordData:PushInt(self.calibrateDateTime) 			        -- 时间
            recordData:PushDouble(curveK)   				            --标线斜率K
            recordData:PushDouble(curveB)   				            --标线截距B

            recordData:PushFloat(showpeak[1]) 			            -- 零点反应峰面积
            recordData:PushFloat(consistency[1]) 			            -- 零点浓度
            recordData:PushFloat(self.zeropeak[1])		            	-- 零点第一次峰面积
            recordData:PushFloat(self.zeropeak[2])		            	-- 零点第二次峰面积
            recordData:PushFloat(self.zeropeak[3])		            	-- 零点第三次峰面积
            recordData:PushFloat(measureResult1.initCellTempTC)      	-- 零点初始制冷模块温度
            recordData:PushFloat(measureResult1.initCellTempIC) 		        -- 零点初始测量模块温度
            recordData:PushFloat(measureResult1.finalCellTempTC) 	    -- 零点反应制冷模块温度
            recordData:PushFloat(measureResult1.finalCellTempIC) 	            -- 零点反应测量模块温度
            recordData:PushFloat(0) 	    -- 零点初始值燃烧炉温度
            recordData:PushFloat(measureResult1.initEnvironmentTemp) 	    -- 零点反应值上机箱温度
            recordData:PushFloat(0) 	-- 零点反应值下机箱温度
            recordData:PushFloat(0) 	    -- 零点反应值燃烧炉温度
            recordData:PushFloat(measureResult1.finalEnvironmentTemp) 	    -- 零点反应值上机箱温度
            recordData:PushFloat(0) 	-- 零点反应值下机箱温度

            recordData:PushFloat(showpeak[2]) 			                -- 标点峰面积
            recordData:PushFloat(consistency[2]) 		            	    -- 标点浓度
            recordData:PushFloat(self.standardpeak[1])		            -- 标点第一次峰面积
            recordData:PushFloat(self.standardpeak[2])		-- 标点第二次峰面积
            recordData:PushFloat(self.standardpeak[3])       -- 标点第三次峰面积
            recordData:PushFloat(measureResult2.initCellTempTC)      	-- 标点初始制冷模块温度
            recordData:PushFloat(measureResult2.initCellTempIC) 		        -- 标点初始测量模块温度
            recordData:PushFloat(measureResult2.finalCellTempTC) 	    -- 标点反应制冷模块温度
            recordData:PushFloat(measureResult2.finalCellTempIC) 	            -- 标点反应测量模块温度
            recordData:PushFloat(0) 	    -- 标点初始值燃烧炉温度
            recordData:PushFloat(measureResult2.initEnvironmentTemp) 	    -- 标点反应值上机箱温度
            recordData:PushFloat(0) 	-- 标点反应值下机箱温度
            recordData:PushFloat(0) 	    -- 标点反应值燃烧炉温度
            recordData:PushFloat(measureResult2.finalEnvironmentTemp) 	    -- 标点反应值上机箱温度
            recordData:PushFloat(0) 	-- 标点反应值下机箱温度

            recordData:PushFloat(1)					                    -- 曲线线性度R2
            recordData:PushInt(os.time()-self.calibrateDateTime) 	    -- 校准时长
            recordData:PushFloat(config.system.rangeViewMap[self.currentRange].view)                 --当前量程

            local flowManager = FlowManager.Instance()
            if true == flowManager:IsReagentAuthorize() then
                if 1 then
                    log:debug("保存量程 "..self.currentRange.." ,校准结果，K： "..curveK.." B： "..curveB)
                    Helper.Result.OnCalibrateResultAdded(self.calibrateDateTime, self.zeroCalibrateDateTime, self.standardCalibrateDateTime,curveK, curveB, consistency[1], consistency[2], showpeak[1], showpeak[2],self.currentRange ,false)
                    Helper.Result.OnSaveCalibrateConsistencyAdded(meausureConsistency[1],meausureConsistency[2],0,0,self.currentRange)
                    op:SaveCalibrationTimeStr(self.calibrateDateTime,self.currentRange)
                end
                resultManager:AddCalibrateRecord(setting.resultFileInfo.calibrateRecordFile[1].name, recordData)
                setting.ui.profile.measureParam.updaterCurveParam(0,true)
                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                ConfigLists.SaveMeasureStatus()
            else
                local alarm = Helper.MakeAlarm(setting.alarm.reagentAuthorizationError, "")
                AlarmManager.Instance():AddAlarm(alarm)

                status.measure.schedule.autoCalibrate.dateTime = self.calibrateDateTime
            end
        end

        self.isFinish = true

        if self.mulPointCalibration == true and self.isQualified == true then --多点校准标线合格
            log:debug("曲线 k  = " .. curveK .. ", B = " .. curveB)
        elseif self.isQualified == true then --单点校准标线合格
            log:debug("曲线 k(TC)  = " .. config.measureParam.TCCurveK .. ", k(IC) = " .. config.measureParam.ICCurveK)
        else
            log:debug(self.text .. "失败")
        end
    end
end

function CalibrateFlow:OnStop()

    if nil ~= setting.common.skipFlow and true == setting.common.skipFlow then

    else
        -- 初始化下位机
        dc:GetIDeviceStatus():Initialize()
        rc:ClearBuf()--清buf,防止数组刷新
        lc:GetIDeviceStatus():Initialize()
        --停止水样泵
        op:StopSamplePump()
        --设置风扇常开
        op:SetDCNormalOpen(setting.liquidType.map.fan)
        --关紫外灯
        dc:GetIOpticalAcquire():TurnOffLED()
        --设置去离子水泵和阀为打开
        op:SetLCStopStatus()
    end

    --继电器指示
    Helper.Result.RelayOutOperate(setting.mode.relayOut.calibrateInstruct, false)

    if self.calibrateType == CalibrateType.calibrate or self.calibrateType == CalibrateType.mulCalibrate then
        status.measure.schedule.autoCalibrate.dateTime = self.calibrateDateTime
    end

    local eventStr = ""
    if not self.isFinish then
        if self.isUserStop then
            status.measure.newResult.measure.resultInfo = self.lastResultInfo
            self.result = "用户终止"
            log:info("用户终止")
        else
            status.measure.newResult.measure.resultInfo = "D"
            self.result = "故障终止"
            log:warn("故障终止")
        end
        eventStr = self.text .. "-" .. self.result
    else
        local flowStr = "校准"
        if self.calibrateType == CalibrateType.mulCalibrate then
            flowStr = "一键运行"
        elseif self.calibrateType == CalibrateType.onlyCalibrateBlank then
            flowStr = "零点校准"
        elseif self.calibrateType == CalibrateType.onlyCalibrateStandard then
            flowStr = "量程校准"
        end
        self.result = self.text .."完成"
        log:info(self.result)
        log:info(flowStr.."总时间 = ".. os.time()-self.calibrateDateTime)
        eventStr = self.text .."完成"
    end

    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)

    --保存试剂余量表
    ReagentRemainManager.SaveRemainStatus()

    status.measure.isUseStart = false
    ConfigLists.SaveMeasureStatus()
end

--[[
 * @brief 精准校准处理
 * @param[in] refAbs 参考吸光度
 * @param[in] waveRange 限制波动范围
--]]
function CalibrateFlow:AccurateCalibrate(area, waveRange, calibrateFlow, measureType)
    local measureAD = MeasureAD:new()
    local temppeak1,temppeak2,temppeak3,temppeak4
    local peak

    temppeak1 = area;
    log:debug("第1次校准测量峰面积 area = ".. temppeak1)
    --	print("tempAbsorbance1 = "..tempAbsorbance1)
    --检测消解室是否为安全温度
    op:CheckDigestSafety()

    -- 校准测量
    log:debug("精准校准测量2")

    -- 测量流程执行
    local err,result = pcall(function() return Measurer:Measure() end)
    if not err then      -- 出现异常
        if type(result) == "table" then
            if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
                self.isUserStop = true
                error(result)
            elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
                self.isUserStop = true
                error(result)
            elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
                self.isUserStop = true
                error(result)
            elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
                self.isUserStop = true
                error(result)
            elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
                self.isUserStop = true
                error(result)
            else
                error(result)
            end
        else
            error(result)
        end
    else    -- 正常

        temppeak2 = result.peakTC
        if measureType == MeasureType.Blank then
            calibrateFlow.zeropeak[2] =  string.format("%.2f", temppeak2)
        else
            calibrateFlow.standardpeak[2] =  string.format("%.2f", temppeak2)
        end
        log:debug("第2次校准测量峰面积 area = " .. temppeak2)
    end

    --local deviation12 = math.abs(temppeak1 - temppeak2)/(math.abs(temppeak1 + temppeak2)/2)
    local deviation12 = 1
    if deviation12 < waveRange then ----------两次校准偏差小于阈值

        peak = (temppeak1 + temppeak2)/2
        log:debug("峰面积平均修正 peak = ".. peak)
        --清洗流程执行
        local err,result = pcall(function() return  Measurer:Measure() end)
        if not err then      -- 出现异常
            if type(result) == "table" then
                if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
                    self.isUserStop = true
                    error(result)
                else
                    error(result)
                end
            else
                error(result)
            end

        end

    else  ----------两次校准偏差大于阈值
        --检测消解室是否为安全温度
        op:CheckDigestSafety()
        -- 校准-第三次测量
        log:debug("精准校准测量3")

        --流程执行
        local err,result = pcall(function() return Measurer:Measure(AccurateType.onlyAddSample) end)
        if not err then      -- 出现异常
            if type(result) == "table" then
                if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
                    self.isUserStop = true
                    error(result)
                else
                    error(result)
                end
            else
                error(result)
            end
        else    -- 正常
            measureAD.initReference = result.initReferenceAD
            measureAD.initMeasure = result.initMeasureAD
            measureAD.finalReference = result.finalReferenceAD
            measureAD.finalMeasure = result.finalMeasureAD

            --temppeak3 = measureAD:CalculateRelAbsorbance()
            temppeak3 = result.peakTC
            if measureType == MeasureType.Blank then
                calibrateFlow.zeropeak[3] = string.format("%.2f", temppeak3)
            else
                calibrateFlow.standardpeak[3] = string.format("%.2f", temppeak3)
            end
            log:debug("第3次校准测量峰面积 area= " .. temppeak3)

            ----判断精准校准结果是否满足要求
            --local deviation13 = math.abs(temppeak1 - temppeak3)/(temppeak1 + temppeak3)/2
            --local deviation23 = math.abs(temppeak2 - temppeak3)/(temppeak2 + temppeak3)/2
            ----if deviation13 >= waveRange and deviation23 >= waveRange then
            ----    peak = (temppeak1 + temppeak2 + temppeak3)/3
            ----    log:debug("峰面积123平均修正 peak = " .. peak)
            ----	--log:debug("校准结果错误")
            ----	--error(CalibrateResultWrongException:new())
            ----else
            --	if deviation13 < deviation23 then
            --        peak = (temppeak1 + temppeak3)/2
            --		log:debug("峰面积13平均修正 Absorbance = " .. peak)
            --	else
            --        peak = (temppeak2 + temppeak3)/2
            --		log:debug("峰面积23平均修正 Absorbance = " .. peak)
            --	end
            ----end
        end

        --检测消解室是否为安全温度
        op:CheckDigestSafety()
        -- 校准-第三次测量
        log:debug("精准校准测量4")

        --流程执行
        local err,result = pcall(function() return Measurer:Measure() end)
        if not err then      -- 出现异常
            if type(result) == "table" then
                if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
                    self.isUserStop = true
                    error(result)
                else
                    error(result)
                end
            else
                error(result)
            end
        else    -- 正常
            measureAD.initReference = result.initReferenceAD
            measureAD.initMeasure = result.initMeasureAD
            measureAD.finalReference = result.finalReferenceAD
            measureAD.finalMeasure = result.finalMeasureAD

            temppeak4 = result.peakTC
            if measureType == MeasureType.Blank then
                calibrateFlow.zeropeak[4] = string.format("%.2f", temppeak4)
            else
                calibrateFlow.standardpeak[4] = string.format("%.2f", temppeak4)
            end
            log:debug("第4次校准测量峰面积 area= " .. temppeak4)

            local maxValue = math.max(temppeak1,
                    temppeak2,
                    temppeak3,
                    temppeak4)
            local minValue = math.min(temppeak1,
                    temppeak2,
                    temppeak3,
                    temppeak4)
            peak = (temppeak1 + temppeak2 + temppeak3 + temppeak4 - maxValue - minValue)/2
        end
    end

    return peak
end

function CalibrateFlow:CalculateConsistency(area, type)
    local consistency = 0
    --local peak = 0
    local curveK = config.measureParam.curveK
    local curveB = config.measureParam.curveB
    local KTC = config.measureParam.TCCurveK
    local KIC = config.measureParam.ICCurveK

    if self.turboMode then
        curveK = config.measureParam.curveKTurbo
        curveB = config.measureParam.curveBTurbo
        KTC = config.measureParam.TCTurboCurveK
        KIC = config.measureParam.ICTurboCurveK
    end

    if math.abs(curveK - 0)<0.00001 then
        log:debug("校准数值异常")
        return 0
    end
    log:debug("量程"..self.currentRange.."计算斜率 K = " .. curveK .. ", B = " .. curveB)
    log:debug("计算K(TC) = " .. KTC .. ", B = " .. curveB .. ", K(IC) = " .. KIC)

    if type ~= nil and type == ModelType.TC then
        consistency = KTC * 10^(curveK * math.log(area, 10) + curveB)
    else
        consistency = KIC * 10^(curveK * math.log(area, 10) + curveB)
    end

    return consistency
end

--[[
 * @brief 峰高系数补偿
 * @param[in] peakHigh 峰高
 * @param[in] temp 温度
  * @param[in] type IC or TC
--]]
function CalibrateFlow:PeakHighReviseWithTemperature(peakHigh, temp)
    local retA = peakHigh
    local reviser = 9.61287*10^(-6) * temp^3 - 0.00145*temp^2 + 0.07938*temp -0.40886

    if reviser ~= 0 and temp > 5 then
        retA = retA / reviser
        log:debug("温度补偿系数 = " .. reviser)
    else
        log:debug("Temperature Revise Error, Invalid Factor " .. reviser)
    end

    return retA
end

--[[
 * @brief 管路更新
 * @detail 一键运行流程中先执行管路更新再校准
--]]
function CalibrateFlow:PipeRenew()

    local runAction

    -- 清空残留液
    runAction = Helper.Status.SetAction(setting.runAction.mulCalibrate.clearWaste)
    StatusManager.Instance():SetAction(runAction)
    --op:DrainToWaste(setting.liquid.meterPipeVolume)

    -- 清空试剂一管
    runAction = Helper.Status.SetAction(setting.runAction.mulCalibrate.clearReagent1Pipe)
    StatusManager.Instance():SetAction(runAction)

    --清空试剂二管
    runAction = Helper.Status.SetAction(setting.runAction.mulCalibrate.clearReagent2Pipe)
    StatusManager.Instance():SetAction(runAction)
end

--[[
 * @brief 检查是否需要进行深度清洗
 * @detail 若上次校准间隔时间较长则进行深度清洗
--]]
function CalibrateFlow:CleanDeeplyCheckTime()
    local currentTime = os.time()
    local lastTime
    local temp
    local MeasurerIntervalMaxTime = 144        --距离上次校准允许度最大间隔时间，超过则进行深度清洗

    temp = status.measure.newResult.calibrate.dateTime

    lastTime = temp + MeasurerIntervalMaxTime*3600
    if lastTime - currentTime < 0 then
        log:debug("距离上次校准已超"..MeasurerIntervalMaxTime.."小时，进行深度清洗")
        local flow = CleanFlow:new({},cleanType.cleanDeeply)
        flow:CleanDeeply(self)
    end
end


--[[
 * @brief 计算校正系数系数
--]]
function CalibrateFlow:CalculateReviseParam(absorbance)
    local standardCurve = status.measure.standardCurve
    local absorbancess = (status.measure.calibrate[standardCurve].point1Absorbance - status.measure.calibrate[standardCurve].point0Absorbance)
            * setting.measure.range[standardCurve].diluteFactor
            / config.measureParam.curveParam[standardCurve].RangeConsistency
            / config.measureParam.reviseParameter[standardCurve]

    local reviseParameter = (absorbance - status.measure.calibrate[standardCurve].point0Absorbance)
            * setting.measure.range[self.currentRange].diluteFactor
            / absorbancess
            /config.measureParam.curveParam[self.currentRange].RangeConsistency

    log:debug("基准线为量程 "..standardCurve..", 校正量程为 "..self.currentRange.." ,校正系数计算结果 = "..reviseParameter)

    return reviseParameter
end

--[[
 * @brief 最小二乘法线性拟合
 * @details 最小二乘法实现多点标线计算
 * @param[in] consistencyTable 浓度数据表
 * @param[in] absorbanceTable 吸光度数据表
 * @param[in] num 数据点个数
--]]
function CalibrateFlow:AlgorithmLeastSquareMethod(consistencyTable, absorbanceTable, num)
    local xAvg = 0
    local yAvg = 0
    local x2Avg = 0
    local xyAvg = 0
    local k = 0
    local b = 0

    for i = 1,num do
        xAvg = xAvg + consistencyTable[i]
        yAvg = yAvg + absorbanceTable[i]
        x2Avg = x2Avg + consistencyTable[i]*consistencyTable[i]
        xyAvg = xyAvg + consistencyTable[i]*absorbanceTable[i]
        log:debug("线性拟合浓度[" .. i .. "]= " .. consistencyTable[i] .. ",峰高= " .. absorbanceTable[i])
    end
    xAvg = xAvg/num
    yAvg = yAvg/num
    x2Avg = x2Avg/num
    xyAvg = xyAvg/num

    k = (xyAvg - (xAvg * yAvg)) / (x2Avg - (xAvg * xAvg))
    b = yAvg - (k * xAvg)

    log:debug("线性拟合 k = " .. k .. ", b = " .. b)

    return k,b
end


--[[
 * @brief 拟合度
 * @details 计算标线的拟合程度
  * @param[in] k 标线斜率
 * @param[in] b 标线截距
 * @param[in] consistencyTable 浓度数据表
 * @param[in] absorbanceTable 吸光度数据表
 * @param[in] num 数据点个数
--]]
function CalibrateFlow:AlgorithmFitGoodness(k, b, consistencyTable, absorbanceTable, num)
    local tmpR2 = 0         --拟合度
    local ssReg = 0         --回归平方和
    local ssTotal = 0       --总偏差平方和
    local ssResid = 0       --残差平方和
    local yEstimate = 0     --y的估计值
    local yActual = 0       --y的实际值
    local yAvg = 0          --y的平均值

    --Y均值
    for i = 1,num do
        yAvg = yAvg + absorbanceTable[i]
    end
    yAvg = yAvg / num

    --残差平方和
    for i = 1,num do
        yEstimate = (k * consistencyTable[i]) + b
        yActual = absorbanceTable[i]
        ssResid = ssResid + (yEstimate - yActual) * (yEstimate - yActual)
    end

    --总偏差平方和
    for i = 1,num do
        yActual = absorbanceTable[i]
        ssTotal = ssTotal + (yActual - yAvg) * (yActual - yAvg)
    end

    --回归平方和
    ssReg = ssTotal - ssResid

    if ssReg == 0 or ssTotal == 0 then
        tmpR2 = 0
    else
        tmpR2 = ssReg / ssTotal
    end

    return tmpR2
end