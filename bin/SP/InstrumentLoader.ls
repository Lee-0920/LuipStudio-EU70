package.path = [[?;?.lua;?.ls;?;./SP/Controller/?.ls;./SP/Flow/?.ls;./SP/Common/?.ls;./SP/ExternalInterface/?.ls;]]
require("ControllerCreater")
require("FlowList")
require("Flow")
require("MeasureAD")
require("Measurer")
require("MeasureFlow")
require("CalibrateFlow")
require("ConfirmFlow")
require("TurboConfirmFlow")
require("CleanFlow")
require("SmartValveDetectFlow")
require("SmartMeasureDetectFlow")
require("LiquidOperateFlow")
require("CombineOperateFlow")
require("CollectSampleFlow")
require("AutoPumpCheckFlow")
require("HardwareTest")
require("CommunicationCheckFlow")
require("ADAdjustFlow")
require("ModbusInterface")
require("Hj212Interface")
require("AutoECZeroFlow")

package.path = [[?;?.lua;?.ls;?;./SP/Setting/?.ls;./SP/Setting/Modbus/?.ls;./SP/Setting/Hj212/?.ls;]]
setting.externalInterface = {}

if config.system.modbusTableType == 0 then
    require("Labsun10")
    modbusStr = "LS1.0"
elseif config.system.modbusTableType == 1 then
    require("PC10")
    modbusStr = "PC1.0"
end

if config.system.hj212Platform.hj212Type == 1 then
    require("Hj212CCEP")        -- CCEP认证协议
elseif config.system.hj212Platform.hj212Type == 2 then
    require("Hj212")            -- 咸阳(HJ212-2017-Wry)
end


function InitHj212()
    if  (config.system.hj212Platform.hj212Type == 1 or config.system.hj212Platform.hj212Type == 2) and
            nil ~= setting.externalInterface.hj212.InitHj212 then

        setting.externalInterface.hj212.InitHj212()
    end
end

--[[
 *@brief 初始化界面中用户自定义的配置
--]]
function InitWidgetUserConfig()
    RefreshRangeMapping()
end


--[[
 *@brief 初始化设备,界面创建之后
--]]
function InitInstrument()
    if dc:IsConnected() then
        -- ReagentManager()
        -- dc:GetIOpticalAcquire():TurnOffLED()
        -- --设置去离子水泵和阀为打开
        -- --op:SetLCStopStatus()
        -- local map = LCValveMap.new(1 | 16)
        -- lc:GetISolenoidValve():SetValveMap(map)
    end
end

--[[
 *@brief 仪器重启后，对注射器进行复位检查，避免注射器推过头
--]]
function ReagentManager()
    local currentVol1 = dc:GetIPeristalticPump():GetPumpOffsetStep(setting.liquidType.reagent1.pump)
    log:debug("当前酸剂余量 " .. string.format("%.2f", currentVol1) .. "ul")

    local currentVol2 = dc:GetIPeristalticPump():GetPumpOffsetStep(setting.liquidType.reagent2.pump)
    log:debug("当前氧化剂余量 " .. string.format("%.2f", currentVol2) .. "ul")

    --体积显示归零，注射器传感器未遮挡，必须进行复位操作
    if currentVol1 == 0 and false == op:SyringeGetSenseStatus(setting.liquidType.reagent1.pump) then
        log:debug("酸剂注射器复位")
        op:StartSamplePump(2)
        op:SyringReset(setting.liquidType.reagent1)
        op:StopSamplePump()
        App.Sleep(500)
    end

    --体积显示归零，注射器传感器未遮挡，必须进行复位操作
    if currentVol2 == 0 and false == op:SyringeGetSenseStatus(setting.liquidType.reagent2.pump)  then
        log:debug("氧化剂注射器复位")
        op:StartSamplePump(2)
        op:SyringReset(setting.liquidType.reagent2)
        op:StopSamplePump()
        App.Sleep(500)
    end
end

--[[
 *@brief 机箱自动温控开关
--]]
function BoxAutoEnvironmentalControl()
    local temperatureMonitor = config.system.temperatureMonitor
    local err = pcall(function()
        dc:SetBoxFanEnable(temperatureMonitor)
        if temperatureMonitor == true then
            log:debug("上机箱温控设置温度：" .. config.system.environmentTemperature)
            log:debug("下机箱温控设置温度：" .. config.system.exEnvironmentTemperature)
        end
    end)
    if not err then
        log:warn("设置机箱风扇自动散热功能==>失败" .. "SetBoxFanEnable(true)" .. " failed.")
    end
end

--[[
 *@brief 量程映射刷新
--]]
function RefreshRangeMapping()
    for _,v in ipairs(_G.setting.ui.profile.measureParam) do
        for _,k in ipairs(v) do
            if DataType.Option == k.type and nil ~= k.UpdateOptions then
                k.UpdateOptions()
            end
        end
    end

    _G.setting.ui.curve.UpdateCurve()
end


function UpdateMeasureTimeStatus(oldTime)
	
	print("oldTime = " .. oldTime)
	
    local err,ret = pcall(function()
		local currTime = os.time()		
		print("currTime = " .. currTime)
		
		status.measure.schedule.autoMeasure.dateTime = currTime - (oldTime - status.measure.schedule.autoMeasure.dateTime)
        local interval = os.date("*t", currTime).day - os.date("*t", oldTime).day
        if MeasureMode.Timed ~= config.scheduler.calibrate.mode then
            status.measure.schedule.autoCalibrate.dateTime = currTime - (oldTime - status.measure.schedule.autoCalibrate.dateTime)
        elseif MeasureMode.Timed == config.scheduler.calibrate.mode and 0 ~= interval then
            local lastCalibrateDateTime = os.date("*t", status.measure.schedule.autoCalibrate.dateTime)
            lastCalibrateDateTime.day = lastCalibrateDateTime.day + interval
            status.measure.schedule.autoCalibrate.dateTime = os.time(lastCalibrateDateTime)
        end
		status.measure.schedule.autoClean.dateTime = currTime - (oldTime - status.measure.schedule.autoClean.dateTime)
		status.measure.schedule.autoCheck.dateTime = currTime - (oldTime - status.measure.schedule.autoCheck.dateTime)
        status.measure.schedule.autoBlankCheck.dateTime = currTime - (oldTime - status.measure.schedule.autoBlankCheck.dateTime)
        status.measure.schedule.zeroCheck.dateTime = currTime - (oldTime - status.measure.schedule.zeroCheck.dateTime)
        status.measure.schedule.rangeCheck.dateTime = currTime - (oldTime - status.measure.schedule.rangeCheck.dateTime)

		ConfigLists.SaveMeasureStatus()
        config.scheduler.calibrate.configChangeTime = currTime - (oldTime - config.scheduler.calibrate.configChangeTime)
        ConfigLists.SaveSchedulerConfig()
		
        return true
    end)

    if not err then      -- 出现异常
        if type(ret) == "userdata" then
            log:warn("UpdateMeasureTimeStatus() =>" .. ret:What())
        elseif type(ret) == "table" then
            log:warn("UpdateMeasureTimeStatus() =>" .. ret:What())
        elseif type(ret) == "string" then
            log:warn("UpdateMeasureTimeStatus() =>" .. ret)	--C++、Lua系统异常
        end
    end
end

