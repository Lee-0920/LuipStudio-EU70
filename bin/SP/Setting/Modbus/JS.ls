setting.externalInterface = {}

setting.externalInterface.modbus =
{
    inputRegAddr = 30001,
    inputRegNum = 300,
    holdRegAddr = 40001,
    holdRegNum = 300,
    isClibrateTime = 0,
    registers =
    {
        [30001] =   --测量值
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 30001 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.measure.consistency)
           end,

            write = function()
            end,
        },
        [30003] =   --吸光度值
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 30003 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.measure.absorbance)
           end,

            write = function()
            end,
        },
        [30005] =     --取水时间，年
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30005 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.measure.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.year - 1970)
            end,

            write = function()
            end,
        },
        [30006] =   --取水时间，月
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30006 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.measure.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.month)
            end,

            write = function()
            end,
        },
        [30007] =   --取水时间，日
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30007 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.measure.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.day)
            end,

            write = function()
            end,
        },
        [30008] =     --取水时间，时
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30008 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.measure.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.hour)
            end,

            write = function()
            end,
        },
        [30009] =   --取水时间，分
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30009 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.measure.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.min)
            end,

            write = function()
            end,
        },
        [30010] =   --取水时间，秒
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30010 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.measure.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.sec)
            end,

            write = function()
            end,
        },
        [30011] =     --仪器状态
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30011 - setting.externalInterface.modbus.inputRegAddr
                local modbusManager = ModbusManager.Instance()
                local alarmManager = AlarmManager.Instance()

                local statusManager = StatusManager.Instance()
                local runStatus = statusManager:GetStatus()
                local name = runStatus:GetName()

                if setting.modbusCoder.statusID[name] ~= nil then
                    if setting.modbusCoder.statusID[name].ID == 0 and  alarmManager:GetUpLoadStatus() then
                        modbusManager:SetShort(RegisterType.Input, offsetAddress, 6) --空闲状态有告警返回报警状态
                    else
                        modbusManager:SetShort(RegisterType.Input, offsetAddress, setting.modbusCoder.statusID[name].jsID)
                    end
                else
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 8)
                end

            end,

            write = function()
            end,
        },
        [30012] =   --报警类型
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30012 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local AlarmManager = AlarmManager.Instance()
                local alarm = AlarmManager:GetCurrentAlarm()
                local name = alarm:GetName()
                local cause = alarm:GetCause()

                local alarmVal = 0
                local modbusManager = ModbusManager.Instance()

                if name == "空闲" or name == "" then
                    alarmVal = 0
                elseif name == "通信异常" then
                    alarmVal = 8
                elseif name == "抽取失败" then
                    if cause == "水样" then
                        alarmVal = 2
                    else
                        alarmVal = 8
                    end
                elseif name == "请更换试剂" then
                    if cause == "零点校准液" then
                        alarmVal = 4
                    elseif cause == "分析废液" then
                        alarmVal = 8
                    else
                        alarmVal = 3
                    end
                elseif name == "耗材已过期" then
                    alarmVal = 8
                elseif name == "温控故障" then
                    alarmVal = 5
                elseif name == "升级失败" then
                    alarmVal = 8
                elseif name == "仪器运行故障" then
                    alarmVal = 8
                elseif name == "排消解液异常" then
                    alarmVal = 6
                elseif name == "蠕动泵校准失败" then
                    alarmVal = 8
                elseif name == "水样超上限" then
                    alarmVal = 7
		        elseif name == "水样超下限" then
                    alarmVal = 7
                elseif name == "水样超量程" then
                    alarmVal = 7
                elseif name == "校准结果错误" then
                    alarmVal = 8
                elseif name == "校准结果错误" then
                    alarmVal = 7
                elseif name == "核查结果错误" then
                    alarmVal = 7
                elseif name == "仪器漏液" then
                    alarmVal = 1
                else
                    alarmVal = 8
                end
                modbusManager:SetShort(RegisterType.Input, offsetAddress, alarmVal)
            end,

            write = function()
            end,
        },
        [30013] =   --有无零点校准结果 0：无，1：有
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30013 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                if status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime == 0 then
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 0)
                else
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 1)
                end
            end,

            write = function()
            end,
        },
        [30014] =     --零点校准浓度测量值
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 30014 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].point0Consistency)
            end,

            write = function()
            end,
        },
        [30016] =     --零点校准结果0：无 1：有效 2：偏离

        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30016 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                if status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime == 0 then
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 0)
                else
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 1)
                end
            end,

            write = function()
            end,
        },
        [30017] =     --零点校准时间，年
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30017 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.year - 1970)
            end,

            write = function()
            end,
        },
        [30018] =   --零点校准时间，月
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30018 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.month)
            end,

            write = function()
            end,
        },
        [30019] =   --零点校准时间，日
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30019 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.day)
            end,

            write = function()
            end,
        },
        [30020] =     --零点校准时间，时
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30020 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.hour)
            end,

            write = function()
            end,
        },
        [30021] =   --零点校准时间，分
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30021 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.min)
            end,

            write = function()
            end,
        },
        [30022] =   --零点校准时间，秒
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30022 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.sec)
            end,

            write = function()
            end,
        },
        [30023] =   --有无校满校准结果 0：无，1：有
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30023 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                if status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime == 0 then
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 0)
                else
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 1)
                end
            end,

            write = function()
            end,
        },
        [30024] =     --校满校准浓度测量值
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 30024 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].point1Consistency)
            end,

            write = function()
            end,
        },
        [30026] =     --校满校准结果0：无 1：有效 2：偏离

        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30026 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                if status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime == 0 then
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 0)
                else
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 1)
                end
            end,

            write = function()
            end,
        },
        [30027] =     --校满校准时间，年
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30027 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.year - 1970)
            end,

            write = function()
            end,
        },
        [30028] =   --校满校准时间，月
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30028 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.month)
            end,

            write = function()
            end,
        },
        [30029] =   --校满校准时间，日
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30029 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.day)
            end,

            write = function()
            end,
        },
        [30030] =     --校满校准时间，时
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30030 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.hour)
            end,

            write = function()
            end,
        },
        [30031] =   --校满校准时间，分
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30031 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.min)
            end,

            write = function()
            end,
        },
        [30032] =   --校满校准时间，秒
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30032 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.sec)
            end,

            write = function()
            end,
        },
        [30033] =     --自动校准间隔
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30033 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()

                modbusManager:SetShort(RegisterType.Input, offsetAddress, config.scheduler.calibrate.interval)

            end,

            write = function()
            end,
        },
        [30034] =     --斜率
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 30034 - setting.externalInterface.modbus.inputRegAddr

                local curveK = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange+1] + 1].curveK
                local curveB = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange+1] + 1].curveB
                curveK, curveB  = CurveParamCurveXYChange(curveK, curveB)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, curveK)
            end,

            write = function()
            end,
        },
        [30036] =     --截距
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 30036 - setting.externalInterface.modbus.inputRegAddr

                local curveK = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange+1] + 1].curveK
                local curveB = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange+1] + 1].curveB
                curveK, curveB  = CurveParamCurveXYChange(curveK, curveB)

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, curveB)
            end,

            write = function()
            end,
        },
        [30038] =    --  修正因子
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 30038 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, config.measureParam.reviseFactor)
            end,

            write = function()
            end,
        },
        [30040] =    -- 消解时长(单位:分钟)
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30040 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local minute = math.ceil(config.measureParam.reacTime/60)

                modbusManager:SetShort(RegisterType.Input, offsetAddress, minute)
            end,

            write = function()
            end,
        },
        [30041] =    --  消解温度
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 30041 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, config.measureParam.reactTemperature)
            end,

            write = function()
            end,
        },
        [30043] =    --  当前量程
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 30043 - setting.externalInterface.modbus.inputRegAddr

                local range = 200
                local rangeNum = config.measureParam.range[config.measureParam.currentRange+1] + 1
                if config.system.rangeViewMap[rangeNum] ~= nil then
                    range = config.system.rangeViewMap[rangeNum].view
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, range)
            end,

            write = function()
            end,
        },
        [30045] =     --  量程校准液浓度
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 30045 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange+1] + 1].RangeConsistency)
            end,

            write = function()
            end,
        },
        [30047] =     --  量程校准液吸光度值
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 30047 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.calibrate[config.measureParam.range[config.measureParam.currentRange+1] + 1].point1Absorbance)
            end,

            write = function()
            end,
        },
        [30049] =     --反控启动测量结果 0：失败1：成功
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30049 - setting.externalInterface.modbus.inputRegAddr
                local modbusManager = ModbusManager.Instance()

                local statusManager = StatusManager.Instance()
                local runStatus = statusManager:GetStatus()
                if runStatus:GetName() == "MeasureSample" then
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 1)
                else
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 0)
                end
           end,

            write = function()
            end,
        },
        [30050] =     --反控校时结果 0：失败1：成功
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30050 - setting.externalInterface.modbus.inputRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Input, offsetAddress, setting.externalInterface.modbus.isClibrateTime)
            end,

            write = function()
            end,
        },
        [30051] =     --反控校准结果 0：失败1：成功
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30051 - setting.externalInterface.modbus.inputRegAddr
                local modbusManager = ModbusManager.Instance()

                local statusManager = StatusManager.Instance()
                local runStatus = statusManager:GetStatus()
                if runStatus:GetName() == "Calibrate" then
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 1)
                else
                    modbusManager:SetShort(RegisterType.Input, offsetAddress, 0)
                end
           end,

            write = function()
            end,
        },
        [30052] =     --核查结果
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 30052 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.check.consistency)
            end,

            write = function()
            end,
        },
        [30054] =     --核查时间，年
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30054 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.check.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.year - 1970)
            end,

            write = function()
            end,
        },
        [30055] =   --核查时间，月
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30055 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.check.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.month)
            end,

            write = function()
            end,
        },
        [30056] =   --核查时间，日
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30056 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.check.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.day)
            end,

            write = function()
            end,
        },
        [30057] =     --核查时间，时
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30057 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.check.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.hour)
            end,

            write = function()
            end,
        },
        [30058] =   --核查时间，分
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30058 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.check.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.min)
            end,

            write = function()
            end,
        },
        [30059] =   --核查时间，秒
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30059 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.check.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.sec)
            end,

            write = function()
            end,
        },
        [30060] =     -- 测量参数
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30060 - setting.externalInterface.modbus.inputRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Input, offsetAddress, setting.instrument.deviceTypeNum.TOC)
           end,

            write = function()
            end,
        },
        [30061] =     --测试结果类型
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30061 - setting.externalInterface.modbus.inputRegAddr

                local code = status.measure.report.measure.resultType
                if status.measure.report.measure.resultType == MeasureType.Blank then
                    code = 0
                elseif status.measure.report.measure.resultType == MeasureType.Sample then
                    code = 1
                elseif status.measure.report.measure.resultType == MeasureType.Standard then
                    code = 2
                elseif status.measure.newResult.measure.resultType == MeasureType.ExtAddstandard then
                    code = 8
                elseif status.measure.newResult.measure.resultType == MeasureType.ExtParallel then
                    code = 9
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
        [30062] =     --测量时长
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30062 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Input, offsetAddress, status.measure.report.measure.measureTime)
           end,

            write = function()
            end,
        },
        [30063] =     --  当前动作
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30063 - setting.externalInterface.modbus.inputRegAddr

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
        [30064] =     --   当前温度
        {
            number = 4,	-- Register number

            read = function()
                local offsetAddress = 30064 - setting.externalInterface.modbus.inputRegAddr

                local dTemp = dc:GetDigestTemperature()
                local eTemp = dc:GetEnvironmentTemperature()

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, dTemp)
                modbusManager:SetFloat(RegisterType.Input, offsetAddress + 2, eTemp)
            end,

            write = function()
            end,
        },
        [30068] =     --报警时间，年
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30068 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local AlarmManager = AlarmManager.Instance()
                local dateTime = AlarmManager:GetCurrentAlarm():GetTime()
                local temp = os.date("*t", dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.year - 1970)
            end,

            write = function()
            end,
        },
        [30069] =   --报警时间，月
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30069 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local AlarmManager = AlarmManager.Instance()
                local dateTime = AlarmManager:GetCurrentAlarm():GetTime()
                local temp = os.date("*t", dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.month)
            end,

            write = function()
            end,
        },
        [30070] =   --报警时间，日
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30070 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local AlarmManager = AlarmManager.Instance()
                local dateTime = AlarmManager:GetCurrentAlarm():GetTime()
                local temp = os.date("*t", dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.day)
            end,

            write = function()
            end,
        },
        [30071] =     --报警时间，时
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30071 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local AlarmManager = AlarmManager.Instance()
                local dateTime = AlarmManager:GetCurrentAlarm():GetTime()
                local temp = os.date("*t", dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.hour)
            end,

            write = function()
            end,
        },
        [30072] =   --报警时间，分
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30072 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local AlarmManager = AlarmManager.Instance()
                local dateTime = AlarmManager:GetCurrentAlarm():GetTime()
                local temp = os.date("*t", dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.min)
            end,

            write = function()
            end,
        },
        [30073] =   --报警时间，秒
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30073 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local AlarmManager = AlarmManager.Instance()
                local dateTime = AlarmManager:GetCurrentAlarm():GetTime()
                local temp = os.date("*t", dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.sec)
            end,

            write = function()
            end,
        },
        [30074] =     --  量程表
        {
            number = 8,	-- Register number

            read = function()
                local offsetAddress = 30074 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local rangeNum = tonumber(setting.measure.range.rangeNum)
                if rangeNum > 4 then rangeNum = 4 end
                if rangeNum ~= nil then
                    for i = 1,rangeNum do
                        if config.system.rangeViewMap[i] ~= nil then
                            modbusManager:SetFloat(RegisterType.Input, offsetAddress + 2*(i-1), config.system.rangeViewMap[i].view)
                        end
                    end
                end

            end,

            write = function()
            end,
        },
        [30082] =     --  产品名称
        {
            number = 32,	-- Register number

            read = function()
                local offsetAddress = 30082 - setting.externalInterface.modbus.inputRegAddr
                local regSize = setting.externalInterface.modbus.registers[30082].number

                local modbusManager = ModbusManager.Instance()
                local str = setting.instrument.name

                modbusManager:SetString(RegisterType.Input, offsetAddress, str, regSize)
           end,

            write = function()
            end,
        },
        [30114] =     --  产品型号
        {
            number = 32,	-- Register number

            read = function()
                local offsetAddress = 30114 - setting.externalInterface.modbus.inputRegAddr
                local regSize = setting.externalInterface.modbus.registers[30114].number

                local modbusManager = ModbusManager.Instance()
                local str = config.info.instrument["model"]

                modbusManager:SetString(RegisterType.Input, offsetAddress, str, regSize)
           end,

            write = function()
            end,
        },
        [30146] =     --  生产厂商
        {
            number = 32,	-- Register number

            read = function()
                local offsetAddress = 30146 - setting.externalInterface.modbus.inputRegAddr
                local regSize = setting.externalInterface.modbus.registers[30146].number

                local modbusManager = ModbusManager.Instance()
                local str = config.info.instrument["manuFacturer"]

                modbusManager:SetString(RegisterType.Input, offsetAddress, str, regSize)
           end,

            write = function()
            end,
        },
        [30178] =     -- 生产序列号
        {
            number = 16,	-- Register number

            read = function()
                local offsetAddress = 30178 - setting.externalInterface.modbus.inputRegAddr
                local regSize = setting.externalInterface.modbus.registers[30178].number

                local modbusManager = ModbusManager.Instance()
                local str = config.info.instrument["sn"]

                modbusManager:SetString(RegisterType.Input, offsetAddress, str, regSize)
           end,

            write = function()
            end,
        },
        [30194] =     --   液路板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if lc ~= nil then
                    local offsetAddress = 30194 - setting.externalInterface.modbus.inputRegAddr

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
                        local modbusManager = ModbusManager.Instance()
                        modbusManager:SetShort(RegisterType.Input, offsetAddress, major * 256 + minor)
                        modbusManager:SetShort(RegisterType.Input, offsetAddress + 1, revision * 256 + build)
                    end
                end
           end,

            write = function()
            end,
        },
        [30196] =     --   温控板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if tc ~= nil then
                    local offsetAddress = 30196 - setting.externalInterface.modbus.inputRegAddr

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
        [30198] =     --   信号板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if oa ~= nil then
                    local offsetAddress = 30198 - setting.externalInterface.modbus.inputRegAddr

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
        [30200] =     --  主控板固件版本
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 30200 - setting.externalInterface.modbus.inputRegAddr

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
        [30202] =     --智能诊断结果
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30202 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Input, offsetAddress, ModbusInterface.detectResult)

            end,

            write = function()
            end,
        },
        [30203] =     --通讯诊断结果
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30203 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Input, offsetAddress, ModbusInterface.commTestResult)

            end,

            write = function()
            end,
        },
        [30204] =     --反应温度
        {
            number = 2,	-- Register number

            read = function()
                local offsetAddress = 30204 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, config.measureParam.reactTemperature)

            end,

            write = function()
            end,
        },
        [30206] =     --反应时间
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30206 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Input, offsetAddress, config.measureParam.reacTime)

            end,

            write = function()
            end,
        },
        [30207] =     --系统时间，年
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30207 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t")
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.year - 1970)
            end,

            write = function()
            end,
        },
        [30208] =   --系统时间，月
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30208 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t")
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.month)
            end,

            write = function()
            end,
        },
        [30209] =   --系统时间，日
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30209 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t")
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.day)
            end,

            write = function()
            end,
        },
        [30210] =     --系统时间，时
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30210 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t")
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.hour)
            end,

            write = function()
            end,
        },
        [30211] =   --系统时间，分
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30211 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t")
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.min)
            end,

            write = function()
            end,
        },
        [30212] =   --系统时间，秒
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30212 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t")
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.sec)
            end,

            write = function()
            end,
        },
        [30213] =     --   驱动板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if dc ~= nil then
                    local offsetAddress = 30213 - setting.externalInterface.modbus.inputRegAddr

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
        [30215] =     --   反应板固件版本
        {
            number = 2,	-- Register number

            read = function()
                if rc ~= nil then
                    local offsetAddress = 30215 - setting.externalInterface.modbus.inputRegAddr

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
        --[30217] =     --标样核查数据标识
        --{
        --    number = 1,	-- Register number
        --
        --    read = function()
        --        local offsetAddress = 30217 - setting.externalInterface.modbus.inputRegAddr
        --
        --        local resultInfo = "N"
        --        if type(status.measure.report.check.resultInfo) == "string" then
        --            resultInfo = status.measure.report.check.resultInfo
        --            if resultInfo == "" then
        --                resultInfo = "N"
        --            end
        --        end
        --
        --        local regSize = setting.externalInterface.modbus.registers[30217].number
        --
        --        local modbusManager = ModbusManager.Instance()
        --        modbusManager:SetString(RegisterType.Input, offsetAddress, resultInfo, regSize)
        --    end,
        --
        --    write = function()
        --    end,
        --},
        [30218] =   --零点核查结果
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 30218 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.zeroCheck.consistency)
            end,

            write = function()
            end,
        },
        [30220] =   --零点核查吸光度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 30220 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.zeroCheck.absorbance)
            end,

            write = function()
            end,
        },
        [30222] =     --零点核查数据标识
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30222 - setting.externalInterface.modbus.inputRegAddr

                local resultInfo = "N"
                if type(status.measure.report.zeroCheck.resultInfo) == "string" then
                    resultInfo = status.measure.report.zeroCheck.resultInfo
                    if resultInfo == "" then
                        resultInfo = "N"
                    end
                end

                local regSize = setting.externalInterface.modbus.registers[30222].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetString(RegisterType.Input, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [30223] =     --零点核查时间，年
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30223 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.zeroCheck.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.year - 1970)
            end,

            write = function()
            end,
        },
        [30224] =   --零点核查时间，月
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30224 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.zeroCheck.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.month)
            end,

            write = function()
            end,
        },
        [30225] =   --零点核查时间，日
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30225 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.zeroCheck.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.day)
            end,

            write = function()
            end,
        },
        [30226] =     --零点核查时间，时
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30226 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.zeroCheck.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.hour)
            end,

            write = function()
            end,
        },
        [30227] =   --零点核查时间，分
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30227 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.zeroCheck.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.min)
            end,

            write = function()
            end,
        },
        [30228] =   --零点核查时间，秒
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30228 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.zeroCheck.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.sec)
            end,

            write = function()
            end,
        },
        [30229] =   --量程核查结果
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 30229 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.rangeCheck.consistency)
            end,

            write = function()
            end,
        },
        [30231] =   --量程核查吸光度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 30231 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetFloat(RegisterType.Input, offsetAddress, status.measure.report.rangeCheck.absorbance)
            end,

            write = function()
            end,
        },
        [30233] =     --量程核查数据标识
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30233 - setting.externalInterface.modbus.inputRegAddr

                local resultInfo = "N"
                if type(status.measure.report.rangeCheck.resultInfo) == "string" then
                    resultInfo = status.measure.report.rangeCheck.resultInfo
                    if resultInfo == "" then
                        resultInfo = "N"
                    end
                end

                local regSize = setting.externalInterface.modbus.registers[30233].number

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetString(RegisterType.Input, offsetAddress, resultInfo, regSize)
            end,

            write = function()
            end,
        },
        [30234] =     --量程核查时间，年
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30234 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.rangeCheck.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.year - 1970)
            end,

            write = function()
            end,
        },
        [30235] =   --量程核查时间，月
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30235 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.rangeCheck.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.month)
            end,

            write = function()
            end,
        },
        [30236] =   --量程核查时间，日
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30236 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.rangeCheck.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.day)
            end,

            write = function()
            end,
        },
        [30237] =     --量程核查时间，时
        {
            number = 1,	-- Register number

            read = function()
                local offsetAddress = 30237 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.rangeCheck.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.hour)
            end,

            write = function()
            end,
        },
        [30238] =   --量程核查时间，分
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30238 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.rangeCheck.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.min)
            end,

            write = function()
            end,
        },
        [30239] =   --量程核查时间，秒
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 30239 - setting.externalInterface.modbus.inputRegAddr

                local modbusManager = ModbusManager.Instance()
                local temp = os.date("*t", status.measure.report.rangeCheck.dateTime)
                modbusManager:SetShort(RegisterType.Input, offsetAddress, temp.sec)
            end,

            write = function()
            end,
        },
        [30240] =     -- 30217 - 30301 预留
        {
            number = 85,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },

        [40001] =     -- 启动测量指令
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 40001 - setting.externalInterface.modbus.holdRegAddr
                local flowManager = FlowManager.Instance()
                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                log:debug("ModbusManager JS 40001 optcode = "..optcode)

                if optcode == 1 and flowManager:IsFlowEnable() == true and config.interconnection.reportMode ~= _G.ReportMode.Maintain then
                    local flow = MeasureFlow:new({}, MeasureType.Sample)
                    flow.name = "MeasureSample"
                    FlowList.AddFlow(flow)
                    flowManager:StartFlow()
                end
            end,
        },
        [40002] =     --校时指令
        {
            number = 7,	-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 40002 - setting.externalInterface.modbus.holdRegAddr
                setting.externalInterface.modbus.isClibrateTime = 0
                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if optcode == 1 then
                    local year = modbusManager:GetShort(RegisterType.Hold, offsetAddress + 1) +  1970
                    local month = modbusManager:GetShort(RegisterType.Hold, offsetAddress + 2)
                    local day = modbusManager:GetShort(RegisterType.Hold, offsetAddress + 3)
                    local hour = modbusManager:GetShort(RegisterType.Hold, offsetAddress + 4)
                    local min = modbusManager:GetShort(RegisterType.Hold, offsetAddress + 5)
                    local sec = modbusManager:GetShort(RegisterType.Hold, offsetAddress + 6)

                    local time = DataTime.new()
                    time:SetYear(year)
                    time:SetMonth(month)
                    time:SetDay(day)
                    time:SetHour(hour)
                    time:SetMin(min)
                    time:SetSec(sec)
                    SetSystemTime.SetDataTime(time)
                    time = nil
                    setting.externalInterface.modbus.isClibrateTime = 1
                end
            end,
        },
        [40009] =     -- 校准指令
        {
            number = 1,	-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 40009 - setting.externalInterface.modbus.holdRegAddr
                local flowManager = FlowManager.Instance()
                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if optcode == 1 and flowManager:IsFlowEnable() == true and config.interconnection.reportMode ~= _G.ReportMode.Maintain then
                    local flow = CalibrateFlow:new({}, CalibrateType.calibrate)
                    flow.name  = "Calibrate"
                    FlowList.AddFlow(flow)
                    flowManager:StartFlow()
                end
            end,
        },
        [40010] =    --  测量控制
        {
            number = 1,	-- Register number
            read = function()
            end,

            write = function()
               local offsetAddress = 40010 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                log:debug("ModbusManager JS 40010 optcode = "..optcode)

                local flowManager = FlowManager.Instance()
                if flowManager:IsAuthorize() == true and config.interconnection.reportMode ~= _G.ReportMode.Maintain then
                    if optcode == 0 then
                        flowManager:StopFlow()

                        if config.scheduler.measure.mode == MeasureMode.Continous then
                            config.scheduler.measure.mode = MeasureMode.Trigger
                            config.modifyRecord.scheduler(true)
                            ConfigLists.SaveSchedulerConfig()

                            local updateWidgetManager = UpdateWidgetManager.Instance()
                            updateWidgetManager:Update(UpdateEvent.ChangeMeaModeOnHome, "Modbus")
                        end
                    elseif optcode == 1 then
                        if flowManager:IsFlowEnable() == true then
                            local flow = MeasureFlow:new({}, MeasureType.Sample)
                            flow.name  = "MeasureSample"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 2 then
                        if flowManager:IsFlowEnable() == true then
                            local flow = CalibrateFlow:new({}, CalibrateType.calibrate)
                            flow.name  = "Calibrate"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 3 then
                        if flowManager:IsFlowEnable() == true then
                            local flow = MeasureFlow:new({}, MeasureType.Standard)
                            flow.name  = "MeasureStandard"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 4 then
                        if flowManager:IsFlowEnable() == true then
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
                            local flow = CleanFlow:new({},cleanType.cleanDeeply)
                            flow.name  = "CleanDeeply"
                            flow.isUseStart = true
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 8 then
                        if flowManager:IsFlowEnable() == true then
                            local flow = CleanFlow:new({},cleanType.cleanAll)
                            flow.name = "CleanAll"
                            flow.isUseStart = true
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 9 then
                        if flowManager:IsFlowEnable() == true then
                            local flow = CollectSampleFlow:new()
                            flow.name  = "CollectSample"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 10 then
                        if flowManager:IsFlowEnable() == true then
                            local flow = AutoPumpCheckFlow:new()
                            flow.name = "AutoCalibratePump"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 13 then
                        if flowManager:IsFlowEnable() == true then
                            local flow = CalibrateFlow:new({}, CalibrateType.onlyCalibrateBlank)
                            flow.name  = "OnlyCalibrateBlank"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 14 then
                        if flowManager:IsFlowEnable() == true then
                            local flow = CalibrateFlow:new({}, CalibrateType.onlyCalibrateStandard)
                            flow.name  = "OnlyCalibrateStandard"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 20 then
                        if flowManager:IsFlowEnable() == true then
                            local flow = MeasureFlow:new({}, MeasureType.ZeroCheck)
                            flow.name  = "MeasureZeroCheck"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    elseif optcode == 21 then
                        if flowManager:IsFlowEnable() == true then
                            local flow = MeasureFlow:new({}, MeasureType.RangeCheck)
                            flow.name  = "MeasureRangeCheck"
                            FlowList.AddFlow(flow)
                            flowManager:StartFlow()
                        end
                    end
                end
            end,
        },
        [40011] =    --  外部采水继电器控制
        {
            number = 1,	-- Register number
            read = function()
            end,

            write = function()
                local offsetAddress = 40011 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                local optcode = modbusManager:GetShort(RegisterType.Hold, offsetAddress)

                if config.interconnection.collectSampleMode == setting.mode.collectSample.trigger then
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
        [40012] =     -- 40012 - 40301 预留
        {
            number = 289,	-- Register number

            read = function()
            end,

            write = function()
            end,
        },
    }
}
