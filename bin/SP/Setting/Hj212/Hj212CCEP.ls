setting.externalInterface.hj212 =
{
    -- 监测因子
    monitoringFactor =
    {
        text = setting.instrument.argus[1],
        name = setting.resultFileInfo.measureRecordFile[1].name,
        polId = "",
        oldPolId = "",
        decimal = config.system.decimalNum,
        unit = "mg/L",
    },

    -- 监测信息
    monitoringInfo =
    {
        [1] =
        {
            index = MonitoringInfoIndex.iEquipmentLogCode,
            infoId = "i11001",
            text = "运行日志",
            unit = " ",
            infoType = InfoType.Logger,
            read = function()   --C++已实现
                local str = ""
                return str
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [2] =
        {
            index = MonitoringInfoIndex.iEquipmentStauts,
            infoId = "i12001",
            text = "工作状态",
            unit = " ",
            infoType = InfoType.Status,
            read = function()
                local statusManager = StatusManager.Instance()
                local runStatus = statusManager:GetStatus()
                local name = runStatus:GetName()

                local status = 0
                if setting.hj212Coder.statusID[name] ~= nil then
                    status = setting.hj212Coder.statusID[name].dbsID
                end
                return status
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [3] =
        {
            index = MonitoringInfoIndex.iReagentMargin,
            infoId = "i12003",
            text = "反应试剂余量",
            unit = "%",
            infoType = InfoType.Status,
            read = function()
                local rate = 100
                local remain = status.remain.reagent1  --试剂二余量
                local total = config.remain.reagent1.total  --试剂二总量
                rate = math.floor(100 * (remain / total))
                return rate
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [4] =
        {
            index = MonitoringInfoIndex.iAlarmInfo,
            infoId = "i12031",
            text = "分析仪报警状态",
            unit = " ",
            infoType = InfoType.Status,
            read = function()
                local AlarmManager = AlarmManager.Instance()
                local alarm = AlarmManager:GetCurrentAlarm()
                local name = alarm:GetName()
                local cause = alarm:GetCause()

                local alarmVal = 0
                if name == "空闲" then
                    alarmVal = 0 --无告警
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
                return alarmVal
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [5] =
        {
            index = MonitoringInfoIndex.iRangeValue,
            infoId = "i13001",
            text = "测量量程",
            unit = "ml/L",
            infoType = InfoType.Parameter,
            read = function()
                local rangeMax = 0
                local currentRange = config.measureParam.range[config.measureParam.currentRange + 1] + 1

                if config.system.rangeViewMap[currentRange].view ~= nil then
                    rangeMax = config.system.rangeViewMap[currentRange].view
                end

                return rangeMax
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [6] =
        {
            index = MonitoringInfoIndex.iMeaAccuracy,
            infoId = "i13002",
            text = "测量精度",
            unit = " ",
            infoType = InfoType.Parameter,
            read = function()
                local decimalNum = setting.measureResult.decimalNum

                if nil ~= config.system.decimalNum then
                    decimalNum = config.system.decimalNum
                end

                return decimalNum
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [7] =
        {
            index = MonitoringInfoIndex.iMeaInterval,
            infoId = "i13003",
            text = "测量间隔",
            unit = "分钟",
            infoType = InfoType.Parameter,
            read = function()
                local interval = 0
                if config.scheduler.measure.mode == MeasureMode.Timed then
                    local timedStart = 0
                    local timedEnd = 1
                    local timedInterval = 0
                    for i=1,24 do
                        if config.scheduler.measure.timedPoint[i] == true then
                            timedEnd = i
                            if timedStart ~= 0 and timedEnd - timedStart > 0 then
                                if timedInterval ~= 0 then
                                    if timedInterval ~= timedEnd - timedStart then
                                        timedInterval = config.scheduler.measure.interval
                                        break
                                    end
                                else
                                    timedInterval = timedEnd - timedStart
                                end
                            end
                            timedStart = i
                        end
                    end
                    interval = math.ceil(timedInterval * 60)
                else
                    interval = math.ceil(config.scheduler.measure.interval * 60)
                end

                return interval
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [8] =
        {
            index = MonitoringInfoIndex.iDigestionTemp,
            infoId = "i13004",
            text = "消解温度",
            unit = "摄氏度",
            infoType = InfoType.Parameter,
            read = function()
                local temp = 25
                return temp
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [9] =
        {
            index = MonitoringInfoIndex.iDigestionTime,
            infoId = "i13005",
            text = "消解时长",
            unit = "分钟",
            infoType = InfoType.Parameter,
            read = function()
                local time = 0
                return time
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [10] =
        {
            index = MonitoringInfoIndex.iBlankCalibrationDateTime,
            infoId = "i13006",
            text = "空白校准时间",      --YYYYMMDDHHMMSS
            unit = " ",
            infoType = InfoType.Parameter,
            read = function()
				local modbusManager = ModbusManager.Instance()
                local time = status.measure.newResult.calibrate.zeroCalibrateDateTime
                local ret = modbusManager:FormatTime("yyyyMMddhhmmss",  time)
                return ret
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [11] =
        {
            index = MonitoringInfoIndex.iCurveB,
            infoId = "i13007",
            text = "曲线截距",
            unit = " ",
            infoType = InfoType.Parameter,
            read = function()
                local curveB = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].curveB
                return curveB
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [12] =
        {
            index = MonitoringInfoIndex.iCurveK,
            infoId = "i13008",
            text = "曲线斜率",
            unit = " ",
            infoType = InfoType.Parameter,
            read = function()
                local curveK = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].curveK
                return curveK
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [13] =
        {
            index = MonitoringInfoIndex.iDetectionLimit,
            infoId = "i13009",
            text = "测量检出限",
            unit = "ml/L",
            infoType = InfoType.Parameter,
            read = function()
                local quantifyLowLimit = 0

                if nil ~= setting.measureResult.quantifyLowLimit then
                    quantifyLowLimit = setting.measureResult.quantifyLowLimit
                end

                return quantifyLowLimit
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [14] =
        {
            index = MonitoringInfoIndex.iMeaProcessValue,
            infoId = "i13010",
            text = "测量信号值",
            unit = " ",
            infoType = InfoType.Parameter,
            read = function()
                local absorbance = status.measure.newResult.measure.absorbance
                return absorbance
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [15] =
        {
            index = MonitoringInfoIndex.iLinear,
            infoId = "i13011",
            text = "线性相关系数（R²）",
            unit = " ",
            infoType = InfoType.Parameter,
            read = function()
                local R2 = 0

                if nil ~= status.measure.newResult.calibrate.R2 then
                    R2 = status.measure.newResult.calibrate.R2
                end

                return R2
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [16] =
        {
            index = MonitoringInfoIndex.iStdCalibrationDateTime,
            infoId = "i13013",
            text = "标准样校准时间",
            unit = " ",
            infoType = InfoType.Parameter,
            read = function()
                local modbusManager = ModbusManager.Instance()
                local time = status.measure.newResult.calibrate.standardCalibrateDateTime
                local ret = modbusManager:FormatTime("yyyyMMddhhmmss",  time)
                return ret
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
    },

    InitHj212 = function ()
        log:debug("Init Hj212 CCEP")
        
        if nil ~= setting.instrument.deviceTypeNum then
            local polId = 0
            for k,v in pairs(setting.instrument.deviceTypeNum) do
                polId = v
            end

            setting.externalInterface.hj212.monitoringFactor.polId = string.format("w%05d", polId)
            log:debug("polId "..setting.externalInterface.hj212.monitoringFactor.polId)
        end

        if nil ~= setting.instrument.oldTypeNum then
            local oldPolId = 0
            for k,v in pairs(setting.instrument.oldTypeNum) do
                oldPolId = v
            end

            setting.externalInterface.hj212.monitoringFactor.oldPolId = string.format("%03d", oldPolId)
            log:debug("oldPolId "..setting.externalInterface.hj212.monitoringFactor.oldPolId)
        end
    end,

    StartEquipmentFlow = function(flowCode)
        log:debug("HJ212远程控制 Code = ".. flowCode)
        local flowManager = FlowManager.Instance()
        local ret = true

        if flowManager:IsAuthorize() == true then

            if flowCode == 0 then  --定标
                if flowManager:IsFlowEnable() == true then
                    local flow = CalibrateFlow:new({}, CalibrateType.calibrate)
                    flow.name  = "Calibrate"
                    FlowList.AddFlow(flow)
                    flowManager:StartFlow()
                end
            elseif flowCode == 1 then  --测量水样
                if flowManager:IsFlowEnable() == true then
                    local flow = MeasureFlow:new({}, MeasureType.Sample)
                    flow.name = "MeasureSample"
                    FlowList.AddFlow(flow)
                    flowManager:StartFlow()
                end
            elseif flowCode == 2 then  --标样
                if flowManager:IsFlowEnable() == true then
                    local flow = MeasureFlow:new({}, MeasureType.Standard)
                    flow.name = "MeasureStandard"
                    FlowList.AddFlow(flow)
                    flowManager:StartFlow()
                end
            elseif flowCode == 3 then  --空白校准
                if flowManager:IsFlowEnable() == true then
                    local flow = CalibrateFlow:new({}, CalibrateType.onlyCalibrateBlank)
                    flow.name  = "OnlyCalibrateBlank"
                    FlowList.AddFlow(flow)
                    flowManager:StartFlow()
                end
            elseif flowCode == 4 then  --标液校准
                if flowManager:IsFlowEnable() == true then
                    local flow = CalibrateFlow:new({}, CalibrateType.onlyCalibrateStandard)
                    flow.name  = "OnlyCalibrateStandard"
                    FlowList.AddFlow(flow)
                    flowManager:StartFlow()
                end
            elseif flowCode == 5 then  --零点核查

            elseif flowCode == 6 then  --跨度核查

            elseif flowCode == 7 then  --加标回收 --

            elseif flowCode == 8 then  --平行样 --

            elseif flowCode == 9 then  --空白测试 --
                if flowManager:IsFlowEnable() == true then
                    local flow = MeasureFlow:new({}, MeasureType.Blank)
                    flow.name  = "MeasureBlank"
                    FlowList.AddFlow(flow)
                    flowManager:StartFlow()
                end
            elseif flowCode == 20 then  --清洗 --
                if flowManager:IsFlowEnable() == true then
                    local flow = CleanFlow:new({},cleanType.cleanDeeply)
                    flow.name  = "CleanDeeply"
                    FlowList.AddFlow(flow)
                    flowManager:StartFlow()
                end
            else
                log:debug("无此命令")
            end
        end
        log:debug("HJ212远程控制 Code = ".. flowCode .. " 结果: " .. tostring(ret))
        return ret
    end,

    GetEquipmentMonitoringInfo = function(infoId)
        local value = ""
        for _,v in ipairs(setting.externalInterface.hj212.monitoringInfo) do
            if v.infoId == infoId then
                value = v.read()
                return value
            end
        end
        return value
    end,

    SetEquipmentMonitoringInfo = function(infoId, value)
        local ret = false
        for _,v in ipairs(setting.externalInterface.hj212.monitoringInfo) do
            if v.infoId == infoId then
                ret = v.write(value)
                ConfigLists.SaveMeasureParamConfig()
                return ret
            end
        end
        return ret
    end,

    GetParallelData = function(flowCode)
        local parallelTime = status.measure.report.parallel.dateTime
        local parallelValve = status.measure.report.parallel.consistency
        local parallelMode = status.measure.report.parallel.resultInfo
        local sampleTime = status.measure.newResult.measure.dateTime
        local sampleValve = status.measure.newResult.measure.consistency
        return parallelTime,parallelValve,parallelMode,sampleTime,sampleValve
    end,

    GetRealTimeData = function(flowCode)
        local time = status.measure.newResult.measure.dateTime
        local valve = status.measure.newResult.measure.consistency
        local mode = status.measure.newResult.measure.resultInfo
        local isException = false
        if status.measure.newResult.measure.resultInfo == "T" then
            isException = true
        end
        return time,valve,mode,isException
    end,

    GetZeroCheckData = function(flowCode)
        local time = status.measure.report.zeroCheck.dateTime
        local valve = status.measure.report.zeroCheck.consistency
        local standardValve = config.measureParam.checkConsistency
        local spanValve = 0
        local currentRange = config.measureParam.currentRangeIndex + 1
        if config.system.rangeViewMap[currentRange].view ~= nil then
            spanValve = config.system.rangeViewMap[currentRange].view
        end
        local mode = "K"
        local absorbance = status.measure.report.zeroCheck.absorbance
        return time,valve,standardValve,spanValve,mode,absorbance
    end,

    GetAddStandardData = function(flowCode)
        local cDataTime = status.measure.report.addstandard.dateTime
        local checkValue = status.measure.report.addstandard.consistency
        local waterTime = 0
        local waterValue = 0
        local chroma = 0
        local sVolume = 0
        local dVolume = 0
        local flag = status.measure.report.addstandard.resultInfo
        return cDataTime,checkValue,waterTime,waterValue,chroma,sVolume,dVolume,flag
    end,

    GetSpanCheckData = function(flowCode)
        local cDataTime = status.measure.report.rangeCheck.dateTime
        local checkValue = status.measure.report.rangeCheck.consistency
        local standardValue = 0
        local SpanValue = 0
        local currentRange = config.measureParam.currentRangeIndex + 1
        if config.system.rangeViewMap[currentRange].view ~= nil then
            SpanValue = config.system.rangeViewMap[currentRange].view
        end
        local flag = "K"
        local processValue = status.measure.report.rangeCheck.absorbance
        return cDataTime,checkValue,standardValue,SpanValue,flag,processValue
    end,
}

