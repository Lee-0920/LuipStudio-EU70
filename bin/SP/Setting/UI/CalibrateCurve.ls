setting.ui.calibrateCurve =
{
    columnCount = 2,
    curveType = "A",
    {
        name = "dateTime",
        text = "时间",
        format = "yyyy-MM-dd hh:mm",
        width = 214,
    },
    {-- 校准曲线界面第二列固定为显示曲线，此表位置不能变
        name = "curve",
        text = "曲线",
        format = "%.3f",
        width = 302,
    },
    --{
    --    name = "point0peak",
    --    text = "零点",
    --    format = "%.1f",
    --    width = 72,
    --},
    --{
    --    name = "point1peak",
    --    text = "标点",
    --    format = "%.1f",
    --    width = 70,
    --},
    --{
    --    name = "measureRange",
    --    text = "测试量程",
    --    width = 80,
    --},
    defaultRestore = function(profileTableName, path, curveParamName)
        local func = load("return " .. profileTableName)
        local configTable = func()
        --恢复测量状态
        local defaultTable = ConfigLists.LoadMeasureStatus(true)
        for i=1,setting.measure.range.rangeNum do
            status.measure.calibrate[i].dateTime = defaultTable.calibrate[i].dateTime
            status.measure.calibrate[i].curveB = defaultTable.calibrate[i].curveB
            status.measure.calibrate[i].curveK = defaultTable.calibrate[i].curveK
            status.measure.calibrate[i].point0Consistency = defaultTable.calibrate[i].point0Consistency
            status.measure.calibrate[i].point1Consistency = defaultTable.calibrate[i].point1Consistency
            status.measure.calibrate[i].point0Absorbance = defaultTable.calibrate[i].point0Absorbance
            status.measure.calibrate[i].point1Absorbance = defaultTable.calibrate[i].point1Absorbance
            status.measure.calibrate[i].R2 = defaultTable.calibrate[i].R2
            status.measure.calibrate[i].zeroCalibrateDateTime = defaultTable.calibrate[i].zeroCalibrateDateTime
            status.measure.calibrate[i].standardCalibrateDateTime = defaultTable.calibrate[i].standardCalibrateDateTime
            status.measure.calibrate[i].userSettings = defaultTable.calibrate[i].userSettings
        end
        status.measure.standardCurve = 0
        ConfigLists.SaveMeasureStatus()

        defaultTable = Serialization.LoadEncryptionFile(scriptsPath .. "/Config/Default/" .. path, profileTableName, false)

        for i = 1,setting.measure.range.rangeNum do
            config.measureParam.curveParam[i].RangeConsistency = defaultTable.curveParam[i].RangeConsistency
            config.measureParam.curveParam[i].ZeroConsistency = defaultTable.curveParam[i].ZeroConsistency
            config.measureParam.curveParam[i].timeStr = defaultTable.curveParam[i].timeStr
            config.measureParam.curveParam[i].curveB = defaultTable.curveParam[i].curveB
            config.measureParam.curveParam[i].curveK = defaultTable.curveParam[i].curveK
        end

        setting.ui.profile.measureParam.updaterCurveParam(0,true)
        --if setting.ui.measureDataWindow.calibrateCurve[1].curveParamName == curveParamName then
        --    Helper.DefaultRestore(defaultTable.curveParam, configTable.curveParam)
        --end

        config.modifyRecord.measureParam(true)
        ConfigLists.SaveMeasureParamConfig()
        Serialization.SaveEncryptionFile(configTable, scriptsPath .. "/Config/" .. path, profileTableName)
end,
    saveFile = function(profileTableName, path, isUpdate)
        local func = load("return " .. profileTableName)
        local configTable = func()
        local logger = Log.Instance():GetLogger()

        logger:info(Helper.GetRoleTypeStr() .. " 修改测量参数")
        config.modifyRecord.measureParam(isUpdate)
        ConfigLists.SaveMeasureParamConfig()
        Serialization.SaveEncryptionFile(configTable, scriptsPath .. "/Config/" .. path, profileTableName)
    end,

    changeCurve = function(rangeValve, calibrateTime, currentCalibrate)
        local K,B
        K,B = op:GetCurveParam(currentCalibrate)
        local rangeNum =  setting.measure.range.rangeNum
        for i = 1, rangeNum do
            if math.abs(config.system.rangeViewMap[i].view - rangeValve) < 0.000001 then
                config.measureParam.curveParam[i].curveK = K
                config.measureParam.curveParam[i].curveB = B
                config.measureParam.curveParam[i].timeStr = calibrateTime
                status.measure.calibrate[i].curveB = B
                status.measure.calibrate[i].curveK = K
                for j = 1, 3 do
                    if i == (config.measureParam.range[j] + 1) then
                        config.measureParam.currentRange = j-1
                    end
                end
            end
        end

        ConfigLists.SaveMeasureParamConfig()
        ConfigLists.SaveMeasureStatus()

        setting.ui.profile.measureParam.updaterCurveParam(0,true)
    end,

    checkCurveExist = function(rangeValve)
        local ret = false
        local rangeNum =  setting.measure.range.rangeNum
        for i = 1, rangeNum do
            if math.abs(config.system.rangeViewMap[i].view - rangeValve) < 0.000001 then
                if         i == (config.measureParam.range[1] + 1)
                        or i == (config.measureParam.range[2] + 1)
                        or i == (config.measureParam.range[3] + 1) then
                    ret = true
                end
                break
            end
        end
        return ret
    end,

    getExtraInfo = function(profileTableName)
        local calibrateOne = string.format("%.1f", config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].ZeroConsistency)
        local calibrateTwo = string.format("%.1f", config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].RangeConsistency)
        --return "校准浓度: " .. calibrateOne .. "/" .. calibrateTwo .. "mg/L"
        return " "
    end,

    getCurrentCurve = function(valveName)
        local result = 0
        if valveName == 0 then
            result = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].curveK
        elseif valveName == 1 then
            result = config.measureParam.curveParam[config.measureParam.range[config.measureParam.currentRange + 1]+1].curveB
        elseif valveName == 2 then
            if config.system.rangeViewMap[config.measureParam.range[config.measureParam.currentRange + 1] + 1] ~= nil then
                result = config.system.rangeViewMap[config.measureParam.range[config.measureParam.currentRange + 1] + 1].view
            end
        end
        return result
    end,

    toInput = function(recordData, dateTime, curveK, curveB)

        recordData:PushInt(dateTime) 			        -- 时间
        recordData:PushDouble(curveK)   				            --标线斜率K
        recordData:PushDouble(curveB)   				            --标线截距B

        recordData:PushFloat(0) 			            -- 零点反应峰面积
        recordData:PushFloat(0) 			            -- 零点浓度
        recordData:PushFloat(0)		            	-- 零点第一次峰面积
        recordData:PushFloat(0)		            	-- 零点第二次峰面积
        recordData:PushFloat(0)		            	-- 零点第三次峰面积
        recordData:PushFloat(0)      	-- 零点初始制冷模块温度
        recordData:PushFloat(0) 		        -- 零点初始测量模块温度
        recordData:PushFloat(0) 	    -- 零点反应制冷模块温度
        recordData:PushFloat(0) 	            -- 零点反应测量模块温度
        recordData:PushFloat(0) 	    -- 零点初始值燃烧炉温度
        recordData:PushFloat(0) 	    -- 零点反应值上机箱温度
        recordData:PushFloat(0) 	-- 零点反应值下机箱温度
        recordData:PushFloat(0) 	    -- 零点反应值燃烧炉温度
        recordData:PushFloat(0) 	    -- 零点反应值上机箱温度
        recordData:PushFloat(0) 	-- 零点反应值下机箱温度

        recordData:PushFloat(0) 			                -- 标点峰面积
        recordData:PushFloat(0) 		            	    -- 标点浓度
        recordData:PushFloat(0)		            	-- 标点第一次峰面积
        recordData:PushFloat(0)		            	-- 标点第二次峰面积
        recordData:PushFloat(0)                       -- 标点第三次峰面积
        recordData:PushFloat(0)      	-- 标点初始制冷模块温度
        recordData:PushFloat(0) 		        -- 标点初始测量模块温度
        recordData:PushFloat(0) 	    -- 标点反应制冷模块温度
        recordData:PushFloat(0) 	            -- 标点反应测量模块温度
        recordData:PushFloat(0) 	    -- 标点初始值燃烧炉温度
        recordData:PushFloat(0) 	    -- 标点反应值上机箱温度
        recordData:PushFloat(0) 	-- 标点反应值下机箱温度
        recordData:PushFloat(0) 	    -- 标点反应值燃烧炉温度
        recordData:PushFloat(0) 	    -- 标点反应值上机箱温度
        recordData:PushFloat(0) 	-- 标点反应值下机箱温度

        recordData:PushFloat(1)					                    -- 曲线线性度R2
        recordData:PushInt(0) 	    -- 校准时长
        recordData:PushFloat(config.system.rangeViewMap[config.measureParam.range[config.measureParam.currentRange + 1] + 1].view) --  量程
        return recordData
    end,
    curveParamCurveXYChange = function(valueK, valueB)
        local retK = valueK
        local retB = valueB
        retK, retB = CurveParamCurveXYChange(valueK, valueB)
        return retK, retB
    end,
}
