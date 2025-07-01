setting.ui.update =
{
    rowCount = 3,
    superRow = 0,
    administratorRow = 3,
    writePrivilege=  RoleType.Administrator,
    readPrivilege = RoleType.Administrator,
    {
        name = "TOCDriveControllerPlugin",
        text = "驱动板",
        writePrivilege = RoleType.Administrator,
        readPrivilege = RoleType.Administrator,
        fileName = "EU70DriveController.hex",
    },
    {
        name = "LiquidControllerPlugin",
        text = "液路板",
        writePrivilege=  RoleType.Administrator,
        readPrivilege = RoleType.Administrator,
        fileName = "EU70LiquidController.hex",
    },
    {
        name = "ReactControllerPlugin",
        text = "信号板",
        writePrivilege=  RoleType.Administrator,
        readPrivilege = RoleType.Administrator,
        fileName = "EU70ReactController.hex",
    },
    --{
    --    name = "OutputControllerPlugin",
    --    text = "输出板",
    --    writePrivilege=  RoleType.Administrator,
    --    readPrivilege = RoleType.Administrator,
    --    fileName = "EU70OutputController.hex",
    --},
}
return setting.ui.update
