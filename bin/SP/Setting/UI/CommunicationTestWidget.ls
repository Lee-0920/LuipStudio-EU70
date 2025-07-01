setting.ui.diagnosis.communicationCheck =
{
    name ="communicationCheck",
    text= "通信检测",
    rowCount = 4,
    writePrivilege=  RoleType.Administrator,
    readPrivilege = RoleType.Administrator,
    superRow = 0,
    administratorRow = 4,
    {
        text = "板卡通信",
        name = "BoardCommunication ",
        {
            name ="DCCommunicationCheck",
            text= "驱动板",
            createFlow = function()
				local flow = CommunicationCheckFlow:new({})
				flow.name  = setting.ui.diagnosis.communicationCheck[1][1].name
                flow.text = setting.ui.diagnosis.communicationCheck[1].name .. "驱动板"
				FlowList.AddFlow(flow)
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="LCCommunicationCheck",
            text= "液路板",
            createFlow = function()
                local flow = CommunicationCheckFlow:new({})
                flow.name  = setting.ui.diagnosis.communicationCheck[1][2].name
                flow.text = setting.ui.diagnosis.communicationCheck[1].name .. "液路板"
                FlowList.AddFlow(flow)
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="RCCommunicationCheck",
            text= "信号板",
            createFlow = function()
                local flow = CommunicationCheckFlow:new({})
                flow.name  = setting.ui.diagnosis.communicationCheck[1][3].name
                flow.text = setting.ui.diagnosis.communicationCheck[1].name .. "信号板"
                FlowList.AddFlow(flow)
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="OCCommunicationCheck",
            text= "输出板",
            createFlow = function()
                local flow = CommunicationCheckFlow:new({})
                flow.name  = setting.ui.diagnosis.communicationCheck[1][4].name
                flow.text = setting.ui.diagnosis.communicationCheck[1].name .. "输出板"
                FlowList.AddFlow(flow)
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
    },
}
return setting.ui.diagnosis.communicationCheck
