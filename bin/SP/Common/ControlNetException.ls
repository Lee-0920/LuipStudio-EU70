
--[[
@brief 表示在LUA程序执行期间发生的错误。
@details 此类是只在LUA使用的异常的基类。当发生错误时，
系统或当前正在执行的应用程序通过引发包含关于该错误的信息的异常来报告错误。
异常发生后，将由该应用程序或默认异常处理程序进行处理。
--]]
Exception =
{
    message = "Exception",
}

function Exception:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function Exception:What()
    return self.message
end


--[[
@brief 控制器网络异常。
--]]
ControlNetException = Exception:new
{
    message = "ControlNetException",
}

--[[
@brief 泵操作异常。
--]]
PumpException = ControlNetException:new
{
    message = "泵操作异常",
    liquidType = setting.liquidType.none, --溶液类型。
    dir = RollDirection.Suck,  --蠕动泵旋转方向。
}


--[[
@brief 泵操作中途出现故障，未能完成异常。
--]]
PumpFailedException = PumpException:new
{
    message = "PumpFailedException",
}


--[[
@brief 泵操作被停止异常。
--]]
PumpStoppedException = PumpException:new
{
    message = "PumpStoppedException",
}

--[[
@brief 精确定量操作异常。
--]]
MeterException = ControlNetException:new
{
    message = "MeterException",
    liquidType = setting.liquidType.none, --溶液类型。
    What = function(self)
        return self.message .. " Liquid:" .. tostring(self.liquidType.name)
    end
}

--[[
@brief 定量中途出现故障，未能完成异常。
--]]
MeterFailedException = MeterException:new
{
    message = "MeterFailedException",
}

--[[
@brief 定量被停止异常。
--]]
MeterStoppedException = MeterException:new
{
    message = "MeterStoppedException",
}

--[[
@brief 定量溢出异常。
--]]
MeterOverflowException = MeterException:new
{
    message = "MeterOverflowException",
}

--[[
@brief 定量目标未达成异常。
--]]
MeterUnfinishedException = MeterException:new
{
    message = "MeterUnfinishedException",
}

--[[
@brief 定量时产生气泡。
--]]
MeterAirBubbleException = MeterException:new
{
    message = "MeterAirBubbleException",
}

--[[
@brief 定量时期望事件超时。
--]]
MeterExpectTimeoutException = MeterException:new
{
    message = "MeterExpectTimeoutException",
}

--[[
@brief 恒温操作异常。
--]]
ThermostatException = ControlNetException:new
{
    message = "恒温异常",
    mode = 0,  -- 恒温模式。
    targetTemp = 0,
    toleranceTemp = 0,
    timeout = 0,
    What = function(self)
        local str = self.message .. " Mode:" .. tostring(self.mode) .. ", Target:" .. tostring(self.targetTemp)
                .. ", Tolerance:" .. tostring(self.toleranceTemp).. ", Timeout=:" .. tostring(self.timeout)
        return str
    end,
}

--[[
@brief 恒温中途出现故障，未能完成异常。
--]]
ThermostatFailedException = ThermostatException:new
{
    message = "恒温失败异常",
}

--[[
@brief 恒温被停止异常。
--]]
ThermostatStoppedException = ThermostatException:new
{
    message = "恒温被停止",
}

--[[
@brief 恒温超时，指定时间内仍未达到目标温度异常。
--]]
ThermostatTimeoutException = ThermostatException:new
{
    message = "恒温超时异常",
}

--[[
@brief 维护异常。
--]]
MaintainException = ControlNetException:new
{
    message = "MaintainException",
}

--[[
@brief 试剂余量异常。
--]]
ReagentRemainWarn = MaintainException:new
{
    message = "ReagentRemainWarn",
    liquidType = setting.liquidType.none, --溶液类型。
    What = function(self)
        return self.message .. " Liquid:" .. tostring(self.liquidType.name)
    end
}

--[[
@brief 耗材过期异常。
--]]
MaterialGoBadException = MaintainException:new
{
    message = "MaterialGoBadWarn",
    materialType = setting.materialType.none ,
    What = function(self)
        return self.message .. " Material:" .. tostring(self.materialType.name)
    end
}

--[[
@brief 光学采集异常。
--]]
AcquirerADException = ControlNetException:new
{
    message = "光学采集异常",
}


--[[
@brief 光学采集中途出现故障，未能完成异常。
--]]
AcquirerADFailedException = AcquirerADException:new
{
    message = "AcquirerADFailedException",
}


--[[
@brief 光学采集被停止异常。
--]]
AcquirerADStoppedException = AcquirerADException:new
{
    message = "AcquirerADStoppedException",
}

--[[
@brief 测量模块调节异常
--]]
MeasureLEDControlException = ControlNetException:new
{
    message = "MeasureLEDControlException",
}
--[[
@brief 测量模块调节失败异常
--]]
MeasureLEDControlFailedException = MeasureLEDControlException:new
{
    message = "MeasureLEDControlFailedException",
}


--[[
@brief 测量模块调节被停止异常
--]]
MeasureLEDControlStoppedException = MeasureLEDControlException:new
{
    message = "MeasureLEDControlStoppedException",
}

--[[
@brief 静态AD调节异常
--]]
StaticADControlException = ControlNetException:new
{
    message = "StaticADControlException",
}

--[[
@brief 静态AD调节失败异常
--]]
StaticADControlFailedException = StaticADControlException:new
{
    message = "StaticADControlFailedException",
}


--[[
@brief 静态AD调节被停止异常
--]]
StaticADControlStoppedException = StaticADControlException:new
{
    message = "StaticADControlStoppedException",
}


