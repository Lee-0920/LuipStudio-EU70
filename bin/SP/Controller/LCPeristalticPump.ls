--[[
 * @brief 单个蠕动泵。
 * @details 对单个蠕动泵的功能进行封装。
--]]

LCPeristalticPump =
{
    index = 0,
    isRunning = false,
    peristalticPumpInterface =  0,
}

ExOffsetIndex = 4

function LCPeristalticPump:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    self.__metatable = "LCPeristalticPump"
    return o
end

function LCPeristalticPump:GetFactor()
    return self.peristalticPumpInterface:GetPumpFactor(self.index - ExOffsetIndex);
end

function LCPeristalticPump:SetFactor(factor)
    return  self.peristalticPumpInterface:SetPumpFactor(self.index - ExOffsetIndex, factor)

end

function LCPeristalticPump:GetMotionParam()
    return self.peristalticPumpInterface:GetMotionParam(self.index - ExOffsetIndex)
end

function LCPeristalticPump:SetMotionParam(param)
    return self.peristalticPumpInterface:SetMotionParam(self.index - ExOffsetIndex, param)
end

function LCPeristalticPump:GetStatus()
    return self.peristalticPumpInterface:GetPumpStatus(self.index - ExOffsetIndex)
end

function LCPeristalticPump:Start(dir, volume, speed)
    self.isRunning = true;
    return self.peristalticPumpInterface:StartPump(self.index - ExOffsetIndex, dir, volume, speed)
end

function LCPeristalticPump:Stop()
    self.isRunning = false;
    return self.peristalticPumpInterface:StopPump(self.index - ExOffsetIndex)
end

function LCPeristalticPump:GetVolume()
    return self.peristalticPumpInterface:GetPumpVolume(self.index - ExOffsetIndex)
end

function LCPeristalticPump:ExpectResult(timeout)
    local  pumpResult = LCPumpResult.new()
    pumpResult:SetIndex(0)
    pumpResult:SetResult(LCPumpOperateResult.Failed)

    pumpResult = self.peristalticPumpInterface:ExpectPumpResult(timeout)

    self.isRunning = false

    return pumpResult
end
