setting.ui.profile.scheduler =
{
    name = "scheduler",
    text = "测量排期",
    index = 1,
    rowCount = 4,
    superRow = 0,
    administratorRow = 8,
    isMeaParaml = false,
    writePrivilege=  RoleType.Maintain,
    readPrivilege = RoleType.Maintain,
    updateEvent = UpdateEvent.ChangeAutoMeasure,
    {
        name = "automeasurement",
        text = "水样测量",
        {
            name = "measure.mode",
            text = "测量模式",
            type = DataType.Option,
            writePrivilege=  RoleType.Maintain,
            readPrivilege = RoleType.Maintain,
            options =
            {
                "外部触发",
                "周期测量",
                "整点测量",
                "连续测量",
            },
        },
        {
            name = "measure.interval",
            text = "间隔周期",
            type = DataType.Float,
            unit = "小时",
            writePrivilege=  RoleType.Maintain,
            readPrivilege = RoleType.Maintain,
            checkValue = function(value)
                if setting.ui.profile.scheduler.tempPattern(value) == true then
                    return value
                else
                    return string.format("%.1f", config.scheduler.measure.interval)
                end
            end,
        },
        {
            name = "measure.timedPoint",
            text = "整点设置",
            type = DataType.IntArray,
            writePrivilege=  RoleType.Maintain,
            readPrivilege = RoleType.Maintain,
        },
    },
    {
        name = "SchedulerSetting",
        text = "整点启动",
        {
            name = "timedPointJudgeTime",
            text = "判定延长时间",
            type = DataType.Int,
            unit = "秒",
            writePrivilege=  RoleType.Administrator,
            readPrivilege = RoleType.Administrator,
            checkValue = function(value)
                if setting.ui.profile.scheduler.IntPattern(value) == true then
                    local num = tonumber(value)
                    if num < 0 or num > 600 then
                        return string.format("%d", config.scheduler.timedPointJudgeTime)
                    else
                        return value
                    end
                else
                    return string.format("%d", config.scheduler.timedPointJudgeTime)
                end
            end,
        },
    },
    defaultRestore = function(userType)

        local defaultTable = ConfigLists.LoadSchedulerConfig(true)
        Helper.DefaultRestore(defaultTable, config.scheduler)
        local logger = Log.Instance():GetLogger()
        logger:info(Helper.GetRoleTypeStr().." 恢复默认"..setting.ui.profile.scheduler.text)
        config.modifyRecord.scheduler(true)
        ConfigLists.SaveSchedulerConfig()
    end,
    saveFile = function(isUpdate)
        local isChange = false
        local changeTable = {}
        local isRemind = false
        local remindStr = ""

        local logger = Log.Instance():GetLogger()
        logger:info(Helper.GetRoleTypeStr() .. " 修改".. setting.ui.profile.scheduler.text)
        isChange,changeTable=config.modifyRecord.scheduler(isUpdate)
        ConfigLists.SaveSchedulerConfig()

        --检测整点定标功能是否有更改
        if config.scheduler.calibrate.mode == MeasureMode.Timed then
            if changeTable ~= nil and type(changeTable) == "table" then
                for num,name in pairs(changeTable) do
                    if name == "calibrate.mode"
                            or name == "calibrate.oneTimedPoint"
                            or name == "calibrate.timedPointInterval" then
                        isRemind = true
                    end
                end
                if isRemind then
                    local curTime = os.time()
                    local curDateTime = os.date("*t", curTime)
                    curDateTime.hour    = 0
                    curDateTime.min     = 0
                    curDateTime.sec     = 0
                    local newTime = os.time(curDateTime)
                    status.measure.schedule.autoCalibrate.dateTime = newTime
                    config.scheduler.calibrate.configChangeTime = curTime
                    ConfigLists.SaveSchedulerConfig()
                    local isValid
                    local nextStartTime = 0
                    for i,tpye in pairs(setting.measureScheduler) do
                        if tpye.text == "校准" then
                            isValid,nextStartTime = tpye.getNextTime()
                            local DataTime = os.date("*t",nextStartTime)
                            remindStr = DataTime.year.."年"..DataTime.month.."月"..DataTime.day.."日"..DataTime.hour.."时"..DataTime.min.."分"..DataTime.sec.."秒"
                            remindStr = "检测到校准整点模式参数修改，下次启动校准的时间为 "..remindStr
                        end
                    end
                end
            end
        end

        return isRemind,remindStr
    end,
    tempPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local decimalPatterm = "^%d?%d?%d%.%d$"
            local integerPatterm = "^%d?%d?%d$"
            if not string.find(value, decimalPatterm) then
                if string.find(value, integerPatterm) then
                    ret = true
                end
            else
                ret = true
            end
            return ret
        else
            return false
        end
    end,
    IntPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local integerPatterm = "^%d?%d?%d$"
            if string.find(value, integerPatterm) then
                ret = true
            end
            return ret
        else
            return false
        end
    end,
    LongPattern = function(value)
        if type(value) == "string" then
            local ret = false
            local integerPatterm = "^%d?%d?%d?%d$"
            if string.find(value, integerPatterm) then
                ret = true
            end
            return ret
        else
            return false
        end
    end,
}

return setting.ui.profile.scheduler
