#if !defined(SETTING_ENVIRONMENT_H)
#define SETTING_ENVIRONMENT_H


#include <memory>
#include "System/Types.h"

using namespace System;

namespace Configuration
{
/**
 * @brief 环境参数
 */
class Environment
{
public:
    static Environment* Instance();
    /**
     * @brief 初始化环境参数
     */
    void Init();

    void CreateFolder();
    /**
     * @brief 获取应用程序运行路径
     * @return 应用程序运行路径
     */
    String GetAppPath();
    /**
     * @brief 获取设备数据保存路径
     * @return 设备数据保存路径
     */
    String GetDeviceDataPath();
    /**
     * @brief 获取数据库默认存档保存路径
     * @return 设备数据库默认存档
     */
    String GetDataBaseArchivedPath();
    /**
     * @brief 获取数据库默认备份保存路径
     * @return 设备数据库默认备份
     */
    String GetDataBaseBakPath();
    /**
     * @brief 获取默认配置保存路径
     * @return 默认配置保存路径
     */
    String GetDefaultConfigPath();
    /**
     * @brief 获取升级文件所在路径
     * @return 升级文件所在路径
     */
    String GetUpdatePath();
    /**
     * @brief 获取升级文件解压路径
     * @return 升级文件解压路径
     */
    String GetUnzipPath();
    /**
     * @brief 获取应用程序数据路径
     * @return 应用程序数据路径
     */
    String GetAppDataPath();
private:
    String m_appPath; //应用程序运行路径
    static std::unique_ptr<Environment> m_instance;
};
}

#endif // SETTING_ENVIRONMENT_H
