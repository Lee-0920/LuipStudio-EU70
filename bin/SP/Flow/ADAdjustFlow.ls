ADAdjustFlow = Flow:new
{
    action = setting.runAction.meterADAdjust
}

function ADAdjustFlow:new(o, action)
    o = o or {}
    setmetatable(o, self)
    self.__index = self

    o.action = action

    return o
end

function ADAdjustFlow:GetRuntime()
    local runtime = 0

    if self.action == setting.runAction.meterADAdjust then
        runtime = setting.runStatus.meterADAdjust.GetTime()
    elseif self.action == setting.runAction.measureADAdjust then
        runtime = setting.runStatus.measureADAdjust.GetTime()
    end

    return runtime
end

function ADAdjustFlow:OnStart()
    -- 初始化下位机
    dc:GetIDeviceStatus():Initialize()

    local runStatus
    if self.action == setting.runAction.meterADAdjust then
        runStatus = Helper.Status.SetStatus(setting.runStatus.meterADAdjust)
        StatusManager.Instance():SetStatus(runStatus)

    elseif self.action == setting.runAction.measureADAdjust then
        runStatus = Helper.Status.SetStatus(setting.runStatus.measureADAdjust)
        StatusManager.Instance():SetStatus(runStatus)
    end

end

function ADAdjustFlow:OnProcess()

    self.isUserStop = false
    self.isFinish = false

    self.dateTime = os.time()

    local runAction = Helper.Status.SetAction(self.action)
    StatusManager.Instance():SetAction(runAction)

    --清洗流程执行
    local err,result = pcall
    (
    function()
        if self.action == setting.runAction.meterADAdjust then
            return self:MeterADAdjust()
        elseif self.action == setting.runAction.measureADAdjust then
            return self:MeasureADAdjust()
        end
    end)
    print("finish call")
    if not err then      -- 出现异常
        if type(result) == "table" then
            if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
                self.isUserStop = true
                error(result)
            elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
                self.isUserStop = true
                error(result)
            elseif getmetatable(result) == StaticADControlStoppedException then  	--静态AD调节被停止异常。
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
    print("stop")
    self.isFinish = true
end

function ADAdjustFlow:OnStop()

    -- 初始化下位机
    dc:GetIDeviceStatus():Initialize()

    if not self.isFinish then
        if self.isUserStop then
            self.result = "用户终止"
            log:info("用户终止")
        else
            self.result = "故障终止"
            log:warn("故障终止")
        end
    else
        self.result = self.action.text .. "结束"
        log:info(self.action.text .. "结束")
        local str = self.action.text .. "流程总时间 = " .. tostring(os.time() - self.dateTime)
        log:debug(str)
    end
end

function ADAdjustFlow:MeterADAdjust()
    local ret = false
    local factor = dc:GetIPeristalticPump():GetPumpFactor(0)

    op:Drain(setting.liquidType.waste, 3, setting.liquid.prefabricateDrainSpeed * factor)

    --定量点1 AD调节
    ret = op:AutoStaticADControl(2, config.measureParam.meterLedAD[1])
    if ret == true then
        log:info("定量点1AD调节成功")
    else
        log:info("定量点1AD调节失败")
    end

    App.Sleep(1500)

    --定量点2 AD调节
    ret = op:AutoStaticADControl(3, config.measureParam.meterLedAD[2])
    if ret == true then
        log:info("定量点2AD调节成功")
    else
        log:info("定量点2AD调节失败")
    end

    local configVector = ConfigVector.new()
    
    local configValue1 = Config.new()
    local configValue2 = Config.new()

    local meter1Value = string.format("%d", dc:GetIOpticalAcquire():GetStaticADControlParam(2))
    local meter2Value = string.format("%d", dc:GetIOpticalAcquire():GetStaticADControlParam(3))

    configValue1:__set_profile(setting.ui.profile.hardwareParamIterms.name)
    configValue1:__set_config(setting.ui.profile.hardwareParamIterms[5][1].name)
    configValue1:__set_value(meter1Value)

    configValue2:__set_profile(setting.ui.profile.hardwareParamIterms.name)
    configValue2:__set_config(setting.ui.profile.hardwareParamIterms[5][2].name)
    configValue2:__set_value(meter2Value)

    configVector:Push(configValue1)
    configVector:Push(configValue2)
    WqimcManager.Instance():updateConfigs(configVector)

end

function ADAdjustFlow:MeasureADAdjust()
    local ret = false

    local period = config.hardwareConfig.measureLed.period.set
    dc:GetIOpticalAcquire():SetLEDDefaultValue(period)
    log:debug("重设LED测量周期： " .. period)

    local ScanData = 0
    local ScanLen = rc:GetScanLen()

    for i = 1, 5 do
        ScanData = ScanData + rc:GetScanData(ScanLen - 1)
        ScanLen = ScanLen + 1
        App.Sleep(1000)
    end
    ScanData = ScanData / 5

    log:debug("测量值： " .. ScanData)
    local tagetUpperLimit = config.measureParam.measureLedAD.measure * (1+config.measureParam.measureLedAD.offsetPercent)
    local tagetLowerLimit = config.measureParam.measureLedAD.measure * (1-config.measureParam.measureLedAD.offsetPercent)
    if ScanData >=  tagetUpperLimit or  ScanData <= tagetLowerLimit then
        --参考AD调节
        ret = op:AutoMeasureLEDControl(config.measureParam.measureLedAD.measure,config.measureParam.measureLedAD.offsetPercent,config.measureParam.measureLedAD.timeout * 1000)
        if ret == true then
            log:info("测量LED调节成功")
        else
            log:info("测量LED调节失败")
        end
    else
        log:debug("当前测量值满足参数设置要求，" .. "测量调节上限: " .. tagetUpperLimit .. "测量调节下限: " .. tagetLowerLimit)
    end

    App.Sleep(1500)

    --local configVector = ConfigVector.new()
    --
    --local configValue1 = Config.new()
    --local configValue2 = Config.new()
    --
    --local referenceValue = string.format("%d", dc:GetIOpticalAcquire():GetStaticADControlParam(0))
    --local measureValue = string.format("%d", dc:GetIOpticalAcquire():GetStaticADControlParam(1))
    --
    --
    --configValue1:__set_profile(setting.ui.profile.hardwareParamIterms.name)
    --configValue1:__set_config(setting.ui.profile.hardwareParamIterms[6][1].name)
    --configValue1:__set_value(referenceValue)
    --
    --configValue2:__set_profile(setting.ui.profile.hardwareParamIterms.name)
    --configValue2:__set_config(setting.ui.profile.hardwareParamIterms[6][2].name)
    --configValue2:__set_value(measureValue)
    --
    --configVector:Push(configValue1)
    --configVector:Push(configValue2)
    --WqimcManager.Instance():updateConfigs(configVector)


end
