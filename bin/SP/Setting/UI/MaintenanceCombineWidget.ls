setting.ui.operation.maintainCombine =
{
    name ="maintainCombine",
    text= "维护组合",
    writePrivilege=  RoleType.Administrator,
    readPrivilege = RoleType.Administrator,
    {
        name = "MeasureSample",
        text= "测量水样",
        createFlow= function()
            local flow = MeasureFlow:new({}, MeasureType.Sample)
            flow.name = "MeasureSample"
            flow.isUseStart = true
            FlowList.AddFlow(flow)
        end,
    },
    {
        name = "Calibrate",
        text= "校准",
        createFlow= function()
            local flow = CalibrateFlow:new({}, CalibrateType.calibrate)
            flow.name  = "Calibrate"
            flow.isUseStart = true
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="MeasureStandard",
        text= "测量程校准液",
        createFlow= function()
            local flow = MeasureFlow:new({}, MeasureType.Standard)
            flow.name ="MeasureStandard"
            flow.isUseStart = true
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="MeasureBlank",
        text= "测零点校准液",
        createFlow= function()
            local flow = MeasureFlow:new({}, MeasureType.Blank)
            flow.name ="MeasureBlank"
            flow.isUseStart = true
            FlowList.AddFlow(flow)
        end,
    },

    ClearFlowList = function()
        log:debug("MaintainCombine ClearFlowList")
        FlowList.ClearList()
    end,
}
return setting.ui.operation.maintainCombine
