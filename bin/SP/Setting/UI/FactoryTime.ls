setting.ui.factoryTime =
{
    defaultRestore = function(userType)
        --[[
        测量排期, 测量参数,外联接口，上报模式,高级参数,锁屏参数，授权信息，
        设置IP模式,测量排期的状态
        --]]
        local isRestart = false

        local configTableInfo =
        {
            {
                fileName = "InterconnectionConfig.ls",
                tableName = "config.interconnection",
                configTable = config.interconnection,
                modifyRecord = function()
                    local isChanged = config.modifyRecord.interconnection(true)
                    return isChanged
                end
            },
            {
                fileName = "SchedulerConfig.ls",
                tableName = "config.scheduler",
                configTable = config.scheduler,
                modifyRecord = function()
                    config.modifyRecord.scheduler(true)
                    return false
                end
            },
        }

        for _,v in ipairs(configTableInfo) do

            local defaultTable = Serialization.LoadEncryptionFile(scriptsPath .. "/Config/Default/" .. v.fileName, v.tableName, false)
            Helper.DefaultRestore(defaultTable, v.configTable)
            if v.modifyRecord then
                if v.modifyRecord() == true then
                    isRestart = true
                end
            end
            Serialization.SaveEncryptionFile(v.configTable, scriptsPath .. "/Config/" .. v.fileName, v.tableName)

        end

        --恢复试剂配置
        local defaultTable = ConfigLists.LoadRemainConfig(true)
        Helper.DefaultRestore(defaultTable, config.remain)
        ConfigLists.SaveRemainConfig()
        --上传试剂配置
        WqimcManager.Instance():uploadReagentProfile()

        --恢复试剂记录
        local defaultTable = ConfigLists.LoadRemainStatus(true)
        Helper.DefaultRestore(defaultTable, status.remain)
        ConfigLists.SaveRemainStatus()
        --恢复试剂余量检查状态
        ReagentRemainManager.RecoverRemainCheckStatus()
        --上传试剂余量
        ReagentRemainManager.SaveRemainStatus()

        --恢复耗材配置
        local defaultTable = ConfigLists.LoadConsumableConfig(true)
        Helper.DefaultRestore(defaultTable, config.consumable)
        ConfigLists.SaveConsumableConfig()
        --上传耗材配置
        WqimcManager.Instance():uploadConsumableProfile()
        --恢复耗材检查状态
        MaterialLifeManager.RecoverMaterialLifeCheckStatus()

        --恢复测量状态
        if userType == RoleType.Super then
            local defaultTable = ConfigLists.LoadMeasureStatus(true)
            Helper.DefaultRestore(defaultTable.schedule, status.measure.schedule)
            status.measure.lastMeasureEndTime = defaultTable.lastMeasureEndTime
            for i=1,setting.measure.range.rangeNum do
                status.measure.calibrate[i].dateTime = defaultTable.calibrate[i].dateTime
                status.measure.calibrate[i].curveB = defaultTable.calibrate[i].curveB
                status.measure.calibrate[i].curveK = defaultTable.calibrate[i].curveK
                status.measure.calibrate[i].point0Consistency = defaultTable.calibrate[i].point0Consistency
                status.measure.calibrate[i].point1Consistency = defaultTable.calibrate[i].point1Consistency
                status.measure.calibrate[i].point0Absorbance = defaultTable.calibrate[i].point0Absorbance
                status.measure.calibrate[i].point1Absorbance = defaultTable.calibrate[i].point1Absorbance
                status.measure.calibrate[i].R2 = defaultTable.calibrate[i].R2
                status.measure.calibrate[i].zeroCalibrateDateTime = defaultTable.calibrate[i].zeroCalibrateDateTime
                status.measure.calibrate[i].standardCalibrateDateTime = defaultTable.calibrate[i].standardCalibrateDateTime
                status.measure.calibrate[i].userSettings = defaultTable.calibrate[i].userSettings
            end
            status.measure.standardCurve = 0
            ConfigLists.SaveMeasureStatus()
        end

        --恢复定量点设置
        local defaultTable = ConfigLists.LoadHardwareConfig(true)
        Helper.DefaultRestore(defaultTable, config.hardwareConfig)

        if math.abs(config.hardwareConfig.meterPoint.point[1].setVolume - defaultTable.meterPoint.point[1].setVolume)> 0.000001 or
            math.abs(config.hardwareConfig.meterPoint.point[2].setVolume - defaultTable.meterPoint.point[2].setVolume)> 0.000001 then
            config.hardwareConfig.meterPoint.point[1].setVolume = defaultTable.meterPoint.point[1].setVolume
            config.hardwareConfig.meterPoint.point[2].setVolume = defaultTable.meterPoint.point[2].setVolume
            ConfigLists.SaveHardwareConfig()
            log:debug("定量点1 设定值 "..config.hardwareConfig.meterPoint.point[1].setVolume.." -> "..defaultTable.meterPoint.point[1].setVolume)
            log:debug("定量点2 设定值 "..config.hardwareConfig.meterPoint.point[2].setVolume.." -> "..defaultTable.meterPoint.point[2].setVolume)

        end

        if userType == RoleType.Super then
            config.hardwareConfig.tempCalibrate.calibrationVoltage = defaultTable.tempCalibrate.calibrationVoltage
            config.hardwareConfig.tempCalibrate.negativeInput = defaultTable.tempCalibrate.negativeInput
            config.hardwareConfig.tempCalibrate.referenceVoltage = defaultTable.tempCalibrate.referenceVoltage
            config.hardwareConfig.NDIRTempCalibrate.calibrationVoltage = defaultTable.NDIRTempCalibrate.calibrationVoltage
            config.hardwareConfig.NDIRTempCalibrate.negativeInput = defaultTable.NDIRTempCalibrate.negativeInput
            config.hardwareConfig.NDIRTempCalibrate.referenceVoltage = defaultTable.NDIRTempCalibrate.referenceVoltage
            config.hardwareConfig.fanUpTempCalibrate.calibrationVoltage = defaultTable.fanUpTempCalibrate.calibrationVoltage
            config.hardwareConfig.fanUpTempCalibrate.negativeInput = defaultTable.fanUpTempCalibrate.negativeInput
            config.hardwareConfig.fanUpTempCalibrate.referenceVoltage = defaultTable.fanUpTempCalibrate.referenceVoltage
            config.hardwareConfig.fanDownTempCalibrate.calibrationVoltage = defaultTable.fanDownTempCalibrate.calibrationVoltage
            config.hardwareConfig.fanDownTempCalibrate.negativeInput = defaultTable.fanDownTempCalibrate.negativeInput
            config.hardwareConfig.fanDownTempCalibrate.referenceVoltage = defaultTable.fanDownTempCalibrate.referenceVoltage
            config.hardwareConfig.stoveTempCalibrate.calibrationFactor = defaultTable.stoveTempCalibrate.calibrationFactor
            config.hardwareConfig.stoveTempCalibrate.calibrationAD = defaultTable.stoveTempCalibrate.calibrationAD
            config.hardwareConfig.stoveTempCalibrate.calibrationTemp = defaultTable.stoveTempCalibrate.calibrationTemp

            config.hardwareConfig.meterLed.maxValue = defaultTable.meterLed.maxValue
            config.hardwareConfig.meterLed.point[1].set = defaultTable.meterLed.point[1].set
            config.hardwareConfig.meterLed.point[1].real = defaultTable.meterLed.point[1].real
            config.hardwareConfig.meterLed.point[2].set = defaultTable.meterLed.point[2].set
            config.hardwareConfig.meterLed.point[2].real = defaultTable.meterLed.point[2].real

            config.hardwareConfig.measureLed.maxValue = defaultTable.measureLed.maxValue
            config.hardwareConfig.measureLed.reference.set = defaultTable.measureLed.reference.set
            config.hardwareConfig.measureLed.reference.real = defaultTable.measureLed.reference.real
            config.hardwareConfig.measureLed.measure.set = defaultTable.measureLed.measure.set
            config.hardwareConfig.measureLed.measure.real = defaultTable.measureLed.measure.real
            config.hardwareConfig.measureLed.targetAD.set = defaultTable.measureLed.targetAD.set
            config.hardwareConfig.measureLed.targetAD.real = defaultTable.measureLed.targetAD.real
            config.hardwareConfig.measureLed.ledDAC.set = defaultTable.measureLed.ledDAC.set
            config.hardwareConfig.measureLed.ledDAC.real = defaultTable.measureLed.ledDAC.real
            config.hardwareConfig.motorOffsetStep.offsetStep.set = defaultTable.motorOffsetStep.offsetStep.set
            config.hardwareConfig.motorOffsetStep.offsetStep.real = defaultTable.motorOffsetStep.offsetStep.real
            isRestart = true
        end
        ConfigLists.SaveHardwareConfig()


        --恢复测量参数,非工程师时不可恢复
        if userType == RoleType.Super then
            local defaultTable = ConfigLists.LoadMeasureParamConfig(true)
            Helper.DefaultRestore(defaultTable, config.measureParam)
            config.measureParam.reactTemperature = defaultTable.reactTemperature
            config.measureParam.reactTime = defaultTable.reactTime
            setting.ui.profile.measureParam.updaterCurveParam(0,true)
            config.modifyRecord.measureParam(true)
            ConfigLists.SaveMeasureParamConfig()
        elseif  userType == RoleType.Administrator then

            local defaultTable = ConfigLists.LoadMeasureParamConfig(true)
            config.measureParam.currentRange = defaultTable.currentRange
            config.measureParam.calibrateRangeIndex = defaultTable.calibrateRangeIndex
            config.measureParam.zeroCheckRangeIndex = defaultTable.zeroCheckRangeIndex
            config.measureParam.rangeCheckRangeIndex = defaultTable.rangeCheckRangeIndex
            config.measureParam.autoChangeRange = defaultTable.autoChangeRange
            config.measureParam.activeRangeMode = defaultTable.activeRangeMode
            config.measureParam.range[1] = defaultTable.range[1]
            config.measureParam.range[2] = defaultTable.range[2]
            config.measureParam.range[3] = defaultTable.range[3]

            config.measureParam.negativeRevise = defaultTable.negativeRevise
            config.measureParam.reviseFactor = defaultTable.reviseFactor


            config.measureParam.checkConsistency = defaultTable.checkConsistency
            config.measureParam.checkErrorLimit = defaultTable.checkErrorLimit
            config.measureParam.failAutoRevise = defaultTable.failAutoRevise
            config.measureParam.accurateCheck = defaultTable.accurateCheck
            config.measureParam.wasteWaterEnvironment = defaultTable.wasteWaterEnvironment

            config.measureParam.cleanBefMeaBlankVol = defaultTable.cleanBefMeaBlankVol
            config.measureParam.cleanAftMeaBlankVol = defaultTable.cleanAftMeaBlankVol
            config.measureParam.extendSamplePipeVolume = defaultTable.extendSamplePipeVolume
            config.measureParam.sampleRenewVolume = defaultTable.sampleRenewVolume
            config.measureParam.zeroAccurateCalibrate = defaultTable.zeroAccurateCalibrate
            config.measureParam.standardAccurateCalibrate = defaultTable.standardAccurateCalibrate
            config.measureParam.rangeAccurateCalibrate = defaultTable.rangeAccurateCalibrate
            config.measureParam.rangeCalibrateDeviation = defaultTable.rangeCalibrateDeviation
            config.measureParam.curveQualifiedDetermination = defaultTable.curveQualifiedDetermination

            	    
            config.modifyRecord.measureParam(true)
            ConfigLists.SaveMeasureParamConfig()

        elseif userType == RoleType.Maintain then
            local defaultTable = ConfigLists.LoadMeasureParamConfig(true)
            config.measureParam.currentRange = defaultTable.currentRange
            config.measureParam.calibrateRangeIndex = defaultTable.calibrateRangeIndex
            config.measureParam.zeroCheckRangeIndex = defaultTable.zeroCheckRangeIndex
            config.measureParam.rangeCheckRangeIndex = defaultTable.rangeCheckRangeIndex
            config.measureParam.autoChangeRange = defaultTable.autoChangeRange
            config.measureParam.activeRangeMode = defaultTable.activeRangeMode
            config.measureParam.range[1] = defaultTable.range[1]
            config.measureParam.range[2] = defaultTable.range[2]
            config.measureParam.range[3] = defaultTable.range[3]
	    
            config.modifyRecord.measureParam(true)
            ConfigLists.SaveMeasureParamConfig()
        end

        --系统参数
        if userType == RoleType.Super then

            local defaultTable = ConfigLists.LoadSystemConfig(true)
            Helper.DefaultRestore(defaultTable, config.system)

            if config.modifyRecord.system(true) == true then
                isRestart = true
            end
            ConfigLists.SaveSystemConfig()
        elseif userType == RoleType.Administrator then

            local defaultTable = ConfigLists.LoadSystemConfig(true)

            config.system.faultBlocking = defaultTable.faultBlocking
            config.system.faultRetry = defaultTable.faultRetry
            config.system.modbusTableType = defaultTable.modbusTableType
            config.system.changeParamLog = defaultTable.changeParamLog
            config.system.adcDetect[1].enable = defaultTable.adcDetect[1].enable
            config.system.adcDetect[2].enable = defaultTable.adcDetect[2].enable
            config.system.displayProformaResult = defaultTable.displayProformaResult
            config.system.reagentLackWarn = defaultTable.reagentLackWarn
            config.system.hj212Type = defaultTable.hj212Type
            config.system.hj212Interval = defaultTable.hj212Interval
            config.system.hj212DataType = defaultTable.hj212DataType
            if config.modifyRecord.system(true) == true then
                isRestart = true
            end
            ConfigLists.SaveSystemConfig()
        end
        App.SetScreenTime(config.system.screenSaver.darkTime, config.system.screenSaver.offTime, config.system.screenSaver.loginKeepingTime)
        App.SetSceenEnable(config.system.screenSaver.enable)

        return isRestart
    end,
    --[[
      * @brief 清除测量/校准/告警/日志/曲线/测量状态
      --]]
    clearData = function()
        local defaultTable = ConfigLists.LoadMeasureStatus(true)
        Helper.DefaultRestore(defaultTable, status.measure)

        ConfigLists.SaveMeasureStatus()
    end,
    --[[
  * @brief 清除故障状态标志
  --]]
    clearFault = function()
        setting.temperature.monitor.stoveAlarm = false
        setting.temperature.monitor.refrigeratorAlarm = false
        setting.temperature.monitor.NDIRAlarm = false
        status.measure.isDeviceFault = false
        ConfigLists.SaveMeasureStatus()

        local isFaultStatus = StatusManager.Instance():IsFaultStatus()
        if isFaultStatus == true then
            StatusManager.Instance():ResetDeviceStatus()
        end
    end,
}

return setting.ui.factoryTime
