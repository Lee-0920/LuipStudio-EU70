--[[
 * @brief 通信检测流程。
--]]

CommunicationCheckFlow = Flow:new
{
    text = "",
}

function CommunicationCheckFlow:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self

    o.name = ""
    o.result = ""

    return o
end

function CommunicationCheckFlow:GetRuntime()
    return 0
end

function CommunicationCheckFlow:OnStart()
    local eventStr = "开始" .. self.text
    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)

    local runStatus = Helper.Status.SetStatus(setting.runStatus.communication)
    StatusManager.Instance():SetStatus(runStatus)

    local runAction
    if self.name == "DCCommunicationCheck" then
        runAction = Helper.Status.SetAction(setting.runAction.DCCommunication)
        StatusManager.Instance():SetAction(runAction)
    elseif self.name == "LCCommunicationCheck" then
        runAction = Helper.Status.SetAction(setting.runAction.LCCommunication)
        StatusManager.Instance():SetAction(runAction)
    elseif self.name == "RCCommunicationCheck" then
        runAction = Helper.Status.SetAction(setting.runAction.RCCommunication)
        StatusManager.Instance():SetAction(runAction)
    elseif self.name == "OCCommunicationCheck" then
        runAction = Helper.Status.SetAction(setting.runAction.OCCommunication)
        StatusManager.Instance():SetAction(runAction)
    end

    local flowManager = FlowManager.Instance()
    flowManager:UpdateFlowMessage(self.name, "测试中")
    ModbusInterface.commTestResult = setting.modbusCoder.detectResultID.detecting

end


function CommunicationCheckFlow:OnProcess()

    local err, ret = pcall(function()
                                    if self.name == "DCCommunicationCheck" then
                                        dc:IsConnected()
                                    elseif self.name == "LCCommunicationCheck" then
                                        lc:IsConnected()
                                    elseif self.name == "RCCommunicationCheck" then
                                        rc:IsConnected()
                                    elseif self.name == "OCCommunicationCheck" then
                                        oc:IsConnected()
                                    end
                                end)

    if not err then      -- 出现异常
        if type(ret) == "userdata" then
            if ret:GetType() == "ExpectEventTimeoutException" then          --期望事件等待超时异常。
                ExceptionHandler.MakeAlarm(ret)
            elseif ret:GetType() == "CommandTimeoutException" then          --命令应答超时异常
                ExceptionHandler.MakeAlarm(ret)
            else
                log:warn("CommunicationCheckFlow.OnProcess()=>" .. ret:What())
            end
        elseif type(ret) == "table" then
            log:warn("CommunicationCheckFlow.OnProcess()=>" .. ret:What())
        elseif type(ret) == "string" then
            log:warn("CommunicationCheckFlow.OnProcess()=>" .. ret)	--C++、Lua系统异常
        end

        self.result = "未通过"
        ModbusInterface.commTestResult = setting.modbusCoder.detectResultID.fail
    else
        self.result = "通过"
        ModbusInterface.commTestResult = setting.modbusCoder.detectResultID.passed
    end

    local eventStr = self.text .. self.result
    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)
end


function CommunicationCheckFlow:OnStop()



end
