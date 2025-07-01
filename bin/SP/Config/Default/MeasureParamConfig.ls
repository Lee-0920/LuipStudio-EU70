config.measureParam =
{                                                       -- 测量参数设置
    currentRange                = 0,                    -- 当前量程（三量程）
    calibrateRangeIndex         = 0,                    -- 校准
    zeroCheckRangeIndex         = 0,                    -- 零点核查量程
    rangeCheckRangeIndex        = 0,                    -- 量程核查量程
    range =
    {
        1,                                              -- 量程一
        2,                                              -- 量程二
        3,                                              -- 量程三
    },
    timeStr = "--",                                     -- 当前量程校准时间
    ZeroConsistency = 0,                                -- 当前量程零点校准液浓度
    RangeConsistency = 0,                               -- 当前量程量程校准液浓度
    curveK = 0,                                         -- 当前量程曲线斜率
    curveB = 0,                                         -- 当前量程曲线截距
    curveKTurbo = 1,                                  -- Turbo TC曲线斜率
    curveBTurbo = 0,                                    -- Turbo 曲线截距
    zeroOffsetArea = 0,                                 -- 零点偏移面积
    zeroAreaFactor = 1,                                 -- 零点面积因子
    syringReactVolume = 0.04,                           -- 当前量程注射器加样体积
    currentRangeIndex = 1,                              -- 当前量程
    autoChangeRange = false,                             -- 量程自动切换
    accurateMeasure = false,                             --精准测量模式
    accurateMeasureDeviation = 0,                       -- 精准测量偏差阈值
    absCalibrateMode = false,                           -- 绝对浓度模式
    activeRangeMode = 0,                                -- 量程自动切换后生效模式
    reactTemperature = 25,                             -- 反应温度
    reactTime = 90,                                     -- 反应时间
    windowTime = 30,
    temperatureIncrement = 0,                         --温度增量(加试剂二)
    negativeRevise = true,                              -- 负值修正
    shiftFactor = 0,                                    -- 平移因子
    reviseFactor = 1,                                   -- 修正因子
    meterLedAD =
    {
        3500,
        3500,
    },
    measureLedAD =
    {
        reference = 3.75,
        measure = 3.75,
        offsetPercent = 0.05,
        timeout = 30,
    },

    pidCtrl = true,                                     -- PID调光
    calibratePointConsistency =                         -- 标定浓度
    {
        0.0,                                            -- 标定0的浓度
        0.25,                                          -- 标定1的浓度
        1.0,                                            -- 标定2的浓度
        5.0,                                          -- 标定3的浓度
        10.0,                                            -- 标定4的浓度
        25.0,                                          -- 标定5的浓度
        50.0,                                            -- 标定6的浓度
    },
    turboCalibratePointConsistency =                         -- 标定浓度
    {
        0.0,                                            -- 标定0的浓度
        0.25,                                          -- 标定1的浓度
        0.5,                                            -- 标定2的浓度
        1.0,                                          -- 标定3的浓度
        3.0,                                            -- 标定4的浓度
        5.0,                                          -- 标定5的浓度
    },
    range2StandardConsistency = 500,                       -- 量程二校正液浓度
    range3StandardConsistency = 2500,                       -- 量程三校正液浓度
    range4StandardConsistency = 500,                       -- 量程4校正液浓度
    range5StandardConsistency = 2500,                       -- 量程5校正液浓度
    range6StandardConsistency = 5000,                       -- 量程6校正液浓度
    range7StandardConsistency = 10000,                       -- 量程7校正液浓度
    cleanBefMeaBlankVol = 0,       -- 测量前清洗，零点校准液体积
    cleanAftMeaBlankVol = 0,       -- 测量后清洗，零点校准液体积
    wasteWaterEnvironment = false,                           -- 污水环境
    highSaltMode = false,          -- 高盐模式
    extendSamplePipeVolume = 0,     -- 水样管延长体积
    sampleRenewVolume = 0,           -- 样品更新体积
    zeroAccurateCalibrate = false,                      -- 零点精准校准
    standardAccurateCalibrate = false,                  -- 标点精准校准
    rangeAccurateCalibrate = false,                     -- 量程精准校正
    curveQualifiedDetermination = false,                -- 标线合格判定
    rangeCalibrateDeviation = 0,                        -- 量程精准校正偏差阈值
    checkConsistency = 100,                             -- 标样核查浓度
    checkErrorLimit = 0.1,                              -- 标样核查偏差限值
    checkReporting = true,
    failAutoRevise = false,                             -- 核查失败自动校准
    accurateCheck = false,                               -- 精准核查
    highClMode = false,                                 -- 高氯模式
    readInitRilentTime = 120,                           -- 读初始值前静置时间
    heaterMaxDutyCycle = 0.2,                           -- 加热丝最大占空比
    bobbleTime = 50,                                    -- 吹气时间
    methodName = "sample",                              -- 方法名称
    methodCreateTime = 0,                               -- 方法创建时间
    methodCreateTimeStr = "--",                         -- 方法创建时间戳
    meaType = 0,                                        -- 方法类型 0-在线 1-离线
    turboMode = false,                                  -- turbo模式
    ICRMode = false,                                    -- IC去除功能
    TOCMode = true,                                     -- 测量TOC
    ECMode  = false,                                    -- 测量电导率
    autoReagent = false,                                -- 自动加试剂
    isUseUVLamp = true,                                 -- 紫外灯控制
    reagent1Vol = 1,                                    -- 试剂1体积 单位：ul/min
    reagent2Vol = 1,                                    -- 试剂2体积 单位：ul/min
    sampleQuickSpeed = 1,                               -- 快速冲洗流速 单位：ul/min
    sampleSlowSpeed = 0.5,                              -- 慢速冲洗流速 单位：ul/min
    quickRefreshTime = 120,                             -- 快速冲洗时间
    normalRefreshTime = 90,                             -- 正常冲洗时间
    measureTimes = 4,                                   -- 测量次数
    rejectTimes = 1,                                    -- 舍弃次数
    mixSampleTime = 90,                                 -- 混合水样时间
    TCConstant = 1,                                     -- TC电导池常数
    ICConstant = 1,                                     -- IC电导池常数
    TCCurveK = 1,                                       -- TC单点校准斜率
    ICCurveK = 1,                                       -- IC单点校准斜率
    TCTurboCurveK = 1,                                  -- TC单点校准斜率
    ICTurboCurveK = 1,                                  -- IC单点校准斜率
    curveParam =                                        -- 曲线参数
    {
        {
            curveK = 1,                                     -- 量程一曲线斜率
            curveB = 0,                                     -- 量程一曲线截距
            ZeroConsistency = 0,
            RangeConsistency = 16,
            timeStr = "--",
        },
        {
            curveK = 1,                                     -- 量程二曲线斜率
            curveB = 0,                                     -- 量程二曲线截距
            ZeroConsistency = 0,
            RangeConsistency = 80,
            timeStr = "--",
        },
        {
            curveK = 1,                                     -- 量程三曲线斜率
            curveB = 0,                                     -- 量程三曲线截距
            ZeroConsistency = 0,
            RangeConsistency = 400,
            timeStr = "--",
        },
        {
            curveK = 1,                                     -- 量程四曲线斜率
            curveB = 0,                                     -- 量程四曲线截距
            ZeroConsistency = 0,
            RangeConsistency = 1600,
            timeStr = "--",
        },
        {
            curveK = 1,                                     -- 量程五曲线斜率
            curveB = 0,                                     -- 量程五曲线截距
            ZeroConsistency = 0,
            RangeConsistency = 4000,
            timeStr = "--",
        },
    },
    reviseParameter =                                   -- 内部校正参数
    {
        _G.setting.measure.range[1].excursion,
        _G.setting.measure.range[2].excursion,
        _G.setting.measure.range[3].excursion,
        _G.setting.measure.range[4].excursion,
        _G.setting.measure.range[5].excursion,
    },
    measureDataOffsetValve = 0,
    pharmacopoeia =                                 --药典测试
    {
        false,
        false,
        false,
        false,
        false,
        false,
        false,
    }
}
