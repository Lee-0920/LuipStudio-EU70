setting.ui.profile.interconnection =
{
    text = "外联接口",
    name = "interconnection",
    updateEvent = UpdateEvent.ChangeInterconnectionParam,
    index = 5,
    rowCount = 25,
    superRow = 1,
    administratorRow = 24,
    writePrivilege=  RoleType.Administrator,
    readPrivilege = RoleType.Administrator,
    isMeaParaml = false,
    -- group 1
    {
        name = "alarm",
        text = "TOC超标设置",
        {
            name = "alarmValue",
            text = "报警",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            isRestart = false,
        },
        {
            name = "meaUpLimit",
            text = "超标上限",
            unit = "mg/L",
            type = DataType.Float,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.interconnection.positiveDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.interconnection[1][3].currentValue~= nil then
                        if num >= setting.ui.profile.interconnection[1][3].currentValue then
                            setting.ui.profile.interconnection[1][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[1][2].currentValue = setting.ui.profile.interconnection[1][3].currentValue
                            return string.format("%.3f", setting.ui.profile.interconnection[1][2].currentValue)
                        end
                    else
                        if num >= config.interconnection.meaLowLimit then
                            setting.ui.profile.interconnection[1][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[1][2].currentValue = config.interconnection.meaUpLimit
                            return string.format("%.3f", config.interconnection.meaUpLimit)
                        end
                    end
                else
                    if setting.ui.profile.interconnection[1][3].currentValue~= nil then
                        setting.ui.profile.interconnection[1][2].currentValue = setting.ui.profile.interconnection[1][3].currentValue
                    else
                        setting.ui.profile.interconnection[1][2].currentValue = config.interconnection.meaUpLimit
                    end
                    return string.format("%.3f", setting.ui.profile.interconnection[1][2].currentValue)
                end
            end,
        },
        {
            name = "meaLowLimit",
            text = "超标下限",
            unit = "mg/L",
            type = DataType.Float,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.interconnection.positiveDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.interconnection[1][2].currentValue~= nil then
                        if num <= setting.ui.profile.interconnection[1][2].currentValue and num >= 0 then
                            setting.ui.profile.interconnection[1][3].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[1][3].currentValue = setting.ui.profile.interconnection[1][2].currentValue
                            return string.format("%.3f", setting.ui.profile.interconnection[1][3].currentValue)
                        end
                    else
                        if num <= config.interconnection.meaUpLimit and num >= 0 then
                            setting.ui.profile.interconnection[1][3].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[1][3].currentValue = config.interconnection.meaLowLimit
                            return string.format("%.3f", config.interconnection.meaLowLimit)
                        end
                    end
                else
                    if setting.ui.profile.interconnection[1][2].currentValue~= nil then
                        setting.ui.profile.interconnection[1][3].currentValue = setting.ui.profile.interconnection[1][2].currentValue
                    else
                        setting.ui.profile.interconnection[1][3].currentValue = config.interconnection.meaLowLimit
                    end
                    return string.format("%.3f", setting.ui.profile.interconnection[1][3].currentValue)
                end
            end,
        },
    },
    -- group 2
    {
        name = "alarm",
        text = "TC超标设置",
        {
            name = "alarmValueTC",
            text = "报警",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            isRestart = false,
        },
        {
            name = "meaUpLimitTC",
            text = "超标上限",
            unit = "mg/L",
            type = DataType.Float,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.interconnection.positiveDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.interconnection[2][3].currentValue~= nil then
                        if num >= setting.ui.profile.interconnection[2][3].currentValue then
                            setting.ui.profile.interconnection[2][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[2][2].currentValue = setting.ui.profile.interconnection[2][3].currentValue
                            return string.format("%.3f", setting.ui.profile.interconnection[2][2].currentValue)
                        end
                    else
                        if num >= config.interconnection.meaLowLimitTC then
                            setting.ui.profile.interconnection[2][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[2][2].currentValue = config.interconnection.meaUpLimitTC
                            return string.format("%.3f", config.interconnection.meaUpLimit)
                        end
                    end
                else
                    if setting.ui.profile.interconnection[2][3].currentValue~= nil then
                        setting.ui.profile.interconnection[2][2].currentValue = setting.ui.profile.interconnection[2][3].currentValue
                    else
                        setting.ui.profile.interconnection[2][2].currentValue = config.interconnection.meaUpLimitTC
                    end
                    return string.format("%.3f", setting.ui.profile.interconnection[2][2].currentValue)
                end
            end,
        },
        {
            name = "meaLowLimitTC",
            text = "超标下限",
            unit = "mg/L",
            type = DataType.Float,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.interconnection.positiveDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.interconnection[2][2].currentValue~= nil then
                        if num <= setting.ui.profile.interconnection[2][2].currentValue and num >= 0 then
                            setting.ui.profile.interconnection[2][3].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[2][3].currentValue = setting.ui.profile.interconnection[2][2].currentValue
                            return string.format("%.3f", setting.ui.profile.interconnection[2][3].currentValue)
                        end
                    else
                        if num <= config.interconnection.meaUpLimitTC and num >= 0 then
                            setting.ui.profile.interconnection[2][3].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[2][3].currentValue = config.interconnection.meaLowLimitTC
                            return string.format("%.3f", config.interconnection.meaLowLimitTC)
                        end
                    end
                else
                    if setting.ui.profile.interconnection[2][2].currentValue~= nil then
                        setting.ui.profile.interconnection[2][3].currentValue = setting.ui.profile.interconnection[2][2].currentValue
                    else
                        setting.ui.profile.interconnection[2][3].currentValue = config.interconnection.meaLowLimitTC
                    end
                    return string.format("%.3f", setting.ui.profile.interconnection[2][3].currentValue)
                end
            end,
        },
    },
    -- group 3
    {
        name = "alarm",
        text = "IC超标设置",
        {
            name = "alarmValueIC",
            text = "报警",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            isRestart = false,
        },
        {
            name = "meaUpLimitIC",
            text = "超标上限",
            unit = "mg/L",
            type = DataType.Float,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.interconnection.positiveDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.interconnection[3][3].currentValue~= nil then
                        if num >= setting.ui.profile.interconnection[3][3].currentValue then
                            setting.ui.profile.interconnection[3][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[3][2].currentValue = setting.ui.profile.interconnection[3][3].currentValue
                            return string.format("%.3f", setting.ui.profile.interconnection[3][2].currentValue)
                        end
                    else
                        if num >= config.interconnection.meaLowLimitIC then
                            setting.ui.profile.interconnection[3][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[3][2].currentValue = config.interconnection.meaUpLimitIC
                            return string.format("%.3f", config.interconnection.meaUpLimit)
                        end
                    end
                else
                    if setting.ui.profile.interconnection[3][3].currentValue~= nil then
                        setting.ui.profile.interconnection[3][2].currentValue = setting.ui.profile.interconnection[3][3].currentValue
                    else
                        setting.ui.profile.interconnection[3][2].currentValue = config.interconnection.meaUpLimitIC
                    end
                    return string.format("%.3f", setting.ui.profile.interconnection[3][2].currentValue)
                end
            end,
        },
        {
            name = "meaLowLimitIC",
            text = "超标下限",
            unit = "mg/L",
            type = DataType.Float,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.interconnection.positiveDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.interconnection[3][2].currentValue~= nil then
                        if num <= setting.ui.profile.interconnection[3][2].currentValue and num >= 0 then
                            setting.ui.profile.interconnection[3][3].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[3][3].currentValue = setting.ui.profile.interconnection[3][2].currentValue
                            return string.format("%.3f", setting.ui.profile.interconnection[3][3].currentValue)
                        end
                    else
                        if num <= config.interconnection.meaUpLimitIC and num >= 0 then
                            setting.ui.profile.interconnection[3][3].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[3][3].currentValue = config.interconnection.meaLowLimitIC
                            return string.format("%.3f", config.interconnection.meaLowLimitIC)
                        end
                    end
                else
                    if setting.ui.profile.interconnection[3][2].currentValue~= nil then
                        setting.ui.profile.interconnection[3][3].currentValue = setting.ui.profile.interconnection[3][2].currentValue
                    else
                        setting.ui.profile.interconnection[3][3].currentValue = config.interconnection.meaLowLimitIC
                    end
                    return string.format("%.3f", setting.ui.profile.interconnection[3][3].currentValue)
                end
            end,
        },
    },
    -- group 4
    {
        name = "RS485",
        text = "RS485",
        {
            name = "RS485BaudRate",
            text = "传输速率",
            unit = "bps",
            type = DataType.Option,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            options =
            {
                "9600",
                "19200",
                "38400",
                "115200",
                "230400",
            },
        },
        {
            name = "RS485Parity",
            text = "校验位",
            type = DataType.Option,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            options =
            {
                "无",
                "奇校验",
                "偶校验",
            },
        },
    },
    -- group 5
    {
        text = "RS232",
        name = "RS232",
        {
            name = "RS232BaudRate",
            text = "传输速率",
            unit = "bps",
            type = DataType.Option,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            options =
            {
                "9600",
                "19200",
                "38400",
                "115200",
                "230400",
            },
        },
        {
            name = "RS232Parity",
            text = "校验位",
            type = DataType.Option,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            options =
            {
                "无",
                "奇校验",
                "偶校验",
            },
        },
    },
    -- group 7
    {
        name = "MODBUS",
        text = "MODBUS",
        {
            name = "connectAddress",
            text = "通信地址",
            type = DataType.Int,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            checkValue = function(value)
                if setting.ui.profile.interconnection.addressPattern(value) == true then
                    local num = tonumber(value)
                    if num <= 0 then
                        return string.format("%d", config.interconnection.connectAddress)
                    else
                        return value
                    end
                else
                    return string.format("%d", config.interconnection.connectAddress)
                end
            end,
        },
    },
    -- group 7
    {
        name = "sample4-20mA",
        text = "[TOC]4-20mA",
        {
            name = "sampleLowLimit",
            text = "4mA对应浓度",
            unit = "mg/L",
            type = DataType.Float,
            analogConfig = true,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.interconnection.positiveDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.interconnection[7][2].currentValue ~= nil then
                        if num <= setting.ui.profile.interconnection[7][2].currentValue and num >= 0 then
                            setting.ui.profile.interconnection[7][1].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[7][1].currentValue = setting.ui.profile.interconnection[7][2].currentValue
                            return string.format("%.3f", setting.ui.profile.interconnection[7][1].currentValue)
                        end
                    else
                        if num <= config.interconnection.sampleUpLimit and num >= 0 then
                            setting.ui.profile.interconnection[7][1].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[7][1].currentValue = config.interconnection.sampleLowLimit
                            return string.format("%.3f", config.interconnection.sampleLowLimit)
                        end
                    end
                else
                    if setting.ui.profile.interconnection[7][2].currentValue~= nil then
                        setting.ui.profile.interconnection[7][1].currentValue = setting.ui.profile.interconnection[7][2].currentValue
                    else
                        setting.ui.profile.interconnection[7][1].currentValue = config.interconnection.sampleLowLimit
                    end
                    return string.format("%.3f", setting.ui.profile.interconnection[7][1].currentValue)
                end
            end,
        },
        {
            name = "sampleUpLimit",
            text = "20mA对应浓度",
            unit = "mg/L",
            type = DataType.Float,
            analogConfig = true,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.interconnection.positiveDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.interconnection[7][1].currentValue ~= nil then
                        if num >= setting.ui.profile.interconnection[7][1].currentValue then
                            setting.ui.profile.interconnection[7][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[7][2].currentValue = setting.ui.profile.interconnection[7][1].currentValue
                            return string.format("%.3f", setting.ui.profile.interconnection[7][2].currentValue)
                        end
                    else
                        if num >= config.interconnection.sampleLowLimit then
                            setting.ui.profile.interconnection[7][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[7][2].currentValue = config.interconnection.sampleUpLimit
                            return string.format("%.3f", config.interconnection.sampleUpLimit)
                        end
                    end
                else
                    if setting.ui.profile.interconnection[7][1].currentValue~= nil then
                        setting.ui.profile.interconnection[7][2].currentValue = setting.ui.profile.interconnection[7][1].currentValue
                    else
                        setting.ui.profile.interconnection[7][2].currentValue = config.interconnection.sampleUpLimit
                    end
                    return string.format("%.3f", setting.ui.profile.interconnection[7][2].currentValue)
                end
            end,
        },
    },
    -- group 8
    {
        name = "sample4-20mA",
        text = "[TC]4-20mA",
        {
            name = "sampleLowLimitTC",
            text = "4mA对应浓度",
            unit = "mg/L",
            type = DataType.Float,
            analogConfig = true,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.interconnection.positiveDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.interconnection[8][2].currentValue ~= nil then
                        if num <= setting.ui.profile.interconnection[8][2].currentValue and num >= 0 then
                            setting.ui.profile.interconnection[8][1].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[8][1].currentValue = setting.ui.profile.interconnection[8][2].currentValue
                            return string.format("%.3f", setting.ui.profile.interconnection[8][1].currentValue)
                        end
                    else
                        if num <= config.interconnection.sampleUpLimitTC and num >= 0 then
                            setting.ui.profile.interconnection[8][1].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[8][1].currentValue = config.interconnection.sampleLowLimitTC
                            return string.format("%.3f", config.interconnection.sampleLowLimitTC)
                        end
                    end
                else
                    if setting.ui.profile.interconnection[8][2].currentValue~= nil then
                        setting.ui.profile.interconnection[8][1].currentValue = setting.ui.profile.interconnection[8][2].currentValue
                    else
                        setting.ui.profile.interconnection[8][1].currentValue = config.interconnection.sampleLowLimitTC
                    end
                    return string.format("%.3f", setting.ui.profile.interconnection[8][1].currentValue)
                end
            end,
        },
        {
            name = "sampleUpLimitTC",
            text = "20mA对应浓度",
            unit = "mg/L",
            type = DataType.Float,
            analogConfig = true,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.interconnection.positiveDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.interconnection[8][1].currentValue ~= nil then
                        if num >= setting.ui.profile.interconnection[8][1].currentValue then
                            setting.ui.profile.interconnection[8][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[8][2].currentValue = setting.ui.profile.interconnection[8][1].currentValue
                            return string.format("%.3f", setting.ui.profile.interconnection[8][2].currentValue)
                        end
                    else
                        if num >= config.interconnection.sampleLowLimitTC then
                            setting.ui.profile.interconnection[8][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[8][2].currentValue = config.interconnection.sampleUpLimitTC
                            return string.format("%.3f", config.interconnection.sampleUpLimitTC)
                        end
                    end
                else
                    if setting.ui.profile.interconnection[8][1].currentValue~= nil then
                        setting.ui.profile.interconnection[8][2].currentValue = setting.ui.profile.interconnection[8][1].currentValue
                    else
                        setting.ui.profile.interconnection[8][2].currentValue = config.interconnection.sampleUpLimitTC
                    end
                    return string.format("%.3f", setting.ui.profile.interconnection[8][2].currentValue)
                end
            end,
        },
    },
    -- group 9
    {
        name = "sample4-20mA",
        text = "[IC]4-20mA",
        {
            name = "sampleLowLimitIC",
            text = "4mA对应浓度",
            unit = "mg/L",
            type = DataType.Float,
            analogConfig = true,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.interconnection.positiveDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.interconnection[9][2].currentValue ~= nil then
                        if num <= setting.ui.profile.interconnection[9][2].currentValue and num >= 0 then
                            setting.ui.profile.interconnection[9][1].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[9][1].currentValue = setting.ui.profile.interconnection[9][2].currentValue
                            return string.format("%.3f", setting.ui.profile.interconnection[9][1].currentValue)
                        end
                    else
                        if num <= config.interconnection.sampleUpLimitIC and num >= 0 then
                            setting.ui.profile.interconnection[9][1].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[9][1].currentValue = config.interconnection.sampleLowLimitIC
                            return string.format("%.3f", config.interconnection.sampleLowLimitIC)
                        end
                    end
                else
                    if setting.ui.profile.interconnection[9][2].currentValue~= nil then
                        setting.ui.profile.interconnection[9][1].currentValue = setting.ui.profile.interconnection[9][2].currentValue
                    else
                        setting.ui.profile.interconnection[9][1].currentValue = config.interconnection.sampleLowLimitIC
                    end
                    return string.format("%.3f", setting.ui.profile.interconnection[9][1].currentValue)
                end
            end,
        },
        {
            name = "sampleUpLimitIC",
            text = "20mA对应浓度",
            unit = "mg/L",
            type = DataType.Float,
            analogConfig = true,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.interconnection.positiveDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if setting.ui.profile.interconnection[9][1].currentValue ~= nil then
                        if num >= setting.ui.profile.interconnection[9][1].currentValue then
                            setting.ui.profile.interconnection[9][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[9][2].currentValue = setting.ui.profile.interconnection[9][1].currentValue
                            return string.format("%.3f", setting.ui.profile.interconnection[9][2].currentValue)
                        end
                    else
                        if num >= config.interconnection.sampleLowLimitIC then
                            setting.ui.profile.interconnection[9][2].currentValue = num
                            return value
                        else
                            setting.ui.profile.interconnection[9][2].currentValue = config.interconnection.sampleUpLimitIC
                            return string.format("%.3f", config.interconnection.sampleUpLimitIC)
                        end
                    end
                else
                    if setting.ui.profile.interconnection[9][1].currentValue~= nil then
                        setting.ui.profile.interconnection[9][2].currentValue = setting.ui.profile.interconnection[9][1].currentValue
                    else
                        setting.ui.profile.interconnection[9][2].currentValue = config.interconnection.sampleUpLimitIC
                    end
                    return string.format("%.3f", setting.ui.profile.interconnection[9][2].currentValue)
                end
            end,
        },
    },
    -- group 10
    {
        name = "multifunctionalrelay",
        text = "多功能继电器",
        {
            name = "relayOne",
            text = "继电器1",
            type = DataType.Option,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            options =
            {
                "超标上限",
                "超标下限",
                "测量指示",
                "校准指示",
            },
        },
        {
            name = "relayTwo",
            text = "继电器2",
            type = DataType.Option,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            options =
            {
                "超标上限",
                "超标下限",
                "测量指示",
                "校准指示",
            },
        },
        {
            name = "relayThree",
            text = "继电器3",
            type = DataType.Option,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            options =
            {
                "超标上限",
                "超标下限",
                "测量指示",
                "校准指示",
            },
        },
        {
            name = "relayFour",
            text = "继电器4",
            type = DataType.Option,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            options =
            {
                "超标上限",
                "超标下限",
                "测量指示",
                "校准指示",
            },
        },
    },
    -- group 11
    {
        name = "networksettings",
        text = "网络设置",
        {
            name = "settingIPMode",
            text = "设置IP模式",
            type = DataType.Option,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            options =
            {
                "DHCP",
                "静态",
            },
        },
    },
    defaultRestore = function()
        local isRestart = false

        local defaultTable = ConfigLists.LoadInterconnectionConfig(true)

        Helper.DefaultRestore(defaultTable, config.interconnection)
        log:info(Helper.GetRoleTypeStr().." 恢复默认"..setting.ui.profile.interconnection.text)
        isRestart = config.modifyRecord.interconnection(true)
        ConfigLists.SaveInterconnectionConfig()

        return isRestart
    end,
    saveFile = function(isUpdate)
        local isRestart = false
        local changeTable = {}
        local logger = Log.Instance():GetLogger()
        logger:info(Helper.GetRoleTypeStr() .. " 修改" .. setting.ui.profile.interconnection.text)
        isRestart,changeTable = config.modifyRecord.interconnection(isUpdate)
        ConfigLists.SaveInterconnectionConfig()

        -- 该UI界面的表（用于处理不需要重启的项）
        local uiTable = setting.ui.profile.interconnection
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
    isShowCheck = function()
        return true
    end,
    checkOEM = function()
        return config.system.OEM
    end,
    limitPattern = function(value) -- 小数
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^[-+]?%d?%d?%d?%d%.%d%d?%d?$"
            local integerPatterm = "^[-+]?%d?%d?%d?%d$"
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
    addressPattern = function(value) -- 匹配范围1-247
        if type(value) == "string" then
            local ret = false
            local patterm1 = "^[1]?%d?%d$"
            local patterm2 = "^[2][0-3]%d$"
            local patterm3 = "^[2][4][0-7]$"
            if not string.find(value, patterm1) then
                if not string.find(value, patterm2) then
                    if string.find(value, patterm3) then
                        ret = true
                    end
                else
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
    timePattern = function(value)
        if type(value) == "string" then
            local patterm = "^%d?%d?%d?%d$"
            if not string.find(value, patterm) then
                return false
            else
                return true
            end
        else
            return false
        end
    end,
    positiveDecimalPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^%d?%d?%d?%d%.%d%d?%d?$"
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
}

return setting.ui.profile.interconnection
