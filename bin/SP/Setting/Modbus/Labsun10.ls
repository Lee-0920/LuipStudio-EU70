setting.externalInterface = {}

setting.externalInterface.modbus =
{
    inputRegAddr = 0,
    inputRegNum = 256,
    holdRegAddr = 256,
    holdRegNum = 256,
    registers =
    {
        [0] =   --测量结果
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 0 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                if config.system.standardRegister == false then
                    modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.measure.consistency)
                else
                    modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.rangeCheck.consistency)

                end
           end,

            write = function()
            end,
        },
        [2] =   --量程校准液核查结果
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 2 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.check.consistency)
           end,

            write = function()
            end,
        },
        [4] =     -- 测量参数
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 4 - setting.externalInterface.modbus.inputRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Input, offsetAddress, setting.instrument.deviceTypeNum.TOC)
            end,

            write = function()
            end,
        },
        [5] =     -- 测量结果类型
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 5 - setting.externalInterface.modbus.inputRegAddr

                local code = status.measure.report.measure.resultType
                if status.measure.report.measure.resultType == MeasureType.Blank then
                    code = 0
                elseif status.measure.report.measure.resultType == MeasureType.Sample then
                    code = 1
                elseif status.measure.report.measure.resultType == MeasureType.Standard then
                    code = 2
                elseif status.measure.report.measure.resultType == MeasureType.RangeCheck then
                    code = 6
                elseif status.measure.report.measure.resultType == MeasureType.ZeroCheck then
                    code = 7
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Input, offsetAddress, code)
            end,

            write = function()
            end,
        },
        [6] =     -- 测量时长
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 6 - setting.externalInterface.modbus.inputRegAddr

                local value = status.measure.report.measure.measureTime

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Input, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [7] =     --测量时间
        {
            number = 3,	-- Register number

            read = function()
                local offsetAddress = 7 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                if config.system.standardRegister == false then
                    modbusManager:SetHexTime(RegisterType.Input, offsetAddress, status.measure.report.measure.dateTime)
                else
                    modbusManager:SetHexTime(RegisterType.Input, offsetAddress, status.measure.report.rangeCheck.dateTime)
                end

            end,

            write = function()
            end,
        },
        [10] =     --核查时间
        {
            number = 3,	-- Register number

            read = function()
                local offsetAddress = 10 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetHexTime(RegisterType.Input, offsetAddress, status.measure.report.check.dateTime)
            end,

            write = function()
            end,
        },
        [13] =     --校准时间
        {
            number = 3,	-- Register number

            read = function()
                local offsetAddress = 13 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetHexTime(RegisterType.Input, offsetAddress, status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime)
            end,

            write = function()
            end,
        },
        [16] =     --   系统状态
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 16 - setting.externalInterface.modbus.inputRegAddr

                local statusManager = StatusManager.Instance()
                local runStatus = statusManager:GetStatus()
                local name = runStatus:GetName()

                local modbusManager = ModbusManager.Instance()
                if setting.modbusCoder.statusID[name] ~= nil then
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, setting.modbusCoder.statusID[name].ID)
                else
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 0)
                end
            end,

            write = function()
            end,
        },
        [17] =     --  当前动作
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 17 - setting.externalInterface.modbus.inputRegAddr

                local statusManager = StatusManager.Instance()
                local action = statusManager:GetAction()
                local name = action:GetName()

                local modbusManager = ModbusManager.Instance()

                if setting.modbusCoder.actionID[name] ~= nil then
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, setting.modbusCoder.actionID[name].ID)
                else
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 0)
                end

            end,

            write = function()
            end,
        },
        [18] =     --   当前温度
        {
            number = 4,	-- Register number

            read = function()
                local offsetAddress = 18 - setting.externalInterface.modbus.inputRegAddr

                local dTemp = dc:GetDigestTemperature()
                local eTemp = dc:GetEnvironmentTemperature()

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, dTemp)
                modbusManager:SetFloat(RegisterType.Input, offsetAddress + 2, eTemp)
            end,

            write = function()
            end,
        },
        [22] =     --  报警时间
        {
            number = 3,	-- Register number

            read = function()
                local offsetAddress = 22 - setting.externalInterface.modbus.inputRegAddr

                local AlarmManager = AlarmManager.Instance()
                local dateTime = AlarmManager:GetCurrentAlarm():GetTime()

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetHexTime(RegisterType.Input, offsetAddress, dateTime )
            end,

            write = function()
            end,
        },
        [25] =   --报警代码
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 25 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local AlarmManager = AlarmManager.Instance()
                local alarm = AlarmManager:GetCurrentAlarm()
                local name = alarm:GetName()
                local cause = alarm:GetCause()

                local alarmID = 0
                local modbusManager = ModbusManager.Instance()

                if name == "抽取失败" then
                    if cause == "零点校准液" then
                        alarmID = 1
                    elseif cause == "量程校准液" then
                        alarmID = 2
                    elseif cause == "水样" then
                        alarmID = 3
                    elseif cause == "逐出混合液" then
                        alarmID = 4
                    elseif cause == "试剂一" then
                        alarmID = 5
                    elseif cause == "试剂二"then
                        alarmID = 6
                    elseif cause == "试剂三" then
                        alarmID = 7
                    elseif cause == "试剂四" then
                        alarmID = 8
                    elseif cause == "试剂五" then
                        alarmID = 9
                    elseif cause == "试剂六" then
                        alarmID = 10
                    elseif cause == "零点核查液" then
                        alarmID = 50
                    elseif cause == "标样(跨度)核查液" then
                        alarmID = 51
                    end
                elseif name == "请更换试剂" then
                    if cause == "零点校准液" then
                        alarmID = 11
                    elseif cause == "量程校准液" then
                        alarmID = 12
                    elseif cause == "试剂一" then
                        alarmID = 13
                    elseif cause == "试剂二"then
                        alarmID = 14
                    elseif cause == "试剂三" then
                        alarmID = 15
                    elseif cause == "试剂四" then
                        alarmID = 16
                    elseif cause == "试剂五" then
                        alarmID = 17
                    elseif cause == "试剂六" then
                        alarmID = 18
                    elseif cause == "零点核查液" then
                        alarmID = 52
                    elseif cause == "标样(跨度)核查液" then
                        alarmID = 53
                    elseif cause == "分析废液" then
                        alarmID = 40
                    elseif cause == "清洗废水" then
                        alarmID = 43
                    end
                elseif name == "温控故障" then
                    alarmID = 19
                elseif name == "通信异常" then
                    if cause == "液路板" then
                        alarmID = 20
                    elseif cause == "温控板" then
                        alarmID = 21
                    elseif cause == "信号板" then
                        alarmID = 22
                    elseif cause == "驱动板" then
                        alarmID = 36
                    elseif cause == "反应板" then
                        alarmID = 37
                    end
                elseif name == "升级失败" then
                    if cause == "液路板" then
                        alarmID = 23
                    elseif cause == "温控板" then
                        alarmID = 24
                    elseif cause == "信号板" then
                        alarmID = 25
                    elseif cause == "驱动板" then
                        alarmID = 38
                    elseif cause == "反应板" then
                        alarmID = 39
                    end
                elseif name == "仪器运行故障" then
                    alarmID = 26
                elseif name == "排消解液异常" then
                    alarmID = 27
                elseif name == "定量泵校准失败" then
                    alarmID = 28
                elseif name == "水样超上限" then
                    alarmID = 29
                elseif name == "校准结果错误" then
                    alarmID = 30
                elseif name == "耗材已过期" then
                    if cause == "蠕动泵软管" then
                        alarmID = 31
                    elseif cause == "夹断阀软管" then
                        alarmID = 32
                    elseif cause == "定量管" then
                        alarmID = 33
                    elseif cause == "定量O形圈"then
                        alarmID = 34
                    elseif cause == "PTFE硬管" then
                        alarmID = 35
                    end
                elseif name == "水样超量程" then
                    alarmID = 41
                elseif name == "校准结果错误" then
                    alarmID = 42
                elseif name == "静态AD调节失败" then
                    alarmID = 44
                elseif name == "消解室加液异常" then
                    alarmID = 45
		        elseif name == "核查结果错误" then
                    alarmID = 46
                elseif name == "仪器漏液" then
                    alarmID = 47
                else
                    alarmID = 0
                end

                modbusManager:SetShort(RegisterType.Input, offsetAddress, alarmID)
            end,

            write = function()
            end,
        },
        [26] =     --  量程表
        {
            number = 8,	-- Register number

            read = function()
                local offsetAddress = 26 - setting.externalInterface.modbus.inputRegAddr

                local range1 = 0
                local range2 = 0
                local range3 = 0
                local range4 = 0

                if config.system.rangeViewMap[1] ~= nil then
                    range1 = config.system.rangeViewMap[1].view
                end

                if config.system.rangeViewMap[2] ~= nil then
                    range2 = config.system.rangeViewMap[2].view
                end

                if config.system.rangeViewMap[3] ~= nil then
                    range3 = config.system.rangeViewMap[3].view
                end

                if config.system.rangeViewMap[4] ~= nil then
                    range4 = config.system.rangeViewMap[4].view
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, range1)
                modbusManager:SetFloat(RegisterType.Input, offsetAddress + 2, range2)
                modbusManager:SetFloat(RegisterType.Input, offsetAddress + 4, range3)
                modbusManager:SetFloat(RegisterType.Input, offsetAddress + 6, range4)
            end,

            write = function()
            end,
        },
        [34] =     --  量程校准液浓度
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 34 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange+1] + 1].RangeConsistency)
            end,

            write = function()
            end,
        },
        [36] =     --  产品名称
        {
            number = 32,	-- Register number

            read = function()
                local offsetAddress = 36 - setting.externalInterface.modbus.inputRegAddr
                local regSize = setting.externalInterface.modbus.registers[36].number

                local modbusManager = ModbusManager.Instance()
                local str = setting.instrument.name

                modbusManager:SetString(RegisterType.Input, offsetAddress, str, regSize)
            end,

            write = function()
            end,
        },
        [68] =     --  产品型号
        {
            number = 32,	-- Register number

            read = function()
                local offsetAddress = 68 - setting.externalInterface.modbus.inputRegAddr
                local regSize = setting.externalInterface.modbus.registers[68].number

                local modbusManager = ModbusManager.Instance()
                local str = config.info.instrument["model"]

                modbusManager:SetString(RegisterType.Input, offsetAddress, str, regSize)
            end,

            write = function()
            end,
        },
        [100] =     --  生产厂商
        {
            number = 32,	-- Register number

            read = function()
                local offsetAddress = 100 - setting.externalInterface.modbus.inputRegAddr
                local regSize = setting.externalInterface.modbus.registers[100].number

                local modbusManager = ModbusManager.Instance()
                local str = config.info.instrument["manuFacturer"]

                modbusManager:SetString(RegisterType.Input, offsetAddress, str, regSize)
            end,

            write = function()
            end,
        },
        [132] =     -- 生产序列号
        {
            number = 16,	-- Register number

            read = function()
                local offsetAddress = 132 - setting.externalInterface.modbus.inputRegAddr
                local regSize = setting.externalInterface.modbus.registers[132].number

                local modbusManager = ModbusManager.Instance()
                local str = config.info.instrument["sn"]

                modbusManager:SetString(RegisterType.Input, offsetAddress, str, regSize)
            end,

            write = function()
            end,
        },
        [148] =     --   液路板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if lc ~= nil then
                    local offsetAddress = 148 - setting.externalInterface.modbus.inputRegAddr

                    local ver = 0
                    local major = 0
                    local minor = 0
                    local build  = 0
                    local revision  = 0
                    local err,result = pcall(function()
                        if lc:GetConnectStatus() == true then
                            ver = lc:GetIDeviceInfo():GetSoftwareVersion()
                        end
                    end)

                    if not err then      -- 出现异常
                        if type(result) == "userdata" then
                            if result:GetType() == "ExpectEventTimeoutException" then          --期望事件等待超时异常。
                                ExceptionHandler.MakeAlarm(result)
                            elseif result:GetType() == "CommandTimeoutException" then          --命令应答超时异常
                                ExceptionHandler.MakeAlarm(result)
                            else
                                log:warn(result:What())
                            end
                        elseif type(result) == "table" then
                            log:warn(result:What())								--其他定义类型异常
                        elseif type(result) == "string" then
                            log:warn(result)	--C++、Lua系统异常
                        end
                    else
                        if ver ~= 0 then
                            major = ver:GetMajor()
                            minor = ver:GetMinor()
                            build  = ver:GetBuild()
                            revision  = ver:GetRevision()
                        end
                        modbusManager = ModbusManager.Instance()
                        modbusManager:SetShort(RegisterType.Input, offsetAddress, major * 256 + minor)
                        modbusManager:SetShort(RegisterType.Input, offsetAddress + 1, revision * 256 + build)
                    end
                end
            end,

            write = function()
            end,
        },
        [150] =     --   温控板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if tc ~= nil then
                    local offsetAddress = 150 - setting.externalInterface.modbus.inputRegAddr

                    local ver = 0
                    local major = 0
                    local minor = 0
                    local build  = 0
                    local revision  = 0
                    local err,result = pcall(function()
                        if tc:GetConnectStatus() == true then
                            ver = tc:GetIDeviceInfo():GetSoftwareVersion()
                        end
                    end)

                    if not err then      -- 出现异常
                        if type(result) == "userdata" then
                            if result:GetType() == "ExpectEventTimeoutException" then          --期望事件等待超时异常。
                                ExceptionHandler.MakeAlarm(result)
                            elseif result:GetType() == "CommandTimeoutException" then          --命令应答超时异常
                                ExceptionHandler.MakeAlarm(result)
                            else
                                log:warn(result:What())
                            end
                        elseif type(result) == "table" then
                            log:warn(result:What())								--其他定义类型异常
                        elseif type(result) == "string" then
                            log:warn(result)	--C++、Lua系统异常
                        end
                    else
                        if ver ~= 0 then
                            major = ver:GetMajor()
                            minor = ver:GetMinor()
                            build  = ver:GetBuild()
                            revision  = ver:GetRevision()
                        end
                        local modbusManager = ModbusManager.Instance()
                        modbusManager:SetShort(RegisterType.Input, offsetAddress, major * 256 + minor)
                        modbusManager:SetShort(RegisterType.Input, offsetAddress + 1, revision * 256 + build)
                    end
                end
            end,

            write = function()
            end,
        },
        [152] =     --   信号板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if oa ~= nil then
                    local offsetAddress = 152 - setting.externalInterface.modbus.inputRegAddr

                    local ver = 0
                    local major = 0
                    local minor = 0
                    local build  = 0
                    local revision  = 0
                    local err,result = pcall(function()
                        if oa:GetConnectStatus() == true then
                            ver = oa:GetIDeviceInfo():GetSoftwareVersion()
                        end
                    end)

                    if not err then      -- 出现异常
                        if type(result) == "userdata" then
                            if result:GetType() == "ExpectEventTimeoutException" then          --期望事件等待超时异常。
                                ExceptionHandler.MakeAlarm(result)
                            elseif result:GetType() == "CommandTimeoutException" then          --命令应答超时异常
                                ExceptionHandler.MakeAlarm(result)
                            else
                                log:warn(result:What())
                            end
                        elseif type(result) == "table" then
                            log:warn(result:What())								--其他定义类型异常
                        elseif type(result) == "string" then
                            log:warn(result)	--C++、Lua系统异常
                        end
                    else
                        if ver ~= 0 then
                            major = ver:GetMajor()
                            minor = ver:GetMinor()
                            build  = ver:GetBuild()
                            revision  = ver:GetRevision()
                        end
                        local modbusManager = ModbusManager.Instance()
                        modbusManager:SetShort(RegisterType.Input, offsetAddress, major * 256 + minor)
                        modbusManager:SetShort(RegisterType.Input, offsetAddress + 1, revision * 256 + build)
                    end
                end
            end,

            write = function()
            end,
        },
        [154] =     --  主控板固件版本
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 154 - setting.externalInterface.modbus.inputRegAddr

                local ver = Version.new(setting.version.software)

                local major = ver:GetMajor()
                local minor = ver:GetMinor()
                local build  = ver:GetBuild()
                local revision  = ver:GetRevision()
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Input, offsetAddress, major * 256 + minor)
                modbusManager:SetShort(RegisterType.Input, offsetAddress + 1, revision * 256 + build)

                ver = nil
            end,

            write = function()
            end,
        },
        [156] =     --   驱动板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if dc ~= nil then
                    local offsetAddress = 156 - setting.externalInterface.modbus.inputRegAddr

                    local ver = 0
                    local major = 0
                    local minor = 0
                    local build  = 0
                    local revision  = 0
                    local err,result = pcall(function()
                        if dc:GetConnectStatus() == true then
                            ver = dc:GetIDeviceInfo():GetSoftwareVersion()
                        end
                    end)

                    if not err then      -- 出现异常
                        if type(result) == "userdata" then
                            if result:GetType() == "ExpectEventTimeoutException" then          --期望事件等待超时异常。
                                ExceptionHandler.MakeAlarm(result)
                            elseif result:GetType() == "CommandTimeoutException" then          --命令应答超时异常
                                ExceptionHandler.MakeAlarm(result)
                            else
                                log:warn(result:What())
                            end
                        elseif type(result) == "table" then
                            log:warn(result:What())								--其他定义类型异常
                        elseif type(result) == "string" then
                            log:warn(result)	--C++、Lua系统异常
                        end
                    else
                        if ver ~= 0 then
                            major = ver:GetMajor()
                            minor = ver:GetMinor()
                            build  = ver:GetBuild()
                            revision  = ver:GetRevision()
                        end
                        local modbusManager = ModbusManager.Instance()
                        modbusManager:SetShort(RegisterType.Input, offsetAddress, major * 256 + minor)
                        modbusManager:SetShort(RegisterType.Input, offsetAddress + 1, revision * 256 + build)
                    end
                end
            end,

            write = function()
            end,
        },
        [158] =     --   反应板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if rc ~= nil then
                    local offsetAddress = 158 - setting.externalInterface.modbus.inputRegAddr

                    local ver = 0
                    local major = 0
                    local minor = 0
                    local build  = 0
                    local revision  = 0
                    local err,result = pcall(function()
                        if rc:GetConnectStatus() == true then
                            ver = rc:GetIDeviceInfo():GetSoftwareVersion()
                        end
                    end)

                    if not err then      -- 出现异常
                        if type(result) == "userdata" then
                            if result:GetType() == "ExpectEventTimeoutException" then          --期望事件等待超时异常。
                                ExceptionHandler.MakeAlarm(result)
                            elseif result:GetType() == "CommandTimeoutException" then          --命令应答超时异常
                                ExceptionHandler.MakeAlarm(result)
                            else
                                log:warn(result:What())
                            end
                        elseif type(result) == "table" then
                            log:warn(result:What())								--其他定义类型异常
                        elseif type(result) == "string" then
                            log:warn(result)	--C++、Lua系统异常
                        end
                    else
                        if ver ~= 0 then
                            major = ver:GetMajor()
                            minor = ver:GetMinor()
                            build  = ver:GetBuild()
                            revision  = ver:GetRevision()
                        end
                        local modbusManager = ModbusManager.Instance()
                        modbusManager:SetShort(RegisterType.Input, offsetAddress, major * 256 + minor)
                        modbusManager:SetShort(RegisterType.Input, offsetAddress + 1, revision * 256 + build)
                    end
                end
            end,

            write = function()
            end,
        },
        [160] =   --零点核查结果
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 160 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.zeroCheck.consistency)
            end,

            write = function()
            end,
        },
        [162] =     --零点核查时间
        {
            number = 3,	-- Register number

            read = function()
                local offsetAddress = 162 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetHexTime(RegisterType.Input, offsetAddress, status.measure.report.zeroCheck.dateTime)
            end,

            write = function()
            end,
        },
        [165] =   --量程核查结果
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 165 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.rangeCheck.consistency)
            end,

            write = function()
            end,
        },
        [167] =     --量程核查时间
        {
            number = 3,	-- Register number

            read = function()
                local offsetAddress = 167 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetHexTime(RegisterType.Input, offsetAddress, status.measure.report.rangeCheck.dateTime)
            end,

            write = function()
            end,
        },
        [170] =   --加标样结果
        {
            number = 2,		-- Register number

            read = function()
                --local offsetAddress = 170 - setting.externalInterface.modbus.inputRegAddr
                --
                --local modbusManager = ModbusManager.Instance()
                --modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.addstandard.consistency)
            end,

            write = function()
            end,
        },
        [172] =     --加标时间
        {
            number = 3,	-- Register number

            read = function()
                --local offsetAddress = 172 - setting.externalInterface.modbus.inputRegAddr
                --
                --local modbusManager = ModbusManager.Instance()
                --modbusManager:SetHexTime(RegisterType.Input, offsetAddress, status.measure.report.addstandard.dateTime)
            end,

            write = function()
            end,
        },
        [175] =   --平行样结果
        {
            number = 2,		-- Register number

            read = function()
                --local offsetAddress = 175 - setting.externalInterface.modbus.inputRegAddr
                --
                --local modbusManager = ModbusManager.Instance()
                --modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.parallel.consistency)
            end,

            write = function()
            end,
        },
        [177] =     --平行时间
        {
            number = 3,	-- Register number

            read = function()
                --local offsetAddress = 177 - setting.externalInterface.modbus.inputRegAddr
                --
                --local modbusManager = ModbusManager.Instance()
                --modbusManager:SetHexTime(RegisterType.Input, offsetAddress, status.measure.report.parallel.dateTime)
            end,

            write = function()
            end,
        },
        [180] =   --空白核查结果
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 180 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.blankCheck.consistency)
            end,

            write = function()
            end,
        },
        [182] =     --空白核查时间
        {
            number = 3,	-- Register number

            read = function()
                local offsetAddress = 182 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetHexTime(RegisterType.Input, offsetAddress, status.measure.report.blankCheck.dateTime)
            end,

            write = function()
            end,
        },
        [185] =     -- 185 - 189 预留
        {
            number = 5,	-- Register number
            read = function()
            end,

            write = function()
            end,
        },
        [190] =   --水样吸光度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 190 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.measure.absorbance)
            end,

            write = function()
            end,
        },
        [192] =     --水样数据标识
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 192 - setting.externalInterface.modbus.inputRegAddr

                local resultInfo = "N"
                if config.system.standardRegister == false then
                    if type(status.measure.report.measure.resultInfo) == "string" then
                        resultInfo = status.measure.report.measure.resultInfo
                        if resultInfo == "" then
                            resultInfo = "N"
                        end
                    end
                else
                    if type(status.measure.report.rangeCheck.resultInfo) == "string" then
                        resultInfo = status.measure.report.rangeCheck.resultInfo
                        if resultInfo == "" then
                            resultInfo = "N"
                        end
                    end
                end

                local regSize = setting.externalInterface.modbus.registers[192].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetString(RegisterType.Input, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [193] =   --标样核查吸光度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 193 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.check.absorbance)
            end,

            write = function()
            end,
        },
        [195] =     --标样核查数据标识
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 195 - setting.externalInterface.modbus.inputRegAddr

                local resultInfo = "N"
                if type(status.measure.report.check.resultInfo) == "string" then
                    resultInfo = status.measure.report.check.resultInfo
                    if resultInfo == "" then
                        resultInfo = "N"
                    end
                end

                local regSize = setting.externalInterface.modbus.registers[195].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetString(RegisterType.Input, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [196] =   --零点核查吸光度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 196 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.zeroCheck.absorbance)
            end,

            write = function()
            end,
        },
        [198] =     --零点核查数据标识
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 198 - setting.externalInterface.modbus.inputRegAddr

                local resultInfo = "N"
                if type(status.measure.report.zeroCheck.resultInfo) == "string" then
                    resultInfo = status.measure.report.zeroCheck.resultInfo
                    if resultInfo == "" then
                        resultInfo = "N"
                    end
                end

                local regSize = setting.externalInterface.modbus.registers[198].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetString(RegisterType.Input, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [199] =   --量程核查吸光度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 199 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.rangeCheck.absorbance)
            end,

            write = function()
            end,
        },
        [201] =     --量程核查数据标识
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 201 - setting.externalInterface.modbus.inputRegAddr

                local resultInfo = "N"
                if type(status.measure.report.rangeCheck.resultInfo) == "string" then
                    resultInfo = status.measure.report.rangeCheck.resultInfo
                    if resultInfo == "" then
                        resultInfo = "N"
                    end
                end

                local regSize = setting.externalInterface.modbus.registers[201].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetString(RegisterType.Input, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [202] =   --加标回收吸光度
        {
            number = 2,		-- Register number

            read = function()
                --local offsetAddress = 202 - setting.externalInterface.modbus.inputRegAddr
                --
                --local modbusManager = ModbusManager.Instance()
                --modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.addstandard.absorbance)
            end,

            write = function()
            end,
        },
        [204] =     --加标回收数据标识
        {
            number = 1,	-- Register number

            read = function()
                --local offsetAddress = 204 - setting.externalInterface.modbus.inputRegAddr
                --
                --local resultInfo = "N"
                --if type(status.measure.report.addstandard.resultInfo) == "string" then
                --    resultInfo = status.measure.report.addstandard.resultInfo
                --    if resultInfo == "" then
                --        resultInfo = "N"
                --    end
                --end
                --
                --local regSize = setting.externalInterface.modbus.registers[204].number
                --
                --local modbusManager = ModbusManager.Instance()
                --modbusManager:SetString(RegisterType.Input, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [205] =   --平行样吸光度
        {
            number = 2,		-- Register number

            read = function()
                --local offsetAddress = 205 - setting.externalInterface.modbus.inputRegAddr
                --
                --local modbusManager = ModbusManager.Instance()
                --modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.parallel.absorbance)
            end,

            write = function()
            end,
        },
        [207] =     --平行样数据标识
        {
            number = 1,	-- Register number

            read = function()
                --local offsetAddress = 207 - setting.externalInterface.modbus.inputRegAddr
                --
                --local resultInfo = "N"
                --if type(status.measure.report.parallel.resultInfo) == "string" then
                --    resultInfo = status.measure.report.parallel.resultInfo
                --    if resultInfo == "" then
                --        resultInfo = "N"
                --    end
                --end
                --
                --local regSize = setting.externalInterface.modbus.registers[207].number
                --
                --local modbusManager = ModbusManager.Instance()
                ----modbusManager:SetCharStringLittleEndian(RegisterType.Input, offsetAddress, resultInfo, regSize)
                --modbusManager:SetString(RegisterType.Input, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [208] =   --空白核查吸光度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 208 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.blankCheck.absorbance)
            end,

            write = function()
            end,
        },
        [210] =     --空白核查数据标识
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 210 - setting.externalInterface.modbus.inputRegAddr

                local resultInfo = "N"
                if type(status.measure.report.blankCheck.resultInfo) == "string" then
                    resultInfo = status.measure.report.blankCheck.resultInfo
                    if resultInfo == "" then
                        resultInfo = "N"
                    end
                end

                local regSize = setting.externalInterface.modbus.registers[210].number

                local modbusManager = ModbusManager.Instance()
                --modbusManager:SetCharStringLittleEndian(RegisterType.Input, offsetAddress, resultInfo, regSize)
                modbusManager:SetString(RegisterType.Input, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [211] =     -- 211 - 219 预留
        {
            number = 9,	-- Register number
            read = function()
            end,

            write = function()
            end,
        },
        [220] =   --校准零点浓度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 220 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].point0Consistency)
            end,

            write = function()
            end,
        },
        [222] =   --校准零点吸光度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 222 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].point0Absorbance)
            end,

            write = function()
            end,
        },
        [224] =   --零点漂移
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 224 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, setting.measureResult.zeroDrift)
            end,

            write = function()
            end,
        },
        [226] =     -- 226 - 229 预留
        {
            number = 4,	-- Register number
            read = function()
            end,

            write = function()
            end,
        },
        [230] =   --校准标点浓度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 230 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].point1Consistency)
            end,

            write = function()
            end,
        },
        [232] =   --校准标点吸光度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 232 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].point1Absorbance)
            end,

            write = function()
            end,
        },
        [234] =   --量程漂移
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 234 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, setting.measureResult.rangeDrift)
            end,

            write = function()
            end,
        },
        [236] =   --复合测量结果
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 236 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.complexResult.consistency)
           end,

            write = function()
            end,
        },
        [238] =     --复合测量时间
        {
            number = 3,	-- Register number

            read = function()
                local offsetAddress = 238 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetHexTime(RegisterType.Input, offsetAddress, status.measure.report.complexResult.dateTime)
            end,

            write = function()
            end,
        },
        [241] =     --复合测量数据标识
        
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 241 - setting.externalInterface.modbus.inputRegAddr

                local resultInfo = "N"
                if type(status.measure.report.check.resultInfo) == "string" then
                    resultInfo = status.measure.report.complexResult.resultInfo
                    if resultInfo == "" then
                        resultInfo = "N"
                    end
                end

                local regSize = setting.externalInterface.modbus.registers[241].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetString(RegisterType.Input, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [242] =     -- 242 - 255 预留
        {
            number = 14,	-- Register number
            read = function()
            end,

            write = function()
            end,
        },
        [256] =    --  当前量程
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 256 - setting.externalInterface.modbus.holdRegAddr

                local range = 0

                if config.system.rangeViewMap[config.measureParam.range[config.measureParam.currentRange + 1] + 1] ~= nil then
                    range = config.system.rangeViewMap[config.measureParam.range[config.measureParam.currentRange + 1] + 1].view
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, range)
            end,

            write = function()
                local offsetAddress = 256 - setting.externalInterface.modbus.holdRegAddr

                if not config.system.solidifyMeaParamFromModbus then
                    local modbusManager = ModbusManager.Instance()
                    local inputValue = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)

                    local rangeNum =  setting.measure.range.rangeNum
                    for i = 1, rangeNum do
                        local temp = inputValue - config.system.rangeViewMap[i].view
                        if temp <= 0.000001 then
                            for j=1 , 3 do
                                if config.measureParam.range[j] == i - 1 then
                                    config.measureParam.currentRange = j - 1
                                end
                            end
                            break
                        end
                    end

                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ModbusChangeParam, "Modbus")
                end
            end,
        },
        [258] =     --斜率
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 258 - setting.externalInterface.modbus.holdRegAddr

                local curveK = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].curveK
                local curveB = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].curveB
                curveK, curveB  = CurveParamCurveXYChange(curveK, curveB)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, curveK)
            end,

            write = function()
                local offsetAddress = 258 - setting.externalInterface.modbus.holdRegAddr

                if not config.system.solidifyMeaParamFromModbus then
                    local modbusManager = ModbusManager.Instance()
                    local inputValue = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)

                    if inputValue >= -999999.999999 and inputValue <= 999999.999999 then
                        config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].curveK = inputValue
                    end

                    config.modifyRecord.measureParam(true)
                    ConfigLists.SaveMeasureParamConfig()
                    setting.ui.profile.measureParam.updaterCurveParam(0,true)

                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ModbusChangeParam, "Modbus")
                end
            end,
        },
        [260] =     --截距
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 260 - setting.externalInterface.modbus.holdRegAddr

                local curveK = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].curveK
                local curveB = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].curveB
                curveK, curveB  = CurveParamCurveXYChange(curveK, curveB)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, curveB)
            end,

            write = function()
                local offsetAddress = 260 - setting.externalInterface.modbus.holdRegAddr

                if not config.system.solidifyMeaParamFromModbus then
                    local modbusManager = ModbusManager.Instance()
                    local inputValue = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)

                    if inputValue >= -999999.999999 and inputValue <= 999999.999999 then
                        config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].curveB = inputValue
                    end

                    config.modifyRecord.measureParam(true)
                    ConfigLists.SaveMeasureParamConfig()
                    setting.ui.profile.measureParam.updaterCurveParam(0,true)

                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ModbusChangeParam, "Modbus")
                end
            end,
        },
        [262] =    --  消解温度
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 262 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.reactTemperature)
            end,

            write = function()

            end,
        },
        [264] =    -- 消解时间
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 264 - setting.externalInterface.modbus.holdRegAddr

                local value = config.measureParam.reacTime
                
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()

            end,
        },
        [265] =     --反应温度
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 265 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.reactTemperature)
            end,

            write = function()
                --local offsetAddress = 265 - setting.externalInterface.modbus.holdRegAddr
                --
                --if not config.system.solidifyMeaParamFromModbus then
                --    local modbusManager = ModbusManager.Instance()
                --    local inputValue = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                --
                --    if inputValue >= 0 and inputValue < 200 then
                --        config.measureParam.reactTemperature = inputValue
                --    end
                --
                --    config.modifyRecord.measureParam(true)
                --    ConfigLists.SaveMeasureParamConfig()
                --
                --    local updateWidgetManager = UpdateWidgetManager.Instance()
                --    updateWidgetManager:Update(UpdateEvent.ModbusChangeParam, "Modbus")
                --end
            end,
        },
        [267] =     --反应时间
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 267 - setting.externalInterface.modbus.holdRegAddr

                local value = config.measureParam.reacTime

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
                --local offsetAddress = 267 - setting.externalInterface.modbus.holdRegAddr
                --
                --if not config.system.solidifyMeaParamFromModbus then
                --    local modbusManager = ModbusManager.Instance()
                --    local inputValue = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                --
                --    if inputValue >= 0 and inputValue < 10000 then
                --        config.measureParam.reactTime = inputValue
                --    end
                --
                --    config.modifyRecord.measureParam(true)
                --    ConfigLists.SaveMeasureParamConfig()
                --
                --    local updateWidgetManager = UpdateWidgetManager.Instance()
                --    updateWidgetManager:Update(UpdateEvent.ModbusChangeParam, "Modbus")
                --end
            end,
        },
        [268] =    --  修正因子
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 268 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.reviseFactor)
            end,

            write = function()
                local offsetAddress = 268 - setting.externalInterface.modbus.holdRegAddr

                if not config.system.solidifyMeaParamFromModbus then
                    local modbusManager = ModbusManager.Instance()
                    local inputValue = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)

                    if inputValue >= 0 and inputValue < 100 then
                        config.measureParam.reviseFactor = inputValue
                    end
                    config.modifyRecord.measureParam(true)
                    ConfigLists.SaveMeasureParamConfig()

                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ModbusChangeParam, "Modbus")
                end
            end,
        },
        [270] =     -- 预留
        {
            number = 2,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [272] =    --  测量控制
        {
            number = 1,	-- Register number
            read = function()
            end,

            write = function()
                local offsetAddress = 272 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                log:debug("远程测量控制 Code = "..optcode)

                local flowManager = FlowManager.Instance()

                if flowManager:IsAuthorize() == true and config.interconnection.reportMode ~= _G.ReportMode.Maintain then
                    if optcode == 0 then
                        if flowManager:IsFlowEnable() == true then
                            status.measure.newResult.measure.isUseStart = false
                            local flow = MeasureFlow:new({}, MeasureType.Sample)
                            flow.name = "MeasureSample"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 1 then
                        if flowManager:IsFlowEnable() == true then
                            status.measure.newResult.measure.isUseStart = false
                            local flow = CalibrateFlow:new({}, CalibrateType.calibrate)
                            flow.name  = "Calibrate"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 2 then
                        flowManager:StopFlow()

                        if config.scheduler.measure.mode == MeasureMode.Continous then
                            config.scheduler.measure.mode = MeasureMode.Trigger
                            config.modifyRecord.scheduler(true)
                            ConfigLists.SaveSchedulerConfig()

                            local updateWidgetManager = UpdateWidgetManager.Instance()
                            updateWidgetManager:Update(UpdateEvent.ChangeMeaModeOnHome, "Modbus")
                        end
                    elseif optcode == 3 then
                        if flowManager:IsFlowEnable() == true then
                            status.measure.newResult.measure.isUseStart = false
                            local flow = MeasureFlow:new({}, MeasureType.Standard)
                            flow.name  = "MeasureRangeCheck"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 4 then
                        if flowManager:IsFlowEnable() == true then
                            status.measure.newResult.measure.isUseStart = false
                            local flow = MeasureFlow:new({}, MeasureType.Blank)
                            flow.name  = "MeasureBlank"
                            flow.isUseStart = true
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 5 then

                    elseif optcode == 6 then

                    elseif optcode == 7 then
                        if flowManager:IsFlowEnable() == true then
                            status.measure.newResult.measure.isUseStart = false
                            local flow = CleanFlow:new({},cleanType.cleanDeeply)
                            flow.name  = "CleanDeeply"
                            flow.isUseStart = true
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 8 then
                        if flowManager:IsFlowEnable() == true then
                            status.measure.newResult.measure.isUseStart = false
                            local flow = CleanFlow:new({},cleanType.cleanAll)
                            flow.name = "CleanAll"
                            flow.isUseStart = true
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 9 then
                        if flowManager:IsFlowEnable() == true then
                            status.measure.newResult.measure.isUseStart = false
                            local flow = CollectSampleFlow:new()
                            flow.name  = "CollectSample"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 10 then
                        if flowManager:IsFlowEnable() == true then
                            status.measure.newResult.measure.isUseStart = false
                            local flow = AutoPumpCheckFlow:new()
                            flow.name = "AutoCalibratePump"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 13 then
                        if flowManager:IsFlowEnable() == true then
                            status.measure.newResult.measure.isUseStart = false
                            local flow = CalibrateFlow:new({}, CalibrateType.onlyCalibrateBlank)
                            flow.name  = "OnlyCalibrateBlank"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 14 then
                        if flowManager:IsFlowEnable() == true then
                            status.measure.newResult.measure.isUseStart = false
                            local flow = CalibrateFlow:new({}, CalibrateType.onlyCalibrateStandard)
                            flow.name  = "OnlyCalibrateStandard"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 20 then
                        if flowManager:IsFlowEnable() == true then
                            status.measure.newResult.measure.isUseStart = false
                            local flow = MeasureFlow:new({}, MeasureType.ZeroCheck)
                            flow.name  = "MeasureZeroCheck"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 21 then
                        if flowManager:IsFlowEnable() == true then
                            status.measure.newResult.measure.isUseStart = false
                            local flow = MeasureFlow:new({}, MeasureType.RangeCheck)
                            flow.name  = "MeasureRangeCheck"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    end
                end
            end,
        },
        [273] =    --  外部采水继电器控制
        {
            number = 1,	-- Register number
            read = function()
            end,

            write = function()
                local offsetAddress = 273 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)

                if config.interconnection.collectSampleMode == setting.mode.collectSample.trigger and config.interconnection.reportMode ~= _G.ReportMode.Maintain then
                    local waterCollector = WaterCollector.Instance()

                    if optcode == 0 then
		            log:debug("远程控制采水继电器 关")
                        if not string.find(config.info.instrument["type"], "PT63P") then
                            waterCollector:TurnOff()
                        end
                        Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, false)
                    elseif optcode == 1 then
		            log:debug("远程控制采水继电器 开")
                        if not string.find(config.info.instrument["type"], "PT63P") then
                            waterCollector:TurnOn()
                        end
                        Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, true)
                    end
                end
            end
        },
        [274] =     -- 系统时间
        {
            number = 3,	-- Register number

            read = function()
                local offsetAddress = 274 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetHexTime(RegisterType.Hold, offsetAddress, os.time() )
            end,

            write = function()
                local offsetAddress = 274 - setting.externalInterface.modbus.holdRegAddr

                if not config.system.solidifyMeaParamFromModbus then
                    local modbusManager = ModbusManager.Instance()
                    modbusManager:SetHexSysTime(RegisterType.Hold, offsetAddress)
                end
            end,
        },
        [277] =     --测量模式
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 277 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.scheduler.measure.mode)
            end,

            write = function()
                local offsetAddress = 277 - setting.externalInterface.modbus.holdRegAddr

                if not config.system.solidifyMeaParamFromModbus then
                    local modbusManager = ModbusManager.Instance()
                    local inputValue = modbusManager:GetShort(RegisterType.Hold, offsetAddress)

                    if inputValue == 0 then
                        config.scheduler.measure.mode = MeasureMode.Trigger
                    elseif inputValue == 1 then
                        config.scheduler.measure.mode = MeasureMode.Periodic
                    elseif inputValue == 2 then
                        config.scheduler.measure.mode = MeasureMode.Timed
                    elseif inputValue == 3 then
                        config.scheduler.measure.mode = MeasureMode.Continous
                    end

                    config.modifyRecord.scheduler(true)
                    ConfigLists.SaveSchedulerConfig()

                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ChangeAutoMeasure, "Modbus")
                end
            end,
        },
        [278] =     --测量间隔设置
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 278 - setting.externalInterface.modbus.holdRegAddr

                local interval = math.floor(config.scheduler.measure.interval)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, interval)
            end,

            write = function()
                local offsetAddress = 278 - setting.externalInterface.modbus.holdRegAddr

                if not config.system.solidifyMeaParamFromModbus then
                    local modbusManager = ModbusManager.Instance()
                    local inputValue = modbusManager:GetShort(RegisterType.Hold, offsetAddress)

                    if inputValue >= 0 and inputValue <= 999 then
                        config.scheduler.measure.interval = inputValue
                    end

                    config.modifyRecord.scheduler(true)
                    ConfigLists.SaveSchedulerConfig()

                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ChangeAutoMeasure, "Modbus")
                end
            end,
        },
        [279] =     --周期校准开关
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 279 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.scheduler.calibrate.mode)
            end,

            write = function()
                local offsetAddress = 279 - setting.externalInterface.modbus.holdRegAddr

                if not config.system.solidifyMeaParamFromModbus then
                    local modbusManager = ModbusManager.Instance()
                    local inputValue = modbusManager:GetShort(RegisterType.Hold, offsetAddress)

                    if inputValue == 0 then
                        config.scheduler.calibrate.mode = MeasureMode.Trigger
                    elseif inputValue == 1 then
                        config.scheduler.calibrate.mode = MeasureMode.Periodic
                    elseif inputValue == 2 then
                        config.scheduler.calibrate.mode = MeasureMode.Timed
                    end

                    config.modifyRecord.scheduler(true)
                    ConfigLists.SaveSchedulerConfig()

                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ChangeAutoMeasure, "Modbus")
                end
            end,
        },
        [280] =     --校准间隔设置
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 280 - setting.externalInterface.modbus.holdRegAddr

                local interval = math.floor(config.scheduler.calibrate.interval)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, interval)
            end,

            write = function()
                local offsetAddress = 280 - setting.externalInterface.modbus.holdRegAddr

                if not config.system.solidifyMeaParamFromModbus then
                    local modbusManager = ModbusManager.Instance()
                    local inputValue = modbusManager:GetShort(RegisterType.Hold, offsetAddress)

                    if inputValue >= 0 and inputValue <= 999 then
                        config.scheduler.calibrate.interval = inputValue
                    end

                    config.modifyRecord.scheduler(true)
                    ConfigLists.SaveSchedulerConfig()

                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ChangeAutoMeasure, "Modbus")
                end
            end,
        },
        [281] =     --当前量程校正因子
        {
            number = 2,	-- Register number

            read = function()
                --local offsetAddress = 281 - setting.externalInterface.modbus.holdRegAddr
                --
                --local currentRange = config.measureParam.range[config.measureParam.currentRange+1] + 1
                --local value = config.measureParam.reviseParameter[currentRange]
                --if value == nil then
                --    value = 0
                --end
                --
                --local modbusManager = ModbusManager.Instance()
                --modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [283] =     --当前量程稀释倍数
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 283 - setting.externalInterface.modbus.holdRegAddr

                local value = 0
                local currentRange = config.measureParam.range[config.measureParam.currentRange + 1] + 1

                if setting.measure.range[currentRange].diluteFactor ~= nil then
                    value = setting.measure.range[currentRange].diluteFactor
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [285] =     --285 ~ 292 预留
        {
            number = 8,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [293] =     -- 当前零点校准液浓度
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 293 - setting.externalInterface.modbus.holdRegAddr

                local value = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange+1]+1].ZeroConsistency

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [295] =     --当前量程校准液浓度
        {
            number = 5,	-- Register number

            read = function()
                local offsetAddress = 295 - setting.externalInterface.modbus.holdRegAddr

                local value = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange+1]+1].RangeConsistency

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [297] =     -- 297 - 299 预留
        {
            number = 3,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [300] =     --  试剂加液体积表
        {
            number = 10,	-- Register number

            read = function()
                local offsetAddress = 300 - setting.externalInterface.modbus.holdRegAddr

                local reagentVolume = {0,0,0,0,0}

                if setting.liquid.reagent1Volume ~= nil then
                    reagentVolume[1] = setting.liquid.reagent1Volume
                end
                if setting.liquid.reagent2Volume ~= nil then
                    reagentVolume[2] = setting.liquid.reagent2Volume
                end
                if setting.liquid.reagent3Volume ~= nil then
                    reagentVolume[3] = setting.liquid.reagent3Volume
                end
                if setting.liquid.reagent4Volume ~= nil then
                    reagentVolume[4] = setting.liquid.reagent4Volume
                end
                if setting.liquid.reagent5Volume ~= nil then
                    reagentVolume[5] = setting.liquid.reagent5Volume
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, reagentVolume[1])
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress + 2, reagentVolume[2])
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress + 4, reagentVolume[3])
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress + 6, reagentVolume[4])
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress + 8, reagentVolume[5])
            end,

            write = function()
            end,
        },
        [310] =     --  量程校正因子表
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 310 - setting.externalInterface.modbus.holdRegAddr
                local value = 0
                if config.measureParam.reviseParameter[config.measureParam.range[config.measureParam.currentRange + 1]+1] ~= nil then
                    value = config.measureParam.reviseParameter[config.measureParam.range[config.measureParam.currentRange + 1]+1]
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [312] = --标样核查允许偏差值
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 312 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local ret = 0
                if config.measureParam.checkErrorLimit ~= nil then
                    ret = config.measureParam.checkErrorLimit
                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, ret*100)
            end,
            write = function()
            end,
        },
        [314] = --标样核查偏差值
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 314 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local ret = 0
                if status.measure.report.rangeCheck.checkError ~= nil then
                    ret = status.measure.report.rangeCheck.checkError
                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, ret*100)
            end,
            write = function()
            end,
        },
        [316] =     -- 316 - 511 预留
        {
            number = 196,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
    }
}
