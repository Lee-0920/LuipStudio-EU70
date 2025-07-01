
setting.runAction =
{
    collectSample =
    {
        collect =
        {
            name = "Collect",
            text = "采集",
            GetTime = function()
                return config.interconnection.miningWaterTime
            end,
        },
        silent =
        {
            name = "Silent",
            text = "静置",
            GetTime = function()
                return config.interconnection.silentTime
            end,
        },
    },
    sampleWaterRelay =
    {
        name = "sampleWaterRelay",
        text = "反吹",
        GetTime = function()
            return config.scheduler.collectSample.collectTime
        end,
    },
    refrigeratorSafetyCheck =
    {
        name = "Cooling",
        text = "制冷器冷却",
        GetTime = function()   -- 消解器冷却时间，单位为秒
            return 60
        end,
    },
    digestCooling =
    {
        name = "Cooling",
        text = "燃烧炉冷却",
        GetTime = function()   -- 消解器冷却时间，单位为秒
            return 600
        end,
    },
    digestHeating =
    {
        name = "Heating",
        text = "燃烧炉升温",
        GetTime = function()   -- 消解器冷却时间，单位为秒
            return 600
        end,
    },
    measure =
    {
        init =
        {
            name = "Init",
            text = "初始化",
            GetTime = function()    -- 等待载气时间，单位为秒
                return 60
            end,
        },
        drainBeforeMeasure =
        {
            name = "DrainBeforeMeasure",
            text = "测量前排液",
            GetTime = function()    -- 测量前排液时间，单位为秒
                return 40
            end,
        },
        cleanBeforeMeasure =
        {
            name = "CleanBeforeMeasure",
            text = "测量前清洗",
            GetTime = function()    -- 测量前清洗时间，单位为秒
                return config.measureParam.cleanBefMeaBlankVol * 60
            end,
        },
        rinse =
        {
            name = "Rinse",
            text = "润洗",
            GetTime = function()
                local totalTime = 30
                totalTime = totalTime + (config.measureParam.sampleRenewVolume * 10)
                return totalTime -- 润洗时间，单位为秒
            end,
        },
        addSample =
        {
            name = "AddSample",
            text = "加待测样",
            GetTime = function()
                local totalTime = 70
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    totalTime = 100
                end
                if config.measureParam.range[config.measureParam.currentRange + 1] == 1 then
                    totalTime = totalTime
                elseif config.measureParam.range[config.measureParam.currentRange + 1] == 2 then
                    totalTime = totalTime + 50
                elseif config.measureParam.range[config.measureParam.currentRange + 1] == 3 then
                    totalTime = totalTime + 200
                elseif config.measureParam.range[config.measureParam.currentRange + 1] == 4 then
                    totalTime = totalTime + 80
                elseif config.measureParam.range[config.measureParam.currentRange + 1] == 5 then
                    totalTime = totalTime + 310
                    if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                        totalTime = totalTime + 60
                    end
                elseif config.measureParam.range[config.measureParam.currentRange + 1] == 6 then
                    totalTime = totalTime + 240
                elseif config.measureParam.range[config.measureParam.currentRange + 1] == 7 then
                    totalTime = totalTime + 320
                end
                return totalTime -- 加待测样时间，单位为秒
            end,
        },
        addReagent1 =
        {
            name = "AddReagent1",
            text = "加试剂一",
            GetTime = function()
                return 25			-- 加试剂一时间，单位为秒
            end,
        },
        mixReagent =
        {
            name = "MixReagent",
            text = "混合",
            GetTime = function()
                return 25			-- 加试剂一时间，单位为秒
            end,
        },
        cleanMeterPipe =
        {
            name = "CleanMeterPipe",
            text = "清洗定量管",
            GetTime = function()
                return 80		-- 清洗定量管时间，单位为秒
            end,
        },
        preheating =
        {
            name = "Preheating",
            text = "消解液预热",
            GetTime = function()
                return 60			-- 试剂一反应时间，单位为秒
            end,
        },
        addReagent2 =
        {
            name = "AddReagent2",
            text = "加试剂二",
            GetTime = function()
                return 240			-- 加试剂二时间，单位为秒
            end,
        },
        digest =
        {
            name = "Digest",
            text = "消解加热",
            GetTime = function()
                return 180 -- 消解时间 +  加热时间
            end,
        },
        digestCooling =
        {
            name = "DigestCooling",
            text = "消解冷却",
            GetTime = function()
                return 400				-- 消解冷却时间，单位为秒
            end,
        },
        react =
        {
            name = "React",
            text = "反应",
            GetTime = function()
                return 90			-- 读测量值时间，单位为秒
            end,
        },
        readMeasure =
        {
            name = "ReadMeasure",
            text = "读反应值",
            GetTime = function()
                return 30			-- 读测量值时间，单位为秒
            end,
        },
        drainAfterMeasure =
        {
            name = "DrainAfterMeasure",
            text = "测量后排液",
            GetTime = function()
                return 100	-- 测量后排液时间，单位为秒
            end,
        },
        cleanAfterMeasure =
        {
            name = "CleanAfterMeasure",
            text = "测量后清洗",
            GetTime = function()
                local time = 180
                if config.measureParam.range[config.measureParam.currentRange + 1] + 1 == 2 then
                    time = time + 30
                elseif config.measureParam.range[config.measureParam.currentRange + 1] + 1 == 3 then
                    time = time + 30
                elseif config.measureParam.range[config.measureParam.currentRange + 1] + 1 == 4 then
                    time = time + 30
                elseif config.measureParam.range[config.measureParam.currentRange + 1] + 1 == 5 then
                    time = time + 60
                end
                if true == config.measureParam.wasteWaterEnvironment then   -- 抗污模式
                    time = time + 160
                end
                return time	-- 测量后清洗时间，单位为秒
            end,
        },
        readInitialAD =
        {
            name = "ReadInitialAD",
            text = "读初始值",
            GetTime = function()
                return 80 + config.measureParam.readInitRilentTime  -- 读初始值时间，单位为秒
            end,
        },
        updateTime =
        {
            name = "UpdateTime",
            text = "管路更新",
            GetTime = function()
                return 50  -- 读反应值时间，单位为秒
            end,
        },
        addSampleTime =
        {
            name = "AddSampleTime",
            text = "加样",
            GetTime = function()
                return 70   -- 读反应值时间，单位为秒
            end,
        },
        baseLineCheck =
        {
            name = "BaseLineCheck",
            text = "加样等待",
            GetTime = function()
                return 120  -- 读反应值时间，单位为秒
            end,
        },
        accurateMeasureMode =
        {
            name = "AccurateMeasureMode",
            text = "精准测量",
            GetTime = function()
                return 120  -- 读反应值时间，单位为秒
            end,
        },
        waittingResult =
        {
            name = "WaittingResult",
            text = "等待测量结果",
            GetTime = function()
                local runTime = setting.runAction.measure.cleanAfterMeasure.GetTime()
                return config.measureParam.reacTime - runTime  -- 读反应值时间，单位为秒
            end,
        },
    },
    mulCalibrate =
    {
        clearWaste =
        {
            name = "ClearWaste",
            text = "清空残留液",
            GetTime = function()
                return 150
            end,
        },
        clearReagent1Pipe =
        {
            name = "ClearReagent1Pipe",
            text = "清空试剂一管",
            GetTime = function()
                return 40
            end,
        },
        clearStandardPipe =
        {
            name = "ClearStandardPipe",
            text = "清空量程校准液管",
            GetTime = function()
                return 40
            end,
        },
        clearReagent2Pipe =
        {
            name = "ClearReagent2Pipe",
            text = "清空试剂二管",
            GetTime = function()
                return 40
            end,
        },
        updateReagent1 =
        {
            name = "UpdateReagent1",
            text = "更新试剂一",
            GetTime = function()
                return 40
            end,
        },
        updateReagent2 =
        {
            name = "UpdateReagent2",
            text = "更新试剂二",
            GetTime = function()
                return 40
            end,
        },
    },
    cleanMeterPipe =
    {
        name = "Clean",
        text = "清洗",
        GetTime = function()
            return 50		-- 清洗定量管流程总时间，单位为秒
        end,
    },
    cleanDeeply =
    {
        name = "Clean",
        text = "清洗",
        GetTime = function()    -- 深度清洗流程总时间，单位为秒
            return 880
        end,
    },
    cleanAll =
    {
        cleanSamplePipe =
        {
            name = "CleanSamplePipe",
            text = "清洗水样管",
            GetTime = function()
                return 40
            end,
        },
        cleanStandardPipe =
        {
            name = "CleanStandardPipe",
            text = "清洗量程校准液管",
            GetTime = function()
                return 40
            end,
        },
        cleanReagent1Pipe =
        {
            name = "CleanReagent1Pipe",
            text = "清洗试剂一管",
            GetTime = function()
                return 40
            end,
        },
        cleanReagent2Pipe =
        {
            name = "CleanReagent2Pipe",
            text = "清洗试剂二管",
            GetTime = function()
                return 40
            end,
        },
        cleanBlankPipe =
        {
            name = "CleanBlankPipe",
            text = "清洗零点校准液管",
            GetTime = function()
                return 40
            end,
        },
        cleanZeroCheckPipe =
        {
            name = "CleanZeroCheckPipe",
            text = "清洗零点核查液管",
            GetTime = function()
                return 40
            end,
        },
        cleanRangeCheckPipe =
        {
            name = "CleanRangeCheckPipe",
            text = "清洗标样(跨度)核查液管",
            GetTime = function()
                return 40
            end,
        },
        drainSamplePipe =
        {
            name = "DrainSamplePipe",
            text = "排空水样管",
            GetTime = function()
                return 20
            end,
        },
        drainStandardPipe =
        {
            name = "DrainStandardPipe",
            text = "排空量程校准液管",
            GetTime = function()
                return 20
            end,
        },
        drainReagent1Pipe =
        {
            name = "DrainReagent1Pipe",
            text = "排空试剂一管",
            GetTime = function()
                return 20
            end,
        },
        drainReagent2Pipe =
        {
            name = "DrainReagent2Pipe",
            text = "排空试剂二管",
            GetTime = function()
                return 20
            end,
        },
        drainBlankPipe =
        {
            name = "DrainBlankPipe",
            text = "排空零点校准液管",
            GetTime = function()
                return 20
            end,
        },
        drainZeroCheckPipe =
        {
            name = "DrainZeroCheckPipe",
            text = "排空零点核查液管",
            GetTime = function()
                return 20
            end,
        },
        drainRangeCheckPipe =
        {
            name = "DrainRangeCheckPipe",
            text = "排空标样(跨度)核查液管",
            GetTime = function()
                return 20
            end,
        },
        drainWastePipe =
        {
            name = "DrainWastePipe",
            text = "排空分析废液管",
            GetTime = function()
                return 20
            end,
        },
        drainWasteWaterPipe =
        {
            name = "DrainWasteWaterPipe",
            text = "排空清洗废水管",
            GetTime = function()
                return 20
            end,
        },
    },
    autoPumpCheck =
    {
        name = "AutoPumpCheck",
        text = "定量泵校准",
        GetTime = function()    -- 定量泵校准总时间，单位为秒
            return 180
        end,
    },
    DCCommunication =
    {
        name = "DCCommunicationCheck",
        text = "驱动板检测",
    },
    LCCommunication =
    {
        name = "LCCommunicationCheck",
        text = "液路板检测",
    },
    RCCommunication =
    {
        name = "RCCommunicationCheck",
        text = "信号板检测",
    },
    OCCommunication =
    {
        name = "OCCommunicationCheck",
        text = "输出板检测",
    },
    smartMeasureDetect =
    {
        name = "SmartMeasureDetect",
        text = "测量模块诊断",
    },
    smartBlankValveDetect =
    {
        name = "SmartBlankValveDetect",
        text = "零点校准液阀诊断",
    },
    smartSampleValveDetect =
    {
        name = "SmartSampleValveDetect",
        text = "水样阀诊断",
    },
    smartStandardValveDetect =
    {
        name = "SmartStandardValveDetect",
        text = "量程校准液阀诊断",
    },
    smartZeroCheckValveDetect =
    {
        name = "SmartZeroCheckValveDetect",
        text = "零点核查液阀诊断",
    },
    smartRangeCheckValveDetect =
    {
        name = "SmartRangeCheckValveDetect",
        text = "标样(跨度)核查液阀诊断",
    },
    smartReagent1ValveDetect =
    {
        name = "SmartReagent1ValveDetect",
        text = "试剂一阀诊断",
    },
    smartReagent2ValveDetect =
    {
        name = "SmartReagent2ValveDetect",
        text = "试剂二阀诊断",
    },
    smartDigestionRoomValveDetect =
    {
        name = "SmartDigestionRoomValveDetect",
        text = "逐出室阀诊断",
    },
    smartWasteValveDetect =
    {
        name = "SmartWasteValveDetect",
        text = "分析废液阀诊断",
    },
    smartWasteWaterValveDetect =
    {
        name = "SmartWasteWaterValveDetect",
        text = "清洗废水阀诊断",
    },
    suckFromBlank =
    {
        name = "SuckFromBlank",
        text = "填充零点校准液",
    },
    drainToBlank =
    {
        name = "DrainToBlank",
        text = "排至零点校准液管",
    },
    suckFromSample =
    {
        name = "SuckFromSample",
        text = "填充水样",
    },
    drainToSample =
    {
        name = "DrainToSample",
        text = "排至水样管",
    },
    suckFromStandard =
    {
        name = "SuckFromStandard",
        text = "填充量程校准液",
    },
    drainToStandard =
    {
        name = "DrainToStandard",
        text = "排至量程校准液管",
    },

    suckFromZeroCheck =
    {
        name = "SuckFromZeroCheck",
        text = "填充零点核查液",
    },
    drainToZeroCheck =
    {
        name = "DrainToZeroCheck",
        text = "排至零点核查液管",
    },
    suckFromRangeCheck =
    {
        name = "SuckFromRangeCheck",
        text = "填充标样(跨度)核查液",
    },
    drainToRangeCheck =
    {
        name = "DrainToRangeCheck",
        text = "排至标样(跨度)核查液管",
    },
    suckFromReagent1 =
    {
        name = "SuckFromReagent1",
        text = "填充试剂1",
    },
    drainToReagent1 =
    {
        name = "DrainToReagent1",
        text = "排至试剂1管",
    },
    suckFromReagent2 =
    {
        name = "SuckFromReagent2",
        text = "填充试剂2",
    },
    drainToReagent2 =
    {
        name = "DrainToReagent2",
        text = "排至试剂2管",
    },
    suckFromDigestionRoom =
    {
        name = "SuckFromDigestionRoom",
        text = "吸逐出室溶液",
    },
    drainToDigestionRoom =
    {
        name = "DrainToDigestionRoom",
        text = "排至逐出室",
    },
    drainToWaste =
    {
        name = "DrainToWaste",
        text = "定量管排至分析废液",
    },
    drainToWasteWater =
    {
        name = "DrainToWasteWater",
        text = "定量管排至清洗废水",
    },
    suckBlankToDigestionRoom =
    {
        name = "SuckBlankToDigestionRoom",
        text= "吸零点校准液至逐出室",
    },
    suckDigestionRoomToWaste =
    {
        name = "SuckDigestionRoomToWaste",
        text= "排逐出液至分析废液",
    },
    suckDigestionRoomToWasteWater =
    {
        name = "SuckDigestionRoomToWasteWater",
        text= "排逐出液至清洗废水",
    },
    gasSealCheck =
    {
        name = "GasSealCheck",
        text = "气密性检查",
    },
    oneKeyHeating =
    {
        name = "OneKeyHeating",
        text = "一键加热",
    },
    meterADAdjust =
    {
        name = "MeterADAdjust",
        text = "定量AD调节",
        GetTime = function()    -- 定量AD调节总时间，单位为秒
            return 80
        end,
    },
    measureADAdjust =
    {
        name = "MeasureADAdjust",
        text = "测量模块调节",
        GetTime = function()    -- 测量AD调节总时间，单位为秒
            return 280
        end,
    },
    suckRefrigeratorWater =
    {
        name = "SuckRefrigeratorWater",
        text = "排冷凝液",
        GetTime = function()    -- 测量AD调节总时间，单位为秒
            return 30
        end,
    },
    syringUpdate =
    {
        name = "SyringUpdate",
        text = "注射器更新",
        GetTime = function()
            return 60
        end,
    },
    syringTest =
    {
        name = "SyringTest",
        text = "注射器测试",
        GetTime = function()
            return 60
        end,
    },
    masterUpdate =
    {
        name = "Updater",
        text = "升级中",
    },
    erase =
    {
        name = "Erase",
        text = "擦除",
    },
    writeProgram =
    {
        name = "WriteProgram",
        text = "烧写",
    },
    hardwareTest =
    {
        pumpTest =
        {
            name = "PumpTest",
            text = "泵测试",
        },
        sampleValveTest =
        {
            name = "SampleValveTest",
            text = "水样阀测试",
        },
        reagent1ValveTest =
        {
            name = "Reagent1ValveTest",
            text = "试剂一阀测试",
        },
        reagent2ValveTest =
        {
            name = "Reagent2ValveTest",
            text = "试剂二阀测试",
        },
        standardValveTest =
        {
            name = "StandardValveTest",
            text = "量程校准液阀测试",
        },
        zeroCheckkValveTest =
        {
            name = "ZeroCheckkValveTest",
            text = "零点核查液阀测试",
        },
        rangeCheckValveTest =
        {
            name = "RangeCheckValveTest",
            text = "标样(跨度)核查液阀测试",
        },
        blankValveTest =
        {
            name = "BlankValveTest",
            text = "零点校准液阀测试",
        },
        wasteValveTest =
        {
            name = "WasteValveTest",
            text = "分析废液阀测试",
        },
        wasteWaterValveTest =
        {
            name = "WasteWaterValveTest",
            text = "清洗废水阀测试",
        },
        digestUpValveTest =
        {
            name = "DigestUpValveTest",
            text = "消解上阀测试",
        },
        digestBottomValveTest =
        {
            name = "DigestBottomValveTest",
            text = "消解下阀测试",
        },
        heaterTest =
        {
            name = "HeaterTest",
            text = "消解加热测试",
        },
        coolerTest =
        {
            name = "CoolerTest",
            text = "消解冷却测试",
        },
        measuerLEDTest =
        {
            name = "MeasuerLEDTest",
            text = "测量LED",
        },
        meter1LEDTest =
        {
            name = "Meter1LEDTest",
            text = "定量点一LED",
        },
        meter2LEDTest =
        {
            name = "Meter2LEDTest",
            text = "定量点二LED",
        },
        collectSampleRelayTest =
        {
            name = "CollectSampleRelayTest",
            text = "采水继电器",
        },
        relay1Test =
        {
            name = "Relay1Test",
            text = "继电器1",
        },
        relay2Test =
        {
            name = "Relay2Test",
            text = "继电器2",
        },
        sampleCurrent4Output =
        {
            name = "SampleCurrent4Output",
            text = "样品4mA输出",
        },
        sampleCurrent12Output =
        {
            name = "SampleCurrent12Output",
            text = "样品12mA输出",
        },
        sampleCurrent20Output =
        {
            name = "SampleCurrent20Output",
            text = "样品20mA输出",
        },
        systemFanTest =
        {
            name = "SystemFanTest",
            text = "机箱冷却",
        },
    },
}
return setting.runAction
