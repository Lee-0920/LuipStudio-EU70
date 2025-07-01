setting.modbusCoder = {}

setting.modbusCoder.statusID=
{
    Idle =
    {
        ID = 0,
        jsID = 1,
        dbsID = 0,
        szbaID = 2048,
    },
    MeasureSample =
    {
        ID = 1,
        jsID = 4,
        dbsID = 1,
        szbaID = 1,
    },
    MulCalibrate =
    {
        ID = 2,
        jsID = 7,
        dbsID = 19,
        szbaID = 2,
    },
    Calibrate =
    {
        ID = 3,
        jsID = 7,
        dbsID = 19,
        szbaID = 2,
    },
    SinglePoint =
    {
        ID = 4,
        jsID = 7,
        dbsID = 19,
        szbaID = 2,
    },
    SystemAdaptability =
    {
        ID = 5,
        jsID = 7,
        dbsID = 19,
        szbaID = 2,
    },
    SterileWaterAdaptability =
    {
        ID = 6,
        jsID = 7,
        dbsID = 19,
        szbaID = 2,
    },
    Robustness =
    {
        ID = 6,
        jsID = 7,
        dbsID = 19,
        szbaID = 2,
    },
    Specificity =
    {
        ID = 7,
        jsID = 7,
        dbsID = 19,
        szbaID = 2,
    },
    Linear =
    {
        ID = 8,
        jsID = 7,
        dbsID = 19,
        szbaID = 2,
    },
    SdbsAdaptability =
    {
        ID = 9,
        jsID = 7,
        dbsID = 19,
        szbaID = 2,
    },
    LiquidOperate =
    {
        ID = 10,
        jsID = 5,
        dbsID = 1001,
    },
    SmartDetect =
    {
        ID = 11,
        jsID = 5,
        dbsID = 1001,
    },
    CommunicationCheck =
    {
        ID = 12,
        jsID = 5,
        dbsID = 1001,
    },
    HardwareTest =
    {
        ID = 13,
        jsID = 5,
        dbsID = 1001,
    },
    MASTER =
    {
        ID = 14,
        jsID = 5,
        dbsID = 1001,
    },
    DCUpdate =
    {
        ID = 15,
        jsID = 5,
        dbsID = 1001,
    },
    LCUpdate =
    {
        ID = 16,
        jsID = 5,
        dbsID = 1001,
    },
    RCUpdate =
    {
        ID = 17,
        jsID = 5,
        dbsID = 1001,
    },
    OCUpdate =
    {
        ID = 18,
        jsID = 5,
        dbsID = 1001,
    },
    CleanDeeply =
    {
        ID = 19,
        jsID = 3,
        dbsID = 10,
        szbaID = 4,
    },
    CombineOperate =
    {
        ID = 20,
        jsID = 5,
        dbsID = 1001,
    },
    OneKeyRenew =   --一键填充试剂
    {
        ID = 21,
        jsID = 3,
        dbsID = 10,
    },
    Fault =
    {
        ID = 255,
        jsID = 6,
        dbsID = 1000,
    },
}

setting.modbusCoder.actionID= {
    Idle =
    {
        ID = 0,
        dbsID = 0xffff,	--空闲
    },
    CleanBeforeMeasure =
    {
        ID = 1,
        dbsID = 12,	--测量传感器
    },
    ReadInitialAD =
    {
        ID = 2,
        dbsID = 13,	--读取基线值
    },
    Rinse =
    {
        ID = 3,
        dbsID = 3,	--润洗液位管
    },
    AddSample =
    {
        ID = 4,
        dbsID = 4,	--抽取样品
    },
    AddReagent1 =
    {
        ID = 5,
        dbsID = 5,	--抽取掩蔽剂
    },
    AddReagent2 =
    {
        ID = 6,
        dbsID = 6,	--抽取消解液
    },
    AddReagent3 =
    {
        ID = 7,
    },
    AddReagent4 =
    {
        ID = 8,
    },
    AddReagent5 =
    {
        ID = 9,
    },
    AddReagent6 =
    {
        ID = 10,
    },
    CleanMeterPipe =
    {
        ID = 11,
    },
    Preheating =
    {
        ID = 12,
    },
    Digest =
    {
        ID = 13,
        dbsID = 8,	--加热消解
    },
    DigestCooling =
    {
        ID = 14,
        dbsID = 10,	--加速冷却
    },
    ReadMeasureAD =
    {
        ID = 15,
        dbsID = 11,	--读取样品值
    },
    Clean =
    {
        ID = 16,
    },
    Cooling =
    {
        ID = 17,
    },
    Collect =
    {
        ID = 18,
    },
    Silent =
    {
        ID = 19,
    },
    AutoPumpCheck =
    {
        ID = 20,
    },
    SuckFromBlank =
    {
        ID = 21,
    },
    DrainToBlank=
    {
        ID = 22,
    },
    SuckFromSample =
    {
        ID = 23,
    },
    DrainToSample =
    {
        ID = 24,
    },
    SuckFromStandard =
    {
        ID = 25,
    },
    DrainToStandard =
    {
        ID = 26,
    },
    SuckFromReagent1 =
    {
        ID = 27,
    },
    DrainToReagent1 =
    {
        ID = 28,
    },
    SuckFromReagent2 =
    {
        ID = 29,
    },
    DrainToReagent2 =
    {
        ID = 30,
    },
    SuckFromReagent3 =
    {
        ID = 31,
    },
    DrainToReagent3 =
    {
        ID = 32,
    },
    SuckFromReagent4 =
    {
        ID = 33,
    },
    DrainToReagent4 =
    {
        ID = 34,
    },
    SuckFromReagent5 =
    {
        ID = 35,
    },
    DrainToReagent5 =
    {
        ID = 36,
    },
    SuckFromReagent6 =
    {
        ID = 37,
    },
    DrainToReagent6 =
    {
        ID = 38,
    },
    SuckFromDigestionRoom =
    {
        ID = 39,
    },
    DrainToDigestionRoom =
    {
        ID = 40,
    },
    DrainToWaste =
    {
        ID = 41,
    },
    LCCommunicationCheck =
    {
        ID = 42,
    },
    OACommunicationCheck =
    {
        ID = 43,
    },
    TCCommunicationCheck =
    {
        ID = 44,
    },
    SmartMeasureDetect =
    {
        ID = 47,
    },
    SmartBlankValveDetect =
    {
        ID = 49,
    },
    SmartSampleValveDetect =
    {
        ID = 50,
    },
    SmartStandardValveDetect =
    {
        ID = 51,
    },
    SmartReagent1ValveDetect =
    {
        ID = 52,
    },
    SmartReagent2ValveDetect =
    {
        ID = 53,
    },
    SmartReagent3ValveDetect =
    {
        ID = 54,
    },
    SmartReagent4ValveDetect =
    {
        ID = 55,
    },
    SmartReagent5ValveDetect =
    {
        ID = 56,
    },
    SmartReagent6ValveDetect =
    {
        ID = 57,
    },
    SmartDigestionRoomValveDetect =
    {
        ID = 58,
    },
    SmartWasteValveDetect =
    {
        ID = 59,
    },
    PumpTest =
    {
        ID = 60,
    },
    SampleValveTest =
    {
        ID = 61,
    },
    BlankValveTest =
    {
        ID = 62,
    },
    StandardValveTest =
    {
        ID = 63,
    },
    Reagent1ValveTest =
    {
        ID = 64,
    },
    Reagent2ValveTest =
    {
        ID = 65,
    },
    Reagent3ValveTest =
    {
        ID = 66,
    },
    Reagent4ValveTest =
    {
        ID = 67,
    },
    Reagent5ValveTest =
    {
        ID = 68,
    },
    Reagent6ValveTest =
    {
        ID = 69,
    },
    WasteValveTest =
    {
        ID = 70,
    },
    DigestUpValveTest =
    {
        ID = 71,
    },
    DigestBottomValveTest =
    {
        ID = 72,
    },
    HeaterTest =
    {
        ID = 73,
    },
    CoolerTest =
    {
        ID = 74,
    },
    MeasuerLEDTest =
    {
        ID = 75,
    },
    Meter1LEDTest =
    {
        ID = 76,
    },
    Meter2LEDTest =
    {
        ID = 77,
    },
    CollectSampleRelayTest =
    {
        ID = 78,
    },
    Relay1Test =
    {
        ID = 79,
    },
    Relay2Test =
    {
        ID = 80,
    },
    SampleCurrent4Output =
    {
        ID = 81,
    },
    SampleCurrent12Output =
    {
        ID = 82,
    },
    SampleCurrent20Output =
    {
        ID = 83,
    },
    CheckCurrent4Output =
    {
        ID = 84,
    },
    CheckCurrent12Output =
    {
        ID = 85,
    },
    CheckCurrent20Output =
    {
        ID = 86,
    },
    Copy =
    {
        ID = 87,
    },
    Updater =
    {
        ID = 88,
    },
    Erase =
    {
        ID = 89,
    },
    WriteProgram =
    {
        ID = 90,
    },
    DrainBeforeMeasure =
    {
        ID = 91,
        dbsID = 0,	--推空液位管
    },
    DrainAfterMeasure =
    {
        ID = 92,
        dbsID = 45,	--排废液
    },
    CleanAfterMeasure =
    {
        ID = 93,
        dbsID = 12,	--清洗检测池
    },
    React =
    {
        ID = 94,
        dbsID = 25,	--正在反应
    },
    SystemFanTest =
    {
        ID = 95,
    },
    ClearWaste =
    {
        ID = 96,
    },
    ClearBlankPipe =
    {
        ID = 97,
    },
    ClearStandardPipe =
    {
        ID = 98,
    },
    ClearReagent1Pipe =
    {
        ID = 99,
    },
    ClearReagent2Pipe =
    {
        ID = 100,
    },
    ClearReagent3Pipe =
    {
        ID = 101,
    },
    ClearReagent4Pipe =
    {
        ID = 102,
    },
    ClearReagent5Pipe =
    {
        ID = 103,
    },
    ClearReagent6Pipe=
    {
        ID = 104,
    },
    CleanSamplePipe =
    {
        ID = 106,
    },
    CleanStandardPipe =
    {
        ID = 107,
    },
    CleanBlankPipe =
    {
        ID = 108,
    },
    CleanReagent1Pipe =
    {
        ID = 109,
    },
    CleanReagent2Pipe =
    {
        ID = 110,
    },
    CleanReagent3Pipe =
    {
        ID = 111,
    },
    CleanReagent4Pipe =
    {
        ID = 112,
    },
    CleanReagent5Pipe =
    {
        ID = 113,
    },
    CleanReagent6Pipe =
    {
        ID = 114,
    },
    DrainSamplePipe =
    {
        ID = 115,
    },
    DrainStandardPipe =
    {
        ID = 116,
    },
    DrainBlankPipe =
    {
        ID = 117,
    },
    DrainReagent1Pipe =
    {
        ID = 118,
    },
    DrainReagent2Pipe =
    {
        ID = 119,
    },
    DrainReagent3Pipe =
    {
        ID = 120,
    },
    DrainReagent4Pipe =
    {
        ID = 121,
    },
    DrainReagent5Pipe=
    {
        ID = 122,
    },
    DrainReagent6Pipe=
    {
        ID = 123,
    },
    DrainWastePipe =
    {
        ID = 124,
    },
    ReadBlankAD =
    {
        ID = 125,
        dbsID = 12,	--计算测试结果
    },
    DCCommunicationCheck =
    {
        ID = 126,
    },
    RCCommunicationCheck =
    {
        ID = 127,
    },
    SuckBlankToDigestionRoom =
    {
        ID = 128,
    },
    SuckDigestionRoomToWaste =
    {
        ID = 129,
    },
    GasSealCheck =
    {
        ID = 130,
    },
    SuckDigestionRoomToWasteWater =
    {
        ID = 131,
    },
    DrainAfterReadBlankAD =
    {
        ID = 132,
    },
    SuckFromZeroCheck =
    {
        ID = 141,
    },
    DrainToZeroCheck =
    {
        ID = 142,
    },
    SuckFromRangeCheck =
    {
        ID = 143,
    },
    DrainToRangeCheck =
    {
        ID = 144,
    },
    SmartWasteWaterValveDetect =
    {
        ID = 145,
    },
    SmartZeroCheckValveDetect =
    {
        ID = 146,
    },
    SmartRangeCheckValveDetect =
    {
        ID = 147,
    },
    DrainZeroCheckPipe =
    {
        ID = 148,
    },
    DrainRangeCheckPipe =
    {
        ID = 149,
    },
    DrainWasteWaterPipe =
    {
        ID = 150,
    },
    MeterADAdjust =
    {
        ID = 151,
    },
    MeasureADAdjust =
    {
        ID = 152,
        dbsID = 53,	--抽取清水
    },
    OneKeyHeating =
    {
        ID = 153,
    },
    WaittingResult =
    {
        ID = 156,
    },
}

setting.modbusCoder.detectResultID=
{
    idle = 0,
    detecting = 1,
    passed = 2,
    fail = 3,
    stop = 4,
}



return setting.modbusCoder
