local _G = _ENV

local type = type
local ipairs = ipairs
local pairs = pairs
local print = print
local io = io
local Serialization = Serialization
local scriptsPath = scriptsPath
local pcall = pcall
local error = error
local string = string
local Log = Log
local App = App

local P = {}
ConfigLists = P
_ENV = P

local defaultPath	= "./SP/Config/Default" --默认的配置文件路径
local dataPath = "./SP/Config/"			 --实际配置文件路径
local protectionZonePath = "/mnt/datadisk/Data"	 --保护区域文件路径


local configFile =
{
    InstrumentInfomation =
    {
        fileName = "InstrumentInfomation.ls",
        tableName = "config.info.instrument",
    },
    MainBoardInfomation =
    {
        fileName = "MainBoardInfomation.ls",
        tableName = "config.info.mainBoard",
    },
    InterconnectionConfig =
    {
        fileName = "InterconnectionConfig.ls",
        tableName = "config.interconnection",
    },
    SchedulerConfig =
    {
        fileName = "SchedulerConfig.ls",
        tableName = "config.scheduler",
    },
    Authorization =
    {
        fileName = "Authorization.ls",
        tableName = "config.authorization",
    },
    SystemConfig =
    {
        fileName = "SystemConfig.ls",
        tableName = "config.system",
    },
    MeasureParamConfig =
    {
        fileName = "MeasureParamConfig.ls",
        tableName = "config.measureParam",
    },
    MeasureStatus =
    {
        fileName = "MeasureStatus.ls",
        tableName = "status.measure",
    },
    MeasureRunning =
    {
        fileName = "MeasureRunning.ls",
        tableName = "status.running",
    },
    HardwareConfig =
    {
        fileName = "HardwareConfig.ls",
        tableName = "config.hardwareConfig",
    },
    RemainConfig =
    {
        fileName = "RemainConfig.ls",
        tableName = "config.remain",
    },
    RemainStatus =
    {--试剂余量状态表依赖试剂配置表，需要在试剂配置表后加载
        fileName = "RemainStatus.ls",
        tableName = "status.remain",
    },
    ConsumableConfig =
    {
        fileName = "ConsumableConfig.ls",
        tableName = "config.consumable",
    },
    ProducerAuthorization =
    {
        fileName = "ProducerAuthorization.ls",
        tableName = "config.producerAuthorization",
    },
}

local protectionZoneConfigTableInfo =
{
    {info = configFile.InstrumentInfomation},
    {info = configFile.MainBoardInfomation},
}


--配置表信息，包含配置文件的名字，在LUA空间的表名字
local configTableInfo =
{
    {info = configFile.InterconnectionConfig},

    {info = configFile.SchedulerConfig},

    {info = configFile.Authorization},

    {info = configFile.SystemConfig},

    {info = configFile.MeasureParamConfig},

    {info = configFile.MeasureStatus},

    {info = configFile.MeasureRunning},

    {info = configFile.HardwareConfig},

    {info = configFile.RemainConfig},

    {info = configFile.RemainStatus},

    {info = configFile.ConsumableConfig},

    {info = configFile.ProducerAuthorization},
}

--拷贝文件
local function CopyFile(sourcePath, destinationPath, targetName)

    local sourceTable = Serialization.LoadEncryptionFile(sourcePath, targetName, false)
    local path = scriptsPath  .. "/Config/Default/InstrumentInfomation.ls"
    if path == sourcePath then
        sourceTable.uuid = App.CreateUUID()
    end
    Serialization.SaveEncryptionFile(sourceTable, destinationPath, targetName)

    return true
end

--同步表的键值
local function SyncTableKey(sourceTable, destinationTable)

    if type(sourceTable) == "table" and type(destinationTable) == "table" then
        for k,v in pairs(sourceTable) do
            if type(destinationTable[k]) == "nil" then -- 查看destinationTable[k]是否存在,不存在则新建
                destinationTable[k] = v
            elseif type(v) ~= "table" and type(destinationTable[k]) == "table" then
               destinationTable[k] = v
            elseif type(v) == "table" then -- 存在，则查看是否是表，是表则同步表里面的数据
                if type(destinationTable[k]) == "table" then
                    SyncTableKey(v, destinationTable[k])
                else
                    destinationTable[k] = v
                end
            end
        end

        for k,v in pairs(destinationTable) do
            if type(sourceTable[k]) == "nil" then
                destinationTable[k] = nil
            elseif type(v) == "table" then
                SyncTableKey(sourceTable[k], v)
            end
        end
        return true
    else
        return false
    end
end

--同步文件中的表
local function SyncTableOffFile(sourcePath, destinationPath, targetName)

    local sourceTable = Serialization.LoadEncryptionFile(sourcePath, targetName, false)
    local destinationTable = 0
    local logger = Log.Instance():GetLogger()

    local err, result = pcall(
    function()
        return Serialization.LoadEncryptionFile(destinationPath, targetName, false)
    end
    )
    if err then
        destinationTable = result
    else
        logger:error(destinationPath .. " LoadEncryptionFile error")
    end

    if not err  or type(destinationTable) ~= "table" then
        if type(destinationTable) ~= "table" then
            logger:error(destinationPath .. " table is nil")
        end
        CopyFile(sourcePath, destinationPath, targetName)
        destinationTable = Serialization.LoadEncryptionFile(destinationPath, targetName, false)
    end

    SyncTableKey(sourceTable, destinationTable)

    --print(Serialization.Serialize(destinationTable))
    --print("--------------------------------------------------------------------")

    --print(Serialization.Serialize(destinationTable))
    --print("======================================================================")

    Serialization.SaveEncryptionFile(destinationTable, destinationPath, targetName)

    sourceTable = nil
    destinationTable = nil
    return true
end

--配置文件初始化，文件不存在则拷贝默认文件夹中的文件，存在则与默认文件夹的文件进行同步
local function SettingDataInit(defaultDir, dataDir, targetName)

    local ret = true
    local file = io.open(dataDir)
    if not file then
        --print(dataDir .. " not exits")
        if not CopyFile(defaultDir, dataDir, targetName) then
            ret = false
        end
        local logger = Log.Instance():GetLogger()
        local str = "生成默认配置文件：" .. dataDir
        logger:debug(str)
    else
        --print(dataDir .. " exits")

        file:close()
        if not SyncTableOffFile(defaultDir, dataDir, targetName) then
            ret = false
        end
    end
    return ret
end

--初始化，脚本所在路径，保护区路径
function Init(scriptsDir, protectionZoneDir)

    if scriptsDir then
        scriptsPath = scriptsDir
        defaultPath = scriptsDir .. "/Config/Default"
        dataPath = scriptsDir .. "/Config"
    end

    if protectionZoneDir then
        protectionZonePath = protectionZoneDir
    end

    --print("---------------------------------")
    --print(defaultPath)
    --print(dataPath)
    --print(protectionZonePath)
    --print("---------------------------------")

    for _,v in ipairs(protectionZoneConfigTableInfo) do
        if not SettingDataInit(defaultPath .. "/" ..  v.info.fileName, protectionZonePath  .. "/" ..  v.info.fileName, v.info.tableName) then
            return false
        end

        Serialization.LoadEncryptionFile(protectionZonePath  .. "/" ..  v.info.fileName, v.info.tableName, true)
    end

    for _,v in ipairs(configTableInfo) do
        if v.info.fileName ~= "ProducerAuthorization.ls" and not SettingDataInit(defaultPath .. "/" ..  v.info.fileName, dataPath  .. "/" ..  v.info.fileName, v.info.tableName) then
            return false
        end

        pcall(
        function()
            Serialization.LoadEncryptionFile(dataPath  .. "/" ..  v.info.fileName, v.info.tableName, true)
        end
        )

    end

    --print("config===============================================================")
    --print(Serialization.Serialize(config))
    --print("setting==============================================================")
    --print(Serialization.Serialize(setting))

    return true
end

-- 加载InterconnectionConfig.ls
function LoadInterconnectionConfig(isDefaultFile)
    if isDefaultFile == true then
        return Serialization.LoadEncryptionFile(defaultPath .. "/" .. configFile.InterconnectionConfig.fileName,
        configFile.InterconnectionConfig.tableName,
        false)
    else
        return Serialization.LoadEncryptionFile(dataPath .. "/" .. configFile.InterconnectionConfig.fileName,
        configFile.InterconnectionConfig.tableName,
        false)
    end
end

-- 保存InterconnectionConfig.ls
function SaveInterconnectionConfig()
    Serialization.SaveEncryptionFile(_G.config.interconnection,
    dataPath .. "/" .. configFile.InterconnectionConfig.fileName,
    configFile.InterconnectionConfig.tableName)
end

-- 加载SchedulerConfig.ls
function LoadSchedulerConfig(isDefaultFile)
    if isDefaultFile == true then
        return Serialization.LoadEncryptionFile(defaultPath .. "/" .. configFile.SchedulerConfig.fileName,
        configFile.SchedulerConfig.tableName,
        false)
    else
        return Serialization.LoadEncryptionFile(dataPath .. "/" .. configFile.SchedulerConfig.fileName,
        configFile.SchedulerConfig.tableName,
        false)

    end
end

-- 保存SchedulerConfig.ls
function SaveSchedulerConfig()
    Serialization.SaveEncryptionFile(_G.config.scheduler,
    dataPath .. "/" .. configFile.SchedulerConfig.fileName,
    configFile.SchedulerConfig.tableName)
end

-- 加载Authorization.ls
function LoadAuthorization(isDefaultFile)
    if isDefaultFile == true then
        return Serialization.LoadEncryptionFile(defaultPath .. "/" .. configFile.Authorization.fileName,
        configFile.Authorization.tableName,
        false)
    else
        return Serialization.LoadEncryptionFile(dataPath .. "/" .. configFile.Authorization.fileName,
        configFile.Authorization.tableName,
        false)
    end
end

-- 保存Authorization.ls 
function SaveAuthorization()
    Serialization.SaveEncryptionFile(_G.config.authorization,
    dataPath .. "/" .. configFile.Authorization.fileName,
    configFile.Authorization.tableName)
end

-- 加载SystemConfig.ls
function LoadSystemConfig(isDefaultFile)
    if isDefaultFile == true then
        return Serialization.LoadEncryptionFile(defaultPath .. "/" .. configFile.SystemConfig.fileName,
        configFile.SystemConfig.tableName,
        false)
    else
        return Serialization.LoadEncryptionFile(dataPath .. "/" .. configFile.SystemConfig.fileName,
        configFile.SystemConfig.tableName,
        false)
    end
end

-- 保存SystemConfig.ls 
function SaveSystemConfig()
    Serialization.SaveEncryptionFile(_G.config.system,
    dataPath .. "/" .. configFile.SystemConfig.fileName,
    configFile.SystemConfig.tableName)
end

-- 加载MeasureParamConfig.ls
function LoadMeasureParamConfig(isDefaultFile)
    if isDefaultFile == true then
        return Serialization.LoadEncryptionFile(defaultPath .. "/" .. configFile.MeasureParamConfig.fileName,
        configFile.MeasureParamConfig.tableName,
        false)
    else
        return Serialization.LoadEncryptionFile(dataPath .. "/" .. configFile.MeasureParamConfig.fileName,
        configFile.MeasureParamConfig.tableName,
        false)
    end
end

-- 保存MeasureParamConfig.ls
function SaveMeasureParamConfig()
    Serialization.SaveEncryptionFile(_G.config.measureParam,
    dataPath .. "/" .. configFile.MeasureParamConfig.fileName,
    configFile.MeasureParamConfig.tableName)

end

-- 加载MeasureStatus.ls
function LoadMeasureStatus(isDefaultFile)
    if isDefaultFile == true then
        return Serialization.LoadEncryptionFile(defaultPath .. "/" .. configFile.MeasureStatus.fileName,
        configFile.MeasureStatus.tableName,
        false)
    else
        return Serialization.LoadEncryptionFile(dataPath .. "/" .. configFile.MeasureStatus.fileName,
        configFile.MeasureStatus.tableName,
        false)
    end
end

-- 保存MeasureStatus.ls
function SaveMeasureStatus()
    Serialization.SaveEncryptionFile(_G.status.measure,
    dataPath .. "/" .. configFile.MeasureStatus.fileName,
    configFile.MeasureStatus.tableName)
end

-- 加载MeasureRunning.ls
function LoadMeasureRunning(isDefaultFile)
    if isDefaultFile == true then
        return Serialization.LoadEncryptionFile(defaultPath .. "/" .. configFile.MeasureRunning.fileName,
        configFile.MeasureRunning.tableName,
        false)
    else
        return Serialization.LoadEncryptionFile(dataPath .. "/" .. configFile.MeasureRunning.fileName,
        configFile.MeasureRunning.tableName,
        false)
    end
end

-- 保存MeasureRunning.ls
function SaveMeasureRunning()
    Serialization.SaveEncryptionFile(_G.status.running,
    dataPath .. "/" .. configFile.MeasureRunning.fileName,
    configFile.MeasureRunning.tableName)
end

-- 加载HardwareConfig.ls
function LoadHardwareConfig(isDefaultFile)
    if isDefaultFile == true then
        return Serialization.LoadEncryptionFile(defaultPath .. "/" .. configFile.HardwareConfig.fileName,
        configFile.HardwareConfig.tableName,
        false)
    else
        return Serialization.LoadEncryptionFile(dataPath .. "/" .. configFile.HardwareConfig.fileName,
        configFile.HardwareConfig.tableName,
        false)
    end
end

-- 保存HardwareConfig.ls
function SaveHardwareConfig()
    Serialization.SaveEncryptionFile(_G.config.hardwareConfig,
    dataPath .. "/" .. configFile.HardwareConfig.fileName,
    configFile.HardwareConfig.tableName)
end

-- 加载RemainConfig.ls
function LoadRemainConfig(isDefaultFile)
    if isDefaultFile == true then
        return Serialization.LoadEncryptionFile(defaultPath .. "/" .. configFile.RemainConfig.fileName,
        configFile.RemainConfig.tableName,
        false)
    else
        return Serialization.LoadEncryptionFile(dataPath .. "/" .. configFile.RemainConfig.fileName,
        configFile.RemainConfig.tableName,
        false)
    end
end

-- 保存RemainConfig.ls
function SaveRemainConfig()
    Serialization.SaveEncryptionFile(_G.config.remain,
    dataPath .. "/" .. configFile.RemainConfig.fileName,
    configFile.RemainConfig.tableName)
end

-- 加载RemainStatus.ls
function LoadRemainStatus(isDefaultFile)
    if isDefaultFile == true then
        return Serialization.LoadEncryptionFile(defaultPath .. "/" .. configFile.RemainStatus.fileName,
        configFile.RemainStatus.tableName,
        false)
    else
        return Serialization.LoadEncryptionFile(dataPath .. "/" .. configFile.RemainStatus.fileName,
        configFile.RemainStatus.tableName,
        false)
    end
end

-- 保存RemainStatus.ls
function SaveRemainStatus()
    Serialization.SaveEncryptionFile(_G.status.remain,
    dataPath .. "/" .. configFile.RemainStatus.fileName,
    configFile.RemainStatus.tableName)
end

-- 加载ConsumableConfig.ls
function LoadConsumableConfig(isDefaultFile)
    if isDefaultFile == true then
        return Serialization.LoadEncryptionFile(defaultPath .. "/" .. configFile.ConsumableConfig.fileName,
        configFile.ConsumableConfig.tableName,
        false)
    else
        return Serialization.LoadEncryptionFile(dataPath .. "/" .. configFile.ConsumableConfig.fileName,
        configFile.ConsumableConfig.tableName,
        false)
    end
end

-- 保存ConsumableConfig.ls
function SaveConsumableConfig()
    Serialization.SaveEncryptionFile(_G.config.consumable,
    dataPath .. "/" .. configFile.ConsumableConfig.fileName,
    configFile.ConsumableConfig.tableName)
end

-- 加载ProducerAuthorization.ls
function LoadProducerAuthorization(isDefaultFile)
    if isDefaultFile == true then
        return Serialization.LoadEncryptionFile(defaultPath .. "/" .. configFile.ProducerAuthorization.fileName,
        configFile.ProducerAuthorization.tableName,
        false)
    else
        return Serialization.LoadEncryptionFile(dataPath .. "/" .. configFile.ProducerAuthorization.fileName,
        configFile.ProducerAuthorization.tableName,
        false)
    end
end

-- 保存ProducerAuthorization.ls
function SaveProducerAuthorization()
    Serialization.SaveEncryptionFile(_G.config.producerAuthorization,
    dataPath .. "/" .. configFile.ProducerAuthorization.fileName,
    configFile.ProducerAuthorization.tableName)
end

return ConfigLists
