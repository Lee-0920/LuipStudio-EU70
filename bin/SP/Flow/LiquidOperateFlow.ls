LiquidOperateFlow = Flow:new
{
    source = setting.liquidType.none,
    dest = setting.liquidType.none,
    mode = 0,
    sVolume = 0,
    dVolume = 0,
    action = setting.runAction.suckFromBlank,
    text = "",
}

function LiquidOperateFlow:new(o, source, dest, mode, sVol, dVol, action)
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

function LiquidOperateFlow:GetRuntime()
    return 0
end

function LiquidOperateFlow:OnStart()
    local eventStr = "开始" .. self.text
    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)
    -- 初始化下位机
    dc:GetIDeviceStatus():Initialize()
    lc:GetIDeviceStatus():Initialize()

    --检测消解室是否为安全温度
    op:CheckDigestSafety()

end

function LiquidOperateFlow:OnProcess()

    self.isUserStop = false
    self.isFinish = false

    self.dateTime = os.time()

    local runStatus = Helper.Status.SetStatus(setting.runStatus.liquidOperate)
    StatusManager.Instance():SetStatus(runStatus)

    local runAction = Helper.Status.SetAction(self.action)
    StatusManager.Instance():SetAction(runAction)

    local err,result = pcall
    (
        function()
            if self.source ~= setting.liquidType.none then
                if self.source == setting.liquidType.reagent1
                    or self.source == setting.liquidType.reagent2 then
                    op:SyringeSuck(self.source, self.sVolume * 1000, 400)
                else
                    op:Pump(self.source, self.sVolume, 0.4)
                end
            end

            if self.dest  ~= setting.liquidType.none then
                if self.dest == setting.liquidType.reagent1
                        or self.dest == setting.liquidType.reagent2 then
                    op:SyringeDrain(self.source, self.sVolume * 1000, 400)
                else
                    op:Drain(self.dest, self.dVolume, 0.4)
                end
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

function LiquidOperateFlow:OnStop()

    -- 初始化下位机
    dc:GetIDeviceStatus():Initialize()
    
    --保存试剂余量表
    ReagentRemainManager.SaveRemainStatus()

    local eventStr

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
        self.result = "管路操作结束"
        log:info("管路操作结束")
        local str = "管路操作流程总时间 = " .. tostring(os.time() - self.dateTime)
        log:debug(str)
        eventStr = self.text .. "结束"
    end

    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)
end
