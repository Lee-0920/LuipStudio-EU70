--[[
 * @brief 硬件测试
--]]

HardwareTest =
{
    -- row 1
    {
        name = "MeterPump",
        open = function()
            local pump = pumps[setting.liquidType.waste.pump + 1]
            pump:Start(RollDirection.Drain, 1000000, 0)
        end,
        close = function()
            local pump = pumps[setting.liquidType.waste.pump + 1]
            pump:Stop()
        end,
        status = false,
    },
    -- row 2
    {
        name = "SampleValve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | setting.liquidType.sample.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~setting.liquidType.sample.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 3
    {
        name = "Reagent1Valve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | setting.liquidType.reagent1.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~setting.liquidType.reagent1.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 4
    {
        name = "Reagent2Valve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | setting.liquidType.reagent2.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~setting.liquidType.reagent2.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 5
    {
        name = "ThreeWatValve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | setting.liquidType.map.valve9)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~setting.liquidType.map.valve9)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 6
    {
        name = "StandardValve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | setting.liquidType.standard.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~setting.liquidType.standard.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 7
    {
        name = "BlankValve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | setting.liquidType.blank.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~setting.liquidType.blank.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 8
    {
        name = "ZeroCheckValve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | setting.liquidType.zeroCheck.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~setting.liquidType.zeroCheck.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 9
    {
        name = "RangeCheckValve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | setting.liquidType.rangeCheck.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~setting.liquidType.rangeCheck.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 10
    {
        name = "WasteValve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | setting.liquidType.waste.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~setting.liquidType.waste.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 11
    {
        name = "WasteWaterValve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | setting.liquidType.wasteWater.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~setting.liquidType.wasteWater.valve)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 12
    {
        name = "MeasuerLED",
        open = function()
            dc:GetIOpticalAcquire():TurnOnLED()

        end,
        close = function()
            dc:GetIOpticalAcquire():TurnOffLED()
        end,
        status = false,
    },
    -- row 13
    {
        name = "CollectSampleRelay",
        open = function()
            if not string.find(config.info.instrument["type"], "PT63P") then
                WaterCollector.Instance():TurnOn()
            end
        end,
        close = function()
            if not string.find(config.info.instrument["type"], "PT63P") then
                WaterCollector.Instance():TurnOff()
            end
        end,
        status = false,
    },
    -- row 14
    {
        name = "Relay1",
        open = function()
            RelayControl.Instance():TurnOn(2)
        end,
        close = function()
            RelayControl.Instance():TurnOff(2)
        end,
        status = false,
    },
    -- row 15
    {
        name = "Relay2",
        open = function()
            RelayControl.Instance():TurnOn(3)
        end,
        close = function()
            RelayControl.Instance():TurnOff(3)
        end,
        status = false,
    },
    -- row 16
    {
        name = "SampleCurrent4OutputA",
        open = function()
            if HardwareTest[17].status or HardwareTest[18].status then
                return false
            end
            oc:GetIOutputControl():SetOutputCurrent(setting.output.currentA, 4)
        end,
        close = function()
            if HardwareTest[17].status or HardwareTest[18].status then
                return false
            end
            op:CurrentOperate(setting.output.currentA, status.measure.report.measure.consistency)
        end,
        status = false,
    },
    -- row 17
    {
        name = "SampleCurrent12OutputA",
        open = function()
            if HardwareTest[16].status or HardwareTest[18].status then
                return false
            end
            oc:GetIOutputControl():SetOutputCurrent(setting.output.currentA, 12)
        end,
        close = function()
            if HardwareTest[16].status or HardwareTest[18].status then
                return false
            end
            op:CurrentOperate(setting.output.currentA, status.measure.report.measure.consistency)
        end,
        status = false,
    },
    -- row 18
    {
        name = "SampleCurrent20OutputA",
        open = function()
            if HardwareTest[16].status or HardwareTest[17].status then
                return false
            end
            --CurrentResultManager.Instance():OutputSampleCurrent(20)
            oc:GetIOutputControl():SetOutputCurrent(setting.output.currentA, 20)
        end,
        close = function()
            if HardwareTest[16].status or HardwareTest[17].status then
                return false
            end
            op:CurrentOperate(setting.output.currentA, status.measure.report.measure.consistency)
        end,
        status = false,
    },
    -- row 19
    {
        name = "SampleCurrent4OutputB",
        open = function()
            if HardwareTest[20].status or HardwareTest[21].status then
                return false
            end
            oc:GetIOutputControl():SetOutputCurrent(setting.output.currentB, 4)
        end,
        close = function()
            if HardwareTest[20].status or HardwareTest[21].status then
                return false
            end
            op:CurrentOperate(setting.output.currentB, status.measure.report.measure.consistency)
        end,
        status = false,
    },
    -- row 20
    {
        name = "SampleCurrent12OutputB",
        open = function()
            if HardwareTest[19].status or HardwareTest[21].status then
                return false
            end
            oc:GetIOutputControl():SetOutputCurrent(setting.output.currentA, 12)
        end,
        close = function()
            if HardwareTest[19].status or HardwareTest[21].status then
                return false
            end
            op:CurrentOperate(setting.output.currentB, status.measure.report.measure.consistency)
        end,
        status = false,
    },
    -- row 21
    {
        name = "SampleCurrent20OutputB",
        open = function()
            if HardwareTest[19].status or HardwareTest[20].status then
                return false
            end
            --CurrentResultManager.Instance():OutputSampleCurrent(20)
            oc:GetIOutputControl():SetOutputCurrent(setting.output.currentB, 20)
        end,
        close = function()
            if HardwareTest[19].status or HardwareTest[20].status then
                return false
            end
            op:CurrentOperate(setting.output.currentB, status.measure.report.measure.consistency)
        end,
        status = false,
    },
    -- row 22
    {
        name = "SampleCurrent4OutputA",
        open = function()
            if HardwareTest[23].status or HardwareTest[24].status then
                return false
            end
            oc:GetIOutputControl():SetOutputCurrent(setting.output.currentC, 4)
        end,
        close = function()
            if HardwareTest[17].status or HardwareTest[18].status then
                return false
            end
            op:CurrentOperate(setting.output.currentC, status.measure.report.measure.consistency)
        end,
        status = false,
    },
    -- row 23
    {
        name = "SampleCurrent12OutputC",
        open = function()
            if HardwareTest[22].status or HardwareTest[24].status then
                return false
            end
            oc:GetIOutputControl():SetOutputCurrent(setting.output.currentC, 12)
        end,
        close = function()
            if HardwareTest[22].status or HardwareTest[24].status then
                return false
            end
            op:CurrentOperate(setting.output.currentC, status.measure.report.measure.consistency)
        end,
        status = false,
    },
    -- row 24
    {
        name = "SampleCurrent20OutputC",
        open = function()
            if HardwareTest[22].status or HardwareTest[23].status then
                return false
            end
            --CurrentResultManager.Instance():OutputSampleCurrent(20)
            oc:GetIOutputControl():SetOutputCurrent(setting.output.currentC, 20)
        end,
        close = function()
            if HardwareTest[22].status or HardwareTest[23].status then
                return false
            end
            op:CurrentOperate(setting.output.currentC, status.measure.report.measure.consistency)
        end,
        status = false,
    },
    -- row 25
    {
        name = "BoxDownFan",
        open = function()
            --op.ITemperatureControl:BoxFanSetOutputForTOC(setting.temperature.boxDownFan, 0.5)
            oc:GetITemperatureControl():BoxFanSetOutputForTOC(0, 0.5)
        end,
        close = function()
            --op.ITemperatureControl:BoxFanSetOutputForTOC(setting.temperature.boxDownFan, 0)
            oc:GetITemperatureControl():BoxFanSetOutputForTOC(0, 0)
        end,
        status = false,
    },
    -- row 26
    {
        name = "BoxUpFan",
        open = function()
            --dc:SetBoxFanEnable(false)
            --dc:GetITemperatureControl():BoxFanSetOutputForTOC(setting.temperature.boxUpFan, 0.5)
            oc:GetITemperatureControl():BoxFanSetOutputForTOC(1, 0.5)
        end,
        close = function()
            --dc:GetITemperatureControl():BoxFanSetOutputForTOC(setting.temperature.boxUpFan, 0)
            --if not HardwareTest[25].status then
            --    dc:SetBoxFanEnable(true)
            --end
            oc:GetITemperatureControl():BoxFanSetOutputForTOC(1, 0)
        end,
        status = false,
    },
    -- row 27
    {
        name = "Relay1",
        open = function()
            oc:GetIOutputControl():RelayOn(setting.output.relay1)
        end,
        close = function()
            oc:GetIOutputControl():RelayOff(setting.output.relay1)
        end,
        status = false,
    },
    -- row 28
    {
        name = "Relay2",
        open = function()
            oc:GetIOutputControl():RelayOn(setting.output.relay2)
        end,
        close = function()
            oc:GetIOutputControl():RelayOff(setting.output.relay2)
        end,
        status = false,
    },
    -- row 29
    {
        name = "Relay3",
        open = function()
            oc:GetIOutputControl():RelayOn(setting.output.relay3)
        end,
        close = function()
            oc:GetIOutputControl():RelayOff(setting.output.relay3)
        end,
        status = false,
    },
    -- row 30
    {
        name = "Relay4",
        open = function()
            oc:GetIOutputControl():RelayOn(setting.output.relay4)
        end,
        close = function()
            oc:GetIOutputControl():RelayOff(setting.output.relay4)
        end,
        status = false,
    },
    -- row 31
    {
        name = "BoxUpFan",
        open = function()
            rc:GetITemperatureControl():BoxFanSetOutput(0.5)
        end,
        close = function()
            rc:GetITemperatureControl():BoxFanSetOutput(0)
        end,
        status = false,
    },
    -- row 32
    {
        name = "TCValve",
        open = function()
            local curmap = rc:GetISolenoidValve():GetValveMap():GetData()
            local map = RCValveMap.new(curmap | 1)
            rc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = rc:GetISolenoidValve():GetValveMap():GetData()
            local map = RCValveMap.new(curmap & ~1)
            rc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 33
    {
        name = "ICValve",
        open = function()
            local curmap = rc:GetISolenoidValve():GetValveMap():GetData()
            local map = RCValveMap.new(curmap | 2)
            rc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = rc:GetISolenoidValve():GetValveMap():GetData()
            local map = RCValveMap.new(curmap & ~2)
            rc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 34
    {
        name = "Reagent1Valve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | 1)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~1)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 35
    {
        name = "Reagent2Valve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | 2)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~2)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 36
    {
        name = "Res1Valve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | 4)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~4)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 37
    {
        name = "Res2Valve",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | 8)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~8)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 38
    {
        name = "TCPumpDrain",
        open = function()
            if HardwareTest[39].status then
                return false
            end
            local pump = pumps[setting.liquidType.sampleTC.pump + 1]
            pump:Start(RollDirection.Drain, 1000000, 0)
        end,
        close = function()
            local pump = pumps[setting.liquidType.sampleTC.pump + 1]
            pump:Stop()
        end,
        status = false,
    },
    -- row 39
    {
        name = "TCPumpSuck",
        open = function()
            if HardwareTest[38].status then
                return false
            end
            local pump = pumps[setting.liquidType.sampleTC.pump + 1]
            pump:Start(RollDirection.Suck, 1000000, 0)
        end,
        close = function()
            local pump = pumps[setting.liquidType.sampleTC.pump + 1]
            pump:Stop()
        end,
        status = false,
    },
    -- row 40
    {
        name = "ICPumpDrain",
        open = function()
            if HardwareTest[41].status then
                return false
            end
            local pump = pumps[setting.liquidType.sampleIC.pump + 1]
            pump:Start(RollDirection.Drain, 1000000, 0)
        end,
        close = function()
            local pump = pumps[setting.liquidType.sampleIC.pump + 1]
            pump:Stop()
        end,
        status = false,
    },
    -- row 41
    {
        name = "ICPumpSuck",
        open = function()
            if HardwareTest[40].status then
                return false
            end
            local pump = pumps[setting.liquidType.sampleIC.pump + 1]
            pump:Start(RollDirection.Suck, 1000000, 0)
        end,
        close = function()
            local pump = pumps[setting.liquidType.sampleIC.pump + 1]
            pump:Stop()
        end,
        status = false,
    },
    -- row 42
    {
        name = "DeionizedWaterPump",
        open = function()
            local curmap = lc:GetISolenoidValve():GetValveMap():GetData()
            local map = LCValveMap.new(curmap | 16)
            lc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = lc:GetISolenoidValve():GetValveMap():GetData()
            local map = LCValveMap.new(curmap & ~16)
            lc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 43
    {
        name = "ICRPump",
        open = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap | 64)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = dc:GetISolenoidValve():GetValveMap():GetData()
            local map = ValveMap.new(curmap & ~64)
            dc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 44
    {
        name = "LCValve1",
        open = function()
            local curmap = lc:GetISolenoidValve():GetValveMap():GetData()
            local map = LCValveMap.new(curmap | 1)
            lc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = lc:GetISolenoidValve():GetValveMap():GetData()
            local map = LCValveMap.new(curmap & ~1)
            lc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 45
    {
        name = "LCValve2",
        open = function()
            local curmap = lc:GetISolenoidValve():GetValveMap():GetData()
            local map = LCValveMap.new(curmap | 2)
            lc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = lc:GetISolenoidValve():GetValveMap():GetData()
            local map = LCValveMap.new(curmap & ~2)
            lc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 46
    {
        name = "LCValve3",
        open = function()
            local curmap = lc:GetISolenoidValve():GetValveMap():GetData()
            local map = LCValveMap.new(curmap | 4)
            lc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = lc:GetISolenoidValve():GetValveMap():GetData()
            local map = LCValveMap.new(curmap & ~4)
            lc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
    -- row 47
    {
        name = "LCValve4",
        open = function()
            local curmap = lc:GetISolenoidValve():GetValveMap():GetData()
            local map = LCValveMap.new(curmap | 8)
            lc:GetISolenoidValve():SetValveMap(map)
        end,
        close = function()
            local curmap = lc:GetISolenoidValve():GetValveMap():GetData()
            local map = LCValveMap.new(curmap & ~8)
            lc:GetISolenoidValve():SetValveMap(map)
        end,
        status = false,
    },
}

function HardwareTest:execute(row, action, text)
    --	print("HardwareTest try execute "..row)
    local event = text
    local ret
    local err,result = pcall
    (
            function()
                if action == true then
                    --		print("HardwareTest try execute "..row.." open")
                    --保存审计日志
                    SaveToAuditTrailSqlite(nil, nil, event, "关", "开", nil)
                    ret = HardwareTest[row].open()
                    --		print(ret)
                elseif action == false then
                    --		print("HardwareTest try execute "..row.." close")
                    --保存审计日志
                    SaveToAuditTrailSqlite(nil, nil, event, "开", "关", nil)
                    ret = HardwareTest[row].close()
                end
            end
    )
    if not err then      -- 出现异常
        if type(result) == "userdata" then
            if result:GetType() == "CommandTimeoutException" then          --命令应答超时异常
                ExceptionHandler.MakeAlarm(result)
            else
                log:warn("HardwareTest:execute() =>" .. result:What())
            end
        elseif type(result) == "table" then
            log:warn("HardwareTest:execute() =>" .. result:What())								--其他定义类型异常
        elseif type(result) == "string" then
            log:warn("HardwareTest:execute() =>" .. result)	--C++、Lua系统异常
        end
        --		print("Action fail")
        return false     --操作未成功
    else
        if ret == false then
            --			print("operate conflict return")
            return false
        end

        if action == true then		-- 状态记录
            HardwareTest[row].status = true
        elseif action == false then
            HardwareTest[row].status = false
        end
        --		print("Action success")
        return true      --操作成功
    end
end
