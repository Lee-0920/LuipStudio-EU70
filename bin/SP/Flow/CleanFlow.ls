--[[
 * @brief 清洗类型
--]]
cleanType =
{
    cleanDeeply = 0,     --深度清洗
    cleanAll = 1,       	--清洗所有管路
    oneKeyRenew = 2,      --一键填充试剂
}

--[[
 * @brief 清洗流程。
--]]
CleanFlow = Flow:new
{
    text = "",
}

function CleanFlow:new(o, target)
    o = o or {}
    setmetatable(o, self)
    self.__index = self

    o.cleanDateTime = os.time()
    o.cleanType = target

    return o
end

function CleanFlow:GetRuntime()
    local runtime = 0

    if self.cleanType == cleanType.cleanDeeply then
        runtime = setting.runStatus.cleanDeeply.GetTime()
    elseif self.cleanType == cleanType.cleanAll then
        runtime = setting.runStatus.cleanAll.GetTime()
    elseif self.cleanType == cleanType.oneKeyRenew then
        runtime = setting.runStatus.oneKeyRenew.GetTime()
    end

    return runtime
end

function CleanFlow:OnStart()
    local eventStr = "开始" .. self.text
    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)

    --组合流程需要重新加载时间
    self.measureDateTime = os.time()
    -- 初始化下位机
    dc:GetIDeviceStatus():Initialize()

    --继电器指示
    Helper.Result.RelayOutOperate(setting.mode.relayOut.cleanInstruct, true)

    --设置运行状态
    local runStatus = Helper.Status.SetStatus(setting.runStatus.cleanDeeply)
    if self.cleanType == cleanType.cleanDeeply then
        runStatus = Helper.Status.SetStatus(setting.runStatus.cleanDeeply)
    elseif self.cleanType == cleanType.cleanAll then
        runStatus = Helper.Status.SetStatus(setting.runStatus.cleanAll)
    elseif self.cleanType == cleanType.oneKeyRenew then
        runStatus = Helper.Status.SetStatus(setting.runStatus.oneKeyRenew)
    end
    StatusManager.Instance():SetStatus(runStatus)
end

function CleanFlow:OnProcess()
    self.isUserStop = false
    self.isFinish = false

    --清洗流程执行
    local err,result = pcall
    (
        function()
            if self.cleanType == cleanType.cleanDeeply then
                return self:CleanDeeply()
            elseif self.cleanType == cleanType.cleanAll then
                return self:CleanAll()
            elseif self.cleanType == cleanType.oneKeyRenew then
                return self:OneKeyRenew()
            end
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
            elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
                self.isUserStop = true
                error(result)
            elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
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

function CleanFlow:OnStop()

    --继电器指示
    Helper.Result.RelayOutOperate(setting.mode.relayOut.cleanInstruct, false)

    if self.cleanType == cleanType.cleanDeeply then
        status.measure.schedule.autoClean.dateTime = self.cleanDateTime
        ConfigLists.SaveMeasureStatus()
    end

    -- 初始化下位机
    dc:GetIDeviceStatus():Initialize()

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
        self.result = self.text .. "完成"
        log:info("完成")
        log:info("流程总时间 = "..os.time()-self.cleanDateTime)
        eventStr = self.text .. "完成"
    end

    --保存审计日志
    local eventStr = self.result .. self.text
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)

    --保存试剂余量表
    ReagentRemainManager.SaveRemainStatus()

    --检测消解室是否为安全温度
    op:CheckDigestSafety()

end

--[[
 * @brief 深度清洗流程
--]]
function CleanFlow:CleanDeeply(flow)
    if nil ~= flow then
        self = flow
    end

    self.isFinish = true

    return true
end

--[[
 * @brief 清洗所有管路
--]]
function CleanFlow:CleanAll()

    self.isFinish = true

    return true
end

--[[
 * @brief 一键管路更新
--]]
function CleanFlow:OneKeyRenew()

    local runAction

    -- 清空试剂一管
    runAction = Helper.Status.SetAction(setting.runAction.mulCalibrate.clearReagent1Pipe)
	StatusManager.Instance():SetAction(runAction)
    op:SyringReset(setting.liquidType.reagent1)

    -- 清空试剂二管
    runAction = Helper.Status.SetAction(setting.runAction.mulCalibrate.clearReagent2Pipe)
    StatusManager.Instance():SetAction(runAction)
    op:SyringReset(setting.liquidType.reagent2)

    -- 更新试剂一管
    runAction = Helper.Status.SetAction(setting.runAction.mulCalibrate.updateReagent1)
    StatusManager.Instance():SetAction(runAction)
    op:ReagentManager(setting.liquidType.reagent1)

    -- 更新试剂二管
    runAction = Helper.Status.SetAction(setting.runAction.mulCalibrate.updateReagent2)
    StatusManager.Instance():SetAction(runAction)
    op:ReagentManager(setting.liquidType.reagent2)
end
