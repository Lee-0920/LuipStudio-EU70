setting = {}
setting.plugins = {}
setting.plugins.controllers=
{
    -- PT63Drive Controllers
    {
        name = "TOCDriveControllerPlugin",
        text = "驱动板",
        address = {1,1,1,0},
    },
    -- Liquid Controllers
    {
        name = "LiquidControllerPlugin",
        text = "液路板",
        address = {1,1,2,0},
    },
    -- React Controllers
    {
        name = "ReactControllerPlugin",
        text = "信号板",
        address = {1,1,4,0},
    },
    -- Output Controllers
    --{
    --    name = "OutputControllerPlugin",
    --    text = "输出板",
    --    address = {1,1,8,0},
    --},
}


return setting.plugins.controllers
