

UpdateType =
{
    DC =
    {
        name = "驱动板",
    },
    MC =
    {
        name = "主控板",
    },
}

--[[
@brief 测量流程异常。
--]]
MeasureFlowException = Exception:new
{
    message = "MeasureFlowException",
}

--[[
@brief 量程错误异常。
--]]
MeasureRangeWrongException = MeasureFlowException:new
{
    message = "MeasureRangeWrongException",
}

--[[
@brief 校准结果错误异常。
--]]
CalibrateResultWrongException = MeasureFlowException:new
{
    message = "CalibrateResultWrongException",
}

--[[
@brief 校准结果错误异常。
--]]
CurveCheckWrongException = MeasureFlowException:new
{
    message = "CurveCheckWrongException",
}

--[[
@brief 快速量程错误异常。
--]]
FastMeasureRangeWrongException = MeasureFlowException:new
{
    message = "FastMeasureRangeWrongException",
    consistency = 0,
    currange = 0,
}

--[[
@brief 仪器故障异常。
--]]
DeviceFaultException = MeasureFlowException:new
{
    message = "DeviceFaultException",
}

--[[
@brief 用户停止异常。
--]]
UserStopException = MeasureFlowException:new
{
    message = "UserStopException",
}

--[[
@brief 升级异常。
--]]
UpdateException = MeasureFlowException:new
{
    message = "UpdateException",
    updateType = UpdateType.DC,
}

--[[
@brief 排消解液异常。
--]]
DrainFromDigestionException = MeasureFlowException:new
{
    message = "DrainFromDigestionException",
}

--[[
@brief 泵校准异常。
--]]
CalibratePumpException = MeasureFlowException:new
{
    message = "CalibratePumpException",
}

--[[
@brief 水样超上限异常。
--]]
MeasureResultOutUpLimitException = MeasureFlowException:new
{
    message = "MeasureResultOutUpLimitException",
}

--[[
@brief 水样超下限异常。
--]]
MeasureResultOutLowLimitException = MeasureFlowException:new
{
    message = "MeasureResultOutLowLimitException",
}

--[[
@brief 消解室加液异常
--]]
AddLiquidToDigestionException = MeasureFlowException:new
{
    message = "AddLiquidToDigestionException",
}

--[[
@brief 测量数据异常
--]]
MeasureDataException = MeasureFlowException:new
{
    message = "MeasureDataException",
}

--[[
@brief 测量信号异常
--]]
MeasureLedException = MeasureFlowException:new
{
    message = "MeasureLedException",
}

--[[
@brief 载气调节异常
--]]
AirFlowException = MeasureFlowException:new
{
    message = "AirFlowException",
}
