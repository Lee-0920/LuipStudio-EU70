setting.ui.profile.system =
{
    text = "系统参数",
    name = "system",
    index = 6,
    updateEvent = UpdateEvent.ChangeSystemParam,
    rowCount = 25,
    superRow = 16,
    administratorRow = 19,
    isMeaParaml = false,
    writePrivilege=  RoleType.Administrator,
    readPrivilege = RoleType.Maintain,
    --1
    {
        name = "faultProtect",
        text = "仪器保护",
        {
            name = "faultBlocking",
            text = "故障停机",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name = "faultRetry",
            text = "异常重测",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name = "reagentLackWarn",
            text = "更换试剂提醒",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
    },
    --2
    {
    	name = "specialParameters",
        text = "特殊参数",
	    {
            name = "language",
            text = "语言",
            type = DataType.Option,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            options =
            {
                "中文简体",
                "中文繁體",
                "English",
            },
        },
        {
            name = "adcDetect[2].enable",
            text = "干接点触发",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name = "solidifyMeaParamFromUI",
            text = "界面固化测量参数",
            type = DataType.Bool,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
        {
            name = "solidifyMeaParamFromModbus",
            text = "Modbus固化测量参数",
            type = DataType.Bool,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
        {
            name = "displayMeaParamInGeneral",
            text = "开放测量参数",
            type = DataType.Bool,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
        {
            name = "changeParamLog",
            text = "参数更改记录",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name = "useQrencode",
            text = "二维码",
            type = DataType.Bool,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
        {
            name = "internationalSupported",
            text = "多语言支持",
            type = DataType.Bool,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
    },
    --3
    {
        name = "environmentTempMonitor",
        text = "智能温控",
        {
            name = "temperatureMonitor",
            text = "温控开关",
            type = DataType.Bool,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
        {
            name = "environmentTemperature",
            text = "机箱温度",
            type = DataType.Float,
            unit = "℃",
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.system.oneDecimalPattern(value) == true then
                    local val = tonumber(value)
                    if val > 50 or val <= 0 then
                        return string.format(config.system.environmentTemperature)
                    else
                        return value
                    end
                else
                    return string.format(config.system.environmentTemperature)
                end
            end,
        },
    },
    --4
    --[[{
        name = "dataReport",
        text = "数据上报",
        {
            name = "hj212Platform.hj212Type",
            text = "传输协议",
            type = DataType.Option,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            options =
            {
                "Modbus",
                "CCEP认证协议",
                "HJ212-2017-Wry",
                "HJ212-2005-Wry",
            },
        },
    },]]--
    --5
    {
        name = "dataReport",
        text = "Modbus通信",
        {
            name = "modbusTableType",
            text = "Modbus协议",
            type = DataType.Option,
            writePrivilege = RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            options =
            {
                "LS1.0",
                "PC1.0",
            },
        },
    },
    --6
    --[[{
        name = "communicationMode",
        text = "HJ212通信",
        {
            name = "hj212Platform.hj212CommunicationMode",
            text = "模式",
            type = DataType.Option,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            options =
            {
                "RS232",
                "RS485",
                "TCP",
            },
        },
        {
            name = "hj212Platform.hj212DataType",
            text = "上报数据",
            type = DataType.Option,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            options =
            {
                "小时数据",
                "分钟数据",
                "实时数据",
            },
        },
        {
            name = "hj212Platform.heartPackTime",
            text = "上报间隔",
            type = DataType.Int,
            unit = "分",
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            isSolidifyFromUI = true,
            checkValue = function(value)
                if setting.ui.profile.system.valuePattern(value) == true then
                    local num = tonumber(value)
                    if num <= 0 or num > 1440 then
                        return tostring(config.system.hj212Platform.heartPackTime)
                    else
                        return value
                    end
                else
                    return tostring(config.system.hj212Platform.heartPackTime)
                end
            end,
        },
        {
            name = "hj212Platform.st",
            text = "ST",
            type = DataType.String,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.system.valuePattern(value) == true then
                    return value
                else
                    return config.system.hj212Platform.st
                end
            end,
        },
        {
            name = "hj212Platform.mn",
            text = "MN",
            type = DataType.String,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
        {
            name = "hj212Platform.pw",
            text = "PW",
            type = DataType.String,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.system.stringPattern(value) == true then
                    return value
                else
                    return config.system.hj212Platform.pw
                end
            end,
        },
    },]]--
    --7
    --[[{
        name = "communicationTCPMode",
        text = "TCP通信设置",
        {
            name = "hj212Platform.address",
            text = "服务器地址",
            type = DataType.IP,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            isSolidifyFromUI = true,
        },
        {
            name = "hj212Platform.port",
            text = "端口",
            type = DataType.Int,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            isSolidifyFromUI = true,
            checkValue = function(value)
                if setting.ui.profile.system.valuePattern(value) == true then
                    local num = tonumber(value)
                    if num <= 0 or num > 65535 then
                        return tostring(config.system.hj212Platform.port)
                    else
                        return value
                    end
                else
                    return tostring(config.system.hj212Platform.port)
                end
            end,
        },
    },]]--
    --{
    --    name = "CCEP",
    --    text = "CCEP",
    --    {
    --        name = "CCEPMode",
    --        text = "CCEP认证",
    --        type = DataType.Bool,
    --        writePrivilege=  RoleType.Super,
    --        readPrivilege = RoleType.Super,
    --    },
    --},
    --8
    {
        name = "screen",
        text = "显示屏",
        {
            name = "screenSaver.enable",
            text = "屏幕保护",
            type = DataType.Bool,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
        {
            name = "screenSaver.darkTime",
            text = "自动暗屏时间",
            unit = "秒",
            type = DataType.Int,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.system.valuePattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.system[8][3].currentValue ~= nil then
                        if num >= 60  and num < setting.ui.profile.system[8][3].currentValue then
                            setting.ui.profile.system[8][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.system[8][2].currentValue = setting.ui.profile.system[8][3].currentValue - 1
                            return tostring(setting.ui.profile.system[8][2].currentValue)
                        end
                    else
                        if num >= 60  and num < config.system.screenSaver.offTime then
                            setting.ui.profile.system[8][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.system[8][2].currentValue = config.system.screenSaver.darkTime
                            return tostring(config.system.screenSaver.darkTime)
                        end
                    end
                else
                    if setting.ui.profile.system[8][3].currentValue ~= nil then
                        setting.ui.profile.system[8][2].currentValue = setting.ui.profile.system[8][3].currentValue - 1
                    else
                        setting.ui.profile.system[8][2].currentValue = config.system.screenSaver.darkTime
                    end
                    return tostring(setting.ui.profile.system[8][2].currentValue)
                end
            end,
        },
        {
            name = "screenSaver.offTime",
            text = "自动关屏时间",
            unit = "秒",
            type = DataType.Int,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.system.valuePattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.system[8][2].currentValue ~= nil then
                        if num >= 60  and num > setting.ui.profile.system[8][2].currentValue then
                            setting.ui.profile.system[8][3].currentValue = num
                            return value
                        else
                            setting.ui.profile.system[8][3].currentValue = setting.ui.profile.system[8][2].currentValue + 1
                            return tostring(setting.ui.profile.system[8][3].currentValue)
                        end
                    else
                        if num >= 60  and num > config.system.screenSaver.darkTime then
                            setting.ui.profile.system[8][3].currentValue = num
                            return value
                        else
                            setting.ui.profile.system[8][3].currentValue = config.system.screenSaver.offTime
                            return tostring(config.system.screenSaver.offTime)
                        end
                    end
                else
                    if setting.ui.profile.system[8][2].currentValue ~= nil then
                        setting.ui.profile.system[8][3].currentValue = setting.ui.profile.system[8][2].currentValue + 1
                    else
                        setting.ui.profile.system[8][3].currentValue = config.system.screenSaver.offTime
                    end
                    return tostring(setting.ui.profile.system[8][3].currentValue)
                end
            end,
        },
        {
            name = "screenSaver.loginKeepingTime",
            text = "登录保持时间",
            unit = "秒",
            type = DataType.Int,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.system.valuePattern(value) == true then
                    local num = tonumber(value)
                    if num < 60 then
                        return tostring(config.system.screenSaver.loginKeepingTime)
                    else
                        return value
                    end
                else
                    return tostring(config.system.screenSaver.loginKeepingTime)
                end
            end,
        },
    },
    {
        name = "debug",
        text = "调试",
        {
            name = "debugMode",
            text = "开发者模式",
            type = DataType.Bool,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
        --{
        --    name = "bridgeMode",
        --    text = "桥接模式",
        --    type = DataType.Bool,
        --    writePrivilege=  RoleType.Super,
        --    readPrivilege = RoleType.Super,
        --},
        --{
        --    name = "bridgeIP",
        --    text = "桥接IP",
        --    type = DataType.IP,
        --    writePrivilege=  RoleType.Super,
        --    readPrivilege = RoleType.Super,
        --},
    },
    {
        name = "unitType",
        text = "单位类型",
        {
            name = "unitIndex",
            text = "浓度单位",
            type = DataType.Option,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            options =
            {
                setting.unit[1].text,
                setting.unit[2].text,
            },
        },
    },
    --[[{
        name = "wqimc",
        text = "水质云",
        {
            name = "serverMode",
            text = "云服务",
            type = DataType.Bool,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
			isSolidifyFromUI = true,
        },
        {
            name = "serverIP",
            text = "服务器",
            type = DataType.IP,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
			isSolidifyFromUI = true,
        },
        {
            name = "serverPort",
            text = "端口",
            type = DataType.Int,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
			isSolidifyFromUI = true,
            checkValue = function(value)
                if setting.ui.profile.system.valuePattern(value) == true then
                    local num = tonumber(value)
                    if num <= 0 or num > 65535 then
                        return tostring(config.system.serverPort)
                    else
                        return value
                    end
                else
                    return tostring(config.system.serverPort)
                end
            end,
        },
    },]]--
    {
        name = "printer",
        text = "打印机",
        {
            name = "printer.connectType",
            text = "模式",
            type = DataType.Option,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            options =
            {
                "串口",
                "网口",
            },
        },
        {
            name = "printer.enable",
            text = "打印功能",
            type = DataType.Bool,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
        {
            name = "printer.address",
            text = "IP地址",
            type = DataType.IP,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
        {
            name = "printer.autoPrint",
            text = "自动打印",
            type = DataType.Bool,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
    },
    {
        name = "resultParam",
        text = "结果参数",
        {
            name = "decimalNum",
            text = "结果小数位数",
            type = DataType.Int,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            isSolidifyFromUI = true,
            checkValue = function(value)
                if setting.ui.profile.system.valuePattern(value) == true then
                    local num = tonumber(value)
                    if num <= 0 or num > 5 then
                        return tostring(config.system.decimalNum)
                    else
                        return value
                    end
                else
                    return tostring(config.system.decimalNum)
                end
            end,
        },
    },
    checkOEM = function()
        return config.system.OEM
    end,
    valuePattern = function(value)
        if type(value) == "string" then
            local patterm = "^%d?%d?%d?%d?%d?%d$"
            if not string.find(value, patterm) then
                return false
            else
                return true
            end
        else
            return false
        end
    end,
    stringPattern = function(value)
        if type(value) == "string" then
            local patterm = "^%w+$"
            if not string.find(value, patterm) then
                return false
            else
                return true
            end
        else
            return false
        end
    end,
    oneDecimalPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^%d?%d?%d?%d?%d%.%d?$"
            local integerPatterm = "^%d?%d?%d?%d?%d$"
            if not string.find(value, decimalPatterm) then
                if string.find(value, integerPatterm) then
                    ret = true
                end
            else
                ret = true
            end
            return ret
        else
            return false
        end
    end,
    twoDecimalPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^%d?%d?%d?%d%.%d%d?$"
            local integerPatterm = "^%d?%d?%d?%d$"
            if not string.find(value, decimalPatterm) then
                if string.find(value, integerPatterm) then
                    ret = true
                end
            else
                ret = true
            end
            return ret
        else
            return false
        end
    end,
    fourDecimalPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^%d?%d?%d?%d%.%d%d?%d?%d?$"
            local integerPatterm = "^%d?%d?%d?%d$"
            if not string.find(value, decimalPatterm) then
                if string.find(value, integerPatterm) then
                    ret = true
                end
            else
                ret = true
            end
            return ret
        else
            return false
        end
    end,
    fourDecimalWithNegativePattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^%d?%d?%d?%d%.%d%d?%d?%d?$"
            local negativeDecimalPatterm = "^-%d?%d?%d?%d%.%d%d?%d?%d?$"
            local integerPatterm = "^%d?%d?%d?%d$"
            local negativeIntegerPatterm = "^-%d?%d?%d?%d$"
            if string.find(value, decimalPatterm) then
                ret = true
            elseif string.find(value, negativeDecimalPatterm) then
                ret = true
            elseif string.find(value, integerPatterm) then
                ret = true
            elseif string.find(value, negativeIntegerPatterm) then
                ret = true
            end
            return ret
        else
            return false
        end
    end,
    defaultRestore = function(userType)
        local isRestart = false

        if userType == RoleType.Super then

            local defaultTable = ConfigLists.LoadSystemConfig(true)

            Helper.DefaultRestore(defaultTable, config.system)
            log:info(Helper.GetRoleTypeStr().." 恢复默认"..setting.ui.profile.system.text)

            isRestart = config.modifyRecord.system(true)
            ConfigLists.SaveSystemConfig()
            config.modifyRecord.system(true)    --日志记录修改量程
            ConfigLists.SaveSystemConfig()

        elseif userType == RoleType.Administrator then

            local defaultTable = ConfigLists.LoadSystemConfig(true)

            config.system.faultBlocking = defaultTable.faultBlocking
            config.system.faultRetry = defaultTable.faultRetry
            config.system.modbusTableType = defaultTable.modbusTableType
            config.system.changeParamLog = defaultTable.changeParamLog
            config.system.adcDetect[1].enable = defaultTable.adcDetect[1].enable
            config.system.adcDetect[2].enable = defaultTable.adcDetect[2].enable
            config.system.displayProformaResult = defaultTable.displayProformaResult
            config.system.reagentLackWarn = defaultTable.reagentLackWarn
            config.system.hj212Type = defaultTable.hj212Type
            config.system.hj212Interval = defaultTable.hj212Interval
            config.system.hj212DataType = defaultTable.hj212DataType

            log:info(Helper.GetRoleTypeStr().." 恢复默认"..setting.ui.profile.system.text)
            isRestart = config.modifyRecord.system(true)
            ConfigLists.SaveSystemConfig()
        end
        App.SetScreenTime(config.system.screenSaver.darkTime, config.system.screenSaver.offTime, config.system.screenSaver.loginKeepingTime)
        App.SetSceenEnable(config.system.screenSaver.enable)

        return isRestart
    end,
    saveFile = function(isUpdate)
        local isRestart = false
        local changeTable = {}
        local logger = Log.Instance():GetLogger()
        logger:info(Helper.GetRoleTypeStr() .. " 修改" .. setting.ui.profile.system.text)
        --先保存COD转换开关值，再通过开关判定是否修改当前量程映射值
        isRestart,changeTable = config.modifyRecord.system(isUpdate)
        ConfigLists.SaveSystemConfig()

        config.modifyRecord.system(isUpdate)  --日志记录修改量程

        App.SetScreenTime(config.system.screenSaver.darkTime, config.system.screenSaver.offTime, config.system.screenSaver.loginKeepingTime)
        App.SetSceenEnable(config.system.screenSaver.enable)
        ConfigLists.SaveSystemConfig()

        -- 该UI界面的表（用于处理不需要重启的项）
        local uiTable = setting.ui.profile.system
        if changeTable ~= nil and type(changeTable) == "table" then
            for num,name in pairs(changeTable) do
                for i, listTable in pairs(uiTable) do
                    if type(listTable) == "table" then
                        for v,list in pairs(listTable) do
                            if  nil ~= list.name and
                                    name == list.name and
                                    nil ~= list.isRestart and
                                    false == list.isRestart then
                                isRestart = false
                            elseif nil ~= list.name and
                                    name == list.name then
                                return true
                            end
                        end
                    end
                end
            end
        end

        return isRestart
    end,
}
return setting.ui.profile.system
