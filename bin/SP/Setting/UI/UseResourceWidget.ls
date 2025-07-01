setting.ui.useResourceWidget =
{
    rowCount = 3,
    superRow = 0,
    administratorRow = 0,
    writePrivilege=  RoleType.Maintain,
    readPrivilege = RoleType.Maintain,
    {
        name = "pump",
        text = "泵",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
    },
    {
        name = "uvLamp",
        text = "紫外灯",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
    },
    {
        name = "resin",
        text = "树脂层",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
    },
    checkMaterialLife = function()
       MaterialLifeManager.CheckAllMaterialLife()
    end,
    changeMaterialLife = function(name)
        MaterialLifeManager.Reset(setting.materialType[name])
    end,
}

return setting.ui.useResourceWidget
