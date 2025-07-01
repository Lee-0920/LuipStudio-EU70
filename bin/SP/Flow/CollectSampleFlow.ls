--[[
 * @brief 采集水样流程。
--]]


CollectSampleFlow = Flow:new
{
	isAutoStart = false,
	text = "",
}

function CollectSampleFlow:new(o, auto)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	
	o.detectTime = os.time()
	o.isAutoStart = auto

	return o
end


function CollectSampleFlow:OnStart()
	local eventStr = "开始" .. self.text
	--保存审计日志
	SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)

	--组合流程需要重新加载时间
	self.measureDateTime = os.time()
	local runStatus = Helper.Status.SetStatus(setting.runStatus.collectSample)
	if self.isAutoStart then
		runStatus = Helper.Status.SetStatus(setting.runStatus.sampleWaterRelay)
	end
	StatusManager.Instance():SetStatus(runStatus)
end

function CollectSampleFlow:OnProcess()
	local waterCollector = WaterCollector.Instance()

	if self.isAutoStart then
		local runAction = Helper.Status.SetAction(setting.runAction.sampleWaterRelay)
		StatusManager.Instance():SetAction(runAction)
		if not string.find(config.info.instrument["type"], "PT63P") then
			waterCollector:TurnOn()
		end
		Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, true)

		if not self:Wait(config.scheduler.collectSample.collectTime) then 	-- 采集等待
			if not string.find(config.info.instrument["type"], "PT63P") then
				waterCollector:TurnOff()
			end
			Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, false)
			error(UserStopException:new())    --用户停止
		end

		if not string.find(config.info.instrument["type"], "PT63P") then
			waterCollector:TurnOff()
		end
		Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, false)
	else
		local runAction = Helper.Status.SetAction(setting.runAction.collectSample.collect)
		StatusManager.Instance():SetAction(runAction)
		if not string.find(config.info.instrument["type"], "PT63P") then
			waterCollector:TurnOn()
		end
		Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, true)

		if not self:Wait(config.interconnection.miningWaterTime) then 	-- 采集等待
			if not string.find(config.info.instrument["type"], "PT63P") then
				waterCollector:TurnOff()
			end
			Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, false)
			error(UserStopException:new())    --用户停止
		end

		local runAction = Helper.Status.SetAction(setting.runAction.collectSample.silent)
		StatusManager.Instance():SetAction(runAction)

		if not string.find(config.info.instrument["type"], "PT63P") then
			waterCollector:TurnOff()
		end
		Helper.Result.RelayOutOperate(setting.mode.relayOut.collectInstruct, false)

		if not self:Wait(config.interconnection.silentTime) then 	-- 静默等待
			error(UserStopException:new())    --用户停止
		end
	end

end	

function CollectSampleFlow:OnStop()

	self.result = "采集水样结束"
	log:info("采集水样结束")
	log:debug("采集水样时间 = "..os.time() - self.detectTime )
end
