config.info.instrument =
{ --仪器信息
    ["model"]="E-Tek 6000 TOC",						-- 型号(产品确定后部可更改)
    ["manuDate"]=
    {													--生产日期
        ["month"]=3,
        ["year"]=2017,
        ["day"]=9,
    },
    ["type"]="EU70-TOC",									-- 类型
    ["manuFacturer"]="深圳市朗石科学仪器有限公司",		-- 生产厂商
    ["uuid"]= "",	-- UUID
    ["sn"]="F00000000",									-- 序列号
    reagentLicense =                                    -- 试剂授权
    {
        enable = false,                                  -- 功能开关
        expirationDate = 0,                              -- 授权截止时间
    },
    expirationDate = 2145888000,
    proLicense = false,         -- DataPro授权
    license = true,
    permanent = true,
    manufactureType = 0,        --0-LS 1-OEM 2-DIY
    showModel = "E-Tek 6000 TOC",             -- 显示型号(可定制显示)
    showName = "总有机碳水质自动在线监测仪(紫外法)",
}
