
function  CreateControllers()
    local controllerManager = ControllerManager.Instance()
    for i, controller in pairs(setting.plugins.controllers) do
        local name = controller.name
        local text = controller.text
        local addr = DscpAddress.new(controller.address[1],  controller.address[2],  controller.address[3], controller.address[4])
        if type(name)  == "string" then
            if name == "TOCDriveControllerPlugin"then
                dc = TOCDriveController.new(addr)
                controllerManager:AddController(name, text, dc)
            elseif name == "LiquidControllerPlugin"then
                lc = LiquidController.new(addr)
                controllerManager:AddController(name, text, lc)
            elseif name == "ReactControllerPlugin" then
                rc = ReactController.new(addr)
                controllerManager:AddController(name, text, rc)
            elseif name == "OutputControllerPlugin" then
                oc = OutputController.new(addr)
                controllerManager:AddController(name, text, oc)
            end
        end
        addr = nil
    end
end

function  CreatePumps()

    pumps = {}
    for _,v in pairs(setting.liquidType)  do
        if v ~= setting.liquidType.map then
            if pumps[v.pump +1]  == nil and (v.dc == true) then
                pump = PeristalticPump:new()
                pump.index =  v.pump
                pump.isRunning = false
                pump.peristalticPumpInterface = dc:GetIPeristalticPump()
                pumps[v.pump +1] = pump
            end
            if v.lc == true then
                pump = LCPeristalticPump:new()
                pump.index =  v.pump
                pump.isRunning = false
                pump.peristalticPumpInterface = lc:GetIPeristalticPump()
                pumps[v.pump +1] = pump
                --print(v.name)
            end
            --if v.oc == true then
            --    pump = LCPeristalticPump:new()
            --    pump.index =  v.pump
            --    pump.isRunning = false
            --    pump.peristalticPumpInterface = lc:GetIExtPeristalticPump()
            --    pumps[v.pump +1] = pump
            --    --print(v.name)
            --end
            --print(v.name)
        end
    end
end

function  CreateOperator()
    op = Operator:new()
end

function  StopOperator()
    op:Stop()
end

