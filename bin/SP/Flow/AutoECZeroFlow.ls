--[[
 * @brief 电导率自动归零流程。
--]]
AutoECZeroFlow = Flow:new
{
    text = "",
    currentRange = 1,
}

function AutoECZeroFlow:new(o, target)
        o = o or {}
        setmetatable(o, self)
        self.__index = self

    o.currentRange = 1
    o.valveTarget = target
    o.detectTime = os.time()
    o.turboMode = config.measureParam.turboMode

        return o
end

function AutoECZeroFlow:GetRuntime()
    return 0
end

function AutoECZeroFlow:OnStart()
    local eventStr = "开始" .. self.text
    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)

    -- -- 初始化下位机
    -- dc:GetIDeviceStatus():Initialize()
    -- lc:GetIDeviceStatus():Initialize()
    -- --关LED
    -- dc:GetIOpticalAcquire():TurnOffLED()

    --更新状态
    local runStatus = Helper.Status.SetStatus(setting.runStatus.autoECZero)
    StatusManager.Instance():SetStatus(runStatus)
    ----更新动作
    --local runAction = Helper.Status.SetAction(self.action)
    --StatusManager.Instance():SetAction(runAction)

    self.isUserStop  = false
end

function AutoECZeroFlow:OnProcess()
    local initAbsorbance = {0,0}
    local absorbance = {0,0}
    local peak = {0,0,0,0,0,0,0}
    local peakIC = {0,0,0,0,0,0,0}
    local consistency = {0, 0}
    local addParam = {setting.calibrate[1],setting.calibrate[2]}
    local measureResult1 = Measurer:GetZeroMeasureResult()
    local measureResult2 = Measurer:GetZeroMeasureResult()
    local measureAD1 = MeasureAD:new()
    local measureAD2 = MeasureAD:new()
    local curveK = 1
    local curveB = 0
    local R2 = 1
    local meausureConsistency = {}
    local oneTimesCreateCurve = false
    local blankConsistency = {TC = 0, IC = 0, TOC = 0}
    local cStr = ""

    --测量零点
    log:debug("校准-动态超纯水测量")

    local updateWidgetManager = UpdateWidgetManager.Instance()

    updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "请接入在线超纯水(1/2)")

    if Measurer.flow then
        Measurer:Reset()
    end
    Measurer.flow = self
    Measurer.measureType = MeasureType.ZeroCheck
    Measurer.currentRange = self.currentRange
    for k,v in pairs(addParam[1]) do
        Measurer.addParam [k] = v
    end
    --根据量程修改参数
    Measurer.addParam.standardVolume = 0
    Measurer.addParam.blankVolume = setting.measure.range[self.currentRange].blankVolume + setting.measure.range[self.currentRange].sampleVolume
    Measurer.addParam.dilutionExtractVolume1 = setting.measure.range[self.currentRange].dilutionExtractVolume1
    Measurer.addParam.dilutionAddBlankVolume1 = setting.measure.range[self.currentRange].dilutionAddBlankVolume1
    Measurer.addParam.dilutionExtractVolume2 = setting.measure.range[self.currentRange].dilutionExtractVolume2
    Measurer.addParam.dilutionAddBlankVolume2 = setting.measure.range[self.currentRange].dilutionAddBlankVolume2
    Measurer.addParam.dilutionExtractVolume3 = setting.measure.range[self.currentRange].dilutionExtractVolume3
    Measurer.addParam.dilutionAddBlankVolume3 = setting.measure.range[self.currentRange].dilutionAddBlankVolume3
    Measurer.addParam.afterReagent1AddBlankVolume = setting.measure.range[self.currentRange].afterReagent1AddBlankVolume
    Measurer.addParam.diluteFactor = setting.measure.range[self.currentRange].diluteFactor
    Measurer.addParam.rinseSampleVolume = 0
    Measurer.addParam.rinseBlankVolume = setting.measure.range[self.currentRange].rinseStandardVolume + setting.measure.range[self.currentRange].rinseSampleVolume
    Measurer.addParam.rinseStandardVolume = 0

    --TC/IC校准结果缓存表
    local signalTCTable = {}
    local signalICTable = {}
    local maxECTC = 0
    local maxECIC = 0
    local consistencyTC, consistencyIC
    --测量次数
    local measureTimes = 7
    --舍弃次数
    local throwNum = 2
    config.measureParam.reagent1Vol = 0
    config.measureParam.reagent2Vol = 0

    config.modifyRecord.measureParam(true)
    ConfigLists.SaveMeasureParamConfig()
    setting.measureResult.continousModeParam.currentMeasureCnt = 0
    -- local err,result = pcall(function() return Measurer:Measure() end)
    -- if not err then      -- 出现异常
    --     if type(result) == "table" then
    --         if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
    --             self.isUserStop = true
    --             error(result)
    --         elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
    --             self.isUserStop = true
    --             error(result)
    --         elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
    --             self.isUserStop = true
    --             error(result)
    --         elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
    --             self.isUserStop = true
    --             error(result)
    --         elseif getmetatable(result)== UserStopException then 				--用户停止测量流程
    --             self.isUserStop = true
    --             error(result)
    --         else
    --             error(result)
    --         end
    --     else
    --         error(result)
    --     end
    -- else    -- 正常
    --     local temptable = {}
    --     temptable, maxECTC = self:GetTableAndMaxECSignal(ModelType.TC)
    --     temptable, maxECIC = self:GetTableAndMaxECSignal(ModelType.IC)
    --     log:debug("超纯水TC电导率峰值= " .. string.format("%.4f", maxECTC) .. ", IC电导率峰值= " .. string.format("%.4f", maxECIC))
    -- end

    -- if maxECTC > 0.057 or maxECIC >0.057 then
    --     updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "电导率自动归零失败[TC = " ..
    --             string.format("%.4f", maxECTC) .. ", IC = " .. string.format("%.4f", maxECIC))
    --     return
    -- end

    log:debug("校准-静态超纯水测量")

    setting.measureResult.continousModeParam.currentMeasureCnt = 0
    Measurer.measureType = MeasureType.RangeCheck

    for i = 1, measureTimes do
        local TCTable = {}
        local ICTable = {}
        local err,result = pcall(function() return Measurer:Measure() end)
        if not err then      -- 出现异常
            if type(result) == "table" then
                if getmetatable(result) == PumpStoppedException then 			--泵操作被停止异常。
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result)== AcquirerADStoppedException then 	    --光学采集被停止异常
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result)== MeterStoppedException then			--定量被停止异常。
                    self.isUserStop = true
                    error(result)
                elseif getmetatable(result) == ThermostatStoppedException then  	--恒温被停止异常。
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
        else    -- 正常
            print("Lua##1[" .. "] " .. status.measure.blankECTableTC[1])
            TCTable, maxECTC = self:GetTableAndMaxECSignal(ModelType.TC)
            ICTable, maxECIC = self:GetTableAndMaxECSignal(ModelType.IC)
            print("Lua##2[" .. "] " .. status.measure.blankECTableTC[1])
            print("Lua##[" .. i .. "] " .. TCTable[1])
            if i > throwNum then
                print("Lua##3[" .. "] " .. status.measure.blankECTableTC[1])
                signalTCTable = self:GetSumTable(signalTCTable, TCTable)
                signalICTable = self:GetSumTable(signalICTable, ICTable)
                print("Lua##4[" .. "] " .. status.measure.blankECTableTC[1])
                log:debug("Lua表测试[" .. i .. "] " .. signalTCTable[1])
                print("Lua[" .. i .. "] " .. signalTCTable[1])
            end
            TCTable = nil
            ICTable = nil
            log:debug("第[" .. i .. "]次超纯水TC电导率峰值= " .. string.format("%.4f", maxECTC) .. ", IC电导率峰值= " .. string.format("%.4f", maxECIC))
        end
    end

    --求平均 默认取最后5组进行平均
    -- status.measure.blankECTableTC = 
    self:GetAvgTable(signalTCTable, 5)
    -- status.measure.blankECTableIC = 
    self:GetAvgTable(signalICTable, 5)
    -- ConfigLists.SaveMeasureStatus()

    signalTCTable = nil
    signalICTable = nil

    updateWidgetManager:Update(UpdateEvent.MulCalibrationTip, "电导率自动归零校准完成(2/2)")
end

--[[
 * @brief 缓存表求和
--]]
function AutoECZeroFlow:GetSumTable(targetTable, dataTable)    
    for k,v in pairs(dataTable) do
        if targetTable[k] ~= nil then
            targetTable[k] = targetTable[k] + v
        else
            table.insert(targetTable, v)
        end
    end

    return targetTable
end

--[[
 * @brief 缓存表求平均
 * @param[div] 分母
--]]
function AutoECZeroFlow:GetAvgTable(sumTable, div)
    local table = {}

    for k,v in pairs(sumTable) do
        table[k] = v / div
        log:debug("校准值[" .. k .. "] = " .. table[k])
    end

    return table
end

--[[
 * @brief 低浓度峰高系数补偿
 * @param[in] mtype 测量类型 TC or IC
--]]
function AutoECZeroFlow:GetTableAndMaxECSignal(mtype)
    local consistency = 0
    local constant = 1
    local temp = 0
    local peak = 0
    local str = ""
    local strPeak = ""
    local peakTable ={}
    local peakTableCal ={}
    local peakTableTemp = {}
    local reviser = 1
    local EC = 0
    local T25EC = 0
    local startIndex = Measurer.measureResult.startIndex
    local endIndex = Measurer.measureResult.endIndex
    local max = 0
    local gainValue = 1
    -- if setting.measureResult.isHighRangeTC and mtype == ModelType.TC then
    --     gainValue = 9.29
    --     log:debug("TC高量程模式")
    -- end
    -- if setting.measureResult.isHighRangeIC and mtype == ModelType.IC then
    --     gainValue = 9.29
    --     log:debug("IC高量程模式")
    -- end

    -- if mtype == ModelType.TC then
    --     constant = config.measureParam.TCConstant
    --     for i = startIndex,endIndex do
    --         peak = rc:GetScanData(i) * gainValue
    --         temp = rc:GetMeaTemp(i)
    --         reviser = -3.16345*10^(-8) * temp^3 + 1.25933*10^(-5) * temp^2 + 5.26393*10^(-4)* temp + 0.03193
    --         EC = peak * constant * 2
    --         T25EC = 1 + reviser * (temp - 25)
    --         table.insert(peakTable, (EC/T25EC))
    --     end

    --     for k,v in pairs(peakTable) do
    --         local average = v
    --         average = peakTable[k]
    --         table.insert(peakTableTemp, average)
    --         if k>5 then
    --             average = (average + peakTableTemp[k-1] + peakTableTemp[k-2] + peakTableTemp[k-3] + peakTableTemp[k-4] )/5
    --         end

    --         if max < average and k > 30 and k < 80 then
    --             max = average
    --         end
    --         table.insert(peakTableCal, average)
    --         log:debug("TC补偿后电导率[" .. k .. "] = " .. tonumber(v) .. ", 平均处理后电导率 = " .. tonumber(peakTableCal[k]))
    --     end
    -- else
    --     constant = config.measureParam.ICConstant
    --     for i = startIndex,endIndex do
    --         peak = rc:GetScanDataRef(i) * gainValue
    --         temp = rc:GetRefTemp(i)
    --         reviser = -3.16345*10^(-8) * temp^3 + 1.25933*10^(-5) * temp^2 + 5.26393*10^(-4)* temp + 0.03193
    --         EC = peak * constant * 2
    --         T25EC = 1 + reviser * (temp - 25)
    --         table.insert(peakTable, (EC/T25EC))
    --     end

    --     for k,v in pairs(peakTable) do
    --         local average = v
    --         average = peakTable[k]
    --         table.insert(peakTableTemp, average)
    --         if k>5 then
    --             average = (average + peakTableTemp[k-1] + peakTableTemp[k-2] + peakTableTemp[k-3] + peakTableTemp[k-4] )/5
    --         end

    --         if max < average and k > 30 and k < 80 then
    --             max = average
    --         end
    --         table.insert(peakTableCal, average)
    --         log:debug("IC补偿后电导率[" .. k .. "] = " .. tonumber(v) .. ", 平均处理后电导率 = " .. tonumber(peakTableCal[k]))
    --     end
    -- end

    if mtype == ModelType.TC then
        peakTable = status.measure.blankECTableTC
        print("Lua##TC[" .. "] " .. status.measure.blankECTableTC[1])
        max = 0.11
    else
        peakTable = status.measure.blankECTableIC
        print("Lua##IC[" .. "] " .. peakTable[1])
        max = 0.22
    end

    log:debug("电导率峰值 " .. max)

    return peakTable, max
end

function AutoECZeroFlow:OnStop()

    if nil ~= setting.common.skipFlow and true == setting.common.skipFlow then

    else
        -- -- 初始化下位机
        -- dc:GetIDeviceStatus():Initialize()
        -- rc:ClearBuf()--清buf,防止数组刷新
        -- lc:GetIDeviceStatus():Initialize()
        -- --停止水样泵
        -- op:StopSamplePump()
        -- --设置风扇常开
        -- op:SetDCNormalOpen(setting.liquidType.map.fan)
        -- --关紫外灯
        -- dc:GetIOpticalAcquire():TurnOffLED()
        -- --设置去离子水泵和阀为打开
        -- op:SetLCStopStatus()
    end

    --保存试剂余量表
    ReagentRemainManager.SaveRemainStatus()

    local eventStr = "结束" .. self.text
    --保存审计日志
    SaveToAuditTrailSqlite(nil, nil, eventStr, nil, nil, nil)
end
