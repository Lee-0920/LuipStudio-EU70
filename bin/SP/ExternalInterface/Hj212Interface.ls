
local _G = _ENV
local type = type
local pairs = pairs
local string = string
local tostring = tostring
local table = table
local error = error
local assert = assert
local print = print
local pcall = pcall
local log = log
local setting = setting

local P = {}
Hj212Interface = P
_ENV = P

function GetParallelData(flowCode)
    local parallelTime = 0
    local parallelValve = 0
    local parallelMode = ""
    local sampleTime = 0
    local sampleValve = 0
    if nil ~= setting.externalInterface.hj212.GetParallelData then
        parallelTime,parallelValve,parallelMode,sampleTime,sampleValve = setting.externalInterface.hj212.GetParallelData(flowCode)
    end
    return parallelTime,parallelValve,parallelMode,sampleTime,sampleValve
end

function GetRealTimeData(flowCode)
    local time = 0
    local valve = 0
    local mode = ""
    local isException = 0
    if nil ~= setting.externalInterface.hj212.GetRealTimeData then
        time,valve,mode,isException = setting.externalInterface.hj212.GetRealTimeData(flowCode)
    end
    return time,valve,mode,isException
end

function GetZeroCheckData(flowCode)
    local time = 0
    local valve = 0
    local standardValve = 0
    local spanValve = 0
    local mode = ""
    local absorbance = 0
    if nil ~= setting.externalInterface.hj212.GetZeroCheckData then
        time,valve,standardValve,spanValve,mode,absorbance = setting.externalInterface.hj212.GetZeroCheckData(flowCode)
    end
    return time,valve,standardValve,spanValve,mode,absorbance
end

function GetAddStandardData(flowCode)
    local cDataTime = 0
    local checkValue = 0
    local waterTime = 0
    local waterValue = 0
    local chroma = 0
    local sVolume = 0
    local dVolume = 0
    local flag = ""
    if nil ~= setting.externalInterface.hj212.GetAddStandardData then
        cDataTime,checkValue,waterTime,waterValue,chroma,sVolume,dVolume,flag = setting.externalInterface.hj212.GetAddStandardData(flowCode)
    end
    return cDataTime,checkValue,waterTime,waterValue,chroma,sVolume,dVolume,flag
end

function GetSpanCheckData(flowCode)
    local cDataTime = 0
    local checkValue = 0
    local standardValue = 0
    local SpanValue = 0
    local flag = ""
    local processValue = 0
    if nil ~= setting.externalInterface.hj212.GetSpanCheckData then
        cDataTime,checkValue,standardValue,SpanValue,flag,processValue= setting.externalInterface.hj212.GetSpanCheckData(flowCode)
    end
    return cDataTime,checkValue,standardValue,SpanValue,flag,processValue
end

function StartEquipmentFlow(flowCode)
    local ret = false
    if nil ~= setting.externalInterface.hj212.StartEquipmentFlow then
        if nil ~= flowCode and type(flowCode) == "number" then
            ret = setting.externalInterface.hj212.StartEquipmentFlow(flowCode)
        end
    end
    return ret
end

function GetEquipmentMonitoringInfo(infoId)
    local value = ""
    if nil ~= setting.externalInterface.hj212.GetEquipmentMonitoringInfo then
        if nil ~= infoId and type(infoId) == "string" then
            value = setting.externalInterface.hj212.GetEquipmentMonitoringInfo(infoId)
        end
    end
    return value
end

function SetEquipmentMonitoringInfo(infoId, value)
    local ret = false
    if nil ~= setting.externalInterface.hj212.SetEquipmentMonitoringInfo then
        if nil ~= infoId and nil ~= value and type(infoId) == "string" and type(value) == "string" then
            ret = setting.externalInterface.hj212.SetEquipmentMonitoringInfo(infoId, value)
        end
    end
    return ret
end

return Hj212Interface