setting.ui.operation = {}
setting.ui.operation.maintain =
{
    name ="maintain",
    text= "维护",
    rowCount = 30,
    superRow = 0,
    administratorRow = 30,
    writePrivilege=  RoleType.Maintain,
    readPrivilege = RoleType.Maintain,
    {
        name ="MeasureSample",
        text= "测量水样",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.measureSample.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> MeasureFlow:Sample")
            config.system.isWaitting = true
            local flow = MeasureFlow:new({}, MeasureType.Sample)
            flow.name = "MeasureSample"
            flow.text = "测量水样"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="AutoECZero",
        text= "电导率自动归零",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.measureSample.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> AutoECZeroFlow")
            local flow = AutoECZeroFlow:new({}, MeasureType.Sample)
            flow.name = "AutoECZero"
            flow.text = "电导率自动归零"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="MulCalibrate",
        text= "多点校准",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> MulCalibrateFlow")
            local flow = CalibrateFlow:new({}, CalibrateType.mulCalibrate)
            flow.name  = "MulCalibrate"
            flow.text = "多点校准"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Calibrate",
        text= "单点校准(1ppm)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> CalibrateFlow")
            local flow = CalibrateFlow:new({}, CalibrateType.calibrate, 1)
            flow.name  = "Calibrate"
            flow.text = "单点校准(1ppm)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Calibrate",
        text= "单点校准(5ppm)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> CalibrateFlow")
            local flow = CalibrateFlow:new({}, CalibrateType.calibrate, 5)
            flow.name  = "Calibrate"
            flow.text = "单点校准(5ppm)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Calibrate",
        text= "单点校准(10ppm)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> CalibrateFlow")
            local flow = CalibrateFlow:new({}, CalibrateType.calibrate, 10)
            flow.name  = "Calibrate"
            flow.text = "单点校准(10ppm)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Calibrate",
        text= "单点校准(25ppm)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> CalibrateFlow")
            local flow = CalibrateFlow:new({}, CalibrateType.calibrate, 25)
            flow.name  = "Calibrate"
            flow.text = "单点校准(25ppm)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Calibrate",
        text= "单点校准(50ppm)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> CalibrateFlow")
            local flow = CalibrateFlow:new({}, CalibrateType.calibrate, 50)
            flow.name  = "Calibrate"
            flow.text = "单点校准(50ppm)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Confirm",
        text= "单点确认(500ppb)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 0.5)
            flow.name  = "SinglePoint"
            flow.text = "单点确认(500ppb)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Confirm",
        text= "单点确认(1ppm)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 1)
            flow.name  = "SinglePoint"
            flow.text = "单点确认(1ppm)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Confirm",
        text= "单点确认(2ppm)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 2)
            flow.name  = "SinglePoint"
            flow.text = "单点确认(2ppm)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Confirm",
        text= "单点确认(5ppm)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 5)
            flow.name  = "SinglePoint"
            flow.text = "单点确认(5ppm)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Confirm",
        text= "单点确认(10ppm)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 10)
            flow.name  = "SinglePoint"
            flow.text = "单点确认(10ppm)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Confirm",
        text= "单点确认(25ppm)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 25)
            flow.name  = "SinglePoint"
            flow.text = "单点确认(25ppm)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Confirm",
        text= "单点确认(50ppm)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.singlePoint, 50)
            flow.name  = "SinglePoint"
            flow.text = "单点确认(50ppm)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="SystemAdaptability",
        text= "系统适用性确认",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.systemAdaptability, nil)
            flow.name  = "SystemAdaptability"
            flow.text = "系统适用性确认"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="SterileWaterAdaptability",
        text= "无菌水适用性确认",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.sterileWaterAdaptability, nil)
            flow.name  = "SterileWaterAdaptability"
            flow.text = "无菌水适用性确认"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Robustness",
        text= "鲁棒性验证",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.robustness, nil)
            flow.name  = "Robustness"
            flow.text = "鲁棒性验证"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Specificity",
        text= "特异性验证",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.specificity, nil)
            flow.name  = "Specificity"
            flow.text = "特异性验证"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Linear",
        text= "线性验证",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.linear, nil)
            flow.name  = "Linear"
            flow.text = "线性验证"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="SdbsAdaptability",
        text= "SDBS适用性验证",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.sdbsAdaptability, nil)
            flow.name  = "SdbsAdaptability"
            flow.text = "SDBS适用性验证"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="OneKeyRenew",
        text= "一键更新试剂",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.oneKeyRenew.GetTime()
        end,
        createFlow= function()
            local flow = CleanFlow:new({}, cleanType.oneKeyRenew)
            flow.name  = "OneKeyRenew"
            flow.text = "一键更新试剂"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Confirm",
        text= "Turbo单点确认(1ppm)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> TurboConfirmFlow")
            local flow = TurboConfirmFlow:new({}, ConfirmType.singlePoint, 1)
            flow.name  = "SinglePoint"
            flow.text = "Turbo单点确认(1ppm)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Confirm",
        text= "Turbo准确性(500ppb)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> TurboConfirmFlow")
            local flow = TurboConfirmFlow:new({}, ConfirmType.accuracy, 0.5)
            flow.name  = "Accuracy"
            flow.text = "Turbo准确性(500ppb)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Confirm",
        text= "Turbo准确性(8ppm)",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> TurboConfirmFlow")
            local flow = TurboConfirmFlow:new({}, ConfirmType.accuracy, 8)
            flow.name  = "Accuracy"
            flow.text = "Turbo准确性(8ppm)"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Confirm",
        text= "Turbo鲁棒性验证",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> TurboConfirmFlow")
            local flow = TurboConfirmFlow:new({}, ConfirmType.robustness, 0.5)
            flow.name  = "Robustness"
            flow.text = "Turbo鲁棒性验证"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Specificity",
        text= "Turbo特异性验证",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> TurboConfirmFlow")
            local flow = TurboConfirmFlow:new({}, ConfirmType.specificity, nil)
            flow.name  = "Specificity"
            flow.text = "Turbo特异性验证"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="Linear",
        text= "Turbo线性验证",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> TurboConfirmFlow")
            local flow = TurboConfirmFlow:new({}, ConfirmType.linear, nil)
            flow.name  = "Linear"
            flow.text = "Turbo线性验证"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="SdbsAdaptability",
        text= "SDBS适用性验证",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> TurboConfirmFlow")
            local flow = TurboConfirmFlow:new({}, ConfirmType.sdbsAdaptability, nil)
            flow.name  = "TurboSdbsAdaptability"
            flow.text = "TurboSDBS适用性验证"
            FlowList.AddFlow(flow)
        end,
    },
    {
        name ="ICR",
        text= "ICR验证",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
        getRunTime = function()
            return setting.runStatus.calibrate.GetTime()
        end,
        createFlow= function()
            log:debug("Maintain createFlow ==> ConfirmFlow")
            local flow = ConfirmFlow:new({}, ConfirmType.icr, 25)
            flow.name  = "ICR"
            flow.text = "ICR验证"
            FlowList.AddFlow(flow)
        end,
    },
    checkOEM = function()
        return config.system.OEM
    end,

}
return setting.ui.operation.maintain
