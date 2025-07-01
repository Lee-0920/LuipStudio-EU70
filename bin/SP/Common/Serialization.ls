local _G = _ENV
local type = type
local pairs = pairs
local string = string
local tostring = tostring
local getmetatable = getmetatable
local error = error
local io = io
local assert = assert
local load = load
local loadfile = loadfile
local loadstring = loadstring
local print = print
local log = log


local P = {_G = _ENV,}
Serialization = P
_ENV = P

-- 序列化指定对象 obj 为一个扁平的字符串并返回。
function Serialize(obj)
	local text = ""
	local t = type(obj)
	if t == "number" then
		text = text .. obj
	elseif t == "boolean" then
		text = text .. tostring(obj)
	elseif t == "string" then
		text = text .. string.format("%q", obj)
	elseif t == "table" then
		text = text .. "{\n"
		for k,v in pairs(obj) do
			text = text .. "[" .. Serialize(k) .. "]=" .. Serialize(v) .. ",\n"
		end
		local metatable = getmetatable(obj)
		if metatable ~= nil and type(metatable.__index) == "table" then
			for k,v in pairs(metatable.__index) do
				text = text .. "[" .. Serialize(k) .. "]=" .. Serialize(v) .. ",\n"
			end
		end
		text = text .. "}"
	elseif t == "nil" then
		log:debug("Serialize(obj) ->  return nil")
		return nil
	else
		log:debug("Serialize(obj) ->  can not serialize a " .. t .. " type.")
		
		error("can not serialize a " .. t .. " type.")
	end
	
	return text
end

-- 反序列化指定文本text，并返回生成的对象。
function Deserialize(text)
	local t = type(text)
	if t == "nil" or text == "" then
		return nil
	elseif t == "number" or t == "string" or t == "boolean" then
		text = tostring(text)
	else
		error("can not unserialize a " .. t .. " type.")
	end
	text = "return " .. text
	local func = load(text)
	if func == nil then
		return nil
	end
	return func()
end

function SaveEncryptionFile(obj, filePath, targetName)
	local text = targetName .. " =" .. Serialize(obj)
	local datacode = string.dump(load(text), true)
	local file = io.open(filePath, "wb")
	file:write(datacode)
	file:close()
end
--[[function SaveEncryptionFile(obj, filePath, targetName)
	local text = targetName .. " =" .. Serialize(obj)
	local file = io.open(filePath, "wb")
	file:write(text)
	file:close()
end--]]

function LoadEncryptionFile(filePath, targetName, isGlobal)
	local obj

	if type(targetName)=="string" then

		-- 在新_ENV创建配置表
		local t = _ENV
		for k, v in string.gmatch (targetName, "([%w_]+)(.?)") do
			t[k] = t[k] or {}  -- 不存在该项则创建空表
			t = t[k]
		end

		-- 从二进制文件加载代码，执行代码
		local datacode = assert(loadfile(filePath, "bt", _ENV))
		datacode()

		-- 从新_ENV获取创建配置表的对象
		local te = _ENV
		for k, v in string.gmatch (targetName, "([%w_]+)(.?)") do
			if v == "." then  -- 不是最后一项
				te = te[k]
			else
				obj = te[k]
			end
		end

		-- 将配置表加载到全局表
		if isGlobal == true then
			local t = _G
			for k, v in string.gmatch (targetName, "([%w_]+)(.?)") do
				if v == "." then  -- 不是最后一项
					t[k] = t[k] or {}  -- 不存在该项则创建空表
					t = t[k]
				else
					t[k] = obj
				end
			end
		end
	end

	return obj
end

-- 序列化指定的对象 obj 并保存到配置文件 filePath。
function SaveFile(obj, filePath)
	local text = Serialize(obj)
	local file = io.open(filePath, "wb")
	file:write(text)
	file:close()
end

-- 导入指定配置文件 filePath 反序列化成对象并返回。
-- 如果指定了targetName，还将同时保存到指定的全局表（如“config.info.mc”）。
function LoadFile(filePath, targetName)
	--print(filePath .. "\n")
    local file = io.open(filePath, "rb")
    local obj = Deserialize(file:read("a"))
    file:close()
    -- 从全局表开始一级一级往下解析
    if type(targetName)=="string" then
	    local t = _G
	    for k, v in string.gmatch (targetName, "([%w_]+)(.?)") do
		if v == "." then  -- 不是最后一项
		    t[k] = t[k] or {}  -- 不存在该项则创建空表
		    t = t[k]
		else
		    t[k] = obj
		end
	    end
    end
    return obj
end

return Serialization
