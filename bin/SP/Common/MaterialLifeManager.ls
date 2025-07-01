local _G = _ENV

local os = os
local pcall = pcall
local type = type
local log = log
local setting = setting
local config = config
local ExceptionHandler = ExceptionHandler
local MaterialGoBadException = MaterialGoBadException
local SaveToAuditTrailSqlite = SaveToAuditTrailSqlite

local P = {}
MaterialLifeManager = P
_ENV = P

local ispumpGoBad = false
local isuvLampGoBad = false
local isresinGoBad = false


function Reset(material)
    local lastTime

    if material == setting.materialType.pump then
        ispumpGoBad = false
        lastTime = os.date("*t", config.consumable.pump.lastTime)
        lastTime.month = lastTime.month + config.consumable.pump.cycle
    end
    if material == setting.materialType.uvLamp then
        isuvLampGoBad = false
        lastTime = os.date("*t", config.consumable.uvLamp.lastTime)
        lastTime.month = lastTime.month + config.consumable.uvLamp.cycle
    end
    if material == setting.materialType.resin then
        isresinGoBad = false
        lastTime = os.date("*t", config.consumable.resin.lastTime)
        lastTime.month = lastTime.month + config.consumable.resin.cycle
    end

    local lastTimeStr = os.date("%Y-%m-%d %H:%M:%S", os.time(lastTime))
    local eventStr = "耗材管理-更换" .. material.text
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, lastTimeStr, nil)
end

local function Check(material)

   local currentTime = os.time()

    if material == setting.materialType.pump then
        local temp = os.date("*t", config.consumable.pump.lastTime)
        temp.month = temp.month + config.consumable.pump.cycle

        local materialGoBadTime = currentTime
        local err,result = pcall(function() return os.time(temp) end)
        if not err then
            if type(result) == "string" then
                log:warn(result)
            else
                log:warn("MaterialLifeManager ==> Check(material) Error.")
            end
        else
            materialGoBadTime = result
            if materialGoBadTime - currentTime < 0 then
                ExceptionHandler.MakeAlarm(MaterialGoBadException:new({materialType = setting.materialType.pump,}))
                ispumpGoBad = true
            end
        end

    end

    if material == setting.materialType.uvLamp then
        local temp = os.date("*t", config.consumable.uvLamp.lastTime)
        temp.month = temp.month + config.consumable.uvLamp.cycle

        local materialGoBadTime = currentTime
        local err,result = pcall(function() return os.time(temp) end)
        if not err then
            if type(result) == "string" then
                log:warn(result)
            else
                log:warn("MaterialLifeManager ==> Check(material) Error.")
            end
        else
            materialGoBadTime = result
            if materialGoBadTime - currentTime < 0 then
                ExceptionHandler.MakeAlarm(MaterialGoBadException:new({materialType = setting.materialType.uvLamp,}))
                isuvLampGoBad = true
            end
        end

    end

    if material == setting.materialType.resin then
        local temp = os.date("*t", config.consumable.resin.lastTime)
        temp.month = temp.month + config.consumable.resin.cycle

        local materialGoBadTime = currentTime
        local err,result = pcall(function() return os.time(temp) end)
        if not err then
            if type(result) == "string" then
                log:warn(result)
            else
                log:warn("MaterialLifeManager ==> Check(material) Error.")
            end
        else
            materialGoBadTime = result
            if materialGoBadTime - currentTime < 0 then
                ExceptionHandler.MakeAlarm(MaterialGoBadException:new({materialType = setting.materialType.resin,}))
                isresinGoBad = true
            end
        end

    end
end

function CheckAllMaterialLife()

    if not ispumpGoBad then
        Check(setting.materialType.pump)
    end
    if not isuvLampGoBad then
        Check(setting.materialType.uvLamp)
    end
    if not isresinGoBad then
        Check(setting.materialType.resin)
    end

    log:debug("check")

end

function RecoverMaterialLifeCheckStatus()
    ispumpGoBad = false
    isuvLampGoBad = false
    isresinGoBad = false
end

return MaterialLifeManager
