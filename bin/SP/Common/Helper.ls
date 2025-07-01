local _G = _ENV
local print = print
local Alarm = Alarm
local AlarmManager = AlarmManager
local RunStatus = RunStatus
local RunAction = RunAction
local type = type
local pairs = pairs
local Serialization = Serialization
local status = status
local config = config
local scriptsPath = scriptsPath
local ReportMode = ReportMode
local ResultMark = ResultMark
local ModelType = ModelType
local MeasureType = MeasureType
local setting = setting
local RelayControl = RelayControl
local RelayOut = RelayOut
local os = os
local CurrentResultManager = CurrentResultManager
local MeasureResultOutUpLimitException = MeasureResultOutUpLimitException
local ExceptionHandler = ExceptionHandler
local RoleType = RoleType
local App = App
local string = string

local P = {}
Helper = P
_ENV = P


StringSplit = function(str, splitChar)
    local subStrTab = {}
    while (true) do
        local pos = string.find(str, splitChar)
        if (not pos) then
            subStrTab[#subStrTab + 1] = str
            break
        end
        local sub_str = string.sub(str, 1, pos - 1)
        if sub_str ~= "" then
            subStrTab[#subStrTab + 1] = sub_str
        end
        str = string.sub(str, pos + 1, #str)
    end

    return subStrTab
end

GetRoleTypeStr = function()
    local roleType = App:GetCurrentRole()
    local str = ""
    if config.info.instrument.proLicense then
        str = App:GetCurrentUserName()
    else
        if roleType == RoleType.General then
            str = "普通用户"
        elseif roleType == RoleType.Maintain then
            str = "运维员"
        elseif roleType == RoleType.Administrator then
            str = "管理员"
        elseif roleType == RoleType.Super then
            str = "超级管理员"
        end
    end


    return str
end


GetLevelStr = function()
    local str = "--"
    if config.info.instrument.proLicense then
        str = App:GetCurrentLevelName()
    end

    return str
end

MakeAlarm  = function (alarm, cause)
    return Alarm.new(alarm.type, alarm.name, cause, alarm.level)
end

DefaultRestore = function(defaultTable, destinationTable)

    if type(defaultTable) == "table" and type(destinationTable) == "table" then

        for k,v in pairs(defaultTable) do

            if type(v) == "table" then
                DefaultRestore(v, destinationTable[k])
            else
                destinationTable[k] = v
            end

        end
        return true
    else
        return false
    end
end

Status = {}

Status.SetStatus = function(statusTable)

    if type(statusTable.GetTime) == "function" then

        return RunStatus.new(statusTable.name, statusTable.text, statusTable:GetTime())

    else

        return RunStatus.new(statusTable.name, statusTable.text, 0)

    end
end

Status.SetAction = function(actionTable)

    if type(actionTable.GetTime) == "function" then

        return RunAction.new(actionTable.name, actionTable.text, actionTable.GetTime())

    else

        return RunAction.new(actionTable.name, actionTable.text, 0)

    end
end

Result = {}

Result.SetComplexResult = function(resultType, dateTime, consistency, mode, absorbance)
    if resultType == MeasureType.Sample then
        if mode == ReportMode.Calibrate then
            status.measure.report.complexResult.resultInfo = "C" --更新数据标识
        else
            status.measure.report.complexResult.resultInfo = status.measure.report.measure.resultInfo
        end

        status.measure.report.complexResult.dateTime = status.measure.report.measure.dateTime
        status.measure.report.complexResult.consistency = status.measure.report.measure.consistency
        status.measure.report.complexResult.resultType = status.measure.report.measure.resultType
        status.measure.report.complexResult.measureTime = status.measure.report.measure.measureTime
        status.measure.report.complexResult.absorbance = status.measure.report.measure.absorbance
        status.measure.report.complexResult.deviceTypeNum = status.measure.report.measure.deviceTypeNum
        status.measure.report.complexResult.oldTypeNum = status.measure.report.measure.oldTypeNum

    elseif resultType == MeasureType.Standard or resultType == MeasureType.Check or resultType == MeasureType.Blank or
            resultType == MeasureType.ZeroCheck or resultType == MeasureType.RangeCheck or  resultType == MeasureType.Addstandard or
            resultType == MeasureType.ExtAddstandard or resultType == MeasureType.Parallel or resultType == MeasureType.ExtParallel then

        if mode == ReportMode.OnLine  or mode == ReportMode.Calibrate then
            status.measure.report.complexResult.resultInfo = "C" --更新数据标识
        elseif mode == ReportMode.OffLine then
            status.measure.report.complexResult.resultInfo = "B"    --仪器离线
        elseif mode == ReportMode.Maintain or mode == ReportMode.Calibrate then
            status.measure.report.complexResult.resultInfo = "M"    --维护数据
        elseif mode == ReportMode.Fault then
            status.measure.report.complexResult.resultInfo = "D"    --故障数据
        end

        status.measure.report.complexResult.dateTime = dateTime
        status.measure.report.complexResult.consistency = consistency
        status.measure.report.complexResult.resultType = resultType
        status.measure.report.complexResult.measureTime = os.time() - dateTime
        status.measure.report.complexResult.absorbance = absorbance
        status.measure.report.complexResult.deviceTypeNum = setting.instrument.deviceTypeNum.TOC
        status.measure.report.complexResult.oldTypeNum = setting.instrument.oldTypeNum.TOC
    end
end

Result.OnMeasureProformaResultAdded = function(dateTime, consistency, absorbance, measureAD)
    status.measure.proformaResult.measure.dateTime = dateTime
    status.measure.proformaResult.measure.consistency = consistency
    status.measure.proformaResult.measure.absorbance = absorbance
    status.measure.proformaResult.measure.blankReference = measureAD.blankReference
    status.measure.proformaResult.measure.blankMeasure = measureAD.blankMeasure
    status.measure.proformaResult.measure.initReference = measureAD.initReference
    status.measure.proformaResult.measure.initMeasure = measureAD.initMeasure
    status.measure.proformaResult.measure.finalReference = measureAD.finalReference
    status.measure.proformaResult.measure.finalMeasure = measureAD.finalMeasure
end
Result.OnMeasureResultAdded = function(resultType, dateTime, consistency, consistencyTC, consistencyIC, mode, absorbance, isUseStart,isFault,resultMark,modbusStr,modelType)
    local currentResultManager = CurrentResultManager.Instance()

    local measureResultMark = ResultMark.N
    if nil ~= resultMark then
        measureResultMark = resultMark
    end

    local ModbusStr = ""
    if nil ~= modbusStr then
        ModbusStr = modbusStr
    end

    local isReporting = true
    if (resultType == MeasureType.ZeroCheck or resultType == MeasureType.RangeCheck) and false == config.measureParam.checkReporting then
        isReporting = false
    end

    if mode ~= ReportMode.OffLine and isReporting then
        status.measure.newResult.measure.dateTime = dateTime
        status.measure.newResult.measure.consistency = consistency
        status.measure.newResult.measure.consistencyTC = consistencyTC
        status.measure.newResult.measure.consistencyIC = consistencyIC
        status.measure.newResult.measure.resultType = resultType
        status.measure.newResult.measure.absorbance = absorbance
        status.measure.newResult.measure.peak = absorbance
        status.measure.newResult.measure.isUseStart = isUseStart
        status.measure.newResult.measure.isFault = isFault
        status.measure.newResult.measure.errorDateTime = dateTime
        status.measure.newResult.measure.modelType = modelType
    end

    if mode == ReportMode.OnLine then
        if measureResultMark == ResultMark.T or measureResultMark == ResultMark.E then
            status.measure.newResult.measure.resultInfo = "T"        --超仪器上限或者超量程（动态管控仅有T）
        elseif measureResultMark == ResultMark.K then
            status.measure.newResult.measure.resultInfo = "K"
        elseif measureResultMark == ResultMark.C then
            status.measure.newResult.measure.resultInfo = "C"
        else
            status.measure.newResult.measure.resultInfo = "N"        --正常
        end
    elseif mode == ReportMode.OffLine then
        status.measure.newResult.measure.resultInfo = "B"    --仪器离线
    elseif mode == ReportMode.Maintain then
        status.measure.newResult.measure.resultInfo = "M"    --维护数据
    elseif mode == ReportMode.Calibrate then
        status.measure.newResult.measure.resultInfo = "C"    --维护数据
    elseif mode == ReportMode.Fault then
        status.measure.newResult.measure.resultInfo = "D"    --故障数据
    elseif mode == ReportMode.Debugging then
        status.measure.newResult.measure.resultInfo = "A"    --调试数据
    end

    if mode == ReportMode.OnLine then
        if resultType == MeasureType.Blank then
            status.measure.newResult.measure.resultMark = "cz"           --测零点校准液(量程校准液一校准)
        elseif resultType == MeasureType.Standard or resultType == MeasureType.Check then
            status.measure.newResult.measure.resultMark = "cs"           --测量程校准液(量程校准液二校准)
        elseif resultType == MeasureType.ZeroCheck then
            status.measure.newResult.measure.resultMark = "bt"           --零点核查
        elseif resultType == MeasureType.RangeCheck then
            status.measure.newResult.measure.resultMark = "sc"           --量程核查
        elseif resultType == MeasureType.Parallel or resultType == MeasureType.ExtParallel then
            status.measure.newResult.measure.resultMark = "ps"           --平行样测试
        elseif resultType == MeasureType.Addstandard or resultType == MeasureType.ExtAddstandard then
            status.measure.newResult.measure.resultMark = "ra"           --加标回收率测试
        elseif resultType == MeasureType.Sample then
            if consistency > config.interconnection.meaUpLimit  then
                status.measure.newResult.measure.resultMark = "T"        --超仪器上限
            elseif consistency < config.interconnection.meaLowLimit then
                status.measure.newResult.measure.resultMark = "L"        --超仪器下限
            else
                status.measure.newResult.measure.resultMark = "N"        --正常
            end
        end
    elseif mode == ReportMode.OffLine then
        status.measure.newResult.measure.resultMark = "B"    --仪器离线
    elseif mode == ReportMode.Maintain or mode == ReportMode.Calibrate then
        status.measure.newResult.measure.resultMark = "M"    --维护数据
    elseif mode == ReportMode.Fault then
        status.measure.newResult.measure.resultMark = "D"    --故障数据
    end

    if mode ~= ReportMode.OffLine then
        status.measure.report.measure.resultType = resultType
    else
        status.measure.offline.measure.resultType = resultType
    end

    if resultType == MeasureType.Sample then
        status.measure.schedule.autoMeasure.dateTime = dateTime

        if mode ~= ReportMode.OffLine then
            status.measure.report.measure.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.report.measure.dateTime = dateTime
            status.measure.report.measure.consistency = consistency
            status.measure.report.measure.consistencyTC = consistencyTC
            status.measure.report.measure.consistencyIC = consistencyIC
            status.measure.report.measure.resultType = resultType
            status.measure.report.measure.measureTime = os.time() - dateTime
            status.measure.report.measure.absorbance = absorbance
            status.measure.report.measure.deviceTypeNum = setting.instrument.deviceTypeNum.COD
            status.measure.report.measure.oldTypeNum = setting.instrument.oldTypeNum.COD
            currentResultManager:OutputSample(consistency)
            currentResultManager:OutputCheck(consistency)
            if mode == ReportMode.Calibrate and status.measure.report.measure.resultInfo ~= "D" then
                status.measure.report.measure.resultInfo = "C" --更新数据标识
            end
        else
            status.measure.offline.measure.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.offline.measure.dateTime = dateTime
            status.measure.offline.measure.consistency = consistency
            status.measure.offline.measure.consistencyTC = consistencyTC
            status.measure.offline.measure.consistencyIC = consistencyIC
            status.measure.offline.measure.resultType = resultType
            status.measure.offline.measure.measureTime = os.time() - dateTime
            status.measure.offline.measure.absorbance = absorbance
            status.measure.offline.measure.deviceTypeNum = setting.instrument.deviceTypeNum.COD
            status.measure.offline.measure.oldTypeNum = setting.instrument.oldTypeNum.COD
        end
        --elseif resultType == MeasureType.Check then
        --    status.measure.schedule.autoAddstandard.dateTime = dateTime
    elseif resultType == MeasureType.Standard then
        status.measure.schedule.autoCheck.dateTime = dateTime

        if mode ~= ReportMode.OffLine then
            status.measure.report.check.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.report.check.dateTime = dateTime
            status.measure.report.check.consistency = consistency
            status.measure.report.check.absorbance = absorbance
        else
            status.measure.offline.check.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.offline.check.dateTime = dateTime
            status.measure.offline.check.consistency = consistency
            status.measure.offline.check.absorbance = absorbance
        end
    elseif resultType == MeasureType.Blank then
        status.measure.schedule.autoBlankCheck.dateTime = dateTime

        if mode ~= ReportMode.OffLine then
            status.measure.report.blankCheck.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.report.blankCheck.dateTime = dateTime
            status.measure.report.blankCheck.consistency = consistency
            status.measure.report.blankCheck.absorbance = absorbance
        else
            status.measure.offline.blankCheck.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.offline.blankCheck.dateTime = dateTime
            status.measure.offline.blankCheck.consistency = consistency
            status.measure.offline.blankCheck.absorbance = absorbance
        end
    elseif resultType == MeasureType.ZeroCheck then
        status.measure.schedule.zeroCheck.dateTime = dateTime

        if mode ~= ReportMode.OffLine and isReporting then
            status.measure.report.zeroCheck.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.report.zeroCheck.dateTime = dateTime
            status.measure.report.zeroCheck.consistency = consistency
            status.measure.report.zeroCheck.absorbance = absorbance
        else
            status.measure.offline.zeroCheck.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.offline.zeroCheck.dateTime = dateTime
            status.measure.offline.zeroCheck.consistency = consistency
            status.measure.offline.zeroCheck.absorbance = absorbance
        end
    elseif resultType == MeasureType.RangeCheck or resultType == MeasureType.Check then
        status.measure.schedule.rangeCheck.dateTime = dateTime

        if mode ~= ReportMode.OffLine and isReporting then
            status.measure.report.rangeCheck.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.report.rangeCheck.dateTime = dateTime
            status.measure.report.rangeCheck.consistency = consistency
            status.measure.report.rangeCheck.absorbance = absorbance
        else
            status.measure.offline.rangeCheck.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.offline.rangeCheck.dateTime = dateTime
            status.measure.offline.rangeCheck.consistency = consistency
            status.measure.offline.rangeCheck.absorbance = absorbance
        end
    elseif resultType == MeasureType.Addstandard or resultType == MeasureType.ExtAddstandard then
        status.measure.schedule.autoAddstandard.dateTime = dateTime

        if mode ~= ReportMode.OffLine then
            status.measure.report.addstandard.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.report.addstandard.dateTime = dateTime
            status.measure.report.addstandard.consistency = consistency
            status.measure.report.addstandard.absorbance = absorbance
        else
            status.measure.offline.addstandard.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.offline.addstandard.dateTime = dateTime
            status.measure.offline.addstandard.consistency = consistency
            status.measure.offline.addstandard.absorbance = absorbance
        end
    elseif resultType == MeasureType.Parallel or resultType == MeasureType.ExtParallel then
        status.measure.schedule.autoParallel.dateTime = dateTime

        if mode ~= ReportMode.OffLine then
            status.measure.report.parallel.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.report.parallel.dateTime = dateTime
            status.measure.report.parallel.consistency = consistency
            status.measure.report.parallel.absorbance = absorbance
        else
            status.measure.offline.parallel.resultInfo = status.measure.newResult.measure.resultInfo --更新数据标识
            status.measure.offline.parallel.dateTime = dateTime
            status.measure.offline.parallel.consistency = consistency
            status.measure.offline.parallel.absorbance = absorbance
        end
    end

    Result.SetComplexResult(resultType, dateTime, consistency, mode, absorbance) --更新复合寄存器的结果

    if config.interconnection.alarmValue == true then
        if (consistency > 0)  and  measureResultMark == ResultMark.E and (resultType == MeasureType.Sample) then
            local alarm = MakeAlarm(setting.alarm.measureResultOutRange, "")
            AlarmManager.Instance():AddAlarm(alarm)
            if consistency > config.interconnection.meaUpLimit then
                Result.RelayOutOperate(setting.mode.relayOut.upLimit, true)
            end
        elseif consistency > config.interconnection.meaUpLimit and resultType == MeasureType.Sample then
            local alarm = MakeAlarm(setting.alarm.measureResultOutUpLimit, "")
            AlarmManager.Instance():AddAlarm(alarm)
            Result.RelayOutOperate(setting.mode.relayOut.upLimit, true)
        elseif (consistency > 0)  and  (config.interconnection.meaLowLimit - consistency > 0.001) and (resultType == MeasureType.Sample) then
            local alarm = MakeAlarm(setting.alarm.measureResultOutLowLimit, "")
            AlarmManager.Instance():AddAlarm(alarm)
            Result.RelayOutOperate(setting.mode.relayOut.lowLimit, true)
        else
            Result.RelayOutOperate(setting.mode.relayOut.upLimit, false)
            Result.RelayOutOperate(setting.mode.relayOut.lowLimit, false)
        end
    end

end

Result.OnCommonMeasureResultAdded = function(name, resultType, dateTime, consistency, mode)
    --Result.OnMeasureResultAdded(resultType, dateTime, consistency, mode, 0)
end

Result.OnSaveCalibrateConsistencyAdded = function(zeroConsistency, standardConsistency, zeroCheckError, standardCheckError,currentRange)
    if zeroConsistency ~= nil then
        status.measure.calibrate[currentRange].zeroCalculateConsistency = zeroConsistency
        status.measure.calibrate[currentRange].zeroCheckError = zeroCheckError
        status.measure.newResult.calibrate.zeroCalculateConsistency = zeroConsistency
        status.measure.newResult.calibrate.zeroCheckError = zeroCheckError
    end
    if standardConsistency ~= nil then
        status.measure.calibrate[currentRange].standardCalculateConsistency = standardConsistency
        status.measure.calibrate[currentRange].standardCheckError = standardCheckError
        status.measure.newResult.calibrate.standardCalculateConsistency = standardConsistency
        status.measure.newResult.calibrate.standardCheckError = standardCheckError
    end
end

Result.OnCalibrateResultAdded = function(dateTime, zeroDateTime, standardDateTime,curveK, curveB, point0Consistency, point1Consistency, point0Absorbance, point1Absorbance, currentRange, userSettings)

    if userSettings == false then
        config.measureParam.curveParam[currentRange].curveK = curveK
        config.measureParam.curveParam[currentRange].curveB = curveB
        config.modifyRecord.measureParam(true)
    end

    status.measure.schedule.autoCalibrate.dateTime = dateTime
    status.measure.newResult.calibrate.dateTime = dateTime
    status.measure.newResult.calibrate.zeroCalibrateDateTime = zeroDateTime
    status.measure.newResult.calibrate.standardCalibrateDateTime = standardDateTime
    status.measure.newResult.calibrate.curveK = curveK
    status.measure.newResult.calibrate.curveB = curveB
    status.measure.newResult.calibrate.point0Consistency = point0Consistency
    status.measure.newResult.calibrate.point1Consistency = point1Consistency
    status.measure.newResult.calibrate.point0Absorbance = point0Absorbance
    status.measure.newResult.calibrate.point1Absorbance = point1Absorbance

    if status.measure.calibrate[currentRange].dateTime ~= nil then
        status.measure.calibrate[currentRange].dateTime = dateTime
        status.measure.calibrate[currentRange].zeroCalibrateDateTime = zeroDateTime
        status.measure.calibrate[currentRange].standardCalibrateDateTime = standardDateTime
        status.measure.calibrate[currentRange].curveK = curveK
        status.measure.calibrate[currentRange].curveB = curveB
        status.measure.calibrate[currentRange].point0Consistency = point0Consistency
        status.measure.calibrate[currentRange].point1Consistency = point1Consistency
        status.measure.calibrate[currentRange].point0Absorbance = point0Absorbance
        status.measure.calibrate[currentRange].point1Absorbance = point1Absorbance
        status.measure.calibrate[currentRange].userSettings = userSettings
    end
end

Result.RelayOutOperate = function(operate,  status)
    local relayOne = config.interconnection.relayOne
    local relayTwo = config.interconnection.relayTwo
    local relayControl = RelayControl.Instance()

    if  operate == setting.mode.relayOut.lowLimit then
        if relayOne == setting.mode.relayOut.lowLimit then
            if status == true then
                relayControl:TurnOn(2)
            else
                relayControl:TurnOff(2)
            end
        end

        if relayTwo == setting.mode.relayOut.lowLimit then
            if status == true then
                relayControl:TurnOn(3)
            else
                relayControl:TurnOff(3)
            end
        end
    elseif operate == setting.mode.relayOut.upLimit then
        if relayOne == setting.mode.relayOut.upLimit then
            if status == true then
                relayControl:TurnOn(2)
            else
                relayControl:TurnOff(2)
            end
        end

        if relayTwo == setting.mode.relayOut.upLimit then
            if status == true then
                relayControl:TurnOn(3)
            else
                relayControl:TurnOff(3)
            end
        end
    elseif operate == setting.mode.relayOut.measureInstruct then
        if relayOne == setting.mode.relayOut.measureInstruct then
            if status == true then
                relayControl:TurnOn(2)
            else
                relayControl:TurnOff(2)
            end
        end

        if relayTwo == setting.mode.relayOut.measureInstruct then
            if status == true then
                relayControl:TurnOn(3)
            else
                relayControl:TurnOff(3)
            end
        end
    elseif operate == setting.mode.relayOut.calibrateInstruct then
        if relayOne == setting.mode.relayOut.calibrateInstruct then
            if status == true then
                relayControl:TurnOn(2)
            else
                relayControl:TurnOff(2)
            end
        end

        if relayTwo == setting.mode.relayOut.calibrateInstruct then
            if status == true then
                relayControl:TurnOn(3)
            else
                relayControl:TurnOff(3)
            end
        end
    elseif operate == setting.mode.relayOut.cleanInstruct then
        if relayOne == setting.mode.relayOut.cleanInstruct then
            if status == true then
                relayControl:TurnOn(2)
            else
                relayControl:TurnOff(2)
            end
        end

        if relayTwo == setting.mode.relayOut.cleanInstruct then
            if status == true then
                relayControl:TurnOn(3)
            else
                relayControl:TurnOff(3)
            end
        end

    elseif operate == setting.mode.relayOut.collectInstruct then
        if relayOne == setting.mode.relayOut.collectInstruct then
            if status == true then
                relayControl:TurnOn(2)
            else
                relayControl:TurnOff(2)
            end
        end

        if relayTwo == setting.mode.relayOut.collectInstruct then
            if status == true then
                relayControl:TurnOn(3)
            else
                relayControl:TurnOff(3)
            end
        end

    elseif operate == setting.mode.relayOut.checkInstruct then
        if relayOne == setting.mode.relayOut.checkInstruct then
            if status == true then
                relayControl:TurnOn(2)
            else
                relayControl:TurnOff(2)
            end
        end

        if relayTwo == setting.mode.relayOut.checkInstruct then
            if status == true then
                relayControl:TurnOn(3)
            else
                relayControl:TurnOff(3)
            end
        end
    end
end

return Helper
