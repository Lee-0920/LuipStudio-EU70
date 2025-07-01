setting.ui.reagentManager =
{
    rowCount = 2,
    writePrivilege=  RoleType.Maintain,
    readPrivilege = RoleType.Maintain,
    superRow = 0,
    administratorRow = 0,
    changeReagent = function(name, vol)
        ReagentRemainManager.ChangeReagent(setting.liquidType[name], vol)
    end,
    checkReagentDate = function()
       ReagentRemainManager.CheckAllReagentDate()
    end,
    {
        name = "reagent1",
        text = "酸剂",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
    },
    {
        name = "reagent2",
        text = "氧化剂",
        writePrivilege=  RoleType.Maintain,
        readPrivilege = RoleType.Maintain,
    },
}

return setting.ui.reagentManager
