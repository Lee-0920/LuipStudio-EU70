local _G = _ENV
local setting = setting
local status = status
local config = config
local Serialization = Serialization
local scriptsPath = scriptsPath
local ReagentRemainWarn = ReagentRemainWarn
local ExceptionHandler = ExceptionHandler
local ConfigLists = ConfigLists
local print = print
local os = os
local pcall = pcall
local type = type
local tostring = tostring
local log = log
local ReagentRemainVector = ReagentRemainVector
local ReagentRemain = ReagentRemain
local WqimcManager = WqimcManager
local SaveToAuditTrailSqlite = SaveToAuditTrailSqlite
local P = {}
ReagentRemainManager = P
_ENV = P

local isBlankEnough = true       -- 零点校准液充足
local isStandardEnough = true   -- 量程校准液充足
local isZeroCheckEnough = true   -- 零点核查液充足
local isRangeCheckEnough = true   -- 量程核查液充足
local isWasteEnough = true       -- 分析废液充足
local isWasteWaterEnough = true  -- 清洗废水充足
local isReagent1Enough = true    -- 试剂一充足
local isReagent2Enough = true   -- 试剂二充足

local isBlankBad = false
local isStandardBad = false
local isZeroCheckBad = false
local isRangeCheckBad = false
local isWasteBad = false
local isWasteWaterBad = false
local isReagent1Bad = false
local isReagent2Bad = false


local REMAIN = 20.000000
local PRECISE = 0.000001

function ChangeReagent(reagent, vol)

    if reagent == setting.liquidType.reagent1 then
        status.remain.reagent1 = vol
        ConfigLists.SaveRemainStatus()
        isReagent1Enough = true
        isReagent1Bad = false

        local lastTime = os.date("*t", config.remain.reagent1.lastTime)
        lastTime.month = lastTime.month + config.remain.reagent1.cycle
        local lastTimeStr = os.date("%Y-%m-%d %H:%M:%S", os.time(lastTime))
        local eventStr = "试剂管理-更换酸剂-" .. tostring(vol) .. "ml"
        SaveToAuditTrailSqlite(nil, nil, eventStr, nil, lastTimeStr, nil)
    elseif reagent == setting.liquidType.reagent2 then
        status.remain.reagent2 = vol
        ConfigLists.SaveRemainStatus()
        isReagent2Enough = true
        isReagent2Bad = false

        local lastTime = os.date("*t", config.remain.reagent2.lastTime)
        lastTime.month = lastTime.month + config.remain.reagent2.cycle
        local lastTimeStr = os.date("%Y-%m-%d %H:%M:%S", os.time(lastTime))
        local eventStr = "试剂管理-更换氧化剂-" .. tostring(vol) .. "ml"
        SaveToAuditTrailSqlite(nil, nil, eventStr, nil, lastTimeStr, nil)
    end
end

local function Check(reagent)

    local currentTime = os.time()

    if reagent == setting.liquidType.blank then
        local temp = os.date("*t", config.remain.blank.lastTime)
        temp.month = temp.month + config.remain.blank.cycle

        local reagentGoBadTime = currentTime
        local err,result = pcall(function() return os.time(temp) end)
        if not err then  --出现异常
            if type(result) == "string" then
                log:warn(result)
            else
                log:warn("ReagentRemainManager ==> Check(reagent) Error.")
            end
        else
            reagentGoBadTime = result
            if reagentGoBadTime - currentTime < 0 then
                local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.blank,})
                ExceptionHandler.MakeAlarm(exception)
                isBlankBad = true
            end
        end

    elseif reagent == setting.liquidType.standard then
        local temp = os.date("*t", config.remain.standard.lastTime)
        temp.month = temp.month + config.remain.standard.cycle

        local reagentGoBadTime = currentTime
        local err,result = pcall(function() return os.time(temp) end)
        if not err then  --出现异常
            if type(result) == "string" then
                log:warn(result)
            else
                log:warn("ReagentRemainManager ==> Check(reagent) Error.")
            end
        else
            reagentGoBadTime = result
            if reagentGoBadTime - currentTime < 0 then
                local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.standard,})
                ExceptionHandler.MakeAlarm(exception)
                isStandardBad = true
            end
        end

    elseif reagent == setting.liquidType.zeroCheck then
        local temp = os.date("*t", config.remain.zeroCheck.lastTime)
        temp.month = temp.month + config.remain.zeroCheck.cycle

        local reagentGoBadTime = currentTime
        local err,result = pcall(function() return os.time(temp) end)
        if not err then  --出现异常
            if type(result) == "string" then
                log:warn(result)
            else
                log:warn("ReagentRemainManager ==> Check(reagent) Error.")
            end
        else
            reagentGoBadTime = result
            if reagentGoBadTime - currentTime < 0 then
                local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.zeroCheck,})
                ExceptionHandler.MakeAlarm(exception)
                isZeroCheckBad = true
            end
        end

    elseif reagent == setting.liquidType.rangeCheck then
        local temp = os.date("*t", config.remain.rangeCheck.lastTime)
        temp.month = temp.month + config.remain.rangeCheck.cycle

        local reagentGoBadTime = currentTime
        local err,result = pcall(function() return os.time(temp) end)
        if not err then  --出现异常
            if type(result) == "string" then
                log:warn(result)
            else
                log:warn("ReagentRemainManager ==> Check(reagent) Error.")
            end
        else
            reagentGoBadTime = result
            if reagentGoBadTime - currentTime < 0 then
                local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.rangeCheck,})
                ExceptionHandler.MakeAlarm(exception)
                isRangeCheckBad = true
            end
        end

    elseif reagent == setting.liquidType.waste then
        local temp = os.date("*t", config.remain.waste.lastTime)
        temp.month = temp.month + config.remain.waste.cycle

        local reagentGoBadTime = currentTime
        local err,result = pcall(function() return os.time(temp) end)
        if not err then  --出现异常
            if type(result) == "string" then
                log:warn(result)
            else
                log:warn("ReagentRemainManager ==> Check(reagent) Error.")
            end
        else
            reagentGoBadTime = result
            if reagentGoBadTime - currentTime < 0 then
                local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.waste,})
                ExceptionHandler.MakeAlarm(exception)
                isWasteBad = true
            end
        end

    elseif reagent == setting.liquidType.wasteWater then
        local temp = os.date("*t", config.remain.wasteWater.lastTime)
        temp.month = temp.month + config.remain.wasteWater.cycle

        local reagentGoBadTime = currentTime
        local err,result = pcall(function() return os.time(temp) end)
        if not err then  --出现异常
            if type(result) == "string" then
                log:warn(result)
            else
                log:warn("ReagentRemainManager ==> Check(reagent) Error.")
            end
        else
            reagentGoBadTime = result
            if reagentGoBadTime - currentTime < 0 then
                local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.wasteWater,})
                ExceptionHandler.MakeAlarm(exception)
                isWasteWaterBad = true
            end
        end

    elseif reagent == setting.liquidType.reagent1 then
        local temp = os.date("*t", config.remain.reagent1.lastTime)
        temp.month = temp.month + config.remain.reagent1.cycle

        local reagentGoBadTime = currentTime
        local err,result = pcall(function() return os.time(temp) end)
        if not err then  --出现异常
            if type(result) == "string" then
                log:warn(result)
            else
                log:warn("ReagentRemainManager ==> Check(reagent) Error.")
            end
        else
            reagentGoBadTime = result
            if reagentGoBadTime - currentTime < 0 then
                local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.reagent1,})
                ExceptionHandler.MakeAlarm(exception)
                isReagent1Bad = true
            end
        end
    end

end

function CheckAllReagentDate()

    if config.system.reagentLackWarn == true then

        if not isReagent1Bad and isReagent1Enough then

            Check(setting.liquidType.reagent1)
        end

        if not isReagent2Bad and isReagent2Enough then

            Check(setting.liquidType.reagent2)
        end
    end
end

local function ReagentRemainCheck()

    if config.system.reagentLackWarn == true then
        if (REMAIN - status.remain.blank >= PRECISE) and isBlankEnough and not isBlankBad then
            isBlankEnough = false
            local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.blank,})
            ExceptionHandler.MakeAlarm(exception)
        end

        if (REMAIN - status.remain.standard >= PRECISE) and isStandardEnough and not isStandardBad then
            isStandardEnough = false
            local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.standard,})
            ExceptionHandler.MakeAlarm(exception)
        end

        if (REMAIN - status.remain.zeroCheck >= PRECISE) and isZeroCheckEnough and not isZeroCheckBad then
            isZeroCheckEnough = false
            local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.zeroCheck,})
            ExceptionHandler.MakeAlarm(exception)
        end

        if (REMAIN - status.remain.rangeCheck >= PRECISE) and isRangeCheckEnough and not isRangeCheckBad then
            isRangeCheckEnough = false
            local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.rangeCheck,})
            ExceptionHandler.MakeAlarm(exception)
        end

        if (REMAIN - status.remain.waste >= PRECISE) and isWasteEnough and not isWasteBad then
            isWasteEnough = false
            local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.waste,})
            ExceptionHandler.MakeAlarm(exception)
        end

        if (REMAIN - status.remain.wasteWater >= PRECISE) and isWasteWaterEnough and not isWasteWaterBad then
            isWasteWaterEnough = false
            local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.wasteWater,})
            ExceptionHandler.MakeAlarm(exception)
        end

        if (REMAIN - status.remain.reagent1 >= PRECISE) and isReagent1Enough and not isReagent1Bad then
            isReagent1Enough = false
            local exception = ReagentRemainWarn:new({liquidType = setting.liquidType.reagent1,})
            ExceptionHandler.MakeAlarm(exception)
        end

    end
end

function ReduceReagent(reagent, vol)

    local multipleValvePipeVolume = setting.liquid.multipleValvePipeVolume

    if reagent == setting.liquidType.blank then
        status.remain.blank = status.remain.blank - (vol + multipleValvePipeVolume)
        status.remain.waste = status.remain.waste - (vol + multipleValvePipeVolume)/2
        status.remain.wasteWater = status.remain.wasteWater - (vol + multipleValvePipeVolume)/2

        if status.remain.blank < 0 then status.remain.blank = 0 end
        if status.remain.blank > config.remain.blank.total then status.remain.blank = config.remain.blank.total end
    elseif reagent == setting.liquidType.standard then
        status.remain.standard = status.remain.standard - (vol + multipleValvePipeVolume)
        status.remain.waste = status.remain.waste - (vol + multipleValvePipeVolume)

        if status.remain.standard < 0 then status.remain.standard = 0 end
        if status.remain.standard > config.remain.standard.total then status.remain.standard = config.remain.standard.total end
    elseif reagent == setting.liquidType.zeroCheck then
        status.remain.zeroCheck = status.remain.zeroCheck - (vol + multipleValvePipeVolume)
        status.remain.waste = status.remain.waste - (vol + multipleValvePipeVolume)

        if status.remain.zeroCheck < 0 then status.remain.zeroCheck = 0 end
        if status.remain.zeroCheck > config.remain.zeroCheck.total then status.remain.zeroCheck = config.remain.zeroCheck.total end
    elseif reagent == setting.liquidType.rangeCheck then
        status.remain.rangeCheck = status.remain.rangeCheck - (vol + multipleValvePipeVolume)
        status.remain.waste = status.remain.waste - (vol + multipleValvePipeVolume)

        if status.remain.rangeCheck < 0 then status.remain.rangeCheck = 0 end
        if status.remain.rangeCheck > config.remain.rangeCheck.total then status.remain.rangeCheck = config.remain.rangeCheck.total end
    elseif reagent == setting.liquidType.sample then
        status.remain.waste = status.remain.waste - (vol + multipleValvePipeVolume)

        if status.remain.waste < 0 then status.remain.waste = 0 end
        if status.remain.waste > config.remain.waste.total then status.remain.waste = config.remain.waste.total end
    elseif reagent == setting.liquidType.reagent1 then
        status.remain.reagent1 = status.remain.reagent1 - (vol + multipleValvePipeVolume)
        status.remain.waste = status.remain.waste - (vol + multipleValvePipeVolume)

        if status.remain.reagent1 < 0 then status.remain.reagent1 = 0 end
        if status.remain.reagent1 > config.remain.reagent1.total then status.remain.reagent1 = config.remain.reagent1.total end
    end

    if status.remain.waste < 0 then status.remain.waste = 0 end
    if status.remain.waste > config.remain.waste.total then status.remain.waste = config.remain.waste.total end
    if status.remain.wasteWater < 0 then status.remain.wasteWater = 0 end
    if status.remain.wasteWater > config.remain.wasteWater.total then status.remain.wasteWater = config.remain.wasteWater.total end
    ReagentRemainCheck()
end

function RecoverReagent(reagent, vol)

    local multipleValvePipeVolume = setting.liquid.multipleValvePipeVolume

    if reagent == setting.liquidType.blank then
        status.remain.blank = status.remain.blank + (vol + multipleValvePipeVolume)
        status.remain.waste = status.remain.waste + (vol + multipleValvePipeVolume)

        if status.remain.blank < 0 then status.remain.blank = 0 end
        if status.remain.blank > config.remain.blank.total then status.remain.blank = config.remain.blank.total end
    elseif reagent == setting.liquidType.standard then
        status.remain.standard = status.remain.standard + (vol + multipleValvePipeVolume)
        status.remain.waste = status.remain.waste + (vol + multipleValvePipeVolume)

        if status.remain.standard < 0 then status.remain.standard = 0 end
        if status.remain.standard > config.remain.standard.total then status.remain.standard = config.remain.standard.total end
    elseif reagent == setting.liquidType.zeroCheck then
        status.remain.zeroCheck = status.remain.zeroCheck + (vol + multipleValvePipeVolume)
        status.remain.waste = status.remain.waste + (vol + multipleValvePipeVolume)

        if status.remain.zeroCheck < 0 then status.remain.zeroCheck = 0 end
        if status.remain.zeroCheck > config.remain.zeroCheck.total then status.remain.zeroCheck = config.remain.zeroCheck.total end
    elseif reagent == setting.liquidType.rangeCheck then
        status.remain.rangeCheck = status.remain.rangeCheck + (vol + multipleValvePipeVolume)
        status.remain.waste = status.remain.waste + (vol + multipleValvePipeVolume)

        if status.remain.rangeCheck < 0 then status.remain.rangeCheck = 0 end
        if status.remain.rangeCheck > config.remain.rangeCheck.total then status.remain.rangeCheck = config.remain.rangeCheck.total end
    elseif reagent == setting.liquidType.sample then
        status.remain.waste = status.remain.waste + (vol + multipleValvePipeVolume)

    elseif reagent == setting.liquidType.reagent1 then
        status.remain.reagent1 = status.remain.reagent1 + (vol + multipleValvePipeVolume)
        status.remain.waste = status.remain.waste + (vol + multipleValvePipeVolume)

        if status.remain.reagent1 < 0 then status.remain.reagent1 = 0 end
        if status.remain.reagent1 > config.remain.reagent1.total then status.remain.reagent1 = config.remain.reagent1.total end
    end
    if status.remain.waste < 0 then status.remain.waste = 0 end
    if status.remain.waste > config.remain.waste.total then status.remain.waste = config.remain.waste.total end
    if status.remain.wasteWater < 0 then status.remain.wasteWater = 0 end
    if status.remain.wasteWater > config.remain.wasteWater.total then status.remain.wasteWater = config.remain.wasteWater.total end
    ReagentRemainCheck()
end

function RecoverRemainCheckStatus()
    isBlankEnough = true       -- 零点校准液充足
    isStandardEnough = true   -- 量程校准液充足
    isWasteEnough = true       -- 分析废液充足
    isWasteWaterEnough = true  --清洗废水充足
    isZeroCheckEnough = true   -- 零点核查液充足
    isRangeCheckEnough = true   -- 量程核查液充足
    isReagent1Enough = true    -- 试剂一充足
    isReagent2Enough = true   -- 试剂二充足

    isBlankBad = false
    isStandardBad = false
    isWasteBad = false
    isWasteWaterBad = false
    isZeroCheckBad = false
    isRangeCheckBad = false
    isReagent1Bad = false
    isReagent2Bad = false
end

function SaveRemainStatus()
    ConfigLists.SaveRemainStatus()

    local reagentRemain = ReagentRemain.new()
    local reagentRemainVector = ReagentRemainVector.new()

    reagentRemain:__set_name("reagent1")
    reagentRemain:__set_volume(status.remain.reagent1)
    reagentRemainVector:Push(reagentRemain)

    reagentRemain:__set_name("reagent2")
    reagentRemain:__set_volume(status.remain.reagent2)
    reagentRemainVector:Push(reagentRemain)

    if reagentRemainVector:Size() > 0 then
        WqimcManager.Instance():updateReagentRemain(reagentRemainVector)
    end

end

return ReagentRemainManager
