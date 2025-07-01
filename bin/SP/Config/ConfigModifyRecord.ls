config = {}
config.modifyRecord =
{
    loadTableValue = function(valueName, valuetable)
        if type(valueName)=="string" then
            local t = valuetable or _G
            for k, v in string.gmatch (valueName, "([%w_]+)(.?)") do
                if v == "." or v == "[" or v == "]" then  -- 不是最后一项
                    local ret = tonumber(k)
                    if ret ~= nil then
                        t = t[ret]
                    else
                        t = t[k]
                    end
                else
                    return t[k]
                end
            end
            return t
        end
    end,
    compare = function(originalTable, uiprofileTable, isUpdate)
        local PRECISE = 0.000001
        local logger = Log.Instance():GetLogger()
        local configVector = ConfigVector.new()
        local isChanged = false
        local roleStr = Helper.GetRoleTypeStr() .. " "
        local changeTable = {}

        for _,typeTable in ipairs(uiprofileTable) do
            if type(typeTable) == "table" then
                for _,item in ipairs(typeTable) do
                    if type(item) == "table" then
                        local changeText = uiprofileTable.text .. "-" .. typeTable.text .. "-" .. item.text .." 更改 "
                        local changeName = "config." .. uiprofileTable.name .. "." .. item.name
                        local changeValue = config.modifyRecord.loadTableValue(changeName)
                        local originalValue = config.modifyRecord.loadTableValue(item.name, originalTable)
                        local unit = item.unit or ""
                        local valueStr = ""
                        local str = nil
                        local oldSetting = ""
                        local newSetting = ""

                        if (item.type == DataType.Bool) then
                            local value = ""
                            if changeValue == true and originalValue == false then
                                table.insert(changeTable,item.name)
                                str = changeText .. "关 -> 开"
                                valueStr = "true"
                                isChanged = true
                                oldSetting = "关"
                                newSetting = "开"
                            elseif changeValue == false and originalValue == true then
                                table.insert(changeTable,item.name)
                                str = changeText .. "开 -> 关"
                                valueStr = "false"
                                isChanged = true
                                oldSetting = "开"
                                newSetting = "关"
                            end

                        elseif (item.type == DataType.Option) then
                            if originalValue ~=  changeValue then
                                table.insert(changeTable,item.name)
                                str = changeText .. item.options[originalValue + 1] .. unit .. " -> "
                                        .. item.options[changeValue + 1] .. unit
                                valueStr = string.format("%d", changeValue)
                                isChanged = true
                                oldSetting = item.options[originalValue + 1]
                                newSetting = item.options[changeValue + 1]
                            end

                        elseif (item.type == DataType.Int) then
                            if originalValue ~=  changeValue then
                                table.insert(changeTable,item.name)
                                str = changeText .. string.format("%d ", originalValue) .. unit .. " -> "
                                        .. string.format("%d", changeValue) .. unit
                                valueStr = string.format("%d", changeValue)
                                isChanged = true
                                oldSetting = string.format("%d ", originalValue)
                                newSetting = string.format("%d ", changeValue)
                            end

                        elseif (item.type == DataType.Float) then
                            if changeValue ~= nil and originalValue ~= nil then
                                if math.abs(originalValue - changeValue) > PRECISE then
                                    table.insert(changeTable,item.name)
                                    str = changeText .. string.format("%f ", originalValue) .. unit .. " -> "
                                            .. string.format("%f ", changeValue) .. unit
                                    valueStr = string.format("%f", changeValue)
                                    isChanged = true
                                    oldSetting = string.format("%f ", originalValue)
                                    newSetting = string.format("%f ", changeValue)
                                end
                            end

                        elseif (item.type == DataType.Double) then
                            if changeValue ~= nil and originalValue ~= nil then
                                if math.abs(originalValue - changeValue) > PRECISE then
                                    table.insert(changeTable,item.name)
                                    str = changeText .. string.format("%f ", originalValue) .. unit .. " -> "
                                            .. string.format("%f ", changeValue) .. unit
                                    valueStr = string.format("%f", changeValue)
                                    isChanged = true
                                    oldSetting = string.format("%f ", originalValue)
                                    newSetting = string.format("%f ", changeValue)
                                end
                            end

                        elseif (item.type == DataType.String) then
                            if originalValue ~=  changeValue then
                                table.insert(changeTable,item.name)
                                str = changeText .. originalValue .. " -> "
                                        .. changeValue
                                valueStr = changeValue
                                isChanged = true
                                oldSetting = originalValue
                                newSetting = changeValue
                            end

                        elseif (item.type == DataType.IP) then
                            if originalValue ~=  changeValue then
                                table.insert(changeTable,item.name)
                                str = changeText .. originalValue .. " -> "
                                        .. changeValue
                                valueStr = changeValue
                                isChanged = true
                                oldSetting = originalValue
                                newSetting = changeValue
                            end
                        elseif (item.type == DataType.IntArray) then
                            local isTimedEquation = true
                            for k,v in ipairs(originalValue) do
                                if v ~= changeValue[k] then
                                    isTimedEquation = false
                                    break
                                end
                            end

                            if isTimedEquation == false then
                                local newTimeStr = ""
                                local oldTimeStr = ""
                                for k,v in ipairs(originalValue) do
                                    if v then
                                        oldTimeStr = oldTimeStr .. tostring(k - 1) .. "/"
                                    end
                                end
                                for k,v in ipairs(changeValue) do
                                    if v then
                                        newTimeStr = newTimeStr .. tostring(k - 1) .. "/"
                                    end
                                end
                                oldTimeStr = string.sub(oldTimeStr, 1, string.len(oldTimeStr)-1)
                                newTimeStr = string.sub(newTimeStr, 1, string.len(newTimeStr)-1)
                                str = changeText .. oldTimeStr .. " -> " .. newTimeStr
                                valueStr = newTimeStr
                                table.insert(changeTable,item.name)
                                isChanged = true
                                if isUpdate == true then
                                    local configValue = Config.new()
                                    configValue:__set_profile(uiprofileTable.name)
                                    configValue:__set_config(item.name)
                                    configValue:__set_value(valueStr)
                                    configVector:Push(configValue)
                                end
                                oldSetting = oldTimeStr
                                newSetting = newTimeStr
                            end

                            --检测到药典变更
                            if item.name == "pharmacopoeia" and isTimedEquation == false then
                                local newTimeStr = ""
                                local oldTimeStr = ""
                                for k,v in ipairs(originalValue) do
                                    if v == true then
                                        oldTimeStr = oldTimeStr .. setting.instrument.pharmacopoeiaName[k] .. "/"
                                    end
                                end
                                for k,v in ipairs(changeValue) do
                                    if v == true then
                                        newTimeStr = newTimeStr .. setting.instrument.pharmacopoeiaName[k] .. "/"
                                    end
                                end
                                str = changeText .. oldTimeStr .. " -> " .. newTimeStr
                                oldSetting = oldTimeStr
                                newSetting = newTimeStr
                            end
                        end

                        if str ~= nil then
                            if config.system.changeParamLog then
                                logger:info(roleStr .. str .. " 成功")
                            else
                                logger:debug(str)
                            end
                            --保存审计日志
                            SaveToAuditTrailSqlite(nil, nil, changeText, oldSetting, newSetting, nil)
                        end

                        if valueStr ~= "" and isUpdate == true then
                            local configValue = Config.new()
                            configValue:__set_profile(uiprofileTable.name)
                            configValue:__set_config(item.name)
                            configValue:__set_value(valueStr)
                            configVector:Push(configValue)
                        end
                    end
                end
            end
        end

        if configVector:Size() > 0  and isUpdate == true then
            WqimcManager.Instance():updateConfigs(configVector)
        end

        return isChanged,changeTable
    end,
    scheduler = function(isUpdate)
        local uiprofileTable = setting.ui.profile.scheduler
        local originalTable = ConfigLists.LoadSchedulerConfig(false)
        return config.modifyRecord.compare(originalTable, uiprofileTable, isUpdate)
    end,

    measureParam = function(isUpdate)
        local uiprofileTable = setting.ui.profile.measureParam
        local originalTable = ConfigLists.LoadMeasureParamConfig(false)
        return config.modifyRecord.compare(originalTable, uiprofileTable, isUpdate)
    end,

    interconnection = function(isUpdate)
        local uiprofileTable = setting.ui.profile.interconnection
        local originalTable = ConfigLists.LoadInterconnectionConfig(false)
        return config.modifyRecord.compare(originalTable, uiprofileTable, isUpdate)
    end,

    system = function(isUpdate)
        local uiprofileTable = setting.ui.profile.system
        local originalTable = ConfigLists.LoadSystemConfig(false)
        return config.modifyRecord.compare(originalTable, uiprofileTable, isUpdate)
    end,
}
