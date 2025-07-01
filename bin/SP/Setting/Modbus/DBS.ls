setting.externalInterface = {}

setting.externalInterface.modbus =
{
    inputRegAddr = 0,
    inputRegNum = 0x1000,
    holdRegAddr = 0x1000,
    holdRegNum = 0x3000,

    registers =
    {
        [0x0] =  --空余
        {
            number = 0x1000,

            read = function()

            end,
            write = function()

            end,
        },
        [0x1000] = --因子编码
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x1000 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0

                if setting.instrument.deviceTypeNum ~= nil then
                    for k,v in pairs(setting.instrument.deviceTypeNum) do
                        ret = v
                    end
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetInt(RegisterType.Hold, offsetAddress, ret)
            end,

            write = function()
            end,
        },
        [0x1002] = --单位
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 0x1002 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, 1) --mg/L
            end,

            write = function()
            end,
        },
        [0x1003] = --标样参考值(标液浓度)
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x1003 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.curveParam[config.measureParam.range[config.measureParam.calibrateRangeIndex + 1]+1].RangeConsistency)
            end,

            write = function()
            end,
        },
        [0x1005] = --水样测量时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 0x1005 - setting.externalInterface.modbus.holdRegAddr

                local time = status.measure.report.measure.dateTime
                --if status.measure.offline.measure.dateTime > status.measure.report.measure.dateTime then
                --    time = status.measure.offline.measure.dateTime
                --end
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [0x1008] = --水样测量结果
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x1008 - setting.externalInterface.modbus.holdRegAddr

                local value = status.measure.report.measure.consistency
                --if status.measure.offline.measure.dateTime > status.measure.report.measure.dateTime then
                --    value = status.measure.offline.measure.consistency
                --end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [0x100A] = --水样数据标识
        {
            number = 6, -- Register number

            read = function()
                local offsetAddress = 0x100A - setting.externalInterface.modbus.holdRegAddr

                local resultInfo = status.measure.report.measure.resultInfo
                --if status.measure.offline.measure.dateTime > status.measure.report.measure.dateTime then
                --    resultInfo = status.measure.offline.measure.resultInfo
                --end

                local regSize = setting.externalInterface.modbus.registers[0x100A].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetCharStringLittleEndian(RegisterType.Hold, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [0x1010] = --标样测量时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 0x1010 - setting.externalInterface.modbus.holdRegAddr

                local time = status.measure.report.check.dateTime
                --if status.measure.offline.check.dateTime > status.measure.report.check.dateTime then
                --    time = status.measure.offline.check.dateTime
                --end
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [0x1013] = --标样测量结果
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x1013 - setting.externalInterface.modbus.holdRegAddr

                local value = status.measure.report.check.consistency
                --if status.measure.offline.check.dateTime > status.measure.report.check.dateTime then
                --    value = status.measure.offline.check.consistency
                --end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [0x1015] = --标样数据标识
        {
            number = 6, -- Register number

            read = function()
                local offsetAddress = 0x1015 - setting.externalInterface.modbus.holdRegAddr

                local resultInfo = status.measure.report.check.resultInfo
                --if status.measure.offline.check.dateTime > status.measure.report.check.dateTime then
                --    resultInfo = status.measure.offline.check.resultInfo
                --end

                local regSize = setting.externalInterface.modbus.registers[0x1015].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetCharStringLittleEndian(RegisterType.Hold, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [0x101B] = --空白数据测量时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 0x101B - setting.externalInterface.modbus.holdRegAddr

                local time = status.measure.report.blankCheck.dateTime
                --if status.measure.offline.blankCheck.dateTime > status.measure.report.blankCheck.dateTime then
                --    time = status.measure.offline.blankCheck.dateTime
                --end
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [0x101E] = --空白数据测量结果
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x101E - setting.externalInterface.modbus.holdRegAddr

                local value = status.measure.report.blankCheck.consistency
                --if status.measure.offline.blankCheck.dateTime > status.measure.report.blankCheck.dateTime then
                --    value = status.measure.offline.blankCheck.consistency
                --end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [0x1020] = --空白数据标识
        {
            number = 6, -- Register number

            read = function()
                local offsetAddress = 0x1020 - setting.externalInterface.modbus.holdRegAddr

                local resultInfo = status.measure.report.blankCheck.resultInfo
                --if status.measure.offline.blankCheck.dateTime > status.measure.report.blankCheck.dateTime then
                --    resultInfo = status.measure.offline.blankCheck.resultInfo
                --end

                local regSize = setting.externalInterface.modbus.registers[0x1020].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetCharStringLittleEndian(RegisterType.Hold, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [0x1026] = --零点核查数据测量时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 0x1026 - setting.externalInterface.modbus.holdRegAddr

                local time = status.measure.report.zeroCheck.dateTime
                --if status.measure.offline.zeroCheck.dateTime > status.measure.report.zeroCheck.dateTime then
                --    time = status.measure.offline.zeroCheck.dateTime
                --end
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [0x1029] = --零点核查数据测量结果
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x1029 - setting.externalInterface.modbus.holdRegAddr

                local value = status.measure.report.zeroCheck.consistency
                --if status.measure.offline.zeroCheck.dateTime > status.measure.report.zeroCheck.dateTime then
                --    value = status.measure.offline.zeroCheck.consistency
                --end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [0x102B] = --零点核查数据标识
        {
            number = 6, -- Register number

            read = function()
                local offsetAddress = 0x102B - setting.externalInterface.modbus.holdRegAddr

                local resultInfo = status.measure.report.zeroCheck.resultInfo
                --if status.measure.offline.zeroCheck.dateTime > status.measure.report.zeroCheck.dateTime then
                --    resultInfo = status.measure.offline.zeroCheck.resultInfo
                --end

                local regSize = setting.externalInterface.modbus.registers[0x102B].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetCharStringLittleEndian(RegisterType.Hold, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [0x1031] = --跨度核查数据测量时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 0x1031 - setting.externalInterface.modbus.holdRegAddr

                local time = status.measure.report.rangeCheck.dateTime
                --if status.measure.offline.rangeCheck.dateTime > status.measure.report.rangeCheck.dateTime then
                --    time = status.measure.offline.rangeCheck.dateTime
                --end
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [0x1034] = --跨度核查数据测量结果
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x1034 - setting.externalInterface.modbus.holdRegAddr

                local value = status.measure.report.rangeCheck.consistency
                --if status.measure.offline.rangeCheck.dateTime > status.measure.report.rangeCheck.dateTime then
                --    value = status.measure.offline.rangeCheck.consistency
                --end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [0x1036] = --跨度核查数据标识
        {
            number = 6, -- Register number

            read = function()
                local offsetAddress = 0x1036 - setting.externalInterface.modbus.holdRegAddr

                local resultInfo = status.measure.report.rangeCheck.resultInfo
                --if status.measure.offline.rangeCheck.dateTime > status.measure.report.rangeCheck.dateTime then
                --    resultInfo = status.measure.offline.rangeCheck.resultInfo
                --end

                local regSize = setting.externalInterface.modbus.registers[0x1036].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetCharStringLittleEndian(RegisterType.Hold, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [0x103C] = --加标回收数据测量时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 0x103C - setting.externalInterface.modbus.holdRegAddr

                local time = status.measure.report.addstandard.dateTime
                --if status.measure.offline.addstandard.dateTime > status.measure.report.addstandard.dateTime then
                --    time = status.measure.offline.addstandard.dateTime
                --end
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [0x103F] = --加标回收数据测量结果
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x103F - setting.externalInterface.modbus.holdRegAddr

                local value = status.measure.report.addstandard.consistency
                --if status.measure.offline.addstandard.dateTime > status.measure.report.addstandard.dateTime then
                --    value = status.measure.offline.addstandard.consistency
                --end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [0x1041] = --加标回收数据标识
        {
            number = 6, -- Register number

            read = function()
                local offsetAddress = 0x1041 - setting.externalInterface.modbus.holdRegAddr

                local resultInfo = status.measure.report.addstandard.resultInfo
                --if status.measure.offline.addstandard.dateTime > status.measure.report.addstandard.dateTime then
                --    resultInfo = status.measure.offline.addstandard.resultInfo
                --end

                local regSize = setting.externalInterface.modbus.registers[0x1041].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetCharStringLittleEndian(RegisterType.Hold, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [0x1047] = --平行样数据测量时间
        {
            number = 3, -- Register number

            read = function()

                local offsetAddress = 0x1047 - setting.externalInterface.modbus.holdRegAddr

                local time = status.measure.report.parallel.dateTime
                --if status.measure.offline.parallel.dateTime > status.measure.report.parallel.dateTime then
                --    time = status.measure.offline.parallel.dateTime
                --end
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [0x104A] = --平行样数据测量结果
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x104A - setting.externalInterface.modbus.holdRegAddr

                local value = status.measure.report.parallel.consistency
                --if status.measure.offline.parallel.dateTime > status.measure.report.parallel.dateTime then
                --    value = status.measure.offline.parallel.consistency
                --end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [0x104C] = --平行样数据标识
        {
            number = 6, -- Register number

            read = function()
                local offsetAddress = 0x104C - setting.externalInterface.modbus.holdRegAddr

                local resultInfo = status.measure.report.parallel.resultInfo
                --if status.measure.offline.parallel.dateTime > status.measure.report.parallel.dateTime then
                --    resultInfo = status.measure.offline.parallel.resultInfo
                --end

                local regSize = setting.externalInterface.modbus.registers[0x104C].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetCharStringLittleEndian(RegisterType.Hold, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [0x1052] =     -- 0x1052 - 0x107F 预留
        {
            number = 46,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [0x1080] = --系统时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 0x1080 - setting.externalInterface.modbus.holdRegAddr

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
        [0x1083] = --仪器状态
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 0x1083 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()

                local statusManager = StatusManager.Instance()
                local runStatus = statusManager:GetStatus()
                local name = runStatus:GetName()

                if setting.modbusCoder.statusID[name] ~= nil then
                    modbusManager:SetShort(RegisterType.Hold, offsetAddress, setting.modbusCoder.statusID[name].dbsID)
                else
                    modbusManager:SetShort(RegisterType.Hold, offsetAddress, 0)
                end
            end,

            write = function()
            end,
        },
        [0x1084] = --测量模式(1连续 2周期 3定点 4受控 5手动)
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 0x1084 - setting.externalInterface.modbus.holdRegAddr

                local measureMode = 4
                if config.scheduler.measure.mode == MeasureMode.Continous then
                    measureMode = 1
                elseif config.scheduler.measure.mode == MeasureMode.Periodic then
                    measureMode = 2
                elseif config.scheduler.measure.mode == MeasureMode.Timed then
                    measureMode = 3
                elseif config.scheduler.measure.mode == MeasureMode.Trigger then
                    measureMode = 4
                elseif config.scheduler.measure.mode == MeasureMode.Manual then
                    measureMode = 5
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, measureMode)

            end,

            write = function()
            end,
        },
        [0x1085] = --报警类型
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 0x1085 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local AlarmManager = AlarmManager.Instance()
                local alarm = AlarmManager:GetCurrentAlarm()
                local name = alarm:GetName()
                local cause = alarm:GetCause()

                local alarmVal = 0
                if name == "空闲" or name == "" then
                    alarmVal = 0
                elseif name == "抽取失败" then
                    if cause == "水样" then
                        alarmVal = 2    --缺水样告警
                    elseif cause == "空白水" then
                        alarmVal = 3    --缺空白水告警
                    elseif cause == "标液" then
                        alarmVal = 4    --缺标液告警
                    elseif cause == "标液二" then
                        alarmVal = 4    --缺标液告警
                    elseif cause == "消解混合液" then
                        alarmVal = 12    --仪器内部其他异常
                    else
                        alarmVal = 1    --缺试剂告警
                    end
                elseif name == "仪器漏液" then
                    alarmVal = 5    --漏液告警
                elseif name == "定标结果错误" then
                    alarmVal = 6    --标定异常告警
                elseif name == "校准结果错误" then
                    alarmVal = 6
                elseif name == "核查结果错误" then
                    alarmVal = 6
                elseif name == "水样超量程" then
                    alarmVal = 7    --超量程告警
                elseif name == "温控故障" then
                    alarmVal = 8 --加热异常
                elseif name == "请更换试剂" then
                    alarmVal = 9    --低试剂预警
                elseif name == "水样超上限" then
                    alarmVal = 10  --超警报上限
                elseif name == "水样超下限" then
                    alarmVal = 11  --超警报下限
                elseif name == "量程切换" then
                    alarmVal = 15   --量程切换异常告警
                else
                    alarmVal = 0   --仪表内部其它异常
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, alarmVal)
            end,

            write = function()
            end,
        },
        [0x1086] = --故障类型
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 0x1086 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local AlarmManager = AlarmManager.Instance()
                local alarm = AlarmManager:GetCurrentAlarm()
                local name = alarm:GetName()
                local cause = alarm:GetCause()

                local faultVal = 0
                if name == "空闲" then
                    faultVal = 0  --无故障
                elseif name == "抽取失败" then
                    if cause == "消解混合液" then
                        faultVal = 1    --电机故障
                    end
                elseif name == "温控故障" then
                    faultVal = 2 --温度故障
                elseif name == "通信异常" then
                    faultVal = 3    --通讯故障
                elseif name == "仪器运行故障" then
                    faultVal = 5  --仪器运行故障
                end

                modbusManager:SetShort(RegisterType.Hold, offsetAddress, faultVal)
            end,

            write = function()
            end,
        },
        [0x1087] =     -- 日志代码/动作记录
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 0x1087 - setting.externalInterface.modbus.holdRegAddr

                local statusManager = StatusManager.Instance()
                local action = statusManager:GetAction()
                local name = action:GetName()

                local modbusManager = ModbusManager.Instance()

                if setting.modbusCoder.actionID[name] ~= nil then
                    if config.system.newDbsModbusLog == false then
                        modbusManager:SetShort(RegisterType.Hold, offsetAddress, setting.modbusCoder.actionID[name].ID)
                    else
                        if setting.modbusCoder.actionID[name].dbsID ~= nil then	--地表水新日志编码
                            modbusManager:SetShort(RegisterType.Hold, offsetAddress, setting.modbusCoder.actionID[name].dbsID)
                        end
                    end
                end
            end,

            write = function()
            end,
        },
        [0x1088] =     --  主控板固件版本
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 0x1088 - setting.externalInterface.modbus.holdRegAddr

                local ver = Version.new(setting.version.software)

                local major = ver:GetMajor()
                local minor = ver:GetMinor()
                local revision  = ver:GetRevision()
                local build  = ver:GetBuild()
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, major * 1000 + minor * 100 + revision * 10 + build)
            end,

            write = function()
            end,
        },
        [0x1089] = -- 测量间隔(分钟)
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 0x1089 - setting.externalInterface.modbus.holdRegAddr

                local temp = math.ceil(config.scheduler.measure.interval * 60)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, temp)
            end,

            write = function()
            end,
        },
        [0x108A] = -- 零点核查间隔(分钟)
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 0x108A - setting.externalInterface.modbus.holdRegAddr

                local temp = math.ceil(config.scheduler.zeroCheck.interval * 60)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, temp)
            end,

            write = function()
            end,
        },
        [0x108B] = -- 跨度核查间隔(分钟)
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 0x108B - setting.externalInterface.modbus.holdRegAddr

                local temp = math.ceil(config.scheduler.rangeCheck.interval * 60)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, temp)
            end,

            write = function()
            end,
        },
        [0x108C] = -- 标样核查间隔(分钟)
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 0x108C - setting.externalInterface.modbus.holdRegAddr

                local temp = math.ceil(config.scheduler.check.interval * 60)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, temp)
            end,

            write = function()
            end,
        },
        [0x108D] =     -- 0x108D - 0x109F 预留
        {
            number = 19,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [0x10A0] = -- 小数位数
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 0x10A0 - setting.externalInterface.modbus.holdRegAddr

                local temp = setting.measureResult.decimalNum

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, temp)
            end,

            write = function()
            end,
        },
        [0x10A1] = --  消解温度
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 0x10A1 - setting.externalInterface.modbus.holdRegAddr

                local temp = 25

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, temp)
            end,

            write = function()
            end,
        },
        [0x10A2] = -- 消解时间(分钟)
        {
            number = 1, -- Register number

            read = function()
                local offsetAddress = 0x10A2 - setting.externalInterface.modbus.holdRegAddr

                local temp = 0

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, temp)
            end,

            write = function()
            end,
        },
        [0x10A3] = --  当前量程下限
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10A3 - setting.externalInterface.modbus.holdRegAddr

                local rangeMin = 0

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, rangeMin)
            end,

            write = function()
            end,
        },
        [0x10A5] = --  当前量程上限
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10A5 - setting.externalInterface.modbus.holdRegAddr

                local rangeMax = 0
                local currentRange = config.measureParam.range[config.measureParam.currentRange + 1] + 1

                if config.system.rangeViewMap[currentRange].view ~= nil then
                    rangeMax = config.system.rangeViewMap[currentRange].view
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, rangeMax)
            end,

            write = function()
                local offsetAddress = 0x10A5 - setting.externalInterface.modbus.holdRegAddr

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
                    config.modifyRecord.measureParam(true)
                    ConfigLists.SaveMeasureParamConfig()

                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ModbusChangeParam, "Modbus")
                end
            end,
        },
        [0x10A7] = --曲线斜率
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10A7 - setting.externalInterface.modbus.holdRegAddr

                local curveK = config.measureParam.curveK
                local curveB = config.measureParam.curveB
                curveK, curveB  = CurveParamCurveXYChange(curveK, curveB)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, curveK)
            end,

            write = function()
            end,
        },
        [0x10A9] = --曲线截距
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10A9 - setting.externalInterface.modbus.holdRegAddr

                local curveK = config.measureParam.curveK
                local curveB = config.measureParam.curveB
                curveK, curveB  = CurveParamCurveXYChange(curveK, curveB)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, curveB)
            end,

            write = function()
            end,
        },
        [0x10AB] = --定标时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 0x10AB - setting.externalInterface.modbus.holdRegAddr

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
        [0x10AE] = --标液一浓度（零点浓度）
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10AE - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, status.measure.newResult.calibrate.point0Consistency)
            end,

            write = function()
            end,
        },
        [0x10B0] = --标液一测量值（零点测量值）
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10B0 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, status.measure.newResult.calibrate.point0Absorbance)
            end,

            write = function()
            end,
        },
        [0x10B2] = --标液二浓度（标点浓度）
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10B2 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, status.measure.newResult.calibrate.point1Consistency)
            end,

            write = function()
            end,
        },
        [0x10B4] = --标液二测量值（标点测量值）
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10B4 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, status.measure.newResult.calibrate.point1Absorbance)
            end,

            write = function()
            end,
        },
        [0x10B6] = --标液三浓度（扩展）
        {
            number = 2, -- Register number
            read = function()
            end,

            write = function()
            end,
        },
        [0x10B8] = --标液三测量值（扩展）
        {
            number = 2, -- Register number
            read = function()
            end,

            write = function()
            end,
        },
        [0x10BA] = --  当前零点核查上限
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10BA - setting.externalInterface.modbus.holdRegAddr

                local rangeMax = 0
                local currentRange = config.measureParam.range[config.measureParam.zeroCheckRangeIndex + 1] + 1

                if config.system.rangeViewMap[currentRange].view ~= nil then
                    rangeMax = config.system.rangeViewMap[currentRange].view
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, rangeMax)
            end,

            write = function()
                local offsetAddress = 0x10BA - setting.externalInterface.modbus.holdRegAddr

                if not config.system.solidifyMeaParamFromModbus then
                    local modbusManager = ModbusManager.Instance()
                    local inputValue = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)

                    local rangeNum =  setting.measure.range.rangeNum
                    for i = 1, rangeNum do
                        local temp = inputValue - config.system.rangeViewMap[i].view
                        if temp <= 0.000001 then
                            for j=1 , 3 do
                                if config.measureParam.range[j] == i - 1 then
                                    config.measureParam.zeroCheckRangeIndex = j - 1
                                end
                            end
                            break
                        end
                    end
                    config.modifyRecord.measureParam(true)
                    ConfigLists.SaveMeasureParamConfig()

                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ModbusChangeParam, "Modbus")
                end
            end,
        },
        [0x10BC] = --  当前标样(跨度)核查上限
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10BC - setting.externalInterface.modbus.holdRegAddr

                local rangeMax = 0
                local currentRange = config.measureParam.range[config.measureParam.rangeCheckRangeIndex + 1] + 1

                if setting.measure.range[currentRange].viewRange ~= nil then
                    rangeMax = setting.measure.range[currentRange].viewRange
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, rangeMax)
            end,

            write = function()
                local offsetAddress = 0x10BC - setting.externalInterface.modbus.holdRegAddr

                if not config.system.solidifyMeaParamFromModbus then
                    local modbusManager = ModbusManager.Instance()
                    local inputValue = modbusManager:GetFloat(RegisterType.Hold, offsetAddress)

                    local rangeNum =  setting.measure.range.rangeNum
                    for i = 1, rangeNum do
                        local temp = inputValue - config.system.rangeViewMap[i].view
                        if temp <= 0.000001 then
                            for j=1 , 3 do
                                if config.measureParam.range[j] == i - 1 then
                                    config.measureParam.rangeCheckRangeIndex = j - 1
                                end
                            end
                            break
                        end
                    end
                    config.modifyRecord.measureParam(true)
                    ConfigLists.SaveMeasureParamConfig()

                    local updateWidgetManager = UpdateWidgetManager.Instance()
                    updateWidgetManager:Update(UpdateEvent.ModbusChangeParam, "Modbus")
                end
            end,
        },
        [0x10BE] =   --0x10BE - 0x10C1 预留
        {
            number = 4, -- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [0x10C2] = --曲线线性相关系数
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10C2 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, status.measure.newResult.calibrate.R2)
            end,

            write = function()
            end,
        },
        [0x10C4] = --试剂余量(百分比)
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10C4 - setting.externalInterface.modbus.holdRegAddr

                local remain = status.remain.reagent2  --试剂二余量
                local total = config.remain.reagent2.total  --试剂二总量

                local rate = math.floor(100 * (remain / total))

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetInt(RegisterType.Hold, offsetAddress, rate)
            end,

            write = function()
            end,
        },
        [0x10C6] = --测量过程值(吸光度)
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10C6 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, status.measure.newResult.measure.absorbance)
            end,

            write = function()
            end,
        },
        [0x10C8] = --空白校准时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 0x10C8 - setting.externalInterface.modbus.holdRegAddr

                --[[local time = status.measure.report.blankCheck.dateTime
                if status.measure.offline.blankCheck.dateTime > status.measure.report.blankCheck.dateTime then
                    time = status.measure.offline.blankCheck.dateTime
                end--]]
                local time = status.measure.newResult.calibrate.zeroCalibrateDateTime  --零点定标时间
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [0x10CB] = --标样校准时间
        {
            number = 3, -- Register number

            read = function()
                local offsetAddress = 0x10CB - setting.externalInterface.modbus.holdRegAddr

                --[[local time = status.measure.report.check.dateTime
                if status.measure.offline.check.dateTime > status.measure.report.check.dateTime then
                    time = status.measure.offline.check.dateTime
                end--]]
                local time = status.measure.newResult.calibrate.standardCalibrateDateTime  --标点定标时间
                if time < 946684800 then
                    time = 946684800
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetBCDTime(RegisterType.Hold, offsetAddress, time)
            end,

            write = function()
            end,
        },
        [0x10CE] = --检出限值(定量下限)
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10CE - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, setting.measureResult.quantifyLowLimit)
            end,

            write = function()
            end,
        },
        [0x10D0] = --校准系数(修正系数)
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10D0 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, config.measureParam.reviseFactor)
            end,

            write = function()
            end,
        },
        [0x10D2] = --设备序列号
        {
            number = 6, -- Register number

            read = function()
                local offsetAddress = 0x10D2 - setting.externalInterface.modbus.holdRegAddr
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
        [0x10D8] =     -- 0x10D8 - 0x10E3 预留
        {
            number = 12,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
        [0x10E4] = --稀释倍数
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10E4 - setting.externalInterface.modbus.holdRegAddr

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
        [0x10E6] = --三项式系数
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10E6 - setting.externalInterface.modbus.holdRegAddr

                local value = 0

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [0x10E8] = --空白标定系数
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10E8 - setting.externalInterface.modbus.holdRegAddr

                local value = 1

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [0x10EA] = --量程校正系数(因子)
        {
            number = 2, -- Register number

            read = function()
                local offsetAddress = 0x10EA - setting.externalInterface.modbus.holdRegAddr

                local currentRange = config.measureParam.range[config.measureParam.calibrateRangeIndex + 1] + 1
                local value = config.measureParam.reviseParameter[currentRange]
                if value == nil then
                    value = 0
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Hold, offsetAddress, value)
            end,

            write = function()
            end,
        },
        [0x10EC] =     -- 0x10EC - 0x11FF 预留
        {
            number = 276,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },

        [0x1200] = -- 控制命令
        {
            number = 1, -- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 0x1200 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                log:debug("ModbusManager DBS optcode = "..optcode)

                local flowManager = FlowManager.Instance()
                if (optcode >=1 and optcode <= 11) or optcode == 19 or optcode == 102 or optcode == 103 then
                    if flowManager:IsAuthorize() == true  and config.scheduler.measure.mode ~= MeasureMode.Manual and config.interconnection.reportMode ~= _G.ReportMode.Maintain then

                        if optcode == 1 then
                            if flowManager:IsFlowEnable() == true then
                                local flow = MeasureFlow:new({}, MeasureType.Sample)
                                flow.name = "MeasureSample"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 2 then
                            --标样核查(测标准液)
                            if flowManager:IsFlowEnable() == true then
                                local flow = MeasureFlow:new({}, MeasureType.Standard)
                                flow.name  = "MeasureStandard"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 3 then
                            --零点核查
                            if flowManager:IsFlowEnable() == true then
                                local flow = MeasureFlow:new({}, MeasureType.ZeroCheck)
                                flow.name  = "MeasureZeroCheck"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 4 then
                            --跨度核查(标样(跨度)核查)
                            if flowManager:IsFlowEnable() == true then
                                local flow = MeasureFlow:new({}, MeasureType.RangeCheck)
                                flow.name  = "MeasureRangeCheck"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 5 then
                            --空白测试(测空白水)
                            if flowManager:IsFlowEnable() == true then
                                local flow = MeasureFlow:new({}, MeasureType.Blank)
                                flow.name  = "MeasureBlank"
                                FlowList.AddFlow(flow)
                                flowManager:StartFlow()
                            end
                        elseif optcode == 6 then    --测平行样(外部)

                        elseif optcode == 7 then    --加标回收

                        elseif optcode == 8 then
                            ----(空白校准)测空白水
                            --local flow = CalibrateFlow:new({}, CalibrateType.onlyCalibrateBlank)
                            --flow.name  = "OnlyCalibrateBlank"
                            --FlowList.AddFlow(flow)
                            --flowManager:StartFlow()
                            log:debug("暂未开放零点校准功能")
                        elseif optcode == 9 then
                            --(标样校准)测标准液
                            local flow = CalibrateFlow:new({}, CalibrateType.onlyCalibrateStandard)
                            flow.name  = "OnlyCalibrateStandard"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        elseif optcode == 10 then
                            --(初始化清洗)深度清洗
                            local flow = CleanFlow:new({},cleanType.cleanDeeply)
                            flow.name  = "CleanDeeply"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        elseif optcode == 11 then
                            flowManager:StopFlow()  --停止

                            if config.scheduler.measure.mode == MeasureMode.Continous then
                                config.scheduler.measure.mode = MeasureMode.Trigger
                                config.modifyRecord.scheduler(true)
                                ConfigLists.SaveSchedulerConfig()

                                local updateWidgetManager = UpdateWidgetManager.Instance()
                                updateWidgetManager:Update(UpdateEvent.ChangeMeaModeOnHome, "Modbus")
                            end
                        elseif optcode == 19 then
                            --定标
                            local flow = CalibrateFlow:new({}, CalibrateType.calibrate)
                            flow.name  = "Calibrate"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        elseif optcode == 102 then  --测加标样(外部)

                        elseif optcode == 103 then  --测平行样(外部)

                        end
                    end
                elseif optcode == 12 then  --仪器重启
                    log:debug("Modbus 远程控制仪器重启")
                    modbusManager:Reboot()
                elseif optcode == 13 then  --校时
                    modbusManager:SetBCDTimeToSystem(RegisterType.Hold, offsetAddress + 1)
                elseif optcode == 14 then  --模式设置
                    if not config.system.solidifyMeaParamFromModbus then
                        local mode = modbusManager:GetShort(RegisterType.Hold, offsetAddress + 1)
                        if mode == 1 then
                            config.scheduler.measure.mode = MeasureMode.Continous
                        elseif mode == 2 then
                            config.scheduler.measure.mode = MeasureMode.Periodic
                        elseif mode == 3 then
                            config.scheduler.measure.mode = MeasureMode.Timed
                        elseif mode == 4 or mode == 5 then
                            config.scheduler.measure.mode = MeasureMode.Trigger
                            --elseif mode == 5 then
                            --config.scheduler.measure.mode = MeasureMode.Manual
                        end

                        config.modifyRecord.scheduler(true)
                        ConfigLists.SaveSchedulerConfig()

                        local updateWidgetManager = UpdateWidgetManager.Instance()
                        updateWidgetManager:Update(UpdateEvent.ChangeAutoMeasure, "Modbus")
                    end
                elseif optcode >= 15 and optcode <= 18 then  --测量间隔设置
                    if not config.system.solidifyMeaParamFromModbus then
                        local modbusManager = ModbusManager.Instance()
                        local inputValue = modbusManager:GetShort(RegisterType.Hold, offsetAddress + 1)

                        if inputValue >= 30 and inputValue <= 65535 then
                            if optcode == 15 then
                                config.scheduler.measure.interval = inputValue / 60
                            elseif optcode == 16 then
                                config.scheduler.zeroCheck.interval = inputValue / 60
                            elseif optcode == 17 then
                                config.scheduler.rangeCheck.interval = inputValue / 60
                            elseif optcode == 18 then
                                config.scheduler.check.interval = inputValue / 60
                            end
                        end

                        config.modifyRecord.scheduler(true)
                        ConfigLists.SaveSchedulerConfig()

                        local updateWidgetManager = UpdateWidgetManager.Instance()
                        updateWidgetManager:Update(UpdateEvent.ChangeAutoMeasure, "Modbus")
                    end
                elseif optcode == 100 then  --开启采水
                    if config.interconnection.collectSampleMode == setting.mode.collectSample.trigger and config.interconnection.reportMode ~= _G.ReportMode.Maintain then
                        log:debug("远程控制采水继电器 开")
                        WaterCollector.Instance():TurnOn()
                        Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, true)
                    end
                elseif optcode == 101 then  --关闭采水
                    if config.interconnection.collectSampleMode == setting.mode.collectSample.trigger and config.interconnection.reportMode ~= _G.ReportMode.Maintain then
                        log:debug("远程控制采水继电器 关")
                        WaterCollector.Instance():TurnOff()
                        Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, false)
                    end
                end
            end,
        },
        [0x1201] =     -- 0x1201 - 0x12FF 预留
        {
            number = 255,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
    }
}