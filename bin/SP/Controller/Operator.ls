--[[
 * @brief 各个驱动器操作业务。
 * @details 对液路器、温控器、信号采集器业务操作的功能进行封装。
--]]
Operator =
{
    ISolenoidValve = 0,
    IPeristalticPump = 0,
    ITemperatureControl = 0,
    IExtTemperatureControl = 0,
    IOpticalAcquire = 0,
    ILCPeristalticPump = 0,
    ILCSolenoidValve = 0,
    ILCTemperatureControl = 0,
    IRCTemperatureControl = 0,
    IOCTemperatureControl = 0,
    IOCOutputControl = 0,
    isMetering = false,
    isThermostat = false,
    isAcquiring = false,
    SPEED = 0.2,
    PRECISE = 0.000001,
}

--[[
 * @brief 废液排放类型。(分析废液 = 1，清洗废水 = 2)
--]]
WasteType =
{
    WasteReagent = 1,
    WasteWater = 2,
}

--[[
 * @brief 管路更新类型。(普通模式 = 1，精准模式第一次 = 2，，精准模式最后一次 = 3)
--]]
FillType =
{
    FillNormal = 1,
    FillAccurateFirst = 2,
    FillAccurateSecond = 3,
    FillAccurateEnd = 4,
}

AccurateType =
{
    normal= 0, --正常校准
    normalWithoutClean= 1, --精准校准不清洗
    addSampleWithClean = 2,--加样及清洗
    onlyAddSample = 3,--仅加样且不清洗
}

function Operator:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    self.__metatable = "Operator"

    self.ISolenoidValve = dc:GetISolenoidValve()
    self.IPeristalticPump = dc:GetIPeristalticPump()
    self.ITemperatureControl = dc:GetITemperatureControl()
    self.IExtTemperatureControl = dc:GetIExtTemperatureControl()
    self.IOpticalAcquire = dc:GetIOpticalAcquire()
    self.ILCSolenoidValve = lc:GetISolenoidValve()
    self.ILCPeristalticPump = lc:GetIPeristalticPump()
    self.ILCTemperatureControl = lc:GetITemperatureControl()
    self.IRCTemperatureControl = rc:GetITemperatureControl()
    --self.IOCTemperatureControl = oc:GetITemperatureControl()
    --self.IOCOutputControl = oc:GetIOutputControl()
    return o
end

--[[
 * @brief 停止所有驱动控制器操作。
--]]
function Operator:Stop()

   -- 停止泵
    for i,pump in pairs(pumps) do
        if pump.isRunning then
            pump:Stop()
        end
    end

	-- 停止信号采集
    if self.isAcquiring then
        self.IOpticalAcquire:StopAcquirer()
        self.isAcquiring= false
    end

end

--[[
 * @brief 泵抽取液体操作。
 * @param[in] source 管道类型。
 * @param[in] volume 溶液体积。
  * @param[in] speed 泵速，0为默认速度。
--]]
function Operator:Pump(source, volume, speed)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()

    local ret =false
    local timeout = math.floor(volume * setting.liquid.meterLimit.pumpTimeoutFactor)   -- 获取操作结果事件超时时间
    local map = ValveMap.new(source.valve)

    local pump = pumps[source.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象

    log:debug("{Pump} source = " .. source.name .. ", volume = " .. volume .. ", speed = " .. speed)

    --打开蠕动泵进行吸操作
    local err,result = pcall(function() return pump:Start(RollDirection.Suck, volume, speed) end)

    if not err then      -- 出现异常
        pump:Stop()                                                            -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            return false
        end
    end

    -- 打开相关液路的阀门
    if source.lc == true then
        map = LCValveMap.new(source.valve)
        err,result = pcall(function() return self.ILCSolenoidValve:SetValveMap(map) end)
    else
        err,result = pcall(function() return self.ISolenoidValve:SetValveMap(map) end)
    end

    if not err then -- 出现异常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        pump:Stop()                                                                  -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            pump:Stop()
            return false
        end
    end

    -- 等待泵操作结果事件
    err,result = pcall(function() return pump:ExpectResult(timeout) end)

    if not err then -- 出现异常
        pump:Stop()                                                                      -- 停止泵
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        error(result)
    else    --函数调用正常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门

        if result:GetResult() == PumpOperateResult.Failed then
            error (PumpFailedException:new{liquidType = source, dir = RollDirection.Suck,})
        elseif result:GetResult()  == PumpOperateResult.Stopped then
            error (PumpStoppedException:new{liquidType = source, dir = RollDirection.Suck,})
        elseif result:GetResult()  == PumpOperateResult.Finished then
            ret = true;
        end
    end

    map = nil
    ReagentRemainManager.ReduceReagent(source, volume)
    flowManager:ClearAllRemainEvent()
    return ret
end

--[[
 * @brief 泵抽取液体操作。
 * @param[in] source 管道类型。
 * @param[in] volume 溶液体积。
  * @param[in] speed 泵速，0为默认速度。
--]]
function Operator:PumpNotCloseValve(source, volume, speed)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()

    local ret =false
    local timeout = math.floor(volume * setting.liquid.meterLimit.pumpTimeoutFactor)   -- 获取操作结果事件超时时间
    local map = ValveMap.new(source.valve)
    local pump = pumps[source.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象

    log:debug("{Pump} source = " .. source.name .. ", volume = " .. volume .. ", speed = " .. speed)

    --打开蠕动泵进行吸操作
    local err,result = pcall(function() return pump:Start(RollDirection.Suck, volume, speed) end)

    if not err then      -- 出现异常
        pump:Stop()                                                            -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            return false
        end
    end

    -- 打开相关液路的阀门
    if source.lc == true then
        map = LCValveMap.new(source.valve)
        err,result = pcall(function() return self.ILCSolenoidValve:SetValveMap(map) end)
    else
        err,result = pcall(function() return self.ISolenoidValve:SetValveMap(map) end)
    end

    if not err then -- 出现异常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        pump:Stop()                                                                  -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            pump:Stop()
            return false
        end
    end

    -- 等待泵操作结果事件
    err,result = pcall(function() return pump:ExpectResult(timeout) end)

    if not err then -- 出现异常
        pump:Stop()                                                                      -- 停止泵
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        error(result)
    else    --函数调用正常
        --map:SetData(0)
        --self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门

        if result:GetResult() == PumpOperateResult.Failed then
            error (PumpFailedException:new{liquidType = source, dir = RollDirection.Suck,})
        elseif result:GetResult()  == PumpOperateResult.Stopped then
            error (PumpStoppedException:new{liquidType = source, dir = RollDirection.Suck,})
        elseif result:GetResult()  == PumpOperateResult.Finished then
            ret = true;
        end
    end

    map = nil
    ReagentRemainManager.ReduceReagent(source, volume)
    flowManager:ClearAllRemainEvent()
    return ret
end

--[[
 * @brief 泵无事件操作。
 * @param[in] source 管道类型。
 * @param[in] volume 溶液体积。
  * @param[in] speed 泵速，0为默认速度。
--]]
function Operator:PumpNoEvent(source, volume, speed)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()

    local ret =false
    local pump = pumps[source.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象

    log:debug("{Pump} source = " .. source.name .. ", volume = " .. volume .. ", speed = " .. speed .. " ml/min")

    --打开蠕动泵进行吸操作
    local err,result = pcall(function() return pump:Start(RollDirection.Suck, volume, speed) end)

    if not err then      -- 出现异常
        pump:Stop()                                                            -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            return false
        end
    end

    return ret
end


--[[
 * @brief 泵无事件操作。
 * @param[in] source 管道类型。
 * @param[in] volume 溶液体积。
  * @param[in] speed 泵速，0为默认速度。
--]]
function Operator:DrainNoEvent(source, volume, speed)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()

    local ret =false
    local pump = pumps[source.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象

    log:debug("{Drain} source = " .. source.name .. ", volume = " .. volume .. ", speed = " .. speed*60 .. " ml/min")

    --打开蠕动泵进行吸操作
    local err,result = pcall(function() return pump:Start(RollDirection.Drain, volume, speed) end)

    if not err then      -- 出现异常
        pump:Stop()                                                            -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            return false
        end
    end

    return ret
end

function Operator:ForwardNoEvent(source, volume, speed)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()

    local ret =false
    local timeout = math.floor(volume * setting.liquid.meterLimit.pumpTimeoutFactor)   -- 获取操作结果事件超时时间
    local map = ValveMap.new(source.valve)
    local pump = pumps[source.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象

    log:debug("{Pump} source = " .. source.name .. ", volume = " .. volume .. ", speed = " .. speed)

    --打开蠕动泵进行吸操作
    local err,result = pcall(function() return pump:Start(RollDirection.Suck, volume, speed) end)

    if not err then      -- 出现异常
        pump:Stop()                                                            -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            return false
        end
    end

    err,result = pcall(function() return pump:ExpectResult(3000) end)

    if not err then -- 出现异常
        pump:Stop()                                                                      -- 停止泵
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        error(result)
    else    --函数调用正常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        if result:GetResult() == PumpOperateResult.Failed then
            error (PumpFailedException:new{liquidType = source, dir = RollDirection.Suck,})
        elseif result:GetResult()  == PumpOperateResult.Stopped then
            error (PumpStoppedException:new{liquidType = source, dir = RollDirection.Suck,})
        elseif result:GetResult()  == PumpOperateResult.Finished then
            ret = true;
        end
    end

    map = nil
    ReagentRemainManager.ReduceReagent(source, volume)
    flowManager:ClearAllRemainEvent()

    return ret
end

function Operator:BackwardNoEvent(source, volume, speed)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()

    local ret =false
    local timeout = math.floor(volume * setting.liquid.meterLimit.pumpTimeoutFactor)   -- 获取操作结果事件超时时间
    local map = ValveMap.new(source.valve)
    local pump = pumps[source.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象

    log:debug("{Drain} source = " .. source.name .. ", volume = " .. volume .. ", speed = " .. speed)

    --打开蠕动泵进行吸操作
    local err,result = pcall(function() return pump:Start(RollDirection.Drain, volume, speed) end)

    if not err then      -- 出现异常
        pump:Stop()                                                            -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            return false
        end
    end

    err,result = pcall(function() return pump:ExpectResult(3000) end)

    if not err then -- 出现异常
        pump:Stop()                                                                      -- 停止泵
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        error(result)
    else    --函数调用正常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        if result:GetResult() == PumpOperateResult.Failed then
            error (PumpFailedException:new{liquidType = source, dir = RollDirection.Suck,})
        elseif result:GetResult()  == PumpOperateResult.Stopped then
            error (PumpStoppedException:new{liquidType = source, dir = RollDirection.Suck,})
        elseif result:GetResult()  == PumpOperateResult.Finished then
            ret = true;
        end
    end

    map = nil
    ReagentRemainManager.ReduceReagent(source, volume)
    flowManager:ClearAllRemainEvent()

    return ret
end

--[[
 * @brief 排液操作。
 * @param[in] dest 管道类型。
 * @param[in] volume 溶液体积。
  * @param[in] speed 泵速，0为默认速度。
--]]
function Operator:Drain(dest, volume, speed)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()

    local ret =false
    local timeout = math.floor(volume * setting.liquid.meterLimit.pumpTimeoutFactor)  -- 获取操作结果事件超时时间
    local map = ValveMap.new(dest.valve)
    local pump = pumps[dest.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象

    log:debug("{Drain} dest = " .. dest.name .. ", volume = " .. volume .. ", speed = " .. speed)

    local err,result
    -- 打开相关液路的阀门
    if dest.lc == true then
        map = LCValveMap.new(source.valve)
        err,result = pcall(function() return self.ILCSolenoidValve:SetValveMap(map) end)
    else
        err,result = pcall(function() return self.ISolenoidValve:SetValveMap(map) end)
    end

    if not err then -- 出现异常
        error(result)
    else    --函数调用正常
         if not result then
            return false
        end
    end

    --打开蠕动泵进行吸操作
    err,result = pcall(function() return pump:Start(RollDirection.Drain, volume, speed) end)

    if not err then      -- 出现异常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        pump:Stop()                                                            -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            return false
        end
    end

    --等待1秒，未开阀之前启动泵，使操作的管路产生负压，减少抽取时产生的气泡
    App.Sleep(1500);

    -- 等待泵操作结果事件
    err,result = pcall(function() return pump:ExpectResult(timeout) end)

    if not err then -- 出现异常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        pump:Stop()                                                                      -- 停止泵
        error(result)
    else    --函数调用正常
        --map:SetData(0)

        if result:GetResult() == PumpOperateResult.Failed then
            error (PumpFailedException:new{liquidType = dest, dir = RollDirection.Drain,})
        elseif result:GetResult()  == PumpOperateResult.Stopped then
            error (PumpStoppedException:new{liquidType = dest, dir = RollDirection.Drain,})
        elseif result:GetResult()  == PumpOperateResult.Finished then
            ret = true;
        end
    end

    map = nil

    ReagentRemainManager.RecoverReagent(dest, volume)
    return ret
end

--[[
 * @brief 排液操作,排液前后不关阀
 * @param[in] dest 管道类型。
 * @param[in] volume 溶液体积。
  * @param[in] speed 泵速，0为默认速度。
--]]
function Operator:DrainNotCloseValve(dest, volume, speed)
    local ret =false
    local timeout = math.floor(volume * setting.liquid.meterLimit.pumpTimeoutFactor)  -- 获取操作结果事件超时时间
    local map = ValveMap.new(dest.valve)
    local pump = pumps[dest.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象

    self:debugPrintf("{Drain} dest = " .. dest.name .. ", volume = " .. volume .. ", speed = " .. speed)

    local err,result = pcall(function() return self.ISolenoidValve:SetValveMap(map) end)

    if not err then -- 出现异常
        error(result)
    else    --函数调用正常
        if not result then
            return false
        end
    end

    --打开蠕动泵进行吸操作
    local err,result = pcall(function() return pump:Start(RollDirection.Drain, volume, speed) end)

    if not err then      -- 出现异常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        pump:Stop()                                                            -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            return false
        end
    end

    --等待1秒，未开阀之前启动泵，使操作的管路产生负压，减少抽取时产生的气泡
--    App.Sleep(1500);

    -- 等待泵操作结果事件
    err,result = pcall(function() return pump:ExpectResult(timeout) end)

    if not err then -- 出现异常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        pump:Stop()                                                                      -- 停止泵
        error(result)
    else    --函数调用正常
        map:SetData(0)

        if result:GetResult() == PumpOperateResult.Failed then
            error (PumpFailedException:new{liquidType = dest, dir = RollDirection.Drain,})
        elseif result:GetResult()  == PumpOperateResult.Stopped then
            error (PumpStoppedException:new{liquidType = dest, dir = RollDirection.Drain,})
        elseif result:GetResult()  == PumpOperateResult.Finished then
            ret = true;
        end
    end

    map = nil

    ReagentRemainManager.RecoverReagent(dest, volume)
    return ret
end

--[[
  * @brief 设置定量过冲程度
  * @param[in] mode 定量模式。
  * @param[in] value 过冲程度。
--]]
function Operator:SetMeterOverValue(mode, value)

    if mode == MeterMode.Accurate then

        self.IOpticalMeter:SetMeterEndPointOverCount(value)
        self:debugPrintf("{MeterOverValue} mode = Accurate, value = " .. value)

    elseif mode == MeterMode.Direct then

    elseif mode == MeterMode.Smart then

    elseif mode == MeterMode.Ropiness then

        self.IOpticalMeter:SetRopinessMeterOverValue(value)
        self:debugPrintf("{MeterOverValue} mode = Ropiness, value = " .. value)

    elseif mode == MeterMode.Layered then

        self.IOpticalMeter:SetRopinessMeterOverValue(value)
        self:debugPrintf("{MeterOverValue} mode = Layered, value = " .. value)

    end

end

--[[
 * @brief 定量液体操作。
  * @param[in] mode 定量模式。
 * @param[in] source 管道类型。
 * @param[in] volume 溶液体积。
  * @param[in] dir 方向。
--]]
function Operator:Meter(mode, source, volume, dir)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()
    local ret =false
    local map = ValveMap.new(source.valve)
    local limitVolume = 0
    local timeout = 0

    if source == setting.liquidType.sample then
        limitVolume = setting.liquid.meterLimit.sampleLimitVolume + volume + config.measureParam.extendSamplePipeVolume    -- 操作限值
    elseif source == setting.liquidType.blank then
        limitVolume = setting.liquid.meterLimit.blankLimitVolume + volume     --操作限值
    elseif source == setting.liquidType.digestionRoom then
        limitVolume = setting.liquid.meterLimit.digestionRoomLimitVolume + volume     -- 操作限值
    else
        limitVolume = setting.liquid.meterLimit.reagentLimitVolume + volume   --操作限值
    end

    if mode == MeterMode.Accurate or mode == MeterMode.Ropiness or mode == MeterMode.Layered then  --限制体积会影响超时判定
        limitVolume = limitVolume * 3
    end
    if mode == MeterMode.Accurate or mode == MeterMode.Ropiness or mode == MeterMode.Layered then
        timeout =  math.floor(limitVolume * setting.liquid.meterLimit.meterTimeoutFactor * 3)   -- 获取操作结果事件超时时间
        if dir == RollDirection.Drain and timeout > 180000 then
            timeout = 180000
        end
    else
        timeout = math.floor(limitVolume * setting.liquid.meterLimit.meterTimeoutFactor)  -- 获取操作结果事件超时时间
    end


    if mode == MeterMode.Accurate then
        log:debug("{Meter} source = " .. source.name .. ", volume = " .. volume .. ", mode = Accurate, dir = " .. dir )
    elseif mode == MeterMode.Direct then
        log:debug("{Meter} source = " .. source.name .. ", volume = " .. volume .. ", mode = Direct, dir = " .. dir )
    elseif mode == MeterMode.Smart then
        log:debug("{Meter} source = " .. source.name .. ", volume = " .. volume .. ", mode = Smart, dir = " .. dir )
    elseif mode == MeterMode.Ropiness then
        log:debug("{Meter} source = " .. source.name .. ", volume = " .. volume .. ", mode = Ropiness, dir = " .. dir )
    elseif mode == MeterMode.Layered then
        log:debug("{Meter} source = " .. source.name .. ", volume = " .. volume .. ", mode = Layered, dir = " .. dir )
    end

    self.isMetering = true

    --打开蠕动泵进行吸操作并精确定量
    local err,result = pcall(function() return self.IOpticalMeter:StartMeter(dir, mode, volume, limitVolume) end)

    if not err then      -- 出现异常
        self:StopMeter()                                                            -- 停止泵
        self.isMetering = false
        error(result)
    else    --函数调用正常
        if not result then
            self.isMetering = false
            return false
        end
    end

    --等待1秒，未开阀之前启动泵，使操作的管路产生负压，减少抽取时产生的气泡
    App.Sleep(1500);

    -- 打开相关液路的阀门
    err,result = pcall(function() return self.ISolenoidValve:SetValveMap(map) end)

    if not err then -- 出现异常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        self:StopMeter()                                                            -- 停止泵
        self.isMetering = false
        error(result)
    else    --函数调用正常
        if not result then
            self:StopMeter()                                                            -- 停止泵
            self.isMetering = false
            return false
        end
    end

    -- 等待泵操作结果事件
    err,result = pcall(function() return self.IOpticalMeter:ExpectMeterResult(timeout) end)

    if not err then -- 出现异常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        self:StopMeter()                                                            -- 停止泵
        self.isMetering = false
        error(result)
    else    --函数调用正常
        self.isMetering = false
         if result == MeterResult.Failed then
                error (MeterFailedException:new{liquidType = source,})
        elseif  result == MeterResult.Overflow then
                error (MeterOverflowException:new{liquidType = source,})
        elseif  result == MeterResult.Stopped then
                error (MeterStoppedException:new{liquidType = source,})
        elseif  result == MeterResult.Unfinished then
            error (MeterUnfinishedException:new{liquidType = source,})
        elseif  result == MeterResult.AirBubble then
                error (MeterAirBubbleException:new{liquidType = source,})
        elseif result == MeterResult.Finished then
                ret = true
        end
    end

    map = nil
    ReagentRemainManager.ReduceReagent(source, volume)
    return ret
end

function Operator:CombineMeter(mode, source, volume, dir, isOpenValve, isCloseValve)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()
    local ret =false
    local map = ValveMap.new(source.valve)
    local limitVolume = 0
    local timeout = 0
    local openValve = true
    local closeValve = true
    if isOpenValve ~= nil then
        openValve = isOpenValve
    end
    if isCloseValve ~= nil then
        closeValve = isCloseValve
    end
    if source == setting.liquidType.sample then
        limitVolume = setting.liquid.meterLimit.sampleLimitVolume + volume + config.measureParam.extendSamplePipeVolume    -- 操作限值
    elseif source == setting.liquidType.blank then
        limitVolume = setting.liquid.meterLimit.blankLimitVolume + volume     --操作限值
    elseif source == setting.liquidType.digestionRoom then
        limitVolume = setting.liquid.meterLimit.digestionRoomLimitVolume + volume     -- 操作限值
    else
        limitVolume = setting.liquid.meterLimit.reagentLimitVolume + volume   --操作限值
    end

    if mode == MeterMode.Accurate or mode == MeterMode.Ropiness or mode == MeterMode.Layered then  --限制体积会影响超时判定
        limitVolume = limitVolume * 3
    end
    if mode == MeterMode.Accurate or mode == MeterMode.Ropiness or mode == MeterMode.Layered then
        timeout =  math.floor(limitVolume * setting.liquid.meterLimit.meterTimeoutFactor * 3)   -- 获取操作结果事件超时时间
    else
        timeout = math.floor(limitVolume * setting.liquid.meterLimit.meterTimeoutFactor)  -- 获取操作结果事件超时时间
    end


    if mode == MeterMode.Accurate then
        log:debug("{Meter} source = " .. source.name .. ", volume = " .. volume .. ", mode = Accurate, dir = " .. dir )
    elseif mode == MeterMode.Direct then
        log:debug("{Meter} source = " .. source.name .. ", volume = " .. volume .. ", mode = Direct, dir = " .. dir )
    elseif mode == MeterMode.Smart then
        log:debug("{Meter} source = " .. source.name .. ", volume = " .. volume .. ", mode = Smart, dir = " .. dir )
    elseif mode == MeterMode.Ropiness then
        log:debug("{Meter} source = " .. source.name .. ", volume = " .. volume .. ", mode = Ropiness, dir = " .. dir )
    elseif mode == MeterMode.Layered then
        log:debug("{Meter} source = " .. source.name .. ", volume = " .. volume .. ", mode = Layered, dir = " .. dir )
    end

    self.isMetering = true

    --打开蠕动泵进行吸操作并精确定量
    local err,result = pcall(function() return self.IOpticalMeter:StartMeter(dir, mode, volume, limitVolume) end)

    if not err then      -- 出现异常
        self:StopMeter()                                                            -- 停止泵
        self.isMetering = false
        error(result)
    else    --函数调用正常
        if not result then
            self.isMetering = false
            return false
        end
    end

    if openValve == true then
        --等待1秒，未开阀之前启动泵，使操作的管路产生负压，减少抽取时产生的气泡
        App.Sleep(1500);

        -- 打开相关液路的阀门
        err,result = pcall(function() return self.ISolenoidValve:SetValveMap(map) end)

        if not err then -- 出现异常
            map:SetData(0)
            self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
            self:StopMeter()                                                            -- 停止泵
            self.isMetering = false
            error(result)
        else    --函数调用正常
            if not result then
                self:StopMeter()                                                            -- 停止泵
                self.isMetering = false
                return false
            end
        end
    end

    -- 等待泵操作结果事件
    err,result = pcall(function() return self.IOpticalMeter:ExpectMeterResult(timeout) end)

    if not err then -- 出现异常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        self:StopMeter()                                                            -- 停止泵
        self.isMetering = false
        error(result)
    else    --函数调用正常
        self.isMetering = false
        if result == MeterResult.Failed then
            error (MeterFailedException:new{liquidType = source,})
        elseif  result == MeterResult.Overflow then
            error (MeterOverflowException:new{liquidType = source,})
        elseif  result == MeterResult.Stopped then
            error (MeterStoppedException:new{liquidType = source,})
        elseif  result == MeterResult.Unfinished then
            error (MeterUnfinishedException:new{liquidType = source,})
        elseif  result == MeterResult.AirBubble then
            error (MeterAirBubbleException:new{liquidType = source,})
        elseif result == MeterResult.Finished then
            ret = true
        end
    end

    map = nil
    ReagentRemainManager.ReduceReagent(source, volume)
    return ret
end

--[[
 * @brief 停止定量操作。
--]]

function Operator:StopMeter()




end

--[[
 * @brief 定量结束自动关闭阀开关
 * @param IsAutoCloseValve Bool 是否自动关闭。TRUE 自动关闭，FALSE 定量结束不关闭阀
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DSCP_ERROR 操作失败；
--]]
function Operator:AutoCloseValve(isCloseValve)
    return self.IOpticalMeter:IsAutoCloseValve(isCloseValve)
end

--[[
 * @brief   消解器冒泡。
 * @details 向消解器空气，使得消解器溶液充分反应。
 * @param[in] time 冒泡时间
--]]
 function Operator:AirToDigestionRoom(time)
     if time == nil then
         time = setting.liquid.pumpAirVolume/setting.liquid.pumpAirSpeed
     end

     local factor = self.IPeristalticPump:GetPumpFactor(0)
     local drainSpeed = setting.liquid.prefabricateDrainSpeed * factor

     --self:Drain(setting.liquidType.none, 0.3, drainSpeed/2)        -- 不开任何阀的情况下，给定量管泵入空气使定量管恢复常压
     self:Drain(setting.liquidType.digestionRoom, time * setting.liquid.pumpAirSpeed, setting.liquid.pumpAirStepSpeed * factor)
end

--[[
 * @brief 管道隔离。
 * @param[in] source 管道类型。
--]]

function Operator:SecludeLiquid(source, vol)
     if source == setting.liquidType.digestionRoom then
         self:Drain(source, setting.liquid.liquidSecludeVolume*10, setting.liquid.liquidSecludeSpeed)
     else
         if vol ~= nil and vol ~= 0 then
             self:Drain(source, vol, setting.liquid.liquidSecludeSpeed)
         else
             self:Drain(source, setting.liquid.liquidSecludeVolume, setting.liquid.liquidSecludeSpeed)
         end
     end
end

--[[
 * @brief 泵操作从定量管排液到废液桶。
 * @param[in] vol 溶液体积。
  * @param[in] wType 排放类型 (默认为废水)
--]]
function Operator:DrainToWaste(vol, wType)
    if vol == nil then
        vol = 1
    end

    local factor = self.IPeristalticPump:GetPumpFactor(0)
    local drainSpeed = setting.liquid.prefabricateDrainSpeed * factor

    if wType == WasteType.WasteReagent or wType == setting.liquidType.waste then
        local drainSpeed = setting.liquid.prefabricateDrainSpeed * factor
        self:Drain(setting.liquidType.waste,
                vol + setting.liquid.multipleValvePipeVolume  + setting.liquid.wastePipeVolume,
                drainSpeed)
    else
        self:Drain(setting.liquidType.wasteWater,
                vol + setting.liquid.multipleValvePipeVolume  + setting.liquid.wastePipeVolume,
                drainSpeed)
    end
end

--[[
 * @brief 泵操作从定量管排液到废液桶。
 * @param[in] vol 溶液体积。
  * @param[in] wType 排放类型 (默认为废液)
--]]
function Operator:DrainToWasteNotCloseValve(vol, wType)
    if vol == nil then
        vol = 1
    end

    local factor = self.IPeristalticPump:GetPumpFactor(0)
    local drainSpeed = setting.liquid.prefabricateDrainSpeed * factor

    if wType == WasteType.WasteWater or wType == setting.liquidType.wasteWater then
        self:DrainNotCloseValve(setting.liquidType.wasteWater, vol, drainSpeed)
    else
        self:DrainNotCloseValve(setting.liquidType.waste, vol, drainSpeed)
    end
end

--[[
 * @brief 泵操作从定量管排液到废水桶。
 * @param[in] vol 溶液体积。
--]]
function Operator:DrainToWasteWater(vol)
    if vol == nil then
        vol = 1
    end

    local factor = self.IPeristalticPump:GetPumpFactor(0)
    local drainSpeed = setting.liquid.prefabricateDrainSpeed * factor

    self:Drain(setting.liquidType.wasteWater,
    vol + setting.liquid.multipleValvePipeVolume  + setting.liquid.wastePipeVolume,
            drainSpeed)
end

--[[
 * @brief 排管道中的溶液。
 * @details 排空水样管。
 * @param[in] source 管道类型。
 * @param[in] vol 溶液体积。
--]]

function Operator:DrainLiquid(source, vol, wType)
    local wasteType = WasteType.WasteWater
    if wType ==  WasteType.WasteReagent or wType == setting.liquidType.waste then
        wasteType = WasteType.WasteReagent
    end

    local factor = self.IPeristalticPump:GetPumpFactor(0)
    local drainSpeed = setting.liquid.prefabricateDrainSpeed * factor

    self:DrainToWaste(0, wasteType)		--先将定量管至废液桶之间的多联阀管路清空
    self:Drain(source, vol, drainSpeed)	--隔离相应试剂管路
end

--[[
 * @brief 恒温控制
 * @details 恒温至目标温度
 * @param[in] mode 恒温模式,
 * @param[in] targetTemp 目标温度,
 * @param[in] tolerance 容差温度,
 * @param[in] timeout 超时时间,
--]]
function Operator:StartThermostat(mode, targetTemp, tolerance, timeout)

        dc:ClearThermostatRemainEvent()  --清空所有事件

        local temp = 0

        local curHeaterMaxDutyCycle = self.ITemperatureControl:GetHeaterMaxDutyCycle()
        if math.abs(config.measureParam.heaterMaxDutyCycle - curHeaterMaxDutyCycle) > self.PRECISE then
            self.ITemperatureControl:SetHeaterMaxDutyCycle(config.measureParam.heaterMaxDutyCycle)
            self:debugPrintf("重设加热丝占空比 "..self.ITemperatureControl:GetHeaterMaxDutyCycle())
        end

        local ret = self.ITemperatureControl:StartThermostat(mode, targetTemp, tolerance, timeout)

        --获取恒温器状态
        if ret == false then
            self:debugPrintf("StartThermostat return ret[",ret, "]!!!")
            
            App.Sleep(100);
            if self.ITemperatureControl:GetThermostatStatus() == 2 then
                ret = true
            end
        end

        if ret == true then

                self:debugPrintf("{StartThermostat} mode = " .. mode .. ", targetTemp = " .. targetTemp .. ", tolerance = " .. tolerance ..", timeout = " .. timeout)
                self.isThermostat = true

                local result = self.ITemperatureControl:ExpectThermostat(timeout * 1000 + 1000);

                self:debugPrintf("{thermostatResult} result = " .. result:GetResult() .. "，temp = " .. result:GetTemp())

                if result:GetResult() == ThermostatOperateResult.Failed then
                    self.isThermostat = false
                    error(ThermostatFailedException:new({mode = mode, targetTemp = targetTemp, toleranceTemp = tolerance, timeout = timeout}))
                elseif result:GetResult() == ThermostatOperateResult.Stopped then
                    self.isThermostat = false
                    error(ThermostatStoppedException:new({mode = mode, targetTemp = targetTemp, toleranceTemp = tolerance, timeout = timeout}))
                elseif result:GetResult() == ThermostatOperateResult.Timeout then
                    self.isThermostat = false
                    error(ThermostatTimeoutException:new({mode = mode, targetTemp = targetTemp, toleranceTemp = tolerance, timeout = timeout}))
                elseif result:GetResult() == ThermostatOperateResult.Reached then
                    self.isThermostat = false
                    temp = result:GetTemp() --返回温度
                end
        end

        return temp
end

--[[
 * @brief 停止恒温控制.
--]]
function Operator:StopThermostat()
       local temp = 0


       local ret = false
        if self.ITemperatureControl:GetThermostatStatus() == 2 then
            ret = self.ITemperatureControl:StopThermostat()
        else
            return ret
        end

        --获取恒温器状态
        if ret == false then
            self:debugPrintf("StopThermostat return ret[",ret, "]!!!")

            local cnt = 5
            while true do
                App.Sleep(100)

                if self.ITemperatureControl:GetThermostatStatus() == 1 then
                    ret = true
                    break
                end

                cnt = cnt -1
                if cnt <= 0 then
                    break
                end
            end
        end

        if ret == true then
                self:debugPrintf("{StopThermostat}")

                local result = self.ITemperatureControl:ExpectThermostat(2000)
                temp  = result:GetTemp()

                self:debugPrintf("{thermostatResult} result = " .. result:GetResult() .. "，temp = " .. result:GetTemp())

                if result:GetResult() == ThermostatOperateResult.Stopped then
                    self.isThermostat = false
                end
        end

        return temp
end

--[[
 * @brief 停止恒温控制.
--]]
function Operator:StopExtThermostat(index)
    local temp = 0

    local ret = false
    if self.IExtTemperatureControl:GetThermostatStatus(index) == 2 then
        ret = self.IExtTemperatureControl:StopThermostat(index, true)
    else
        return ret
    end

    --获取恒温器状态
    if ret == false then
        self:debugPrintf("StopExtThermostat return ret[",ret, "]!!!")

        local cnt = 5
        while true do
            App.Sleep(100)

            if self.IExtTemperatureControl:GetThermostatStatus(index) == 2 then
                ret = true
                break
            end

            cnt = cnt -1
            if cnt <= 0 then
                break
            end
        end
    end

    if ret == true then
        self:debugPrintf("{StopThermostat}")

        local result = self.IExtTemperatureControl:ExpectThermostat(2000)
        temp  = result:GetTemp()

        self:debugPrintf("{thermostatResult} result = " .. result:GetResult() .. "，temp = " .. result:GetTemp())

        if result:GetResult() == ThermostatOperateResult.Stopped then
            self.isThermostat = false
        end
    end

    return temp
end

--[[
 * @brief 修改恒温温度参数
  * @details 恒温至目标温度，不等待事件
   * @param[in] index 恒温器索引，0 - 燃烧炉， 1 - 制冷器
 * @param[in] mode 恒温模式,仅支持Refrigerate， Heater
 * @param[in] targetTemp 目标温度,
 * @param[in] tolerance 容差温度,
 * @param[in] timeout 超时时间,
--]]
function Operator:ReviseThermostatTemp(index, mode, targetTemp, tolerance, timeout)

    dc:ClearThermostatRemainEvent()  --清空所有事件

    local temp = 0
    local flowManager = FlowManager.Instance()

    if flowManager:IsComValid() ~= true then
        log:warn("通信失败，修改恒温温度参数无效")
        return false
    end

    log:debug("Thermostat index: ".. index)

    if mode == ThermostatMode.Auto then
        log:warn("设备不支持自动恒温模式")
        return false
    end

    if index ~= setting.temperature.indexStove then
        op:StopExtThermostat(index)
    else
        op:StopThermostat()
    end

    local ret = self.ITemperatureControl:ReviseThermostatTemp(index, mode, targetTemp, tolerance, timeout)

    ----获取恒温器状态
    if ret == false then
        local str
        if index == setting.temperature.indexRefrigerator then
            log:info("制冷器调节异常")
            str = "制冷器调节异常"
        elseif index == setting.temperature.indexNDIR then
            log:info("测量模块调节异常")
            str = "测量模块调节异常"
        else
            log:info("燃烧炉调节异常")
            str = "燃烧炉调节异常"
        end

        App.Sleep(500);
        if self.ITemperatureControl:GetThermostatStatus() == 2 then
            log:debug("StartThermostat ok ")
            ret = true
        else
            log:debug("StartThermostat failed ")
            log:info("恒温调节失败，重设温度")
            self.ITemperatureControl:ReviseThermostatTemp(index, mode, targetTemp, tolerance, timeout)
            --local alarm = Helper.MakeAlarm(setting.alarm.thermostatFault, str)
            --AlarmManager.Instance():AddAlarm(alarm)
        end
    end

    if ret == true then

        log:debug("{ReviseThermostat} index = " .. index .. ", mode = " .. mode .. ", targetTemp = " .. targetTemp .. ", tolerance = " .. tolerance ..", timeout = " .. timeout)
        self.isThermostat = true

        --local result = self.ITemperatureControl:ExpectThermostat(timeout * 1000 + 1000);
        --
        --self:debugPrintf("{thermostatResult} result = " .. result:GetResult() .. "，temp = " .. result:GetTemp())
        --
        --if result:GetResult() == ThermostatOperateResult.Failed then
        --    self.isThermostat = false
        --    error(ThermostatFailedException:new({mode = mode, targetTemp = targetTemp, toleranceTemp = tolerance, timeout = timeout}))
        --elseif result:GetResult() == ThermostatOperateResult.Stopped then
        --    self.isThermostat = false
        --    error(ThermostatStoppedException:new({mode = mode, targetTemp = targetTemp, toleranceTemp = tolerance, timeout = timeout}))
        --elseif result:GetResult() == ThermostatOperateResult.Timeout then
        --    self.isThermostat = false
        --    error(ThermostatTimeoutException:new({mode = mode, targetTemp = targetTemp, toleranceTemp = tolerance, timeout = timeout}))
        --elseif result:GetResult() == ThermostatOperateResult.Reached then
        --    self.isThermostat = false
        --    temp = result:GetTemp() --返回温度
        --end
    end

    return ret
end

--[[
 * @brief 检查消解室温度安全
 * @details 当消解室温度高于安全温度时，启动降温操作.
--]]
function Operator:CheckDigestSafety(update)

    local safeTemp = setting.temperature.reactTemperatureLimitTop
    local startTime = os.time()

    local  thermostatTemp = dc:GetCurrentTemperature():GetThermostatTemp()
    local refrigeratorTemp = dc:GetReportThermostatTemp(setting.temperature.temperatureRefrigerator)
    local NDIRTemp = dc:GetReportThermostatTemp(setting.temperature.temperatureNDIR)
    self:debugPrintf("安全冷却前燃烧炉温度 = " .. thermostatTemp .. " ℃");
    self:debugPrintf("安全冷却前制冷器温度 = " .. refrigeratorTemp .. " ℃");
    self:debugPrintf("安全冷却前测量池温度 = " .. NDIRTemp .. " ℃");

    if update ~= nil then
        setting.temperature.monitor.stoveLastTemperature = thermostatTemp
        setting.temperature.monitor.stoveAbnormalTemperature = thermostatTemp
        setting.temperature.monitor.refrigeratorLastTemperature = refrigeratorTemp
        setting.temperature.monitor.refrigeratorAbnormalTemperature = refrigeratorTemp
        setting.temperature.monitor.NDIRLastTemperature = NDIRTemp
        setting.temperature.monitor.NDIRAbnormalTemperature = NDIRTemp
    end


    local temp = thermostatTemp - safeTemp

    if  temp >  0.01 then
        --状态设置
        if StatusManager.Instance():IsIdleStatus() == true then
                local runStatus = Helper.Status.SetStatus(setting.runStatus.digestCooling)
                StatusManager.Instance():SetStatus(runStatus)
        end

        --动作设置
        local runAction = Helper.Status.SetAction(setting.runAction.digestCooling)
        StatusManager.Instance():SetAction(runAction)

         --开机箱风扇
        --self.ITemperatureControl:BoxFanSetOutput(1)

        --冷却
        local cnt = 0
        while true do
            --获取恒温器状态
            if self.ITemperatureControl:GetThermostatStatus() == 2 then
                    self:StopThermostat()
            end

            --local err,result = pcall(function()
            --
            --                     return self:StartThermostat(ThermostatMode.Refrigerate, safeTemp, 1, setting.temperature.digestToReactCoolingTimeout)
            --                   end)-----end pcall
            --if not err then      -- 出现异常
            --    if type(result) == "userdata" then
            --        ExceptionHandler.MakeAlarm(result)
            --    elseif type(result) == "table" then
            --        if getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
            --            self:StopThermostat()
            --            self.ITemperatureControl:BoxFanSetOutput(0) 	--关机箱风扇
            --            error(result)
            --        else
            --            ExceptionHandler.MakeAlarm(result)
            --        end
            --    else
            --        log:warn(result)										--C++、Lua系统异常
            --    end
            --end

            thermostatTemp = dc:GetCurrentTemperature():GetThermostatTemp()
            temp = thermostatTemp - safeTemp
            if  temp >  2 then
                self:debugPrintf("安全温度未达标")
                cnt = cnt + 1
                if cnt >= 3 then
                    break
                end
            else
                break
            end
        end

        --获取恒温器状态
        if self.ITemperatureControl:GetThermostatStatus() == 2 then
                self:StopThermostat()
        end

        self:debugPrintf("到达安全冷却目标温度 = ".. thermostatTemp .. " ℃" )
        self:debugPrintf("消解室冷却总时间 = " .. os.time() - startTime)
    end
end

--[[
 * @brief 采集AD
 * @details 采集光学测量AD信号
 * @param[in] acquireTime 采集时间
--]]
function Operator:StartAcquirerAD(acquireTime)
    local AD = OpticalAD.new()
    local temp = 0

    local timeout = acquireTime * 1000 + 1000

    self.isAcquiring = true

    if self.IOpticalAcquire:StartAcquirer(acquireTime) == true then
        local err,result = pcall(function() return self.IOpticalAcquire:ExpectADAcquirer(timeout) end)
        if not err then -- 出现异常
            self.IOpticalAcquire:StopAcquirer() --停止采集
            self.isAcquiring = false
            error(result) --向上层抛异常
        else
            temp = result
            self.isAcquiring = false
            if result.GetResult ~= nil then  --新增采集错误结果码的版本
                if result:GetResult() == AcquiredOperateResult.Failed then
                    error (AcquirerADFailedException:new())
                elseif result:GetResult()  == AcquiredOperateResult.Stopped then
                    error (AcquirerADStoppedException:new())
                elseif result:GetResult()  == AcquiredOperateResult.Finished then
                    temp = result:GetAD()
                end
            end
            local refAD = temp:GetReference()
            local meaAD = temp:GetMeasure()
            AD:SetReference(refAD)
            AD:SetMeasure(meaAD)
        end
    end

    return AD
end

--[[
 * @brief 采集AD(带过滤数据)
 * @details 采集光学测量AD信号
 * @param[in] acquireTime 采集时间
--]]
function Operator:StartAcquirerADWithFiltration(acquireTime)
    local readAdTime = 1        --测量间隔
    local referenceAD = {}
    local measureAD = {}
    local temp = 0
    local referenceADSumData = 0
    local measureADSumData = 0
    local filterNum = 0

    filterNum = math.floor(acquireTime/5)

    local retAD = OpticalAD.new()
    local AD = op:StartAcquirerAD(readAdTime)

    -- 读取AD值
    for i = 1,acquireTime,readAdTime do
        AD = op:StartAcquirerAD(readAdTime)
        referenceAD[i] = AD:GetReference()
        measureAD[i] = AD:GetMeasure()
    end

    for i = 1,acquireTime,1 do
        log:debug("referenceAD["..i.."] = "..referenceAD[i].." measureAD["..i.."] = "..measureAD[i])
    end

    for i = 2, acquireTime, 1 do
        for j = acquireTime,i, -1 do
            if referenceAD[j] < referenceAD[j - 1] then
                temp = referenceAD[j - 1];
                referenceAD[j - 1] = referenceAD[j];
                referenceAD[j] = temp;
            end
            if measureAD[j] < measureAD[j - 1] then
                temp = measureAD[j - 1];
                measureAD[j - 1] = measureAD[j];
                measureAD[j] = temp;
            end
        end
    end

    for i = 1+filterNum,acquireTime-filterNum,1 do
        referenceADSumData = referenceADSumData + referenceAD[i]
        measureADSumData = measureADSumData + measureAD[i]
    end

    local ret1 = math.floor(referenceADSumData/(acquireTime - filterNum*2))
    local ret2 = math.floor(measureADSumData/(acquireTime - filterNum*2))

    retAD:SetReference(ret1)
    retAD:SetMeasure(ret2)

    return retAD
end

function Operator:AutoStaticADControl(index, target)
    local ret = false
    local timeout = 120 * 1000

    local ret = self.IOpticalAcquire:StartStaticADControl(index,target)

    if ret == true then
        self.isStaticADControl = true
        self:debugPrintf("Auto static AD control started index = "..index.." target = "..target)

        local result = self.IOpticalAcquire:ExpectStaticADControlResult(timeout)

        if result == StaticADControlResult.Unfinished then
            if self.isStaticADControl == true then
                self.isStaticADControl = false
                error (StaticADControlFailedException:new())
            elseif self.isStaticADControl == false then  --被停止
                error (StaticADControlStoppedException:new())
            end
            return false
        elseif result == StaticADControlResult.Finished then
            self.isStaticADControl = false
            self:debugPrintf("Auto static AD control index = "..index.." target = "..target.." finished !")
            return true
        end
    else
        self:debugPrintf("Auto static AD control index = "..index.." target = "..target.." start fail !")
        return false
    end
end

function Operator:AutoMeasureLEDControl(target, tolerance, timeout)
    local ret = false

    ret = self.IOpticalAcquire:StartLEDOnceAdjust(target, tolerance, timeout)

    if ret == true then
        self.isStaticADControl = true
        self:debugPrintf("Auto Measure LED control started ".." target = "..target.." tolerance = "..tolerance.." timeout = "..timeout)

        local result = self.IOpticalAcquire:ExpectLEDOnceAdjust(timeout)
        print("LED: " .. result)

        if result == 1 then  --AdjustResult.Failed
            if self.isStaticADControl == true then
                self.isStaticADControl = false
                error (MeasureLEDControlFailedException:new())
            elseif self.isStaticADControl == false then  --被停止
                error (MeasureLEDControlStoppedException:new())
            end
            return false
        elseif result == 0 then --AdjustResult.Finished
            self.isStaticADControl = false
            self:debugPrintf("Auto Measure LED control ".." target = "..target.." finished !")
            return true
        end
    else
        self:debugPrintf("Auto Measure LED control ".." target = "..target.." start fail !")
        return false
    end
end


function Operator:StopStaticADControl()
    local ret = false

    local ret = self.IOpticalAcquire:StopStaticADControl()

    if ret == true then
        self:debugPrintf("{Stop Static AD Control}")

        local result = self.IOpticalAcquire:ExpectStaticADControlResult(2000)

        self.isStaticADControl = false
    end
end

--[[
 * @brief 保存定标时间
 * @detail 将定标时间以String的形式保存
--]]
function Operator:SaveCalibrationTimeStr(CalibrationTime,currentRange)
    if CalibrationTime ~= nil then
        if CalibrationTime ~= 0 then
            local DataTime = os.date("*t",CalibrationTime)

            local year = tostring(DataTime.year)
            local month
            if DataTime.month <10 then
                month="0"..tostring(DataTime.month)
            else
                month=tostring(DataTime.month)
            end
            local day
            if DataTime.day <10 then
                day="0"..tostring(DataTime.day)
            else
                day=tostring(DataTime.day)
            end
            local hour
            if DataTime.hour <10 then
                hour="0"..tostring(DataTime.hour)
            else
                hour=tostring(DataTime.hour)
            end
            local min
            if DataTime.min <10 then
                min="0"..tostring(DataTime.min)
            else
                min=tostring(DataTime.min)
            end
            local sec
            if DataTime.sec <10 then
                sec="0"..tostring(DataTime.sec)
            else
                sec=tostring(DataTime.sec)
            end

            config.measureParam.curveParam[currentRange].timeStr = year.."-"..month.."-"..day.." "..hour..":"..min
        end
    end
end

function Operator:CalculateAbsorbance(curveK, curveB, consistency1, consistency2)
    local absorbance1, absorbance2
    absorbance2 = (curveK * consistency2 + curveB)
    absorbance1 = (curveK * consistency1 + curveB)
    return absorbance1, absorbance2
end

function Operator:SaveUserCurve(calibrateDateTime, curveK, curveB, absorbance0, absorbance1, consistency0, consistency1, runTime, rangeView)
    -- 保存校准结果
    --	print("Push calibrate result data to file.")
    local resultManager = ResultManager.Instance()
    local recordData = RecordData.new(resultManager:GetCalibrateRecordDataSize(setting.resultFileInfo.calibrateRecordFile[1].name))

    recordData:PushInt(calibrateDateTime) 			        -- 时间
    recordData:PushDouble(curveK)   				            --标线斜率K
    recordData:PushDouble(curveB)   				            --标线截距B

    recordData:PushFloat(absorbance0) 			            -- 零点反应峰面积
    recordData:PushFloat(consistency0) 			            -- 零点浓度
    recordData:PushFloat(0)		            	-- 零点第一次峰面积
    recordData:PushFloat(0)		            	-- 零点第二次峰面积
    recordData:PushFloat(0)		            	-- 零点第三次峰面积
    recordData:PushFloat(5)      	-- 零点初始制冷模块温度
    recordData:PushFloat(50) 		        -- 零点初始测量模块温度
    recordData:PushFloat(5) 	    -- 零点反应制冷模块温度
    recordData:PushFloat(50) 	            -- 零点反应测量模块温度
    recordData:PushFloat(680) 	    -- 零点初始值燃烧炉温度
    recordData:PushFloat(35) 	    -- 零点反应值上机箱温度
    recordData:PushFloat(35) 	-- 零点反应值下机箱温度
    recordData:PushFloat(680) 	    -- 零点反应值燃烧炉温度
    recordData:PushFloat(35) 	    -- 零点反应值上机箱温度
    recordData:PushFloat(35) 	-- 零点反应值下机箱温度

    recordData:PushFloat(absorbance1) 			                -- 标点峰面积
    recordData:PushFloat(consistency1) 		            	    -- 标点浓度
    recordData:PushFloat(0)		            	-- 标点第一次峰面积
    recordData:PushFloat(0)		            	-- 标点第二次峰面积
    recordData:PushFloat(0)                       -- 标点第三次峰面积
    recordData:PushFloat(5)      	-- 标点初始制冷模块温度
    recordData:PushFloat(50) 		-- 标点初始测量模块温度
    recordData:PushFloat(5) 	    -- 标点反应制冷模块温度
    recordData:PushFloat(50) 	    -- 标点反应测量模块温度
    recordData:PushFloat(680) 	    -- 标点初始值燃烧炉温度
    recordData:PushFloat(35) 	    -- 标点反应值上机箱温度
    recordData:PushFloat(35) 	    -- 标点反应值下机箱温度
    recordData:PushFloat(680) 	    -- 标点反应值燃烧炉温度
    recordData:PushFloat(35) 	    -- 标点反应值上机箱温度
    recordData:PushFloat(35) 	    -- 标点反应值下机箱温度

    recordData:PushFloat(1)					                    -- 曲线线性度R2
    recordData:PushInt(runTime) 	    -- 校准时长
    recordData:PushFloat(rangeView) --  量程

    resultManager:AddCalibrateRecordSlots(setting.resultFileInfo.calibrateRecordFile[1].name, recordData)


end

function Operator:GetCurveParam(currentCalibrate)
    local start = 1
    local num1 = string.find(currentCalibrate, "*")
    local num2 = string.find(currentCalibrate, "C")
    local len = string.len(currentCalibrate)
    local firstSymbol = string.sub(currentCalibrate,1,1)
    local firstSymbolValve = 1
    local secondSymbol = string.sub(currentCalibrate,num2+2,num2+2)
    local secondSymbolValve = 1

    if firstSymbol == "-" then
        firstSymbolValve = -1
        start = 2
    end

    if secondSymbol == "-" then
        secondSymbolValve = -1
    end

    local K = tonumber(string.sub(currentCalibrate,start,num1-2))*firstSymbolValve
    local B = tonumber(string.sub(currentCalibrate,num2+4,len))*secondSymbolValve

    --log:debug("k= "..K*firstSymbolValve)
    --log:debug("b= "..B*secondSymbolValve)
    --log:debug("len= "..len)

    return K,B
end

--[[
 * @brief 注射器排操作(回退)
 * @param[in] source 管道类型。
 * @param[in] volume 溶液体积。
  * @param[in] speed 泵速，0为默认速度。
--]]
function Operator:SyringeDrain(dest, volume, speed)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()
    local ret = false
    local timeout = math.floor(volume * setting.liquid.meterLimit.syringTimeoutFactor)   -- 注射器获取操作结果事件超时时间
    local map = ValveMap.new(dest.valve)
    local pump = pumps[dest.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象
    local xSpeed = 0.3
    local runVolume = 0

    if volume == 0 then
        timeout = math.floor(1 * setting.liquid.meterLimit.pumpTimeoutFactor)
    end

    if speed ~= 0 and speed~=nil then
        xSpeed = speed
    end

    if volume == 0 then
        runVolume = 0
    elseif volume > setting.liquid.syringeMaxVolume then
        runVolume = setting.liquid.syringeMaxVolume
    else
        runVolume = volume
    end

    if runVolume == 0 then
        timeout = math.floor(3 * setting.liquid.meterLimit.pumpTimeoutFactor)   -- 复位操作结果事件超时时间
    end

    log:debug("{Syringe} Drain = " .. dest.name .. ", volume = " .. runVolume .. "ul, speed = " .. xSpeed)

    -- map:SetData(0)
    -- self.ISolenoidValve:SetValveMap(map)

    -- 打开相关液路的阀门
    local err,result = pcall(function() return self.ISolenoidValve:SetValveMap(map) end)

    if not err then      -- 出现异常
        error(result)
    else    --函数调用正常
        if not result then
            return false
        end
    end
    --
    --if dest ~= setting.liquidType.syringeWaste then
    --    --等待1秒，未开阀之前启动泵，使操作的管路产生负压，减少抽取时产生的气泡
    --    App.Sleep(500);
    --end

    --打开注射器进行吸操作
    err,result = pcall(function() return pump:Start(RollDirection.Drain, runVolume, xSpeed) end)

    if not err then -- 出现异常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        pump:Stop()                                                                  -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            pump:Stop()
            return false
        end
    end

    -- 等待泵操作结果事件
    err,result = pcall(function() return pump:ExpectResult(timeout) end)

    if not err then -- 出现异常
        pump:Stop()                                                                      -- 停止泵
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        error(result)
    else    --函数调用正常
        --map:SetData(0)
        map = ValveMap.new(setting.liquidType.map.valve4)
        self.ISolenoidValve:SetValveMap(map)                                     --不关闭气体总阀

        --if  self.isSyringStop then
        --    self.isSyringStop = false
        --    error(UserStopException:new())
        --end

        if result:GetResult() == PumpOperateResult.Failed then
            error (PumpFailedException:new{liquidType = dest, dir = RollDirection.Drain,})
        elseif result:GetResult()  == PumpOperateResult.Stopped then
            error (PumpStoppedException:new{liquidType = dest, dir = RollDirection.Drain,})
        elseif result:GetResult()  == PumpOperateResult.Finished then
            ret = true;
        end
    end
    map = nil
    --ReagentRemainManager.ReduceReagent(source, volume)
    --App.Sleep(500);
    --flowManager:ClearAllRemainEvent()
    return ret
end

function Operator:SyringeDrainNotOpenValve(dest, volume, speed)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()
    local ret =false
    local timeout = math.floor(volume * setting.liquid.meterLimit.pumpTimeoutFactor * 2)   -- 获取操作结果事件超时时间
    local map = ValveMap.new(dest.valve)
    local pump = pumps[dest.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象
    local xSpeed = 0.3
    local runVolume = 0

    if volume == 0 then
        timeout = math.floor(3 * setting.liquid.meterLimit.pumpTimeoutFactor)
    end

    if speed ~= 0 and speed~=nil then
        xSpeed = speed
    end

    if volume == 0 then
        runVolume = 0
    elseif volume > setting.liquid.syringeMaxVolume then
        runVolume = setting.liquid.syringeMaxVolume
    else
        runVolume = volume
    end

    if runVolume == 0 then
        timeout = math.floor(3 * setting.liquid.meterLimit.pumpTimeoutFactor)   -- 复位操作结果事件超时时间
    end

    log:debug("{Syringe} Drain = " .. dest.name .. ", volume = " .. runVolume .. " ul, speed = " .. xSpeed .. " ul/min")

    --打开注射器进行吸操作
    local err,result = pcall(function() return pump:Start(RollDirection.Drain, runVolume, xSpeed) end)

    if not err then      -- 出现异常
        pump:Stop()                                                            -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            return false
        end
    end

    --等待1秒，未开阀之前启动泵，使操作的管路产生负压，减少抽取时产生的气泡
    --App.Sleep(500);

    if not err then -- 出现异常
        pump:Stop()                                                                  -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            pump:Stop()
            return false
        end
    end

    -- 等待泵操作结果事件
    err,result = pcall(function() return pump:ExpectResult(timeout) end)

    if not err then -- 出现异常
        pump:Stop()                                                                      -- 停止泵
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        error(result)
    else    --函数调用正常
        if result:GetResult() == PumpOperateResult.Failed then
            error (PumpFailedException:new{liquidType = dest, dir = RollDirection.Drain,})
        elseif result:GetResult()  == PumpOperateResult.Stopped then
            error (PumpStoppedException:new{liquidType = dest, dir = RollDirection.Drain,})
        elseif result:GetResult()  == PumpOperateResult.Finished then
            ret = true;
        end
    end
    map = nil
    ReagentRemainManager.ReduceReagent(dest, volume)
    return ret
end


function Operator:SyringeDrainNotWaitEvent(dest, volume, speed)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()
    local ret =false
    local timeout = math.floor(volume * setting.liquid.meterLimit.pumpTimeoutFactor * 2)   -- 获取操作结果事件超时时间
    local map = ValveMap.new(dest.valve)
    local pump = pumps[dest.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象
    local xSpeed = 0.3
    local runVolume = 0

    if volume == 0 then
        timeout = math.floor(3 * setting.liquid.meterLimit.pumpTimeoutFactor)
    end

    if speed ~= 0 and speed~=nil then
        xSpeed = speed
    end

    if volume == 0 then
        runVolume = 0
    elseif volume > setting.liquid.syringeMaxVolume then
        runVolume = setting.liquid.syringeMaxVolume
    else
        runVolume = volume
    end

    if runVolume == 0 then
        timeout = math.floor(3 * setting.liquid.meterLimit.pumpTimeoutFactor)   -- 复位操作结果事件超时时间
    end

    log:debug("{Syringe} Drain = " .. dest.name .. ", volume = " .. runVolume .. " ul, speed = " .. xSpeed .. " ul/min")

    --打开注射器进行吸操作
    local err,result = pcall(function() return pump:Start(RollDirection.Drain, runVolume, xSpeed) end)

    if not err then      -- 出现异常
        pump:Stop()                                                            -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            return false
        end
    end

    if not err then -- 出现异常
        pump:Stop()                                                                  -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            pump:Stop()
            return false
        end
    end

    return ret
end

--[[
 * @brief 注射器抽操作
 * @param[in] source 管道类型。
 * @param[in] volume 溶液体积。
  * @param[in] speed 泵速，0为默认速度。
--]]
function Operator:SyringeSuck(source, volume, speed)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()
    --local source = setting.liquidType.syringFromDigestionRoom
    local ret =false
    local timeout = math.floor(volume * setting.liquid.meterLimit.pumpTimeoutFactor)   -- 获取操作结果事件超时时间
    local map = ValveMap.new(source.valve)
    local pump = pumps[source.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象
    local xSpeed = 0.3
    local runVolume = 0

    if speed ~= 0 and speed~=nil then
        xSpeed = speed
    end

    if volume == 0 then
        runVolume = 0
    elseif volume > setting.liquid.syringeMaxVolume then
        runVolume = setting.liquid.syringeMaxVolume
    else
        runVolume = volume
    end

    if volume == 0 then
        timeout = math.floor(3 * setting.liquid.meterLimit.pumpTimeoutFactor)   -- 复位操作结果事件超时时间
    end

    log:debug("{Syringe} Suck = " .. source.name .. ", volume = " .. runVolume .. "ul, speed = " .. xSpeed)

    --打开注射器进行吸操作
    local err,result = pcall(function() return pump:Start(RollDirection.Suck, runVolume, xSpeed) end)

    if not err then      -- 出现异常
        pump:Stop()                                                            -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            log:debug("Syringe suck error stop: " .. result)
            return false
        end
    end

    --log:debug("{Syringe} num = " .. source.pump)
    --等待，未开阀之前启动泵，使操作的管路产生负压，减少抽取时产生的气泡
    App.Sleep(500);

    -- 打开相关液路的阀门
    err,result = pcall(function() return self.ISolenoidValve:SetValveMap(map) end)

    if not err then -- 出现异常
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        pump:Stop()                                                                  -- 停止泵
        error(result)
    else    --函数调用正常
        if not result then
            pump:Stop()
            return false
        end
    end

    -- 等待泵操作结果事件
    err,result = pcall(function() return pump:ExpectResult(timeout) end)

    if not err then -- 出现异常
        pump:Stop()                                                                      -- 停止泵
        map:SetData(0)
        self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        error(result)
    else    --函数调用正常
        --map:SetData(0)
        --self.ISolenoidValve:SetValveMap(map)                                     --关闭所有阀门
        map = ValveMap.new(setting.liquidType.map.valve4)
        self.ISolenoidValve:SetValveMap(map)                                     --不关闭气体总阀

        if result:GetResult() == PumpOperateResult.Failed then
            error (PumpFailedException:new{liquidType = source, dir = RollDirection.Suck,})
        elseif result:GetResult()  == PumpOperateResult.Stopped then
            error (PumpStoppedException:new{liquidType = source, dir = RollDirection.Suck,})
        elseif result:GetResult()  == PumpOperateResult.Finished then
            ret = true;
        end
    end

    map = nil
    --ConfigLists.SaveMeasureStatus()
    ReagentRemainManager.ReduceReagent(source, volume)
    --flowManager:ClearAllRemainEvent()
    return ret
end

--[[
 * @brief 注射器从逐出室抽出液体并排至废液。
 * @details
 * @details 当出现期望事件等待超时、达到限定体值时，则进行排液后重抽，
 * @details 。
 * @param[in] source 溶液类型。
 * @param[in] vol 溶液体积。
--]]
function Operator:SyringToWaste(vol, speed)

    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()

    local factor = self.IPeristalticPump:GetPumpFactor(2)
    local drainSpeed = setting.liquid.syringeDrainSpeed * factor

    if vol == nil then
        vol = 0
    end
    if speed == nil then
        speed = drainSpeed
    end

    local err,result = pcall(function()
        self:SyringeDrainNotOpenValve(setting.liquidType.syringeStove, vol, drainSpeed)
    end)
    if not err then -- 出现异常
        if type(result) == "userdata" then
            if result:GetType() == "ExpectEventTimeoutException" then          --事件超时异常。
                log:warn(result:What())
            else
                error(result)
            end
        elseif type(result) == "table" then
            if getmetatable(result) == PumpFailedException then
                --elseif getmetatable(result) == PumpStoppedException then
                --    error (PumpFailedException:new{liquidType = setting.liquidType.syringeWaste, dir = RollDirection.Suck,})
            else
                error(UserStopException:new())
            end
        end
    end

    flowManager:ClearAllRemainEvent()

end

--[[
 * @brief 注射器复位点检测
--]]
function Operator:SyringReset(reagent)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()
    local source = setting.liquidType.reagent1
    if reagent ~= nil then
        source = reagent
    end
    local ret = false
    local index
    if source == setting.liquidType.reagent1 then
        index  = setting.liquidType.reagent1.pump
        log:debug("[试剂一注射器]")
    elseif source == setting.liquidType.reagent2 then
        index = setting.liquidType.reagent2.pump
        log:debug("[试剂二注射器]")
    else
        log:debug("Invalid Sensor Input")
        return
    end

    for i = 1,5 do
        if op:SyringeGetSenseStatus(index) then
            ret = true
            break
        end
    end

    if ret == true then
    elseif ret == false then
        --log:info("注射器传感器 未被遮挡")
        log:debug("[注射器复位]")
        local err,result = pcall(function()
            self:SyringeDrainNotOpenValve(source, setting.liquid.syringeResetVolume, 50)
        end)
        if not err then -- 出现异常
            if type(result) == "userdata" then
                if result:GetType() == "ExpectEventTimeoutException" then          --事件超时异常。
                    log:warn(result:What())
                else
                    error(result)
                end
            elseif type(result) == "table" then
                if getmetatable(result) == PumpFailedException then
                    error (PumpFailedException:new{liquidType = setting.liquidType.syringeWaste, dir = RollDirection.Drain,})
                    --elseif getmetatable(result) == PumpStoppedException then
                    --    error (PumpFailedException:new{liquidType = setting.liquidType.syringeWaste, dir = RollDirection.Drain,})
                else
                    error(UserStopException:new())
                end
            end
        end
    end
    flowManager:ClearAllRemainEvent()
end

--[[
 * @brief 注射器更新，进样复位点检测，然后执行重复抽排动作
 * @param[in] count 执行抽排的次数
 * @noted 注射器执行更新动作时，注射器必须处于排废液位置，否则大量液体注入高温燃烧炉会导致发生爆炸
--]]
function Operator:SyringUpdate(count, vol)

    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()
    local ret = false
    local suckVolume = setting.liquid.syringeCleanVolume
    if vol ~= nil and vol < setting.liquid.syringeCleanVolume then
        suckVolume = vol
    end

    --检查是否在复位点，避免往燃烧炉排废液爆炸
    local factor = self.IPeristalticPump:GetPumpFactor(2)
    local drainSpeed = setting.liquid.syringeSlowDrainSpeed * factor

    if(count >= setting.liquid.syringeUpdateUpperLimit) then
        count = setting.liquid.syringeUpdateUpperLimit
    end

    --注射器复位状态检测
    local err,result = pcall(function()
        op:SyringReset()
    end)
    if not err then -- 出现异常
        error(result)
    end

    local motionParam = MotionParam.new()
    local acceleration = setting.liquid.syringeSuckAcceleration
    motionParam =  self.IPeristalticPump:GetMotionParam(2)
    if math.abs(motionParam:GetAcceleration() - acceleration) > 0.001 then
        motionParam:SetAcceleration(acceleration)
        motionParam:SetSpeed(drainSpeed)
        log:debug("[重设注射泵参数]： Set Speed " .. drainSpeed .. ", Set Acceleration " .. acceleration)
        self.IPeristalticPump:SetMotionParam(2, motionParam)
    end

    log:debug("[注射器更新]： " .. count .. "次")

    for i = 1, count-1 do

        local err,result = pcall(function()
            self:SyringeSuck(setting.liquidType.syringeBlank , suckVolume, drainSpeed/4)
            App.Sleep(200)
        end)

        if not err then -- 出现异常
            if type(result) == "userdata" then
                if result:GetType() == "ExpectEventTimeoutException" then          --事件超时异常。
                    log:warn(result:What())
                else
                    error(result)
                end
            elseif type(result) == "table" then
                if getmetatable(result) == PumpFailedException then
                    error (PumpFailedException:new{liquidType = setting.liquidType.syringeBlank, dir = RollDirection.Suck,})
                    --elseif getmetatable(result) == PumpStoppedException then
                    --    error (PumpFailedException:new{liquidType = setting.liquidType.syringFromDigestionRoom, dir = RollDirection.Suck,})
                else
                    error(UserStopException:new())
                end
            end
        end

        local err,result = pcall(function()
            self:SyringeDrain(setting.liquidType.syringeWaste, setting.liquid.syringeResetVolume, drainSpeed)
            App.Sleep(200)
        end)

        if not err then -- 出现异常
            if type(result) == "userdata" then
                if result:GetType() == "ExpectEventTimeoutException" then          --事件超时异常。
                    log:warn(result:What())
                else
                    error(result)
                end
            elseif type(result) == "table" then
                if getmetatable(result) == PumpFailedException then
                    error (PumpFailedException:new{liquidType = setting.liquidType.syringeWaste, dir = RollDirection.Drain,})
                    --elseif getmetatable(result) == PumpStoppedException then
                    --    error (PumpFailedException:new{liquidType = setting.liquidType.syringeWaste, dir = RollDirection.Drain,})
                else
                    error(UserStopException:new())
                end
            end
        end

    end

    local err,result = pcall(function()
        self:SyringeSuck(setting.liquidType.syringeBlank , suckVolume, drainSpeed/4)
        App.Sleep(200)
    end)

    if not err then -- 出现异常
        if type(result) == "userdata" then
            if result:GetType() == "ExpectEventTimeoutException" then          --事件超时异常。
                log:warn(result:What())
            else
                error(result)
            end
        elseif type(result) == "table" then
            if getmetatable(result) == PumpFailedException then
                error (PumpFailedException:new{liquidType = setting.liquidType.blank, dir = RollDirection.Suck,})
                --elseif getmetatable(result) == PumpStoppedException then
                --    error (PumpFailedException:new{liquidType = setting.liquidType.syringFromDigestionRoom, dir = RollDirection.Suck,})
            else
                error(UserStopException:new())
            end
        end
    end

    local err,result = pcall(function()
        self:SyringeDrainNotOpenValve(setting.liquidType.syringeWaste, suckVolume * 0.5, drainSpeed)
        App.Sleep(200)
    end)

    if not err then -- 出现异常
        if type(result) == "userdata" then
            if result:GetType() == "ExpectEventTimeoutException" then          --事件超时异常。
                log:warn(result:What())
            else
                error(result)
            end
        elseif type(result) == "table" then
            if getmetatable(result) == PumpFailedException then
                error (PumpFailedException:new{liquidType = setting.liquidType.syringeWaste, dir = RollDirection.Drain,})
                --elseif getmetatable(result) == PumpStoppedException then
                --    error (PumpFailedException:new{liquidType = setting.liquidType.syringeWaste, dir = RollDirection.Drain,})
            else
                error(UserStopException:new())
            end
        end
    end

end

--[[
 * @brief 基线判定
 * @details
 * @details 当出现期望事件等待超时、达到限定体值时，则进行排液后重抽，
 * @details
 * @param[in] source 溶液类型。
 * @param[in] vol 溶液体积。
--]]
function Operator:BaseLineCheck()
    --等待稳定
    local runAction = Helper.Status.SetAction(setting.runAction.measure.baseLineCheck)
    StatusManager.Instance():SetAction(runAction)
    local warnLimit = 900
    while true do
        if op:IsReachSteady(setting.measureResult.baseLineNum*4) == false then
            if not Measurer.flow:Wait(2) then
                break
            end
        else
            runAction = Helper.Status.SetAction(setting.runAction.measure.reactTime)
            StatusManager.Instance():SetAction(runAction)
            break
        end
        warnLimit = warnLimit - 1
        if warnLimit < 0 then
            log:warn("基线不稳定");
            local alarm = Helper.MakeAlarm(setting.alarm.baseLineCheckFailed, "")
            AlarmManager.Instance():AddAlarm(alarm)
            error(DeviceFaultException:new())
            --error(UserStopException:new())
        end
    end
end

function Operator:Wait(seconds)
    local cnt = math.floor(seconds * 1000/200)

    while true do
        if  Flow.isStarted then
            App.Sleep(200)
        else
            return false
        end

        cnt = cnt -1
        if cnt <= 0 then
            return true
        end
    end
end

function Operator:debugPrintf(str)
    --if config.system.debugMode == true then
        log:debug(str)
    --end
end

--[[
 * @brief 重设LED测量周期
--]]
function Operator:ResetLEDMeasurePeriod()
    local period = dc:GetIOpticalAcquire():GetLEDDefaultValue(0)
    if period == 0 then
        period = config.hardwareConfig.measureLed.period.set
    end
    --负数传参为不写入Flash操作
    dc:GetIOpticalAcquire():SetLEDDefaultValue(-period)
    log:debug("重设LED测量周期： " .. period)
end

--[[
 * @brief 获取传感器状态
 * @param[in] index 传感器索引 0-注射器传感器 1-进样传感器。
 * detail 传感器正常状态下：注射器被遮挡时处于零点处，进样器被遮挡处于非零点处
 * @return 1-未遮挡 2-遮挡
--]]
function Operator:SyringeGetSenseStatus(index)
    local ret = dc:GetIPeristalticPump():GetSenseStatus(index)
    if ret == 2 then
        return true
    else
        --log:debug("Syringe Sensor status " .. ret)
        return false
    end
end


--[[
 * @brief 开采样泵
 * @param[in]speed - ml/min
 * factor:泵校准系数，影响流速的直接因素
 * detail:
 *         单位转换[ml/min  -->> step/s], [step/s] = (ml/factor) / 60,
 *         单位转换[ml/min  -->> ml/s], [ml/s] = ml / 60,
 *         Drain单位[ml/s]
 * @return
--]]
function Operator:StartSamplePump(speed)
    local flowSpeed = speed
    if flowSpeed == nil then
        flowSpeed = 1 --ml/min
    end
    local drainSpeed = flowSpeed / 60
    log:debug("{StartSamplePump}")
    op:DrainNoEvent(setting.liquidType.sampleTC, 10000, drainSpeed) -- DrainNoEvent  PumpNoEvent
    op:DrainNoEvent(setting.liquidType.sampleIC, 10000, drainSpeed)
end

function Operator:StopSamplePump()
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()

    local map = ValveMap.new(setting.liquidType.sampleTC.valve)
    local pump = pumps[setting.liquidType.sampleTC.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象

    log:debug("{StopSamplePump}")

    if pump:GetStatus() == 2 then
        pump:Stop()
    else
        log:debug("{Stop Sample TC Pump Failed, Pump Idle}")
    end

    pump = pumps[setting.liquidType.sampleIC.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象

    if pump:GetStatus() == 2 then
        pump:Stop()
    else
        log:debug("{Stop Sample IC Pump Failed, Pump Idle}")
    end

end

--[[
 * @brief 查询注射器试剂余量
 * @param[in]speed - ml/min
 * factor:泵校准系数，影响流速的直接因素
 * detail:
 * @return
--]]
function Operator:ReagentManager(reagentType)
    local currentVol = dc:GetIPeristalticPump():GetPumpOffsetStep(reagentType.pump)
    log:debug("当前试剂余量 " .. string.format("%.2f", currentVol) .. "ul")
    local PRECISE = 1
    if currentVol - 50 < PRECISE then
        if reagentType == setting.liquidType.reagent1 then
            log:debug("酸剂余量不足,当前余量 " .. string.format("%.2f", currentVol) .. " ul")
        else
            log:debug("氧化剂余量不足,当前余量 " .. string.format("%.2f", currentVol) .. " ul")
        end

        op:StartSamplePump(2)

        App.Sleep(1000)

        if currentVol > 0 then
            op:SyringReset(reagentType)
        end

        --检查是否在复位点，避免往燃烧炉排废液爆炸
        local drainSpeed = 500
        local err,result = pcall(function()
            self:SyringeSuck(reagentType , setting.liquid.syringeMaxVolume - currentVol, drainSpeed)
            App.Sleep(200)
        end)

        if not err then -- 出现异常
            if type(result) == "userdata" then
                if result:GetType() == "ExpectEventTimeoutException" then          --事件超时异常。
                    log:warn(result:What())
                else
                    error(result)
                end
            elseif type(result) == "table" then
                if getmetatable(result) == PumpFailedException then
                    error (PumpFailedException:new{liquidType = reagentType, dir = RollDirection.Suck,})
                else
                    error(UserStopException:new())
                end
            end
        end

        drainSpeed = 500
        err,result = pcall(function()
            self:SyringeDrainNotOpenValve(reagentType , setting.liquid.syringeUndoVolume, drainSpeed)
            App.Sleep(200)
        end)

        if not err then -- 出现异常
            if type(result) == "userdata" then
                if result:GetType() == "ExpectEventTimeoutException" then          --事件超时异常。
                    log:warn(result:What())
                else
                    error(result)
                end
            elseif type(result) == "table" then
                if getmetatable(result) == PumpFailedException then
                    error (PumpFailedException:new{liquidType = setting.liquidType.none, dir = RollDirection.Suck,})
                else
                    error(UserStopException:new())
                end
            end
        end
    end
end

--[[
 * @brief 查询注射器试剂余量
 * @param[in]speed - ml/min
 * factor:泵校准系数，影响流速的直接因素
 * detail:
 * @return
--]]
function Operator:StartReagentMix(dest, vol, speed)

    local err,result = pcall(function()
        self:SyringeDrainNotWaitEvent(dest , vol, speed)
        App.Sleep(1000)
    end)

    if not err then -- 出现异常
        if type(result) == "userdata" then
            if result:GetType() == "ExpectEventTimeoutException" then          --事件超时异常。
                log:warn(result:What())
            else
                error(result)
            end
        elseif type(result) == "table" then
            if getmetatable(result) == PumpFailedException then
                error (PumpFailedException:new{liquidType = setting.liquidType.syringeBlank, dir = RollDirection.Suck,})
            else
                error(UserStopException:new())
            end
        end
    end
end


--[[
 * @brief 停止试剂泵
 * @param[in] source - ul/min
 * detail:
 * @return
--]]
function Operator:StopReagentMix(source)
    local flowManager = FlowManager.Instance()
    flowManager:ClearAllRemainEvent()

    local pump = pumps[source.pump + 1]  --  +1 当泵号为0时，从pumps[1]取泵对象

    log:debug("{StopReagentMix}")

    if pump:GetStatus() == 2 then
        pump:Stop()
        App.Sleep(1000)
        flowManager:ClearAllRemainEvent()
    else
        log:debug("{Stop Reagent Mix Failed " .. source.name .. ", Pump Idle}")
    end

end

--[[
 * @brief 设置液路板阀常开
 * @details
 * @param[in] source 阀对应map，支持多个阀同时设置常开
--]]
function Operator:SetLCNormalOpen(source)
    local map = LCValveMap.new(source)
    lc:GetISolenoidValve():SetValveMapNormalOpen(map)
    lc:GetISolenoidValve():SetValveMap(map)
end

--[[
 * @brief 液路板停止状态
 * @details：测量结束，需要打开去离子水和去离子水阀
--]]
function Operator:SetLCStopStatus()
    local map = LCValveMap.new(setting.liquidType.map.valve1 | setting.liquidType.map.valve5)
    lc:GetISolenoidValve():SetValveMap(map)
end

function Operator:SetRCValveOn()
    local map = RCValveMap.new(1 | 2)
    rc:GetISolenoidValve():SetValveMap(map)
    App.Sleep(100)
end

function Operator:SetRCValveOff()
    local map = RCValveMap.new(0)
    rc:GetISolenoidValve():SetValveMap(map)
end

--[[
 * @brief 设置驱动板阀常开
 * @details
 * @param[in] source 阀对应map，支持多个阀同时设置常开
--]]
function Operator:SetDCNormalOpen(source)
    local map = ValveMap.new(source)
    dc:GetISolenoidValve():SetValveMapNormalOpen(map)
    App.Sleep(200)
    dc:GetISolenoidValve():SetValveMap(map)
end



--[[
 * @brief 采集AD(带过滤数据)
 * @details 采集光学测量AD信号
 * @param[in] acquireTime 采集时间  GetTemperature
--]]
function Operator:GetCalibrateTemperatureWithFiltration(acquireTime)
    local readAdTime = 1        --测量间隔
    local referenceAD = {}
    local measureAD = {}
    local temp = 0
    local referenceADSumData = 0
    local measureADSumData = 0
    local filterNum = 0

    filterNum = math.floor(acquireTime/5)

    local retTemperature = RCTemperature.new()
    local temperature = RCTemperature.new()

    -- 读取AD值
    for i = 1,acquireTime,readAdTime do
        temperature = rc:GetCurrentTemperature()
        --referenceAD[i] = temperature:GetThermostatTemp() -- GetDigestTemperature
        --measureAD[i] = temperature:GetEnvironmentTemp()  -- GetEnvironmentTemperature
        referenceAD[i] = rc:GetDigestTemperature()
        measureAD[i] = rc:GetEnvironmentTemperature()
        App.Sleep(1000)
    end

    for i = 1,acquireTime,1 do
        op:debugPrintf("refTemp["..i.."] = "..referenceAD[i].." meaTemp["..i.."] = "..measureAD[i])
    end

    for i = 2, acquireTime, 1 do
        for j = acquireTime,i, -1 do
            if referenceAD[j] < referenceAD[j - 1] then
                temp = referenceAD[j - 1];
                referenceAD[j - 1] = referenceAD[j];
                referenceAD[j] = temp;
            end
            if measureAD[j] < measureAD[j - 1] then
                temp = measureAD[j - 1];
                measureAD[j - 1] = measureAD[j];
                measureAD[j] = temp;
            end
        end
    end

    for i = 1+filterNum,acquireTime-filterNum,1 do
        referenceADSumData = referenceADSumData + referenceAD[i]
        measureADSumData = measureADSumData + measureAD[i]
    end

    local ret1 = math.floor(referenceADSumData/(acquireTime - filterNum*2))
    local ret2 = math.floor(measureADSumData/(acquireTime - filterNum*2))

    retTemperature:SetThermostatTemp(ret1)
    retTemperature:SetEnvironmentTemp(ret2)

    return retTemperature
end



--[[
 * @brief 检查电导池信号值
 * detail gain 1/2对应测量  gain 3/4对应参考
 * @param[in]
--]]
function  Operator:ConfirmLED()
    local ret = false
    local vaLue = 0
    local num = 0
    local setMap = 0

    local gainMap = rc:GetISolenoidValve():GetGainMap()

    local ScanLen = rc:GetScanLen()
    if ScanLen ~= nil then
        num = ScanLen
    end

    local ScanData = rc:GetScanData(num - 1)
    if ScanData ~= nil then
        vaLue = ScanData
    end

    local ScanDataRef = rc:GetScanDataRef(num - 1)

    if ScanData > 1.5 then
        setMap = 2
    elseif ScanData < 0.1 then
        setMap = 1
    else
        setMap = gainMap & (3)
    end

    if ScanDataRef > 1.5 then
        setMap = setMap + 8
    elseif ScanDataRef < 0.1 then
        setMap = setMap + 4
    else
        setMap = setMap + (gainMap & (12))
    end

    log:debug("gainMap " .. gainMap .. ", mea " .. ScanData .. ", ref " .. ScanDataRef)

    if setMap > 0 then
        rc:GetISolenoidValve():SetGainMap(setMap)
        App.Sleep(500)
        ScanData = rc:GetScanData(rc:GetScanLen() - 1)
        local tmp = rc:GetScanDataRef(rc:GetScanLen() - 1)
        if (setMap & 2) > 1 then
            setting.measureResult.isHighRangeTC = true
        else
            setting.measureResult.isHighRangeTC = false
        end

        if (setMap & 8) > 1 then
            setting.measureResult.isHighRangeIC = true
        else
            setting.measureResult.isHighRangeIC = false
        end

        log:debug("设置增益数值" .. setMap .. ",修改前TC信号值 = " .. vaLue .. "V, 修改后TC信号值 = " .. ScanData .. "V")
        log:debug("设置增益数值" .. setMap .. ",修改前IC信号值 = " .. ScanDataRef .. "V, 修改后IC信号值 = " .. tmp .. "V")
    end

    return ret

end

--[[
 * @brief 基线稳定判断
 * detail 检查当前索引值往前连续10个数值是否在 minDeviation 范围内波动
 * @param[in]
--]]
function Operator:IsReachSteady(value, validCnt, isCheck)

    local index = 1
    if value == nil or value < 30 then
        value = 30
    end

    if isCheck == true then
        index = 0
    end

    local period = setting.ui.profile.hardwareParamIterms[9][1].get()
    if period == 0 then
        period = config.hardwareConfig.measureLed.period.set
    end
    value =  math.floor(1000/period) * value
    print("value " .. value)

    local useValidCnt = math.floor(140 / tonumber(period) * setting.measureResult.validCnt)
    if validCnt ~= nil then
        useValidCnt = validCnt
    end

    print("period " .. period)
    local step = math.floor(setting.measureResult.areaStep / tonumber(period) * 1000)
    --自增量 = 周期系数
    local increment =  15 * 3 --math.floor(140 / tonumber(period) * 15)
    --滤波数量为25 * 2 共50个
    local filterStep = 25
    --数据前后各去掉10个点
    local throwNum = 3
    return dc:IsReachSteady(value, 15, step, increment, filterStep, throwNum, 0)
end

--status 清空数组
function Operator:Calculatepeak(startIndex, endIndex, validCnt , status, measureType)
    local area = -1
    local useValidCnt = setting.measureResult.validCnt
    local period = 200
    local step = 60
    local isExtra = false
    if measureType ~= ModelType.TC then
        isExtra = true
    end

    if validCnt ~= nil then
        useValidCnt = validCnt
    end
    if period == nil or period == 0 then
        period = config.hardwareConfig.measureLed.period.set
    end

    local increment = math.floor(140 / tonumber(period) * 20)
    --滤波数量为25 * 2 共50个
    local filterStep = 5
    --数据前后各去掉10个点
    local throwNum = 2

    print("period " .. period)
    step = math.floor(setting.measureResult.areaStep / tonumber(period) * 1000)

    if endIndex - startIndex > 50  then
        area = rc:NDIRResultHandle(startIndex, endIndex, useValidCnt, step, increment, filterStep, throwNum, isExtra)
        if area <= setting.measureResult.areaLowLimit then
            area = setting.measureResult.areaLowLimit + setting.measureResult.areaLowLimit*(((math.random()- 0.5)*2)*0.05)
        end
    else
        error(MeasureDataException:new())    --用户停止
        log:warn("数据长度异常")
    end
    if status == nil then
        rc:ClearBuf()
    end

    if measureType == ModelType.TC then
        if setting.measureResult.isHighRangeTC then
            area = 9.29 * area
            log:debug("TC高量程模式")
        end
    else
        if setting.measureResult.isHighRangeIC then
            area = 9.29 * area
            log:debug("IC高量程模式")
        end
    end

    return area
end


--[[
* @brief 获取信号峰值
 * @param[result] 测量结果表，包含开始标记点、结束标记点
 * @param[mtype] 用于区分电导池 ModelType.TC / ModelType.IC
]]--
function Operator:SearchPeak(result, mtype)
    local constant = 1
    local temp = 0
    local peak = 0
    local peakTable ={}
    local peakTableCal ={}
    local peakTableTemp = {}
    local reviser = 1
    local EC = 0
    local T25EC = 0
    local startIndex = result.startIndex
    local endIndex = result.endIndex
    local max = 0
    local gainValue = 1
    local debug = config.system.debugMode
    if setting.measureResult.isHighRangeTC and mtype == ModelType.TC then
        gainValue = 9.29
        log:debug("TC高量程模式")
    end
    if setting.measureResult.isHighRangeIC and mtype == ModelType.IC then
        gainValue = 9.29
        log:debug("IC高量程模式")
    end

    if mtype == ModelType.TC then
        constant = config.measureParam.TCConstant
        for i = startIndex,endIndex do
            peak = rc:GetScanData(i) * gainValue
            temp = rc:GetMeaTemp(i)
            reviser = -3.16345*10^(-8) * temp^3 + 1.25933*10^(-5) * temp^2 + 5.26393*10^(-4)* temp + 0.03193
            EC = peak * constant * 2
            T25EC = 1 + reviser * (temp - 25)
            table.insert(peakTable, (EC/T25EC))
        end

        for k,v in pairs(peakTable) do
            local average = v
            average = peakTable[k] -  status.measure.blankECTableTC[k] + 0.055
            table.insert(peakTableTemp, average)
            if k>5 then
                average = (average + peakTableTemp[k-1] + peakTableTemp[k-2] + peakTableTemp[k-3] + peakTableTemp[k-4] )/5
            end

            if max < average and k > 30 and k < 80 then
                max = average
            end
            table.insert(peakTableCal, average)
            if debug then
                log:debug("TC补偿后电导率[" .. k .. "] = " .. tonumber(v) .. ", 平均处理后电导率 = " .. tonumber(peakTableCal[k]))
            end
        end
    else
        constant = config.measureParam.ICConstant
        for i = startIndex,endIndex do
            peak = rc:GetScanDataRef(i) * gainValue
            temp = rc:GetRefTemp(i)
            reviser = -3.16345*10^(-8) * temp^3 + 1.25933*10^(-5) * temp^2 + 5.26393*10^(-4)* temp + 0.03193
            EC = peak * constant * 2
            T25EC = 1 + reviser * (temp - 25)
            table.insert(peakTable, (EC/T25EC))
        end

        for k,v in pairs(peakTable) do
            local average = v
            average = peakTable[k] -  status.measure.blankECTableIC[k] + 0.055
            table.insert(peakTableTemp, average)
            if k>5 then
                average = (average + peakTableTemp[k-1] + peakTableTemp[k-2] + peakTableTemp[k-3] + peakTableTemp[k-4] )/5
            end

            if max < average and k > 30 and k < 80 then
                max = average
            end
            table.insert(peakTableCal, average)
            if debug then
                log:debug("IC补偿后电导率[" .. k .. "] = " .. tonumber(v) .. ", 平均处理后电导率 = " .. tonumber(peakTableCal[k]))
            end
        end
    end

    log:debug("电导率峰值 " .. max)
    return max
end

--[[
* @brief 获取峰值温度
 * @param[startIndex] 标记开始点
 * @param[endIndex]   标记结束点
 * @param[measureType] 用于区分电导池 ModelType.TC / ModelType.IC
]]--
function Operator:GetPeakTemperature(startIndex, endIndex, measureType)
    local peakTemperature = 1
    local isExtra = false
    if measureType ~= ModelType.TC then
        isExtra = true
    end

    if endIndex - startIndex > 20  then
        peakTemperature = rc:GetPeakTemperature(startIndex, endIndex, isExtra)
    else
        error(MeasureDataException:new())    --用户停止
        log:warn("数据长度异常")
    end

    return peakTemperature
end

--[[
* @brief Turbo模式计算平均信号和平均温度
 * @param[startIndex] 标记开始点
 * @param[endIndex]   标记结束点
 * @param[measureType] 用于区分电导池 ModelType.TC / ModelType.IC

]]--
function Operator:TurboCalculatepeak(startIndex, endIndex,measureType)
    local sumTemp = 0
    local sumPeak = 0
    local avgTemp = 0
    local avgPeak = 0
    local tempValue = 0
    local peakValue = 0
    local debugMode = config.system.debugMode
    if measureType == ModelType.IC then
        for i = startIndex, endIndex - 1 do
            tempValue = rc:GetRefTemp(i)
            peakValue = rc:GetScanDataRef(i)
            sumTemp = sumTemp + tempValue
            sumPeak = sumPeak + peakValue
            if debugMode then
                log:debug("peakValueTC = " .. peakValue .. " ,tempValueTC = " .. tempValue)
            end
        end
        if endIndex ~= startIndex then
            avgTemp = sumTemp / (endIndex - startIndex)
            avgPeak = sumPeak / (endIndex - startIndex)
            if debugMode then
                log:debug("avgPeakTC = " .. avgPeak .. " ,avgTempTC = " .. avgTemp)
            end
        end
        if setting.measureResult.isHighRangeIC then
            avgPeak = 9.29 * avgPeak
            log:debug("IC高量程模式")
        end
    else
        for i = startIndex, endIndex - 1 do
            tempValue = rc:GetMeaTemp(i)
            peakValue = rc:GetScanData(i)
            sumTemp = sumTemp + tempValue
            sumPeak = sumPeak + peakValue
            if debugMode then
                log:debug("peakValueIC = " .. peakValue .. " ,tempValueIC = " .. tempValue)
            end
        end
        if endIndex ~= startIndex then
            avgTemp = sumTemp / (endIndex - startIndex)
            avgPeak = sumPeak / (endIndex - startIndex)
            if debugMode then
                log:debug("avgPeakIC = " .. avgPeak .. " ,avgTempIC = " .. avgTemp)
            end
            if setting.measureResult.isHighRangeTC then
                avgPeak = 9.29 * avgPeak
                log:debug("TC高量程模式")
            end
        end
    end

    return avgPeak, avgTemp

end

function Operator:AccurateSubMeasure(resultData, currentCnt)


    --停止更新基线状态
    status.measure.isCheckBaseLine = false
    ConfigLists.SaveMeasureStatus()

    rc:ClearBuf()

    resultData.startIndex = rc:GetScanLen()   --标记开始
    log:debug("精准模式第 " .. currentCnt .. "次标记开始： ".. resultData.startIndex);

    --if not Measurer.flow:Wait(30) then
    --    error(UserStopException:new())
    --end

    if not Measurer.flow:Wait(5) then
        error(UserStopException:new())
    end

end

--[[
 * @brief 精准测量模式
 * @param[in] resultData    Measurer.measureResult数据入口，用于给startIndex,ednIndex,peak赋值。
 * @param[in] 精准测量动作流程: 滑块复位->等待第一次测量结果->精准模式填充管路(剩余体积定量到下定量点)->相同的加样流程
 * @param[in] 如果修改普通测量加样流程，就必须同步修改精准测量模式/校准加样流程，确保两者流程一致
--]]
function Operator:AccurateMeasure(resultData)
    local runAction = Helper.Status.SetAction(setting.runAction.measure.accurateMeasureMode)
    StatusManager.Instance():SetAction(runAction)

    if not Measurer.flow:Wait(config.measureParam.reacTime/2) then
        error(UserStopException:new())
    end
    Measurer:CalibrateMeasureEndJudge(config.measureParam.reacTime/2)
    --if not Measurer.flow:Wait(40) then
    --    error(UserStopException:new())
    --end

    resultData.endIndex = rc:GetScanLen()   --标记结束
    log:debug("精准模式标记结束： " .. resultData.endIndex);
    resultData.accurateArea1 = op:Calculatepeak(resultData.startIndex, resultData.endIndex, nil, nil, ModelType.TC)
    log:debug("精准模式第 " .. 1 .." 次面积： ".. resultData.accurateArea1)
    resultData.endTemperature = dc:GetReportThermostatTemp(setting.temperature.temperatureRefrigerator)
    log:debug("测量结束制冷温度： " .. resultData.endTemperature);

    --------精准测量第二次管路填充&加样---------
    self:AccurateSubMeasure(resultData, 2)

    if not Measurer.flow:Wait(config.measureParam.reacTime/2) then
        error(UserStopException:new())
    end
    Measurer:CalibrateMeasureEndJudge(config.measureParam.reacTime/2)
    --if not Measurer.flow:Wait(40) then
    --    error(UserStopException:new())
    --end

    resultData.endIndex = rc:GetScanLen()   --标记结束
    log:debug("精准模式标记结束： " .. resultData.endIndex);
    resultData.accurateArea2 = op:Calculatepeak(resultData.startIndex, resultData.endIndex, nil, nil, ModelType.TC)
    log:debug("精准模式第 " .. 2 .." 次面积： ".. resultData.accurateArea2)
    resultData.endTemperature = dc:GetReportThermostatTemp(setting.temperature.temperatureRefrigerator)
    log:debug("测量结束制冷温度： " .. resultData.endTemperature);

    local deviation12, deviation23, deviation13
    deviation12 = math.abs(resultData.accurateArea2 - resultData.accurateArea1)/((resultData.accurateArea2 + resultData.accurateArea1)/2)
    log:debug("精准测量面积1 =  " .. resultData.accurateArea1 .. ", 面积2 = " .. resultData.accurateArea2 ..
            ", 偏差阈值 = " .. config.measureParam.accurateMeasureDeviation .. ", 精准测量面积12偏差 = " .. deviation12)
    if deviation12 < config.measureParam.accurateMeasureDeviation then
        resultData.peak = (resultData.accurateArea2 + resultData.accurateArea1)/2
        setting.measureResult.immediatelyResultHandle = true
        setting.measureResult.isFinishAccurateMeasure = true
        return 0
    end
    --------精准测量第二次结束---------


    --------精准测量第三次管路填充&加样---------
    self:AccurateSubMeasure(resultData, 3)

    if not Measurer.flow:Wait(config.measureParam.reacTime/2) then
        error(UserStopException:new())
    end
    Measurer:CalibrateMeasureEndJudge(config.measureParam.reacTime/2)
    --if not Measurer.flow:Wait(40) then
    --    error(UserStopException:new())
    --end

    resultData.endIndex = rc:GetScanLen()   --标记结束
    log:debug("精准模式标记结束： " .. resultData.endIndex);
    resultData.accurateArea3 =  op:Calculatepeak(resultData.startIndex, resultData.endIndex, nil, nil, ModelType.TC)
    log:debug("精准模式第 " .. 3 .." 次面积： ".. resultData.accurateArea3)
    resultData.endTemperature = dc:GetReportThermostatTemp(setting.temperature.temperatureRefrigerator)
    log:debug("测量结束制冷温度： " .. resultData.endTemperature);

    deviation23 = math.abs(resultData.accurateArea3 - resultData.accurateArea2)/((resultData.accurateArea3 + resultData.accurateArea2)/2)
    deviation13 = math.abs(resultData.accurateArea3 - resultData.accurateArea1)/((resultData.accurateArea3 + resultData.accurateArea1)/2)
    log:debug("精准测量面积2 =  " .. resultData.accurateArea2 .. ", 面积3 = " .. resultData.accurateArea3 ..
            ", 偏差阈值 = " .. config.measureParam.accurateMeasureDeviation .. ", 精准测量面积23偏差 = " .. deviation23)
    log:debug("精准测量面积1 =  " .. resultData.accurateArea1 .. ", 面积3 = " .. resultData.accurateArea3 ..
            ", 偏差阈值 = " .. config.measureParam.accurateMeasureDeviation .. ", 精准测量面积13偏差 = " .. deviation13)
    if deviation23 < config.measureParam.accurateMeasureDeviation
            or deviation13 < config.measureParam.accurateMeasureDeviation  then
        setting.measureResult.immediatelyResultHandle = true
        setting.measureResult.isFinishAccurateMeasure = true
        if deviation23 < config.measureParam.accurateMeasureDeviation then
            resultData.peak = (resultData.accurateArea3 + resultData.accurateArea2)/2
        else
            resultData.peak = (resultData.accurateArea3 + resultData.accurateArea1)/2
        end

        return 0
    end

    self:AccurateSubMeasure(resultData, 4)
    setting.measureResult.isFinishAccurateMeasure = true
end

function Operator:CurrentOperate(index,  consistency)
    local upLimit = config.interconnection.meaUpLimit
    local downLimit = config.interconnection.meaLowLimit
    local currentValue = 0;

    local k = 16/(upLimit-downLimit);
    local b = 4-k*downLimit;
    currentValue = k*consistency + b;

    oc:GetIOutputControl():SetOutputCurrent(index, currentValue)
end