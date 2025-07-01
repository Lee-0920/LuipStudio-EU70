setting.runStatus =
{
    collectSample =
    {
        name = "CollectSample",
        text = "采集水样",
        GetTime = function()
            return config.interconnection.miningWaterTime + config.interconnection.silentTime
        end,
    },
    sampleWaterRelay =
    {
        name = "sampleWaterRelay",
        text = "反吹",
        GetTime = function()
            return config.scheduler.collectSample.collectTime
        end,
    },
    digestCooling =
    {
        name = "DigestCooling",
        text = "燃烧炉冷却",
        GetTime = function()
            return setting.runAction.digestCooling.GetTime()
        end,
    },
    digesterHeating =
    {
        name = "digesterHeating",
        text = "燃烧炉升温",
        GetTime = function()
            return setting.runAction.digesterHeating.GetTime()
        end,
    },
    refrigeratorCooling =
    {
        name = "RefrigeratorCooling",
        text = "制冷器冷却",
        GetTime = function()
            return setting.runAction.refrigeratorSafetyCheck.GetTime()
        end,
    },
    cleanMeterPipe =
    {
        name = "CleanMeterPipe",
        text = "清洗定量管",
        GetTime = function()
            return setting.runAction.cleanMeterPipe.GetTime()
        end,
    },
    cleanDeeply =
    {
        name = "CleanDeeply",
        text = "深度清洗",
        GetTime = function()
            return setting.runAction.cleanDeeply.GetTime()
        end,
    },
    oneKeyRenew =
    {
        name = "OneKeyRenew",
        text = "一键更新试剂",
        GetTime = function()
            return 80
        end,
    },
    cleanAll =
    {
        name = "CleanAll",
        text = "清洗所有管路",
        GetTime = function()
            local totalTime = 400
            return totalTime		-- 清洗所有管路流程总时间，单位为秒
        end,
    },
    measureBlank =
    {
        name = "MeasureBlank",
        text = "测量零点校准液",
        GetTime = function()
            local totalTime = 900
            return totalTime
        end,
    },
    measureSample =
    {
        name = "MeasureSample",
        text = "测量在线水样",
        GetTime = function()
            local totalTime = 480
            local accurateRunTime = 700

            if config.measureParam.accurateMeasure == true then
                totalTime = totalTime + accurateRunTime
            end

            --if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
            --    totalTime = totalTime + 160 + 30
            --end

            if  setting.measureResult.isStartAccurateMeasure == true then
                accurateRunTime = setting.runAction.measure.accurateMeasureMode.GetTime()
                totalTime = totalTime + accurateRunTime
            end
            if config.measureParam.range[config.measureParam.currentRange + 1] == 1 then
                totalTime = totalTime
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 2 then
                totalTime = totalTime
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 3 then
                totalTime = totalTime + 160
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    totalTime = totalTime + 60
                end
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 4 then
                totalTime = totalTime + 190
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 5 then
                totalTime = totalTime + 170
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    totalTime = totalTime + 60
                end
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 6 then
                totalTime = totalTime + 400
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 7 then
                totalTime = totalTime + 480
            end
            return totalTime
        end,
    },
    measureStandard =
    {
        name = "MeasureStandard",
        text = "测量离线水样",
        GetTime = function()
            local totalTime = 900
            local accurateRunTime = 0
            if config.scheduler.measure.mode == MeasureMode.Continous then
                totalTime = 650
            end

            if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                totalTime = totalTime + 160 + 30
            end

            if setting.measureResult.isStartAccurateMeasure == true then
                accurateRunTime = setting.runAction.measure.accurateMeasureMode.GetTime()
                totalTime = totalTime + accurateRunTime
            end
            if config.measureParam.range[config.measureParam.currentRange + 1] == 1 then
                totalTime = totalTime
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 2 then
                totalTime = totalTime
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 3 then
                totalTime = totalTime + 160
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    totalTime = totalTime + 60
                end
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 4 then
                totalTime = totalTime + 190
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 5 then
                totalTime = totalTime + 170
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    totalTime = totalTime + 60
                end
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 6 then
                totalTime = totalTime + 400
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 7 then
                totalTime = totalTime + 480
            end
            return totalTime
        end,
    },
    measureSampleRangeCheck =
    {
        name = "SampleRangeCheck",
        text = "水样核查",
        GetTime = function()
            local totalTime = 900
            local accurateRunTime = 0
            if config.scheduler.measure.mode == MeasureMode.Continous then
                totalTime = 650
            end

            if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                totalTime = totalTime + 160 + 30
            end

            if  setting.measureResult.isStartAccurateMeasure == true then
                accurateRunTime = setting.runAction.measure.accurateMeasureMode.GetTime()
                totalTime = totalTime + accurateRunTime
            end
            if config.measureParam.range[config.measureParam.currentRange + 1] == 1 then
                totalTime = totalTime
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 2 then
                totalTime = totalTime
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 3 then
                totalTime = totalTime + 160
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    totalTime = totalTime + 60
                end
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 4 then
                totalTime = totalTime + 190
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 5 then
                totalTime = totalTime + 170
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    totalTime = totalTime + 60
                end
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 6 then
                totalTime = totalTime + 400
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 7 then
                totalTime = totalTime + 480
            end
            return totalTime
        end,
    },
    measureZeroCheck =
    {
        name = "MeasureZeroCheck",
        text = "零点核查",
        GetTime = function()
            local totalTime = 900
            local accurateRunTime = 0
            if config.scheduler.measure.mode == MeasureMode.Continous then
                totalTime = 650
            end

            if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                totalTime = totalTime + 160 + 30
            end

            if setting.measureResult.isStartAccurateMeasure == true then
                accurateRunTime = setting.runAction.measure.accurateMeasureMode.GetTime()
                totalTime = totalTime + accurateRunTime
            end
            if config.measureParam.range[config.measureParam.zeroCheckRangeIndex + 1] == 1 then
                totalTime = totalTime
            elseif config.measureParam.range[config.measureParam.zeroCheckRangeIndex + 1] == 2 then
                totalTime = totalTime
            elseif config.measureParam.range[config.measureParam.zeroCheckRangeIndex + 1] == 3 then
                totalTime = totalTime + 160
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    totalTime = totalTime + 60
                end
            elseif config.measureParam.range[config.measureParam.zeroCheckRangeIndex + 1] == 4 then
                totalTime = totalTime + 190
            elseif config.measureParam.range[config.measureParam.zeroCheckRangeIndex + 1] == 5 then
                totalTime = totalTime + 170
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    totalTime = totalTime + 60
                end
            elseif config.measureParam.range[config.measureParam.zeroCheckRangeIndex + 1] == 6 then
                totalTime = totalTime + 400
            elseif config.measureParam.range[config.measureParam.zeroCheckRangeIndex + 1] == 7 then
                totalTime = totalTime + 480
            end
            return totalTime
        end,
    },
    measureRangeCheck =
    {
        name = "MeasureRangeCheck",
        text = "标样(跨度)核查",
        GetTime = function()
            local totalTime = 900
            local accurateRunTime = 0
            if config.scheduler.measure.mode == MeasureMode.Continous then
                totalTime = 650
            end

            if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                totalTime = totalTime + 160 + 30
            end

            if setting.measureResult.isStartAccurateMeasure == true then
                accurateRunTime = setting.runAction.measure.accurateMeasureMode.GetTime()
                totalTime = totalTime + accurateRunTime
            end
            if config.measureParam.range[config.measureParam.rangeCheckRangeIndex + 1] == 1 then
                totalTime = totalTime
            elseif config.measureParam.range[config.measureParam.rangeCheckRangeIndex + 1] == 2 then
                totalTime = totalTime
            elseif config.measureParam.range[config.measureParam.rangeCheckRangeIndex + 1] == 3 then
                totalTime = totalTime + 160
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    totalTime = totalTime + 60
                end
            elseif config.measureParam.range[config.measureParam.rangeCheckRangeIndex + 1] == 4 then
                totalTime = totalTime + 190
            elseif config.measureParam.range[config.measureParam.rangeCheckRangeIndex + 1] == 5 then
                totalTime = totalTime + 170
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    totalTime = totalTime + 60
                end
            elseif config.measureParam.range[config.measureParam.rangeCheckRangeIndex + 1] == 6 then
                totalTime = totalTime + 400
            elseif config.measureParam.range[config.measureParam.rangeCheckRangeIndex + 1] == 7 then
                totalTime = totalTime + 480
            end
            return totalTime
        end,
    },
    measureQualityHandle =
    {
        name = "MeasureQualityHandle",
        text = "测质控样",
        GetTime = function()
            local totalTime = 900
            local accurateRunTime = 0
            if config.scheduler.measure.mode == MeasureMode.Continous then
                totalTime = 650
            end

            if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                totalTime = totalTime + 160 + 30
            end

            if setting.measureResult.isStartAccurateMeasure == true then
                accurateRunTime = setting.runAction.measure.accurateMeasureMode.GetTime()
                totalTime = totalTime + accurateRunTime
            end
            if config.measureParam.range[config.measureParam.currentRange + 1] == 1 then
                totalTime = totalTime
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 2 then
                totalTime = totalTime
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 3 then
                totalTime = totalTime + 160
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    totalTime = totalTime + 60
                end
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 4 then
                totalTime = totalTime + 190
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 5 then
                totalTime = totalTime + 170
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    totalTime = totalTime + 60
                end
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 6 then
                totalTime = totalTime + 400
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 7 then
                totalTime = totalTime + 480
            end
            return totalTime
        end,
    },
    measureAddstandard =
    {
        name = "MeasureAddstandard",
        text = "加标回收",
        GetTime = function()
            local totalTime = 4500
            return totalTime
        end,
    },
    measureParallel =
    {
        name = "MeasureParallel",
        text = "平行测量",
        GetTime = function()
            local totalTime = 4500
            return totalTime
        end,
    },
    measureExtAddstandard =
    {
        name = "MeasureExtAddstandard",
        text = "测加标样(外部)",
        GetTime = function()
            local totalTime = 2300
            if config.measureParam.range[config.measureParam.currentRange + 1] == 1 then
                totalTime = totalTime + 150
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 2 then
                totalTime = totalTime + 350
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 3 then
                totalTime = totalTime + 500
            end
            return totalTime
        end,
    },
    measureExtParallel =
    {
        name = "MeasureExtParallel",
        text = "测平行样(外部)",
        GetTime = function()
            local totalTime = 2300
            if config.measureParam.range[config.measureParam.currentRange + 1] == 1 then
                totalTime = totalTime + 150
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 2 then
                totalTime = totalTime + 350
            elseif config.measureParam.range[config.measureParam.currentRange + 1] == 3 then
                totalTime = totalTime + 500
            end
            return totalTime
        end,
    },
    calibrate =
    {
        name = "Calibrate",
        text = "单点校准",
        GetTime = function()
            local totalTime = 1350
            if config.measureParam.rangeAccurateCalibrate == true then
                totalTime = totalTime + 1300
            end
            return totalTime
        end,
    },
    mulCalibrate =
    {
        name = "MulCalibrate",
        text = "多点校准",
        GetTime = function()
            local totalTime = 2200 + 90
            if config.measureParam.rangeAccurateCalibrate == true then
                totalTime = totalTime + 1300
            end
            return totalTime     -- 一键维护流程总时间，单位为秒
        end,
    },
    singlePoint =
    {
        name = "SinglePoint",
        text = "单点确认",
        GetTime = function()
            local totalTime = 2200 + 90
            return totalTime     -- 一键维护流程总时间，单位为秒
        end,
    },
    systemAdaptability =
    {
        name = "SystemAdaptability",
        text = "系统适用性确认",
        GetTime = function()
            local totalTime = 2200 + 90
            return totalTime     -- 一键维护流程总时间，单位为秒
        end,
    },
    sterileWaterAdaptability =
    {
        name = "SterileWaterAdaptability",
        text = "无菌水适用性确认",
        GetTime = function()
            local totalTime = 2200 + 90
            return totalTime     -- 一键维护流程总时间，单位为秒
        end,
    },
    robustness =
    {
        name = "Robustness",
        text = "鲁棒性验证",
        GetTime = function()
            local totalTime = 2200 + 90
            return totalTime     -- 一键维护流程总时间，单位为秒
        end,
    },
    specificity =
    {
        name = "Specificity",
        text = "特异性验证",
        GetTime = function()
            local totalTime = 2200 + 90
            return totalTime     -- 一键维护流程总时间，单位为秒
        end,
    },
    linear =
    {
        name = "Linear",
        text = "线性验证",
        GetTime = function()
            local totalTime = 2200 + 90
            return totalTime     -- 一键维护流程总时间，单位为秒
        end,
    },
    sdbsAdaptability =
    {
        name = "SdbsAdaptability",
        text = "SDBS适用性验证",
        GetTime = function()
            local totalTime = 2200 + 90
            return totalTime     -- 一键维护流程总时间，单位为秒
        end,
    },
    accuracy =
    {
        name = "Accuracy",
        text = "准确性验证",
        GetTime = function()
            local totalTime = 2200 + 90
            return totalTime     -- 一键维护流程总时间，单位为秒
        end,
    },
    icr =
    {
        name = "ICR",
        text = "ICR验证",
        GetTime = function()
            local totalTime = 2200 + 90
            return totalTime     -- 一键维护流程总时间，单位为秒
        end,
    },
    onlyCalibrateBlank =
    {
        name = "OnlyCalibrateBlank",
        text = "零点校准",
        GetTime = function()
            local totalTime = 900
            if config.measureParam.rangeAccurateCalibrate == true then
                totalTime = totalTime + 500
            end
            return totalTime     -- 零点校准流程总时间，单位为秒
        end,
    },
    onlyCalibrateStandard =
    {
        name = "OnlyCalibrateStandard",
        text = "量程校准",
        GetTime = function()
            local totalTime = 900
            if config.measureParam.rangeAccurateCalibrate == true then
                totalTime = totalTime + 500
            end
            return totalTime     -- 量程校准流程总时间，单位为秒
        end,
    },
    range1CurveCalibrate =
    {
        name = "Range1CurveCalibrate",
        text = "量程一因子校准",
        GetTime = function()
            local totalTime = 900
            if config.measureParam.rangeAccurateCalibrate == true then
                totalTime = totalTime + 500
            end
            return totalTime
        end,
    },
    range2CurveCalibrate =
    {
        name = "Range2CurveCalibrate",
        text = "量程二因子校准",
        GetTime = function()
            local totalTime = 900
            if config.measureParam.rangeAccurateCalibrate == true then
                totalTime = totalTime + 500
            end
            return totalTime
        end,
    },
    range3CurveCalibrate =
    {
        name = "Range3CurveCalibrate",
        text = "量程三因子校准",
        GetTime = function()
            local totalTime = 900
            if config.measureParam.rangeAccurateCalibrate == true then
                totalTime = totalTime + 500
            end
            return totalTime
        end,
    },
    range4CurveCalibrate =
    {
        name = "Range4CurveCalibrate",
        text = "量程四因子校准",
        GetTime = function()
            local totalTime = 880
            if config.measureParam.rangeAccurateCalibrate == true then
                totalTime = totalTime + 1020
            end
            return totalTime
        end,
    },
    range5CurveCalibrate =
    {
        name = "Range5CurveCalibrate",
        text = "量程五因子校准",
        GetTime = function()
            local totalTime = 960
            if config.measureParam.rangeAccurateCalibrate == true then
                totalTime = totalTime + 1020
            end
            return totalTime
        end,
    },
    range6CurveCalibrate =
    {
        name = "Range6CurveCalibrate",
        text = "量程六因子校准",
        GetTime = function()
            local totalTime = 1040
            if config.measureParam.rangeAccurateCalibrate == true then
                totalTime = totalTime + 1020
            end
            return totalTime
        end,
    },
    range7CurveCalibrate =
    {
        name = "Range7CurveCalibrate",
        text = "量程七因子校准",
        GetTime = function()
            local totalTime = 1120
            if config.measureParam.rangeAccurateCalibrate == true then
                totalTime = totalTime + 1020
            end
            return totalTime
        end,
    },
    autoECZero =
    {
        name = "AutoECZero",
        text = "电导率自动归零",
    },
    communication =
    {
        name = "CommunicationCheck",
        text = "通信检测",
    },
    smartDetect =
    {
        name = "SmartDetect",
        text = "智能诊断",
    },
    liquidOperate =
    {
        name = "LiquidOperate",
        text = "管道操作",
    },
    combineOperate =
    {
        name = "CombineOperate",
        text = "组合操作",
    },
    DCUpdate =
    {
        name = "DCUpdate",
        text = "驱动板升级",
    },
    LCUpdate =
    {
        name = "LCUpdate",
        text = "液路板升级",
    },
    RCUpdate =
    {
        name = "RCUpdate",
        text = "信号板升级",
    },
    OCUpdate =
    {
        name = "OCUpdate",
        text = "输出板升级",
    },
    hardwareTest =
    {
        name = "HardwareTest",
        text = "硬件测试",
    },
    masterUpdate =
    {
        name = "MASTER",
        text = "主控板升级",
    },
    autoPumpCheck =
    {
        name = "AutoPumpCheck",
        text = "泵校准",
        GetTime = function()
            return setting.runAction.autoPumpCheck.GetTime()
        end,
    },
    meterADAdjust =
    {
        name = "MeterADAdjust",
        text = "定量AD调节",
        GetTime = function()
            return setting.runAction.meterADAdjust.GetTime()
        end,
    },
    measureADAdjust =
    {
        name = "MeasureADAdjust",
        text = "测量模块调节",
        GetTime = function()
            return setting.runAction.measureADAdjust.GetTime()
        end,
    },
    suckRefrigeratorWater =
    {
        name = "SuckRefrigeratorWater",
        text = "排冷凝液",
        GetTime = function()
            return setting.runAction.suckRefrigeratorWater.GetTime()
        end,
    },
    quickMeasure =
    {
        name = "QuickMeasure",
        text = "快速测量",
        GetTime = function()    -- 快速测量总时间，单位为秒
            return 300
        end,
    },
}
return setting.runStatus
