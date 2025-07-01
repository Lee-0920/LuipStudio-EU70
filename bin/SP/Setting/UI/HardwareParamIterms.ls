setting.ui.profile.hardwareParamIterms =
{
    name = "hardwareParamIterms",
    text = "硬件校准",
    writePrivilege=  RoleType.Administrator,
    readPrivilege = RoleType.Administrator,
    rowCount = 5,
    superRow = 0,
    administratorRow = 5,
    index = 10,
    {
        name = "pumpFactor",
        text = "泵校准",
        pumpFactor =
        {
            0,
            0,
        },
        tempValue =
        {
            0,
            0,
        },
        get = function()
            if dc:GetConnectStatus() then
                local status,result = pcall(function()
                    local ret = {}
                    ret[1]  = dc:GetIPeristalticPump():GetPumpFactor(0)
                    ret[2]  = dc:GetIPeristalticPump():GetPumpFactor(2)
                    return ret
                end)
                if not status then
                    ExceptionHandler.MakeAlarm(result)
                    for k in pairs(setting.ui.profile.hardwareParamIterms[1].pumpFactor) do
                        setting.ui.profile.hardwareParamIterms[1].pumpFactor[k] = 0
                    end
                    --setting.ui.profile.hardwareParamIterms[1].pumpFactor[1] = 0
                    --setting.ui.profile.hardwareParamIterms[1].pumpFactor[2] = 0
                else
                    setting.ui.profile.hardwareParamIterms[1].pumpFactor = nil
                    setting.ui.profile.hardwareParamIterms[1].pumpFactor = result
                    setting.ui.profile.hardwareParamIterms[1].tempValue = result
                end
            else
                for k in pairs(setting.ui.profile.hardwareParamIterms[1].pumpFactor) do
                    setting.ui.profile.hardwareParamIterms[1].pumpFactor[k] = 0
                end
                --setting.ui.profile.hardwareParamIterms[1].pumpFactor[1] = 0
                --setting.ui.profile.hardwareParamIterms[1].pumpFactor[2] = 0
            end
        end,
        set = function()
            if dc:GetConnectStatus() then
                local status,result = pcall(function()
                    dc:GetIPeristalticPump():SetPumpFactor(0, setting.ui.profile.hardwareParamIterms[1].pumpFactor[1])
                    dc:GetIPeristalticPump():SetPumpFactor(2, setting.ui.profile.hardwareParamIterms[1].pumpFactor[2])
                end)
                if not status then
                    ExceptionHandler.MakeAlarm(result)
                    return false, "设置泵校准失败\n"
                else
                    config.hardwareConfig.backupParam.pumpMeter = string.format("%.8f", setting.ui.profile.hardwareParamIterms[1].pumpFactor[1])
                    config.hardwareConfig.backupParam.pumpSyring = string.format("%.8f", setting.ui.profile.hardwareParamIterms[1].pumpFactor[2])
                    setting.ui.profile.hardwareParamIterms.hardwareParamAuditTrail(setting.ui.profile.hardwareParamIterms[1][1].name)
                    setting.ui.profile.hardwareParamIterms.hardwareParamAuditTrail(setting.ui.profile.hardwareParamIterms[1][2].name)
                    return true, ""
                end
            else
                return false, "驱动板连接断开,\n设置泵校准失败\n"
            end
        end,
        synchronize = function()
            if dc:GetConnectStatus() then
                local status,result = pcall(function()
                    local PRECISE = 0.000001
                    local pumpfactor = dc:GetIPeristalticPump():GetPumpFactor(0)
                    if math.abs(pumpfactor - config.hardwareConfig.backupParam.pumpMeter) > PRECISE then
                        log:debug("参数同步-[定量泵系数(ml/步)] " .. config.hardwareConfig.backupParam.pumpMeter)
                        dc:GetIPeristalticPump():SetPumpFactor(0, config.hardwareConfig.backupParam.pumpMeter)
                        App.Sleep(200)

                        pumpfactor = dc:GetIPeristalticPump():GetPumpFactor(0)
                        if math.abs(pumpfactor - config.hardwareConfig.backupParam.pumpMeter) > PRECISE then
                            log:warn("同步定量泵系数失败。")
                        else
                            log:warn("同步定量泵系数成功。")
                        end
                    end
                end)
                if not status then
                    ExceptionHandler.MakeAlarm(result)
                    log:warn("同步定量泵系数失败。")
                end

                App.Sleep(200)
                local status,result = pcall(function()
                    local PRECISE = 0.000001
                    local pumpfactor = dc:GetIPeristalticPump():GetPumpFactor(2)
                    if math.abs(pumpfactor - config.hardwareConfig.backupParam.pumpSyring) > PRECISE then
                        log:debug("[参数同步-注射泵系数(ml/步)] " .. config.hardwareConfig.backupParam.pumpSyring)
                        dc:GetIPeristalticPump():SetPumpFactor(2, config.hardwareConfig.backupParam.pumpMeter)
                        App.Sleep(200)

                        pumpfactor = dc:GetIPeristalticPump():GetPumpFactor(2)
                        if math.abs(pumpfactor - config.hardwareConfig.backupParam.pumpSyring) > PRECISE then
                            log:warn("同步注射泵系数失败。")
                        else
                            log:warn("同步注射泵系数成功。")
                        end
                    end
                end)
                if not status then
                    ExceptionHandler.MakeAlarm(result)
                    log:warn("同步注射泵系数数失败。")
                end
            end
        end,
        {
            name = "meterPump",
            text = "定量泵",
            refData = "pumpFactor[1]",
            unit = "ml/步",
            get = function()
                return setting.ui.profile.hardwareParamIterms[1].pumpFactor[1]
            end,
            checkValue = function(value)
                if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                    return value
                else
                    return string.format("%.8f",setting.ui.profile.hardwareParamIterms[1][1].get())
                end
            end,
            set = function(value)
                setting.ui.profile.hardwareParamIterms[1].pumpFactor[1] = value
            end,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Administrator,
        },
        {
            name = "SyringePump",
            text = "注射泵",
            refData = "pumpFactor[2]",
            unit = "ml/步",
            get = function()
                return setting.ui.profile.hardwareParamIterms[1].pumpFactor[2]
            end,
            checkValue = function(value)
                if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                    return value
                else
                    return string.format("%.8f",setting.ui.profile.hardwareParamIterms[1][2].get())
                end
            end,
            set = function(value)
                setting.ui.profile.hardwareParamIterms[1].pumpFactor[2] = value
            end,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Administrator,
        },
    },
    {
        name = "tempCalibrate",
        text = "温度校准",
        tempCalibrate = TempCalibrateFactor.new(),
        tempValue = TempCalibrateFactor.new(),
        get = function()
            if dc:GetConnectStatus() then
                local status,result = pcall(function()
                    --return dc:GetITemperatureControl():GetCalibrateFactor()
                    return dc:GetITemperatureControl():GetCalibrateFactorForTOC(1)
                end)
                if not status then
                    ExceptionHandler.MakeAlarm(result)
                    setting.ui.profile.hardwareParamIterms[2].tempCalibrate:SetNegativeInput(0)
                    setting.ui.profile.hardwareParamIterms[2].tempCalibrate:SetReferenceVoltage(0)
                    setting.ui.profile.hardwareParamIterms[2].tempCalibrate:SetCalibrationVoltage(0)
                else
                    setting.ui.profile.hardwareParamIterms[2].tempCalibrate = nil
                    setting.ui.profile.hardwareParamIterms[2].tempCalibrate = result
                    setting.ui.profile.hardwareParamIterms[2].tempValue = result
                end
            else
                setting.ui.profile.hardwareParamIterms[2].tempCalibrate:SetNegativeInput(0)
                setting.ui.profile.hardwareParamIterms[2].tempCalibrate:SetReferenceVoltage(0)
                setting.ui.profile.hardwareParamIterms[2].tempCalibrate:SetCalibrationVoltage(0)
            end
        end,
        set = function()
            if dc:GetConnectStatus() then
                local status,result = pcall(function()
                    return dc:GetITemperatureControl():SetCalibrateFactorForTOC(1, setting.ui.profile.hardwareParamIterms[2].tempCalibrate)
                end)
                if not status then
                    ExceptionHandler.MakeAlarm(result)
                    return false, "设置温度校准系数失败\n"
                else
                    config.hardwareConfig.backupParam.CoolerTempCalibrate.negativeInput = string.format("%.4f", setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetNegativeInput())
                    config.hardwareConfig.backupParam.CoolerTempCalibrate.referenceVoltage =  string.format("%.4f", setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetReferenceVoltage())
                    config.hardwareConfig.backupParam.CoolerTempCalibrate.calibrationVoltage =  string.format("%.4f", setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetCalibrationVoltage())
                    setting.ui.profile.hardwareParamIterms.hardwareParamAuditTrail(setting.ui.profile.hardwareParamIterms[2].name)
                    return true, ""
                end
            else
                return false,"驱动板连接断开,\n设置温度校准系数失败\n"
            end
        end,
        synchronize = function()
            if dc:GetConnectStatus() then
                local status,result = pcall(function()
                    local PRECISE = 0.000001
                    local getTempCalibrate = dc:GetITemperatureControl():GetCalibrateFactorForTOC(1)
                    if math.abs(getTempCalibrate:GetNegativeInput() - config.hardwareConfig.backupParam.CoolerTempCalibrate.negativeInput) > PRECISE
                            or math.abs(getTempCalibrate:GetReferenceVoltage() - config.hardwareConfig.backupParam.CoolerTempCalibrate.referenceVoltage) > PRECISE
                            or math.abs(getTempCalibrate:GetCalibrationVoltage() - config.hardwareConfig.backupParam.CoolerTempCalibrate.calibrationVoltage) > PRECISE then

                        local tempCalibrate = TempCalibrateFactor.new()
                        tempCalibrate:SetNegativeInput(config.hardwareConfig.backupParam.CoolerTempCalibrate.negativeInput)
                        tempCalibrate:SetReferenceVoltage(config.hardwareConfig.backupParam.CoolerTempCalibrate.referenceVoltage)
                        tempCalibrate:SetCalibrationVoltage(config.hardwareConfig.backupParam.CoolerTempCalibrate.calibrationVoltage)

                        log:debug("参数同步-[制冷校准系数(V)-负输入分压-参考电压-校准电压] " ..  config.hardwareConfig.backupParam.CoolerTempCalibrate.negativeInput
                                .. ", "  .. config.hardwareConfig.backupParam.CoolerTempCalibrate.referenceVoltage
                                .. ", "  .. config.hardwareConfig.backupParam.CoolerTempCalibrate.calibrationVoltage)
                        dc:GetITemperatureControl():SetCalibrateFactorForTOC(1, tempCalibrate)
                        App.Sleep(200)

                        getTempCalibrate = dc:GetITemperatureControl():GetCalibrateFactorForTOC(1)
                        if math.abs(getTempCalibrate:GetNegativeInput() - config.hardwareConfig.backupParam.CoolerTempCalibrate.negativeInput) > PRECISE
                                or math.abs(getTempCalibrate:GetReferenceVoltage() - config.hardwareConfig.backupParam.CoolerTempCalibrate.referenceVoltage) > PRECISE
                                or math.abs(getTempCalibrate:GetCalibrationVoltage() - config.hardwareConfig.backupParam.CoolerTempCalibrate.calibrationVoltage) > PRECISE then
                            log:warn("同步制冷温度系数失败。")
                        else
                            log:warn("同步制冷温度系数成功。")
                        end
                    end
                end)
                if not status then
                    ExceptionHandler.MakeAlarm(result)
                    log:warn("同步制冷温度系数失败。")
                end
            end
        end,
        {
            name = "negativeInput",
            text = "负输入分压",
            refData = "tempCalibrate.negativeInput",
            unit = "V",
            get = function()
                return setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetNegativeInput()
            end,
            checkValue = function(value)
                if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                    return value
                else
                    return string.format("%.4f",setting.ui.profile.hardwareParamIterms[2][1].get())
                end
            end,
            set = function(value)
                setting.ui.profile.hardwareParamIterms[2].tempCalibrate:SetNegativeInput(value)
            end,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
        {
            name = "referenceVoltage",
            text = "参考电压",
            refData = "tempCalibrate.referenceVoltage",
            unit = "V",
            get = function()
                return setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetReferenceVoltage()
            end,
            checkValue = function(value)
                if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                    return value
                else
                    return string.format("%.4f",setting.ui.profile.hardwareParamIterms[2][2].get())
                end
            end,
            set = function(value)
                setting.ui.profile.hardwareParamIterms[2].tempCalibrate:SetReferenceVoltage(value)
            end,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
        {
            name = "calibrationVoltage",
            text = "校准电压",
            refData = "tempCalibrate.calibrationVoltage",
            unit = "V",
            get = function()
                return setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetCalibrationVoltage()
            end,
            checkValue = function(value)
                if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                    return value
                else
                    return string.format("%.4f",setting.ui.profile.hardwareParamIterms[2][3].get())
                end
            end,
            set = function(value)
                setting.ui.profile.hardwareParamIterms[2].tempCalibrate:SetCalibrationVoltage(value)
            end,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
        },
    },
    autoTempCalibrate =
    {
        {
            name = "tempCalibrate1",
            text = "温度1",
            index = 0,
            unit = "℃",
            checkValue = function(value)
                if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                    return value
                else
                    return ""
                end
            end,
            calibrateFunc = function(index, value)
                if dc:GetConnectStatus() then
                    local err,ret = pcall(function()
                        local tempAD = 0
                        for i=1,5 do
                            tempAD = tempAD + dc:GetStoveADValue()
                            App.Sleep(1000)
                        end
                        tempAD = tempAD/5
                        log:debug("getad " .. tempAD)
                        config.hardwareConfig.twoPointTempCalibrate.firstTempAD = tempAD
                        config.hardwareConfig.twoPointTempCalibrate.firstTemp = value
                        return true
                    end)

                    if not err then      -- 出现异常
                        if type(ret) == "userdata" then
                            log:warn("TemperatureCalibrate() =>" .. ret:What())
                        elseif type(ret) == "table" then
                            log:warn("TemperatureCalibrate() =>" .. ret:What())
                        elseif type(ret) == "string" then
                            log:warn("TemperatureCalibrate() =>" .. ret)	--C++、Lua系统异常
                        end
                    end
                else
                    log:debug("驱动板未连接")
                end
            end
        },
        {
            name = "tempCalibrate2",
            text = "温度2",
            index = 1,
            unit = "℃",
            checkValue = function(value)
                if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                    return value
                else
                    return ""
                end
            end,
            calibrateFunc = function(index, value)
                if dc:GetConnectStatus() then
                    local err,ret = pcall(function()
                        local tempAD = 0
                        for i=1,5 do
                            tempAD = tempAD + dc:GetStoveADValue()
                            App.Sleep(1000)
                        end
                        tempAD = tempAD/5
                        log:debug("getad " .. tempAD)
                        config.hardwareConfig.twoPointTempCalibrate.secondTempAD = tempAD
                        config.hardwareConfig.twoPointTempCalibrate.secondTemp = value
                        return true
                    end)
                    if not err then      -- 出现异常
                        if type(ret) == "userdata" then
                            log:warn("TemperatureCalibrate() =>" .. ret:What())
                        elseif type(ret) == "table" then
                            log:warn("TemperatureCalibrate() =>" .. ret:What())
                        elseif type(ret) == "string" then
                            log:warn("TemperatureCalibrate() =>" .. ret)	--C++、Lua系统异常
                        end
                    end
                else
                    log:debug("驱动板未连接")
                end
            end
        },
    },
    exAutoTempCalibrate =
    {
        {
            name = "tempCalibrate",
            text = "制冷温度",
            index = 1,
            unit = "℃",
            checkValue = function(value)
                if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                    return value
                else
                    return ""
                end
            end,
            calibrateFunc = function(index, value)
                if dc:GetConnectStatus() then
                    local tempCalibrate = TempCalibrateFactor.new()

                    local status,result = pcall(function()
                        return dc:GetITemperatureControl():GetCalibrateFactorForTOC(index)
                    end)

                    if not status then
                        ExceptionHandler.MakeAlarm(result)
                        return false
                    else
                        tempCalibrate = nil
                        tempCalibrate = result
                    end

                    local offsetFactor = 0.01
                    local startNegativeInput = result:GetNegativeInput()
                    local setNegativeInput = startNegativeInput - offsetFactor
                    local startTemperature = 0
                    local reviseTemperature = 0
                    local temperatureFactor
                    local temperatureTolerance = 1.5
                    for i=1,5 do
                        startTemperature = startTemperature + dc:GetReportThermostatTemp(setting.temperature.temperatureRefrigerator)
                        App.Sleep(1000)
                    end
                    startTemperature = startTemperature / 5
                    if value > startTemperature  then
                        setNegativeInput = startNegativeInput - offsetFactor
                    else
                        setNegativeInput = startNegativeInput + offsetFactor
                    end
                    tempCalibrate:SetNegativeInput(setNegativeInput)
                    log:debug("targetTemperature " .. value .. ", startTemperature " .. startTemperature .. ", startNegativeInput "  .. startNegativeInput .. ", setNegativeInput " .. setNegativeInput)

                    local status,result = pcall(function()
                        return dc:GetITemperatureControl():SetCalibrateFactorForTOC(index, tempCalibrate)
                    end)
                    if not status then
                        ExceptionHandler.MakeAlarm(result)
                        return false, "设置温度校准系数失败\n"
                    else
                        App.Sleep(1000)
                        for i=1,5 do
                            reviseTemperature = reviseTemperature + dc:GetReportThermostatTemp(setting.temperature.temperatureRefrigerator)
                            App.Sleep(1000)
                        end
                        reviseTemperature = reviseTemperature / 5

                        temperatureFactor = offsetFactor / (math.abs(reviseTemperature - startTemperature))

                        offsetFactor = temperatureFactor * (math.abs(value - startTemperature))

                        if value > startTemperature then
                            setNegativeInput = startNegativeInput - offsetFactor
                        else
                            setNegativeInput = startNegativeInput + offsetFactor
                        end
                        tempCalibrate:SetNegativeInput(setNegativeInput)
                        log:debug("reviseTemperature " .. reviseTemperature .. ", setNegativeInput " .. setNegativeInput .. ", factor " .. offsetFactor)

                        local status,result = pcall(function()
                            return dc:GetITemperatureControl():SetCalibrateFactorForTOC(index, tempCalibrate)
                        end)
                        App.Sleep(1000)
                        if not status then
                            ExceptionHandler.MakeAlarm(result)
                            return false, "设置温度校准系数失败\n"
                        else
                            reviseTemperature = 0
                            for i=1,5 do
                                reviseTemperature = reviseTemperature + dc:GetReportThermostatTemp(setting.temperature.temperatureRefrigerator)
                                App.Sleep(1000)
                            end

                            reviseTemperature = reviseTemperature / 5
                            if math.abs(value - reviseTemperature) < temperatureTolerance then
                                return true
                            else
                                return false
                            end
                        end
                    end

                else
                    log:debug("驱动板未连接")
                end
            end,
        },
        {
            name = "NDIRTempCalibrate",
            text = "测量温度",
            tempCalibrate = TempCalibrateFactor.new(),
            index = 2,
            unit = "℃",
            checkValue = function(value)
                if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                    return value
                else
                    return ""
                end
            end,
            calibrateFunc = function(index, value)
                if dc:GetConnectStatus() then
                    local tempCalibrate = TempCalibrateFactor.new()

                    local status,result = pcall(function()
                        return dc:GetITemperatureControl():GetCalibrateFactorForTOC(index)
                    end)

                    if not status then
                        ExceptionHandler.MakeAlarm(result)
                        return false
                    else
                        tempCalibrate = nil
                        tempCalibrate = result
                    end

                    local offsetFactor = 0.01
                    local startNegativeInput = result:GetNegativeInput()
                    local setNegativeInput = startNegativeInput - offsetFactor
                    local startTemperature = 0
                    local reviseTemperature = 0
                    local temperatureFactor
                    local temperatureTolerance = 1.5
                    for i=1,5 do
                        startTemperature = startTemperature + dc:GetReportThermostatTemp(setting.temperature.temperatureNDIR)
                        App.Sleep(1000)
                    end
                    startTemperature = startTemperature / 5
                    if value > startTemperature  then
                        setNegativeInput = startNegativeInput - offsetFactor
                    else
                        setNegativeInput = startNegativeInput + offsetFactor
                    end
                    tempCalibrate:SetNegativeInput(setNegativeInput)
                    log:debug("targetTemperature " .. value .. ", startTemperature " .. startTemperature .. ", startNegativeInput "  .. startNegativeInput .. ", setNegativeInput " .. setNegativeInput)

                    local status,result = pcall(function()
                        return dc:GetITemperatureControl():SetCalibrateFactorForTOC(index, tempCalibrate)
                    end)
                    if not status then
                        ExceptionHandler.MakeAlarm(result)
                        return false, "设置温度校准系数失败\n"
                    else
                        App.Sleep(1000)
                        for i=1,5 do
                            reviseTemperature = reviseTemperature + dc:GetReportThermostatTemp(setting.temperature.temperatureNDIR)
                            App.Sleep(1000)
                        end
                        reviseTemperature = reviseTemperature / 5

                        temperatureFactor = offsetFactor / (math.abs(reviseTemperature - startTemperature))

                        offsetFactor = temperatureFactor * (math.abs(value - startTemperature))

                        if value > startTemperature then
                            setNegativeInput = startNegativeInput - offsetFactor
                        else
                            setNegativeInput = startNegativeInput + offsetFactor
                        end
                        tempCalibrate:SetNegativeInput(setNegativeInput)
                        log:debug("reviseTemperature " .. reviseTemperature .. ", setNegativeInput " .. setNegativeInput .. ", factor " .. offsetFactor)

                        local status,result = pcall(function()
                            return dc:GetITemperatureControl():SetCalibrateFactorForTOC(index, tempCalibrate)
                        end)
                        App.Sleep(1000)
                        if not status then
                            ExceptionHandler.MakeAlarm(result)
                            return false, "设置温度校准系数失败\n"
                        else
                            reviseTemperature = 0
                            for i=1,5 do
                                reviseTemperature = reviseTemperature + dc:GetReportThermostatTemp(setting.temperature.temperatureNDIR)
                                App.Sleep(1000)
                            end

                            reviseTemperature = reviseTemperature / 5
                            if math.abs(value - reviseTemperature) < temperatureTolerance then
                                return true
                            else
                                return false
                            end
                        end
                    end

                else
                    log:debug("驱动板未连接")
                end
            end,
        },
        {
            name = "fanUpTempCalibrate",
            text = "上机箱温度",
            tempCalibrate = TempCalibrateFactor.new(),
            index = 3,
            unit = "℃",
            checkValue = function(value)
                if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                    return value
                else
                    return ""
                end
            end,
            calibrateFunc = function(index, value)
                if dc:GetConnectStatus() then
                    local tempCalibrate = TempCalibrateFactor.new()

                    local status,result = pcall(function()
                        return dc:GetITemperatureControl():GetCalibrateFactorForTOC(index)
                    end)

                    if not status then
                        ExceptionHandler.MakeAlarm(result)
                        return false
                    else
                        tempCalibrate = nil
                        tempCalibrate = result
                    end

                    local offsetFactor = 0.01
                    local startNegativeInput = result:GetNegativeInput()
                    local setNegativeInput = startNegativeInput - offsetFactor
                    local startTemperature = 0
                    local reviseTemperature = 0
                    local temperatureFactor
                    local temperatureTolerance = 1.5
                    for i=1,5 do
                        startTemperature = startTemperature + dc:GetEnvironmentTemperature()
                        App.Sleep(1000)
                    end
                    startTemperature = startTemperature / 5
                    if value > startTemperature  then
                        setNegativeInput = startNegativeInput - offsetFactor
                    else
                        setNegativeInput = startNegativeInput + offsetFactor
                    end
                    tempCalibrate:SetNegativeInput(setNegativeInput)
                    log:debug("targetTemperature " .. value .. ", startTemperature " .. startTemperature .. ", startNegativeInput "  .. startNegativeInput .. ", setNegativeInput " .. setNegativeInput)

                    local status,result = pcall(function()
                        return dc:GetITemperatureControl():SetCalibrateFactorForTOC(index, tempCalibrate)
                    end)
                    if not status then
                        ExceptionHandler.MakeAlarm(result)
                        return false, "设置温度校准系数失败\n"
                    else
                        App.Sleep(1000)
                        for i=1,5 do
                            reviseTemperature = reviseTemperature + dc:GetEnvironmentTemperature()
                            App.Sleep(1000)
                        end
                        reviseTemperature = reviseTemperature / 5

                        temperatureFactor = offsetFactor / (math.abs(reviseTemperature - startTemperature))

                        offsetFactor = temperatureFactor * (math.abs(value - startTemperature))

                        if value > startTemperature then
                            setNegativeInput = startNegativeInput - offsetFactor
                        else
                            setNegativeInput = startNegativeInput + offsetFactor
                        end
                        tempCalibrate:SetNegativeInput(setNegativeInput)
                        log:debug("reviseTemperature " .. reviseTemperature .. ", setNegativeInput " .. setNegativeInput .. ", factor " .. offsetFactor)

                        local status,result = pcall(function()
                            return dc:GetITemperatureControl():SetCalibrateFactorForTOC(index, tempCalibrate)
                        end)
                        App.Sleep(1000)
                        if not status then
                            ExceptionHandler.MakeAlarm(result)
                            return false, "设置温度校准系数失败\n"
                        else
                            reviseTemperature = 0
                            for i=1,5 do
                                reviseTemperature = reviseTemperature + dc:GetEnvironmentTemperature()
                                App.Sleep(1000)
                            end

                            reviseTemperature = reviseTemperature / 5
                            if math.abs(value - reviseTemperature) < temperatureTolerance then
                                return true
                            else
                                return false
                            end
                        end
                    end

                else
                    log:debug("驱动板未连接")
                end
            end,
        },
        {
            name = "fanDownTempCalibrate",
            text = "下机箱温度",
            tempCalibrate = TempCalibrateFactor.new(),
            index = 4,
            unit = "℃",
            checkValue = function(value)
                if setting.ui.profile.hardwareParamIterms.manyDecimalPattern(value) == true then
                    return value
                else
                    return ""
                end
            end,
            calibrateFunc = function(index, value)
                if dc:GetConnectStatus() then
                    local tempCalibrate = TempCalibrateFactor.new()

                    local status,result = pcall(function()
                        return dc:GetITemperatureControl():GetCalibrateFactorForTOC(index)
                    end)

                    if not status then
                        ExceptionHandler.MakeAlarm(result)
                        return false
                    else
                        tempCalibrate = nil
                        tempCalibrate = result
                    end

                    local offsetFactor = 0.01
                    local startNegativeInput = result:GetNegativeInput()
                    local setNegativeInput
                    local startTemperature = 0
                    local reviseTemperature = 0
                    local temperatureFactor
                    local temperatureTolerance = 1.5
                    for i=1,5 do
                        startTemperature = startTemperature + dc:GetReportThermostatTemp(setting.temperature.temperatureBoxLow)
                        App.Sleep(1000)
                    end
                    startTemperature = startTemperature / 5
                    if value > startTemperature  then
                        setNegativeInput = startNegativeInput - offsetFactor
                    else
                        setNegativeInput = startNegativeInput + offsetFactor
                    end
                    tempCalibrate:SetNegativeInput(setNegativeInput)
                    log:debug("targetTemperature " .. value .. ", startTemperature " .. startTemperature .. ", startNegativeInput "  .. startNegativeInput .. ", setNegativeInput " .. setNegativeInput)

                    local status,result = pcall(function()
                        return dc:GetITemperatureControl():SetCalibrateFactorForTOC(index, tempCalibrate)
                    end)
                    if not status then
                        ExceptionHandler.MakeAlarm(result)
                        return false, "设置温度校准系数失败\n"
                    else
                        App.Sleep(1000)
                        for i=1,5 do
                            reviseTemperature = reviseTemperature + dc:GetReportThermostatTemp(setting.temperature.temperatureBoxLow)
                            App.Sleep(1000)
                        end
                        reviseTemperature = reviseTemperature / 5

                        temperatureFactor = offsetFactor / (math.abs(reviseTemperature - startTemperature))

                        offsetFactor = temperatureFactor * (math.abs(value - startTemperature))

                        if value > startTemperature then
                            setNegativeInput = startNegativeInput - offsetFactor
                        else
                            setNegativeInput = startNegativeInput + offsetFactor
                        end
                        tempCalibrate:SetNegativeInput(setNegativeInput)
                        log:debug("reviseTemperature " .. reviseTemperature .. ", setNegativeInput " .. setNegativeInput .. ", factor " .. offsetFactor)

                        local status,result = pcall(function()
                            return dc:GetITemperatureControl():SetCalibrateFactorForTOC(index, tempCalibrate)
                        end)
                        App.Sleep(1000)
                        if not status then
                            ExceptionHandler.MakeAlarm(result)
                            return false, "设置温度校准系数失败\n"
                        else
                            reviseTemperature = 0
                            for i=1,5 do
                                reviseTemperature = reviseTemperature + dc:GetReportThermostatTemp(setting.temperature.temperatureBoxLow)
                                App.Sleep(1000)
                            end

                            reviseTemperature = reviseTemperature / 5
                            if math.abs(value - reviseTemperature) < temperatureTolerance then
                                return true
                            else
                                return false
                            end
                        end
                    end

                else
                    log:debug("驱动板未连接")
                end
            end,
        },
    },
    manyDecimalPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^%d?%d?%d%.%d%d?%d?%d?%d?%d?%d?%d?$"
            local integerPatterm = "^%d?%d?%d$"
            if not string.find(value, decimalPatterm) then
                if string.find(value, integerPatterm) then
                    ret = true
                end
            else
                ret = true
            end
            return ret
        else
            return false
        end
    end,
    twoDecimalPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^%d?%d?%d%.%d%d?$"
            local integerPatterm = "^%d?%d?%d$"
            if not string.find(value, decimalPatterm) then
                if string.find(value, integerPatterm) then
                    ret = true
                end
            else
                ret = true
            end
            return ret
        else
            return false
        end
    end,
    threeDecimalPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^%d?%d?%d%.%d%d?%d?$"
            local integerPatterm = "^%d?%d?%d$"
            if not string.find(value, decimalPatterm) then
                if string.find(value, integerPatterm) then
                    ret = true
                end
            else
                ret = true
            end
            return ret
        else
            return false
        end
    end,
    integerPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local integerPatterm = "^%d?%d?%d?%d$"
            if string.find(value, integerPatterm) then
                ret = true
            end
            return ret
        else
            return false
        end
    end,
    electricPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^[-+]?%d?%d?%d?%d%.%d%d?$"
            local integerPatterm = "^[-+]?%d?%d?%d?%d$"
            if not string.find(value, decimalPatterm) then
                if string.find(value, integerPatterm) then
                    ret = true
                end
            else
                ret = true
            end
            return ret
        else
            return false
        end
    end,
    symbolIntegerPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^[-+]?%d?%d?%d?%d$"
            local integerPatterm = "^[-+]?%d?%d?%d?%d$"
            if not string.find(value, decimalPatterm) then
                if string.find(value, integerPatterm) then
                    ret = true
                end
            else
                ret = true
            end
            return ret
        else
            return false
        end
    end,
    hardwareParamAuditTrail = function(itemName)
        if type(itemName) == "string" then
            local eventStr = "--"
            local oldSetting = "--"
            local newSetting = "--"
            local PRECISE = 0.000001
            if itemName == "meterPump" then
                local oldValue = setting.ui.profile.hardwareParamIterms[1].tempValue[1]
                local newValue = setting.ui.profile.hardwareParamIterms[1].pumpFactor[1]
                if math.abs(oldValue - newValue) > PRECISE then
                    eventStr = setting.ui.profile.hardwareParamIterms.text .. "-" .. setting.ui.profile.hardwareParamIterms[1].text .. "-"
                            .. setting.ui.profile.hardwareParamIterms[1][1].text .. " 更改"
                    oldSetting = string.format("%.8f", oldValue)
                    newSetting = string.format("%.8f", newValue)
                    SaveToAuditTrailSqlite(nil, nil, eventStr, oldSetting, newSetting, nil)
                end
            elseif itemName == "SyringePump" then
                local oldValue = setting.ui.profile.hardwareParamIterms[1].tempValue[2]
                local newValue = setting.ui.profile.hardwareParamIterms[1].pumpFactor[2]
                if math.abs(oldValue - newValue) > PRECISE then
                    eventStr = setting.ui.profile.hardwareParamIterms.text .. "-" .. setting.ui.profile.hardwareParamIterms[1].text .. "-"
                            .. setting.ui.profile.hardwareParamIterms[1][2].text .. " 更改"
                    oldSetting = string.format("%.8f", oldValue)
                    newSetting = string.format("%.8f", newValue)
                    SaveToAuditTrailSqlite(nil, nil, eventStr, oldSetting, newSetting, nil)
                end
            elseif itemName == "tempCalibrate" then
                local newValueNI = setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetNegativeInput()
                local newValueRV = setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetReferenceVoltage()
                local newValueCV = setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetCalibrationVoltage()

                local oldValueNI = setting.ui.profile.hardwareParamIterms[2].tempValue:GetNegativeInput()
                local oldValueRV = setting.ui.profile.hardwareParamIterms[2].tempValue:GetReferenceVoltage()
                local oldValueCV = setting.ui.profile.hardwareParamIterms[2].tempValue:GetCalibrationVoltage()

                if math.abs(newValueNI - oldValueNI) > PRECISE then
                    eventStr = setting.ui.profile.hardwareParamIterms.text .. "-" .. setting.ui.profile.hardwareParamIterms[2].text .. "-"
                            .. setting.ui.profile.hardwareParamIterms[2][1].text .. " 更改"
                    oldSetting = string.format("%.4f", oldValueNI)
                    newSetting = string.format("%.4f", newValueNI)
                    SaveToAuditTrailSqlite(nil, nil, eventStr, oldSetting, newSetting, nil)
                end
                if math.abs(newValueRV - oldValueRV) > PRECISE then
                    eventStr = setting.ui.profile.hardwareParamIterms.text .. "-" .. setting.ui.profile.hardwareParamIterms[2].text .. "-"
                            .. setting.ui.profile.hardwareParamIterms[2][2].text .. " 更改"
                    oldSetting = string.format("%.4f", oldValueRV)
                    newSetting = string.format("%.4f", newValueRV)
                    SaveToAuditTrailSqlite(nil, nil, eventStr, oldSetting, newSetting, nil)
                end
                if math.abs(newValueCV - oldValueCV) > PRECISE then
                    eventStr = setting.ui.profile.hardwareParamIterms.text .. "-" .. setting.ui.profile.hardwareParamIterms[2].text .. "-"
                            .. setting.ui.profile.hardwareParamIterms[2][3].text .. " 更改"
                    oldSetting = string.format("%.4f", oldValueCV)
                    newSetting = string.format("%.4f", newValueCV)
                    SaveToAuditTrailSqlite(nil, nil, eventStr, oldSetting, newSetting, nil)
                end
            end


        else
            return false
        end
    end,
    synchronize = function()
        if config.hardwareConfig.backupSign then

        end
    end,

    backupParam = function()
        local ret = false
        if dc:GetConnectStatus() then
            config.hardwareConfig.backupSign = true
            --泵系数
            config.hardwareConfig.backupParam.pumpMeter = string.format("%.8f", setting.ui.profile.hardwareParamIterms[1].pumpFactor[1])
            config.hardwareConfig.backupParam.pumpSyring = string.format("%.8f", setting.ui.profile.hardwareParamIterms[1].pumpFactor[2])
            log:debug("[定量泵系数(ml/步)] " .. config.hardwareConfig.backupParam.pumpMeter)
            log:debug("[注射泵系数(ml/步)] " .. config.hardwareConfig.backupParam.pumpSyring)

            --制冷温度
            config.hardwareConfig.backupParam.CoolerTempCalibrate.negativeInput = string.format("%.4f", setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetNegativeInput())
            config.hardwareConfig.backupParam.CoolerTempCalibrate.referenceVoltage =  string.format("%.4f", setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetReferenceVoltage())
            config.hardwareConfig.backupParam.CoolerTempCalibrate.calibrationVoltage =  string.format("%.4f", setting.ui.profile.hardwareParamIterms[2].tempCalibrate:GetCalibrationVoltage())
            log:debug("[制冷校准系数(V)-负输入分压-参考电压-校准电压] " ..  config.hardwareConfig.backupParam.CoolerTempCalibrate.negativeInput
                    .. ", "  .. config.hardwareConfig.backupParam.CoolerTempCalibrate.referenceVoltage
                    .. ", "  .. config.hardwareConfig.backupParam.CoolerTempCalibrate.calibrationVoltage)

            ConfigLists.SaveHardwareConfig()
            log:info("参数备份成功")
            SaveToAuditTrailSqlite(nil, nil, "参数备份成功", nil, nil, nil)
            ret = true
        else
            log:warn("通信异常，参数备份失败")
        end

        return ret
    end,
}
