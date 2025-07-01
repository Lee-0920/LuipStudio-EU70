setting.adc =
{
    -- type = 1
    {
        --name = "WeepingDetect",
        --text = "漏液指示",
        --fileName = "/sys/bus/iio/devices/iio:device0/in_voltage4_raw",
        --filterNum = 10,
        --intervalTime = 100,
        --waitTime = 5000,
        --limitAD = 3640,        -- < 4095/1.8 *1.6;
        --compareType = _G.CompareType.LT,
        --detectHandle = function()
        --    log:debug("Weeping Detect!")
        --    local alarm = Helper.MakeAlarm(setting.alarm.instrumentWeeping, "")
        --    AlarmManager.Instance():AddAlarm(alarm)
        --
        --    FlowManager.Instance():StopFlow()
        --end,
    },
    -- type = 2
    {
        name = "MeasureDetect",
        text = "测量触发",
        fileName = "/sys/bus/iio/devices/iio:device0/in_voltage5_raw",
        filterNum = 5,
        intervalTime = 100,
        waitTime = 500,
        limitAD = 2000,
        compareType = _G.CompareType.LT,
        detectHandle = function()
            log:debug("Measure Detect!")
            local flowManager = FlowManager.Instance()

            if flowManager:IsAuthorize() == true and config.interconnection.reportMode ~= _G.ReportMode.Maintain then
                local flow = MeasureFlow:new({}, MeasureType.Sample)
                FlowList.AddFlow(flow)
                if flowManager:IsFlowEnable() == true then
                    flowManager:StartFlow()
                end
            end
        end,
    },
}
