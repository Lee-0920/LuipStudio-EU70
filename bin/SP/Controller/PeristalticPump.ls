--[[
 * @brief 单个蠕动泵。
 * @details 对单个蠕动泵的功能进行封装。
--]]

PeristalticPump =
{
    index = 0,
    isRunning = false,
    peristalticPumpInterface =  0,
}

function PeristalticPump:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    self.__metatable = "PeristalticPump"
    return o
end

function PeristalticPump:GetFactor()
    return self.peristalticPumpInterface:GetPumpFactor(self.index);
end

function PeristalticPump:SetFactor(factor)
    return  self.peristalticPumpInterface:SetPumpFactor(self.index, factor)

end

function PeristalticPump:GetMotionParam()
    return self.peristalticPumpInterface:GetMotionParam(self.index)
end

function PeristalticPump:SetMotionParam(param)
    return self.peristalticPumpInterface:SetMotionParam(self.index, param)
end

function PeristalticPump:GetStatus()
    return self.peristalticPumpInterface:GetPumpStatus(self.index)
end

function PeristalticPump:Start(dir, volume, speed)
    self.isRunning = true;
    return self.peristalticPumpInterface:StartPump(self.index, dir, volume, speed)
end

function PeristalticPump:Stop()
    self.isRunning = false;
    return self.peristalticPumpInterface:StopPump(self.index)
end

function PeristalticPump:GetVolume()
    return self.peristalticPumpInterface:GetPumpVolume(self.index)
end

function PeristalticPump:ExpectResult(timeout)
    local  pumpResult = PumpResult.new()
    pumpResult:SetIndex(0)
    pumpResult:SetResult(PumpOperateResult.Failed)

    pumpResult = self.peristalticPumpInterface:ExpectPumpResult(timeout)

    self.isRunning = false

    return pumpResult
end
