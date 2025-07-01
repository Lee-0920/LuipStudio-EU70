
Measurer =
{
    flow = nil,
    measureType = MeasureType.Sample,
    currentRange = 1,
    addParam = {},
    temperatureBeforeAddReagent2 = 0,

    measureResult =
    {
        initReferenceAD =0,      		-- 初始参考AD值
        initMeasureAD =0,         		-- 初始测量AD值
        finalReferenceAD =0,    	 	-- 结果参考AD值
        finalMeasureAD =0,        		-- 结果参考AD值
        initCellTempTC =0,              -- 初始TC电导池温度
        initCellTempIC =0,              -- 初始IC电导池温度
        finalCellTempTC =0,             -- 反应TC电导池温度
        finalCellTempIC =0,             -- 反应TC电导池温度
        initEnvironmentTemp =0,   	    -- 初始环境温度
        finalEnvironmentTemp =0,  	    -- 反应环境温度
        TCConductivityCellTemp =0,      -- TC电导池温度
        ICConductivityCellTemp =0,      -- IC电导池温度
        startIndex = 0,                 -- 读反应值数组开始索引
        endIndex = 0,                   -- 读反应值数组结束索引
        peakTC = 0,                     -- TC峰值
        peakIC = 0 ,                    -- IC峰值
        peakTemperatureTC = 0,          -- TC峰值温度
        peakTemperatureIC = 0 ,         -- IC峰值温度
        accurateArea1 = 0,              --精准测量面积1
        accurateArea2 = 0,              --精准测量面积2
        accurateArea3 = 0,              --精准测量面积3
        accurateArea4 = 0,              --精准测量面积4
        measureDate = os.time(),         --测量日期
        lastMeasureDate = os.time(),     --上一次测量日期
        lastAccurateMeasureTime = 0,
        startTemperature = 0,
        endTemperature = 0,
    },

    procedure = {},

    steps =
    {
        --1 管路初始化
        {
            action = setting.runAction.measure.init,
            execute = function()
                local startTime = os.time()

                -- if config.measureParam.reagent1Vol > 0 then
                --     -- --停止试剂1泵
                --     -- op:StopReagentMix(setting.liquidType.reagent1)
                --     -- --酸剂余量检测
                --     -- op:ReagentManager(setting.liquidType.reagent1)
                --     -- --混合酸剂 1 uL/min
                --     -- op:StartReagentMix(setting.liquidType.reagent1, setting.liquid.syringeMaxVolume, config.measureParam.reagent1Vol)
                -- end

                -- if config.measureParam.reagent2Vol > 0 then
                --     --停止试剂2泵
                --     op:StopReagentMix(setting.liquidType.reagent2)
                --     --氧化剂余量检测
                --     op:ReagentManager(setting.liquidType.reagent2) --]]--
                --     --混合氧化剂 1 uL/min
                --     op:StartReagentMix(setting.liquidType.reagent2, setting.liquid.syringeMaxVolume, config.measureParam.reagent2Vol)
                -- end--]]--

                -- if setting.measureResult.continousModeParam.currentMeasureCnt == 0 then
                --     --去离子回路限流阀
                --     log:debug("开去离子水泵")
                --     log:debug("开去离子水泵限流阀")
                --     op:SetLCNormalOpen(setting.liquidType.deionizedOn.valve)
                -- end


                -- --设置去离子水限流阀常开
                -- op:SetLCNormalOpen(setting.liquidType.deionizedOn.valve)

                ----测试
                ----Turbo模式以及电导率自动归零流程无需等待
                --if config.measureParam.turboMode == false or Measurer.measureType == MeasureType.ZeroCheck then
                --    log:debug("ZeroCheck")
                --    if not Measurer.flow:Wait(10) then
                --        error(UserStopException:new())
                --    end
                --end

                --检查增益倍数
                -- op:ConfirmLED()

                -- --ICR模式
                -- if config.measureParam.ICRMode == true then
                --     --设置两路阀+ICR直流泵常开+风扇常开
                --     op:SetDCNormalOpen(setting.liquidType.icr.valve | setting.liquidType.map.fan)
                -- else
                --     --设置两路阀+ICR直流泵常闭,风扇常开
                --     op:SetDCNormalOpen(setting.liquidType.map.fan)
                -- end


                --连续测量时间管理
                Measurer.measureResult.measureDate = os.time()
                --电导率自动归零测量过程关闭水样泵
                if  Measurer.measureType == MeasureType.RangeCheck then

                else
                    if setting.measureResult.continousModeParam.currentMeasureCnt == 0 then
                        print("first" )
                        -- op:StartSamplePump(config.measureParam.sampleQuickSpeed)
                        if not Measurer.flow:Wait(config.measureParam.quickRefreshTime) then
                            error(UserStopException:new())
                        end

                        -- --电导率自动归零不需要设置低流速
                        -- if Measurer.measureType ~= MeasureType.ZeroCheck then
                        --     op:StopSamplePump()
                        --     op:StartSamplePump(config.measureParam.sampleSlowSpeed)
                        -- end

                    else
                        -- --电导率自动归零不需要设置低流速
                        -- if Measurer.measureType ~= MeasureType.ZeroCheck then
                        --     op:StartSamplePump(config.measureParam.sampleSlowSpeed)
                        -- end
                         print("cnt = " .. setting.measureResult.continousModeParam.currentMeasureCnt)
                    end
                end


                -- local temp = rc:GetCurrentTemperature()
                -- Measurer.measureResult.initCellTempTC = temp:GetThermostatTemp()
                -- Measurer.measureResult.initCellTempIC = temp:GetEnvironmentTemp()
                -- Measurer.measureResult.initEnvironmentTemp = dc:GetEnvironmentTemperature()

                log:debug("初始化时间 = " .. os.time() - startTime);
                end
        },
        --2 mixReagent 混合
        {
            action = setting.runAction.measure.mixReagent,
            execute = function()
                local startTime = os.time()

                if setting.measureResult.continousModeParam.currentMeasureCnt == 0 and Measurer.measureType ~= MeasureType.ZeroCheck then
                     print("first fresh")
                    if not Measurer.flow:Wait(config.measureParam.normalRefreshTime) then
                        error(UserStopException:new())
                    end
                end

                -- --Turbo模式下无需等待及阀切换 -- 电导率自动归零不需要关阀
                -- if config.measureParam.turboMode == false and Measurer.measureType ~= MeasureType.ZeroCheck then
                --     --关去离子水限流阀
                --     op:SetLCNormalOpen(setting.liquidType.deionizedOff.valve)
                -- end
                log:debug("混合时间 = " .. os.time() - startTime);
            end
        },
        --3 react 反应
        {
            action = setting.runAction.measure.react,
            execute = function()
                local startTime = os.time()

                -- --Turbo模式下无需等待及阀切换 --电导率自动归零不需要切换阀
                -- if config.measureParam.turboMode == false and Measurer.measureType ~= MeasureType.ZeroCheck then
                --     if not Measurer.flow:Wait(config.measureParam.reactTime) then
                --         error(UserStopException:new())
                --     end

                --     --检查增益倍数
                --     op:ConfirmLED()

                --     local temp = rc:GetCurrentTemperature()
                --     temp = op:GetCalibrateTemperatureWithFiltration(10)

                --     Measurer.measureResult.TCConductivityCellTemp = temp:GetThermostatTemp()
                --     Measurer.measureResult.ICConductivityCellTemp = temp:GetEnvironmentTemp()

                --     rc:ClearBuf()

                --     Measurer.measureResult.startIndex = rc:GetScanLen()   --标记开始

                --     log:debug("标记开始： ".. Measurer.measureResult.startIndex);
                --     --App.Sleep(3000)

                --     --开IC阀
                --     op:SetRCValveOn()
                --     App.Sleep(100)

                --     --开去离子水限流阀
                --     op:SetLCNormalOpen(setting.liquidType.deionizedOn.valve)

                --     --App.Sleep(3000)
                --     --关IC阀
                --     op:SetRCValveOff()

                --     --if not Measurer.flow:Wait(1) then
                --     --    error(UserStopException:new())
                --     --end

                -- elseif Measurer.measureType == MeasureType.ZeroCheck then
                --     --local restTime = 600 - os.time() - Measurer.measureResult.measureDate
                --     --if not Measurer.flow:Wait(restTime) then
                --     --    error(UserStopException:new())
                --     --end

                --     if not Measurer.flow:Wait(config.measureParam.reactTime) then
                --         error(UserStopException:new())
                --     end

                --     --检查增益倍数
                --     op:ConfirmLED()

                --     rc:ClearBuf()

                --     App.Sleep(1000)

                --     Measurer.measureResult.startIndex = rc:GetScanLen()   --标记开始
                --     log:debug("标记开始： ".. Measurer.measureResult.startIndex);

                --     local temp = rc:GetCurrentTemperature()
                --     Measurer.measureResult.TCConductivityCellTemp = temp:GetThermostatTemp()
                --     Measurer.measureResult.ICConductivityCellTemp = temp:GetEnvironmentTemp()

                --     rc:ClearBuf()
                -- else
                --     --检查增益倍数
                --     op:ConfirmLED()

                --     local temp = rc:GetCurrentTemperature()
                --     Measurer.measureResult.TCConductivityCellTemp = temp:GetThermostatTemp()
                --     Measurer.measureResult.ICConductivityCellTemp = temp:GetEnvironmentTemp()

                --     rc:ClearBuf()
                --     log:debug("标记开始： ".. Measurer.measureResult.startIndex);
                -- end

                --Measurer.measureResult.startIndex = rc:GetScanLen()   --标记开始
                --
                --log:debug("标记开始： ".. Measurer.measureResult.startIndex);

                log:debug("反应时间 = " .. os.time() - startTime);
            end
        },
        --4 readMeasure 读反应值
        {
            action = setting.runAction.measure.readMeasure,
            execute = function()
                local startTime = os.time()

                -- --Turbo模式下无需等待及阀切换
                -- if Measurer.measureType == MeasureType.RangeCheck
                --         or Measurer.measureType == MeasureType.ZeroCheck then --电导率自动归零测量时长(尽可能比测水样时间长，否则出现数组越界)
                --     if not Measurer.flow:Wait(40) then
                --         error(UserStopException:new())
                --     end
                -- elseif config.measureParam.turboMode == false then --电导率自动归零测量时长(尽可能比测水样时间长，否则出现数组越界)
                --     if not Measurer.flow:Wait(config.measureParam.windowTime) then
                --         error(UserStopException:new())
                --     end
                -- else
                --     if not Measurer.flow:Wait(4) then
                --         error(UserStopException:new())
                --     end
                -- end

                -- local temp = rc:GetCurrentTemperature()
                -- Measurer.measureResult.finalCellTempTC = temp:GetThermostatTemp()
                -- Measurer.measureResult.finalCellTempIC = temp:GetEnvironmentTemp()
                -- Measurer.measureResult.finalEnvironmentTemp = dc:GetEnvironmentTemperature()

                -- Measurer.measureResult.endIndex = rc:GetScanLen()   --标记开始

                -- log:debug("标记结束： ".. Measurer.measureResult.endIndex);
                --停止峰形图数据更新

                ----非turbo模式下不关闭水样泵
                --if config.measureParam.turboMode == false then
                --    --停止水样泵
                --    op:StopSamplePump()
                --end

                log:debug("读反应值时间 = " .. os.time() - startTime);
            end
        },
        --5 返回结果
        {
            action = nil,
            execute = function()
                status.measure.lastMeasureEndTime = os.time()

                -- if config.measureParam.turboMode == false then
                --     --开发者模式下打印原始数据
                --     if config.system.debugMode then
                --         local peakTC = op:Calculatepeak( Measurer.measureResult.startIndex,
                --                 Measurer.measureResult.endIndex, nil, false, ModelType.TC)
                --         local peakIC = op:Calculatepeak( Measurer.measureResult.startIndex,
                --                 Measurer.measureResult.endIndex, nil, false, ModelType.IC)
                --     end
                --     Measurer.measureResult.peakTemperatureTC = op:GetPeakTemperature( Measurer.measureResult.startIndex,
                --             Measurer.measureResult.endIndex, ModelType.TC)
                --     Measurer.measureResult.peakTemperatureIC = op:GetPeakTemperature( Measurer.measureResult.startIndex,
                --             Measurer.measureResult.endIndex, ModelType.IC)
                --     Measurer.measureResult.peakTC = op:SearchPeak(Measurer.measureResult, ModelType.TC)
                --     Measurer.measureResult.peakIC = op:SearchPeak(Measurer.measureResult, ModelType.IC)
                --     log:debug("TC原始峰高 = " .. Measurer.measureResult.peakTC .. ", IC原始峰高 = " ..  Measurer.measureResult.peakIC)
                --     log:debug("TC峰值温度 = " .. Measurer.measureResult.peakTemperatureTC .. ", IC峰值温度 = " ..  Measurer.measureResult.peakTemperatureIC)
                -- else
                --     --TC计算平均峰值和平均温度
                --     Measurer.measureResult.peakTC,
                --     Measurer.measureResult.TCConductivityCellTemp = op:TurboCalculatepeak(
                --                                     Measurer.measureResult.startIndex,
                --                                     Measurer.measureResult.endIndex,
                --                                     ModelType.TC)
                --     --IC计算平均峰值和平均温度
                --     Measurer.measureResult.peakIC,
                --     Measurer.measureResult.ICConductivityCellTemp = op:TurboCalculatepeak(
                --                                     Measurer.measureResult.startIndex,
                --                                     Measurer.measureResult.endIndex,
                --                                     ModelType.IC)
                -- end

                -- if Measurer.flow.ResultHandle then
                --     Measurer.flow:ResultHandle(Measurer.measureResult)
                -- end
                App.Sleep(2000)
                setting.measureResult.continousModeParam.currentMeasureCnt = setting.measureResult.continousModeParam.currentMeasureCnt + 1
                log:debug("流程执行次数 = " .. setting.measureResult.continousModeParam.currentMeasureCnt);
            end
        },
    },
}

function Measurer:SkipFlow()
    status.measure.lastMeasureEndTime = os.time()
    if Measurer.flow.ResultHandle then
        Measurer.flow:ResultHandle(Measurer.measureResult)
    end
end

function Measurer:Measure()
    if nil ~= setting.common.skipFlow and true == setting.common.skipFlow then
        Measurer:SkipFlow()
        print("--------------Skip All Procedure--------------")
        return Measurer.measureResult
    end
    if #self.procedure ~= 0  then
        print("--------------Execute configuration procedure--------------")
        for i, index in pairs(self.procedure) do
            print("index = ".. index)
            local step = self.steps[index]
            if step.action then
                local runAction = Helper.Status.SetAction(step.action)
                StatusManager.Instance():SetAction(runAction)
                log:info(step.action.text)
            end

            step.execute()
        end
    else
        print("--------------Execute default procedure--------------")
        for i, step in pairs(self.steps) do
            print("i = ".. i)
            if step.action then
                local runAction = Helper.Status.SetAction(step.action)
                StatusManager.Instance():SetAction(runAction)
                log:info(step.action.text)
            end

            step.execute()
        end
    end

    return Measurer.measureResult
end

function Measurer:QuickMeasure()
    if #self.procedure ~= 0  then
        print("--------------Execute configuration procedure--------------")
        for i, index in pairs(self.procedure) do
            print("index = ".. index)
            local step = self.steps[index]
            if step.action then
                local runAction = Helper.Status.SetAction(step.action)
                StatusManager.Instance():SetAction(runAction)
                log:info(step.action.text)
            end

            step.execute()
        end
    else
        print("--------------Execute default procedure--------------")
        for i, step in pairs(self.steps) do
            print("i = ".. i)
            if step.action then
                local runAction = Helper.Status.SetAction(step.action)
                StatusManager.Instance():SetAction(runAction)
                log:info(step.action.text)
            end

            step.execute()
        end
    end

    return Measurer.measureResult
end

--[[
 * @brief 校准测量终点判断
 * @details
--]]
function Measurer:CalibrateMeasureEndJudge(time)
    local startTime = os.time()
    local timeout = 0
    if time ~= nil and time > 0 then
        timeout = time
    end

    while true do
        local currentTime = os.time()
        if currentTime - startTime > timeout then
            break
        elseif op:IsReachSteady(setting.measureResult.baseLineNum*4, setting.measureResult.validCnt) == false then
            if not Measurer.flow:Wait(2) then
                break
            end
        else
            break
        end
    end
end

function Measurer:Reset()
    Measurer.flow = nil
    Measurer.measureType = MeasureType.Sample
    Measurer.currentRange = 1
    Measurer.procedure = {}

    print("--------------Reset AddParam & MeasureResult--------------")

    for k, v in pairs(Measurer.addParam) do
        Measurer.addParam[k] = nil
    end

    for k, v in pairs(Measurer.measureResult) do
        v = 0
    end
end

function Measurer:GetMeasureResult()
    local dstTable = {}
    for k, v in pairs(Measurer.measureResult) do
        dstTable[k] = v
    end
    return dstTable
end

function Measurer:GetZeroMeasureResult()
    local dstTable = {}
    for k, v in pairs(Measurer.measureResult) do
        dstTable[k] = 0
    end
    return dstTable
end


function Measurer:TimeCheck()
    local currentTime = os.time()
    local lastTime
    local temp
    local MeasurerIntervalMaxTime = 36*60*60        --距离上次测量允许度最大间隔时间，超过则进行额外清洗

    temp = status.measure.lastMeasureEndTime

    if temp  == 0 then
        log:debug("出厂首次流程测量")
        return true
    end

    lastTime = temp + MeasurerIntervalMaxTime
    if lastTime - currentTime < 0 then
        log:debug("距离上次测量已超36小时")
        return true
    else
        return false
    end
end

function Measurer:GetRinseVol()
    local rinseVol = 0

    if Measurer:TimeCheck() then
        rinseVol = setting.unitVolume * 2
    else
        if Measurer.addParam.rinseSampleVolume > 0 then
            rinseVol = Measurer.addParam.rinseSampleVolume
        elseif Measurer.addParam.rinseStandardVolume > 0 then
            rinseVol = Measurer.addParam.rinseStandardVolume
        elseif Measurer.addParam.rinseZeroCheckVolume > 0 then
            rinseVol = Measurer.addParam.rinseZeroCheckVolume
        elseif Measurer.addParam.rinseRangeCheckVolume > 0 then
            rinseVol = Measurer.addParam.rinseRangeCheckVolume
        elseif Measurer.addParam.rinseBlankVolume > 0 then
            rinseVol = Measurer.addParam.rinseBlankVolume
        end
    end


    return rinseVol
end


--[[
 * @brief 生成测量数据
 * @details 该函数应当用于连续模式切换导致其他模式此类情形
--]]
function Measurer:ContinousMeasureSafetyStop()
    if setting.measureResult.continousModeParam.currentMeasureCnt ~= 0 then

        log:debug("safe currentMeasureCnt: " .. setting.measureResult.continousModeParam.currentMeasureCnt)
        log:debug("safe lastStartIndex: " .. setting.measureResult.continousModeParam.lastStartIndex)

        Measurer.measureResult.endIndex = 0
        Measurer.measureResult.startIndex = setting.measureResult.continousModeParam.lastStartIndex
        local reactTime = config.measureParam.reacTime

        local restTime = os.time() - Measurer.measureResult.lastAccurateMeasureTime
        log:debug("restTime: " .. (reactTime - restTime) .. ", done: " .. restTime)

        if restTime < reactTime or setting.measureResult.immediatelyResultHandle == false then
            if not Measurer.flow:Wait((reactTime - restTime)/2) then
                error(UserStopException:new())
            end
            Measurer:CalibrateMeasureEndJudge((reactTime - restTime)/2)
            --if not Measurer.flow:Wait(40) then
            --    error(UserStopException:new())
            --end
        end

        Measurer:Handle(Measurer.measureResult)

        --连续测量中时间排卤素液抢占
        Measurer.measureResult.measureDate = Measurer.measureResult.lastMeasureDate
        if Measurer.flow.ResultHandle then
            Measurer.flow:ResultHandle(Measurer.measureResult)
            log:info("测量完成")
            log:info("测量流程总时间 = ".. os.time() - Measurer.measureResult.measureDate)
        end
    end
end


--[[
 * @brief 检测连续模式测量结果
 * @details 用于检测反应时间是否超时
--]]
function Measurer:ContinousModeCheckResult()
    if setting.measureResult.continousModeParam.isfinishContinousMeasure == true then

        Measurer.measureResult.endIndex = 0
        Measurer.measureResult.startIndex = setting.measureResult.continousModeParam.lastStartIndex
        local restTime = os.time() -  Measurer.measureResult.lastAccurateMeasureTime

        if op:IsReachSteady(setting.measureResult.baseLineNum*4, setting.measureResult.validCnt) == true or
                setting.measureResult.immediatelyResultHandle == true or restTime > config.measureParam.reacTime   then

            if restTime < config.measureParam.reacTime/2 then
                return
            end
            --if not Measurer.flow:Wait(20) then
            --    error(UserStopException:new())
            --end
            Measurer:Handle(Measurer.measureResult)

            if Measurer.flow.ResultHandle then
                Measurer.flow:ResultHandle(Measurer.measureResult)
                log:info("测量完成")
                log:info("测量流程总时间 = ".. os.time() - Measurer.measureResult.measureDate)
            end
            setting.measureResult.continousModeParam.isfinishContinousMeasure = false
        end
    end
end

--[[
 * @brief 结果处理
 * @details
--]]
function Measurer:Handle(measureResult)
    measureResult.finalCellTempTC = dc:GetReportThermostatTemp(setting.temperature.temperatureRefrigerator)
    measureResult.finalCellTempIC = dc:GetReportThermostatTemp(setting.temperature.temperatureNDIR)
    measureResult.finalThermostatTemp = dc:GetDigestTemperature()
    measureResult.finalEnvironmentTemp = dc:GetEnvironmentTemperature()
    measureResult.finalEnvironmentTempDown = dc:GetReportThermostatTemp(setting.temperature.temperatureBoxLow)

    local num = rc:GetScanLen()
    if num ~= nil then
        measureResult.endIndex = num
        log:debug("标记结束： " .. num);
    end

    if config.measureParam.accurateMeasure == false and setting.measureResult.isFinishAccurateMeasure == false then
        measureResult.peakTC = op:Calculatepeak(measureResult.startIndex, measureResult.endIndex, nil, false, ModelType.TC)
        log:debug("普通测量： " .. measureResult.peakTC)
    elseif config.measureParam.accurateMeasure == true or setting.measureResult.isFinishAccurateMeasure == true then
        --清精准定量动作完成标志位
        setting.measureResult.isFinishAccurateMeasure = false
        if setting.measureResult.immediatelyResultHandle == true then
            --完成精准测量无需赋值
            setting.measureResult.immediatelyResultHandle = false
        else
            measureResult.accurateArea4 = op:Calculatepeak(measureResult.startIndex, measureResult.endIndex, nil, false, ModelType.TC)
            if measureResult.accurateArea4 == -1 then
                error(UserStopException:new())
            end
            if measureResult.accurateArea1 == 0 and
                    measureResult.accurateArea2 == 0 and
                    measureResult.accurateArea3 == 0 then
                measureResult.peakTC = measureResult.accurateArea4
            else
                local deviation12 = math.abs(measureResult.accurateArea2 - measureResult.accurateArea1)/
                        ((measureResult.accurateArea2 + measureResult.accurateArea1)/2)
                local deviation23 = math.abs(measureResult.accurateArea3 - measureResult.accurateArea2)/
                        ((measureResult.accurateArea3 + measureResult.accurateArea2)/2)
                local deviation34 = math.abs(measureResult.accurateArea4 - measureResult.accurateArea3)/
                        ((measureResult.accurateArea4 + measureResult.accurateArea3)/2)
                local deviation14 = math.abs(measureResult.accurateArea4 - measureResult.accurateArea1)/
                        ((measureResult.accurateArea4 + measureResult.accurateArea1)/2)
                local deviation13 = math.abs(measureResult.accurateArea3 - measureResult.accurateArea1)/
                        ((measureResult.accurateArea3 + measureResult.accurateArea1)/2)
                local deviation24 = math.abs(measureResult.accurateArea4 - measureResult.accurateArea2)/
                        ((measureResult.accurateArea4 + measureResult.accurateArea2)/2)
                local minDeviation = math.min(deviation12, deviation23, deviation34, deviation14, deviation13, deviation24)
                log:debug("deviation12 " .. deviation12 .. ", deviation23 " .. deviation23 .. ", deviation34 " .. deviation34)
                log:debug("deviation14 " .. deviation14 .. ", deviation13 " .. deviation13 .. ", deviation24 " .. deviation24)
                log:debug("minDeviation " .. minDeviation)
                if measureResult.accurateArea1 ~= 0 and
                        measureResult.accurateArea2 ~= 0 and
                        measureResult.accurateArea3 ~= 0 and
                        measureResult.accurateArea4 ~= 0 then
                    local maxValue = math.max(measureResult.accurateArea1,
                            measureResult.accurateArea2,
                            measureResult.accurateArea3,
                            measureResult.accurateArea4)
                    local minValue = math.min(measureResult.accurateArea1,
                            measureResult.accurateArea2,
                            measureResult.accurateArea3,
                            measureResult.accurateArea4)
                    measureResult.peakTC = (measureResult.accurateArea1+measureResult.accurateArea2
                            +measureResult.accurateArea3+measureResult.accurateArea4
                            - maxValue - minValue)/2
                else
                    if minDeviation == deviation12 then
                        measureResult.peakTC = (measureResult.accurateArea2 + measureResult.accurateArea1)/2
                    elseif minDeviation == deviation23 then
                        measureResult.peakTC = (measureResult.accurateArea3 + measureResult.accurateArea2)/2
                    elseif minDeviation == deviation34 then
                        measureResult.peakTC = (measureResult.accurateArea4 + measureResult.accurateArea3)/2
                    elseif  minDeviation == deviation14 then
                        measureResult.peakTC = (measureResult.accurateArea4 + measureResult.accurateArea1)/2
                    elseif  minDeviation == deviation13 then
                        measureResult.peakTC = (measureResult.accurateArea3 + measureResult.accurateArea1)/2
                    elseif  minDeviation == deviation24 then
                        measureResult.peakTC = (measureResult.accurateArea4 + measureResult.accurateArea2)/2
                    end
                end
            end
        end
        log:debug("精准测量： " .. string.format("%.3f", measureResult.accurateArea1) .. ", " .. string.format("%.3f", measureResult.accurateArea2) .. ", " ..
                string.format("%.3f", measureResult.accurateArea3) .. ", " .. string.format("%.3f", measureResult.accurateArea4))
        log:debug("精准模式平均面积： " .. measureResult.peakTC)
        log:debug("精准测量： " .. measureResult.peakTC)
    end
    measureResult.accurateArea1 = 0
    measureResult.accurateArea2 = 0
    measureResult.accurateArea3 = 0
    measureResult.accurateArea4 = 0

    --开始更新基线状态
    status.measure.isCheckBaseLine = true
    ConfigLists.SaveMeasureStatus()
end


