setting.ui.operation.combineOperator =
{
    name ="combineOperator",
    text= "组合操作",
    rowCount = 5,
    superRow = 0,
    administratorRow = 5,
    writePrivilege=  RoleType.Maintain,
    readPrivilege = RoleType.Maintain,
    -- row = 1
    {
        name = "SyringReagent1Reset",
        text= "酸剂注射器复位",
        data = 1,
        mode = 5,
        createFlow= function(mode, volume)
            local flow = CombineOperateFlow:new({}, setting.liquidType.reagent1, setting.liquidType.none, mode, volume, volume, setting.runAction.syringUpdate)
            flow.name = setting.ui.operation.combineOperator[1].name
            flow.text = "酸剂注射器复位"
            FlowList.AddFlow(flow)
        end,
        writePrivilege=  RoleType.Administrator,
        readPrivilege = RoleType.Administrator,
    },
    -- row = 2
    {
        name = "SyringReagent2Reset",
        text= "氧化剂注射器复位",
        data = 1,
        mode = 5,
        createFlow= function(mode, volume)
            local flow = CombineOperateFlow:new({}, setting.liquidType.reagent2, setting.liquidType.none, mode, volume, volume, setting.runAction.syringUpdate)
            flow.name = setting.ui.operation.combineOperator[2].name
            flow.text = "氧化剂注射器复位"
            FlowList.AddFlow(flow)
        end,
        writePrivilege=  RoleType.Administrator,
        readPrivilege = RoleType.Administrator,
    },
    -- row = 3
    {
        name = "UpdateReagent1",
        text= "酸剂注射器更新",
        data = 1,
        mode = 5,
        createFlow= function(mode, volume)
            local flow = CombineOperateFlow:new({}, setting.liquidType.none, setting.liquidType.reagent1, mode, volume, volume, setting.runAction.syringUpdate)
            flow.name = setting.ui.operation.combineOperator[3].name
            flow.text = "酸剂注射器更新"
            FlowList.AddFlow(flow)
        end,
        writePrivilege=  RoleType.Administrator,
        readPrivilege = RoleType.Administrator,
    },
    -- row = 4
    {
        name = "UpdateReagent2",
        text= "氧化剂注射器更新",
        data = 1,
        mode = 5,
        createFlow= function(mode, volume)
            local flow = CombineOperateFlow:new({}, setting.liquidType.none, setting.liquidType.reagent2, mode, volume, volume, setting.runAction.syringUpdate)
            flow.name = setting.ui.operation.combineOperator[4].name
            flow.text = "氧化剂注射器更新"
            FlowList.AddFlow(flow)
        end,
        writePrivilege=  RoleType.Administrator,
        readPrivilege = RoleType.Administrator,
    },
    -- row = 5
    {
        name = "MixLiquid",
        text= "注射器流量测试",
        data = 1,
        mode = 5,
        createFlow= function(mode, volume)
            local flow = CombineOperateFlow:new({}, setting.liquidType.reagent1, setting.liquidType.reagent2, mode, volume, volume, setting.runAction.syringUpdate)
            flow.name = setting.ui.operation.combineOperator[5].name
            flow.text = "注射器流量测试"
            FlowList.AddFlow(flow)
        end,
        writePrivilege=  RoleType.Administrator,
        readPrivilege = RoleType.Administrator,
    },
    checkvalue = function(mode, value)
        local pointNum = config.hardwareConfig.meterPoint.num
        local maxPoint = config.hardwareConfig.meterPoint.point[pointNum].setVolume
        local minPoint = config.hardwareConfig.meterPoint.point[1].setVolume
        local PRECISE = 0.000001

        local vol = tonumber(value)

        if not vol then
            return string.format("%.2f", maxPoint)
        end

        if mode == 1 then
            local operateVol = 0
            if vol > 18 then
                return string.format("%.2f", 3*maxPoint)
            elseif vol < minPoint then
                return string.format("%.2f", minPoint)
            else
                local volList = {}

                for i = 1, pointNum do
                    volList[i] = config.hardwareConfig.meterPoint.point[i].setVolume
                end

                if volList[1] - vol >= -PRECISE then -- 输入体积小于或等于低定量点
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
                        operateVol = maxPoint
                    end

                end
                return string.format("%.2f", operateVol)
            end

        elseif mode == 2 then

            local operateVol = 0
            local volList = {}

            for i = 1, pointNum do
                volList[i] = config.hardwareConfig.meterPoint.point[i].setVolume
            end

            if vol <= PRECISE then -- 输入体积为0

                operateVol = minPoint

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
            else  --输入体积大于等于高定量点
                operateVol = maxPoint
            end
            return string.format("%.2f", operateVol)

        elseif mode == 3 then
            if type(value) == "string" then
                local patterm = "^%d?%d?%d?%d$"
                if string.find(value, patterm) then
                    if vol >= 3 and vol <= 5 then
                        return tostring(vol)
                    end
                end
            end
            return tostring(4)
        elseif mode == 4 then
            if type(value) == "string" then
                local patterm = "^%d?%d?%d?%d$"
                if string.find(value, patterm) then
                    if vol >= 1 and vol <= setting.liquid.syringeUpdateUpperLimit then
                        return tostring(vol)
                    end
                end
            end
            return tostring(1)
        elseif mode == 5 then
            if vol <= 24 and vol > 0 then
                return tostring(vol)
            else
                return tostring(6)
            end

        else  --泵计步 type == 0
            if vol > 200 then
                return string.format("%.2f", maxPoint)
            elseif vol < 0 or vol - PRECISE < 0 then
                return string.format("%.2f", 0)
            else
                return string.format("%.2f", tonumber(value))
            end
        end
    end,
}
return setting.ui.operation.combineOperator