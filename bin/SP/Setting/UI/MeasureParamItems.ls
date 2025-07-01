setting.ui.profile = {}
setting.ui.profile.measureParam =
{
    name = "measureParam",
    text = "测量参数",
    updateEvent = UpdateEvent.ChangeMeasureParam,
    index = 2,
    rowCount = 33,
    superRow = 27,
    administratorRow = 6,
    writePrivilege=  RoleType.Maintain,
    readPrivilege = RoleType.Maintain,
    isMeaParaml = true,
    -- 1
    {
        name = "currentRangeParam",
        text = "当前量程信息",
        {
            name = "curveK",
            text = "斜率",
            type = DataType.Double,
            unit = setting.measureResult.curve[1].curveKLowLimit.."~"..setting.measureResult.curve[1].curveKUpLimit,
            rangeParamStart = true,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Maintain,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(value) == true then
                    setting.ui.profile.measureParam[1][1].currentValue = tonumber(value)
                    return value
                else
                    local retK = config.measureParam.curveK
                    local retB = config.measureParam.curveB
                    retK, retB = CurveParamCurveXYChange(retK, retB)
                    setting.ui.profile.measureParam[1][1].currentValue = retK
                    return string.format("%.6f", retK)
                end
            end,
            curveParamCurveXYChange = function(value, isUpdateCurrentValue)
                local retK = value
                local retB = config.measureParam.curveB
                if setting.ui.profile.measureParam[1][2].currentValue ~= nil and isUpdateCurrentValue == false then
                    retB = setting.ui.profile.measureParam[1][2].currentValue
                end

                retK, retB = CurveParamCurveXYChange(retK, retB)
                if isUpdateCurrentValue == true then
                    setting.ui.profile.measureParam[1][1].currentValue = retK
                else
                    -- 必须在函数checkValue更新，此时是原始值
                end
                return retK
            end,
        },
        {
            name = "curveB",
            text = "截距",
            type = DataType.Double,
            unit = setting.measureResult.curve[1].curveBLowLimit.."~"..setting.measureResult.curve[1].curveBUpLimit,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            currentValue = nil,
            checkValue = function(value)
                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(value) == true then
                    setting.ui.profile.measureParam[2][2].currentValue = tonumber(value)
                    return value
                else
                    local retK = config.measureParam.curveK
                    local retB = config.measureParam.curveB
                    retK, retB = CurveParamCurveXYChange(retK, retB)
                    setting.ui.profile.measureParam[2][2].currentValue = retB
                    return string.format("%.6f", retB)
                end
            end,
            curveParamCurveXYChange = function(value, isUpdateCurrentValue)
                local retK = config.measureParam.curveK
                local retB = value
                if setting.ui.profile.measureParam[2][1].currentValue ~= nil and isUpdateCurrentValue == false then
                    retK = setting.ui.profile.measureParam[2][1].currentValue
                end

                retK, retB = CurveParamCurveXYChange(retK, retB)
                if isUpdateCurrentValue == true then
                    setting.ui.profile.measureParam[2][2].currentValue = retB
                else
                    -- 必须在函数checkValue更新，此时是原始值
                end
                return retB
            end,
        },
        {
            name = "timeStr",
            text = "校准时间",
            type = DataType.String,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Maintain,
            checkValue = function(value)
                return config.measureParam.timeStr
            end,
        },
    },
    {
        name = "revise",
        text = "修正",
        {
            name = "negativeRevise",
            text = "负值修正",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name = "reviseFactor",
            text = "斜率修正",
            type = DataType.Float,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            checkValue = function(value)
                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(value) == true then
                    return value
                else
                    return string.format("%.4f", config.measureParam.reviseFactor)
                end
            end,
        },
        {
            name = "shiftFactor",
            text = "截距修正",
            type = DataType.Float,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(value) == true then
                    return value
                else
                    return string.format("%.4f", config.measureParam.shiftFactor)
                end
            end,
        },
        {
            name = "measureDataOffsetValve",
            text = "偏移量",
            type = DataType.Float,
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.measureParam.fourDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if num > setting.measureResult.quantifyLowLimit*2 or num < -(setting.measureResult.quantifyLowLimit*2) then
                        return string.format("%.4f", config.measureParam.measureDataOffsetValve)
                    else
                        return value
                    end
                else
                    return string.format("%.4f", config.measureParam.measureDataOffsetValve)
                end
            end,
        },
    },
    {
        name = "pharmacopoeia",
        text = "药典测试",
        {
            name = "pharmacopoeia",
            text = "药典设置",
            type = DataType.IntArray,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
    },
    {
        name = "Turbo",
        text = "Turbo设置",
        {
            name = "turboMode",
            text = "Turbo模式",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name = "curveKTurbo",
            text = "斜率(Turbo)",
            type = DataType.Float,
            unit = "",
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(value) == true then
                    return value
                else
                    return string.format("%.6f", config.measureParam.curveKTurbo)
                end
            end,
        },
        {
            name = "curveBTurbo",
            text = "截距(Turbo)",
            type = DataType.Float,
            unit = "",
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(value) == true then
                    return value
                else
                    return string.format("%.6f", config.measureParam.curveBTurbo)
                end
            end,
        },
        {
            name = "TCTurboCurveK",
            text = "TC修正(Turbo)",
            type = DataType.Float,
            unit = "",
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(value) == true then
                    return value
                else
                    return string.format("%.6f", config.measureParam.TCTurboCurveK)
                end
            end,
        },
        {
            name = "ICTurboCurveK",
            text = "IC修正(Turbo)",
            type = DataType.Float,
            unit = "",
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(value) == true then
                    return value
                else
                    return string.format("%.6f", config.measureParam.ICTurboCurveK)
                end
            end,
        },
    },
    {
        name = "UVLampControl",
        text = "紫外灯",
        {
            name = "isUseUVLamp",
            text = "紫外灯控制",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
    },
    {
        name = "SpeedParam",
        text = "流速参数",
       {
            name = "sampleQuickSpeed",
            text = "快速冲洗速度",
            unit = "uL/min",
            type = DataType.Float,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            checkValue = function(value)
                if setting.ui.profile.measureParam.twoDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if num < 0 or num > 100 then
                        return string.format("%.1f", config.measureParam.sampleQuickSpeed)
                    else
                        return value
                    end
                else
                    return string.format("%.1f", config.measureParam.sampleQuickSpeed)
                end
            end,
        },
        {
            name = "sampleSlowSpeed",
            text = "慢速冲洗速度",
            unit = "uL/min",
            type = DataType.Float,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            checkValue = function(value)
                if setting.ui.profile.measureParam.twoDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if num < 0 or num > 100 then
                        return string.format("%.1f", config.measureParam.sampleSlowSpeed)
                    else
                        return value
                    end
                else
                    return string.format("%.1f", config.measureParam.sampleSlowSpeed)
                end
            end,
        },
    },
    {
        name = "debugParam",
        text = "调试参数",
        {
            name = "meaType",
            text = "类型",
            type = DataType.Option,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            options =
            {
                "在线",
                "离线",
            },
        },
        {
            name = "methodName",
            text = "方法名称",
            type = DataType.String,
            unit = "",
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            checkValue = function(value)
                return config.measureParam.methodName
            end,
        },
        {
            name = "ICRMode",
            text = "ICR模式",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name = "TOCMode",
            text = "TOC测量",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name = "ECMode",
            text = "电导率测量",
            type = DataType.Bool,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
        },
        {
            name = "reagent1Vol",
            text = "酸剂",
            unit = "uL",
            type = DataType.Float,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            checkValue = function(value)
                if setting.ui.profile.measureParam.twoDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if num < 0 or num > 100 then
                        return string.format("%.1f", config.measureParam.reagent1Vol)
                    else
                        return value
                    end
                else
                    return string.format("%.1f", config.measureParam.reagent1Vol)
                end
            end,
        },
        {
            name = "reagent2Vol",
            text = "氧化剂",
            unit = "uL",
            type = DataType.Float,
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            checkValue = function(value)
                if setting.ui.profile.measureParam.twoDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if num < 0 or num > 100 then
                        return string.format("%.1f", config.measureParam.reagent2Vol)
                    else
                        return value
                    end
                else
                    return string.format("%.1f", config.measureParam.reagent2Vol)
                end
            end,
        },
        {
            name = "normalRefreshTime",
            text = "慢速冲洗时间",
            type = DataType.Int,
            unit = "秒",
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            checkValue = function(value)
                if setting.ui.profile.measureParam.timePattern(value) == true then
                    return value
                else
                    return tostring(config.measureParam.normalRefreshTime)
                end
            end,
        },
        {
            name = "quickRefreshTime",
            text = "快速冲洗时间",
            type = DataType.Int,
            unit = "秒",
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            checkValue = function(value)
                if setting.ui.profile.measureParam.timePattern(value) == true then
                    return value
                else
                    return tostring(config.measureParam.quickRefreshTime)
                end
            end,
        },
        {
            name = "mixSampleTime",
            text = "水样混合时间",
            type = DataType.Int,
            unit = "秒",
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            checkValue = function(value)
                if setting.ui.profile.measureParam.timePattern(value) == true then
                    return value
                else
                    return tostring(config.measureParam.mixSampleTime)
                end
            end,
        },
        {
            name = "reactTime",
            text = "反应时间",
            type = DataType.Float,
            unit = "秒",
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.measureParam.twoDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if num >= 0 and num <= 1000 then
                        return value
                    else
                        return string.format("%.2f", config.measureParam.reactTime)
                    end
                else
                    return string.format("%.2f", config.measureParam.reactTime)
                end
            end,
        },
        {
            name = "TCConstant",
            text = "TC电导池常数",
            type = DataType.Float,
            unit = "",
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(value) == true then
                    return value
                else
                    return string.format("%.6f", config.measureParam.TCConstant)
                end
            end,
        },
        {
            name = "ICConstant",
            text = "IC电导池常数",
            type = DataType.Float,
            unit = "",
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(value) == true then
                    return value
                else
                    return string.format("%.6f", config.measureParam.ICConstant)
                end
            end,
        },
        {
            name = "TCCurveK",
            text = "TC斜率",
            type = DataType.Float,
            unit = "",
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(value) == true then
                    return value
                else
                    return string.format("%.6f", config.measureParam.TCCurveK)
                end
            end,
        },
        {
            name = "ICCurveK",
            text = "IC斜率",
            type = DataType.Float,
            unit = "",
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.measureParam.fourDecimalWithNegativePattern(value) == true then
                    return value
                else
                    return string.format("%.6f", config.measureParam.ICCurveK)
                end
            end,
        },
        {
            name = "windowTime",
            text = "窗口时间",
            type = DataType.Float,
            unit = "秒",
            writePrivilege=  RoleType.Super,
            readPrivilege = RoleType.Super,
            checkValue = function(value)
                if setting.ui.profile.measureParam.twoDecimalPattern(value) == true then
                    local num = tonumber(value)
                    if num >= 0 and num <= 1000 then
                        return value
                    else
                        return string.format("%.2f", config.measureParam.windowTime)
                    end
                else
                    return string.format("%.2f", config.measureParam.windowTime)
                end
            end,
        },
    },
    isShowCheck = function()
        return true
    end,
    checkOEM = function()
        return config.system.OEM
    end,
    defaultRestore = function(userType, defaultReviseParameter)

        if userType == RoleType.Super then
            local RangeConsistency = {}
            local ZeroConsistency = {}
            local timeStr = {}
            local curveB = {}
            local curveK = {}
            local reviseParameter = {}
            for i = 1,setting.measure.range.rangeNum do
                RangeConsistency[i] = config.measureParam.curveParam[i].RangeConsistency
                ZeroConsistency[i] = config.measureParam.curveParam[i].ZeroConsistency
                timeStr[i] = config.measureParam.curveParam[i].timeStr
                curveB[i] = config.measureParam.curveParam[i].curveB
                curveK[i] = config.measureParam.curveParam[i].curveK
            end

            for k,v in pairs(config.measureParam.reviseParameter) do
                table.insert(reviseParameter, config.measureParam.reviseParameter[k])
            end

            local defaultTable = ConfigLists.LoadMeasureParamConfig(true)
            Helper.DefaultRestore(defaultTable, config.measureParam)

            for i = 1,setting.measure.range.rangeNum do
                config.measureParam.curveParam[i].RangeConsistency = RangeConsistency[i]
                config.measureParam.curveParam[i].ZeroConsistency = ZeroConsistency[i]
                config.measureParam.curveParam[i].timeStr = timeStr[i]
                config.measureParam.curveParam[i].curveB = curveB[i]
                config.measureParam.curveParam[i].curveK = curveK[i]
            end

            if false == defaultReviseParameter then
                --校准参数不恢复
                for k,v in pairs(config.measureParam.reviseParameter) do
                    config.measureParam.reviseParameter[k] = reviseParameter[k]
                end
            else
                for k,v in pairs(config.measureParam.reviseParameter) do
                    log:debug("校正参数 "..k.." 恢复默认 ,修改前值为 "..reviseParameter[k])
                end
            end

            for i = 1,setting.measure.range.rangeNum do
                config.measureParam.curveParam[i].RangeConsistency = RangeConsistency[i]
                config.measureParam.curveParam[i].ZeroConsistency = ZeroConsistency[i]
                config.measureParam.curveParam[i].timeStr = timeStr[i]
                config.measureParam.curveParam[i].curveB = curveB[i]
                config.measureParam.curveParam[i].curveK = curveK[i]
            end
            config.measureParam.temperatureIncrement = defaultTable.temperatureIncrement
            config.measureParam.measureLedAD.reference = defaultTable.measureLedAD.reference
            config.measureParam.measureLedAD.measure = defaultTable.measureLedAD.measure
            config.measureParam.meterLedAD[1] = defaultTable.meterLedAD[1]
            config.measureParam.meterLedAD[2] = defaultTable.meterLedAD[2]
            config.measureParam.measureDataOffsetValve = defaultTable.measureDataOffsetValve
            config.measureParam.readInitRilentTime = defaultTable.readInitRilentTime
            config.measureParam.reactTemperature = defaultTable.reactTemperature
            config.measureParam.reactTime = defaultTable.reactTime
            config.measureParam.checkConsistency = defaultTable.checkConsistency
            log:info(Helper.GetRoleTypeStr().." 恢复默认"..setting.ui.profile.measureParam.text)
            config.modifyRecord.measureParam(true)
            ConfigLists.SaveMeasureParamConfig()

        elseif userType == RoleType.Administrator then

            local defaultTable = ConfigLists.LoadMeasureParamConfig(true)
            config.measureParam.currentRange = defaultTable.currentRange
            config.measureParam.calibrateRangeIndex = defaultTable.calibrateRangeIndex
            config.measureParam.zeroCheckRangeIndex = defaultTable.zeroCheckRangeIndex
            config.measureParam.rangeCheckRangeIndex = defaultTable.rangeCheckRangeIndex
            config.measureParam.autoChangeRange = defaultTable.autoChangeRange
            config.measureParam.activeRangeMode = defaultTable.activeRangeMode
            config.measureParam.range[1] = defaultTable.range[1]
            config.measureParam.range[2] = defaultTable.range[2]
            config.measureParam.range[3] = defaultTable.range[3]

            config.measureParam.negativeRevise = defaultTable.negativeRevise
            config.measureParam.reviseFactor = defaultTable.reviseFactor
            config.measureParam.checkConsistency = defaultTable.checkConsistency
            config.measureParam.checkErrorLimit = defaultTable.checkErrorLimit
            config.measureParam.failAutoRevise = defaultTable.failAutoRevise
            config.measureParam.accurateCheck = defaultTable.accurateCheck
            config.measureParam.wasteWaterEnvironment = defaultTable.wasteWaterEnvironment
            config.measureParam.highSaltMode = defaultTable.highSaltMode
            config.measureParam.cleanBefMeaBlankVol = defaultTable.cleanBefMeaBlankVol
            config.measureParam.cleanAftMeaBlankVol = defaultTable.cleanAftMeaBlankVol
            config.measureParam.extendSamplePipeVolume = defaultTable.extendSamplePipeVolume
            config.measureParam.sampleRenewVolume = defaultTable.sampleRenewVolume
            config.measureParam.zeroAccurateCalibrate = defaultTable.zeroAccurateCalibrate
            config.measureParam.standardAccurateCalibrate = defaultTable.standardAccurateCalibrate

            config.measureParam.rangeAccurateCalibrate = defaultTable.rangeAccurateCalibrate
            config.measureParam.rangeCalibrateDeviation = defaultTable.rangeCalibrateDeviation
            config.measureParam.curveQualifiedDetermination = defaultTable.curveQualifiedDetermination
            config.measureParam.highClMode = defaultTable.highClMode
            config.measureParam.checkConsistency = defaultTable.checkConsistency
            log:info(Helper.GetRoleTypeStr().." 恢复默认"..setting.ui.profile.measureParam.text)
            config.modifyRecord.measureParam(true)
            ConfigLists.SaveMeasureParamConfig()

        elseif userType == RoleType.Maintain then

            local defaultTable = ConfigLists.LoadMeasureParamConfig(true)
            config.measureParam.currentRange = defaultTable.currentRange
            config.measureParam.calibrateRangeIndex = defaultTable.calibrateRangeIndex
            config.measureParam.zeroCheckRangeIndex = defaultTable.zeroCheckRangeIndex
            config.measureParam.rangeCheckRangeIndex = defaultTable.rangeCheckRangeIndex
            config.measureParam.autoChangeRange = defaultTable.autoChangeRange
            config.measureParam.activeRangeMode = defaultTable.activeRangeMode
            config.measureParam.range[1] = defaultTable.range[1]
            config.measureParam.range[2] = defaultTable.range[2]
            config.measureParam.range[3] = defaultTable.range[3]

            log:info(Helper.GetRoleTypeStr().." 恢复默认"..setting.ui.profile.measureParam.text)
            config.modifyRecord.measureParam(true)
            ConfigLists.SaveMeasureParamConfig()
        end
        setting.ui.profile.measureParam.updaterCurveParam(0,true)

        return false  --无需重启仪器
    end,
    saveFile = function(isUpdate, saveCurveParam)
        local flowManager = FlowManager.Instance()
        log:info(Helper.GetRoleTypeStr() .. " 修改" .. setting.ui.profile.measureParam.text)
        config.modifyRecord.measureParam(isUpdate)
        ConfigLists.SaveMeasureParamConfig()
        if saveCurveParam then
            setting.ui.profile.measureParam.checkCurveParamChange()
        end

        ConfigLists.SaveMeasureParamConfig()
        ConfigLists.SaveMeasureStatus()

        return false  --无需重启仪器
    end,

    checkCurveParamChange = function()

        local currentRange  = config.measureParam.range[config.measureParam.currentRange+1]+1
        config.measureParam.curveParam[currentRange].curveK = config.measureParam.curveK
        config.measureParam.curveParam[currentRange].curveB = config.measureParam.curveB
        config.measureParam.curveParam[currentRange].timeStr = config.measureParam.timeStr

        local calibrateRange= config.measureParam.range[config.measureParam.calibrateRangeIndex+1]+1
        config.measureParam.curveParam[calibrateRange].ZeroConsistency = config.measureParam.ZeroConsistency
        config.measureParam.curveParam[calibrateRange].RangeConsistency = config.measureParam.RangeConsistency

        local rangeNum =  setting.measure.range.rangeNum
        for i = 1, rangeNum do
            if math.abs(config.measureParam.curveParam[i].curveK - status.measure.calibrate[i].curveK) > 0.00001
                    or math.abs(config.measureParam.curveParam[i].curveB - status.measure.calibrate[i].curveB) > 0.00001 then
                local consistency = {config.measureParam.curveParam[i].ZeroConsistency,config.measureParam.curveParam[i].RangeConsistency}
                local saveTime = os.time()
                local absorbance1, absorbance2
                absorbance1, absorbance2 = op:CalculateAbsorbance(config.measureParam.curveParam[i].curveK,config.measureParam.curveParam[i].curveB,consistency[1],consistency[2])
                Helper.Result.OnCalibrateResultAdded(saveTime, saveTime, saveTime, config.measureParam.curveParam[i].curveK, config.measureParam.curveParam[i].curveB, consistency[1], consistency[2], absorbance1, absorbance2, i,true)
                op:SaveCalibrationTimeStr(saveTime,i)
                local calibrateDateTime = math.floor((2300+math.random()*300)+(2300+math.random()*300))
                op:SaveUserCurve(saveTime, config.measureParam.curveParam[i].curveK, config.measureParam.curveParam[i].curveB, absorbance1, absorbance2, consistency[1], consistency[2], calibrateDateTime, config.system.rangeViewMap[i].view)
                --App.Sleep(100)
                --config.measureParam.timeStr = config.measureParam.curveParam[i].timeStr
            end
        end
    end,

    --获取KB值合理范围
    getCurveKBRange = function(rangeIndex,useMeasureParamConfig)
        local index
        if false == useMeasureParamConfig then
            index = tonumber(rangeIndex)+1
        else
            index = config.measureParam.range[config.measureParam.currentRange+1]+1
        end
        local curveKRange = ""
        local curveBRange = ""

        if nil ~= setting.measureResult.curve[index].curveKLowLimit and nil ~=setting.measureResult.curve[index].curveKUpLimit then
            curveKRange = setting.measureResult.curve[index].curveKLowLimit.."~"..setting.measureResult.curve[index].curveKUpLimit
        end

        if nil ~= setting.measureResult.curve[index].curveBLowLimit and nil ~=setting.measureResult.curve[index].curveBUpLimit then
            curveBRange = setting.measureResult.curve[index].curveBLowLimit.."~"..setting.measureResult.curve[index].curveBUpLimit
        end

        return curveKRange,curveBRange
    end,

    --更新当前标线参数
    updaterCurveParam = function(rangeIndex,useMeasureParamConfig, isCalibrateIndex)
        local currentRange
        local calibrateRange
        if false == useMeasureParamConfig then
            currentRange = tonumber(rangeIndex)+1
            calibrateRange = tonumber(rangeIndex)+1
        else
            currentRange = config.measureParam.range[config.measureParam.currentRange+1]+1
            calibrateRange = config.measureParam.range[config.measureParam.calibrateRangeIndex+1]+1
        end

        if nil ~= isCalibrateIndex and isCalibrateIndex then
            config.measureParam.ZeroConsistency = config.measureParam.curveParam[calibrateRange].ZeroConsistency
            config.measureParam.RangeConsistency = config.measureParam.curveParam[calibrateRange].RangeConsistency
        elseif true == useMeasureParamConfig then
            config.measureParam.curveK = config.measureParam.curveParam[currentRange].curveK
            config.measureParam.curveB = config.measureParam.curveParam[currentRange].curveB
            config.measureParam.timeStr = config.measureParam.curveParam[currentRange].timeStr
            config.measureParam.ZeroConsistency = config.measureParam.curveParam[calibrateRange].ZeroConsistency
            config.measureParam.RangeConsistency = config.measureParam.curveParam[calibrateRange].RangeConsistency
        else
            config.measureParam.curveK = config.measureParam.curveParam[currentRange].curveK
            config.measureParam.curveB = config.measureParam.curveParam[currentRange].curveB
            config.measureParam.timeStr = config.measureParam.curveParam[currentRange].timeStr
        end

        ConfigLists.SaveMeasureParamConfig()
    end,

    tempPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^[+-]?[0-1]?%d?%d%.%d$"
            local integerPatterm = "^[+-]?[0-1]?%d?%d$"
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
    timePattern = function(value)
        if type(value) == "string" then
            local patterm = "^%d?%d?%d?%d$"
            if not string.find(value, patterm) then
                return false
            else
                return true
            end
        else
            return false
        end
    end,
    arguPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^[-+]?%d?%d?%d?%d%.%d%d?%d?%d?%d?%d?$"
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
    fourDecimalPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^%d?%d?%d?%d%.%d%d?%d?%d?$"
            local integerPatterm = "^%d?%d?%d?%d$"
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
    fourDecimalWithNegativePattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^%d?%d?%d?%d%.%d%d?%d?%d?$"
            local negativeDecimalPatterm = "^-%d?%d?%d?%d%.%d%d?%d?%d?$"
            local integerPatterm = "^%d?%d?%d?%d$"
            local negativeIntegerPatterm = "^-%d?%d?%d?%d$"
            if string.find(value, decimalPatterm) then
                ret = true
            elseif string.find(value, negativeDecimalPatterm) then
                ret = true
            elseif string.find(value, integerPatterm) then
                ret = true
            elseif string.find(value, negativeIntegerPatterm) then
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
            local decimalPatterm = "^%d?%d?%d?%d%.%d%d?%d?$"
            local integerPatterm = "^%d?%d?%d?%d$"
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
            local decimalPatterm = "^%d?%d?%d?%d?%d%.%d%d?$"
            local integerPatterm = "^%d?%d?%d?%d$"
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
            local integerPatterm = "^%d?%d?%d?%d?%d?%d?%d?%d$"
            if string.find(value, integerPatterm) then
                ret = true
            end
            return ret
        else
            return false
        end
    end,
    meterPointMultipleCheck = function(value)

        local pointNum = config.hardwareConfig.meterPoint.num
        local maxPoint = config.hardwareConfig.meterPoint.point[pointNum].setVolume
        local minPoint = config.hardwareConfig.meterPoint.point[1].setVolume
        local PRECISE = 0.000001
        local operateVol = 0

        local vol = tonumber(value)

        if not vol then
            return false, string.format("%.2f", 0)
        end


        local volList = {}

        for i = 1, pointNum do
            volList[i] = config.hardwareConfig.meterPoint.point[i].setVolume
        end

        if vol <= PRECISE then -- 输入体积为0

            operateVol = 0

        elseif volList[1] - vol >= -PRECISE then -- 输入体积小于或等于低定量点

            operateVol = minPoint

        elseif volList[1] - vol < -PRECISE  and
                volList[pointNum] - vol > PRECISE then -- 输入体积大于低定量点，小于高定量点

            pointNum = pointNum + 1
            volList[pointNum] = vol

            local temp = 0
            for i = 1, pointNum - 1 do
                for j = pointNum, i + 1, -1 do
                    if volList[j] < volList[j - 1] then
                        temp = volList[j - 1]
                        volList[j - 1] = volList[j]
                        volList[j] = temp
                    end
                end
            end

            local index = 1
            for i = 1, pointNum do
                if (-PRECISE <= volList[i] - vol) and
                        (volList[i] - vol <= PRECISE) then
                    break;
                end
                index = index + 1
            end

            local pPoint = volList[index - 1]
            local nPoint = volList[index + 1]
            local pD = vol - pPoint

            if pD < (nPoint - pPoint) / 2 then
                operateVol = pPoint
            else
                operateVol = nPoint
            end
        else -- 输入体积大于或等于高定量点

            local isMatch = false

            for i = 1, pointNum do
                local MP = volList[i]
                local fcount = vol / MP
                local count = math.floor(fcount + PRECISE)
                local residue = vol - count * MP

                if (-PRECISE <= residue) and (residue <= PRECISE) then
                    operateVol = vol
                    isMatch = true
                    break
                end
            end

            if isMatch == false then
                return false, string.format("%.2f", 0)
                --operateVol = maxPoint
            end

        end

        return true, string.format("%.2f", operateVol)
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
}
return setting.ui.profile.measureParam
