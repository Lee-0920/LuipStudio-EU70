setting.externalInterface = {}

setting.externalInterface.modbus =
{
    inputRegAddr = 0,
    inputRegNum = 1000,
    holdRegAddr = 1000,
    holdRegNum = 2000,

    registers =
    {
        [0] =  --空余
        {
            number = 1000,

            read = function()

            end,
            write = function()

            end,
        },
        [1000] = --TC测量结果
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress =  1000 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, status.measure.newResult.measure.consistencyTC)
            end,

            write = function()
            end,
        },
        [1002] = --IC测量结果
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1002 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, status.measure.newResult.measure.consistencyIC)
            end,

            write = function()
            end,
        },
        [1004] = --TC测量结果
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1004 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, status.measure.newResult.measure.consistency)

            end,

            write = function()
            end,
        },
        [1006] = --TC测量峰值
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1006 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, status.measure.newResult.measure.peak)
            end,

            write = function()
            end,
        },
        [1008] = --IC测量峰值
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1008 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, status.measure.newResult.measure.peakIC)
            end,

            write = function()
            end,
        },
        [1010] = --测量时长
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1010 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, status.measure.report.measure.measureTime)
            end,

            write = function()
            end,
        },
        [1011] = --测量时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 1011 - setting.externalInterface.modbus.holdRegAddr

                local time = status.measure.report.measure.dateTime
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [1014] = --系统状态
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1014 - setting.externalInterface.modbus.holdRegAddr

                local statusManager = StatusManager.Instance()
                local runStatus = statusManager:GetStatus()
                local name = runStatus:GetName()

                local modbusManager = ModbusManager.Instance()
                if setting.modbusCoder.statusID[name] ~= nil then
                    modbusManager:SetShort(RegisterType.Hold, offsetAddress, setting.modbusCoder.statusID[name].ID)
                else
                    modbusManager:SetShort(RegisterType.Hold, offsetAddress, 0)
                end
            end,

            write = function()
            end,
        },
        [1015] = --当前动作
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1015 - setting.externalInterface.modbus.holdRegAddr
                local statusManager = StatusManager.Instance()
                local action = statusManager:GetAction()
                local name = action:GetName()

                local modbusManager = ModbusManager.Instance()

                if setting.modbusCoder.actionID[name] ~= nil then
                    modbusManager:SetShort(RegisterType.Hold, offsetAddress, setting.modbusCoder.actionID[name].ID)
                else
                    modbusManager:SetShort(RegisterType.Hold, offsetAddress, 0)
                end
            end,

            write = function()
            end,
        },
        [1016] = --当前温度
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1016 - setting.externalInterface.modbus.holdRegAddr

                local eTemp = dc:GetEnvironmentTemperature()

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, eTemp)
            end,

            write = function()
            end,
        },
        [1018] = --报警代码
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1018 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local AlarmManager = AlarmManager.Instance()
                local alarm = AlarmManager:GetCurrentAlarm()
                local name = alarm:GetName()
                local cause = alarm:GetCause()

                local alarmVal = 0
                if name == "空闲" or name == "" then
                    alarmVal = 0
                else
                    alarmVal = 0   --仪表内部其它异常
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, alarmVal)
            end,

            write = function()
            end,
        },
        [1019] = --系统时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 1019 - setting.externalInterface.modbus.holdRegAddr


                local time = os.time()
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [1022] = --告警时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 1022 - setting.externalInterface.modbus.holdRegAddr

                local time = os.time()
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [1025] =     -- 1025 - 1035 预留
        {
            number = 11,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [1036] =     --  产品名称
        {
            number = 32,	-- Register number

            read = function()
                local offsetAddress = 1036 - setting.externalInterface.modbus.holdRegAddr
                local regSize = setting.externalInterface.modbus.registers[1036].number

                local modbusManager = ModbusManager.Instance()
                local str = setting.instrument.name

                modbusManager:SetString(RegisterType.Hold, offsetAddress, str, regSize)
            end,

            write = function()
            end,
        },
        [1068] =     --  产品型号
        {
            number = 32,	-- Register number

            read = function()
                local offsetAddress = 1068 - setting.externalInterface.modbus.holdRegAddr
                local regSize = setting.externalInterface.modbus.registers[1068].number

                local modbusManager = ModbusManager.Instance()
                local str = config.info.instrument["model"]

                modbusManager:SetString(RegisterType.Hold, offsetAddress, str, regSize)
            end,

            write = function()
            end,
        },
        [1100] =     --  生产厂商
        {
            number = 32,	-- Register number

            read = function()
                local offsetAddress = 1100 - setting.externalInterface.modbus.holdRegAddr
                local regSize = setting.externalInterface.modbus.registers[1100].number

                local modbusManager = ModbusManager.Instance()
                local str = config.info.instrument["manuFacturer"]

                modbusManager:SetString(RegisterType.Hold, offsetAddress, str, regSize)
            end,

            write = function()
            end,
        },
        [1132] =     -- 生产序列号
        {
            number = 16,	-- Register number

            read = function()
                local offsetAddress = 1132 - setting.externalInterface.modbus.holdRegAddr
                local regSize = setting.externalInterface.modbus.registers[1132].number

                local modbusManager = ModbusManager.Instance()
                local str = config.info.instrument["sn"]

                modbusManager:SetString(RegisterType.Hold, offsetAddress, str, regSize)
            end,

            write = function()
            end,
        },
        [1148] =     --  主控板固件版本
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1148 - setting.externalInterface.modbus.holdRegAddr

                local ver = Version.new(setting.version.software)

                local major = ver:GetMajor()
                local minor = ver:GetMinor()
                local build  = ver:GetBuild()
                local revision  = ver:GetRevision()
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, major * 256 + minor)
                modbusManager:SetShort(RegisterType.Hold, offsetAddress + 1, revision * 256 + build)

                ver = nil
            end,

            write = function()
            end,
        },
        [1150] =     --   驱动板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if dc ~= nil then
                    local offsetAddress = 1150 - setting.externalInterface.modbus.holdRegAddr

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
                        modbusManager:SetShort(RegisterType.Hold, offsetAddress, major * 256 + minor)
                        modbusManager:SetShort(RegisterType.Hold, offsetAddress + 1, revision * 256 + build)
                    end
                end
            end,

            write = function()
            end,
        },
        [1152] =     --   液路板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if lc ~= nil then
                    local offsetAddress = 1152 - setting.externalInterface.modbus.holdRegAddr

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
                        modbusManager:SetShort(RegisterType.Hold, offsetAddress, major * 256 + minor)
                        modbusManager:SetShort(RegisterType.Hold, offsetAddress + 1, revision * 256 + build)
                    end
                end
            end,

            write = function()
            end,
        },
        [1154] =     --   信号板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if rc ~= nil then
                    local offsetAddress = 1154 - setting.externalInterface.modbus.holdRegAddr

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
                        modbusManager:SetShort(RegisterType.Hold, offsetAddress, major * 256 + minor)
                        modbusManager:SetShort(RegisterType.Hold, offsetAddress + 1, revision * 256 + build)
                    end
                end
            end,

            write = function()
            end,
        },
        [1156] =     --   输出板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if oc ~= nil then
                    local offsetAddress = 1158 - setting.externalInterface.modbus.holdRegAddr

                    local ver = 0
                    local major = 0
                    local minor = 0
                    local build  = 0
                    local revision  = 0
                    local err,result = pcall(function()
                        if oc:GetConnectStatus() == true then
                            ver = oc:GetIDeviceInfo():GetSoftwareVersion()
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
                        modbusManager:SetShort(RegisterType.Hold, offsetAddress, major * 256 + minor)
                        modbusManager:SetShort(RegisterType.Hold, offsetAddress + 1, revision * 256 + build)
                    end
                end
            end,

            write = function()
            end,
        },
        [1158] = --设备序列号
        {
            number = 6, -- Register number

            read = function()
                local offsetAddress = 10 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                --[[
                                --hex:3575 BCD1 5BC6 14E2 DFDC 1C35
                                local head = 53   --8bit
                                local firm = 123456789   --28bit
                                local class = 12345678  --24bit
                                local sn = tonumber(string.sub(config.info.instrument.sn, 2, 9))  --36bit

                                local reg1 = (head << 8) + (firm >> 20)
                                local reg2 = (firm >> 4) & (2^16 - 1)
                                local reg3 = ((firm & (2^4 - 1)) << 12) + (class >> 12)
                                local reg4 = ((class & (2^12 - 1)) << 4) + (sn >> 32)
                                local reg5 = (sn & (2^32 - 1)) >> 16
                                local reg6 = (sn & (2^16 - 1))
                --]]
                --hex:X24C 3ADC 8XXX XXXX XXXX XXXX
                local reg1 = 0x024C
                local reg2 = 0x3ADC
                local reg3 = 0x8000
                local reg4 = 0x0000
                local reg5 = 0x0000
                local reg6 = 0x0000

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, reg1)
                modbusManager:SetShort(RegisterType.Hold, offsetAddress + 1, reg2)
                modbusManager:SetShort(RegisterType.Hold, offsetAddress + 2, reg3)
                modbusManager:SetShort(RegisterType.Hold, offsetAddress + 3, reg4)
                modbusManager:SetShort(RegisterType.Hold, offsetAddress + 4, reg5)
                modbusManager:SetShort(RegisterType.Hold, offsetAddress + 5, reg6)
            end,

            write = function()
            end,
        },
        [1165] =     --  弹窗信息
        {
            number = 32,	-- Register number

            read = function()
                local offsetAddress = 1165 - setting.externalInterface.modbus.holdRegAddr
                local regSize = setting.externalInterface.modbus.registers[1165].number

                local modbusManager = ModbusManager.Instance()
                local str = status.measure.messageDialogStr

                modbusManager:SetString(RegisterType.Hold, offsetAddress, str, regSize)
            end,

            write = function()
            end,
        },
        [1197] = --耗材管理 泵周期
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1197 - setting.externalInterface.modbus.holdRegAddr

                local ret = config.consumable.pump.cycle
                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1197 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    config.consumable.pump.cycle = value
                    ConfigLists.SaveConsumableConfig()
                end
            end,
        },
        [1198] = --耗材管理-泵-更换时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 1198 - setting.externalInterface.modbus.holdRegAddr

                local time = config.consumable.pump.lastTime
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [1201] = --耗材管理-泵-过期时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 1201 - setting.externalInterface.modbus.holdRegAddr

                local lastTime = os.date("*t", config.consumable.pump.lastTime)
                lastTime.month = lastTime.month + config.consumable.pump.cycle
                local time = os.time(lastTime)

                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [1204] = --耗材管理 紫外灯周期
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1204 - setting.externalInterface.modbus.holdRegAddr

                local ret = config.consumable.uvLamp.cycle
                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1204 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    config.consumable.uvLamp.cycle = value
                    ConfigLists.SaveConsumableConfig()
                end
            end,
        },
        [1205] = --耗材管理-紫外灯-更换时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 1205 - setting.externalInterface.modbus.holdRegAddr

                local time = config.consumable.uvLamp.lastTime
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [1208] = --耗材管理-泵-过期时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 1208 - setting.externalInterface.modbus.holdRegAddr

                local lastTime = os.date("*t", config.consumable.uvLamp.lastTime)
                lastTime.month = lastTime.month + config.consumable.uvLamp.cycle
                local time = os.time(lastTime)

                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [1211] = --耗材管理 树脂层周期
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1211 - setting.externalInterface.modbus.holdRegAddr

                local ret = config.consumable.resin.cycle
                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1211 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    config.consumable.resin.cycle = value
                    ConfigLists.SaveConsumableConfig()
                end
            end,
        },
        [1212] = --耗材管理-树脂层-更换时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 1212 - setting.externalInterface.modbus.holdRegAddr

                local time = config.consumable.resin.lastTime
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [1215] = --耗材管理-树脂层-过期时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 1215 - setting.externalInterface.modbus.holdRegAddr

                local lastTime = os.date("*t", config.consumable.uvLamp.lastTime)
                lastTime.month = lastTime.month + config.consumable.resin.cycle
                local time = os.time(lastTime)

                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [1218] =     -- 1218 - 1255 预留
        {
            number = 38,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [1256] = --曲线斜率
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1256 - setting.externalInterface.modbus.holdRegAddr

                local curveK = config.measureParam.curveK
                local curveB = config.measureParam.curveB
                curveK, curveB  = CurveParamCurveXYChange(curveK, curveB)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, curveK)
            end,

            write = function()
                local offsetAddress = 1256 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                value = string.format("%.4f", value)

                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(tostring(value)) == true then
                    setting.ui.profile.measureParam[1][1].currentValue = tonumber(value)
                    config.measureParam.curveK = tonumber(value)
                    setting.ui.profile.measureParam.checkCurveParamChange()
                    config.modifyRecord.measureParam(true)
                    ConfigLists.SaveMeasureParamConfig()
                end
            end,
        },
        [1258] = --曲线截距
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1258 - setting.externalInterface.modbus.holdRegAddr

                local curveK = config.measureParam.curveK
                local curveB = config.measureParam.curveB
                curveK, curveB  = CurveParamCurveXYChange(curveK, curveB)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, curveB)
            end,

            write = function()
                local offsetAddress = 1258 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                value = string.format("%.4f", value)

                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(tostring(value)) == true then
                    setting.ui.profile.measureParam[1][2].currentValue = tonumber(value)
                    config.measureParam.curveB = tonumber(value)
                    setting.ui.profile.measureParam.checkCurveParamChange()
                    config.modifyRecord.measureParam(true)
                    ConfigLists.SaveMeasureParamConfig()
                end
            end,
        },
        [1260] = --定标时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 1260 - setting.externalInterface.modbus.holdRegAddr

                local time = status.measure.newResult.calibrate.dateTime
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [1263] = --负值修正
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1263 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                if config.measureParam.negativeRevise == true then
                    ret = 1
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1263 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode == 0 then
                    config.measureParam.negativeRevise = false
                elseif mode == 1 then
                    config.measureParam.negativeRevise = true
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1264] = --斜率修正
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1264 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.reviseFactor)
            end,

            write = function()
                local offsetAddress = 1264 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value ~= 0 then
                    value = string.format("%.4f", value)
                    config.measureParam.reviseFactor = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1266] = --截距修正
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1266 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.shiftFactor)
            end,

            write = function()
                local offsetAddress = 1266 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value ~= nil then
                    value = string.format("%.4f", value)
                    config.measureParam.shiftFactor = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1268] = --偏移量
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1268 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.measureDataOffsetValve)
            end,

            write = function()
                local offsetAddress = 1268 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value ~= nil then
                    value = string.format("%.4f", value)
                    config.measureParam.measureDataOffsetValve = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1270] = --修正因子
        {
            number = 2, -- Register number

            read = function()

            end,

            write = function()

            end,
        },
        [1272] = --药典设置
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1272 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                for i = 1,7 do
                    if config.measureParam.pharmacopoeia[i] == true then
                    ret = ret | (1 << (i-1))
                    end
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
                end,

            write = function()
                local offsetAddress = 1272 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)

                for i = 1,7 do
                    if (value & (1 << (i-1))) > 0 then
                        config.measureParam.pharmacopoeia[i] = true
                    else
                        config.measureParam.pharmacopoeia[i] = false
                    end
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1273] = --Turbo模式
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1273 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                if config.measureParam.turboMode == true then
                    ret = 1
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1273 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode == 0 then
                    config.measureParam.turboMode = false
                elseif mode == 1 then
                    config.measureParam.turboMode = true
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1274] = --ICR模式
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1274 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                if config.measureParam.ICRMode == true then
                    ret = 1
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1274 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode == 0 then
                    config.measureParam.ICRMode = false
                elseif mode == 1 then
                    config.measureParam.ICRMode = true
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1275] = --电导率模式
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1275 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                if config.measureParam.ECMode == true then
                    ret = 1
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1275 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode == 0 then
                    config.measureParam.ECMode = false
                elseif mode == 1 then
                    config.measureParam.ECMode = true
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1276] = --酸剂
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1276 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.reagent1Vol)
            end,

            write = function()
                local offsetAddress = 1276 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    value = string.format("%.4f", value)
                    config.measureParam.reagent1Vol = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1278] = --氧化剂
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1278 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.reagent2Vol)
            end,

            write = function()
                local offsetAddress = 1278 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    value = string.format("%.4f", value)
                    config.measureParam.reagent2Vol = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1280] = --慢速冲洗时间
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1280 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.measureParam.normalRefreshTime)
            end,

            write = function()
                local offsetAddress = 1280 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    value = string.format("%.0f", value)
                    config.measureParam.normalRefreshTime = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1281] = --快速冲洗时间
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1281 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.measureParam.quickRefreshTime)
            end,

            write = function()
                local offsetAddress = 1281 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    value = string.format("%.0f", value)
                    config.measureParam.quickRefreshTime = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1282] = --水样混合时间
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1282 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.measureParam.mixSampleTime)
            end,

            write = function()
                local offsetAddress = 1282 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    value = string.format("%.0f", value)
                    config.measureParam.mixSampleTime = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1283] = --反应时间
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1283 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.measureParam.reactTime)
            end,

            write = function()
                local offsetAddress = 1283 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    value = string.format("%.0f", value)
                    config.measureParam.reactTime = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1284] = --TC电导池常数
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1284 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.TCConstant)
            end,

            write = function()
                local offsetAddress = 1284 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    value = string.format("%.4f", value)
                    config.measureParam.TCConstant = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1286] = --IC电导池常数
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1286 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.ICConstant)
            end,

            write = function()
                local offsetAddress = 1286 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    value = string.format("%.4f", value)
                    config.measureParam.ICConstant = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1288] = --TC斜率
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1288 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.TCCurveK)
            end,

            write = function()
                local offsetAddress = 1288 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    value = string.format("%.4f", value)
                    config.measureParam.TCCurveK = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1290] = --IC斜率
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1290 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.ICCurveK)
            end,

            write = function()
                local offsetAddress = 1290 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    value = string.format("%.4f", value)
                    config.measureParam.ICCurveK = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()

                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1292] = --TOC模式
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1292 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                if config.measureParam.TOCMode == true then
                    ret = 1
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1292 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode == 0 then
                    config.measureParam.TOCMode = false
                elseif mode == 1 then
                    config.measureParam.TOCMode = true
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1293] = --自动加试剂
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1293 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                if config.measureParam.autoReagent == true then
                    ret = 1
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1293 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode == 0 then
                    config.measureParam.autoReagent = false
                elseif mode == 1 then
                    config.measureParam.autoReagent = true
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1294] = --测量次数(离线)
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1294 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.measureParam.measureTimes)
            end,

            write = function()
                local offsetAddress = 1294 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    value = string.format("%.0f", value)
                    config.measureParam.measureTimes = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1295] = --舍弃次数(离线)
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1295 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.measureParam.rejectTimes)
            end,

            write = function()
                local offsetAddress = 1295 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    value = string.format("%.0f", value)
                    config.measureParam.rejectTimes = tonumber(value)
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1296] = --1295-1320 预留
        {
            number = 25, -- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [1321] = --测量模式(1连续 2周期 3定点 4受控 5手动)
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1321 - setting.externalInterface.modbus.holdRegAddr

                local measureMode = 4
                if config.scheduler.measure.mode == MeasureMode.Continous then
                    measureMode = 1
                elseif config.scheduler.measure.mode == MeasureMode.Periodic then
                    measureMode = 2
                elseif config.scheduler.measure.mode == MeasureMode.Timed then
                    measureMode = 3
                elseif config.scheduler.measure.mode == MeasureMode.Trigger then
                    measureMode = 4
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, measureMode)

            end,

            write = function()
                local offsetAddress = 1321 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local inputValue = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if inputValue == 1 then
                    config.scheduler.measure.mode = MeasureMode.Continous
                elseif inputValue == 2 then
                    config.scheduler.measure.mode = MeasureMode.Periodic
                elseif inputValue == 3 then
                    config.scheduler.measure.mode = MeasureMode.Timed
                elseif inputValue == 4 then
                    config.scheduler.measure.mode = MeasureMode.Trigger
                end
                config.modifyRecord.scheduler(true)
                ConfigLists.SaveSchedulerConfig()

                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeAutoMeasure, "Modbus")
            end,
        },
        [1322] = -- 测量间隔(分钟)
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1322 - setting.externalInterface.modbus.holdRegAddr

                local temp = math.ceil(config.scheduler.measure.interval * 60)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, temp)
            end,

            write = function()
                local offsetAddress = 1322 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local inputValue = modbusManager:GetShort(RegisterType.Hold, offsetAddress)

                if inputValue >= 0 and inputValue <= 999 then
                    config.scheduler.measure.interval = inputValue / 60
                end

                config.modifyRecord.scheduler(true)
                ConfigLists.SaveSchedulerConfig()

                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeAutoMeasure, "Modbus")
            end,
        },
        [1323] = -- 整点延长判定(秒)
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1323 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.scheduler.timedPointJudgeTime)
            end,

            write = function()
                local offsetAddress = 1323 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    config.scheduler.timedPointJudgeTime = value
                end

                config.modifyRecord.scheduler(true)
                ConfigLists.SaveSchedulerConfig()

                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeAutoMeasure, "Modbus")
            end,
        },
        [1324] = -- 整点设置
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1324 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                for i = 1,24 do
                    if config.scheduler.measure.timedPoint[i] == true then
                        ret = ret | (1 << (i-1))
                    end
                end

                modbusManager:SetInt(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1324 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetInt(RegisterType.Hold, offsetAddress)

                for i = 1,24 do
                    if (value & (1 << (i-1))) > 0 then
                        config.scheduler.measure.timedPoint[i] = true
                    else
                        config.scheduler.measure.timedPoint[i] = false
                    end
                end

                config.modifyRecord.scheduler(true)
                ConfigLists.SaveSchedulerConfig()

                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeAutoMeasure, "Modbus")
            end,
        },
        [1326] =     -- 1326 - 1336 预留
        {
            number = 11,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },

        [1337] = -- 故障停机
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1337 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                if config.system.faultBlocking == true then
                    ret = 1
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1337 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode == 0 then
                    config.system.faultBlocking = false
                elseif mode == 1 then
                    config.system.faultBlocking = true
                end

                config.modifyRecord.system(true)
                ConfigLists.SaveSystemConfig()

                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeSystemParam, "Modbus")
            end,
        },
        [1338] = -- 异常重测
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1338 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                if config.system.faultRetry == true then
                    ret = 1
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1338 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode == 0 then
                    config.system.faultRetry = false
                elseif mode == 1 then
                    config.system.faultRetry = true
                end

                config.modifyRecord.system(true)
                ConfigLists.SaveSystemConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeSystemParam, "Modbus")
            end,
        },
        [1339] = -- 更换试剂提醒
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1339 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                if config.system.reagentLackWarn == true then
                    ret = 1
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1339 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode == 0 then
                    config.system.reagentLackWarn = false
                elseif mode == 1 then
                    config.system.reagentLackWarn = true
                end

                config.modifyRecord.system(true)
                ConfigLists.SaveSystemConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeSystemParam, "Modbus")
            end,
        },
        [1340] = -- 语言
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1340 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.system.language)
            end,

            write = function()
                local offsetAddress = 1340 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value < 3 then
                    config.system.language = value
                end

                config.modifyRecord.system(true)
                ConfigLists.SaveSystemConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeSystemParam, "Modbus")
            end,
        },
        [1341] = -- 干接点触发
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1341 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                if config.system.adcDetect[2].enable == true then
                    ret = 1
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1341 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode == 0 then
                    config.system.adcDetect[2].enable = false
                elseif mode == 1 then
                    config.system.adcDetect[2].enable = true
                end

                config.modifyRecord.system(true)
                ConfigLists.SaveSystemConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeSystemParam, "Modbus")
            end,
        },
        [1342] =     -- 1342 - 1369 预留
        {
            number = 28,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [1370] = -- TOC超标告警
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1370 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                if config.interconnection.alarmValue == true then
                    ret = 1
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1370 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode == 0 then
                    config.interconnection.alarmValue = false
                elseif mode == 1 then
                    config.interconnection.alarmValue = true
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1371] = --TOC超标上限
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1371 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.interconnection.meaUpLimit)
            end,

            write = function()
                local offsetAddress = 1371 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    config.interconnection.meaUpLimit = value
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1373] = --TOC超标下限
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1373 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.interconnection.meaLowLimit)
            end,

            write = function()
                local offsetAddress = 1373 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    config.interconnection.meaLowLimit = value
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1375] = -- TC超标告警
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1375 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                if config.interconnection.alarmValueTC == true then
                    ret = 1
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1375 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode == 0 then
                    config.interconnection.alarmValueTC = false
                elseif mode == 1 then
                    config.interconnection.alarmValueTC = true
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1376] = --TC超标上限
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1376 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.interconnection.meaUpLimitTC)
            end,

            write = function()
                local offsetAddress = 1376 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    config.interconnection.meaUpLimitTC = value
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1378] = --TC超标下限
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1378 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.interconnection.meaLowLimitTC)
            end,

            write = function()
                local offsetAddress = 1378 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    config.interconnection.meaLowLimitTC = value
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1380] = -- IC超标告警
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1380 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local modbusManager = ModbusManager.Instance()
                if config.interconnection.alarmValueIC == true then
                    ret = 1
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
                local offsetAddress = 1380 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode == 0 then
                    config.interconnection.alarmValueIC = false
                elseif mode == 1 then
                    config.interconnection.alarmValueIC = true
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1381] = --IC超标上限
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1381 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.interconnection.meaUpLimitIC)
            end,

            write = function()
                local offsetAddress = 1381 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    config.interconnection.meaUpLimitIC = value
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1383] = --IC超标下限
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1383 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.interconnection.meaLowLimitIC)
            end,

            write = function()
                local offsetAddress = 1383 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    config.interconnection.meaLowLimitIC = value
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1385] = -- RS485波特率
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1385 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.interconnection.RS485BaudRate)
            end,

            write = function()
                local offsetAddress = 1385 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode < 5 then
                    config.interconnection.RS485BaudRate = mode
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1386] = -- RS485校验位
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1386 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.interconnection.RS485Parity)
            end,

            write = function()
                local offsetAddress = 1386 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode < 3 then
                    config.interconnection.RS485Parity = mode
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1387] = -- RS232波特率
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1387 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.interconnection.RS232BaudRate)
            end,

            write = function()
                local offsetAddress = 1387 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode < 5 then
                    config.interconnection.RS232BaudRate = mode
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1388] = -- RS232校验位
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1388 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.interconnection.RS232Parity)
            end,

            write = function()
                local offsetAddress = 1388 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode < 3 then
                    config.interconnection.RS232Parity = mode
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1389] = -- Modbus通信地址
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1389 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.interconnection.connectAddress)
            end,

            write = function()
                local offsetAddress = 1389 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value < 255 then
                    config.interconnection.connectAddress = value
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1390] = --TOC 4mA对应浓度
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1390 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.interconnection.sampleLowLimit)
            end,

            write = function()
                local offsetAddress = 1390 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    config.interconnection.sampleLowLimit = value
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1392] = --TOC 20mA对应浓度
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1392 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.interconnection.sampleUpLimit)
            end,

            write = function()
                local offsetAddress = 1392 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    config.interconnection.sampleUpLimit = value
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1394] = --TC 4mA对应浓度
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1394 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.interconnection.sampleLowLimitTC)
            end,

            write = function()
                local offsetAddress = 1394 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    config.interconnection.sampleLowLimitTC = value
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1396] = --TC 20mA对应浓度
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1396 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.interconnection.sampleUpLimitTC)
            end,

            write = function()
                local offsetAddress = 1396 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    config.interconnection.sampleUpLimitTC = value
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1398] = --IC 4mA对应浓度
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1398 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.interconnection.sampleLowLimitIC)
            end,

            write = function()
                local offsetAddress = 1398 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    config.interconnection.sampleLowLimitIC = value
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1400] = --IC 20mA对应浓度
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 1400 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.interconnection.sampleUpLimitIC)
            end,

            write = function()
                local offsetAddress = 1400 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)
                if value >= 0 then
                    config.interconnection.sampleUpLimitIC = value
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1402] = -- 继电器1
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1402 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.interconnection.relayOne)
            end,

            write = function()
                local offsetAddress = 1402 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode < 5 then
                    config.interconnection.relayOne = mode
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1403] = -- 继电器2
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1403 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.interconnection.relayTwo)
            end,

            write = function()
                local offsetAddress = 1403 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode < 5 then
                    config.interconnection.relayTwo = mode
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1404] = -- 继电器3
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1404 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.interconnection.relayThree)
            end,

            write = function()
                local offsetAddress = 1404 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode < 5 then
                    config.interconnection.relayThree = mode
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1405] = -- 继电器1
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1405 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.interconnection.relayFour)
            end,

            write = function()
                local offsetAddress = 1405 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode < 5 then
                    config.interconnection.relayFour = mode
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1406] = -- 网络设置
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1406 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.interconnection.settingIPMode)
            end,

            write = function()
                local offsetAddress = 1406 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if mode < 2 then
                    config.interconnection.settingIPMode = mode
                end

                config.modifyRecord.interconnection(true)
                ConfigLists.SaveInterconnectionConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
            end,
        },
        [1407] = -- 1407-1420 预留
        {
            number = 13, -- Register number

            read = function()
            end,

            write = function()
            end,
        },


        [1420] = -- 维护控制命令
        {
            number = 1, -- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 1420 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                log:debug("ModbusManager PC optcode = "..optcode)

                local flowManager = FlowManager.Instance()
                if (optcode >= 0 and optcode <= 21)  then
                    if flowManager:IsAuthorize() == true  then

                        if optcode == 0 then
                            flowManager:StopFlow()  --停止

                            if config.scheduler.measure.mode == MeasureMode.Continous then
                                config.scheduler.measure.mode = MeasureMode.Trigger
                                config.modifyRecord.scheduler(true)
                                ConfigLists.SaveSchedulerConfig()

                                local updateWidgetManager = UpdateWidgetManager.Instance()
                                updateWidgetManager:Update(UpdateEvent.ChangeMeaModeOnHome, "Modbus")
                            end
                        elseif optcode == 1 then
                            config.scheduler.measure.mode = MeasureMode.Continous
                            config.modifyRecord.scheduler(true)
                            ConfigLists.SaveSchedulerConfig()

                            local updateWidgetManager = UpdateWidgetManager.Instance()
                            updateWidgetManager:Update(UpdateEvent.ChangeAutoMeasure, "Modbus")
                        elseif optcode == 2 then
                            --多点校准
                            if flowManager:IsFlowEnable() == true then
                                log:debug("Maintain createFlow ==> MulCalibrateFlow")
                                local flow = CalibrateFlow:new({}, CalibrateType.mulCalibrate)
                                flow.name  = "MulCalibrate"
                                flow.text = "多点校准"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 3 then
                            --单点校准(1ppm)
                            if flowManager:IsFlowEnable() == true then
                                local flow = CalibrateFlow:new({}, CalibrateType.calibrate, 1)
                                flow.name  = "Calibrate"
                                flow.text = "单点校准(1ppm)"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 4 then
                            --单点校准(5ppm)
                            if flowManager:IsFlowEnable() == true then
                                local flow = CalibrateFlow:new({}, CalibrateType.calibrate, 5)
                                flow.name  = "Calibrate"
                                flow.text = "单点校准(5ppm)"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 5 then
                            --单点校准(10ppm)
                            if flowManager:IsFlowEnable() == true then
                                local flow = CalibrateFlow:new({}, CalibrateType.calibrate, 10)
                                flow.name  = "Calibrate"
                                flow.text = "单点校准(10ppm)"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 6 then
                            --单点校准(25ppm)
                            if flowManager:IsFlowEnable() == true then
                                local flow = CalibrateFlow:new({}, CalibrateType.calibrate, 25)
                                flow.name  = "Calibrate"
                                flow.text = "单点校准(25ppm)"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 7 then
                            --单点校准(50ppm)
                            if flowManager:IsFlowEnable() == true then
                                local flow = CalibrateFlow:new({}, CalibrateType.calibrate, 50)
                                flow.name  = "Calibrate"
                                flow.text = "单点校准(50ppm)"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 8 then
                            --单点确认(500ppb)
                            if flowManager:IsFlowEnable() == true then
                                local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 0.5)
                                flow.name  = "SinglePoint"
                                flow.text = "单点确认(500ppb)"
                                flowManager:StartFlow()
                            end
                        elseif optcode == 9 then
                            --单点确认(1ppm)
                            if flowManager:IsFlowEnable() == true then
                                local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 1)
                                flow.name  = "SinglePoint"
                                flow.text = "单点确认(1ppm)"
                                flowManager:StartFlow()
                            end
                        elseif optcode == 10 then
                            --单点确认(2ppm)
                            if flowManager:IsFlowEnable() == true then
                                local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 2)
                                flow.name  = "SinglePoint"
                                flow.text = "单点确认(2ppm)"
                                flowManager:StartFlow()
                            end
                        elseif optcode == 11 then
                            --单点确认(5ppm)
                            if flowManager:IsFlowEnable() == true then
                                local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 5)
                                flow.name  = "SinglePoint"
                                flow.text = "单点确认(5ppm)"
                                flowManager:StartFlow()
                            end
                        elseif optcode == 12 then
                            --单点确认(10ppm)
                            if flowManager:IsFlowEnable() == true then
                                local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 10)
                                flow.name  = "SinglePoint"
                                flow.text = "单点确认(10ppm)"
                                flowManager:StartFlow()
                            end
                        elseif optcode == 13 then
                            --单点确认(25ppm)
                            if flowManager:IsFlowEnable() == true then
                                local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 25)
                                flow.name  = "SinglePoint"
                                flow.text = "单点确认(25ppm)"
                                flowManager:StartFlow()
                            end
                        elseif optcode == 14 then
                            --单点确认(50ppm)
                            if flowManager:IsFlowEnable() == true then
                                local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 50)
                                flow.name  = "SinglePoint"
                                flow.text = "单点确认(50ppm)"
                                flowManager:StartFlow()
                            end
                        elseif optcode == 15 then
                            --系统适用性确认
                            if flowManager:IsFlowEnable() == true then
                                local flow = ConfirmFlow:new({}, ConfirmType.systemAdaptability, nil)
                                flow.name  = "SystemAdaptability"
                                flow.text = "系统适用性确认"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 16 then
                            --无菌水适用性确认
                            if flowManager:IsFlowEnable() == true then
                                local flow = ConfirmFlow:new({}, ConfirmType.sterileWaterAdaptability, nil)
                                flow.name  = "SterileWaterAdaptability"
                                flow.text = "无菌水适用性确认"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 17 then
                            --鲁棒性验证
                            if flowManager:IsFlowEnable() == true then
                                local flow = ConfirmFlow:new({}, ConfirmType.robustness, nil)
                                flow.name  = "Robustness"
                                flow.text = "鲁棒性验证"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 18 then
                            --特异性验证
                            if flowManager:IsFlowEnable() == true then
                                local flow = ConfirmFlow:new({}, ConfirmType.specificity, nil)
                                flow.name  = "SystemAdaptability"
                                flow.text = "特异性验证"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 19 then
                            --线性验证
                            if flowManager:IsFlowEnable() == true then
                                local flow = ConfirmFlow:new({}, ConfirmType.linear, nil)
                                flow.name  = "Linear"
                                flow.text = "线性验证"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 20 then
                            --SDBS适用性验证
                            if flowManager:IsFlowEnable() == true then
                                local flow = ConfirmFlow:new({}, ConfirmType.sdbsAdaptability, nil)
                                flow.name  = "SystemAdaptability"
                                flow.text = "SDBS适用性验证"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 21 then
                            --一键更新试剂
                            if flowManager:IsFlowEnable() == true then
                                local flow = CleanFlow:new({}, cleanType.oneKeyRenew)
                                flow.name  = "OneKeyRenew"
                                flow.text = "一键更新试剂"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()

                            end
                        end
                    end
                elseif optcode == 50 then  --仪器重启
                    log:debug("Modbus 远程控制仪器重启")
                    modbusManager:Reboot()
                end
            end,
        },
        [1421] = -- 管道操作控制命令
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 1421 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                local volume = modbusManager:GetFloat(RegisterType.Hold, offsetAddress + 1)
                log:debug("ModbusManager PC optcode = "..optcode .. ", vol = " .. volume)

                local flowManager = FlowManager.Instance()
                if (optcode >= 0 and optcode <= 21)  then
                    if flowManager:IsAuthorize() == true  then

                        if optcode == 0 then
                            flowManager:StopFlow()  --停止
                        elseif optcode == 1 then
                            --填充酸剂
                            if flowManager:IsFlowEnable() == true then
                                log:debug("PipeManager createFlow ==> SuckReagent1")
                                local flow = LiquidOperateFlow:new({}, setting.liquidType.reagent1, setting.liquidType.none, 0, volume, 0,setting.runAction.suckFromReagent1)
                                flow.name = setting.ui.operation.liquidOperator[1].name
                                flow.text = "填充酸剂"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 2 then
                            --填充氧化剂
                            if flowManager:IsFlowEnable() == true then
                                log:debug("PipeManager createFlow ==> SuckReagent2")
                                local flow = LiquidOperateFlow:new({}, setting.liquidType.reagent2, setting.liquidType.none, 0, volume, 0,setting.runAction.suckFromReagent2)
                                flow.name = setting.ui.operation.liquidOperator[2].name
                                flow.text = "填充氧化剂"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 3 then
                            --排至酸剂管
                            if flowManager:IsFlowEnable() == true then
                                log:debug("PipeManager createFlow ==> DrainReagent1")
                                local flow = LiquidOperateFlow:new({}, setting.liquidType.none, setting.liquidType.reagent1, 0,0, volume, setting.runAction.drainToReagent1)
                                flow.name = setting.ui.operation.liquidOperator[3].name
                                flow.text = "排至酸剂管"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 4 then
                            --排至氧化剂管
                            if flowManager:IsFlowEnable() == true then
                                log:debug("PipeManager createFlow ==> DrainReagent2")
                                local flow = LiquidOperateFlow:new({}, setting.liquidType.none, setting.liquidType.reagent2, 0,0, volume, setting.runAction.drainToReagent2)
                                flow.name = setting.ui.operation.liquidOperator[4].name
                                flow.text = "排至氧化剂管"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        end
                    end
                end
            end,
        },
        [1422] = -- 1422-1424 预留
        {
            number = 2, -- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [1424] = -- 硬件测试
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 1424 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress + 1)

                log:debug("HardWardTest PC optcode = "..optcode .. ", action = " .. value)

                local statusManager = StatusManager.Instance()
                local runStatus = statusManager:GetStatus()
                local name = runStatus:GetName()

                local action = true
                if value == 1 then
                    action = false
                end

                local flowManager = FlowManager.Instance()
                if (optcode >= 0 and optcode <= 30)  then
                    if flowManager:IsAuthorize() == true
                            and dc:GetConnectStatus()
                            and lc:GetConnectStatus()
                            and rc:GetConnectStatus()
                            and oc:GetConnectStatus() then

                        if optcode == 0 then
                            --预留
                        elseif optcode == 1 then
                            -- TC泵正转
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(38, action, "TC泵正转")
                            end
                        elseif optcode == 2 then
                            --TC泵反转
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(39, action, "TC泵反转")
                            end
                        elseif optcode == 3 then
                            --IC泵反转
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(40, action, "IC泵正转")
                            end
                        elseif optcode == 4 then
                            --IC泵反转
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(41, action, "IC泵反转")
                            end
                        elseif optcode == 5 then
                            --去离子水泵转
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(42, action, "去离子水泵转")
                            end
                        elseif optcode == 6 then
                            --ICR泵
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(43, action, "ICR泵")
                            end
                        elseif optcode == 7 then
                            --LC阀1
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(44, action, "LC阀1")
                            end
                        elseif optcode == 8 then
                            --LC阀2
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(45, action, "LC阀2")
                            end
                        elseif optcode == 9 then
                            --LC阀3
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(46, action, "LC阀3")
                            end
                        elseif optcode == 10 then
                            --LC阀4
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(47, action, "LC阀4")
                            end
                        elseif optcode == 11 then

                        elseif optcode == 12 then

                        elseif optcode == 13 then

                        elseif optcode == 14 then

                        elseif optcode == 15 then
                            --紫外灯
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(12, action, "紫外灯")
                            end
                        elseif optcode == 16 then
                            --继电器1
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(27, action, "继电器1")
                            end
                        elseif optcode == 17 then
                            --继电器2
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(28, action, "继电器2")
                            end
                        elseif optcode == 18 then
                            --继电器3
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(29, action, "继电器3")
                            end
                        elseif optcode == 19 then
                            --继电器4
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(30, action, "继电器4")
                            end
                        elseif optcode == 20 then
                            --[TOC]4mA输出
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(16, action, "[TOC]4mA输出")
                            end
                        elseif optcode == 21 then
                            --[TOC]12mA输出
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(17, action, "[TOC]12mA输出")
                            end
                        elseif optcode == 22 then
                            --[TOC]20mA输出
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(18, action, "[TOC]20mA输出")
                            end
                        elseif optcode == 23 then
                            --[TC]4mA输出
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(19, action, "[TC]4mA输出")
                            end
                        elseif optcode == 24 then
                            --[TC]12mA输出
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(20, action, "[TC]12mA输出")
                            end
                        elseif optcode == 25 then
                            --[TC]20mA输出
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(21, action, "[TC]20mA输出")
                            end
                        elseif optcode == 26 then
                            --[IC]4mA输出
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(22, action, "[IC]4mA输出")
                            end
                        elseif optcode == 27 then
                            --[IC]12mA输出
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(23, action, "[IC]12mA输出")
                            end
                        elseif optcode == 28 then
                            --[IC]20mA输出
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(24, action, "[IC]20mA输出")
                            end
                        elseif optcode == 29 then
                            --机箱风扇
                            if flowManager:IsFlowEnable() == true or name == "HardwareTest" then
                                HardwareTest:execute(25, action, "机箱风扇")
                            end
                        end
                    else
                        log:debug("驱动连接异常")
                    end
                end
            end,
        },
        [1425] = -- 预留
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [1426] = -- 试剂管理
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 1426 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress + 1)
                local month = modbusManager:GetShort(RegisterType.Hold, offsetAddress + 2)

                log:debug("ReagentManager PC optcode = "..optcode .. ", value = " .. value .. ", month = " .. month)

                local flowManager = FlowManager.Instance()
                if (optcode >= 0 and optcode <= 30)  then
                    if flowManager:IsAuthorize() == true  then

                        if optcode == 0 then
                            --预留
                        elseif optcode == 1 then
                            -- 更换酸剂
                            ReagentRemainManager.ChangeReagent(setting.liquidType.reagent1, value)
                            config.remain.reagent1.total = value
                            config.remain.reagent1.cycle = month
                            config.remain.reagent1.lastTime = os.time()
                            ConfigLists.SaveRemainStatus()
                            ConfigLists.SaveRemainConfig()
                        elseif optcode == 2 then
                            -- 更换氧化剂
                            ReagentRemainManager.ChangeReagent(setting.liquidType.reagent2, value)
                            config.remain.reagent2.total = value
                            config.remain.reagent2.cycle = month
                            config.remain.reagent2.lastTime = os.time()
                            ConfigLists.SaveRemainStatus()
                            ConfigLists.SaveRemainConfig()
                        end
                    end
                end
            end,
        },
        [1427] = -- 预留
        {
            number = 2,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [1429] = -- 耗材管理
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 1429 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                local month = modbusManager:GetShort(RegisterType.Hold, offsetAddress + 1)
                log:debug("UseResource PC optcode = ".. optcode .. ", month = " .. month)

                local flowManager = FlowManager.Instance()
                if (optcode >= 0 and optcode <= 30)  then
                    if flowManager:IsAuthorize() == true  then

                        if optcode == 0 then
                            --预留
                        elseif optcode == 1 then
                            -- 更换泵
                            config.consumable.pump.cycle = month
                            config.consumable.pump.lastTime = os.time()
                            ConfigLists.SaveConsumableConfig()
                            MaterialLifeManager.Reset(setting.materialType.pump)
                        elseif optcode == 2 then
                            -- 更换紫外灯
                            config.consumable.uvLamp.cycle = month
                            config.consumable.uvLamp.lastTime = os.time()
                            ConfigLists.SaveConsumableConfig()
                            MaterialLifeManager.Reset(setting.materialType.uvLamp)
                        elseif optcode == 3 then
                            -- 更换树脂层
                            config.consumable.resin.cycle = month
                            config.consumable.resin.lastTime = os.time()
                            ConfigLists.SaveConsumableConfig()
                            MaterialLifeManager.Reset(setting.materialType.resin)
                        end
                    end
                end
            end,
        },
        [1430] = -- 预留
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [1431] = -- 组合操作
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 1431 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress + 1)

                log:debug("Combine PC optcode = "..optcode .. ", value = " .. value)
                local mode = 5
                local volume = 1

                local flowManager = FlowManager.Instance()
                if (optcode >= 0 and optcode <= 30)  then
                    if flowManager:IsAuthorize() == true  then

                        if optcode == 0 then
                            --预留
                            flowManager:StopFlow()  --停止
                        elseif optcode == 1 then
                            -- 酸剂注射器复位
                            local flow = CombineOperateFlow:new({}, setting.liquidType.reagent1, setting.liquidType.none, mode, volume, volume, setting.runAction.syringUpdate)
                            flow.name = setting.ui.operation.combineOperator[1].name
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        elseif optcode == 2 then
                            -- 氧化剂注射器复位
                            local flow = CombineOperateFlow:new({}, setting.liquidType.reagent2, setting.liquidType.none, mode, volume, volume, setting.runAction.syringUpdate)
                            flow.name = setting.ui.operation.combineOperator[2].name
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        elseif optcode == 3 then
                            -- 酸剂注射器更新
                            local flow = CombineOperateFlow:new({}, setting.liquidType.none, setting.liquidType.reagent1, mode, volume, volume, setting.runAction.syringUpdate)
                            flow.name = setting.ui.operation.combineOperator[3].name
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        elseif optcode == 4 then
                            -- 酸剂注射器更新
                            local flow = CombineOperateFlow:new({}, setting.liquidType.none, setting.liquidType.reagent2, mode, volume, volume, setting.runAction.syringUpdate)
                            flow.name = setting.ui.operation.combineOperator[4].name
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    end
                end
            end,
        },
        [1432] = -- 预留
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [1433] = -- 通信检测
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 1433 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if dc:GetConnectStatus() then
                    value = value | (1<<0)
                end
                if lc:GetConnectStatus() then
                    value = value | (1<<1)
                end
                if rc:GetConnectStatus() then
                    value = value | (1<<2)
                end
                if oc:GetConnectStatus() then
                    value = value | (1<<3)
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [1434] = -- 1033-1500 预留
        {
            number = 66,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [1500] = -- 硬件校准 - 泵系数
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1500 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if dc:GetConnectStatus()
                    and lc:GetConnectStatus()
                    and rc:GetConnectStatus()
                    and oc:GetConnectStatus() then
                    value = setting.ui.profile.hardwareParamIterms[1][1].get()
                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
                local offsetAddress = 1500 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)

                log:debug("HW meterPump = " .. value)

                if dc:GetConnectStatus()
                        and lc:GetConnectStatus()
                        and rc:GetConnectStatus()
                        and oc:GetConnectStatus() then
                    if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                        setting.ui.profile.hardwareParamIterms[1][1].set(value)
                        setting.ui.profile.hardwareParamIterms[1].set()
                    end
                end
            end,
        },
        [1502] = -- 温度校准 负输入分压
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1502 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if dc:GetConnectStatus()
                        and lc:GetConnectStatus()
                        and rc:GetConnectStatus()
                        and oc:GetConnectStatus() then
                    value = setting.ui.profile.hardwareParamIterms[2][1].get()
                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
                local offsetAddress = 1502 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)

                log:debug("HW tempCalibrate = " .. value)
                if dc:GetConnectStatus()
                        and lc:GetConnectStatus()
                        and rc:GetConnectStatus()
                        and oc:GetConnectStatus() then
                    if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                        setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetNegativeInput()
                        setting.ui.profile.hardwareParamIterms[2].set()
                    end
                end
            end,
        },
        [1504] = -- 温度校准 参考电压
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1504 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if dc:GetConnectStatus()
                        and lc:GetConnectStatus()
                        and rc:GetConnectStatus()
                        and oc:GetConnectStatus() then
                    value = setting.ui.profile.hardwareParamIterms[2][2].get()
                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
                local offsetAddress = 1504 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)

                log:debug("HW tempCalibrate = " .. value)
                if dc:GetConnectStatus()
                        and lc:GetConnectStatus()
                        and rc:GetConnectStatus()
                        and oc:GetConnectStatus() then
                    if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                        setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetReferenceVoltage()
                        setting.ui.profile.hardwareParamIterms[2].set()
                    end
                end
            end,
        },
        [1506] = -- 温度校准 参考电压
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1506 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if dc:GetConnectStatus()
                        and lc:GetConnectStatus()
                        and rc:GetConnectStatus()
                        and oc:GetConnectStatus() then
                    value = setting.ui.profile.hardwareParamIterms[2][3].get()
                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
                local offsetAddress = 1506 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)

                log:debug("HW tempCalibrate = " .. value)
                if dc:GetConnectStatus()
                        and lc:GetConnectStatus()
                        and rc:GetConnectStatus()
                        and oc:GetConnectStatus() then
                    if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                        setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetCalibrationVoltage()
                        setting.ui.profile.hardwareParamIterms[2].set()
                    end
                end
            end,
        },
        [1508] = -- 1508-2000 预留
        {
            number = 92,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [1600] = -- 1600 测量排期恢复默认
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()

                local offsetAddress = 1600 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value == 1 then
                    setting.ui.profile.scheduler(RoleType.Super)

                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ChangeAutoMeasure, "Modbus")
                end
            end,
        },
        [1601] = -- 1601 测量参数恢复默认
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()

                local offsetAddress = 1601 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                local update = false

                if value == 0 then
                    update = true
                    setting.ui.profile.measureParam.defaultRestore(RoleType.Super, false)
                elseif value == 1 then
                    update = true
                    setting.ui.profile.measureParam.defaultRestore(RoleType.Super, false)
                elseif value == 2 then
                    update = true
                    setting.ui.profile.measureParam.defaultRestore(RoleType.Super, false)
                end

                if update == true then
                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
                end
            end,
        },
        [1602] = -- 1601 外联接口恢复默认
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()

                local offsetAddress = 1602 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                local update = false

                if value == 0 then
                    update = true
                    setting.ui.profile.interconnection.defaultRestore(RoleType.Super, false)
                elseif value == 1 then
                    update = true
                    setting.ui.profile.interconnection.defaultRestore(RoleType.Administrator, false)
                elseif value == 2 then
                    update = true
                    setting.ui.profile.interconnection.defaultRestore(RoleType.Maintain, false)
                end

                if update == true then
                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
                end
            end,
        },
        [1603] = -- 1601 系统参数恢复默认
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()

                local offsetAddress = 1603 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                local update = false

                if value == 0 then
                    update = true
                    setting.ui.profile.system.defaultRestore(RoleType.Super, false)
                elseif value == 1 then
                    update = true
                    setting.ui.profile.system.defaultRestore(RoleType.Administrator, false)
                elseif value == 2 then
                    update = true
                    setting.ui.profile.system.defaultRestore(RoleType.Maintain, false)
                end

                if update == true then
                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ChangeSystemParam, "Modbus")
                end
            end,
        },
        [1604] = -- 1600-2000 恢复出厂
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()

                local offsetAddress = 1604 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                local update = false
                if value == 0 then
                    update = true
                    setting.ui.factoryTime.defaultRestore(RoleType.Super)
                elseif value == 1 then
                    update = true
                    setting.ui.factoryTime.defaultRestore(RoleType.Administrator)
                elseif value == 2 then
                    update = true
                    setting.ui.factoryTime.defaultRestore(RoleType.Maintain)
                end

                if update == true then
                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ChangeAutoMeasure, "Modbus")
                    updateWidgetManager:Update(UpdateEvent.ChangeInterconnectionParam, "Modbus")
                    updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
                    updateWidgetManager:Update(UpdateEvent.ChangeSystemParam, "Modbus")
                end
            end,
        },
        [1605] = -- 1605 清除弹窗信息
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()

                local offsetAddress = 1605 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)

                if value == 1 then
                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ModbusChangeParam, "Modbus")
                end
            end,
        },
        [1606] = -- 测量类型 0-在线 1-离线
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 1606 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, config.measureParam.meaType)
            end,

            write = function()
                local offsetAddress = 1606 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    config.measureParam.meaType = 1
                else
                    config.measureParam.meaType = 0
                end

                config.modifyRecord.measureParam(true)
                ConfigLists.SaveMeasureParamConfig()
                local updateWidgetManager = UpdateWidgetManager.Instance()
                updateWidgetManager:Update(UpdateEvent.ChangeConfigParam, "Modbus")
            end,
        },
        [1607] = --方法创建时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 1607 - setting.externalInterface.modbus.holdRegAddr

                local time = config.measureParam.methodCreateTime
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
                local offsetAddress = 1607 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTimeToSystem(RegisterType.Hold, offsetAddress + 1)
            end,
        },
        [1610] = -- 方法名称
        {
            number = 20,	-- Register number

            read = function()
                local offsetAddress = 1610 - setting.externalInterface.modbus.holdRegAddr
                local regSize = setting.externalInterface.modbus.registers[1610].number

                local modbusManager = ModbusManager.Instance()
                local str = config.measureParam.methodName

                modbusManager:SetString(RegisterType.Hold, offsetAddress, str, regSize)
            end,

            write = function()
                local offsetAddress = 1610 - setting.externalInterface.modbus.holdRegAddr
                local regSize = setting.externalInterface.modbus.registers[1610].number

                local modbusManager = ModbusManager.Instance()
                local str = modbusManager:GetString(RegisterType.Hold, offsetAddress, regSize)
                if string.len(str) then
                    config.measureParam.methodName = str
                end
            end,
        },
        [1630] = --保存当前方法
        {
            number = 1, -- Register number

            read = function()

            end,

            write = function()
                local offsetAddress = 1630 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if value > 0 then
                    SaveToMethodSqlite()
                end
            end,
        },
        [1631] = --根据创建时间应用方法
        {
            number = 3, -- Register number

            read = function()

            end,

            write = function()
                local offsetAddress = 1631 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local timeStr =  modbusManager:GetTime(RegisterType.Hold, offsetAddress)

                local _,_,y,m,d,hh,mm,ss = string.find(timeStr,"(%d+)-(%d+)-(%d+)%s*(%d+):(%d+):(%d+)")
                local timeStamp = os.time({year=y,month=m,day=d,hour=hh,min=mm,sec=ss})

                local mthodViewWidget = MethodViewWidget.Instance()
                mthodViewWidget:MethodApplyForModbus(timeStamp)
            end,
        },
        [1634] = --根据创建时间删除方法
        {
            number = 3, -- Register number

            read = function()

            end,

            write = function()
                local offsetAddress = 1634 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local timeStr =  modbusManager:GetTime(RegisterType.Hold, offsetAddress)

                local _,_,y,m,d,hh,mm,ss = string.find(timeStr,"(%d+)-(%d+)-(%d+)%s*(%d+):(%d+):(%d+)")
                local timeStamp = os.time({year=y,month=m,day=d,hour=hh,min=mm,sec=ss})

                local mthodViewWidget = MethodViewWidget.Instance()
                mthodViewWidget:MethodDelectForModbus(timeStamp)
            end,
        },
        [1637] = -- 1637-1700 预留
        {
            number = 63,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [1700] = --  测量信号
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1700 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if rc:GetConnectStatus() then
                    value = rc:GetScanData(rc:GetScanLen() - 1)
                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [1702] = --  参考信号
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1702 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if rc:GetConnectStatus() then
                    value = rc:GetScanDataRef(rc:GetScanLen() - 1)
                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [1704] = --  吸光度
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1704 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if rc:GetConnectStatus() then
                    local mea = rc:GetScanData(rc:GetScanLen() - 1)
                    local ref = rc:GetScanDataRef(rc:GetScanLen() - 1)
                    value = 1000 * math.log(ref/mea,10)
                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [1706] = --  紫外灯PD
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1706 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if dc:GetConnectStatus() then

                end
                modbusManager:SetInt(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [1708] = --  输出板温度
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1708 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if oc:GetConnectStatus() then
                    value = oc:GetEnvironmentTemperature()
                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [1710] = --  信号板温度1
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1710 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if rc:GetConnectStatus() then
                    value = oc:GetDigestTemperature()
                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [1712] = --  信号板温度2
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1712 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if rc:GetConnectStatus() then
                    value = oc:GetEnvironmentTemperature()
                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [1714] = --  信号板温度3
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1714 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if rc:GetConnectStatus() then

                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [1716] = --  信号板温度4
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1716 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if rc:GetConnectStatus() then

                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [1718] = --  压力值
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 1718 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local value = 0
                if dc:GetConnectStatus() then
                   value = dc:GetPressure(0)
                end
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [1720] = -- 1720-2000 预留
        {
            number = 280,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
    }
}