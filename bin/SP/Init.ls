log = Log.Instance():GetLogger()

package.path = [[?;?.lua;?.ls;?;./SP/Common/?.ls;./SP/Setting/?.ls]]
require("Serialization")
require("Setting")
package.path = [[?;?.lua;?.ls;?;./SP/Common/?.ls;./SP/Setting/?.ls;./SP/Config/?.ls]]
require("ControlNetException")
require("MeasureFlowException")

--脚本文件夹所在路径
scriptsPath = "./SP"
require("ConfigLists")
require("ConfigModifyRecord")

--[[
 * @brief 标线XY进行调换。
 * @details 将标线模型mAbs=K*C+B转换成C=K*mAbs+B
 * @param[in] valueK 标线K值，或者表示变量X含义的字符串
 * @param[in] valueB 标线B值，或者表示变量Y含义的字符串
 * @return retK 转换后的标线K值，或者表示变量X含义的字符串
 * @return retB 转换后的标线B值，或者表示变量Y含义的字符串
--]]
function CurveParamCurveXYChange(valueK, valueB)
    local retK = valueK
    local retB = valueB

    if config.system.curveXYChange == false then
        -- 转换关闭,直接返回
        return retK, retB
    end

    if type(valueK) == "number" and type(valueK) == "number" then
        if valueK == 0 then
            retK = 0
            retB = 0
        else
            retK = 1 / valueK
            retB = (- valueB) / valueK
        end
        --print("valueK = " .. valueK .. " valueB = " .. valueB .. ", retK = " ..  retK .. " retB = " .. retB)
        return retK, retB
    end

    if type(valueK) == "string" and type(valueB) == "string" then
        if valueK == "C" then
            retK = "mAbs"
        end

        if valueB == "mAbs" then
            retB = "C"
        end
        --print("valueK = " .. valueK .. " valueB = " .. valueB .. ", retK = " ..  retK .. " retB = " .. retB)
        return retK, retB
    end
    --print("valueK = " .. valueK .. " valueB = " .. valueB .. ", retK = " ..  retK .. " retB = " .. retB)
    return retK, retB
end


function ArguUnitChange(value, changeType)
    local ret = value
    local unitIndex = config.system.unitIndex
    local valid = 0
    if setting.unit.valid == true then
        valid = 1
    end
    if (unitIndex == 1 and setting.unit.valid == true) or (setting.unit.valid == false and unitIndex == 0)then
        if type(value) == "string" then
            if value == "mg/L" then
                ret = "ppb"
            else
                ret = "ppm"
            end
        elseif type(value) == "number" then
            if changeType == UnitChange.Read then
                ret = value*setting.unit[2].multiple
                -- print("read value = " .. value .. ", ret = ", ret)
            elseif changeType == UnitChange.Write then
                ret = value/setting.unit[2].multiple
                --print("write value = " .. value .. ", ret = ", ret)
            end
        end
    end

    return ret
end

--[[
 * @brief 保存审计日志到数据库中。
 * @details 以"#"分隔各个输入参数
 * @param[in] role 用户名称字符串 (运维、管理员或者新建的个人名称)
 * @param[in] level 用户等级字符串
 * @param[in] option 操作字符串(开始、停止测量，修改某参数名称)
 * @param[in] oldSetting 参数旧值字符串(开、关、数字等)
 * @param[in] newSetting 参数新值字符串(开、关、数字等)
 * @param[in] detail 详情信息字符串(若某操作需要特殊说明，应当在详情进行说明)
--]]
function SaveToAuditTrailSqlite(role, level, event, oldSetting, newSetting, detail)
    local roleInfo = Helper.GetRoleTypeStr()
    local levelInfo = Helper.GetLevelStr()
    local eventInfo = "--"
    local oldSettingInfo = "--"
    local newSettingInfo = "--"
    local detailInfo = "--"
    local createTime = tostring(os.time())
    --local createTime = os.date("%Y-%m-%d %H:%M:%S",os.time())
    if role ~= nil then
        roleInfo = role
    end
    if level ~= nil then
        levelInfo = level
    end
    if event ~= nil then
        eventInfo = event
    end
    if oldSetting ~= nil then
        oldSettingInfo = oldSetting
    end
    if newSetting ~= nil then
        newSettingInfo = newSetting
    end
    if detail ~= nil then
        detailInfo = detail
    end

    local info = createTime .. "#" .. roleInfo .. "#" .. levelInfo .. "#" .. eventInfo .. "#"
                    .. oldSettingInfo .. "#" .. newSettingInfo .. "#" .. detailInfo
    local resultManager = ResultManager.Instance()
    resultManager:AddAuditTrailRecord(info)
end

--[[
 * @brief 保存当前方法参数到数据库中。
 * @details 以"#"分隔各个输入参数
 * @details 成员个数必须保证正确，当前13个成员
--]]
function SaveToMethodSqlite()
    local methodName = config.measureParam.methodName
    --local createTime = tostring(os.time())
    local createTime = os.date("%Y-%m-%d %H:%M:%S",os.time())
    local meaType = config.measureParam.meaType --0-在线 1-离线
    local turboMode = config.measureParam.turboMode == true and 1 or 0
    local ICRMode = config.measureParam.ICRMode == true and 1 or 0
    local TOCMode = config.measureParam.TOCMode == true and 1 or 0
    local ECMode = config.measureParam.ECMode == true and 1 or 0
    local autoReagent = config.measureParam.autoReagent and 1 or 0
    local reagent1Vol = config.measureParam.reagent1Vol
    local reagent2Vol = config.measureParam.reagent2Vol
    local normalRefreshTime = config.measureParam.normalRefreshTime
    local measureTimes = config.measureParam.measureTimes
    local rejectTimes = config.measureParam.rejectTimes

    local info = methodName .. "#" .. createTime .. "#" .. tostring(meaType) .. "#" .. tostring(turboMode) .. "#" .. tostring(ICRMode) .. "#"
            .. tostring(TOCMode) .. "#" .. tostring(ECMode) .. "#" .. tostring(autoReagent) .. "#" .. tostring(reagent1Vol) .. "#" .. tostring(reagent2Vol) .. "#"
            .. tostring(normalRefreshTime) .. "#" .. tostring(measureTimes) .. "#" .. tostring(rejectTimes)

    local mthodViewWidget = MethodViewWidget.Instance()
    mthodViewWidget:MethodSaveForModbus(info)
end