--[[
 * @brief 测量流程。
--]]

CCEPSafeData = 0   --全局变量

MeasureFlow = Flow:new
{
    measureTarget = setting.liquidType.none,
    measureType = MeasureType.Sample,
    reportMode = ReportMode.OnLine,
    adjustTime = false,
    currentRange = 1,
    isCheck = false,
    consistency = 0,
    consistencyTC = 0,
    consistencyIC = 0,
    absorbance = 0,
    isUseStart = false,
    faultTimes = 0,
    isRetryMeasure = false,
    isRetryOnStart = false,
    isCrashMeasure = false,
    diluteFactor = 1,
    peakTC = 0,
    peakIC = 0,
    isChangeRangeAtTheEnd = false,
    targetRange = 1,
    isSampleRangeCheck = false,  --水样核查
    isOverProof = false,
    text = "",
    meaType = "在线",
}

function MeasureFlow:new(o, meaType)
    o = o or {}
    setmetatable(o, self)
    self.__index = self

    o.measureType = meaType
    o.measureDateTime = os.time()
    o.currentRange = config.measureParam.range[config.measureParam.currentRange + 1] + 1
    o.reportMode = config.interconnection.reportMode
    o.adjustTime = false
    o.rangeAccurateCalibrate = config.measureParam.rangeAccurateCalibrate
    o.curveCalibrateConsistency = {}
    o.curveCalibrateTime = 1	        -- 量程校正当前次数
    o.curveCalibrateOver = false        -- 量程校正是否完成
    o.deviation12 = 1
    o.deviation13 = 1
    o.deviation23 = 1
    o.autoCheckTimes = 1
    o.isOldCheck = false        -- 是否为低配版仪器标样核查
    o.isUseStart = false
    o.currentModelType = ModelType.TOC    --当前测量参数
    o.isOverProof = false
    o.turboMode = config.measureParam.turboMode
    return o
end

function MeasureFlow:GetRuntime()
    local runtime = 0

    if self.measureType == MeasureType.Blank then
        runtime = setting.runStatus.measureBlank.GetTime()
    elseif self.measureType == MeasureType.Standard then
        runtime = setting.runStatus.measureStandard.GetTime()
    elseif self.measureType == MeasureType.Sample then
        runtime = setting.runStatus.measureSample.GetTime()
    elseif self.measureType == MeasureType.ZeroCheck then
        runtime = setting.runStatus.measureZeroCheck.GetTime()
    elseif self.measureType == MeasureType.QualityHandle then
        runtime = setting.runStatus.measureQualityHandle.GetTime()
    elseif self.measureType == MeasureType.RangeCheck then
        runtime = setting.runStatus.measureRangeCheck.GetTime()
    elseif self.measureType == MeasureType.Addstandard then
        runtime = setting.runStatus.measureAddstandard.GetTime()
    elseif self.measureType == MeasureType.Parallel then
        runtime = setting.runStatus.measureParallel.GetTime()
    elseif self.measureType == MeasureType.ExtAddstandard then
        runtime = setting.runStatus.measureExtAddstandard.GetTime()
    elseif self.measureType == MeasureType.ExtParallel then
        runtime = setting.runStatus.measureExtParallel.GetTime()
    end

    return runtime
end

function MeasureFlow:OnStart()
    local eventStr = "开始" .. self.text
    if config.measureParam.meaType == MeaType.Offline then
        self.meaType = "离线"
        self.measureType = MeasureType.Standard
    else
        -- if lc:IsConnected() and config.measureParam.isUseIOS then
        --     local map = lc:GetISolenoidValve():GetSensorsMap()
        --     --local updateWidgetManager = UpdateWidgetManager.Instance()
        --     --updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "传感器状态0x" .. map)
        --     if (map & (1<<0)) > 0 then
        --         local updateWidgetManager = UpdateWidgetManager.Instance()
        --         updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "IOS流量传感器异常，请检查液路连接是否正常！")
        --         config.scheduler.measure.mode = MeasureMode.Trigger
        --         config.modifyRecord.scheduler(true)
        --         ConfigLists.SaveSchedulerConfig()
        --         error(UserStopException:new())
        --         return
        --     end
        --     if (map & (1<<1)) > 0 then
        --         local updateWidgetManager = UpdateWidgetManager.Instance()
        --         updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "IOS样瓶口门异常，请检查样瓶口门是否关闭！")
        --         config.scheduler.measure.mode = MeasureMode.Trigger
        --         config.modifyRecord.scheduler(true)
        --         ConfigLists.SaveSchedulerConfig()
        --         error(UserStopException:new())
        --         return
        --     end
        -- end
    end
print("OnProcess 0.1")
    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, self.meaType, nil)

    --组合流程需要重新加载时间
    self.measureDateTime = os.time()
    if self.isCrashMeasure == true then
        self.measureDateTime = status.running.lastMeasureTime
    else
        status.running.lastMeasureTime = self.measureDateTime
    end

    status.measure.isUseStart = self.isUseStart
    ConfigLists.SaveMeasureStatus()

    -- 设置测量标志
    if self.measureType == MeasureType.Sample then  --测水样
        status.running.isMeasuring = true
    end

    ConfigLists.SaveMeasureRunning()

    --继电器指示
    Helper.Result.RelayOutOperate(setting.mode.relayOut.measureInstruct, true)

    if self.measureType == MeasureType.Blank then  --测零点校准液
        local runStatus = Helper.Status.SetStatus(setting.runStatus.measureBlank)
        StatusManager.Instance():SetStatus(runStatus)
        self.currentRange = config.measureParam.range[config.measureParam.currentRange + 1] + 1   --测零点校准液
    elseif self.measureType == MeasureType.Standard then  --测量程校准液
        local runStatus = Helper.Status.SetStatus(setting.runStatus.measureStandard)
        StatusManager.Instance():SetStatus(runStatus)
        self.currentRange = config.measureParam.range[config.measureParam.currentRange + 1] + 1 --测量程校准液量程
    elseif self.measureType == MeasureType.Sample then   --测水样
        local runStatus = Helper.Status.SetStatus(setting.runStatus.measureSample)
        StatusManager.Instance():SetStatus(runStatus)
        self.currentRange = config.measureParam.range[config.measureParam.currentRange + 1] + 1  --测水样量程
    elseif self.measureType == MeasureType.ZeroCheck then   --测零点核查液
        local runStatus = Helper.Status.SetStatus(setting.runStatus.measureZeroCheck)
        StatusManager.Instance():SetStatus(runStatus)
        self.currentRange = config.measureParam.range[config.measureParam.currentRange + 1] + 1 --测零点核查量程
    elseif self.measureType == MeasureType.RangeCheck then   --测量程核查液
        if self.isSampleRangeCheck == true then
            local runStatus = Helper.Status.SetStatus(setting.runStatus.currentRange)
            StatusManager.Instance():SetStatus(runStatus)
            self.currentRange = config.measureParam.range[config.measureParam.currentRange + 1] + 1  --测水样量程
        else
            local runStatus = Helper.Status.SetStatus(setting.runStatus.measureRangeCheck)
            StatusManager.Instance():SetStatus(runStatus)
            self.currentRange = config.measureParam.range[config.measureParam.rangeCheckRangeIndex + 1] + 1 --测量程核查量程
        end
    end

    if nil ~= setting.common.skipFlow and true == setting.common.skipFlow then

    else
        -- -- 初始化下位机
        -- dc:GetIDeviceStatus():Initialize()
        -- lc:GetIDeviceStatus():Initialize()
        -- --设置风扇常开
        -- op:SetDCNormalOpen(setting.liquidType.map.fan)
        -- if config.measureParam.isUseUVLamp then
        --     --开LED
        --     dc:GetIOpticalAcquire():TurnOnLED()
        -- else
        --     --关LED
        --     dc:GetIOpticalAcquire():TurnOffLED()
        -- end

    end

    -- rc:ClearBuf()--清buf,防止数组刷新
end

function MeasureFlow:OnProcess()
    local loopcnt = 3
    self.isRetryMeasure = true
    self.isRetryOnStart = false

    self.isUserStop = false
    self.isFinish = false
    print("OnProcess 1")

    --重测循环
    while self.isRetryMeasure == true and loopcnt > 0 do
        loopcnt = loopcnt - 1
        self.isRetryMeasure = false

        local err,result = pcall(			-- 捕获异常
            function()
                if self.isRetryOnStart == true then
                    self.isRetryOnStart = false
                    self:OnStart()
                end
print("OnProcess 2")
                --测量流程表复位、参数配置
                if Measurer.flow then
                    Measurer:Reset()
                end
                Measurer.flow = self
                if self.measureType == MeasureType.Sample or self.measureType == MeasureType.Addstandard or self.measureType == MeasureType.Parallel then
                    Measurer.measureType = MeasureType.Sample   --(内部)加标加标/平行的第一次测量类型为水样
                else
                    Measurer.measureType = self.measureType
                end
                Measurer.currentRange = self.currentRange

                for k,v in pairs(setting.measure.range[self.currentRange]) do
                    Measurer.addParam [k] = v
                end

                log:debug("当前量程 = " .. self.currentRange)
                --Turbo模式下必须加酸剂、氧化剂
                if config.measureParam.turboMode then
                    config.measureParam.reagent1Vol = 2
                    config.measureParam.reagent2Vol = 2
                    ConfigLists.SaveMeasureParamConfig()
                end

                --根据测量类型调整加液参数
                if self.measureType == MeasureType.Blank then
                    Measurer.addParam.blankVolume = Measurer.addParam.blankVolume + Measurer.addParam.sampleVolume
                    Measurer.addParam.sampleVolume = 0
                    Measurer.addParam.rinseBlankVolume = Measurer.addParam.rinseBlankVolume + Measurer.addParam.rinseSampleVolume
                    Measurer.addParam.rinseSampleVolume = 0
                elseif self.measureType == MeasureType.Standard then
                    Measurer.addParam.standardVolume = Measurer.addParam.standardVolume + Measurer.addParam.sampleVolume
                    Measurer.addParam.sampleVolume = 0
                    Measurer.addParam.rinseStandardVolume = Measurer.addParam.rinseStandardVolume + Measurer.addParam.rinseSampleVolume
                    Measurer.addParam.rinseSampleVolume = 0
                elseif self.measureType == MeasureType.ZeroCheck then
                    Measurer.addParam.zeroCheckVolume = Measurer.addParam.zeroCheckVolume + Measurer.addParam.sampleVolume
                    Measurer.addParam.sampleVolume = 0
                    Measurer.addParam.rinseZeroCheckVolume = Measurer.addParam.rinseZeroCheckVolume + Measurer.addParam.rinseSampleVolume
                    Measurer.addParam.rinseSampleVolume = 0
                elseif self.measureType == MeasureType.RangeCheck then --非低配版标样核查使用量程核查通道
                    if self.isSampleRangeCheck == true then
                        --按水样加,走水样管
                    else
                        Measurer.addParam.rangeCheckVolume = Measurer.addParam.rangeCheckVolume + Measurer.addParam.sampleVolume
                        Measurer.addParam.sampleVolume = 0
                        Measurer.addParam.rinseRangeCheckVolume = Measurer.addParam.rinseRangeCheckVolume + Measurer.addParam.rinseSampleVolume
                        Measurer.addParam.rinseSampleVolume = 0
                    end
                end

                self.diluteFactor = Measurer.addParam.diluteFactor  --稀释系数

                -- 采水样
                if self.measureType == MeasureType.Sample then
                    self:CollectSample()
                end
print("OnProcess 3")
                -- CCEP模式下检测到崩溃重启
                if self.isCrashMeasure == true and config.system.CCEPMode == true then
                    self:RetryMeasureCheck()
                else
                    --测量流程执行
                    setting.measureResult.continousModeParam.lastAccuratepeak = 0
                    setting.measureResult.continousModeParam.lastStartIndex = 0
                    setting.measureResult.continousModeParam.currentMeasureCnt = 0
                    setting.measureResult.continousModeParam.isfinishContinousMeasure = false --连续测量标志位
                    setting.measureResult.isStartAccurateMeasure = false    --精确测量标志位
                    setting.measureResult.isFinishAccurateMeasure = false   --精确测量标志位
                    setting.measureResult.immediatelyResultHandle = false   --精确结果输出标志位

                    if config.scheduler.measure.mode == MeasureMode.Continous and (self.measureType == MeasureType.Sample or self.measureType == MeasureType.Standard) then
                        while config.scheduler.measure.mode == MeasureMode.Continous and (self.measureType == MeasureType.Sample or self.measureType == MeasureType.Standard) do
                            local runStatus = Helper.Status.SetStatus(setting.runStatus.measureSample)
                            if self.measureType == MeasureType.Standard then
                                runStatus = Helper.Status.SetStatus(setting.runStatus.measureStandard)
                            end
                            StatusManager.Instance():SetStatus(runStatus)

                            Measurer:Measure()
                            if  self.isUserStop == true then
                                break
                            end

                            --量程切换
                            if self.currentRange ~= config.measureParam.range[config.measureParam.currentRange + 1] + 1 and setting.measureResult.continousModeParam.isfinishContinousMeasure == true then
                                log:debug("change range")
                                Measurer:ContinousMeasureSafetyStop()
                                self.isFinish = true
                                break
                            end

                            if config.scheduler.calibrate.mode ~= MeasureMode.Trigger and setting.measureResult.continousModeParam.isfinishContinousMeasure == true then
                                --校准排期
                                local currentTime = os.time()
                                local ret, actionTime = setting.measureScheduler[1].getNextTime()
                                if currentTime - actionTime >= 0 then
                                    log:debug("calibrate")
                                    Measurer:ContinousMeasureSafetyStop()
                                    self.isFinish = true
                                    break
                                end
                            end

                            if config.scheduler.standard.enable == true and setting.measureResult.continousModeParam.isfinishContinousMeasure == true then
                                --量程校准排期
                                local currentTime = os.time()
                                local ret, actionTime = setting.measureScheduler[5].getNextTime()
                                if currentTime - actionTime >= 0 then
                                    log:debug("standard")
                                    Measurer:ContinousMeasureSafetyStop()
                                    self.isFinish = true
                                    break
                                end
                            end

                            if config.scheduler.blankCheck.enable == true and setting.measureResult.continousModeParam.isfinishContinousMeasure == true then
                                --零点校准排期
                                local currentTime = os.time()
                                local ret, actionTime = setting.measureScheduler[7].getNextTime()
                                if currentTime - actionTime >= 0 then
                                    log:debug("blankCheck")
                                    Measurer:ContinousMeasureSafetyStop()
                                    self.isFinish = true
                                    break
                                end
                            end

                            if config.scheduler.zeroCheck.mode ~= MeasureMode.Trigger and setting.measureResult.continousModeParam.isfinishContinousMeasure == true then
                                --零点核查排期
                                local currentTime = os.time()
                                local ret, actionTime = setting.measureScheduler[3].getNextTime()
                                if currentTime - actionTime >= 0 then
                                    log:debug("zeroCheck")
                                    Measurer:ContinousMeasureSafetyStop()
                                    self.isFinish = true
                                    break
                                end
                            end

                            if config.scheduler.rangeCheck.mode ~= MeasureMode.Trigger and setting.measureResult.continousModeParam.isfinishContinousMeasure == true then
                                --量程核查排期
                                local currentTime = os.time()
                                local ret, actionTime = setting.measureScheduler[2].getNextTime()
                                if currentTime - actionTime >= 0 then
                                    log:debug("rangeCheck")
                                    Measurer:ContinousMeasureSafetyStop()
                                    self.isFinish = true
                                    break
                                end
                            end
                        end
                    else
                        Measurer:Measure()
                        if setting.measureResult.continousModeParam.isfinishContinousMeasure == true then
                            --非连续模式改为连续测量时安全出数
                            Measurer:ContinousMeasureSafetyStop()
                            self.isFinish = true
                        end
                    end
                end
            end
        )	-- 捕获异常结束

        if not err then      -- 出现异常
            if type(result) == "userdata" then
                if result:GetType() == "ExpectEventTimeoutException" then          --期望事件等待超时异常。
                    ExceptionHandler.MakeAlarm(result)
                    self:RetryMeasureCheck()
                elseif result:GetType() == "CommandTimeoutException" then          --命令应答超时异常
                    ExceptionHandler.MakeAlarm(result)
                    self:RetryMeasureCheck()
                else
                    log:warn("MeasureFlow:OnProcess =>" .. result:What())
                end
            elseif type(result) == "table" then
                if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
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
                elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
                    self.isUserStop = true 
                    error(result)
                elseif getmetatable(result) == AcquirerADFailedException then 	    --光学采集中途出现故障，未能完成异常。
                    ExceptionHandler.MakeAlarm(result)
                    self:RetryMeasureCheck()                    
                elseif getmetatable(result) == PumpFailedException then 			--泵操作中途出现故障，未能完成异常。
                    ExceptionHandler.MakeAlarm(result)
                    self:RetryMeasureCheck()
                elseif getmetatable(result) == MeterFailedException then 			--定量中途出现故障，未能完成异常。
                    ExceptionHandler.MakeAlarm(result)
                    self:RetryMeasureCheck()
                elseif getmetatable(result) == MeterOverflowException then 			--定量溢出异常。
                    ExceptionHandler.MakeAlarm(result)
                    self:RetryMeasureCheck()
                elseif getmetatable(result) == MeterUnfinishedException then 		--定量目标未达成异常。
                    ExceptionHandler.MakeAlarm(result)
                    self:RetryMeasureCheck()
                elseif getmetatable(result)== MeterAirBubbleException then			--定量气泡异常。
                    ExceptionHandler.MakeAlarm(result)
                    self:RetryMeasureCheck()
                elseif getmetatable(result) == MeterExpectTimeoutException then 	--定量超时异常。
                    ExceptionHandler.MakeAlarm(result)
                    self:RetryMeasureCheck()
                elseif getmetatable(result) == ThermostatFailedException then 		--恒温中途出现故障，未能完成异常。
                    ExceptionHandler.MakeAlarm(result)
                    self:RetryMeasureCheck()
                elseif getmetatable(result) == ThermostatTimeoutException then 		--恒温超时，指定时间内仍未达到目标温度异常
                    ExceptionHandler.MakeAlarm(result)
                    self:RetryMeasureCheck()
                elseif getmetatable(result) == DrainFromDigestionException then 	 	--排消解液异常
                    if config.system.CCEPMode == true then
                        ExceptionHandler.MakeAlarm(result)
                        self:RetryMeasureCheck()
                    else  --严重故障直接终止
                        error(result)
                    end
                elseif getmetatable(result) == AddLiquidToDigestionException then 	 	--消解室加液异常
                    if config.system.CCEPMode == true then
                        ExceptionHandler.MakeAlarm(result)
                        self:RetryMeasureCheck()
                    else  --严重故障直接终止
                        error(result)
                    end
                elseif getmetatable(result) == MeasureDataException then 	 	-- 测量数据异常
                    ExceptionHandler.MakeAlarm(result)
                    self:RetryMeasureCheck()
                elseif getmetatable(result) == MeasureLedException then 	 	-- 测量信号异常
                    ExceptionHandler.MakeAlarm(result)
                    self:RetryMeasureCheck()
                elseif getmetatable(result) == MeasureRangeWrongException then 	 	--量程错误异常
                    self:RetryMeasureCheck(true)
                else
                    log:warn("MeasureFlow:OnProcess =>" .. result:What())								--其他定义类型异常
                end
            elseif type(result) == "string" then
                log:warn("MeasureFlow:OnProcess =>" .. result)	--C++、Lua系统异常
            end
        end
    end-----重测循环
end


function MeasureFlow:OnStop()
    -- 设置测量标志
    if status.running.isCorrectStopFlow ~= nil and status.running.isCorrectStopFlow == true then
        status.running.isMeasuring = false
        ConfigLists.SaveMeasureRunning()
    end

    -- 隐藏预估值
    if config.system.displayProformaResult == true then
        UpdateWidgetManager.Instance():Update(UpdateEvent.ShowNewProformaData, "")
    end

    local eventStr = ""

    if not self.isFinish then
        if self.measureType == MeasureType.Sample then
            status.measure.schedule.autoMeasure.dateTime = self.measureDateTime
        elseif self.measureType == MeasureType.RangeCheck then
            status.measure.schedule.rangeCheck.dateTime = self.measureDateTime
        elseif self.measureType == MeasureType.Standard then
            status.measure.schedule.autoCheck.dateTime = self.measureDateTime
        elseif self.measureType == MeasureType.Blank then
            status.measure.schedule.autoBlankCheck.dateTime = self.measureDateTime
        elseif self.measureType == MeasureType.Addstandard or self.measureType == MeasureType.ExtAddstandard then
            status.measure.schedule.autoAddstandard.dateTime = self.measureDateTime
        elseif self.measureType == MeasureType.Parallel or self.measureType == MeasureType.ExtParallel then
            status.measure.schedule.autoParallel.dateTime = self.measureDateTime
        elseif self.measureType == MeasureType.ZeroCheck then
            status.measure.schedule.zeroCheck.dateTime = self.measureDateTime
        end

        if self.isUserStop then
            self.result = "用户终止"
            log:info("用户终止")
        else
            status.measure.newResult.measure.resultMark = "D"  --故障数据标记
            status.measure.newResult.measure.resultInfo = "D"  --故障数据标识
            status.measure.report.complexResult.resultInfo = "D"  --故障数据标记
            if self.measureType == MeasureType.Sample then
                status.measure.report.measure.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            elseif self.measureType == MeasureType.Standard then
                status.measure.report.check.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            elseif self.measureType == MeasureType.Blank then
                status.measure.report.blankCheck.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            elseif self.measureType == MeasureType.Addstandard or self.measureType == MeasureType.ExtAddstandard then
                status.measure.report.addstandard.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            elseif self.measureType == MeasureType.Parallel or self.measureType == MeasureType.ExtParallel then
                status.measure.report.parallel.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            elseif self.measureType == MeasureType.ZeroCheck then
                status.measure.report.zeroCheck.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            elseif self.measureType == MeasureType.RangeCheck then
                status.measure.report.rangeCheck.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            elseif self.measureType == MeasureType.QualityHandle then
                status.measure.report.qualityHandle.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            end

            self.result = "故障终止"
            log:warn("故障终止")

            local isSave = false
            if self.measureType == MeasureType.Sample and config.scheduler.measure.mode == MeasureMode.Continous then
                config.scheduler.measure.mode = MeasureMode.Trigger --连续测量变触发
                isSave = true
            elseif self.measureType == MeasureType.ZeroCheck and config.scheduler.zeroCheck.mode == MeasureMode.Continous then
                config.scheduler.zeroCheck.mode = MeasureMode.Trigger --连续测量变触发
                isSave = true
            elseif self.measureType == MeasureType.RangeCheck and config.scheduler.rangeCheck.mode == MeasureMode.Continous then
                config.scheduler.rangeCheck.mode = MeasureMode.Trigger --连续测量变触发
                isSave = true
            end

            if isSave == true then
                config.modifyRecord.scheduler(true)
                ConfigLists.SaveSchedulerConfig()

                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeMeaModeOnHome, "DeviceFaultException")
            end
        end
        ConfigLists.SaveMeasureStatus()
        eventStr = self.text .. "-" .. self.result
    else
        status.measure.isDeviceFault = false
        ConfigLists.SaveMeasureStatus()

        self.result = "测量完成"
        log:info("测量完成")
        log:info("测量流程总时间 = ".. os.time() - self.measureDateTime)
        eventStr = self.text .. "完成"
    end

    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)

    --保存试剂余量表
    ReagentRemainManager.SaveRemainStatus()

    --低配版标样核查继电器关闭
    if self.isOldCheck == true then
        Helper.Result.RelayOutOperate(setting.mode.relayOut.checkInstruct, false)
    end

    --继电器指示
    Helper.Result.RelayOutOperate(setting.mode.relayOut.measureInstruct, false)

    --关闭采水继电器
    if config.interconnection.collectSampleMode ~= setting.mode.collectSample.trigger then
        local waterCollector = WaterCollector.Instance()
        if not string.find(config.info.instrument["type"], "PT63P") then
            waterCollector:TurnOff()
        end
        Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, false)
    end

    --开始更新基线状态
    status.measure.isCheckBaseLine = true
    status.measure.isUseStart = false
    ConfigLists.SaveMeasureStatus()

    if nil ~= setting.common.skipFlow and true == setting.common.skipFlow then

    else
        -- -- 初始化下位机
        -- dc:GetIDeviceStatus():Initialize()
        -- lc:GetIDeviceStatus():Initialize()
        -- --停止水样泵
        -- op:StopSamplePump()
        -- --设置风扇常开
        -- op:SetDCNormalOpen(setting.liquidType.map.fan)
        -- --关紫外灯
        -- dc:GetIOpticalAcquire():TurnOffLED()
        -- --设置去离子水泵和阀为打开
        -- op:SetLCStopStatus()
    end
end

--[[
 * @brief 测量结果处理
 * @param[in] result 测量过程得到的各参数电流值和对应波形文件索引
--]]
function MeasureFlow:ResultHandle(result)

    local measureAD = MeasureAD:new()
    local peakTC = result.peakTC
    local peakIC = result.peakIC
    local ICConductivityCellTemp = result.ICConductivityCellTemp
    local TCConductivityCellTemp = result.TCConductivityCellTemp
    local peakTemperatureTC = result.peakTemperatureTC
    local peakTemperatureIC = result.peakTemperatureIC

    measureAD.initReference = result.initReferenceAD
    measureAD.initMeasure = result.initMeasureAD
    measureAD.finalReference = result.finalReferenceAD
    measureAD.finalMeasure = result.finalMeasureAD

    self.measureDateTime = result.measureDate

    self.peakTC = result.peakTC
    self.peakIC = result.peakIC

    -- log:debug("TC原始峰高 = " .. peakTC .. ", IC原始峰高 = " ..  peakIC)
    -- --峰高修正

    -- log:debug("TC补偿峰高 = " .. self.peakTC .. ", IC补偿峰高 = " ..  self.peakIC)

   --浓度计算  当前result.peak 应避免未空值
    if nil ~= setting.common.skipFlow and true == setting.common.skipFlow then
        self.consistency = setting.common.minRandomData + math.random() * (setting.common.maxRandomData - setting.common.minRandomData)
        self.consistencyTC = 0.1 + math.random() * (setting.common.maxRandomData - setting.common.minRandomData)
        self.consistencyIC = 0.2 + math.random() * (setting.common.maxRandomData - setting.common.minRandomData)
        self.measureDateTime = os.time()
    else
        self.consistencyTC = self:CalculateConsistency(self.peakTC, ModelType.TC)
        self.consistencyIC = self:CalculateConsistency(self.peakIC, ModelType.IC)
        self.consistency = self.consistencyTC - self.consistencyIC
    end
    log:debug("TC补偿峰高 = " .. self.peakTC .. ", IC补偿峰高 = " ..  self.peakIC)
    log:info("TOC浓度 = " .. self.consistency .. ", TC浓度 = " .. self.consistencyTC .. "IC浓度 = " .. self.consistencyIC)

    --管理员 浓度修正
    local tempConsistency = self.consistency
    self.consistency = tempConsistency * config.measureParam.reviseFactor
    log:debug("结果浓度修正 修正系数 = " .. config.measureParam.reviseFactor.."，原浓度 = "..tempConsistency.."，修正浓度 = " .. self.consistency)

    --超级管理员平移修正
    self.consistency = self.consistency + config.measureParam.shiftFactor
    local shiftAbs = config.measureParam.shiftFactor*config.measureParam.curveParam[self.currentRange].curveK/(config.measureParam.reviseFactor*config.measureParam.reviseParameter[self.currentRange])
    self.peakTC = self.peakTC + shiftAbs
    log:debug("Shift Revise shiftFactor = "..config.measureParam.shiftFactor.."，newC = " .. self.consistency.." shiftAbs = "..shiftAbs..", newA = "..self.peakTC)

    self.consistency = self:ConsistencyOffset(self.consistency)
    --self.absorbance = self:ReviseAbs(self.consistency)
    --self.peakTC = self:ReviseAbs(self.consistency)      --峰面积反算 ，后面确定标线后再加上


    if config.measureParam.negativeRevise == true then
        --浓度小值显示校正
        self.consistency = self:ConsistencyRevise(self.consistency)
    end

    --if self.consistency > config.system.rangeViewMap[self.currentRange].view*150/100 and true == self:CheckCurve() then
    --    self.consistency = config.system.rangeViewMap[self.currentRange].view*150/100 +
    --            math.random() * setting.measureResult.quantifyLowLimit
    --    self.peakTC = self:ReviseAbs(self.consistency)      --峰面积反算 ，后面确定标线后再加上
    --end

    --自动量程切换检查
    if Measurer.measureType == MeasureType.Sample and config.measureParam.autoChangeRange == true then
        self:RangeCheck(self.consistency, self.currentRange)
    end

    --结果数据标识
    local resultMark = ResultMark.N
    self.reportMode = config.interconnection.reportMode     -- 出结果时重拿一次上报模式

    if self.reportMode == ReportMode.OnLine then
        --在线情况下 水样才去监测超上限与超量程的阈值，且核查才会标记为K
        if self.measureType == MeasureType.Sample then
            if self.consistency > config.interconnection.meaUpLimit and 1 == config.interconnection.overProofResultMark then
                resultMark = ResultMark.T
                self.isOverProof = true
            elseif self.consistency > config.system.rangeViewMap[self.currentRange].view then
                resultMark = ResultMark.E
            end
        elseif self.measureType == MeasureType.RangeCheck or self.measureType == MeasureType.ZeroCheck then
            resultMark = ResultMark.C
        elseif self.measureType == MeasureType.QualityHandle then
            resultMark = ResultMark.K
        end

        if self.measureType == MeasureType.RangeCheck or self.measureType == MeasureType.ZeroCheck then
            if 1 == config.interconnection.rangeCheckResultMark then
                resultMark = ResultMark.K
            end
        end
    elseif self.reportMode == ReportMode.OffLine then
        resultMark = ResultMark.B
    elseif self.reportMode == ReportMode.Maintain then
        resultMark = ResultMark.M
    elseif self.reportMode == ReportMode.Fault then
        resultMark = ResultMark.D
    elseif self.reportMode == ReportMode.Calibrate then
        resultMark = ResultMark.C
    elseif self.reportMode == ReportMode.Debugging then
        resultMark = ResultMark.A
    end

    --四川协议下零点校准液和量程校准液直接标记为 维护M 数据
    if modbusStr == "SC" then
        if self.measureType == MeasureType.Blank or self.measureType == MeasureType.Standard then
            resultMark = ResultMark.M
            self.reportMode = ReportMode.Maintain
        end
    end


    --整点测量时，测量时间整点校正，手动启动不校正
    if self.adjustTime == true then  --排期设置
        if (self.measureType == MeasureType.Sample and config.scheduler.measure.mode == MeasureMode.Timed) or
                (self.measureType == MeasureType.ZeroCheck and config.scheduler.zeroCheck.mode == MeasureMode.Timed) or
                (self.measureType == MeasureType.RangeCheck and config.scheduler.rangeCheck.mode == MeasureMode.Timed) then

            self.measureDateTime = self:AdjustMeasureDateTime(self.measureDateTime)
        end
    end
    --print("Push result data to file.")
    local resultManager = ResultManager.Instance()
    local recordData = RecordData.new(resultManager:GetMeasureRecordDataSize(setting.resultFileInfo.measureRecordFile[1].name))
    recordData:PushInt(self.measureDateTime) -- 时间
    recordData:PushFloat(self.consistency)   -- TOC浓度
    recordData:PushFloat(self.consistencyTC) -- TC浓度
    recordData:PushFloat(self.consistencyIC) -- IC浓度
    recordData:PushFloat(self.peakTC)        -- TC峰值
    recordData:PushFloat(self.peakIC)        -- IC峰值
    recordData:PushByte(Measurer.measureType)           -- 类型(当前测量过程)
    recordData:PushFloat(result.initCellTempTC)         -- 初始TC电导池温度
    recordData:PushFloat(result.initCellTempIC)         -- 初始IC电导池温度
    recordData:PushFloat(result.finalCellTempTC)        -- 反应TC电导池温度
    recordData:PushFloat(result.finalCellTempIC)        -- 反应IC电导池温度
    recordData:PushFloat(result.initEnvironmentTemp)    -- 初始环境温度
    recordData:PushFloat(result.finalEnvironmentTemp)   -- 反应环境温度
    recordData:PushInt(os.time()-self.measureDateTime) -- 测量时长
    recordData:PushFloat(config.system.rangeViewMap[self.currentRange].view)   -- 当前使用量程
    recordData:PushInt(config.measureParam.meaType)     -- 测量类型 0-在线 1-离线
    recordData:PushBool(config.measureParam.turboMode)  -- Turbo模式
    recordData:PushBool(config.measureParam.ICRMode)    -- ICR模式
    recordData:PushBool(config.measureParam.TOCMode)    -- TOC测量
    recordData:PushBool(config.measureParam.ECMode)     -- 电导率测量
    recordData:PushBool(config.measureParam.autoReagent)-- 自动加试剂
    recordData:PushFloat(config.measureParam.reagent1Vol)    -- 酸剂流量
    recordData:PushFloat(config.measureParam.reagent2Vol)    -- 氧化剂流量
    recordData:PushInt(config.measureParam.normalRefreshTime)-- 冲洗时间
    recordData:PushInt(config.measureParam.measureTimes)     -- 测量次数(离线)
    recordData:PushInt(config.measureParam.rejectTimes)      -- 舍弃次数(离线)

    -- 隐藏预估值
    Helper.Result.OnMeasureProformaResultAdded(self.measureDateTime, self.consistency, self.absorbance, measureAD)
    if config.system.displayProformaResult == true then
        UpdateWidgetManager.Instance():Update(UpdateEvent.ShowNewProformaData, "")
    end

    local flowManager = FlowManager.Instance()
    if true == flowManager:IsReagentAuthorize() then
        Helper.Result.OnMeasureResultAdded(Measurer.measureType, self.measureDateTime, self.consistency, self.consistencyTC, self.consistencyIC, self.reportMode, self.peakTC ,self.isUseStart,false,resultMark,modbusStr, self.currentModelType)
        resultManager:AddMeasureRecord(setting.resultFileInfo.measureRecordFile[1].name, recordData, true)

        if config.system.printer.enable == true and config.system.printer.autoPrint == true and setting.ui.measureDataPrint.measure.printer ~= nil then
            UpdateWidgetManager.Instance():Update(UpdateEvent.PrintNewMeasureData, "MeasureFlow")
        end
    else
        local alarm = Helper.MakeAlarm(setting.alarm.reagentAuthorizationError, "")
        AlarmManager.Instance():AddAlarm(alarm)

        if self.measureType == MeasureType.Sample then
            status.measure.schedule.autoMeasure.dateTime = self.measureDateTime
        elseif self.measureType == MeasureType.Standard then
            status.measure.schedule.autoCheck.dateTime = self.measureDateTime
        elseif self.measureType == MeasureType.Blank then
            status.measure.schedule.autoBlankCheck.dateTime = self.measureDateTime
        elseif self.measureType == MeasureType.Addstandard or self.measureType == MeasureType.ExtAddstandard then
            status.measure.schedule.autoAddstandard.dateTime = self.measureDateTime
        elseif self.measureType == MeasureType.Parallel or self.measureType == MeasureType.ExtParallel then
            status.measure.schedule.autoParallel.dateTime = self.measureDateTime
        elseif self.measureType == MeasureType.ZeroCheck then
            status.measure.schedule.zeroCheck.dateTime = self.measureDateTime
        elseif self.measureType  == MeasureType.RangeCheck then
            status.measure.schedule.rangeCheck.dateTime = self.measureDateTime
        end
    end

    --测量流程类型和测量结果类型一致才认定完成
    if self.measureType == Measurer.measureType or self.isCheck == true then
        self.isFinish = true
    end

    if config.system.CCEPMode == true then
        CCEPSafeData = 0   --完成测量，安全标志清0
    end

    ConfigLists.SaveMeasureStatus()

    self.isCrashMeasure = false  --清除崩溃标记
    -- 设置测量结束标志
    status.running.isMeasuring = false
    ConfigLists.SaveMeasureRunning()
    --测量结果
    log:debug("测量结果：".."iRef = " .. measureAD.initReference .. "，iMea = " .. measureAD.initMeasure ..
    "，fRef = " .. measureAD.finalReference .."，fMea =" .. measureAD.finalMeasure .. "，A = " .. self.absorbance .. "，C = " .. self.consistency)

    if true == self.isChangeRangeAtTheEnd then
        config.measureParam.currentRange = self.targetRange 		--更新量程配置文件
        config.modifyRecord.measureParam(true)
        ConfigLists.SaveMeasureParamConfig()
    end
end

--[[
 * @brief 异常吸光度修正(吸光度超正常范围上限)
 * @param[in] absorbance 吸光度
--]]
function MeasureFlow:AbnormalAbsorbanceRevise(absorbance)
    local retA = absorbance
    if absorbance > setting.measureResult.absLimit then
        local random = 0.1*(math.random() - 0.5)*setting.measureResult.absLimit
        retA = setting.measureResult.absLimit + random

        log:warn("Abnormal Absorbance Revise ==> Abs0 = "..absorbance..",Abs = "..retA)
    end
    return retA
end

--[[
 * @brief 峰高系数补偿
 * @param[in] peakHigh 峰高
 * @param[in] temp 温度
  * @param[in] type IC or TC
--]]
function MeasureFlow:PeakHighReviseWithTemperature(peakHigh, temp)
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
 * @brief 低浓度峰高系数补偿
 * @param[in] peakHigh 峰高
 * @param[in] temp 温度
--]]
function MeasureFlow:SearchPeakWithCalculate(result, mtype)
    local consistency = 0
    local constant = 1
    local temp = 0
    local peak = 0
    local str = ""
    local strPeak = ""
    local peakTable ={}
    local peakTableCal ={}
    local peakTableTemp = {}
    local reviser = 1
    local EC = 0
    local T25EC = 0
    local startIndex = result.startIndex
    local endIndex = result.endIndex
    local max = 0
    local gainValue = 1
    local debug = config.system.debugMode
    if setting.measureResult.isHighRangeTC and mtype == ModelType.TC then
        gainValue = 9.29
        log:debug("TC高量程模式")
    end
    if setting.measureResult.isHighRangeIC and mtype == ModelType.IC then
        gainValue = 9.29
        log:debug("IC高量程模式")
    end

    if mtype == ModelType.TC then
        constant = config.measureParam.TCConstant
        for i = startIndex,endIndex do
            peak = rc:GetScanData(i) * gainValue
            temp = rc:GetMeaTemp(i)
            reviser = -3.16345*10^(-8) * temp^3 + 1.25933*10^(-5) * temp^2 + 5.26393*10^(-4)* temp + 0.03193
            EC = peak * constant * 2
            T25EC = 1 + reviser * (temp - 25)
            table.insert(peakTable, (EC/T25EC))
        end

        for k,v in pairs(peakTable) do
            local average = v
            average = peakTable[k] -  status.measure.blankECTableTC[k] + 0.055
            table.insert(peakTableTemp, average)
            if k>5 then
                average = (average + peakTableTemp[k-1] + peakTableTemp[k-2] + peakTableTemp[k-3] + peakTableTemp[k-4] )/5
            end

            if max < average and k > 30 and k < 80 then
                max = average
            end
            table.insert(peakTableCal, average)
            if debug then
                log:debug("TC补偿后电导率[" .. k .. "] = " .. tonumber(v) .. ", 平均处理后电导率 = " .. tonumber(peakTableCal[k]))
            end
        end
    else
        constant = config.measureParam.ICConstant
        for i = startIndex,endIndex do
            peak = rc:GetScanDataRef(i) * gainValue
            temp = rc:GetRefTemp(i)
            reviser = -3.16345*10^(-8) * temp^3 + 1.25933*10^(-5) * temp^2 + 5.26393*10^(-4)* temp + 0.03193
            EC = peak * constant * 2
            T25EC = 1 + reviser * (temp - 25)
            table.insert(peakTable, (EC/T25EC))
        end

        for k,v in pairs(peakTable) do
            local average = v
            average = peakTable[k] -  status.measure.blankECTableIC[k] + 0.055
            table.insert(peakTableTemp, average)
            if k>5 then
                average = (average + peakTableTemp[k-1] + peakTableTemp[k-2] + peakTableTemp[k-3] + peakTableTemp[k-4] )/5
            end

            if max < average and k > 30 and k < 80 then
                max = average
            end
            table.insert(peakTableCal, average)
            if debug then
                log:debug("IC补偿后电导率[" .. k .. "] = " .. tonumber(v) .. ", 平均处理后电导率 = " .. tonumber(peakTableCal[k]))
            end
        end
    end

    log:debug("电导率峰值 " .. max)

    consistency = self:CalculateConsistency(max, mtype)
    if consistency < 0.0022 then
        consistency = (max - 0.055) * 0.22
        log:debug("低浓度计算结果[小于2.2ppb]: " .. consistency)
    end

    return consistency, max
end

--[[
 * @brief 浓度计算
 * @param[in] absorbance 吸光度

--]]
function MeasureFlow:CalculateConsistency(area, type)
    local consistency = 0
    --local peak = 0
    local curveK = config.measureParam.curveK
    local curveB = config.measureParam.curveB

    if self.turboMode then
        curveK = config.measureParam.curveKTurbo
        curveB = config.measureParam.curveBTurbo
    end

    if math.abs(curveK - 0)<0.00001 then
        log:debug("校准数值异常")
        return 0
    end
    log:debug("计算斜率 K = " .. curveK .. ", B = " .. curveB)

    if type ~= nil and type == ModelType.TC then
        consistency = 10^(curveK * math.log(area, 10) + curveB)
    else
        consistency = 10^(curveK * math.log(area, 10) + curveB)
    end

    if consistency < 0.0022 then
        consistency = (area - 0.055) * 0.22
        log:debug("低浓度计算结果[小于2.2ppb]: " .. consistency)
    end

    return consistency
end

--[[
 * @brief 吸光度反算
 * @param[in] consistency 浓度

--]]
function MeasureFlow:ReviseAbs(consistency)
    local absorbance = 0
    local curveK = config.measureParam.curveParam[self.currentRange].curveK
    local curveB = config.measureParam.curveParam[self.currentRange].curveB

    if math.abs(curveK - 0)<0.00001 then
        log:debug("校准数值异常")
        return 0
    end

    --consistency = consistency/diluteFactor
    absorbance = (consistency*curveK) + curveB


    return absorbance
end

--[[
 * @brief 检查标线是否为定标
--]]
function MeasureFlow:CheckCurve()
    local ret = true
    local timeStr = config.measureParam.curveParam[self.currentRange].timeStr

    if timeStr == "--" then
        ret = false
    end

    return ret
end

--[[
 * @brief 浓度小值修正
 * @param[in] consistency 浓度度
--]]
function MeasureFlow:ConsistencyRevise(consistency,infoPrintf)
    local ret = 0
    local pLowLimit = setting.measureResult.measureLowLimit
    local quantifyLowLimit = setting.measureResult.quantifyLowLimit
    local nLowLimit = -pLowLimit

    quantifyLowLimit = quantifyLowLimit*0.4
    if consistency < quantifyLowLimit then
        if infoPrintf == false then
            log:debug("低于检出限")
        else
            log:info("低于检出限")
        end
    end

    if consistency < nLowLimit then
        local random = 2 * (math.random() - 0.5 ) * setting.measureResult.negativeReviseWaveRange
        ret = setting.measureResult.negativeReviseBaseValue + random
    elseif nLowLimit <= consistency and consistency < -0.001 then
        ret = math.abs(consistency)
    elseif -0.001 <= consistency and consistency <= 0.001 then
        ret = setting.measureResult.zeroReviseValue
    else
        ret = consistency
    end

    return ret
end

--[[
 * @brief 浓度偏移
 * @param[in] consistency 浓度度
--]]
function MeasureFlow:ConsistencyOffset(consistency)
    local ret = consistency
    local quantifyLowLimit = setting.measureResult.quantifyLowLimit * self.diluteFactor

    if consistency < quantifyLowLimit then
        log:debug("结果偏移 原数值 = "..consistency.." 修正后数值 = "..(consistency + config.measureParam.measureDataOffsetValve))
        ret = consistency + config.measureParam.measureDataOffsetValve
    end

    return ret
end

--[[
 * @brief 量程范围检查
 * @param[in] result 结果浓度
 * @param[in] currange 当前量程
--]]
function MeasureFlow:RangeCheck(result, currange)
    if result >= setting.measure.range[currange].rangeWindowMin and
         result <= setting.measure.range[currange].rangeWindowMax then
        --log:debug("水样测量结果在量程内")
    elseif currange == 1 and result < setting.measure.range[currange].rangeWindowMin then
        log:info("水样超量程下限")
    elseif currange == setting.measure.range.rangeNum and result > setting.measure.range[currange].viewRange then
		log:info("水样超量程上限")
    else
        log:debug("结果"..self.consistency.."不在量程"..self.currentRange.."内")
        self.targetRange = self:RangeChange(self.consistency, self.currentRange)  --改变当前量程
        if config.system.rangeViewMap[config.measureParam.range[self.targetRange + 1] + 1].view ~= nil then
            log:info("量程自动切换至 = " .. config.system.rangeViewMap[config.measureParam.range[self.targetRange + 1] + 1].view)
        end

        if config.measureParam.activeRangeMode == setting.mode.activeRange.now and self.targetRange ~= config.measureParam.currentRange then
            log:info("量程切换：立即生效")
            if self.faultTimes == 2 then
                log:debug("次数三次 停止测试")
                return
            end
            config.measureParam.currentRange = self.targetRange 		--更新量程配置文件
            ConfigLists.SaveMeasureParamConfig()
            self.currentRange = config.measureParam.range[config.measureParam.currentRange + 1] + 1
            config.modifyRecord.measureParam(true)
            ConfigLists.SaveMeasureParamConfig()
            error(MeasureRangeWrongException:new())  --抛出量程错误异常
        elseif config.measureParam.activeRangeMode == setting.mode.activeRange.next and self.targetRange ~= config.measureParam.currentRange then
            log:info("量程切换：下次生效")
            self.isChangeRangeAtTheEnd = true
        elseif self.targetRange == config.measureParam.currentRange then
            log:info("量程切换：无需切换")
        end
    end

end


--[[
 * @brief 量程切换
 * @details 当测量结果不在当前量程范围内，自动切换量程。
 * @param[in] result, 结果浓度。
 * @param[in]  currange 当前量程。
--]]
function MeasureFlow:RangeChange(result, currange)
    local destRange = 0                                         --真实量程
    local realRange = config.measureParam.currentRange          --界面当前量程
    local rangeNum = 0

    --将量程存进表中
    local rangeTable = {}
    for i = 1, setting.measure.range.rangeNum do
        if         i == (config.measureParam.range[1] + 1)
                or i == (config.measureParam.range[2] + 1)
                or i == (config.measureParam.range[3] + 1) then
            rangeNum = rangeNum + 1
            table.insert(rangeTable, i)
        end
    end

    --向上切换 只允许向上切换一级
    if result > setting.measure.range[currange].rangeWindowMax then
        if rangeTable[rangeNum] == currange then
            log:info("量程已是配置的最大量程")
        else
            for num = 1, rangeNum do
                if rangeTable[num] > currange then
                    for i = 1, 3 do
                        if (rangeTable[num]-1) == config.measureParam.range[i] then
                            realRange = i-1
                            break
                        end
                    end
                    break
                end
            end
        end
        --向下切换
    elseif result < setting.measure.range[currange].rangeWindowMin then
        if currange ~= rangeTable[1] then
            for num, range in pairs(rangeTable) do
                if result < setting.measure.range[range].rangeWindowMax then
                    destRange = range
                    break
                end
            end

            --真实量程转换为界面当前量程一二三
            for i = 1, 3 do
                if (destRange - 1) == config.measureParam.range[i] then
                    realRange = i-1
                    break
                end
            end
        else
            log:info("量程已是配置的最小量程")
        end
    end

    return realRange
end


--[[
 * @brief 水样采集
 * @details 测量水样时，采集更新水样
--]]
function MeasureFlow:CollectSample()
    local waterCollector = WaterCollector.Instance()
    local mode = config.interconnection.collectSampleMode

    if mode == setting.mode.collectSample.beforeMeasure then

        local runAction = Helper.Status.SetAction(setting.runAction.collectSample.collect)
        StatusManager.Instance():SetAction(runAction)

        if not string.find(config.info.instrument["type"], "PT63P") then
            waterCollector:TurnOn()
        end
        Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, true)

        if not self:Wait(config.interconnection.miningWaterTime) then 	-- 采集等待
            if config.interconnection.collectSampleMode ~= setting.mode.collectSample.trigger then
                if not string.find(config.info.instrument["type"], "PT63P") then
                    waterCollector:TurnOff()
                end
                Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, false)
            end

            error(UserStopException:new())    --用户停止
        end

        runAction = Helper.Status.SetAction(setting.runAction.collectSample.silent)
        StatusManager.Instance():SetAction(runAction)

        if not string.find(config.info.instrument["type"], "PT63P") then
            waterCollector:TurnOff()
        end
        Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, false)

        if not self:Wait(config.interconnection.silentTime) then 	-- 静默等待
            error(UserStopException:new())    --用户停止
        end
    elseif mode == setting.mode.collectSample.toAddSampleEnd or mode == setting.mode.collectSample.toMeaFlowEnd then
        local runAction = Helper.Status.SetAction(setting.runAction.collectSample.collect)
        StatusManager.Instance():SetAction(runAction)

        if not string.find(config.info.instrument["type"], "PT63P") then
            waterCollector:TurnOn()
        end
        Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, true)

        if not self:Wait(config.interconnection.miningWaterTime) then 	-- 采集等待
            if config.interconnection.collectSampleMode ~= setting.mode.collectSample.trigger then
                if not string.find(config.info.instrument["type"], "PT63P") then
                    waterCollector:TurnOff()
                end
                Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, false)
            end

            error(UserStopException:new())    --用户停止
        end
    end
end


--[[
 * @brief 重新测量检查
 * @details 当测量出现异常或结果异常时，进行重新测量。
--]]
function MeasureFlow:RetryMeasureCheck(isRangeCheck)
    local rangeCheckRetry = false
    self.isCrashMeasure = false  --清除崩溃标记

    if setting.measureResult.continousModeParam.isfinishContinousMeasure == true then
        Measurer:ContinousMeasureSafetyStop()
        setting.measureResult.continousModeParam.isfinishContinousMeasure = false
    end

    if self.isFinish == true then
        log:debug("已出结果，无需重测");
        return false
    end

    --关闭异常重测
    if config.system.faultRetry ~= true then
        return false
    end

    if isRangeCheck == nil then
        rangeCheckRetry = false
    else
        rangeCheckRetry = isRangeCheck
    end

    self.faultTimes = self.faultTimes + 1   --更新重测次数记录

    if config.system.CCEPMode == true and CCEPSafeData < 2 then
        local useTime = os.time() - self.measureDateTime
        local leftTime = 3600 - useTime

        log:debug("安全测量剩余时间 = " .. leftTime .. " 安全测量时间 = " .. setting.measureResult.normalMeasureTime)
        if leftTime > setting.measureResult.normalMeasureTime then  --剩余时间充足,判断重测
            if self.faultTimes >= 3 then

                local isSave = false
                if self.measureType == MeasureType.Sample and config.scheduler.measure.mode == MeasureMode.Continous then
                    config.scheduler.measure.mode = MeasureMode.Trigger --连续测量变触发
                    isSave = true
                elseif self.measureType == MeasureType.ZeroCheck and config.scheduler.zeroCheck.mode == MeasureMode.Continous then
                    config.scheduler.zeroCheck.mode = MeasureMode.Trigger --连续测量变触发
                    isSave = true
                elseif self.measureType == MeasureType.RangeCheck and config.scheduler.rangeCheck.mode == MeasureMode.Continous then
                    config.scheduler.rangeCheck.mode = MeasureMode.Trigger --连续测量变触发
                    isSave = true
                end

                if isSave == true then
                    config.modifyRecord.scheduler(true)
                    ConfigLists.SaveSchedulerConfig()

                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ChangeMeaModeOnHome, "DeviceFaultException")
                end
                error(DeviceFaultException:new())   --抛出仪器故障运行停止异常
            else
                self.isRetryMeasure = true
                self.isRetryOnStart = true
                log:warn("测量异常重测次数 = "..self.faultTimes)
            end
        else	--生成安全测量结果
            self.isRetryMeasure = false
            self.isRetryOnStart = false
            --self:SafeResultGenerate()
            self.isFinish = true
        end

    else
        if self.faultTimes >= 3 then

            --量程切换不作为错误返回
            if rangeCheckRetry == true then
                self.isRetryMeasure = false
                self.isRetryOnStart = false
                return true
            end

            local isSave = false
            if self.measureType == MeasureType.Sample and config.scheduler.measure.mode == MeasureMode.Continous then
                config.scheduler.measure.mode = MeasureMode.Trigger --连续测量变触发
                isSave = true
            elseif self.measureType == MeasureType.ZeroCheck and config.scheduler.zeroCheck.mode == MeasureMode.Continous then
                config.scheduler.zeroCheck.mode = MeasureMode.Trigger --连续测量变触发
                isSave = true
            elseif self.measureType == MeasureType.RangeCheck and config.scheduler.rangeCheck.mode == MeasureMode.Continous then
                config.scheduler.rangeCheck.mode = MeasureMode.Trigger --连续测量变触发
                isSave = true
            end

            if isSave == true then
                config.modifyRecord.scheduler(true)
                ConfigLists.SaveSchedulerConfig()

                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeMeaModeOnHome, "DeviceFaultException")
            end
            error(DeviceFaultException:new())   --抛出仪器故障运行停止异常
        else
            self.isRetryMeasure = true
            self.isRetryOnStart = true
            log:warn("测量异常重测次数 = "..self.faultTimes)
        end
    end

    return true
end


--[[
 * @brief 安全结果生成
 * @details 剩余时间无法进行故障重测时，生成安全结果。
--]]
--[[function MeasureFlow:SafeResultGenerate()

    local curveK = config.measureParam.curveParam[self.currentRange].curveK
    local curveB = config.measureParam.curveParam[self.currentRange].curveB
    local diluteFactor = self.diluteFactor
    local random = 2*(math.random() - 0.5)*setting.measureResult.resultWaveRange

    local consistency = status.measure.report.measure.consistency + random   --生成浓度

    local oldConsistency = consistency/config.measureParam.reviseParameter[self.currentRange] --计算原始浓度
    local peak = oldConsistency * curveK + curveB  --生成吸光度

    --结果数据标识
    local resultMark = ResultMark.N
    self.reportMode = config.interconnection.reportMode     -- 出结果时重拿一次上报模式
    if self.reportMode == ReportMode.OnLine then
        --在线情况下 水样才去监测超上限与超量程的阈值，且核查才会标记为K
        if self.measureType == MeasureType.Sample then
            if self.consistency > config.interconnection.meaUpLimit and 1 == config.interconnection.overProofResultMark then
                resultMark = ResultMark.T
                self.isOverProof = true
            elseif self.consistency > config.system.rangeViewMap[self.currentRange].view then
                resultMark = ResultMark.E
            end
        elseif self.measureType == MeasureType.RangeCheck or self.measureType == MeasureType.ZeroCheck then
            resultMark = ResultMark.C
        elseif self.measureType == MeasureType.QualityHandle then
            resultMark = ResultMark.K
        end

        if self.measureType == MeasureType.RangeCheck or self.measureType == MeasureType.ZeroCheck then
            if 1 == config.interconnection.rangeCheckResultMark then
                resultMark = ResultMark.K
            end
        end
    elseif self.reportMode == ReportMode.OffLine then
        resultMark = ResultMark.B
    elseif self.reportMode == ReportMode.Maintain then
        resultMark = ResultMark.M
    elseif self.reportMode == ReportMode.Fault then
        resultMark = ResultMark.D
    elseif self.reportMode == ReportMode.Calibrate then
        resultMark = ResultMark.C
    elseif self.reportMode == ReportMode.Debugging then
        resultMark = ResultMark.A
    end

    --整点测量时，测量时间整点校正，手动启动不校正
    if self.adjustTime == true then  --排期设置
        if (self.measureType == MeasureType.Sample and config.scheduler.measure.mode == MeasureMode.Timed) or
                (self.measureType == MeasureType.ZeroCheck and config.scheduler.zeroCheck.mode == MeasureMode.Timed) or
                (self.measureType == MeasureType.RangeCheck and config.scheduler.rangeCheck.mode == MeasureMode.Timed) then

            self.measureDateTime = self:AdjustMeasureDateTime(self.measureDateTime)
        end
    end
--	print("Safe Result Generate.")
    local resultManager = ResultManager.Instance()
    local recordData = RecordData.new(resultManager:GetMeasureRecordDataSize(setting.resultFileInfo.measureRecordFile[1].name))
    recordData:PushInt(self.measureDateTime) -- 时间
    recordData:PushFloat(consistency) -- 浓度
    recordData:PushFloat(peak) -- 吸光度
    recordData:PushByte(resultMark) -- 结果标识
    recordData:PushByte(Measurer.measureType) -- 类型(当前测量过程)
    recordData:PushFloat(0) -- 初始制冷模块温度
    recordData:PushFloat(0) -- 初始测量模块温度
    recordData:PushFloat(0) -- 反应制冷模块温度
    recordData:PushFloat(0) -- 反应测量模块温度
    recordData:PushFloat(0) -- 初始值燃烧炉温度
    recordData:PushFloat(0) -- 初始值上机箱温度
    recordData:PushFloat(0) -- 初始值下机箱温度
    recordData:PushFloat(0) -- 反应值燃烧炉温度
    recordData:PushFloat(0) -- 反应值上机箱温度
    recordData:PushFloat(0) -- 反应值下机箱温度
    recordData:PushInt(os.time()-self.measureDateTime) -- 测量时长
    recordData:PushFloat(config.measureParam.curveParam[self.currentRange].RangeConsistency) -- 加量程校准液浓度
    recordData:PushFloat(config.system.rangeViewMap[self.currentRange].view)   -- 当前使用量程

    -- 隐藏预估值
    if config.system.displayProformaResult == true then
        UpdateWidgetManager.Instance():Update(UpdateEvent.ShowNewProformaData, "")
    end

    Helper.Result.OnMeasureResultAdded(self.measureType, self.measureDateTime, consistency, 0, 0, self.reportMode, peak,self.isUseStart,false)
    resultManager:AddMeasureRecord(setting.resultFileInfo.measureRecordFile[1].name, recordData, true)
    ConfigLists.SaveMeasureStatus()

    self.isFinish = true

    --测量结果
    log:debug("测量结果：A = "..peak.."C = "..consistency)

    CCEPSafeData = CCEPSafeData + 1   --安全记录
    log:debug("Safe Result Generate Times = "..CCEPSafeData)
end]]--

--[[
 * @brief 流程错误，生成上次测量结果
--]]
function MeasureFlow:FaultGetLastTimeResult()
    local consistency = status.measure.newResult.measure.consistency
    local peak = status.measure.newResult.measure.peak
    local measureType = status.measure.newResult.measure.resultType

    -- 隐藏预估值
    if config.system.displayProformaResult == true then
        UpdateWidgetManager.Instance():Update(UpdateEvent.ShowNewProformaData, "")
    end

    Helper.Result.OnMeasureResultAdded(measureType, self.measureDateTime, consistency, self.reportMode, peak,self.isUseStart, true)
    ConfigLists.SaveMeasureStatus()

    --测量结果
    log:debug("流程错误，测量结果生成：A = "..peak.." ,C = "..consistency)
end

--[[
 * @brief 整点测量的测量时间整点校准
 * @details 测量时间不是0分0秒时，对测量时间校准到整点。
--]]
function MeasureFlow:AdjustMeasureDateTime(meaDataTime)
    local newTime = meaDataTime

    local strOldTime = os.date("%Y-%m-%d %H:%M:%S",meaDataTime)
    log:debug("整点测量-原测量时间：" .. strOldTime .. " (" .. meaDataTime .. ")")

    if meaDataTime and meaDataTime >= 0 then

        local minute = tonumber(os.date("%M",meaDataTime))
        local second = tonumber(os.date("%S",meaDataTime))

        if minute >= 58 then
            newTime = newTime - minute * 60 - second + 3600   -- 当测量时间的分钟大于58时，分、秒清零，加1小时
        elseif minute > 0 and minute <= 20 then
            newTime = newTime - minute * 60 - second          -- 当测量时间的分钟大于0且小于5时，分、秒清零
        elseif minute == 0 and second > 0 then
            newTime = newTime - second                           -- 当测量时间的分钟等于0且秒大于0，秒清零
        end

        local strNewTime = os.date("%Y-%m-%d %H:%M:%S",newTime)
        log:debug("整点测量-新测量时间：" .. strNewTime .. " (" .. newTime .. ")")
    end

    return newTime
end