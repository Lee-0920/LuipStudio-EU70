setting.ui.curve =
{
    name = "curve",
    text = "量程信息",
    rowCount = 10,
    {
        range = "0-" .. tostring(setting.measure.range[1].viewRange),
    },
    {
        range = "0-" .. tostring(setting.measure.range[2].viewRange),
    },
    {
        range = "0-" .. tostring(setting.measure.range[3].viewRange),
    },
    {
        range = "0-" .. tostring(setting.measure.range[4].viewRange),
    },
    {
        range = "0-" .. tostring(setting.measure.range[5].viewRange),
    },
    UpdateCurve = function()
        for k,v in pairs(config.system.rangeViewMap) do
            setting.ui.curve[k].range = "0-" .. tostring(v.view)
        end
    end,
    getCalibrationTime = function(curveNum)
        local calibrationTime = status.measure.calibrate[curveNum+1].dateTime
        local timeStr = "--"
        if calibrationTime ~= nil then
            if calibrationTime ~= 0 then
                local DataTime = os.date("*t",calibrationTime)

                local year = tostring(DataTime.year)
                local month
                if DataTime.month <10 then
                    month="0"..tostring(DataTime.month)
                else
                    month=tostring(DataTime.month)
                end
                local day
                if DataTime.day <10 then
                    day="0"..tostring(DataTime.day)
                else
                    day=tostring(DataTime.day)
                end
                local hour
                if DataTime.hour <10 then
                    hour="0"..tostring(DataTime.hour)
                else
                    hour=tostring(DataTime.hour)
                end
                local min
                if DataTime.min <10 then
                    min="0"..tostring(DataTime.min)
                else
                    min=tostring(DataTime.min)
                end
                local sec
                if DataTime.sec <10 then
                    sec="0"..tostring(DataTime.sec)
                else
                    sec=tostring(DataTime.sec)
                end


                -- 拼接成  年-月-日 时:分:秒
                timeStr = year.."-"..month.."-"..day.." "..hour..":"..min

                config.measureParam.curveParam[curveNum+1].timeStr = year.."-"..month.."-"..day.." "..hour..":"..min
                ConfigLists.SaveMeasureParamConfig()
            end
        end
        return timeStr
    end,

    getCurveK = function(curveNum)
        if config.measureParam.curveParam[curveNum+1].curveK ~= nil then
            local ret = tonumber(string.format("%.2f",config.measureParam.curveParam[curveNum+1].curveK))
            return ret
        else
            return 0
        end
    end,

    getCurveB = function(curveNum)
        if config.measureParam.curveParam[curveNum+1].curveB ~= nil then
            local ret = tonumber(string.format("%.2f",config.measureParam.curveParam[curveNum+1].curveB))
            return ret
        else
            return 0
        end
    end,

    getZeroConsistency = function(curveNum)
        if config.measureParam.curveParam[curveNum+1].ZeroConsistency ~= nil then
            local ret = tonumber(string.format("%.2f",config.measureParam.curveParam[curveNum+1].ZeroConsistency))
            return ret
        else
            return 0
        end
    end,

    getRangeConsistency = function(curveNum)
        if config.measureParam.curveParam[curveNum+1].RangeConsistency ~= nil then
            local ret = tonumber(string.format("%.2f",config.measureParam.curveParam[curveNum+1].RangeConsistency))
            return ret
        else
            return 0
        end
    end,
}

return setting.ui.curve
