
local _G = _ENV
local type = type
local pairs = pairs
local string = string
local tostring = tostring
local table = table
local error = error
local assert = assert
local print = print
local pcall = pcall
local collectgarbage = collectgarbage
local log = log
local StopOperator = StopOperator
local FlowManager = FlowManager
local ControllerManager = ControllerManager
local ExceptionHandler = ExceptionHandler
local status = status
local config = config

local P = {}
FlowList = P
_ENV = P

list ={}

function AddFlow(flow)
	if status.measure.isFatalError == false then
		if status.measure.isDeviceFault == false or config.system.faultBlocking == false then
			table.insert(list, flow)
		end
	end
end

function ClearList()
	for i = #list, 1, -1 do
		if list[i] then
			list[i]  = nil
		end
	end

--	collectgarbage()

end

function StopAllFlow()
	StopOperator()

	for i = 1,#list do
		if list[i] then
			list[i]:Stop()
		end
	end
end

function RemoveFlow(flowname)
	local over = false

	StopOperator()

	while #list > 0 and not over do
		for i = 1,#list do
			if list[i] and list[i].name == flowname then
				if list[i].isStarted == true then
					list[i]:Stop()
				else
					table.remove(list, i)
				end
				break
			end
			if i == #list then
				over = true
			end

		end
	end
end

function RunAllFlow()
	local flowManager = FlowManager.Instance()

	log:debug("flow begin stack count = " .. collectgarbage ("count"))

	local err,ret = pcall(function()
										for i = 1,#list do
											local flow = list[i]
											if flow then
												local name = flow.name
												local result = ""

												if name == "collectSample"  or name == "DCCommunicationCheck"
														or name == "LCCommunicationCheck"
														or name == "RCCommunicationCheck" then
													flowManager:UpdateFlowTime(name, flow:GetRuntime())
													name, result = flow:Run()
													flowManager:UpdateFlowResult(name, result)
												else
													if flowManager:IsComValid() == true then
														flowManager:UpdateFlowTime(name, flow:GetRuntime())
														name, result = flow:Run()
														flowManager:UpdateFlowResult(name, result)
													end
												end
											end
										end

										flowManager:UpdateAllFlowFinish()
								end)

	if not err then      -- 出现异常
		if type(ret) == "userdata" then
			log:warn("FlowList.RunAllFlow() =>" .. ret:What())
		elseif type(ret) == "table" then
			log:warn("FlowList.RunAllFlow() =>" .. ret:What())
		elseif type(ret) == "string" then
			log:warn("FlowList.RunAllFlow() =>" .. ret)	--C++、Lua系统异常
		end
	end

	ClearList()

	log:debug("flow end stack count = " .. collectgarbage ("count"))
end


return FlowList
