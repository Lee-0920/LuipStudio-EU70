status.running =
{
    isMeasuring = false, -- 是否正常测量，用于测量过程中断电或重启后重新测量
    isCorrectStopFlow = true,    -- 用于区分是否为掉电保护调用的停止流程
    lastMeasureTime = 0,
}
