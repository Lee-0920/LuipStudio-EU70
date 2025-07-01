setting.ui.operation.hardwareTest =
{
    name ="hardwareTest",
    text= "硬件测试",
    rowCount = 33,
    writePrivilege=  RoleType.Administrator,
    readPrivilege = RoleType.Administrator,
    superRow = 0,
    administratorRow = 33,
    {
        name = "PumpGroup",
        text = "泵组",
        {
            name ="TCPump",
            text= "TC泵正转",
            createFlow= function(action)
                -- print("MeterPump", action)
                return HardwareTest:execute(38, action, "TC泵正转")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="TCPump",
            text= "TC泵反转",
            createFlow= function(action)
                -- print("MeterPump", action)
                return HardwareTest:execute(39, action, "TC泵反转")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="ICPump",
            text= "IC泵正转",
            createFlow= function(action)
                -- print("MeterPump", action)
                return HardwareTest:execute(40, action, "IC泵正转")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="ICPump",
            text= "IC泵反转",
            createFlow= function(action)
                -- print("MeterPump", action)
                return HardwareTest:execute(41, action, "IC泵反转")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="DeionizedPump",
            text= "去离子水泵转",
            createFlow= function(action)
                -- print("MeterPump", action)
                return HardwareTest:execute(42, action, "去离子水泵转")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="ICRPump",
            text= "ICR泵",
            createFlow= function(action)
                -- print("MeterPump", action)
                return HardwareTest:execute(43, action, "ICR泵")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
    },
   --[[ {
        name = "SyringeGroup",
        text = "注射器",
        {
            name ="ResetReagentSyringePump",
            text= "注射泵排空酸剂",
            createFlow= function(action)
                -- print("MeterPump", action)
                return HardwareTest:execute(34, action, "注射泵排空酸剂")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="AddReagentSyringePump",
            text= "注射泵加酸剂",
            createFlow= function(action)
                -- print("MeterPump", action)
                return HardwareTest:execute(34, action, "注射泵加酸剂")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="ResetOxidantSyringePump",
            text= "注射泵排空氧化剂",
            createFlow= function(action)
                -- print("MeterPump", action)
                return HardwareTest:execute(34, action, "注射泵排空氧化剂")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="AddOxidantSyringePump",
            text= "注射泵加氧化剂",
            createFlow= function(action)
                -- print("MeterPump", action)
                return HardwareTest:execute(34, action, "注射泵加氧化剂")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
    },]]--
    {
        name = "ValveGroup",
        text = "阀组",
        {
            name ="TCValve",
            text= "TC阀",
            createFlow= function(action)
                --print("WasteValve", action)
                return HardwareTest:execute(32, action, "TC阀")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="ICValve",
            text= "IC阀",
            createFlow= function(action)
                --print("WasteValve", action)
                return HardwareTest:execute(33, action, "IC阀")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="ReagentValve",
            text= "酸剂阀",
            createFlow= function(action)
                --print("reagent1Valve", action)
                return HardwareTest:execute(34, action, "酸剂阀")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="OxidantValve",
            text= "氧化剂阀",
            createFlow= function(action)
                --print("reagent1Valve", action)
                return HardwareTest:execute(35, action, "氧化剂阀")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="ICRValve1",
            text= "ICR到联线阀",
            createFlow= function(action)
                --print("DigestRoomUpValve", action)
                return HardwareTest:execute(36, action, "ICR到联线阀")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="ICRValve2",
            text= "ICR到旁路阀",
            createFlow= function(action)
                --print("DigestRoomUpValve", action)
                return HardwareTest:execute(37, action, "ICR到旁路阀")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="LCValve1",
            text= "LC阀1",
            createFlow= function(action)
                --print("DigestRoomUpValve", action)
                return HardwareTest:execute(44, action, "LC阀1")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="LCValve2",
            text= "LC阀2",
            createFlow= function(action)
                --print("DigestRoomUpValve", action)
                return HardwareTest:execute(45, action, "LC阀2")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="LCValve3",
            text= "LC阀3",
            createFlow= function(action)
                --print("DigestRoomUpValve", action)
                return HardwareTest:execute(46, action, "LC阀3")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="LCValve4",
            text= "LC阀4",
            createFlow= function(action)
                --print("DigestRoomUpValve", action)
                return HardwareTest:execute(47, action, "LC阀4")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
    },
    {
        name = "MeasurementModule",
        text = "测量模块",
        {
            name ="MeasuerLED",
            text= "紫外灯",
            createFlow= function(action)
                -- print("MeasuerLED", action)
                return HardwareTest:execute(12, action, "紫外灯")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
    },
    {
        name = "MiningRelay",
        text = "继电器",
        {
            name ="Relay1",
            text= "继电器1",
            createFlow= function(action)
                -- print("Relay1", action)
                return HardwareTest:execute(27, action, "继电器1")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="Relay2",
            text= "继电器2",
            createFlow= function(action)
                -- print("Relay2", action)
                return HardwareTest:execute(28, action, "继电器2")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="Relay3",
            text= "继电器3",
            createFlow= function(action)
                -- print("Relay1", action)
                return HardwareTest:execute(29, action, "继电器3")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="Relay4",
            text= "继电器4",
            createFlow= function(action)
                -- print("Relay2", action)
                return HardwareTest:execute(30, action, "继电器4")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
    },
    {
        name = "SampleA4-20mA",
        text = "[TOC]4-20mA",
        {
            name ="SampleCurrent4Output",
            text= "4mA输出",
            analogConfig = true,
            createFlow= function(action)
                --   print("SampleCurrent4Output", action)
                return HardwareTest:execute(16, action, "[TOC]4mA输出")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="SampleCurrent12Output",
            text= "12mA输出",
            analogConfig = true,
            createFlow= function(action)
                --  print("SampleCurrent12Output", action)
                return HardwareTest:execute(17, action, "[TOC]12mA输出")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="SampleCurrent20Output",
            text= "20mA输出",
            analogConfig = true,
            createFlow= function(action)
                --print("SampleCurrent20Output", action)
                return HardwareTest:execute(18, action, "[TOC]20mA输出")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
    },
    {
        name = "SampleB4-20mA",
        text = "[TC]4-20mA",
        {
            name ="SampleCurrent4Output",
            text= "4mA输出",
            analogConfig = true,
            createFlow= function(action)
                --   print("SampleCurrent4Output", action)
                return HardwareTest:execute(19, action, "[TC]4mA输出")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="SampleCurrent12Output",
            text= "12mA输出",
            analogConfig = true,
            createFlow= function(action)
                --  print("SampleCurrent12Output", action)
                return HardwareTest:execute(20, action, "[TOC]12mA输出")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="SampleCurrent20Output",
            text= "20mA输出",
            analogConfig = true,
            createFlow= function(action)
                --print("SampleCurrent20Output", action)
                return HardwareTest:execute(21, action, "[TOC]20mA输出")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
    },
    {
        name = "SampleC4-20mA",
        text = "[IC]4-20mA",
        {
            name ="SampleCurrent4Output",
            text= "4mA输出",
            analogConfig = true,
            createFlow= function(action)
                --   print("SampleCurrent4Output", action)
                return HardwareTest:execute(22, action, "[IC]4mA输出")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="SampleCurrent12Output",
            text= "12mA输出",
            analogConfig = true,
            createFlow= function(action)
                --  print("SampleCurrent12Output", action)
                return HardwareTest:execute(23, action, "[IC]12mA输出")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="SampleCurrent20Output",
            text= "20mA输出",
            analogConfig = true,
            createFlow= function(action)
                --print("SampleCurrent20Output", action)
                return HardwareTest:execute(24, action, "[IC]20mA输出")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
    },
    {
        name = "TemperatureMonitoring",
        text = "温度监控",
        {
            name ="BoxUpFan1",
            text= "机箱风扇1",
            createFlow= function(action)
                -- print("SystemFanTest", action)
                return HardwareTest:execute(25, action, "机箱风扇1")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="BoxUpFan2",
            text= "机箱风扇2",
            createFlow= function(action)
                -- print("SystemFanTest", action)
                return HardwareTest:execute(26, action, "机箱风扇2")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name ="BoxUpFan3",
            text= "机箱风扇3",
            createFlow= function(action)
                -- print("SystemFanTest", action)
                return HardwareTest:execute(31, action, "机箱风扇3")
            end,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
    },
    checkOEM = function()
        return config.system.OEM
    end,
    isShowCheck = function()
        return true
    end,
}
return setting.ui.operation.hardwareTest
