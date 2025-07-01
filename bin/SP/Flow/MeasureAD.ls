--[[
 * @brief 吸光度计算
--]]

MeasureAD = 
{
	initReference = 0,
	initMeasure = 0,
	finalReference = 0,
	finalMeasure = 0
}

function MeasureAD:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	self.__metatable = "MeasureAD"
	return o
end

function MeasureAD:CalculateRelAbsorbance()
	local abs = 0;
	if self.initReference <= 0 or self.initMeasure <= 0 or self.finalReference <= 0 or self.finalMeasure <= 0 then
		print("MeasureAD return 0")
		return 0
	end

	abs = math.log(((self.finalReference/self.finalMeasure)/(self.initReference/self.initMeasure)),10)

	return abs*1000
end

function MeasureAD:CalculateAbsAbsorbance()
	local abs = 0;
	if self.finalReference <= 0 or self.finalMeasure <= 0 then
		print("MeasureAD CalculateAbsAbsorbance return 0")
		return 0
	end

	abs = math.log((self.finalReference/self.finalMeasure),10)

	return abs*1000
end

function MeasureAD:CalculateInitAbsorbance()
	local abs = 0;
	if self.initReference <= 0 or self.initMeasure <= 0 then
		print("MeasureAD CalculateInitAbsorbance return 0")
		return 0
	end

	abs = math.log((self.initReference/self.initMeasure),10)

	return abs*1000
end