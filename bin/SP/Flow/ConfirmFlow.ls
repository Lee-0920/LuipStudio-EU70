--[[
 * @brief 校准流程。
--]]

ConfirmType =
{
    singlePoint= 0,
    systemAdaptability = 1,
    sterileWaterAdaptability = 2,
    robustness = 3,
    specificity = 4,
    linear = 5,
    sdbsAdaptability = 6,
    accuracy = 7,
    icr = 8,
}

ConfirmFlow = Flow:new
{
    calibrateDateTime = 0,
    zeroCalibrateDateTime = 0,
    standardCalibrateDateTime = 0,
    measureBlank = true,
    measureStandard = true,
    measureRss = false,
    measureSpecificity = false,
    measureLinear = false,
    measureSDBS = false,
    measureICR = false,
    currentRange = 1,
    isUseStart = false,
    curveCalibrateRange = 0,
    lastResultInfo = "N",
    text = "",
}

function ConfirmFlow:new(o, target, consistency)
    o = o or {}
    setmetatable(o, self)
    self.__index = self

    o.calibrateDateTime = os.time()
    o.zeroCalibrateDateTime = status.measure.calibrate[config.measureParam.range[config.measureParam.calibrateRangeIndex + 1] + 1].zeroCalibrateDateTime
    o.standardCalibrateDateTime = status.measure.calibrate[config.measureParam.range[config.measureParam.calibrateRangeIndex + 1] + 1].standardCalibrateDateTime
    o.ConfirmType = target
    o.currentRange = config.measureParam.range[config.measureParam.calibrateRangeIndex + 1] + 1
    o.isUseStart = false
    o.curveCalibrateRange = 0
    o.lastResultInfo = status.measure.newResult.measure.resultInfo
    o.zeropeak = {0,0,0,0}
    o.standardpeak = {0,0,0,0}
    o.confirmConsistency = consistency

    return o
end

function ConfirmFlow:GetRuntime()
    local runtime = 0
    if self.ConfirmType == ConfirmType.systemAdaptability then
        runtime = setting.runStatus.systemAdaptability.GetTime()
    elseif self.ConfirmType == ConfirmType.sterileWaterAdaptability then
        runtime = setting.runStatus.sterileWaterAdaptability.GetTime()
    elseif self.ConfirmType == ConfirmType.robustness then
        runtime = setting.runStatus.robustness.GetTime()
    elseif self.ConfirmType == ConfirmType.specificity then
        runtime = setting.runStatus.specificity.GetTime()
    elseif self.ConfirmType == ConfirmType.linear then
        runtime = setting.runStatus.linear.GetTime()
    elseif self.ConfirmType == ConfirmType.sdbsAdaptability then
        runtime = setting.runStatus.sdbsAdaptability.GetTime()
    elseif self.ConfirmType == ConfirmType.icr then
        runtime = setting.runStatus.icr.GetTime()
    end
    return runtime
end

function ConfirmFlow:OnStart()
    local eventStr = "开始" .. self.text
    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)

    if 1 == config.system.OEM and self.ConfirmType == ConfirmType.systemAdaptability then
        self.ConfirmType = ConfirmType.calibrate
    end
    --组合流程需要重新加载时间
    self.measureDateTime = os.time()
    -- 初始化下位机
    dc:GetIDeviceStatus():Initialize()
    lc:GetIDeviceStatus():Initialize()
    log:debug("打开紫外灯")
    dc:GetIOpticalAcquire():TurnOnLED()

    status.measure.isUseStart = self.isUseStart
    status.measure.newResult.measure.resultInfo = "C"
    ConfigLists.SaveMeasureStatus()

    --继电器指示
    Helper.Result.RelayOutOperate(setting.mode.relayOut.calibrateInstruct, true)

    --设置运行状态
    local runStatus
    self.currentRange = 1
    if self.ConfirmType == ConfirmType.singlePoint then
        runStatus = Helper.Status.SetStatus(setting.runStatus.singlePoint)
    elseif self.ConfirmType == ConfirmType.systemAdaptability then
        runStatus = Helper.Status.SetStatus(setting.runStatus.systemAdaptability)
    elseif self.ConfirmType == ConfirmType.sterileWaterAdaptability then
        runStatus = Helper.Status.SetStatus(setting.runStatus.sterileWaterAdaptability)
    elseif self.ConfirmType == ConfirmType.robustness then
        runStatus = Helper.Status.SetStatus(setting.runStatus.robustness)
    elseif self.ConfirmType == ConfirmType.specificity then
        runStatus = Helper.Status.SetStatus(setting.runStatus.specificity)
    elseif self.ConfirmType == ConfirmType.linear then
        runStatus = Helper.Status.SetStatus(setting.runStatus.linear)
    elseif self.ConfirmType == ConfirmType.sdbsAdaptability then
        runStatus = Helper.Status.SetStatus(setting.runStatus.sdbsAdaptability)
    elseif self.ConfirmType == ConfirmType.icr then
        runStatus = Helper.Status.SetStatus(setting.runStatus.icr)
    else
        runStatus = Helper.Status.SetStatus(setting.runStatus.calibrate)
    end

    StatusManager.Instance():SetStatus(runStatus)
end


function ConfirmFlow:OnProcess()
    local initAbsorbance = {0,0}
    local absorbance = {0,0}
    local peak = {0,0,0,0,0,0,0}
    local peakIC = {0,0,0,0,0,0,0}
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
    local checkError = {TC = 0, IC = 0, TOC = 0}
    local oneTimesCreateCurve = false
    local blankConsistency = {TC = 0, IC = 0, TOC = 0}
    local cRSD = {TC = 0, IC = 0, TOC = 0}
    local flowName = ""
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

    --一键运行增加管路更新操作
    if self.ConfirmType == ConfirmType.systemAdaptability then
        self.confirmConsistency = 0.5
        self.measureRss = true
        self.measureStandard = false
        flowName = "系统适用性确认"
    elseif self.ConfirmType == ConfirmType.sterileWaterAdaptability then
        self.confirmConsistency = 8
        self.measureRss = true
        self.measureStandard = false
        flowName = "无菌水适用性确认"
    elseif self.ConfirmType == ConfirmType.robustness then
        self.confirmConsistency = 0.5
        self.measureStandard = false
        self.measureRss = true
        flowName = "鲁棒性验证"
    elseif self.ConfirmType == ConfirmType.specificity then
        self.confirmConsistency = 0.5
        self.measureSpecificity = true
        self.measureStandard = false
        flowName = "特异性验证"
    elseif self.ConfirmType == ConfirmType.linear then
        self.confirmConsistency = 0.5
        self.measureLinear = true
        self.measureStandard = false
        flowName = "线性验证"
    elseif self.ConfirmType == ConfirmType.sdbsAdaptability then
        self.confirmConsistency = 0.5
        self.measureSDBS = true
        self.measureStandard = false
        flowName = "SDBS适用性验证"
    elseif self.ConfirmType == ConfirmType.icr then
        self.confirmConsistency = 25
        self.measureICR = true
        self.measureBlank = false
        self.measureStandard = false
        flowName = "SDBS适用性验证"
    else
        if self.confirmConsistency == nil then
            log:warn("单点确认浓度异常")
            return
        end
        self.measureStandard = true
        self.measureRss = false
        consistency[1] = 0
        consistency[2] = self.confirmConsistency
        flowName = "单点确认"
        log:debug("单点确认浓度 = " .. self.confirmConsistency)
        if self.confirmConsistency < 1 then
            cStr = "[" .. string.format("%.0f", self.confirmConsistency * 1000) .. "ppb]"
        else
            cStr = "[" .. string.format("%.0f", self.confirmConsistency) .. "ppm]"
        end

        local updateWidgetManager = UpdateWidgetManager.Instance()
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "即将进行" .. cStr .. "单点确认")
    end

    --空白水酸剂氧化剂流速
    if self.ConfirmType == ConfirmType.sterileWaterAdaptability then
        config.measureParam.reagent1Vol = 0.8
        config.measureParam.reagent2Vol = 0
    else
        config.measureParam.reagent1Vol = 1
        config.measureParam.reagent2Vol = 0
    end
    config.modifyRecord.measureParam(true)
    ConfigLists.SaveMeasureParamConfig()
    setting.measureResult.continousModeParam.currentMeasureCnt = 0

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

    --需要测量零点
    if self.measureBlank == true then
        --测量零点
        log:debug("确认-零点测量")

        local updateWidgetManager = UpdateWidgetManager.Instance()
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好[空白水]后，点击确认开始下一步校准(1/3)")

        if Measurer.flow then
            Measurer:Reset()
        end
        Measurer.flow = self
        Measurer.measureType = MeasureType.Blank
        Measurer.currentRange = self.currentRange
        for k,v in pairs(addParam[1]) do
            Measurer.addParam [k] = v
        end
        --根据量程修改参数
        Measurer.addParam.standardVolume = 0
        Measurer.addParam.blankVolume = setting.measure.range[self.currentRange].blankVolume + setting.measure.range[self.currentRange].sampleVolume
        Measurer.addParam.dilutionExtractVolume1 = setting.measure.range[self.currentRange].dilutionExtractVolume1
        Measurer.addParam.dilutionAddBlankVolume1 = setting.measure.range[self.currentRange].dilutionAddBlankVolume1
        Measurer.addParam.dilutionExtractVolume2 = setting.measure.range[self.currentRange].dilutionExtractVolume2
        Measurer.addParam.dilutionAddBlankVolume2 = setting.measure.range[self.currentRange].dilutionAddBlankVolume2
        Measurer.addParam.dilutionExtractVolume3 = setting.measure.range[self.currentRange].dilutionExtractVolume3
        Measurer.addParam.dilutionAddBlankVolume3 = setting.measure.range[self.currentRange].dilutionAddBlankVolume3
        Measurer.addParam.afterReagent1AddBlankVolume = setting.measure.range[self.currentRange].afterReagent1AddBlankVolume
        Measurer.addParam.diluteFactor = setting.measure.range[self.currentRange].diluteFactor
        Measurer.addParam.rinseSampleVolume = 0
        Measurer.addParam.rinseBlankVolume = setting.measure.range[self.currentRange].rinseStandardVolume + setting.measure.range[self.currentRange].rinseSampleVolume
        Measurer.addParam.rinseStandardVolume = 0

        --零点流程执行
        local peakHighTabelTC = {}
        local peakHighTabelIC = {}
        --测量次数
        local measureTimes = 5
        --舍弃次数
        local throwNum = 2
        config.measureParam.reagent1Vol = 1
        config.measureParam.reagent2Vol = 0
        config.modifyRecord.measureParam(true)
        ConfigLists.SaveMeasureParamConfig()
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
                log:debug("第" .. j .. "次空白水TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
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
        peak[1] = sumTC / (measureTimes - throwNum)
        peakIC[1] =  sumIC / (measureTimes - throwNum)
        log:debug("空白水测试TC峰值= " .. peak[1] .. ", IC峰值= " .. peakIC[1])
        blankConsistency.TC = self:CalculateConsistency(peak[1], ModelType.TC)
        blankConsistency.IC = self:CalculateConsistency(peakIC[1], ModelType.IC)
        blankConsistency.TOC = blankConsistency.TC - blankConsistency.IC

        for k ,v in pairs(config.measureParam.calibratePointConsistency) do
            if k > 1 then
                consistency[k] = v + blankConsistency.TOC + blankConsistency.IC
            end
            print(" C" .. " [" .. k .. "] = " .. consistency[k])
        end
        log:debug("空白水测试TC浓度= " .. blankConsistency.TC .. ", IC浓度= " .. blankConsistency.IC .. ", TOC浓度= " .. blankConsistency.TOC)
    end

    --需要测量标点
    if self.measureStandard == true then

        log:debug("确认-单点确认TC测量")

        local updateWidgetManager = UpdateWidgetManager.Instance()
        local tipStr = "[" .. string.format("%.3f", self.confirmConsistency) .. "ppm]"
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. tipStr .. "TOC标液后，点击确认开始下一步确认(2/3)")

        --测量标点
        if Measurer.flow then
            Measurer:Reset()
        end
        Measurer.flow = self
        Measurer.measureType = MeasureType.Standard
        Measurer.currentRange = self.currentRange
        for k,v in pairs(addParam[2]) do
            Measurer.addParam [k] = v
        end

        --根据量程修改参数
        Measurer.addParam.standardVolume = setting.measure.range[self.currentRange].sampleVolume
        Measurer.addParam.blankVolume = setting.measure.range[self.currentRange].blankVolume
        Measurer.addParam.dilutionExtractVolume1 = setting.measure.range[self.currentRange].dilutionExtractVolume1
        Measurer.addParam.dilutionAddBlankVolume1 = setting.measure.range[self.currentRange].dilutionAddBlankVolume1
        Measurer.addParam.dilutionExtractVolume2 = setting.measure.range[self.currentRange].dilutionExtractVolume2
        Measurer.addParam.dilutionAddBlankVolume2 = setting.measure.range[self.currentRange].dilutionAddBlankVolume2
        Measurer.addParam.dilutionExtractVolume3 = setting.measure.range[self.currentRange].dilutionExtractVolume3
        Measurer.addParam.dilutionAddBlankVolume3 = setting.measure.range[self.currentRange].dilutionAddBlankVolume3
        Measurer.addParam.afterReagent1AddBlankVolume = setting.measure.range[self.currentRange].afterReagent1AddBlankVolume
        Measurer.addParam.diluteFactor = setting.measure.range[self.currentRange].diluteFactor
        Measurer.addParam.rinseSampleVolume = 0
        Measurer.addParam.rinseBlankVolume = 0
        Measurer.addParam.rinseStandardVolume = setting.measure.range[self.currentRange].rinseStandardVolume + setting.measure.range[self.currentRange].rinseSampleVolume

        local peakHighTabelTC = {}
        local peakHighTabelIC = {}
        --测量次数
        local measureTimes = 4
        --舍弃次数
        local throwNum = 1
        if self.confirmConsistency > 0.5 then
            config.measureParam.reagent1Vol = 1
            config.measureParam.reagent2Vol = 0
        else
            config.measureParam.reagent1Vol = 1
            config.measureParam.reagent2Vol = 0
        end

        config.modifyRecord.measureParam(true)
        ConfigLists.SaveMeasureParamConfig()
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
                log:debug("第" .. j .. "次" .. tipStr .. "TOC标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
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
        log:debug(tipStr .. "TOC标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算单点浓度(TOC)
        local measureTOC = {}
        --TOC理论浓度+空白水TOC浓度
        local consistencyTOC = blankConsistency.TOC + self.confirmConsistency
        local avconsistencyTOC
        --实测浓度
        measureTOC[1] =  self:CalculateConsistency(peakHighTabelTC[2], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[2] =  self:CalculateConsistency(peakHighTabelTC[3], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[3], ModelType.IC)
        measureTOC[3] =  self:CalculateConsistency(peakHighTabelTC[4], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[4], ModelType.IC)
        avconsistencyTOC = (measureTOC[1] + measureTOC[2] + measureTOC[3]) / 3
        --计算TOC相对标准偏差
        cRSD.TOC = self:ConsistencyRSD(measureTOC,3) * 100
        --计算TOC偏差值
        checkError.TOC = math.abs(avconsistencyTOC - consistencyTOC) / consistencyTOC * 100
        log:debug("TOC浓度 = " .. string.format("%.3f", avconsistencyTOC) .. ", 理论浓度 = " .. string.format("%.3f", consistencyTOC))
        log:debug("TOC浓度相对标准偏差 = " .. string.format("%.2f", cRSD.TOC))
        log:debug("TOC浓度偏差 = " .. string.format("%.2f", checkError.TOC) .. "%")
        if cRSD.TOC > setting.measureResult.rsdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            tipStr = tipStr .. "单点确认失败,RSD " .. string.format("%.2f", cRSD.TOC) .. "%(2/3)"
            log:debug(tipStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
            return
        end

        local checkFault = false
        if checkError.TOC > setting.measureResult.hsdLimited * 100 and self.confirmConsistency > setting.measureResult.keyConsistency then
            checkFault = true
        elseif checkError.TOC > setting.measureResult.sdLimited * 100 and self.confirmConsistency <= setting.measureResult.keyConsistency then
            checkFault = true
        end

        if checkFault then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            tipStr = tipStr .. "单点确认失败,偏差" .. string.format("%.2f", checkError.TOC) .. "%(2/3)"
            log:debug(tipStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
            return
        else
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = tipStr .. "TOC标液确认成功,偏差" .. string.format("%.2f", checkError.TOC) .. "%(2/3)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
        end

        log:debug("确认-单点确认IC测量")

        local updateWidgetManager = UpdateWidgetManager.Instance()
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. tipStr .. "IC标液后，点击确认开始下一步确认(3/3)")

        log:debug("关闭紫外灯")
        dc:GetIOpticalAcquire():TurnOffLED()	--关LED

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
                log:debug("第" .. j .. "次" .. tipStr .. "IC标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
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
        log:debug(tipStr .. "IC标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算单点浓度(IC)
        local measureIC = {}
        local measureTC = {}
        local measureTOC = {}
        --理论IC浓度 = TC浓度
        local consistencyIC
        local avConsistencyIC
        --实测TC浓度
        measureTC[1] =  self:CalculateConsistency(peakHighTabelTC[2], ModelType.TC)
        measureTC[2] =  self:CalculateConsistency(peakHighTabelTC[3], ModelType.TC)
        measureTC[3] =  self:CalculateConsistency(peakHighTabelTC[4], ModelType.TC)
        --实测IC浓度
        measureIC[1] =  self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureIC[2] =  self:CalculateConsistency(peakHighTabelIC[3], ModelType.IC)
        measureIC[3] =  self:CalculateConsistency(peakHighTabelIC[4], ModelType.IC)
        --实测TOC浓度
        measureTOC[1] = measureTC[1] - measureIC[1]
        measureTOC[2] = measureTC[2] - measureIC[2]
        measureTOC[3] = measureTC[3] - measureIC[3]

        avConsistencyIC = (measureIC[1] + measureIC[2] + measureIC[3]) /3
        consistencyIC = (measureTC[1] + measureTC[2] + measureTC[3]) /3

        --IC相对标准偏差
        cRSD.IC = self:ConsistencyRSD(measureIC,3) * 100

        --TC相对标准偏差
        cRSD.TC = self:ConsistencyRSD(measureTC,3) * 100

        --TOC相对标准偏差
        cRSD.TOC = self:ConsistencyRSD(measureTOC,3) * 100

        checkError.IC = math.abs(avConsistencyIC - consistencyIC) / consistencyIC * 100

        log:debug("IC浓度 = " .. string.format("%.3f", avConsistencyIC) .. ", 理论浓度 = " .. string.format("%.3f", consistencyIC))
        log:debug("IC浓度相对标准偏差 = " .. string.format("%.2f", cRSD.IC))
        log:debug("IC浓度偏差 = " .. string.format("%.2f", checkError.IC))

        if cRSD.TC > setting.measureResult.rsdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            tipStr = tipStr .. "单点确认失败,TC RSD " .. string.format("%.2f", cRSD.TC) .. "%(3/3)"
            log:debug(tipStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
            return
        end

        if cRSD.IC > setting.measureResult.rsdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            tipStr = tipStr .. "单点确认失败,IC RSD " .. string.format("%.2f", cRSD.IC) .. "%(3/3)"
            log:debug(tipStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
            return
        end

        if cRSD.TOC > setting.measureResult.rsdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            tipStr = tipStr .. "单点确认失败,TOC RSD " .. string.format("%.2f", cRSD.TOC) .. "%(3/3)"
            log:debug(tipStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
            return
        end

        local checkFault = false
        if checkError.IC > setting.measureResult.hsdLimited * 100 and self.confirmConsistency > setting.measureResult.keyConsistency then
            checkFault = true
        elseif checkError.IC > setting.measureResult.sdLimited * 100 and self.confirmConsistency <= setting.measureResult.keyConsistency then
            checkFault = true
        end

        if checkFault then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            tipStr = tipStr .. "单点确认失败,偏差" .. string.format("%.2f", checkError.IC) .. "%(2/3)"
            log:debug(tipStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
            return
        else
            local updateWidgetManager = UpdateWidgetManager.Instance()
            tipStr = tipStr .. "IC标液确认成功,偏差" .. string.format("%.2f", checkError.IC) .. "%(2/3)"
            log:debug(tipStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
        end
        self.isFinish = true
        log:debug("打开紫外灯")
        dc:GetIOpticalAcquire():TurnOnLED()
    end

    --需要测量苯醌Rss
    if self.measureRss == true then

        log:debug("确认-蔗糖测量")

        local updateWidgetManager = UpdateWidgetManager.Instance()
        local tipStr = "[" .. string.format("%.3f", self.confirmConsistency) .. "ppm]"
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. tipStr .. "蔗糖标液后，点击确认开始下一步确认(2/3)")

        --测量标点
        if Measurer.flow then
            Measurer:Reset()
        end
        Measurer.flow = self
        Measurer.measureType = MeasureType.Standard
        Measurer.currentRange = self.currentRange
        for k,v in pairs(addParam[2]) do
            Measurer.addParam [k] = v
        end

        --根据量程修改参数
        Measurer.addParam.standardVolume = setting.measure.range[self.currentRange].sampleVolume
        Measurer.addParam.blankVolume = setting.measure.range[self.currentRange].blankVolume
        Measurer.addParam.dilutionExtractVolume1 = setting.measure.range[self.currentRange].dilutionExtractVolume1
        Measurer.addParam.dilutionAddBlankVolume1 = setting.measure.range[self.currentRange].dilutionAddBlankVolume1
        Measurer.addParam.dilutionExtractVolume2 = setting.measure.range[self.currentRange].dilutionExtractVolume2
        Measurer.addParam.dilutionAddBlankVolume2 = setting.measure.range[self.currentRange].dilutionAddBlankVolume2
        Measurer.addParam.dilutionExtractVolume3 = setting.measure.range[self.currentRange].dilutionExtractVolume3
        Measurer.addParam.dilutionAddBlankVolume3 = setting.measure.range[self.currentRange].dilutionAddBlankVolume3
        Measurer.addParam.afterReagent1AddBlankVolume = setting.measure.range[self.currentRange].afterReagent1AddBlankVolume
        Measurer.addParam.diluteFactor = setting.measure.range[self.currentRange].diluteFactor
        Measurer.addParam.rinseSampleVolume = 0
        Measurer.addParam.rinseBlankVolume = 0
        Measurer.addParam.rinseStandardVolume = setting.measure.range[self.currentRange].rinseStandardVolume + setting.measure.range[self.currentRange].rinseSampleVolume

        local peakHighTabelTC = {}
        local peakHighTabelIC = {}
        local cRw = blankConsistency.TOC  --空白浓度
        local cRs = 0    --蔗糖浓度
        local cRsRsd = 0 --蔗糖相对标准偏差
        local cRss = 0   --苯醌浓度
        local cRssRsd = 0 --苯醌相对标准偏差
        --测量次数
        local measureTimes = 4
        --舍弃次数
        local throwNum = 1


        if self.ConfirmType == ConfirmType.sterileWaterAdaptability then
            config.measureParam.reagent1Vol = 2
            config.measureParam.reagent2Vol = 1
        end
        config.modifyRecord.measureParam(true)
        ConfigLists.SaveMeasureParamConfig()
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
                log:debug("第" .. j .. "次" .. tipStr .. "蔗糖标液TC修正峰值= " .. peakHighTabelTC[j] .. ", IC修正峰值= " .. peakHighTabelIC[j])
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
        log:debug(tipStr .. "蔗糖标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算单点浓度(TOC)
        local measureTOC = {}
        --实测浓度
        measureTOC[1] =  self:CalculateConsistency(peakHighTabelTC[2], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[2] =  self:CalculateConsistency(peakHighTabelTC[3], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[3] =  self:CalculateConsistency(peakHighTabelTC[4], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        --蔗糖浓度
        cRs = (measureTOC[1] + measureTOC[2] + measureTOC[3]) / 3
        --蔗糖相对标准偏差
        cRsRsd = self:ConsistencyRSD(measureTOC,3) * 100

        log:debug("确认-苯醌测量")

        local updateWidgetManager = UpdateWidgetManager.Instance()
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. tipStr .. "苯醌标液后，点击确认开始下一步确认(3/3)")

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
                log:debug("第" .. j .. "次" .. tipStr .. "苯醌标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
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
        log:debug(tipStr .. "苯醌标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算单点浓度(TOC)
        local measureTOC = {}
        --实测浓度
        measureTOC[1] =  self:CalculateConsistency(peakHighTabelTC[2], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[2] =  self:CalculateConsistency(peakHighTabelTC[3], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[3] =  self:CalculateConsistency(peakHighTabelTC[4], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        --苯醌浓度
        cRss = (measureTOC[1] + measureTOC[2] + measureTOC[3]) / 3
        --苯醌相对标准偏差
        cRssRsd = self:ConsistencyRSD(measureTOC,3) * 100
        --响应效率
        local corEff = math.abs(cRss - cRw) / (cRs - cRw) * 100

        log:debug("空白浓度cRw = " .. string.format("%.3f", cRw)
                .. ", 蔗糖浓度cRs = " .. string.format("%.3f", cRs)
                .. ", 苯醌浓度cRss = " .. string.format("%.3f", cRss))
        log:debug("蔗糖RsRSD = " .. string.format("%.3f", cRsRsd) ..
                ",苯醌RssRSD = " .. string.format("%.3f", cRssRsd) ..
                ",响应效率 = " .. string.format("%.3f", corEff) .. "%")
        if self.ConfirmType == ConfirmType.systemAdaptability or self.ConfirmType == ConfirmType.sterileWaterAdaptability then
            if corEff > 115 or corEff < 85  then
                local updateWidgetManager = UpdateWidgetManager.Instance()
                tipStr = tipStr .. flowName .. "失败,响应效率 " .. string.format("%.2f", corEff) .. "%(3/3)"
                log:debug(tipStr)
                updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
                return
            else
                local updateWidgetManager = UpdateWidgetManager.Instance()
                tipStr = tipStr .. flowName .. "成功,响应效率 " .. string.format("%.2f", corEff) .. "%(3/3)"
                log:debug(tipStr)
                updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
                self.isFinish = true
            end
        elseif self.ConfirmType == ConfirmType.robustness then
            if cRsRsd > setting.measureResult.rsdLimited*100 or cRssRsd > setting.measureResult.rsdLimited*100 then
                local updateWidgetManager = UpdateWidgetManager.Instance()
                tipStr = tipStr .. flowName .. "失败,[蔗糖]RSD " .. string.format("%.2f", cRsRsd) .. "%, 苯醌RSD "
                        .. string.format("%.2f", cRssRsd) .. "%(3/3)"
                log:debug(tipStr)
                updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
                return
            else
                local updateWidgetManager = UpdateWidgetManager.Instance()
                tipStr = tipStr .. flowName .. "成功,[蔗糖]RSD " .. string.format("%.2f", cRsRsd) .. "%, 苯醌RSD "
                        .. string.format("%.2f", cRssRsd) .. "%(3/3)"
                log:debug(tipStr)
                updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
            end
        end
        self.isFinish = true
    end

    --需要测量特异性 甲醇-烟酰胺-KHP
    if self.measureSpecificity == true then

        log:debug("确认-甲醇测量")

        local updateWidgetManager = UpdateWidgetManager.Instance()
        local tipStr = "[" .. string.format("%.3f", self.confirmConsistency) .. "ppm]"
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. tipStr .. "甲醇标液后，点击确认开始下一步确认(2/4)")

        --测量标点
        if Measurer.flow then
            Measurer:Reset()
        end
        Measurer.flow = self
        Measurer.measureType = MeasureType.Standard
        Measurer.currentRange = self.currentRange
        for k,v in pairs(addParam[2]) do
            Measurer.addParam [k] = v
        end

        --根据量程修改参数
        Measurer.addParam.standardVolume = setting.measure.range[self.currentRange].sampleVolume
        Measurer.addParam.blankVolume = setting.measure.range[self.currentRange].blankVolume
        Measurer.addParam.dilutionExtractVolume1 = setting.measure.range[self.currentRange].dilutionExtractVolume1
        Measurer.addParam.dilutionAddBlankVolume1 = setting.measure.range[self.currentRange].dilutionAddBlankVolume1
        Measurer.addParam.dilutionExtractVolume2 = setting.measure.range[self.currentRange].dilutionExtractVolume2
        Measurer.addParam.dilutionAddBlankVolume2 = setting.measure.range[self.currentRange].dilutionAddBlankVolume2
        Measurer.addParam.dilutionExtractVolume3 = setting.measure.range[self.currentRange].dilutionExtractVolume3
        Measurer.addParam.dilutionAddBlankVolume3 = setting.measure.range[self.currentRange].dilutionAddBlankVolume3
        Measurer.addParam.afterReagent1AddBlankVolume = setting.measure.range[self.currentRange].afterReagent1AddBlankVolume
        Measurer.addParam.diluteFactor = setting.measure.range[self.currentRange].diluteFactor
        Measurer.addParam.rinseSampleVolume = 0
        Measurer.addParam.rinseBlankVolume = 0
        Measurer.addParam.rinseStandardVolume = setting.measure.range[self.currentRange].rinseStandardVolume + setting.measure.range[self.currentRange].rinseSampleVolume

        local peakHighTabelTC = {}
        local peakHighTabelIC = {}
        local cRw = blankConsistency.TOC + self.confirmConsistency --空白浓度
        local cMeoh = 0    --甲醇浓度
        local cMeohRsd = 0 --甲醇相对标准偏差
        local cMeohDe = 0  --甲醇偏差
        local cNico = 0    --烟酰胺浓度
        local cNicoRsd = 0 --烟酰胺相对标准偏差
        local cNicoDe = 0  --甲醇偏差
        local cKHP = 0     --KHP浓度
        local cKHPRsd = 0  --KHP相对标准偏差
        local cKHPDe = 0   --KHP偏差
        --测量次数
        local measureTimes = 4
        --舍弃次数
        local throwNum = 1

        if self.ConfirmType == ConfirmType.sterileWaterAdaptability then
            config.measureParam.reagent1Vol = 2
            config.measureParam.reagent2Vol = 1
        end
        config.modifyRecord.measureParam(true)
        ConfigLists.SaveMeasureParamConfig()
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
                log:debug("第" .. j .. "次" .. tipStr .. "甲醇标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
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
        log:debug(tipStr .. "甲醇标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算单点浓度(TOC)
        local measureTOC = {}
        --实测浓度
        measureTOC[1] =  self:CalculateConsistency(peakHighTabelTC[2], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[2] =  self:CalculateConsistency(peakHighTabelTC[3], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[3] =  self:CalculateConsistency(peakHighTabelTC[4], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        --甲醇浓度
        cMeoh = (measureTOC[1] + measureTOC[2] + measureTOC[3]) / 3
        --甲醇相对标准偏差
        cMeohRsd = self:ConsistencyRSD(measureTOC,3) * 100
        --甲醇偏差
        cMeohDe = math.abs(cMeoh - cRw) / cRw * 100

        --满足条件
        if cMeohRsd <= setting.measureResult.rsdLimited * 100
                and cMeohDe <= setting.measureResult.sdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = tipStr .. flowName .. "[甲醇]偏差 " .. string.format("%.2f", cMeohDe) .. "%, " ..
                    ",[甲醇]RSD " .. string.format("%.2f", cMeohRsd) .. "%(2/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
        elseif cMeohRsd > setting.measureResult.rsdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = tipStr .. flowName .. "失败,[甲醇]RSD " .. string.format("%.2f", cMeohRsd) .. "%(2/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        elseif cMeohDe > setting.measureResult.sdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = tipStr .. flowName .. "失败,[甲醇]偏差 " .. string.format("%.2f", cMeohDe) .. "%(2/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        end

        log:debug("确认-烟酰胺测量")

        local updateWidgetManager = UpdateWidgetManager.Instance()
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. tipStr .. "烟酰胺标液后，点击确认开始下一步确认(3/4)")

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
                log:debug("第" .. j .. "次" .. tipStr .. "烟酰胺标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
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
        log:debug(tipStr .. "烟酰胺标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算单点浓度(TOC)
        local measureTOC = {}
        --实测浓度
        measureTOC[1] =  self:CalculateConsistency(peakHighTabelTC[2], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[2] =  self:CalculateConsistency(peakHighTabelTC[3], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[3] =  self:CalculateConsistency(peakHighTabelTC[4], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        --烟酰胺浓度
        cNico = (measureTOC[1] + measureTOC[2] + measureTOC[3]) /3
        --烟酰胺相对标准偏差
        cNicoRsd = self:ConsistencyRSD(measureTOC,3) * 100
        --烟酰胺偏差
        cNicoDe = math.abs(cNico - cRw) / cRw * 100

        if cNicoRsd <= setting.measureResult.rsdLimited * 100
            and cNicoDe <= setting.measureResult.sdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = tipStr .. flowName .. "[烟酰胺]RSD " .. string.format("%.2f", cNicoRsd) .. "%," ..
                    ",[烟酰胺]偏差 " .. string.format("%.2f", cNicoDe) .. "%(3/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
        elseif cNicoRsd > setting.measureResult.rsdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = tipStr .. flowName .. "失败,[烟酰胺]RSD " .. string.format("%.2f", cNicoRsd) .. "%(3/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        elseif cNicoDe > setting.measureResult.sdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = tipStr .. flowName .. "失败,[烟酰胺]偏差 " .. string.format("%.2f", cNicoDe) .. "%(3/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        end

        log:debug("确认-KHP测量")

        local updateWidgetManager = UpdateWidgetManager.Instance()
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. tipStr .. "KHP标液后，点击确认开始下一步确认(4/4)")

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
                log:debug("第" .. j .. "次" .. tipStr .. "KHP标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
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
        log:debug(tipStr .. "KHP标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算单点浓度(TOC)
        local measureTOC = {}
        --实测浓度
        measureTOC[1] =  self:CalculateConsistency(peakHighTabelTC[2], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[2] =  self:CalculateConsistency(peakHighTabelTC[3], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[3] =  self:CalculateConsistency(peakHighTabelTC[4], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        --KHP浓度
        cKHP = (measureTOC[1] + measureTOC[2] + measureTOC[3]) /3
        --KHP相对标准偏差
        cKHPRsd = self:ConsistencyRSD(measureTOC,3) * 100
        --KHP偏差
        cKHPDe = math.abs(cKHP - cRw) / cRw * 100
        if cKHPRsd <= setting.measureResult.rsdLimited * 100
            and cKHPDe <= setting.measureResult.sdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = tipStr .. flowName .. "[KHP]RSD " .. string.format("%.2f", cKHPRsd) .. "%," ..
                    ",[KHP]偏差 " .. string.format("%.2f", cKHPDe) .. "%(4/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
        elseif cKHPRsd > setting.measureResult.rsdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local  showStr = tipStr .. flowName .. "失败,[KHP]RSD " .. string.format("%.2f", cKHPRsd) .. "%(4/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        elseif cKHPDe > setting.measureResult.sdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = tipStr .. flowName .. "失败,[KHP]偏差 " .. string.format("%.2f", cKHPDe) .. "%(4/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        end

        local updateWidgetManager = UpdateWidgetManager.Instance()
        tipStr = tipStr .. flowName .. "成功(4/4)"
        log:debug(tipStr)
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)

        self.isFinish = true
    end

    --需要测量线性 250-500-750
    if self.measureLinear == true then

        log:debug("确认-[250ppb]测量")
        self.confirmConsistency = 0.25
        local updateWidgetManager = UpdateWidgetManager.Instance()
        local tipStr = "[" .. string.format("%.3f", self.confirmConsistency) .. "ppm]"
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. tipStr .. "TOC标液后，点击确认开始下一步确认(2/4)")

        --测量标点
        if Measurer.flow then
            Measurer:Reset()
        end
        Measurer.flow = self
        Measurer.measureType = MeasureType.Standard
        Measurer.currentRange = self.currentRange
        for k,v in pairs(addParam[2]) do
            Measurer.addParam [k] = v
        end

        --根据量程修改参数
        Measurer.addParam.standardVolume = setting.measure.range[self.currentRange].sampleVolume
        Measurer.addParam.blankVolume = setting.measure.range[self.currentRange].blankVolume
        Measurer.addParam.dilutionExtractVolume1 = setting.measure.range[self.currentRange].dilutionExtractVolume1
        Measurer.addParam.dilutionAddBlankVolume1 = setting.measure.range[self.currentRange].dilutionAddBlankVolume1
        Measurer.addParam.dilutionExtractVolume2 = setting.measure.range[self.currentRange].dilutionExtractVolume2
        Measurer.addParam.dilutionAddBlankVolume2 = setting.measure.range[self.currentRange].dilutionAddBlankVolume2
        Measurer.addParam.dilutionExtractVolume3 = setting.measure.range[self.currentRange].dilutionExtractVolume3
        Measurer.addParam.dilutionAddBlankVolume3 = setting.measure.range[self.currentRange].dilutionAddBlankVolume3
        Measurer.addParam.afterReagent1AddBlankVolume = setting.measure.range[self.currentRange].afterReagent1AddBlankVolume
        Measurer.addParam.diluteFactor = setting.measure.range[self.currentRange].diluteFactor
        Measurer.addParam.rinseSampleVolume = 0
        Measurer.addParam.rinseBlankVolume = 0
        Measurer.addParam.rinseStandardVolume = setting.measure.range[self.currentRange].rinseStandardVolume + setting.measure.range[self.currentRange].rinseSampleVolume

        local peakHighTabelTC = {}
        local peakHighTabelIC = {}
        local cRw = blankConsistency.TOC + self.confirmConsistency  --空白浓度
        local cAvg = {}         --TOC平均浓度
        local cRsd = {}         --TOC浓度相对标准偏差
        local cDeviation = {}   --TOC浓度偏差
        --测量次数
        local measureTimes = 4
        --舍弃次数
        local throwNum = 1


        config.measureParam.reagent1Vol = 1
        config.measureParam.reagent2Vol = 0
        config.modifyRecord.measureParam(true)
        ConfigLists.SaveMeasureParamConfig()
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
                log:debug("第" .. j .. "次" .. tipStr .. "TOC标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
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
        log:debug("[250ppb]TOC标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算单点浓度(TOC)
        local measureTOC = {}
        --实测浓度
        measureTOC[1] =  self:CalculateConsistency(peakHighTabelTC[2], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[2] =  self:CalculateConsistency(peakHighTabelTC[3], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[3] =  self:CalculateConsistency(peakHighTabelTC[4], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        --250ppb浓度
        cAvg[1] = (measureTOC[1] + measureTOC[2] + measureTOC[3]) / 3
        --250ppb相对标准偏差
        cRsd[1] = self:ConsistencyRSD(measureTOC,3) * 100
        --250ppb偏差
        cDeviation[1] = math.abs(cAvg[1] - cRw) / cRw * 100

        log:debug("空白浓度cRw = " .. string.format("%.3f", cRw) .. "标液浓度 = " .. string.format("%.3f", cAvg[1]))
        log:debug("[250ppb]RSD = " .. string.format("%.3f", cRsd[1]) .. ", De = " .. string.format("%.3f", cDeviation[1]))

        if cRsd[1] > setting.measureResult.rsdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = flowName .. "失败,[250ppb]RSD " .. string.format("%.2f", cRsd[1]) .. "%(2/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        end
        if cDeviation[1] > setting.measureResult.sdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = flowName .. "失败,[250ppb]偏差 " .. string.format("%.2f", cDeviation[1]) .. "%(2/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        end

        local updateWidgetManager = UpdateWidgetManager.Instance()
        local showStr = flowName .. "成功,[250ppb]偏差 " .. string.format("%.2f", cDeviation[1]) .. "%(2/4)"
        log:debug(showStr)
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)

        log:debug("确认-[500ppb]测量")
        self.confirmConsistency = 0.5
        tipStr = "[" .. string.format("%.3f", self.confirmConsistency) .. "ppm]"
        cRw = blankConsistency.TOC + self.confirmConsistency  --空白浓度
        local updateWidgetManager = UpdateWidgetManager.Instance()
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. tipStr .. "TOC标液后，点击确认开始下一步确认(3/4)")

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
                log:debug("第" .. j .. "次" .. tipStr .. "TOC标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
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
        log:debug("[500ppb]TOC标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算单点浓度(TOC)
        local measureTOC = {}
        --实测浓度
        measureTOC[1] =  self:CalculateConsistency(peakHighTabelTC[2], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[2] =  self:CalculateConsistency(peakHighTabelTC[3], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[3] =  self:CalculateConsistency(peakHighTabelTC[4], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        --500ppb浓度
        cAvg[2] = (measureTOC[1] + measureTOC[2] + measureTOC[3]) / 3
        --500ppb相对标准偏差
        cRsd[2] = self:ConsistencyRSD(measureTOC,3) * 100
        --500ppb偏差
        cDeviation[2] = math.abs(cAvg[2] - cRw) / cRw * 100

        log:debug("空白浓度cRw = " .. string.format("%.3f", cRw) .. "标液浓度 = " .. string.format("%.3f", cAvg[2]))
        log:debug("[500ppb]RSD = " .. string.format("%.3f", cRsd[2]) .. ", De = " .. string.format("%.3f", cDeviation[2]))

        if cRsd[2] > setting.measureResult.rsdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = flowName .. "失败,[500ppb]RSD " .. string.format("%.2f", cRsd[2]) .. "%(3/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        end
        if cDeviation[2] > setting.measureResult.sdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = flowName .. "失败,[500ppb]偏差 " .. string.format("%.2f", cDeviation[2]) .. "%(3/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        end

        showStr = flowName .. "成功,[500ppb]偏差 " .. string.format("%.2f", cDeviation[2]) .. "%(3/4)"
        log:debug(showStr)
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)

        log:debug("确认-[750ppb]测量")
        self.confirmConsistency = 0.75
        tipStr = "[" .. string.format("%.3f", self.confirmConsistency) .. "ppm]"
        cRw = blankConsistency.TOC + self.confirmConsistency  --空白浓度
        local updateWidgetManager = UpdateWidgetManager.Instance()
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. tipStr .. "TOC标液后，点击确认开始下一步确认(4/4)")

        config.measureParam.reagent1Vol = 1
        config.measureParam.reagent2Vol = 0.1
        config.modifyRecord.measureParam(true)
        ConfigLists.SaveMeasureParamConfig()
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
                log:debug("第" .. j .. "次" .. tipStr .. "KHP标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
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
        log:debug("[750ppb]TOC标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算单点浓度(TOC)
        local measureTOC = {}
        --实测浓度
        measureTOC[1] =  self:CalculateConsistency(peakHighTabelTC[2], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[2] =  self:CalculateConsistency(peakHighTabelTC[3], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[3] =  self:CalculateConsistency(peakHighTabelTC[4], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        --750ppb浓度
        cAvg[3] = (measureTOC[1] + measureTOC[2] + measureTOC[3]) / 3
        --750ppb相对标准偏差
        cRsd[3] = self:ConsistencyRSD(measureTOC,3) * 100
        --750ppb偏差
        cDeviation[3] = math.abs(cAvg[3] - cRw) / cRw * 100

        log:debug("空白浓度cRw = " .. string.format("%.3f", cRw) .. "标液浓度 = " .. string.format("%.3f", cAvg[1]))
        log:debug("[750ppb]RSD = " .. string.format("%.3f", cRsd[3]) .. ", De = " .. string.format("%.3f", cDeviation[3]))
        if cRsd[3] > setting.measureResult.rsdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = flowName .. "失败,[750ppb]RSD " .. string.format("%.2f", cRsd[3]) .. "%(4/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        end
        if cDeviation[3] > setting.measureResult.sdLimited * 100 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = flowName .. "失败,[750ppb]偏差 " .. string.format("%.2f", cDeviation[3]) .. "%(4/4)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        end

        showStr = flowName .. "成功,[750ppb]偏差 " .. string.format("%.2f", cDeviation[3]) .. "%(3/4)"
        log:debug(showStr)
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)

        local updateWidgetManager = UpdateWidgetManager.Instance()
        local showStr = flowName .. "成功(4/4)"
        log:debug(showStr)
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
        self.isFinish = true
    end

    --需要测量SDBS
    if self.measureSDBS == true then

        log:debug("确认-SDBS测量")

        local updateWidgetManager = UpdateWidgetManager.Instance()
        local tipStr = "[" .. string.format("%.3f", self.confirmConsistency) .. "ppm]"
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. tipStr .. "SDBS标液后，点击确认开始下一步确认(2/2)")

        --测量标点
        if Measurer.flow then
            Measurer:Reset()
        end
        Measurer.flow = self
        Measurer.measureType = MeasureType.Standard
        Measurer.currentRange = self.currentRange
        for k,v in pairs(addParam[2]) do
            Measurer.addParam [k] = v
        end

        --根据量程修改参数
        Measurer.addParam.standardVolume = setting.measure.range[self.currentRange].sampleVolume
        Measurer.addParam.blankVolume = setting.measure.range[self.currentRange].blankVolume
        Measurer.addParam.dilutionExtractVolume1 = setting.measure.range[self.currentRange].dilutionExtractVolume1
        Measurer.addParam.dilutionAddBlankVolume1 = setting.measure.range[self.currentRange].dilutionAddBlankVolume1
        Measurer.addParam.dilutionExtractVolume2 = setting.measure.range[self.currentRange].dilutionExtractVolume2
        Measurer.addParam.dilutionAddBlankVolume2 = setting.measure.range[self.currentRange].dilutionAddBlankVolume2
        Measurer.addParam.dilutionExtractVolume3 = setting.measure.range[self.currentRange].dilutionExtractVolume3
        Measurer.addParam.dilutionAddBlankVolume3 = setting.measure.range[self.currentRange].dilutionAddBlankVolume3
        Measurer.addParam.afterReagent1AddBlankVolume = setting.measure.range[self.currentRange].afterReagent1AddBlankVolume
        Measurer.addParam.diluteFactor = setting.measure.range[self.currentRange].diluteFactor
        Measurer.addParam.rinseSampleVolume = 0
        Measurer.addParam.rinseBlankVolume = 0
        Measurer.addParam.rinseStandardVolume = setting.measure.range[self.currentRange].rinseStandardVolume + setting.measure.range[self.currentRange].rinseSampleVolume

        local peakHighTabelTC = {}
        local peakHighTabelIC = {}
        local cRw = blankConsistency.TOC  --空白浓度
        local cAvgSDBS = 0    --SDBS浓度
        local cSub = 0        --SDBS与空白差值
        --测量次数
        local measureTimes = 10
        --舍弃次数
        local throwNum = 7

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
                log:debug("第" .. j .. "次" .. tipStr .. "SDBS标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
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
        log:debug(tipStr .. "SDBS标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算单点浓度(TOC)
        local measureTOC = {}
        --实测浓度
        measureTOC[1] =  self:CalculateConsistency(peakHighTabelTC[2], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[2] =  self:CalculateConsistency(peakHighTabelTC[3], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureTOC[3] =  self:CalculateConsistency(peakHighTabelTC[4], ModelType.TC) - self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        --SDBS浓度
        cAvgSDBS = (measureTOC[1] + measureTOC[2] + measureTOC[3]) / 3
        cSub = cAvgSDBS - cRw
        log:debug("空白浓度cRw = " .. string.format("%.3f", cRw))
        log:debug("SDBS = " .. string.format("%.3f", cAvgSDBS))

        if cRw > 0.25 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = tipStr .. flowName .. "失败,空白TOC平均值= " .. string.format("%.2f", cRw) .. "(2/2)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        end

        if cSub < 0.45 then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            local showStr = tipStr .. flowName .. "失败,SDBS标样差值= " .. string.format("%.2f", cSub) .. "(2/2)"
            log:debug(showStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
            return
        end

        local updateWidgetManager = UpdateWidgetManager.Instance()
        local showStr = tipStr .. flowName .. "成功,SDBS标样差值= " .. string.format("%.2f", cSub) ..
                ",空白TOC平均值= " .. string.format("%.2f", cRw) .. "(2/2)"
        log:debug(showStr)
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, showStr)
        self.isFinish = true
    end

    --需要测量ICR
    if self.measureICR == true then

        log:debug("确认-ICR验证[开]")
        config.measureParam.ICRMode = true
        config.modifyRecord.measureParam(true)
        ConfigLists.SaveMeasureParamConfig()

        local updateWidgetManager = UpdateWidgetManager.Instance()
        local tipStr = "[" .. string.format("%.3f", self.confirmConsistency) .. "ppm]"
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. tipStr .. "IC标液后，点击确认开始下一步确认(1/2)")

        --测量标点
        if Measurer.flow then
            Measurer:Reset()
        end
        Measurer.flow = self
        Measurer.measureType = MeasureType.Standard
        Measurer.currentRange = self.currentRange
        for k,v in pairs(addParam[2]) do
            Measurer.addParam [k] = v
        end

        --根据量程修改参数
        Measurer.addParam.standardVolume = setting.measure.range[self.currentRange].sampleVolume
        Measurer.addParam.blankVolume = setting.measure.range[self.currentRange].blankVolume
        Measurer.addParam.dilutionExtractVolume1 = setting.measure.range[self.currentRange].dilutionExtractVolume1
        Measurer.addParam.dilutionAddBlankVolume1 = setting.measure.range[self.currentRange].dilutionAddBlankVolume1
        Measurer.addParam.dilutionExtractVolume2 = setting.measure.range[self.currentRange].dilutionExtractVolume2
        Measurer.addParam.dilutionAddBlankVolume2 = setting.measure.range[self.currentRange].dilutionAddBlankVolume2
        Measurer.addParam.dilutionExtractVolume3 = setting.measure.range[self.currentRange].dilutionExtractVolume3
        Measurer.addParam.dilutionAddBlankVolume3 = setting.measure.range[self.currentRange].dilutionAddBlankVolume3
        Measurer.addParam.afterReagent1AddBlankVolume = setting.measure.range[self.currentRange].afterReagent1AddBlankVolume
        Measurer.addParam.diluteFactor = setting.measure.range[self.currentRange].diluteFactor
        Measurer.addParam.rinseSampleVolume = 0
        Measurer.addParam.rinseBlankVolume = 0
        Measurer.addParam.rinseStandardVolume = setting.measure.range[self.currentRange].rinseStandardVolume + setting.measure.range[self.currentRange].rinseSampleVolume

        local peakHighTabelTC = {}
        local peakHighTabelIC = {}
        --测量次数
        local measureTimes = 4
        --舍弃次数
        local throwNum = 1
        config.measureParam.reagent1Vol = 1
        config.measureParam.reagent2Vol = 0

        config.modifyRecord.measureParam(true)
        ConfigLists.SaveMeasureParamConfig()
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
                log:debug("第" .. j .. "次" .. tipStr .. "TOC标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
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
        log:debug(tipStr .. "IC标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算打开ICR浓度(IC)
        local measureIC = {}
        --ICR开和关测量浓度
        local consistencyICRTurnOn, consistencyICRTurnOff
        --实测浓度
        measureIC[1] =  self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureIC[2] =  self:CalculateConsistency(peakHighTabelIC[3], ModelType.IC)
        measureIC[3] =  self:CalculateConsistency(peakHighTabelIC[4], ModelType.IC)
        consistencyICRTurnOn = (measureIC[1] + measureIC[2] + measureIC[3]) / 3

        log:debug("确认-ICR验证[关]")

        config.measureParam.ICRMode = false
        config.modifyRecord.measureParam(true)
        ConfigLists.SaveMeasureParamConfig()

        local updateWidgetManager = UpdateWidgetManager.Instance()
        updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请准备好" .. tipStr .. "IC标液后，点击确认开始下一步确认(2/2)")

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
                log:debug("第" .. j .. "次" .. tipStr .. "IC标液TC修正值= " .. peakHighTabelTC[j] .. ", IC修正值= " .. peakHighTabelIC[j])
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
        log:debug(tipStr .. "IC标液测试TC峰值= " .. peak[2] .. ", IC峰值= " .. peakIC[2])

        --计算单点浓度(IC)
        local measureIC = {}
        --实测浓度
        measureIC[1] =  self:CalculateConsistency(peakHighTabelIC[2], ModelType.IC)
        measureIC[2] =  self:CalculateConsistency(peakHighTabelIC[3], ModelType.IC)
        measureIC[3] =  self:CalculateConsistency(peakHighTabelIC[4], ModelType.IC)
        consistencyICRTurnOff = (measureIC[1] + measureIC[2] + measureIC[3]) / 3

        checkError.IC = math.abs(consistencyICRTurnOff - consistencyICRTurnOn) / consistencyICRTurnOff * 100

        local checkFault = false
        if checkError.IC >= 98 then
            checkFault = true
        end

        if checkFault then
            local updateWidgetManager = UpdateWidgetManager.Instance()
            tipStr = tipStr .. "ICR验证失败,偏差" .. string.format("%.2f", checkError.IC) .. "%(2/2)"
            log:debug(tipStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
            return
        else
            local updateWidgetManager = UpdateWidgetManager.Instance()
            tipStr = tipStr .. "ICR验证成功,偏差" .. string.format("%.2f", checkError.IC) .. "%(2/2)"
            log:debug(tipStr)
            updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, tipStr)
        end
        self.isFinish = true
    end
end

function ConfirmFlow:OnStop()

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

    if self.ConfirmType == ConfirmType.calibrate or self.ConfirmType == ConfirmType.systemAdaptability then
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
        eventStr = self.text .. self.result
    else
        local flowStr = "单点确认"

        if self.ConfirmType == ConfirmType.systemAdaptability then
            flowStr = "系统适用性确认"
        elseif self.ConfirmType == ConfirmType.sterileWaterAdaptability then
            self.measureStandard = false
            flowStr = "无菌水适用性确认"
        elseif self.ConfirmType == ConfirmType.robustness then
            flowStr = "鲁棒性验证"
        elseif self.ConfirmType == ConfirmType.specificity then
            flowStr = "特异性验证"
        elseif self.ConfirmType == ConfirmType.linear then
            flowStr = "线性验证"
        elseif self.ConfirmType == ConfirmType.sdbsAdaptability then
            flowStr = "SDBS适用性验证"
        end

        self.result = self.text .. "完成"
        log:info(self.result)
        log:info(flowStr.."总时间 = ".. os.time()-self.calibrateDateTime)
        eventStr = self.result
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
function ConfirmFlow:AccurateCalibrate(area, waveRange, ConfirmFlow, measureType)
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
            ConfirmFlow.zeropeak[2] =  string.format("%.2f", temppeak2)
        else
            ConfirmFlow.standardpeak[2] =  string.format("%.2f", temppeak2)
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
                ConfirmFlow.zeropeak[3] = string.format("%.2f", temppeak3)
            else
                ConfirmFlow.standardpeak[3] = string.format("%.2f", temppeak3)
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
                ConfirmFlow.zeropeak[4] = string.format("%.2f", temppeak4)
            else
                ConfirmFlow.standardpeak[4] = string.format("%.2f", temppeak4)
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

function ConfirmFlow:CalculateConsistency(area, type)
    local consistency = 0
    --local peak = 0
    local curveK = config.measureParam.curveK
    local curveB = config.measureParam.curveB
    local KTC = config.measureParam.TCCurveK
    local KIC = config.measureParam.ICCurveK

    if math.abs(curveK - 0)<0.00001 then
        log:debug("校准数值异常")
        return 0
    end
    log:debug("计算斜率 K = " .. curveK .. ", B = " .. curveB)
    log:debug("计算K(TC) = " .. KTC .. ", B = " .. curveB .. ", K(IC) = " .. KIC)

    if type ~= nil and type == ModelType.TC then
        consistency = KTC * 10^(curveK * math.log(area, 10) + curveB)
    else
        consistency = KIC * 10^(curveK * math.log(area, 10) + curveB)
    end

    return consistency
end

--[[
 * @brief 管路更新
 * @detail 一键运行流程中先执行管路更新再校准
--]]
function ConfirmFlow:PipeRenew()

    local runAction

    -- 清空残留液
    runAction = Helper.Status.SetAction(setting.runAction.systemAdaptability.clearWaste)
    StatusManager.Instance():SetAction(runAction)
    --op:DrainToWaste(setting.liquid.meterPipeVolume)

    -- 清空试剂一管
    runAction = Helper.Status.SetAction(setting.runAction.systemAdaptability.clearReagent1Pipe)
    StatusManager.Instance():SetAction(runAction)

    --清空试剂二管
    runAction = Helper.Status.SetAction(setting.runAction.systemAdaptability.clearReagent2Pipe)
    StatusManager.Instance():SetAction(runAction)
end

--[[
 * @brief 检查是否需要进行深度清洗
 * @detail 若上次校准间隔时间较长则进行深度清洗
--]]
function ConfirmFlow:CleanDeeplyCheckTime()
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
function ConfirmFlow:CalculateReviseParam(absorbance)
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
function ConfirmFlow:AlgorithmLeastSquareMethod(consistencyTable, absorbanceTable, num)
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
function ConfirmFlow:AlgorithmFitGoodness(k, b, consistencyTable, absorbanceTable, num)
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


--[[
 * @brief 相对标准偏差
 * @details
 * @param[in] consistencyTable 浓度数据表
 * @param[in] num 数据点个数
--]]
function ConfirmFlow:ConsistencyRSD(consistencyTable, num)
    local rsd = 0           --相对标准偏差
    local cAvg = 0          --浓度的平均值
    local ssStd = 0         --浓度标准差

    --Y均值
    for i = 1,num do
        cAvg = cAvg + consistencyTable[i]
    end
    cAvg = cAvg / num

    --浓度方差和
    for i = 1,num do
        ssStd = ssStd + (consistencyTable[i]-cAvg)^2
    end
    ssStd = ssStd / num

    rsd = math.sqrt(ssStd)

    return rsd
end