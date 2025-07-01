setting.externalInterface = {}

setting.externalInterface.modbus =
{
    inputRegAddr = 0,
    inputRegNum = 0,
    holdRegAddr = 0,
    holdRegNum = 256,
    registers =
    {
        [0] = --测量浓度值 (最近测量结果的浓度值)
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 0 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetSmallFloat(RegisterType.Hold, offsetAddress, status.measure.newResult.measure.consistency)
            end,
            write = function()
            end,
        },
        [2] = --测量时间 年、月
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 2 - setting.externalInterface.modbus.holdRegAddr
                local ret = 0
                local temp = os.date("*t",status.measure.newResult.measure.dateTime)
                ret = (temp.year%100)*100 + temp.month

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [3] = --测量时间 日、时
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 3 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local temp = os.date("*t",status.measure.newResult.measure.dateTime)
                ret = temp.day*100 + temp.hour

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [4] = --测量时间 分、秒
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 4 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local temp = os.date("*t",status.measure.newResult.measure.dateTime)
                ret = temp.min*100 + temp.sec

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [5] = --仪器状态
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 5 - setting.externalInterface.modbus.holdRegAddr

                local statusManager = StatusManager.Instance()
                local runStatus = statusManager:GetStatus()
                local name = runStatus:GetName()

                local modbusManager = ModbusManager.Instance()
                if setting.modbusCoder.statusID[name].szbaID ~= nil then
                    modbusManager:SetShort(RegisterType.Hold, offsetAddress, setting.modbusCoder.statusID[name].szbaID)
                else
                    modbusManager:SetShort(RegisterType.Hold, offsetAddress, 0)
                end
            end,
            write = function()
            end,
        },
        [6] = --数据标记
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 6 - setting.externalInterface.modbus.holdRegAddr

                local resultInfo = status.measure.newResult.measure.resultInfo
                local ret = 0

                if resultInfo == "N" then
                    ret = 0
                elseif resultInfo == "F" then
                    ret = 1
                elseif resultInfo == "M" then
                    ret = 2
                elseif resultInfo == "D" then
                    ret = 4
                elseif resultInfo == "C" then
                    ret = 5
                elseif resultInfo == "T" then
                    ret = 8
                elseif resultInfo == "L" then
                    ret = 9
                end


                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [7] =     -- 7 预留
        {
            number = 1,	-- Register number
            read = function()
            end,

            write = function()
            end,
        },
        [8] = --消解温度
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 8 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, 0)
            end,
            write = function()
            end,
        },
        [9] = --消解时长
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 9 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local time = 0
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, time)
            end,
            write = function()
            end,
        },
        [10] = --测量量程
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 10 - setting.externalInterface.modbus.holdRegAddr
                local rangeMax = 0

                if config.system.rangeViewMap[config.measureParam.range[config.measureParam.currentRange+1] + 1] ~= nil then
                    rangeMax = config.system.rangeViewMap[config.measureParam.range[config.measureParam.currentRange+1] + 1].view
                end

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetSmallFloat(RegisterType.Hold, offsetAddress, rangeMax)
            end,
            write = function()
            end,
        },
        [12] = --测量量程
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 12 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetSmallFloat(RegisterType.Hold, offsetAddress, status.measure.newResult.measure.absorbance)
            end,
            write = function()
            end,
        },
        [14] = --测量精度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 14 - setting.externalInterface.modbus.holdRegAddr
                local accuracy = setting.measureResult.useNum
                local ret = 1
                for i = 1, accuracy do
                    ret = ret/10
                end
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetSmallFloat(RegisterType.Hold, offsetAddress,ret)
            end,
            write = function()
            end,
        },
        [16] = --测量截距
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 16 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetSmallFloat(RegisterType.Hold, offsetAddress,config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange+1]+1].curveB)
            end,
            write = function()
            end,
        },
        [18] = --测量斜率
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 18 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetSmallFloat(RegisterType.Hold, offsetAddress,config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange+1]+1].curveK)
            end,
            write = function()
            end,
        },
        [20] = --标液吸光度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 20 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetSmallFloat(RegisterType.Hold, offsetAddress,status.measure.report.rangeCheck.absorbance)
            end,
            write = function()
            end,
        },
        [22] = --零点吸光度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 22 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetSmallFloat(RegisterType.Hold, offsetAddress,status.measure.report.zeroCheck.absorbance)
            end,
            write = function()
            end,
        },
        [24] = --标液浓度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 24 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetSmallFloat(RegisterType.Hold, offsetAddress,status.measure.report.rangeCheck.consistency)
            end,
            write = function()
            end,
        },
        [26] = --零点浓度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 26 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetSmallFloat(RegisterType.Hold, offsetAddress,status.measure.report.zeroCheck.consistency)
            end,
            write = function()
            end,
        },
        [28] = --基准吸光度
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 28 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetSmallFloat(RegisterType.Hold, offsetAddress,status.measure.report.check.absorbance)
            end,
            write = function()
            end,
        },
        [30] = --标样校准浓度值
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 30 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetSmallFloat(RegisterType.Hold, offsetAddress,status.measure.report.check.consistency)
            end,
            write = function()
            end,
        },
        [32] = --校准时间 年、月
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 32 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local temp = os.date("*t",status.measure.newResult.calibrate.standardCalibrateDateTime)
                ret = (temp.year%100)*100 + temp.month

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [33] = --校准时间 日、时
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 33 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local temp = os.date("*t",status.measure.newResult.calibrate.standardCalibrateDateTime)
                ret = temp.day*100 + temp.hour

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [34] = --校准时间 分、秒
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 34 - setting.externalInterface.modbus.holdRegAddr

                local ret = 0
                local temp = os.date("*t",status.measure.newResult.calibrate.standardCalibrateDateTime)
                ret = temp.min*100 + temp.sec

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [35] = --零点核查结果
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 35 - setting.externalInterface.modbus.holdRegAddr
                local ret = 0
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [36] = --量程核查结果
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 36 - setting.externalInterface.modbus.holdRegAddr
                local ret = 0
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [37] = --标样核查结果
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 37 - setting.externalInterface.modbus.holdRegAddr
                local ret = 0
                local consistency =status.measure.offline.rangeCheck.consistency
                if status.measure.offline.rangeCheck.dateTime > status.measure.report.rangeCheck.dateTime then
                    consistency = status.measure.offline.rangeCheck.consistency
                end
                local checkConsistency = config.measureParam.checkConsistency
                local checkError = math.abs(consistency - checkConsistency)/checkConsistency
                if status.measure.offline.rangeCheck.dateTime == 0 then
                    ret = 0
                elseif checkError < config.measureParam.checkErrorLimit then
                    ret = 1
                else
                    ret = 2
                end
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [38] = --标样校准结果
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 38 - setting.externalInterface.modbus.holdRegAddr
                local ret = 0
                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [39] =     -- 39 预留
        {
            number = 1,	-- Register number
            read = function()
            end,

            write = function()
            end,
        },
        [40] = --标样核查时间 年、月
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 40 - setting.externalInterface.modbus.holdRegAddr

                local time = status.measure.report.rangeCheck.dateTime
                if status.measure.offline.rangeCheck.dateTime > status.measure.report.rangeCheck.dateTime then
                    time = status.measure.offline.rangeCheck.dateTime
                end
                local ret = 0
                local temp = os.date("*t",time)
                ret = (temp.year%100)*100 + temp.month

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [41] = --标样核查时间 日、时
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 41 - setting.externalInterface.modbus.holdRegAddr

                local time = status.measure.report.rangeCheck.dateTime
                if status.measure.offline.rangeCheck.dateTime > status.measure.report.rangeCheck.dateTime then
                    time = status.measure.offline.rangeCheck.dateTime
                end
                local ret = 0
                local temp = os.date("*t",time)
                ret = temp.day*100 + temp.hour

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [42] = --标样核查时间 分、秒
        {
            number = 1,		-- Register number

            read = function()
                local offsetAddress = 42 - setting.externalInterface.modbus.holdRegAddr

                local time = status.measure.report.rangeCheck.dateTime
                if status.measure.offline.rangeCheck.dateTime > status.measure.report.rangeCheck.dateTime then
                    time = status.measure.offline.rangeCheck.dateTime
                end
                local ret = 0
                local temp = os.date("*t",time)
                ret = temp.min*100 + temp.sec

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetShort(RegisterType.Hold, offsetAddress, ret)
            end,
            write = function()
            end,
        },
        [43] = --测量检出限
        {
            number = 2,		-- Register number

            read = function()
                local offsetAddress = 43 - setting.externalInterface.modbus.holdRegAddr

                local modbusManager = ModbusManager.Instance()
                modbusManager:SetSmallFloat(RegisterType.Hold, offsetAddress, setting.measureResult.quantifyLowLimit)
            end,
            write = function()
            end,
        },
        [45] =     -- 44-49 预留
        {
            number = 5,	-- Register number
            read = function()
            end,

            write = function()
            end,
        },
        [50] =   --启动测量
        {
            number = 1,		-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 50 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local ret = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if ret ~= 0 then
                    local flowManager = FlowManager.Instance()
                    if flowManager:IsAuthorize() == true then
                        local flow = MeasureFlow:new({}, MeasureType.Sample)
                        flow.name = "MeasureSample"
                        FlowList.AddFlow(flow)
                        flowManager:StartFlow()
                    end
                end
            end,
        },
        [51] =   --启动标定
        {
            number = 1,		-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 51 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local ret = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if ret ~= 0 then
                    local flowManager = FlowManager.Instance()
                    if flowManager:IsAuthorize() == true then
                        local flow = CalibrateFlow:new({}, CalibrateType.calibrate)
                        flow.name  = "Calibrate"
                        FlowList.AddFlow(flow)
                        flowManager:StartFlow()
                    end
                end
            end,
        },
        [52] =     -- 52 预留
        {
            number = 52,	-- Register number
            read = function()
            end,

            write = function()
            end,
        },
        [53] =   --停止测量
        {
            number = 1,		-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 53 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local ret = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if ret ~= 0 then
                    local flowManager = FlowManager.Instance()
                    flowManager:StopFlow()

                    if config.scheduler.measure.mode == MeasureMode.Continous then
                        config.scheduler.measure.mode = MeasureMode.Trigger
                        config.modifyRecord.scheduler(true)
                        ConfigLists.SaveSchedulerConfig()

                        local updateWidgetManager = UpdateWidgetManager.Instance()
                        updateWidgetManager:Update(UpdateEvent.ChangeMeaModeOnHome, "Modbus")
                    end
                end
            end,
        },
        [54] =   --远程复位
        {
            number = 1,		-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 54 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local ret = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if ret ~= 0 then
                    modbusManager:Reboot()
                end
            end,
        },
        [55] =   --标液核查（量程核查）
        {
            number = 1,		-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 55 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local ret = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if ret ~= 0 then
                    local flowManager = FlowManager.Instance()
                    if flowManager:IsAuthorize() == true then
                        local flow = MeasureFlow:new({}, MeasureType.RangeCheck)
                        flow.name  = "MeasureRangeCheck"
                        FlowList.AddFlow(flow)
                        flowManager:StartFlow()
                    end
                end
            end,
        },
        [56] =   --零点检查
        {
            number = 1,		-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 56 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local ret = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if ret ~= 0 then
                    log:debug("该设备没有零点检查流程")
                    local flowManager = FlowManager.Instance()
                    if flowManager:IsAuthorize() == true then
                        local flow = MeasureFlow:new({}, MeasureType.ZeroCheck)
                        flow.name  = "MeasureZeroCheck"
                        FlowList.AddFlow(flow)
                        flowManager:StartFlow()
                    end
                end
            end,
        },
        [57] =   --零点校准
        {
            number = 1,		-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 57 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local ret = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if ret ~= 0 then
                    local flowManager = FlowManager.Instance()
                    if flowManager:IsAuthorize() == true then
                        local flow = CalibrateFlow:new({}, CalibrateType.onlyCalibrateBlank)
                        flow.name  = "OnlyCalibrateBlank"
                        FlowList.AddFlow(flow)
                        flowManager:StartFlow()
                    end
                end
            end,
        },
        [58] =   --标液校准
        {
            number = 1,		-- Register number

            read = function()
            end,

            write = function()
                local offsetAddress = 58 - setting.externalInterface.modbus.holdRegAddr
                local modbusManager = ModbusManager.Instance()
                local ret = modbusManager:GetShort(RegisterType.Hold, offsetAddress)
                if ret ~= 0 then
                    local flowManager = FlowManager.Instance()
                    if flowManager:IsAuthorize() == true then
                        local flow = CalibrateFlow:new({}, CalibrateType.onlyCalibrateStandard)
                        flow.name  = "OnlyCalibrateStandard"
                        FlowList.AddFlow(flow)
                        flowManager:StartFlow()
                    end
                end
            end,
        },
        [59] =     -- 59-255 预留
        {
            number = 197,	-- Register number
            read = function()
            end,

            write = function()
            end,
        },
    }
}
