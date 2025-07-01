--[[
 * @brief 各个业务流程的基类。
 * @details 各个业务流程的公共部分。
--]]

Flow =
{
	isStarted = false,
	isUserStop = false,
	isFinish = false,
	isDeviceFault = false,
	isFatalError = false,

	name = "",
	text = "",
	result = "",
}

function Flow:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self

	return o
end

function Flow:Stop()
	self.isStarted = false
end

function Flow:Wait(seconds)
	local cnt = math.floor(seconds * 1000/200)

	while true do
		if  self.isStarted then
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

function Flow:GetRuntime()
	return 0
end

function Flow:OnStart()
	print("Flow OnStart")
end

function Flow:OnProcess()
	print("Flow OnProcess")
end

function Flow:OnStop()
	print("Flow OnStop")

end

function Flow:Run()
	print("Flow Run")

	self.isStarted = true

	--清除告警
	if AlarmManager.Instance().ClearAlarm ~= nil then
		AlarmManager.Instance():ClearAlarm()
	end

	local err,result = pcall(function() self:OnStart() self:OnProcess() end)

	self:ExceptionHandle(err, result)

	err,result = pcall(function() self:OnStop() end)

	self:ExceptionHandle(err, result)

	self.isStarted = false

	if Measurer.flow then
		Measurer:Reset()
	end

	--检查是否发生仪器运行故障
	if self.isDeviceFault == true and config.system.faultBlocking == true then
		status.measure.isDeviceFault = true
		ConfigLists.SaveMeasureStatus()
	end

	--检查是否发生仪器严重故障
	if self.isFatalError == true then
		status.measure.isFatalError = true
		ConfigLists.SaveMeasureStatus()
		if UpdateEvent.FatalErrorOccurrence ~= nil then
			UpdateWidgetManager.Instance():Update(UpdateEvent.FatalErrorOccurrence, "Flow")
		end
	end

	--仪器状态显示复位
	StatusManager.Instance():ResetDeviceStatus()

	return self.name, self.result
end

function Flow:ExceptionHandle(err, result)
	if not err then      -- 出现异常
		if type(result) == "userdata" then
			if result:GetType() == "ExpectEventTimeoutException" then          --期望事件等待超时异常。
				ExceptionHandler.MakeAlarm(result)
			elseif result:GetType() == "CommandTimeoutException" then          --命令应答超时异常
				ExceptionHandler.MakeAlarm(result)
			else
				log:warn(result:What())
			end
		elseif type(result) == "table" then
			if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
				ExceptionHandler.MakeAlarm(result)
				self.isUserStop = true
			elseif getmetatable(result) == PumpFailedException then 			--泵操作中途出现故障，未能完成异常。
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
				ExceptionHandler.MakeAlarm(result)
				self.isUserStop = true
			elseif getmetatable(result) == AcquirerADFailedException then 	    --光学采集中途出现故障，未能完成异常。
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result)== StaticADControlStoppedException then 	    --静态AD调节被停止异常
				ExceptionHandler.MakeAlarm(result)
				self.isUserStop = true
			elseif getmetatable(result) == StaticADControlFailedException then 	    --静态AD出现故障，未能完成异常。
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
				ExceptionHandler.MakeAlarm(result)
				self.isUserStop = true
			elseif getmetatable(result) == MeterFailedException then 			--定量中途出现故障，未能完成异常。
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result) == MeterOverflowException then 		--定量溢出异常。
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result) == MeterUnfinishedException then 		--定量目标未达成异常。
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result)== MeterAirBubbleException then			--定量气泡异常。
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result) == MeterExpectTimeoutException then 	--定量超时异常。
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
				ExceptionHandler.MakeAlarm(result)
				self.isUserStop = true
			elseif getmetatable(result) == ThermostatFailedException then 		--恒温中途出现故障，未能完成异常。
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result) == ThermostatTimeoutException then 		--恒温超时，指定时间内仍未达到目标温度异常
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result) == DeviceFaultException then			--仪器异常停机异常。
				self.isDeviceFault = true
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result) == DrainFromDigestionException then 	 	--排消解液异常
				ExceptionHandler.MakeAlarm(result)
				if config.system.CCEPMode ~= true then
					self.isFatalError = true
				end
			elseif getmetatable(result) == AddLiquidToDigestionException then 	 	--消解室加液异常
				ExceptionHandler.MakeAlarm(result)
				if config.system.CCEPMode ~= true then
					self.isFatalError = true
				end
			elseif getmetatable(result) == MeasureDataException then 	--测量数据异常。
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result) == MeasureLedException then 	--测量信号异常。
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result) == UpdateException then 				--升级异常。
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
				ExceptionHandler.MakeAlarm(result)
				self.isUserStop = true
			elseif getmetatable(result) == CurveCheckWrongException then 	 	--校准结果错误异常
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result)== CalibrateResultWrongException then 	--精准定标结果错误异常
				ExceptionHandler.MakeAlarm(result)
			elseif getmetatable(result) == AirFlowException then 	--载气调节异常。
				ExceptionHandler.MakeAlarm(result)
			else
				log:warn(result:What())								--其他定义类型异常	
			end
		elseif type(result) == "string" then
			log:warn(result)										--C++、Lua系统异常
		else

		end
	end
end
