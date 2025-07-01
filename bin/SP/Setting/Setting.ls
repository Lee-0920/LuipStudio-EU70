require("Version")

setting.unitVolume = 1

MeaType =
{
    Online = 0,
    Offline = 1,
}

ModelType =
{
    TOC = 0,
    TC = 1,
    IC = 2,
}

PointNum =
{
    First = 1,
    Second = 2,
}

Target =
{
    TOC = 1,
}

ResultMark =
{
    N = 0,      --正常/在线
    B = 1,      --离线
    M = 2,      --维护
    D = 3,      --故障
    C = 4,      --校准
    E = 5,      --超量程
    T = 6,      --超上限
    K = 7,      --质控
    A = 8,      --调试
}

setting.unit =
{
    valid = true,
    {
        text = "ppm",
        multiple = 1,
    },
    {
        text = "ppb",
        multiple = 1000,
    },
}

setting.instrument =
{
    name = "总有机碳水质自动在线监测仪",
    argus =
    {
        "总有机碳",
    },
    deviceTypeNum =           -- 设备参数类型号
    {
        TOC = 1020,  --15
    },
    oldTypeNum =           -- 旧设备参数类型号
    {
        TOC = 15,
    },
    typeChanges =     -- 型码更改
    {
        {
            type = "EU70",
            ShowDetails = "(有屏软件)",
        },
        {
            type = "PT65",
            ShowDetails = "(有屏软件)",
        },
        {
            type = "PT70",
            ShowDetails = "(有屏软件)",
        },
        {
            type = "EU71",
            ShowDetails = "(无屏软件)",
            bindCmd = "echo [vnc]=1 >/mnt/datadisk/Data/DeviceParam",
            unBindCmd = "echo [vnc]=0 >/mnt/datadisk/Data/DeviceParam",
            isReStart = true;
        },
    },
    pharmacopoeiaName =
    {
        "ChP",
        "USP",
        "USP无菌水",
        "EP",
        "IP",
        "JP",
        "KP",
    },
}

setting.liquid = --液路相关参数
{
    meterLimit = --定量限值参设置--保留
    {
        sampleLimitVolume = 8, 			-- 水样定量的最大限值
        blankLimitVolume = 8,			-- 零点校准液定量的最大限值
        reagentLimitVolume = 8,			-- 试剂定量的最大限值
        digestionRoomLimitVolume = 5,		-- 消解室定量的最大限值
        pumpTimeoutFactor = 60000,		-- 泵操作结果等待超时时间系数,单位为s/mL
        meterTimeoutFactor = 30000,     -- 精确定量操作结果等待超时时间系数,单位为s/mL
        syringTimeoutFactor = 12000,     -- 精确定量操作结果等待超时时间系数,单位为s/mL
    },
    reagent1Volume = 0.5,			-- 试剂1体积，单位mL
    reagent2Volume = 0,			-- 试剂2体积，单位mL
    sampleMeterPoint = 1,			-- 水样精确定量点，1表示第一个定量点
    standardMeterPoint = 1,			-- 量程校准液精确定量点，1表示第一个定量点
    zeroCheckMeterPoint = 1,			-- 零点核查液精确定量点，1表示第一个定量点
    rangeCheckMeterPoint = 1,			-- 量程核查液精确定量点，1表示第一个定量点
    blankMeterPoint = 1,			-- 零点校准液精确定量点，1表示第一个定量点
    reagent1MeterPoint = 1,			-- 试剂1精确定量点，1表示第一个定量点
    reagent2MeterPoint = 1,			-- 试剂2精确定量点，1表示第一个定量点
    digestionRoomMeterPoint = 2,    -- 消解混合液精确定量点，1表示第一个定量点
    samplePipeVolume = 5,			-- 水样管体积，单位mL
    standardPipeVolume = 4,			-- 量程校准液管体积，单位mL
    blankPipeVolume = 4,			-- 空白液液管体积，单位mL
    zeroCheckPipeVolume = 4,			-- 零点核查液管体积，单位mL
    rangeCheckPipeVolume = 4,			-- 量程核查液管体积，单位mL
    reagent1PipeVolume = 4,			-- 试剂1管体积，单位mL
    reagent2PipeVolume = 4,			-- 试剂2管体积，单位mL
    meterPipeVolume = 6,			-- 定量管体积，单位mL
    multipleValvePipeVolume = 1,	-- 多联阀体积，单位mL
    digestionRoomPipeVolume = 2,		    -- 消解室管路体积，单位mL
    wastePipeVolume = 1,			            -- 定量分析废液管体积，单位mL
    wasteWaterPipeVolume = 2,       -- 定量清洗废水管体积，单位mL
    pumpAirVolume = 4.5,			-- 冒泡的体积，单位mL
    pumpAirSpeed = 0.6,				-- 冒泡的速度，单位mL/S
    pumpAirStepSpeed = 400,			-- 冒泡的转速度，单位步/S
    liquidSecludeVolume = 0.2,	    -- 管道隔离的体积，单位mL
    liquidSecludeSpeed = 0.1,		-- 管道隔离的速度，单位mL/S
    meterPointMaxNum = 2,			            -- 最大定量点数目
    slowMeterSpeed = 20,				-- 慢速定量速度,单位ml/s
    meterSpeed = 200,				-- 定量速度,单位ml/s
    drainSpeed = 300,				-- 排液定量速度,单位ml/s
    prefabricateDrainSpeed = 400,	-- 预制的排液定量速度,单位ml/s
    syringeDrainSpeed = 1040,        --注射器最大排液速度,单位ml/s
    syringeSlowDrainSpeed = 800,    --
    meterFailedReTry = 3,			-- 定量失败重试次数
    meterOverflowReTry = 3,			-- 定量溢出重试次数
    meterUnfinishedReTry = 3,		-- 定量未达成目标重试次数
    meterAirBubbleReTry = 6,		-- 定量有气泡重试次数
    meterTimeoutReTry = 3,			-- 定量超时重试次数
    meterCleanVolume = 3,			-- 定量故障后，零点校准液清洗体积
    readInitADBlankVolume = 4,		        -- 读初始值时零点校准液体积，单位mL
    pipeRenewVolume = 2,            -- 管路更新体积
    ropinessMeterOverValue = 150,   -- 分层/粘稠定量过冲程度
    accurateMeterOverValue = 40,    -- 精确定量过冲程度
    reagent1MeterOverValue = 100,    -- 精确定量过冲程度
    syringeMaxVolume = 300,          --注射器最大抽液体积,单位ul
    syringeCleanVolume = 300,        --注射器更新体积,单位ul
    syringeResetVolume = 0,          --注射器排空体积
    syringeUndoVolume = 50,        --防挂液排液体积
    syringeRelifeVolume = 0.03,      --催化剂再生排液体积
    syringeUpdateUpperLimit = 12,     --注射器最大更新次数
    syringeSuckAcceleration = 0.1,     --注射器抽液加速度
    cleanAftMeaBlankVol = setting.unitVolume * 4,   --测量后清洗体积
}

setting.signal =	--信号相关
{
    readADTime = 10,				-- 读AD值的时间,单位为秒
    readDarkADTime = 0.5,			-- 读暗电流AD值的时间,单位为秒
    dryFireReadADTime = 0.5,        -- 干烧检测读AD值时间，单位为秒
}

setting.temperature  =	--温度相关
{
    readInitADTemp = 50,			-- 读初始值的温度,单位为℃
    digestTempTimeout = 3600,		-- 加热到消解温度的超时时间,单位为秒
    coolTempTimeout = 900,          --制冷模块超时时间
    NDIRTempTimeout = 900,          --NDIR加热超时时间
    reactTempTimeout = 1800,			-- 加热到反应温度的超时时间,单位为秒
    naturalCoolingTimeout = 10000,	-- 加热后再自然冷却时间,单位为秒
    digestToReactCoolingTimeout = 600, -- 消解到反应冷却时间,单位为秒
    naturalCoolingTempValue = 2,	-- 消解冷却温度到反应温度的自然冷却温度差,单位为℃
    preheating = 60,				-- 加浓硫酸前预热温度,单位为℃
    cleanDigestTemp = 500,			-- 清洗消解器的温度,单位为℃
    digestSafetyTemp = 500,			-- 消解室安全温度,单位为℃
    insideEnvironmentTemp = 40,		        -- 机箱内部环境温度,单位为℃
    tempMonitorControlPeriod = 5,         --温度监测频率
    isOpenTempMonitor = true,               --是否打开恒温监测
    toleranceTemp = 1,				-- 容差温度，与目标温度的差值在该参数范围内即认为到达目标温度
    reviseToleranceTemp = 10,        -- 燃烧炉显示温度修正范围
    coolToleranceTemp = 0.15,          -- 制冷容差温度
    NDIRToleranceTemp = 0.1,          -- NDIR容差温度
    smartHeatDetectTemp = 10,		        -- 加热模块智能诊断上升温度,单位为℃
    smartHeatDetectTime = 600,		        -- 加热模块智能诊断加热时间,单位为秒
    smartCoolDetectTime = 180,		        -- 加热模块智能诊断加热时间,单位为秒
    oneKeyHeatingTemp = 90,         --一键加热消解室的加热目标温度,单位为℃
    oneKeyHeatingTempTimeout = 300, --一键加热的加热超时时间,单位为秒
    oneKeyHeatingCoolingTemp = 40,  --一键加热消解室的冷却目标温度,单位为℃
    oneKeyHeatingCoolingTempTimeout = 1200,  --一键加热的冷却超时时间,单位为秒
    digestTemperatureLimitTop = 800,    -- 消解温度输入上限,单位为℃
    reactTemperatureLimitTop = 780,      -- 反应温度输入上限,单位为℃
    checkTemperatureTolerance = 780,     --温度检查容忍值
    indexStoveFan = 3,                  --电炉风扇
    indexRefrigeratorFan = 2,           --制冷器风扇
    indexReserveFan = 5,                --预留风扇
    indexStove = 0,                 --燃烧炉
    indexRefrigerator = 1,          --制冷器
    indexNDIR = 2,                  --红外检测器
    temperatureRefrigerator = 0,    --制冷温度
    temperatureNDIR = 1,            --红外检测器温度
    temperatureBoxLow = 2,          --下机箱温度
    boxUpFan = 1,                   --上机箱风扇
    boxDownFan = 0,                 --下机箱风扇
    presstureTemperatureUpLimit = 50, --压力补偿上限温度
    presstureTemperatureLowLimit = 5, --压力补偿下限温度
    monitor =
    {
        time = 0,                           --监测时长
        stoveAlarm = false,                 --燃烧炉温度异常告警
        stoveLastTemperature = 0,           --燃烧炉5分钟前温度
        stoveAbnormalTemperature = 0,       --燃烧炉5秒钟前温度
        stoveTimeout = 250,                 --燃烧炉监测周期
        refrigeratorAlarm = false,          --制冷器温度异常告警
        refrigeratorLastTemperature = 0,    --制冷器五分钟前温度
        refrigeratorAbnormalTemperature = 0,--制冷器五秒钟前温度
        refrigeratorTimeout = 1200,          --制冷器监测周期
        NDIRAlarm = false,                  --NDIR温度异常告警
        NDIRLastTemperature = 0,            --NDIR 5分钟前温度
        NDIRAbnormalTemperature = 0,        --NDIR 5秒钟前温度
        NDIRTimeout = 900,                  --NDIR监测周期
    },
}

setting.measureResult =
{
    decimalNum = 4,					-- 测量结果小数位//有效位数
    getDecimalNum = function()      -- 实际小数位数
        return config.system.decimalNum
    end,
    useNum = 3,						-- 测量结果有效位数
    quantifyLowLimit = 5,	        -- 定量下限
    measureUpLimit = 4000,         -- 测定上限
    measureLowLimit = 0.5,	        -- 测定下限
    zeroReviseValue = 0.001,		-- 零修正值
    faultValue = 0.002,				-- 错误值,作为错误标识
    normalMeasureTime = 3200,		-- 正常测量时间,单位为秒
    resultWaveRange = 0.02,			-- 结果波动范围
    zeroDrift = 5,                  -- 零点漂移
    rangeDrift = 5,                 -- 量程漂移
    absLimit = 1000,                -- 正常吸光度上限
    checkErrorLimit = 0.1,          -- 标样核查偏差限制
    negativeReviseBaseValue = 0.24,    -- 负值修正基准值
    negativeReviseWaveRange = 0.24,    -- 负值修正波动范围
    yTopValue = 3.9,                  -- 峰形图Y轴结束数值
    yBottomValue = 3.7,               -- 峰形图Y轴起始数值
    updaterPeriod = 1,                  --信号刷新周期1s，必须与上报的频率一致
    displayPeakHigh = 0.5,         --显示峰高  单位：V
    deviation = 0.0008,              --基线稳定判断范围 单位：V
    minDeviation = 0.0004,             --NDIR稳定值左侧峰波动下限
    endDeviation = 0.0004,              --NDIR稳定值右侧峰波动下限
    validCnt = 16,                  --终点合格次数
    baseLineNum = 30,               --基线判定时长 单位为秒
    areaStep = 10,                   --寻峰步长 单位为秒
    filterStep = 25,                 --滤波步长 = filterStep * 2 (前后各去n个作平均)
    throwNum = 3,                    --过滤值 = throwNum * 2
    zeroValidCnt = 250,             --零点终点合格次数
    areaLowLimit = -1,             --面积下限值
    defaultReactVolume = 0.04,      --默认反应体积
    reactVolume = {
        0.035,
        0.06,
        0.1,
    },
    zeroArea = {
        0.2,
        0.35,
        0.5,
    },
    continousModeParam =
    {
        currentMeasureCnt = 0,
        lastMeasureTime = 0,
        lastStartIndex = 0,
        lastAccuratepeak = 0,
        isfinishContinousMeasure = false,
    },
    isStartAccurateMeasure = false,
    isFinishAccurateMeasure = false,
    immediatelyResultHandle = false,
    isHighRangeTC = false,  --高量程标志
    isHighRangeIC = false,  --高量程标志
    startReactVolume = 0.04,
    displayPeakHighList =
    {
        0.5,
        0.1,
        0.05,
        0.01,
        0.005,
    },
    LedUpperLimit = 4.5,    -- LED信号上限
    LedLowerLimit = 1.0,    -- LED信号下限
    curve =
    {
        {
            curveKUpLimit = 1.8,               -- K值上限
            curveKLowLimit = 0.6,	            -- K值下限
            curveBUpLimit = 1,                 -- 截距上限
            curveBLowLimit = 0,	               -- 截距下限
        },
        {
            curveKUpLimit = 0.83,               -- K值上限
            curveKLowLimit = 0.47,	            -- K值下限
            curveBUpLimit = 1,                 -- 截距上限
            curveBLowLimit = 0,	               -- 截距下限
        },
        {
            curveKUpLimit = 0.18,               -- K值上限
            curveKLowLimit = 0.1,	            -- K值下限
            curveBUpLimit = 1,                 -- 截距上限
            curveBLowLimit = 0,	               -- 截距下限
        },
        {
            curveKUpLimit = 0.047,               -- K值上限
            curveKLowLimit = 0.027,	            -- K值下限
            curveBUpLimit = 0,                 -- 截距上限
            curveBLowLimit = 0,	               -- 截距下限
        },
        {
            curveKUpLimit = 0.018,               -- K值上限
            curveKLowLimit = 0.01,	            -- K值下限
            curveBUpLimit = 1,                 -- 截距上限
            curveBLowLimit = 0,	               -- 截距下限
        },
    },
    rsdLimited = 0.03,      --相对标准偏差上限
    sdLimited = 0.07,       --500ppb以下偏差上限
    hsdLimited = 0.05,      --大于500ppb偏差上限
    keyConsistency = 0.5,   --偏差分界线浓度
}

setting.common =
{
    zeroCalibrateWaveRange = 2,		-- 精准校准-零点波动范围
    standardCalibrateWaveRange = 20,-- 精准校准-标点波动范围
    dataCopyDir = "/mnt/udisk/EU70-TOC_",     -- 数据复制路径
    --dataCopyDir = "E:/PT63-CODC_",
    skipFlow = false,
    minRandomData = 0,
    maxRandomData = 1,
}

setting.measure = {} --测量参数设置
setting.measure.range  = --量程
{
    rangeNum = 5,					-- 量程的数量
    {--量程1
        viewRange = 20,			            -- 用于显示的量程值
        rangeWindowMin = 0,			        -- 量程切换窗口下限
        rangeWindowMax = 55,	        -- 量程切换窗口上限
        sampleVolume = setting.unitVolume * 2,			-- 水样体积，单位mL
        standardVolume = 0,			-- 量程校准液体积，单位mL
        zeroCheckVolume = 0,            -- 零点核查样体积，单位mL
        rangeCheckVolume = 0,            -- 量程核查样体积，单位mL
        blankVolume = 0,			-- 零点校准液体积，单位mL
        dilutionExtractVolume1 = 0,	-- 第1 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume1 = 0,-- 第1 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume2 = 0,	-- 第2 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume2 = 0,-- 第2 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume3 = 0,	-- 第3 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume3 = 0,-- 第3 次定量稀释加零点校准液体积，单位mL
        afterReagent1AddBlankVolume = 0,    -- 试剂一后加零点校准液体积，单位mL
        rinseEnable = true,			-- 润洗开关
        rinseSampleVolume = setting.unitVolume * 1,	-- 润洗水样体积，单位mL
        rinseStandardVolume = 0,	-- 润洗量程校准液体积，单位mL
        rinseZeroCheckVolume = 0,        -- 润洗零点核查样体积，单位mL
        rinseRangeCheckVolume = 0,        -- 润洗量程核查样体积，单位mL
        rinseBlankVolume = 0,		-- 润洗零点校准液体积，单位mL
        diluteFactor = 1.25,                     --稀释系数
        excursion = 1,
    },
    {--量程2
        viewRange = 100,			            -- 用于显示的量程值
        rangeWindowMin = 45,			-- 量程切换窗口下限
        rangeWindowMax = 110,	        -- 量程切换窗口上限
        sampleVolume = setting.unitVolume * 2,			-- 水样体积，单位mL
        standardVolume = 0,			-- 量程校准液体积，单位mL
        zeroCheckVolume = 0,            -- 零点核查样体积，单位mL
        rangeCheckVolume = 0,            -- 量程核查样体积，单位mL
        blankVolume = 0,			-- 零点校准液体积，单位mL
        dilutionExtractVolume1 = 0,	-- 第1 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume1 = 0,-- 第1 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume2 = 0,	-- 第2 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume2 = 0,-- 第2 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume3 = 0,	-- 第3 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume3 = 0,-- 第3 次定量稀释加零点校准液体积，单位mL
        afterReagent1AddBlankVolume = 0,    -- 试剂一后加零点校准液体积，单位mL
        rinseEnable = true,			-- 润洗开关
        rinseSampleVolume = setting.unitVolume * 2,	-- 润洗水样体积，单位mL
        rinseStandardVolume = 0,	-- 润洗量程校准液体积，单位mL
        rinseZeroCheckVolume = 0,        -- 润洗零点核查样体积，单位mL
        rinseRangeCheckVolume = 0,        -- 润洗量程核查样体积，单位mL
        rinseBlankVolume = 0,		-- 润洗零点校准液体积，单位mL
        diluteFactor = 1.25,                --稀释系数
        excursion = 1,
    },
    {--量程3
        viewRange = 500,			            -- 用于显示的量程值
        rangeWindowMin = 90,			-- 量程切换窗口下限
        rangeWindowMax = 220,	        -- 量程切换窗口上限
        sampleVolume = 0.5,			-- 水样体积，单位mL
        standardVolume = 0,			-- 量程校准液体积，单位mL
        zeroCheckVolume = 0,            -- 零点核查样体积，单位mL
        rangeCheckVolume = 0,            -- 量程核查样体积，单位mL
        blankVolume = setting.unitVolume * 2,			-- 零点校准液体积，单位mL
        dilutionExtractVolume1 = 0,	-- 第1 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume1 = 0,-- 第1 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume2 = 0,	-- 第2 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume2 = 0,-- 第2 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume3 = 0,	-- 第3 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume3 = 0,-- 第3 次定量稀释加零点校准液体积，单位mL
        afterReagent1AddBlankVolume = setting.unitVolume * 1,    -- 试剂一后加零点校准液体积，单位mL
        rinseEnable = true,			-- 润洗开关
        rinseSampleVolume = setting.unitVolume * 2,	-- 润洗水样体积，单位mL
        rinseStandardVolume = 0,	-- 润洗量程校准液体积，单位mL
        rinseZeroCheckVolume = 0,        -- 润洗零点核查样体积，单位mL
        rinseRangeCheckVolume = 0,        -- 润洗量程核查样体积，单位mL
        rinseBlankVolume = 0,		-- 润洗零点校准液体积，单位mL
        diluteFactor = 5.25,                --稀释系数
        excursion = 1,
    },
    {--量程4
        viewRange = 2000,			            -- 用于显示的量程值
        rangeWindowMin = 180,		-- 量程切换窗口下限
        rangeWindowMax = 1100,	        -- 量程切换窗口上限
        sampleVolume = 0.5,			-- 水样体积，单位mL
        standardVolume = 0,			-- 量程校准液体积，单位mL
        zeroCheckVolume = 0,            -- 零点核查样体积，单位mL
        rangeCheckVolume = 0,            -- 量程核查样体积，单位mL
        blankVolume = setting.unitVolume * 10,			-- 零点校准液体积，单位mL
        dilutionExtractVolume1 = 0,	-- 第1 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume1 = 0,-- 第1 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume2 = 0,	-- 第2 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume2 = 0,-- 第2 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume3 = 0,	-- 第3 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume3 = 0,-- 第3 次定量稀释加零点校准液体积，单位mL
        afterReagent1AddBlankVolume = setting.unitVolume * 1,    -- 试剂一后加零点校准液体积，单位mL
        rinseEnable = true,			-- 润洗开关
        rinseSampleVolume = setting.unitVolume * 1,	-- 润洗水样体积，单位mL
        rinseStandardVolume = 0,	-- 润洗量程校准液体积，单位mL
        rinseZeroCheckVolume = 0,        -- 润洗零点核查样体积，单位mL
        rinseRangeCheckVolume = 0,        -- 润洗量程核查样体积，单位mL
        rinseBlankVolume = 0,		-- 润洗零点校准液体积，单位mL
        diluteFactor = 22,           --稀释系数
        excursion = 1,
    },
    {--量程5
        viewRange = 5000,			            -- 用于显示的量程值
        rangeWindowMin = 900,		-- 量程切换窗口下限
        rangeWindowMax = 5500,	        -- 量程切换窗口上限
        sampleVolume = 0.5,			-- 水样体积，单位mL
        standardVolume = 0,			-- 量程校准液体积，单位mL
        zeroCheckVolume = 0,            -- 零点核查样体积，单位mL
        rangeCheckVolume = 0,            -- 量程核查样体积，单位mL
        blankVolume = setting.unitVolume * 10,			-- 零点校准液体积，单位mL
        dilutionExtractVolume1 = setting.unitVolume * 8,	-- 第1 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume1 = setting.unitVolume * 4,-- 第1 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume2 = 0,	-- 第2 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume2 = 0,-- 第2 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume3 =  0,	-- 第3 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume3 = 0,-- 第3 次定量稀释加零点校准液体积，单位mL
        afterReagent1AddBlankVolume = setting.unitVolume * 1,    -- 试剂一后加零点校准液体积，单位mL
        rinseEnable = true,			-- 润洗开关
        rinseSampleVolume = setting.unitVolume * 1,	-- 润洗水样体积，单位mL
        rinseStandardVolume = 0,	-- 润洗量程校准液体积，单位mL
        rinseZeroCheckVolume = 0,        -- 润洗零点核查样体积，单位mL
        rinseRangeCheckVolume = 0,        -- 润洗量程核查样体积，单位mL
        rinseBlankVolume = 0,		-- 润洗零点校准液体积，单位mL
        diluteFactor = 58.8,           --稀释系数
        excursion = 1,
    },
}

setting.calibrate = --标定参数设置
{
    {--定点0
        viewRange = 0,				-- 用于显示的量程值
        rangeWindowMin = 0,			-- 量程切换窗口下限
        rangeWindowMax = 0,			-- 量程切换窗口上限
        sampleVolume = 0,			-- 水样体积，单位mL
        standardVolume = 0,			-- 量程校准液体积，单位mL
        zeroCheckVolume = 0,            -- 零点核查样体积，单位mL
        rangeCheckVolume = 0,            -- 量程核查样体积，单位mL
        blankVolume = setting.unitVolume * 4,			-- 零点校准液体积，单位mL
        dilutionExtractVolume1 = 0,	-- 第1 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume1 = 0,-- 第1 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume2 = 0,	-- 第2 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume2 = 0,-- 第2 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume3 = 0,	-- 第3 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume3 = 0,-- 第3 次定量稀释加零点校准液体积，单位mL
        afterReagent1AddBlankVolume = 0,    -- 试剂一后加零点校准液体积，单位mL
        rinseEnable = true,			-- 润洗开关
        rinseSampleVolume = 0,		-- 润洗水样体积，单位mL
        rinseStandardVolume = setting.unitVolume * 2,	-- 润洗量程校准液体积，单位mL
        rinseZeroCheckVolume = 0,        -- 润洗零点核查样体积，单位mL
        rinseRangeCheckVolume = 0,        -- 润洗量程核查样体积，单位mL
        rinseBlankVolume = 0,		-- 润洗零点校准液体积，单位mL
        diluteFactor = 1,			-- 稀释系数
    },
    {--定点1
        viewRange = 0,				-- 用于显示的量程值
        rangeWindowMin = 0,			-- 量程切换窗口下限
        rangeWindowMax = 0,			-- 量程切换窗口上限
        sampleVolume = 0,			-- 水样体积，单位mL
        standardVolume = setting.unitVolume * 4,		-- 量程校准液体积，单位mL
        zeroCheckVolume = 0,            -- 零点核查样体积，单位mL
        rangeCheckVolume = 0,            -- 量程核查样体积，单位mL
        blankVolume = 0,			-- 零点校准液体积，单位mL
        dilutionExtractVolume1 = 0,	-- 第1 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume1 = 0,-- 第1 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume2 = 0,	-- 第2 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume2 = 0,-- 第2 次定量稀释加零点校准液体积，单位mL
        dilutionExtractVolume3 = 0,	-- 第3 次定量稀释抽取混合液体积，单位mL
        dilutionAddBlankVolume3 = 0,-- 第3 次定量稀释加零点校准液体积，单位mL
        afterReagent1AddBlankVolume = 0,    -- 试剂一后加零点校准液体积，单位mL
        rinseEnable = true,			-- 润洗开关
        rinseSampleVolume = 0,		-- 润洗水样体积，单位mL
        rinseStandardVolume = setting.unitVolume * 2,	-- 润洗量程校准液体积，单位mL
        rinseZeroCheckVolume = 0,        -- 润洗零点核查样体积，单位mL
        rinseRangeCheckVolume = 0,        -- 润洗量程核查样体积，单位mL
        rinseBlankVolume = 0,		-- 润洗零点校准液体积，单位mL
        diluteFactor = 1,			-- 稀释系数
    },
    --{--定点2
    --    viewRange = 0,			            -- 用于显示的量程值
    --    rangeWindowMin = 0,			-- 量程切换窗口下限
    --    rangeWindowMax = 0,	        -- 量程切换窗口上限
    --    sampleVolume = 0,			-- 水样体积，单位mL
    --    standardVolume = setting.unitVolume * 1,			-- 量程校准液体积，单位mL
    --    zeroCheckVolume = 0,            -- 零点核查样体积，单位mL
    --    rangeCheckVolume = 0,            -- 量程核查样体积，单位mL
    --    blankVolume = setting.unitVolume * 3,			-- 零点校准液体积，单位mL
    --    dilutionExtractVolume1 = setting.unitVolume * 2,	-- 第1 次定量稀释抽取混合液体积，单位mL
    --    dilutionAddBlankVolume1 = 0,-- 第1 次定量稀释加零点校准液体积，单位mL
    --    dilutionExtractVolume2 = 0,	-- 第2 次定量稀释抽取混合液体积，单位mL
    --    dilutionAddBlankVolume2 = 0,-- 第2 次定量稀释加零点校准液体积，单位mL
    --    dilutionExtractVolume3 = 0,	-- 第3 次定量稀释抽取混合液体积，单位mL
    --    dilutionAddBlankVolume3 = 0,-- 第3 次定量稀释加零点校准液体积，单位mL
    --    afterReagent1AddBlankVolume = setting.unitVolume * 1,    -- 试剂一后加零点校准液体积，单位mL
    --    rinseEnable = true,			-- 润洗开关
    --    rinseSampleVolume = 0,	-- 润洗水样体积，单位mL
    --    rinseStandardVolume = setting.unitVolume * 2,	-- 润洗量程校准液体积，单位mL
    --    rinseZeroCheckVolume = 0,        -- 润洗零点核查样体积，单位mL
    --    rinseRangeCheckVolume = 0,        -- 润洗量程核查样体积，单位mL
    --    rinseBlankVolume = 0,		-- 润洗零点校准液体积，单位mL
    --    diluteFactor = 4,                --稀释系数
    --},
    --{--定点3
    --    viewRange = 0,			            -- 用于显示的量程值
    --    rangeWindowMin = 0,		-- 量程切换窗口下限
    --    rangeWindowMax = 0,	        -- 量程切换窗口上限
    --    sampleVolume = 0,			-- 水样体积，单位mL
    --    standardVolume = setting.unitVolume * 1,			-- 量程校准液体积，单位mL
    --    zeroCheckVolume = 0,            -- 零点核查样体积，单位mL
    --    rangeCheckVolume = 0,            -- 量程核查样体积，单位mL
    --    blankVolume = setting.unitVolume * 1,			-- 零点校准液体积，单位mL
    --    dilutionExtractVolume1 = setting.unitVolume * 1,	-- 第1 次定量稀释抽取混合液体积，单位mL
    --    dilutionAddBlankVolume1 = setting.unitVolume * 1,-- 第1 次定量稀释加零点校准液体积，单位mL
    --    dilutionExtractVolume2 = setting.unitVolume * 1,	-- 第2 次定量稀释抽取混合液体积，单位mL
    --    dilutionAddBlankVolume2 = 0,-- 第2 次定量稀释加零点校准液体积，单位mL
    --    dilutionExtractVolume3 = 0,	-- 第3 次定量稀释抽取混合液体积，单位mL
    --    dilutionAddBlankVolume3 = 0,-- 第3 次定量稀释加零点校准液体积，单位mL
    --    afterReagent1AddBlankVolume = setting.unitVolume * 1,    -- 试剂一后加零点校准液体积，单位mL
    --    rinseEnable = true,			-- 润洗开关
    --    rinseSampleVolume = 0,	-- 润洗水样体积，单位mL
    --    rinseStandardVolume = setting.unitVolume * 2,	-- 润洗量程校准液体积，单位mL
    --    rinseZeroCheckVolume = 0,        -- 润洗零点核查样体积，单位mL
    --    rinseRangeCheckVolume = 0,        -- 润洗量程核查样体积，单位mL
    --    rinseBlankVolume = 0,		-- 润洗零点校准液体积，单位mL
    --    diluteFactor = 8,           --稀释系数
    --},
    --{--定点4
    --    viewRange = 0,			            -- 用于显示的量程值
    --    rangeWindowMin = 0,		-- 量程切换窗口下限
    --    rangeWindowMax = 0,	        -- 量程切换窗口上限
    --    sampleVolume = 0,			-- 水样体积，单位mL
    --    standardVolume = setting.unitVolume * 1,			-- 量程校准液体积，单位mL
    --    zeroCheckVolume = 0,            -- 零点核查样体积，单位mL
    --    rangeCheckVolume = 0,            -- 量程核查样体积，单位mL
    --    blankVolume = setting.unitVolume * 2,			-- 零点校准液体积，单位mL
    --    dilutionExtractVolume1 = setting.unitVolume * 2,	-- 第1 次定量稀释抽取混合液体积，单位mL
    --    dilutionAddBlankVolume1 = setting.unitVolume * 2,-- 第1 次定量稀释加零点校准液体积，单位mL
    --    dilutionExtractVolume2 = setting.unitVolume * 2,	-- 第2 次定量稀释抽取混合液体积，单位mL
    --    dilutionAddBlankVolume2 = 0,-- 第2 次定量稀释加零点校准液体积，单位mL
    --    dilutionExtractVolume3 =  0,	-- 第3 次定量稀释抽取混合液体积，单位mL
    --    dilutionAddBlankVolume3 = 0,-- 第3 次定量稀释加零点校准液体积，单位mL
    --    afterReagent1AddBlankVolume = setting.unitVolume * 1,    -- 试剂一后加零点校准液体积，单位mL
    --    rinseEnable = true,			-- 润洗开关
    --    rinseSampleVolume = 0,	-- 润洗水样体积，单位mL
    --    rinseStandardVolume = setting.unitVolume * 2,	-- 润洗量程校准液体积，单位mL
    --    rinseZeroCheckVolume = 0,        -- 润洗零点核查样体积，单位mL
    --    rinseRangeCheckVolume = 0,        -- 润洗量程核查样体积，单位mL
    --    rinseBlankVolume = 0,		-- 润洗零点校准液体积，单位mL
    --    diluteFactor = 18,           --稀释系数
    --},
    --{--定点5
    --    viewRange = 0,			            -- 用于显示的量程值
    --    rangeWindowMin = 0,		    -- 量程切换窗口下限
    --    rangeWindowMax = 0,	        -- 量程切换窗口上限
    --    sampleVolume = 0,			-- 水样体积，单位mL
    --    standardVolume = setting.unitVolume * 1,			-- 量程校准液体积，单位mL
    --    zeroCheckVolume = 0,            -- 零点核查样体积，单位mL
    --    rangeCheckVolume = 0,            -- 量程核查样体积，单位mL
    --    blankVolume = setting.unitVolume * 3,			-- 零点校准液体积，单位mL
    --    dilutionExtractVolume1 = setting.unitVolume * 3,	-- 第1 次定量稀释抽取混合液体积，单位mL
    --    dilutionAddBlankVolume1 = setting.unitVolume * 3,-- 第1 次定量稀释加零点校准液体积，单位mL
    --    dilutionExtractVolume2 = setting.unitVolume * 3,	-- 第2 次定量稀释抽取混合液体积，单位mL
    --    dilutionAddBlankVolume2 = 0,-- 第2 次定量稀释加零点校准液体积，单位mL
    --    dilutionExtractVolume3 =  0,	-- 第3 次定量稀释抽取混合液体积，单位mL
    --    dilutionAddBlankVolume3 = 0,-- 第3 次定量稀释加零点校准液体积，单位mL
    --    afterReagent1AddBlankVolume = setting.unitVolume * 1,    -- 试剂一后加零点校准液体积，单位mL
    --    rinseEnable = true,			-- 润洗开关
    --    rinseSampleVolume = 0,	-- 润洗水样体积，单位mL
    --    rinseStandardVolume = setting.unitVolume * 2,	-- 润洗量程校准液体积，单位mL
    --    rinseZeroCheckVolume = 0,        -- 润洗零点核查样体积，单位mL
    --    rinseRangeCheckVolume = 0,        -- 润洗量程核查样体积，单位mL
    --    rinseBlankVolume = 0,		-- 润洗零点校准液体积，单位mL
    --    diluteFactor = 32,           --稀释系数
    --},
}
setting.clean =	--清洗参数设置
{
    cleanSource = 1,
    cleanVolume = 6,
    cleanTime = 3,
    syringeCleanTime = 5,
}

setting.sensor =	--传感器索引
{
    syringeIndex = 0,
    reagent1SensorIndex = 1,
    reagent2SensorIndex = 0,
    pumpIndex = 2,  --与管路操作绑定电机一致
}

setting.pressure =	--信号相关
{
    indexExposureValve = 0,
    indexCarryPressureValve = 1,
    masterReleaseLimit = 10,         --总阀泄压值
    masterPressureMinLimit = 40,    --总阀压力下限告警值
    masterPressureMaxLimit = 400,   --总阀压力上限告警值
    WarningPressureTolerance = 5,   -- 压力报警值上下限 kPa
    pressureMaster = 0,     --总阀压力索引
    pressureExposure = 1,   --曝气压力索引
    pressureCarry = 2,      --载气压力索引
    delayTime = 2,          --关总阀延时
    maxValveVoltage = 2.5,  --比例阀最大输出电压
    minRangePressure = 30,  --基本量程载气压力
    otherRangePressure = 60,  --其他量程载气压力
}

setting.output =	--输出板
{
    currentA = 0,
    currentB = 1,
    currentC = 2,
    relay1 = 0,
    relay2 = 1,
    relay3 = 2,
    relay4 = 3,
}

setting.reagent =	--输出板
{
    reagent1Pump = 0, --酸泵
    OxidantPump = 1,  --氧化剂泵
    reagentMixSpeed = 0.1, --Ul/Min
}

package.path = [[?;?.lua;?.ls;?;./SP/Setting/?.ls;./SP/Setting/ResultManager/?.ls;./SP/Setting/UI/?.ls;./SP/Setting/Modbus/?.ls;./SP/Setting/Hj212/?.ls]]

require("MeasureFileFormat")
require("CalibrateFileFormat")
require("ResultFileInfo")
require("Alarm")
require("RunStatus")
require("RunAction")
require("ModeDefine")
require("MeasureData")
require("CalibrateCurve")
require("WarnManagerWidget")
require("SignalIterms")
require("MeasureParamItems")
require("AutoMeasureWidget")
require("CurveManagerWidget")
require("LiquidType")
require("MaterialType")
require("InterconnectionWidget")
require("ReagentManagerWidget")
require("UseResourceWidget")
require("HardwareParamIterms")
require("DeviceInformation")
require("FactoryTime")
require("WareData")
require("MeasureScheduler")
require("MaintenanceWidget")
require("MaintenanceCombineWidget")
require("PipeManngerWidget")
require("CombineOpsManagerWidget")
require("IntellectCheckWidget")
require("CommunicationTestWidget")
require("HardwareTestWidget")
require("UpdateWidget")
require("RunStatusWindow")
require("SystemConfigWidget")
require("ModbusCoder")
require("MeasureDataWindow")
require("MeasureDataPrintWidget")
require("ADCDetect")
require("PeakData")
require("Hj212Coder")

return setting
