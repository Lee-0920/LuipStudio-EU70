
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
local log = log
local setting = setting


local P = {}
ModbusInterface = P
_ENV = P

detectResult = setting.modbusCoder.detectResultID.idle
commTestResult = setting.modbusCoder.detectResultID.idle

readError = false
writeError = false

function ReadCheck(funCode, regAddr, regNum)

	if funCode == 4 then
		if regAddr >= setting.externalInterface.modbus.inputRegAddr and regAddr <= setting.externalInterface.modbus.inputRegAddr + setting.externalInterface.modbus.inputRegNum then
			if regNum > 0 and regNum <= setting.externalInterface.modbus.inputRegNum then
				return true
			end
		end
	elseif funCode == 3 then
		if regAddr >= setting.externalInterface.modbus.holdRegAddr and regAddr <= setting.externalInterface.modbus.holdRegAddr + setting.externalInterface.modbus.holdRegNum then
			if regNum > 0 and regNum <= setting.externalInterface.modbus.holdRegNum then
				return true
			end
		end
	end

	return false
end

function OnRegisterPreread(funCode, regAddr, regNum)
--	print("OnRegisterPreread")
--	print("funCode = "  .. funCode)
--	print("regAddr = "  .. regAddr)
--	print("regNum = "  .. regNum)

	local err,result = pcall(function()

									if ReadCheck(funCode, regAddr, regNum) == false then
										return false
									end

									local num = 0
									local addr = 0

									while not setting.externalInterface.modbus.registers[regAddr] do
										regAddr = regAddr - 1
										regNum = regNum + 1

										if funCode == 4 then
											if regAddr <= setting.externalInterface.modbus.inputRegAddr then
												break
											end
										elseif funCode == 3 then
											if regAddr <= setting.externalInterface.modbus.holdRegAddr then
												break
											end
										else
											return false
										end
									end

									while true do
										addr = regAddr + num
--										print("addr = "  .. addr)
										if setting.externalInterface.modbus.registers[addr] ~= nil then
											setting.externalInterface.modbus.registers[addr].read()
											num = num + setting.externalInterface.modbus.registers[addr].number
--											print("num = "  .. num)
										else
											break
										end
--										print("num = "  .. num .. ",regNum = "  .. regNum)
										if num >= regNum then
--											print("end num = "  .. num)
											break
										end
									end
								end)

	if not err then      -- 出现异常
		if readError == false then
			readError = true
			if type(result) == "userdata" then
				log:warn("ModbusInterface.OnRegisterPreread() =>" .. result:What())
			elseif type(result) == "table" then
				log:warn("ModbusInterface.OnRegisterPreread() =>" .. result:What())
			elseif type(result) == "string" then
				log:warn("ModbusInterface.OnRegisterPreread() =>" .. result)	--C++、Lua系统异常
			else
				log:warn("ModbusInterface.OnRegisterPreread() => Unknown Type Error")
			end
		end
	else  --正常
		if readError == true then
			readError = false
			log:debug("ModbusInterface.OnRegisterPreread() => Recover Right.")
		end
	end
end

function WriteCheck(funCode, regAddr, regNum)

	if regAddr >= setting.externalInterface.modbus.holdRegAddr and regAddr <= setting.externalInterface.modbus.holdRegAddr + setting.externalInterface.modbus.holdRegNum then
		if regNum > 0 and regNum <= setting.externalInterface.modbus.holdRegNum then
			return true
		end
	end

	return false
end

function OnRegisterWrited(funCode, regAddr, regNum)
--	print("OnRegisterWrited")
--	print("funCode = "  .. funCode)
--	print("regAddr = "  .. regAddr)
--	print("regNum = "  .. regNum)

	local err,result = pcall(function()
										if WriteCheck(funCode, regAddr, regNum) == false then
											return false
										end

										local num = 0
										local addr = 0

										while true do
											addr = regAddr + num
--												print("addr = "  .. addr)
											if not setting.externalInterface.modbus.registers[addr]  ~= nil then
												setting.externalInterface.modbus.registers[addr].write()
												num = num + setting.externalInterface.modbus.registers[addr].number
--												print("num = "  .. num)
											else
												break
											end

--											print("num = "  .. num .. ",regNum = "  .. regNum)
											if num >= regNum then
--												print("end num = "  .. num)
												break
											end
										end
								end)

	if not err then      -- 出现异常
		if writeError == false then
			writeError = true
			if type(result) == "userdata" then
				log:warn("ModbusInterface.OnRegisterWrited() =>" .. result:What())
			elseif type(result) == "table" then
				log:warn("ModbusInterface.OnRegisterWrited() =>" .. result:What())
			elseif type(result) == "string" then
				log:warn("ModbusInterface.OnRegisterWrited() =>" .. result)	--C++、Lua系统异常
			else
				log:warn("ModbusInterface.OnRegisterWrited() => Unknown Type Error")
			end
		end
	else --正常
		if writeError == true then
			writeError = false
			log:debug("ModbusInterface.OnRegisterWrited() => Recover Right.")
		end
	end
end


return ModbusInterface
