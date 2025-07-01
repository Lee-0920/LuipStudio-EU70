/**
 * @file
 * @brief 设备升级接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#if !defined(CONTROLLER_API_DEVICEUPDATEINTERFACE_H)
#define CONTROLLER_API_DEVICEUPDATEINTERFACE_H

#include "../LuipShare.h"
#include "Code/DeviceUpdateInterface.h"
#include "DeviceInterface.h"
#include "DeviceStatusInterface.h"
#include "Version.h"

namespace Controller
{
namespace API
{

typedef struct
{
    Uint16 status;
    Uint16 seq;
}WriteProgramResult;
/**
 * @brief 设备升级接口。
 * @details 提供了设备程序更新的一序列操作，通过该接口，能为设备在线地更新程序。
 */
class LUIP_SHARE DeviceUpdateInterface : public DeviceInterface
{
public:
    DeviceUpdateInterface(DscpAddress addr);
    // 查询程序当前的运行模式。
    // 
    // 在执行后继升级命令之前，必须确保当前程序处于升级程序的模式的另一个模式。
    RunMode GetRunMode();
    // 查询设备的类型。
    String GetType();
    // 查询 Updater 的版本号。
    // 
    // 不同的 Updater 版本，使用的升级参数和调用时序可能不一致。
    Version GetVersion();
    int GetMaxFragmentSize();
    // 进入Updater模式。
    // 
    // 在 App 模式下调用本命令，程序将跳转到升级模式，等待升级相关的指令。
    void EnterUpdater();
    // 进入应用程序。
    // 
    // 在 Updater 模式下调用本命令，程序将跳转到应用程序。通常在升级未开始前不打算继续升级，或者在升级完成后调用。
    void EnterApplication();
    // 擦除指定的存储区域。
    // 
    // 在调用 Write 写数据之前，必须先调用本操作。对于没有擦除的区域，直接写入程序可能会造成数据混乱。如果目标程序数据有多个不连续的存储区域，则需要多次调用本操作。只有擦除成功后，才能继续对该区域进行后续的 writeProgram() 调用。
    bool Erase(void);
    // 向设备写程序数据。
    // 
    // 升级管理程序需要对程序文件进行分片，然后针对每一分片，按序调用本操作更新程序。
    WriteProgramResult WriteProgram(Uint16 length, Byte* data, Uint16 seq);
    // 程序完整性检验。
    bool CheckIntegrity(short checksum);

    EraseResult ExpectErase(long timeout);
};

}
}

#endif  //CONTROLLER_API_DEVICEUPDATEINTERFACE_H
