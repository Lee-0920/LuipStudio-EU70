config.scheduler =
{
    measure =		-- 测量排期
    {
        mode = _G.MeasureMode.Trigger,	-- 触发测量模式
        interval = 2,		-- 自动测量间隔周期
        timedPoint =		--
        {
            true, true, true, true, true,
            true, true, true, true, true,
            true, true, true, true, true,
            true, true, true, true, true,
            true, true, true, true,
        },
    },
    zeroCheck =		-- 零点核查
    {
        mode = _G.MeasureMode.Trigger,	-- 触发测量模式
        interval= 168,		-- 间隔周期
        timedPoint =		--
        {
            false, false, false, false, false,
            false, false, false, false, false,
            false, false, false, false, false,
            false, false, false, false, false,
            false, false, false, false,
        },
    },
    rangeCheck =		-- 量程核查
    {
        mode = _G.MeasureMode.Trigger,	-- 触发测量模式
        interval= 168,		-- 间隔周期
        timedPoint =		--
        {
            false, false, false, false, false,
            false, false, false, false, false,
            false, false, false, false, false,
            false, false, false, false, false,
            false, false, false, false,
        },
    },
    calibrate =		-- 校准排期
    {
        enable = false,		-- 是否开着
        mode = _G.MeasureMode.Trigger,	-- 触发测量模式
        interval = 168,		            -- 自动测量间隔周期
        oneTimedPoint = 8 ,		        -- 整点测量时间
        timedPointInterval = 7,         -- 整点测量间隔时间
        configChangeTime = _G.os.time(),-- 参数修改时间
    },
    clean =			-- 清洗排期
    {
        enable = false,		--
        interval= 168,		--
    },
    blankCheck =			-- 测零点校准液
    {
        enable = false,		--
        interval= 168,		--
    },
    check =			-- 测量程校准液排期
    {
        enable = false,		--
        interval= 168,		--
    },
    standard =			-- 测核查液排期
    {
        enable = false,		--
        mode = _G.MeasureMode.Trigger,	-- 触发测量模式
        interval= 168,		-- 间隔周期
        timedPoint =		--
        {
            false, false, false, false, false,
            false, false, false, false, false,
            false, false, false, false, false,
            false, false, false, false, false,
            false, false, false, false,
        },
    },
    parallel =			-- 平行排期
    {
        enable = false,		--
        interval= 168,		--
    },
    quickMeasure =		    -- 快速测量
    {
        mode = _G.MeasureMode.Trigger,	-- 触发测量模式
        interval = 2,		-- 自动测量间隔周期
        timedPoint =		--
        {
            false, false, false, false, false,
            false, false, false, false, false,
            false, false, false, false, false,
            false, false, false, false, false,
            false, false, false, false,
        },
    },
    timedPointJudgeTime = 120,-- 整点启动-延长时间
}
