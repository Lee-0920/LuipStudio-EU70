setting.ui.measureDataPrint =
{
    showPrivilege=  RoleType.Administrator,
    measure =
    {
        printer = nil,
        totalWidth = 48,
        data =
        {
            {
                name = "measureFactor",
                header = " 参数 ",
                content = " TOC  ",
                width = 6,
            },
            {
                name = "dateTime",
                header = "     测量时间       ",
                format = "yyyy-MM-dd hh:mm:ss",
                width = 20,
            },
            {
                name = "consistency",
                header = " 浓度mg/L ",
                format = "%.3f",
                width = 10,
            },
            {
                name = "mode",
                header = "标识  ",
                width = 6,
            },
            {
                name = "resultType",
                header = "类型  ",
                width = 6,
            },
        },
    },
    calibrate =
    {
        printer = nil,
        totalWidth = 51,
        data =
        {
            {
                name = "dateTime",
                header = "     测量时间       ",
                format = "yyyy-MM-dd hh:mm:ss",
                width = 20,
            },
            {
                name = "curveK",
                header = " 斜率 ",
                format = "%.3f",
                width = 13,
            },
            {
                name = "curveB",
                header = " 截距  ",
                format = "%.3f",
                width = 8,
            },
            -- {
            --     name = "point0peak",
            --     header = " 零点 ",
            --     format = "%.3f",
            --     width = 10,
            -- },
            -- {
            --     name = "point1peak",
            --     header = " 标点 ",
            --     format = "%.3f",
            --     width = 10,
            -- },
            {
                name = "measureRange",
                header = " 量程 ",
                width = 8,
            },
        },
    },
}

return setting.ui.measureDataPrint
