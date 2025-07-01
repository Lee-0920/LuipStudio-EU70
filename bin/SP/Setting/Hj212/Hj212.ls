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
                local status = 0           -- 0=待机1=测量水样2=维护3=故障4=校准5=标样核查
                local mode = config.interconnection.reportMode
                local statusManager = StatusManager.Instance()
                local runStatus = statusManager:GetStatus()
                local name = runStatus:GetName()
                if mode == ReportMode.Maintain then
                    status = 2
                elseif mode == ReportMode.Fault then
                    status = 3
                elseif mode == ReportMode.Calibrate then
                    status = 4
                else
                    if name == "Idle" then
                        status = 0
                    elseif name == "MeasureSample" then
                        status = 1
                    elseif name == "MeasureRangeCheck" then
                        status = 5
                    end
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
            index = MonitoringInfoIndex.iAlarmCode,
            infoId = "i12003",
            text = "分析仪报警状态",        --0=正常 1=报警
            unit = "",
            infoType = InfoType.Status,
            read = function()
                local infoId = "i12103"
                local status = 0
                for _,v in ipairs(setting.externalInterface.hj212.monitoringInfo) do
                    if v.infoId == infoId then
                        status = v.read()
                        return status
                    end
                end
                return status
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [4] =
        {
            index = MonitoringInfoIndex.iAlarmInfo,
            infoId = "i12103",
            text = "报警详情",        --0=正常 1=缺试剂 2=缺蒸馅水 3=缺标液 4=缺水样 5=加热故障 6=光源异常 7=测量值超上限 8=测量值超下 9=排残液故障 10=釆样故障
            unit = "",
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
                        alarmVal = 4    --缺水样告警
                    elseif cause == "空白水" then
                        alarmVal = 2    --缺蒸馅水
                    elseif cause == "标液" then
                        alarmVal = 3    --缺标液告警
                    elseif cause == "标液二" then
                        alarmVal = 3    --缺标液告警
                    else
                        alarmVal = 1    --缺试剂告警
                    end
                elseif name == "温控故障" then
                    alarmVal = 5 --加热异常
                elseif name == "水样超上限" then
                    alarmVal = 10  --超警报上限
                elseif name == "水样超下限" then
                    alarmVal = 11  --超警报下限
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
            index = MonitoringInfoIndex.iSamplingLastTime,
            infoId = "i13115",
            text = "最近一次采样时间",        --YYYYMMDDHHMMSS
            unit = "",
            infoType = InfoType.Status,
            read = function()
                -- return status.measure.newResult.measure.dateTime
                local modbusManager = ModbusManager.Instance()
                local time = status.measure.newResult.measure.dateTime
                local ret = modbusManager:FormatTime("yyyyMMddhhmmss", time)
                return ret
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [6] =
        {
            index = MonitoringInfoIndex.iRangeValue,
            infoId = "i13001",
            text = "测量量程",
            unit = " ",
            infoType = InfoType.Parameter,
            read = function()
                local rangeMax = 0
                local currentRange = config.measureParam.currentRangeIndex + 1
                if setting.measure.range[currentRange].viewRange ~= nil then
                    rangeMax = setting.measure.range[currentRange].viewRange
                end
                return rangeMax
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [7] =
        {
            index = MonitoringInfoIndex.iMeaAccuracy,
            infoId = "i13002",
            text = "测量精度",
            unit = " ",
            infoType = InfoType.Parameter,
            read = function()
                local decimalNum = setting.measureResult.decimalNum
                return decimalNum
            end,
            write = function(value)
                local ret = false
                --if nil ~= value and type(value) == "string" then
                --    local decimalNum = tonumber(value)
                --    decimalNum = math.floor(decimalNum)
                --    if decimalNum > 0 then
                --        setting.measureResult.decimalNum = decimalNum
                --        ret = true
                --    end
                --end
                return ret
            end,
        },
        [8] =
        {
            index = MonitoringInfoIndex.iMeaInterval,
            infoId = "i13003",
            text = "测量间隔",
            unit = "分钟",
            infoType = InfoType.Parameter,
            read = function()
                local interval = config.scheduler.measure.interval * 60
                return interval
            end,
            write = function(value)
                local ret = false
                if nil ~= value and type(value) == "string" then
                    local interval = tonumber(value)
                    interval = math.floor( interval / 60)
                    if interval > 0 then
                        config.scheduler.measure.interval = interval
                        ret = true
                    end
                end
                return ret
            end,
        },
        [9] =
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
                local ret = true

                return ret
            end,
        },
        [10] =
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
        [11] =
        {
            index = MonitoringInfoIndex.iBlankCalibrationDateTime,
            infoId = "i13006",
            text = "空白校准时间",      --YYYYMMDDHHMMSS
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                -- return status.measure.report.blankCheck.dateTime
				local modbusManager = ModbusManager.Instance()
                local time = status.measure.report.blankCheck.dateTime
                local ret = modbusManager:FormatTime("yyyyMMddhhmmss",  time)
                return ret
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [12] =
        {
            index = MonitoringInfoIndex.iCorrectionFactor,
            infoId = "i13050",
            text = "修正系数",     
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return config.measureParam.reviseFactor
            end,
            write = function(value)
                local ret = false
                if nil ~= value and type(value) == "string" then
                    local factor = tonumber(value)
                    config.measureParam.reviseFactor = factor
                    ret = true
                end
                return ret
            end,
        },
        [13] =
        {
            index = MonitoringInfoIndex.iCorrectionValue,
            infoId = "i13049",
            text = "修正值",     
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return '0'
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [14] =
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
                if nil ~= value and type(value) == "string" then
                    local curveB = tonumber(value)
                    config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].curveB = curveB
                    ret = true
                end
                return ret
            end,
        },
        [15] =
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
                if nil ~= value and type(value) == "string" then
                    local curveK = tonumber(value)
                    config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].curveK = curveK
                    ret = true
                end
                return ret
            end,
        },
        [16] =
        {
            index = MonitoringInfoIndex.iSystemTime,
            infoId = "i13013",
            text = "系统时间",     
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
				local modbusManager = ModbusManager.Instance()
                local time = os.time()
                local ret = modbusManager:FormatTime("yyyyMMddhhmmss",  time)
                return ret
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [17] =
        {
            index = MonitoringInfoIndex.iStdCheckLastTime,
            infoId = "i13114",
            text = "最近一次标液核查时间",      --YYYYMMDDHHMMSS
            unit = "",
            infoType = InfoType.Status,
            read = function()
                -- return status.measure.report.check.dateTime
				local modbusManager = ModbusManager.Instance()
                local time = status.measure.report.check.dateTime
                local ret = modbusManager:FormatTime("yyyyMMddhhmmss",  time)
                return ret
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [18] =
        {
            index = MonitoringInfoIndex.iStdCheckResult,
            infoId = "i12100",
            text = "最近一次标液核查结果",      --0合格 1不合格
            unit = "",
            infoType = InfoType.Status,
            read = function()
                return status.measure.report.check.consistency
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [19] =
        {
            index = MonitoringInfoIndex.iAutoCalibrationInterval,
            infoId = "i13121",
            text = "自动校准时间间隔",      
            unit = "分钟",
            infoType = InfoType.Parameter,
            read = function()
                local time = 60 * config.scheduler.measure.interval
                return time
            end,
            write = function(value)
                local ret = false
                if nil ~= value and type(value) == "string" then
                    local interval = tonumber(value)
                    interval = math.floor( interval / 60)
                    if interval > 0 then
                        config.scheduler.check.interval = interval
                        ret = true
                    end
                end
                return ret
            end,
        },
        [20] =
        {
            index = MonitoringInfoIndex.iStd1AbsorbanceValue,
            infoId = "i13010",
            text = "标1吸光度值",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return status.measure.report.check.absorbance
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [21] =
        {
            index = MonitoringInfoIndex.iZeroAbsorbanceValue,
            infoId = "i13011",
            text = "零点吸光度值",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return status.measure.report.zeroCheck.absorbance
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [22] =
        {
            index = MonitoringInfoIndex.iStd2AbsorbanceValue,
            infoId = "i13012",
            text = "标2吸光度值",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return '0'
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [23] =
        {
            index = MonitoringInfoIndex.iZeroCheckLastTime,
            infoId = "i13109",
            text = "最后一次零点核查时间",      
            unit = "",
            infoType = InfoType.Status,
            read = function()
                -- return status.measure.report.zeroCheck.dateTime
				local modbusManager = ModbusManager.Instance()
                local time = status.measure.report.zeroCheck.dateTime
                local ret = modbusManager:FormatTime("yyyyMMddhhmmss",  time)
                return ret
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [24] =
        {
            index = MonitoringInfoIndex.iMeaAbsorbanceValue,
            infoId = "i13016",
            text = "测量吸光度值",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return status.measure.report.measure.absorbance
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [25] =
        {
            index = MonitoringInfoIndex.iZeroCheckDensity,
            infoId = "i13104",
            text = "零点校准浓度",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return status.measure.report.zeroCheck.consistency
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [26] =
        {
            index = MonitoringInfoIndex.iZeroCheckOriginalValue,
            infoId = "i13105",
            text = "零点校准原始值",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return status.measure.report.zeroCheck.absorbance
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [27] =
        {
            index = MonitoringInfoIndex.iZeroCheckDrift,
            infoId = "i13106",
            text = "零点校准漂移",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return setting.measureResult.zeroDrift
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [28] =
        {
            index = MonitoringInfoIndex.iRangeCheckTime,
            infoId = "i13107",
            text = "量程校准时间",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                -- return status.measure.report.rangeCheck.dateTime
				local modbusManager = ModbusManager.Instance()
                local time = status.measure.report.rangeCheck.dateTime
                local ret = modbusManager:FormatTime("yyyyMMddhhmmss",  time)
                return ret
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [29] =
        {
            index = MonitoringInfoIndex.iRangeCheckDensity,
            infoId = "i13108",
            text = "量程校准浓度",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return status.measure.report.rangeCheck.consistency
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [30] =
        {
            index = MonitoringInfoIndex.iRangeCheckOriginalValue,
            infoId = "i13110",
            text = "量程校准原始值",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return status.measure.report.rangeCheck.absorbance
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [31] =
        {
            index = MonitoringInfoIndex.iRangeCheckDrift,
            infoId = "i13112",
            text = "量程校准漂移",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return setting.measureResult.zeroDrift
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        -- [32] =
        -- {
        --     index = MonitoringInfoIndex.iMeaOriginalValue,
        --     infoId = "i13115",
        --     text = "测量原始值",      
        --     unit = "",
        --     infoType = InfoType.Parameter,
        --     read = function()
        --         return status.measure.report.measure.absorbance
        --     end,
        --     write = function(value)
        --         local ret = false
        --         return ret
        --     end,
        -- },
        [32] =
        {
            index = MonitoringInfoIndex.iCorrectionK,
            infoId = "i13119",
            text = "校正因子（修正斜率）",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                local currentRange = config.measureParam.currentRangeIndex + 1
                local value = config.measureParam.reviseParameter[currentRange]
                if value == nil then
                    value = 0
                end
                return value
            end,
            write = function(value)
                local ret = false
                if nil ~= value and type(value) == "string" then
                    local paramete = tonumber(value)
                    local currentRange = config.measureParam.currentRangeIndex + 1
                    config.measureParam.reviseParameter[currentRange] = paramete
                    ret = true
                end
                return ret
            end,
        },
        [33] =
        {
            index = MonitoringInfoIndex.iCorrectionB,
            infoId = "i13120",
            text = "修正因子（修正截距）",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return config.measureParam.reviseFactor
            end,
            write = function(value)
                local ret = false
                if nil ~= value and type(value) == "string" then
                    local factor = tonumber(value)
                    config.measureParam.reviseFactor = factor
                    ret = true
                end
                return ret
            end,
        },
        [34] =
        {
            index = MonitoringInfoIndex.iStdCheckTime,
            infoId = "i13128",
            text = "标样核査时间",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                -- return status.measure.report.check.dateTime
				local modbusManager = ModbusManager.Instance()
                local time = status.measure.report.check.dateTime
                local ret = modbusManager:FormatTime("yyyyMMddhhmmss",  time)
                return ret
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
        [35] =
        {
            index = MonitoringInfoIndex.iStdCheckDensity,
            infoId = "i13130",
            text = "标样核查标液浓度",      
            unit = "",
            infoType = InfoType.Parameter,
            read = function()
                return status.measure.report.check.consistency
            end,
            write = function(value)
                local ret = false
                return ret
            end,
        },
    },

    InitHj212 = function ()
        log:debug("Init Hj212")

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
        local sampleTime = status.measure.report.measure.dateTime
        local sampleValve = status.measure.report.measure.consistency
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
        local spanValve
        local currentRange = config.measureParam.currentRangeIndex + 1
        if config.system.rangeViewMap[currentRange].view ~= nil then
            spanValve = config.system.rangeViewMap[currentRange].view
        end
        local mode = status.measure.report.zeroCheck.resultInfo
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
        local flag = status.measure.report.rangeCheck.resultInfo
        local processValue = status.measure.report.rangeCheck.absorbance
        return cDataTime,checkValue,standardValue,SpanValue,flag,processValue
    end,
}

