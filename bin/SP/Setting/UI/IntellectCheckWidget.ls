setting.ui.diagnosis = {}
setting.ui.diagnosis.smartDetect =
{
    name ="smartDetect",
    text= "智能诊断",
    rowCount = 1,
    writePrivilege=  RoleType.Administrator,
    readPrivilege = RoleType.Administrator,
    superRow = 0,
    administratorRow = 1,
    {-- 11
        name ="SmartMeasureDetect",
        text= "测量模块",
        createFlow= function()
            local flow = SmartMeasureDetectFlow:new()
            flow.name  = setting.ui.diagnosis.smartDetect[1].name
            flow.text = "测量模块"
            FlowList.AddFlow(flow)
        end,
        writePrivilege=  RoleType.Administrator,
        readPrivilege = RoleType.Administrator,
    },
}
