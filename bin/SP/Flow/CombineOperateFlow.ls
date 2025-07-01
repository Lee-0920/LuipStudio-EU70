CombineOperateFlow = Flow:new
{
    source = setting.liquidType.none,
    dest = setting.liquidType.none,
    mode = 0,
    sVolume = 0,
    dVolume = 0,
    action = setting.runAction.suckFromBlank,
    text = "",
}

function CombineOperateFlow:new(o, source, dest, mode, sVol, dVol, action)
    o = o or {}
    setmetatable(o, self)
    self.__index = self

    o.source = source
    o.dest = dest
    o.mode = mode
    o.sVolume = sVol
    o.dVolume = dVol
    o.action = action

    return o
end

function CombineOperateFlow:GetRuntime()
    return 0
end

function CombineOperateFlow:OnStart()
    local eventStr = "开始" .. self.text
    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)

    -- 初始化下位机
    dc:GetIDeviceStatus():Initialize()
    lc:GetIDeviceStatus():Initialize()
    --检测消解室是否为安全温度
    --op:CheckDigestSafety()

end

function CombineOperateFlow:OnProcess()

    self.isUserStop = false
    self.isFinish = false

    self.dateTime = os.time()
    local flowManager = FlowManager.Instance()
    local runStatus = Helper.Status.SetStatus(setting.runStatus.combineOperate)
    StatusManager.Instance():SetStatus(runStatus)

    local runAction = Helper.Status.SetAction(self.action)
    StatusManager.Instance():SetAction(runAction)

    local err,result = pcall
    (
            function()
                if self.source ~= setting.liquidType.none then

                    if self.source == setting.liquidType.reagent1 and self.dest == setting.liquidType.none then
                        print("reagent1 reset")
                        op:SyringReset(setting.liquidType.reagent1)
                    elseif self.source == setting.liquidType.reagent2 and self.dest == setting.liquidType.none then
                        print("reagent2 reset")
                        op:SyringReset(setting.liquidType.reagent2)
                    elseif self.source == setting.liquidType.reagent1 and self.dest == setting.liquidType.reagent2 then
                        print("mix liquid")
                        --混合酸剂 1 uL/min
                        op:StartReagentMix(setting.liquidType.reagent1, setting.liquid.syringeMaxVolume, config.measureParam.reagent1Vol) --self.dVolume

                        ----混合氧化剂 1 uL/min
                        op:StartReagentMix(setting.liquidType.reagent2, setting.liquid.syringeMaxVolume, config.measureParam.reagent2Vol) --]]--
                    else
                        log:warn("此模式不支持")
                        return false
                    end
                else
                    op:SyringReset(self.dest)
                    op:ReagentManager(self.dest)
                end
                return true
            end
    )
    if not err then      -- 出现异常
        if type(result) == "table" then
            if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
                self.isUserStop = true
                error(result)
            elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
                self.isUserStop = true
                error(result)
            else
                error(result)
            end
        else
            error(result)
        end
    end
    self.isFinish = true
end

function CombineOperateFlow:OnStop()
    -- 初始化下位机
    --dc:GetIDeviceStatus():Initialize()
    --lc:GetIDeviceStatus():Initialize()

    --保存试剂余量表
    ReagentRemainManager.SaveRemainStatus()

    local eventStr = ""
    if not self.isFinish then
        if self.isUserStop then
            self.result = "用户终止"
            log:info("用户终止")
        else
            self.result = "故障终止"
            log:warn("故障终止")
        end
        eventStr = self.text .. "-" .. self.result
    else
        self.result = "组合操作结束"
        log:info("组合操作结束")
        local str = "组合操作流程总时间 = " .. tostring(os.time() - self.dateTime)
        log:debug(str)
        eventStr = self.result .. "-" .. self.text
    end

    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)
end


--[[
 * @brief 加热
 * @details 加热至目标温度
 * @param[in] targetTemp 目标温度,
--]]
function CombineOperateFlow:Heating(targetTemp)
    local startTime = os.time()

    local isHeating = false
    local thermostatTemp = dc:GetCurrentTemperature():GetThermostatTemp()
    log:debug("加热消解前温度 = " .. thermostatTemp .. " ℃");

    local temp = targetTemp - thermostatTemp
    if temp > 0.01  then
        isHeating = true

        local err,result = pcall(function()
            return op:StartThermostat(ThermostatMode.Auto,
                    targetTemp,
                    setting.temperature.toleranceTemp,
                    setting.temperature.oneKeyHeatingTempTimeout)
        end)
        if not err then -- 出现异常
            if type(result) == "table" then        --Lua异常
                if getmetatable(result) == ThermostatFailedException then
                    ExceptionHandler.MakeAlarm(result)
                elseif getmetatable(result) == ThermostatTimeoutException then
                    ExceptionHandler.MakeAlarm(result)
                else
                    error(result)
                end
            elseif type(result) == "userdata" then --C++异常
                if result:GetType() == "ExpectEventTimeoutException" then
                    ExceptionHandler.MakeAlarm(result)
                else
                    error(result)
                end
            else
                error(result)
            end
        else  -- 正常
            thermostatTemp = result
        end
    end
    log:debug("到达加热目标温度 = " .. thermostatTemp .. "℃")


    if isHeating == true then
        op:StopThermostat()  --停止恒温器
    end

    log:debug("加热时间 = " .. os.time() - startTime);
end

--[[
 * @brief 降温
 * @details 开风扇降温至目标温度
 * @param[in] targetTemp 目标温度,
--]]
function CombineOperateFlow:Cooling(targetTemp)
    local startTime = os.time()

    local thermostatTemp = dc:GetCurrentTemperature():GetThermostatTemp()
    log:debug("降温前温度 = ".. thermostatTemp .."℃")

    -- 冷却
    local difference = thermostatTemp - targetTemp
    if difference > 0.01 then
        local err,result =  pcall(function()
            return op:StartThermostat(ThermostatMode.Auto,
                    targetTemp,
                    setting.temperature.toleranceTemp,
                    setting.temperature.oneKeyHeatingCoolingTempTimeout)
        end)
        if not err then -- 出现异常
            if type(result) == "table" then        --Lua异常
                if getmetatable(result) == ThermostatFailedException then
                    ExceptionHandler.MakeAlarm(result)
                elseif getmetatable(result) == ThermostatTimeoutException then
                    ExceptionHandler.MakeAlarm(result)
                else
                    error(result)
                end
            elseif type(result) == "userdata" then --C++异常
                if result:GetType() == "ExpectEventTimeoutException" then
                    ExceptionHandler.MakeAlarm(result)
                else
                    error(result)
                end
            else
                error(result)
            end
        else -- 正常
            thermostatTemp = result
        end
    end
    log:debug("到达冷却目标温度 = ".. thermostatTemp .."℃")

    log:debug("消解冷却时间 = " .. os.time() - startTime);
end