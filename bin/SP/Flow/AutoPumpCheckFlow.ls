AutoPumpCheckFlow = Flow:new()

function AutoPumpCheckFlow:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self

    o.SPEED = 0.2
    return o
end

function AutoPumpCheckFlow:GetRuntime()
	return 0
end

function AutoPumpCheckFlow:OnStart()
	-- 初始化下位机

	dc:GetIDeviceStatus():Initialize()

	--检测消解室是否为安全温度
	op:CheckDigestSafety()

    local runStatus = Helper.Status.SetStatus(setting.runStatus.autoPumpCheck)
    StatusManager.Instance():SetStatus(runStatus)

    local runAction = Helper.Status.SetAction(setting.runAction.autoPumpCheck)
    StatusManager.Instance():SetAction(runAction)
end

function AutoPumpCheckFlow:OnProcess()

    self.isUserStop = false
    self.isFinish = false
    self.dateTime = os.time()

	local err,result = pcall
    (
        function()

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

function AutoPumpCheckFlow:OnStop()

	-- 初始化下位机
	dc:GetIDeviceStatus():Initialize()


	--保存试剂余量表
	ReagentRemainManager.SaveRemainStatus()

    if not self.isFinish then
		if self.isUserStop then
			self.result = "用户终止"
			log:info("用户终止")
		else
			self.result = "故障终止"
			log:warn("故障终止")
		end
	else
		self.result = "泵校准结束"
        log:info("泵校准结束")
        local str = "泵校准流程总时间 = " .. tostring(os.time() - self.dateTime)
        log:debug(str)
        UpdateWidgetManager.Instance():Update(UpdateEvent.AutoPumpCheck, "AutoPumpCheckFlow")
	end

end


