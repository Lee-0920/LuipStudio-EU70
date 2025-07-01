setting.ui.runStatus =
{
    reportModeList =
    {
        "   运行 ",
        "   离线 ",
        "   维护 ",
        "   故障 ",
        "   校准 ",
        "   调试 ",
    },
    targets =
    {
        {
            name = "TOC",   --TOC模式下显示测量类型

            getProformaData = function()
                local consistency = status.measure.proformaResult.measure.consistency

                return consistency
            end,

            getData = function()
                local dateTime = status.measure.newResult.measure.dateTime
                local consistency = status.measure.newResult.measure.consistency

                return dateTime, consistency
            end,

            getDataTC = function()
                local dateTime = status.measure.newResult.measure.dateTime
                local consistency = status.measure.newResult.measure.consistencyTC

                return dateTime, consistency
            end,

            getDataIC = function()
                local dateTime = status.measure.newResult.measure.dateTime
                local consistency = status.measure.newResult.measure.consistencyIC

                return dateTime, consistency
            end,

            getResultType = function()
                local resultType  = status.measure.newResult.measure.resultType
                return resultType
            end,
        },
    },
    mulCalibrate = function()
        log:debug("RunStatusWindow createFlow ==> CalibrateFlow:mulCalibrate")
        local flow = CalibrateFlow:new({}, CalibrateType.mulCalibrate)
        flow.name = "mulCalibrate"
        FlowList.AddFlow(flow)
    end,

    UseExpandName = function()
        return false
    end,

    WeepingDetectTempValue = 0,
    WeepingDetectHandle = function(value)
        -- @DSCP_EVENT_DSI_CHECK_LEAKING_NOTICE
        if 0 ~= setting.ui.runStatus.WeepingDetectTempValue and
                true == config.system.adcDetect[1].enable and
                value <= setting.ui.runStatus.WeepingDetectTempValue*(config.system.weepingLimitValve/100) then
            log:debug("Weeping Detect! "..value)
            local alarm = Helper.MakeAlarm(setting.alarm.instrumentWeeping, "")
            AlarmManager.Instance():AddAlarm(alarm)
            FlowManager.Instance():StopFlow()
        end

        if value ~= 0 then
            setting.ui.runStatus.WeepingDetectTempValue = value
        else
            setting.ui.runStatus.WeepingDetectTempValue = 1
        end
    end,

    getOpticalData = function()
        local data = 0
        local num = 0
        local ScanLen = rc:GetScanLen()
        if ScanLen ~= nil then
            num = ScanLen
        end

        local ScanData = rc:GetScanData(num - 1)
        if ScanData ~= nil then
            data = ScanData
        end

        return  data
    end,
    unitChange = function(value, changeType) return ArguUnitChange(value, changeType) end,
}
return setting.ui.runStatus
