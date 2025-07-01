setting.ui = {}
setting.ui.measureData =
{
    columnCount = 7,
    resultTypeList =
    {
        "全部",
        "在线",
        "离线",
    },
    resultMarkList =
    {
        "全部",
        "正常N",
        "离线B",
        "维护M",
        "故障D",
        "校准C",
        "超量程T",
        "超上限",
        "质控K",
        "调试A",
    },
    {
        name = "dateTime",
        text = "时间",
        format = "yyyy-MM-dd hh:mm:ss",
        width = 168,
    },
    --{
    --    name = "mode",
    --    text = "标识",
    --    width = 59,
    --},
    {
        name = "resultType",
        text = "类型",
        width = 60,
    },
    --{
    --    name = "measureRange",
    --    text = "量程",
    --    format = "%.0f",
    --    width = 67,
    --},
    {
        name = "consistency",
        text = "TOC",
        format = "%.3f",
        width = 110,
    },
    {
        name = "consistencyTC",
        text = "TC",
        format = "%.3f",
        width = 110,
    },
    {
        name = "consistencyIC",
        text = "IC",
        format = "%.3f",
        width = 110,
    },
    {
        name = "peakTC",
        text = "TC峰高",
        format = "%.2f",
        width = 67,
    },
    {
        name = "peakIC",
        text = "IC峰高",
        format = "%.2f",
        width = 67,
    },
    addstandard =
    {
        widget = nil,

        addstandardList = {},

        addConsistency = function(consistency, resultType, addstandardConsistency, range)
            local t = {}
            t.consistency = consistency
            t.resultType = resultType
            t.addstandardConsistency = addstandardConsistency
            t.range = range

            table.insert(setting.ui.measureData.addstandard.addstandardList, t)
        end,

        getInfo = function(record)

            local info = ""
            local  list = setting.ui.measureData.addstandard.addstandardList

            local smc ="水样浓度: "
            local astc ="加标浓度: "
            local smstc ="加标样浓度: "
            local reco = "回收率: "

            local sampleConsistency = 0   --水样测量浓度
            local addstandardConsistency= 0   --量程校准液浓度
            local sampleStandardConsistency= 0  --加标样测量浓度
            local resultType	= 0
            local range = 1
            local recovery = 100

            for i = #list, 1, -1 do

                if list[i] .resultType == MeasureType.Sample then
                    sampleConsistency = list[i] .consistency
                elseif list[i] .resultType == MeasureType.Addstandard then
                    sampleStandardConsistency = list[i] .consistency
                end

                addstandardConsistency= list[i] .addstandardConsistency
                range= list[i] .range
            end

            if range == 1 then
                --非高氯模式
                if math.abs(addstandardConsistency * 10 - math.floor(addstandardConsistency * 10))  < 0.4 then
                    recovery = 100 * (sampleStandardConsistency - sampleConsistency * 0.5) / (addstandardConsistency * 0.5)
                else
                    --开高氯模式
                    addstandardConsistency = addstandardConsistency - 0.05
                    recovery = 100 * (sampleStandardConsistency - sampleConsistency) / (addstandardConsistency * 0.5 )
                end
            else
                recovery = 100 * (sampleStandardConsistency - sampleConsistency) / (addstandardConsistency * 0.5 )
            end

            --log:debug("样品浓度 = "..sampleConsistency.."加标浓度 = "..addstandardConsistency..
            --"加标样浓度 = "..sampleStandardConsistency .. "加标回收率 = " .. recovery .. "%")

            smc = smc ..  string.format("%0.2f", sampleConsistency)
            astc = astc ..  string.format("%0.2f", addstandardConsistency)
            smstc = smstc .. string.format("%0.2f", sampleStandardConsistency)
            reco = reco .. string.format("%0.2f", recovery) .. "%"

            info = smc .. "      " ..  astc .. "      " .. smstc .. "      " ..  reco

            for i = #list, 1, -1 do
                if list[i] then
                    list[i]  = nil
                end
            end

            return info
        end,
    },
    parallel =
    {
        widget = nil,

        parallelList = {},

        addConsistency = function(consistency, resultType, addstandardConsistency, range)
            local t = {}
            t.consistency = consistency
            t.resultType = resultType
            t.addstandardConsistency = addstandardConsistency
            t.range = range

            table.insert(setting.ui.measureData.parallel.parallelList, t)
        end,

        getInfo = function(record)

            local info = ""
            local  list = setting.ui.measureData.parallel.parallelList

            local strSMC ="水样浓度: "
            local strPC ="平行样浓度: "
            local strVC ="MN: "
            local strSD = "SD: "
            local strCV = "CV: "

            local sampleConsistency = 0
            local paralleConsistency= 0
            local resultType	= 0
            local range = 0
            local vc = 0
            local sd = 0
            local cv = 0


            for i = #list, 1, -1 do

                if list[i] .resultType == MeasureType.Sample then
                    sampleConsistency = list[i] .consistency
                elseif list[i] .resultType == MeasureType.Parallel then
                    paralleConsistency = list[i] .consistency
                end

                range= list[i] .range
            end

            vc = (sampleConsistency + paralleConsistency)/2
            sd = math.sqrt((sampleConsistency - vc)^2 + (paralleConsistency - vc)^2)
            cv = 100*sd/vc

            --log:debug("水样浓度 = "..sampleConsistency.."平行样浓度 = "..paralleConsistency.. "均值 = "..
            --vc .. "标准偏差 = " .. sd .. "变异系数 = " .. cv .. "%")

            strSMC = strSMC ..  string.format("%0.2f", sampleConsistency)
            strPC = strPC ..  string.format("%0.2f", paralleConsistency)
            strVC = strVC .. string.format("%0.2f", vc)
            strSD = strSD .. string.format("%0.2f", sd)
            strCV = strCV .. string.format("%0.2f", cv) .. "%"

            info = strSMC .. "     " ..  strPC .. "     " .. strVC .. "     " ..  strSD .. "     " ..  strCV

            for i = #list, 1, -1 do
                if list[i] then
                    list[i]  = nil
                end
            end

            return info
        end,
    },
    unitChange = function(value, changeType) return ArguUnitChange(value, changeType) end,
}
