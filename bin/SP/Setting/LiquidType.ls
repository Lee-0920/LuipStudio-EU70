setting.liquidType = {}

setting.liquidType.map =
{
    valve1 = 1,
    valve2 = 2,
    valve3 = 4,
    valve4 = 8,
    valve5 = 16,
    valve6 = 32,
    valve7 = 64,
    valve8 = 128,
    valve9 = 256,
    valve10= 512,
    valve11= 1024,
    valve12= 2048,
    valve13= 4096,
    valve14= 8192,
    valve15= 16384,
    valve16= 32768,
    lcOffsetIndex = 4,
    fan = 32,
}

--无任何阀
setting.liquidType.none =
{
    name = "None",
    pump = 0,
    valve = 0,
    dc = true,
}

--无任何阀
setting.liquidType.sampleTC =
{
    name = "SampleTC",
    pump = 0 + setting.liquidType.map.lcOffsetIndex,
    valve = 0,
    lc = true,
}

--无任何阀
setting.liquidType.sampleIC =
{
    name = "SampleIC",
    pump = 1 + setting.liquidType.map.lcOffsetIndex,
    valve = 0,
    lc = true,
}

--试剂1
setting.liquidType.reagent1 =
{
    name = "Reagent1",
    pump = 1,
    valve = setting.liquidType.map.valve2,
    dc = true,
}

--试剂2
setting.liquidType.reagent2 =
{
    name = "Reagent2",
    pump = 0,
    valve = setting.liquidType.map.valve1,
    dc = true,
}
--总阀
setting.liquidType.master =
{
    name = "Master",
    pump = 0,
    valve = setting.liquidType.map.valve4,
    dc = true,
}

--水样
setting.liquidType.sample =
{
    name = "Sample",
    pump = 0,
    valve = setting.liquidType.map.valve1,
    dc = true,
}

--量程校准液
setting.liquidType.standard =
{
    name = "Standard",
    pump = 0,
    valve = setting.liquidType.map.valve2,
    dc = true,
}

--零点校准液
setting.liquidType.blank =
{
    name = "Blank",
    pump = 0,
    valve = setting.liquidType.map.valve3,
    dc = true,
}

--量程核查液
setting.liquidType.rangeCheck =
{
    name = "RangeCheck",
    pump = 0,
    valve = setting.liquidType.map.valve15,
    dc = true,
}

--零点核查液
setting.liquidType.zeroCheck =
{
    name = "ZeroCheck",
    pump = 0,
    valve = setting.liquidType.map.valve14,
    dc = true,
}

--消解室
setting.liquidType.digestionRoom =
{
    name = "DigestionRoom",
    pump = 0,
    valve = setting.liquidType.map.valve10,
    dc = true,
}

--废水
setting.liquidType.wasteWater =
{
    name = "WasteWater",
    pump = 0,
    valve = setting.liquidType.map.valve11,
    dc = true,
}

--分析废液
setting.liquidType.waste =
{
    name = "Waste",
    pump = 0,
    valve = setting.liquidType.map.valve6,
    dc = true,
}

--气密性检查阀
setting.liquidType.gas =
{
    name = "Gas",
    pump = 0,
    valve = setting.liquidType.map.valve2
            + setting.liquidType.map.valve3
            + setting.liquidType.map.valve7,
    dc = true,
}

--注射器废液
setting.liquidType.syringeWaste =
{
    name = "SyringeWaste",
    pump = 2,
    valve = setting.liquidType.map.valve4,
    dc = true,

}

--注射器空白水
setting.liquidType.syringeBlank =
{
    name = "SyringeBlank",
    pump = 2,
    valve = setting.liquidType.map.valve8
            +setting.liquidType.map.valve4,
    dc = true,

}

--ICR液路
setting.liquidType.icr =
{
    name = "ICR",
    pump = 0,
    valve = setting.liquidType.map.valve3
            + setting.liquidType.map.valve4
            + setting.liquidType.map.valve7,
    dc = true,
}

--去离子液路
setting.liquidType.deionizedOn =
{
    name = "Deionized",
    pump = 0 + setting.liquidType.map.lcOffsetIndex,
    valve = setting.liquidType.map.valve1 + setting.liquidType.map.valve5,
    lc = true,
}

setting.liquidType.deionizedOff =
{
    name = "Deionized",
    pump = 0 + setting.liquidType.map.lcOffsetIndex,
    valve = setting.liquidType.map.valve5,
    lc = true,
}

return setting.liquidType
