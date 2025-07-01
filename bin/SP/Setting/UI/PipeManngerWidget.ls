setting.ui.operation.liquidOperator =
{
    name ="liquidOperator",
    text= "管道操作",
    rowCount = 4,
    superRow = 0,
    administratorRow = 0,
    writePrivilege=  RoleType.Maintain,
    readPrivilege = RoleType.Maintain,
    -- row = 1
    {
        name = "SuckFromReagent1",
        text= "填充酸剂",
        data = 0.1,
        createFlow= function(mode, volume)
            local flow = LiquidOperateFlow:new({}, setting.liquidType.reagent1, setting.liquidType.none, mode, volume, 0,setting.runAction.suckFromReagent1)
            flow.name = setting.ui.operation.liquidOperator[1].name
            flow.text = "填充酸剂"
            FlowList.AddFlow(flow)
        end,
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
    },
    {
        name = "SuckFromReagent2",
        text= "填充氧化剂",
        data = 0.1,
        createFlow= function(mode, volume)
            local flow = LiquidOperateFlow:new({}, setting.liquidType.reagent2, setting.liquidType.none, mode, volume, 0,setting.runAction.suckFromReagent2)
            flow.name = setting.ui.operation.liquidOperator[2].name
            flow.text = "填充氧化剂"
            FlowList.AddFlow(flow)
        end,
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
    },
    -- row = 16
    {
        name = "DrainToReagent1",
        text= "排至酸剂管",
        data = 0.1,
        createFlow= function(mode, volume)
            local flow = LiquidOperateFlow:new({}, setting.liquidType.none, setting.liquidType.reagent1, mode,0, volume, setting.runAction.drainToReagent1)
            flow.name = setting.ui.operation.liquidOperator[3].name
            flow.text = "排至酸剂管"
            FlowList.AddFlow(flow)
        end,
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
    },
    -- row = 16
    {
        name = "DrainToReagent2",
        text= "排至氧化剂管",
        data = 0.1,
        createFlow= function(mode, volume)
            local flow = LiquidOperateFlow:new({}, setting.liquidType.none, setting.liquidType.reagent2, mode,0, volume, setting.runAction.drainToReagent2)
            flow.name = setting.ui.operation.liquidOperator[4].name
            flow.text = "排至氧化剂管"
            FlowList.AddFlow(flow)
        end,
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
    },

    checkvalue = function(mode, value)
        local maxPoint = setting.liquid.syringeMaxVolume / 1000
        local PRECISE = 0.000001

        local vol = tonumber(value)

        if not vol then
            return string.format("%.2f", maxPoint)
        end

        if vol > 0.3 then
            return string.format("%.2f", maxPoint)
        elseif vol < 0 or vol - PRECISE < 0 then
            return string.format("%.2f", 0)
        else
            return string.format("%.2f", tonumber(value))
        end
    end,
}
return setting.ui.operation.liquidOperator
