/**
 * @file
 * @brief 设备升级接口。
 * @details 
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2015/3/7
 */


#include "Communication/Dscp/DscpStatus.h"
#include "Communication/SyncCaller.h"
#include "Communication/EventHandler.h"
#include "DeviceUpdateInterface.h"
#include "Version.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;
using namespace System;

namespace Controller
{
namespace API
{

/**
 * @brief 设备升级接口构造。
 * @param[in] addr 设备地址。
 */
DeviceUpdateInterface::DeviceUpdateInterface(DscpAddress addr)
    : DeviceInterface(addr)
{

}

/**
 * @brief 查询程序当前的运行模式。
 * @details 在执行后继升级命令之前，必须确保当前程序处于 Updater 模式。
 * @return 运行模式，Uint8。请参考 @ref DeviceRunMode 。
 * @note App 模式和 Updater 模式都支持本命令。见： @ref DSCP_CMD_DUI_GET_RUN_MODE ，
 *  注意这两条命令的码值是一致，只是名称不同而已。
 */
RunMode DeviceUpdateInterface::GetRunMode()
{
    RunMode mode = RunMode::Application;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DUI_GET_RUN_MODE));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        mode =  (RunMode)(*resp->data);
    }

    return mode;
}

/**
 * @brief 查询设备的类型。
 * @details 应用时，类型字串应该包含产品类型相关信息。
 * @return 设备的类型，String，以0结尾的ASCII字符串。
 * @note App 模式和 Updater 模式都支持本命令。
 *  App 模式下命令为： @ref DSCP_CMD_DII_GET_TYPE ，注意命令码和定义都是一致的，
 *  仅名称不同而已。
 */
String DeviceUpdateInterface::GetType()
{
    String type;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DUI_GET_TYPE));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        type = (char*)resp->data;
    }

    return type;
}

/**
 * @brief 查询 Updater 的版本号。
 * @details 不同的 Updater 版本，使用的升级参数和调用时序可能不一致，请参考：
 *   @ref sec_DUI_ChangeLog 。
 * @return 版本号，Byte[4]：
 *  - @b major Uint8，主版本号；
 *  - @b minor Uint8，次版本号；
 *  - @b revision Uint8，修订版本号；
 *  - @b build Uint8，编译版本号；
 */
Version DeviceUpdateInterface::GetVersion()
{
    Version ver;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DUI_GET_VERSION));
    SyncCaller syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        ver.SetData(resp->data, 4);
    }

    return ver;
}

/**
 * @brief 查询 Updater 支持的最大分片大小。
 * @details 在调用 @ref DSCP_CMD_DUI_WRITE_PROGRAM 命令进行数据烧写时，
 *  最大的分片大小不能超过设备的限制长度。
 * @return 最大分片大小，Uint16，单位为字节。
 */
int DeviceUpdateInterface::GetMaxFragmentSize()
{
    int fragmentSize = 0;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DUI_GET_MAX_FRAGMENT_SIZE));
    SyncCaller  syncCaller(m_retries);
    DscpRespPtr resp = syncCaller.Send(cmd);
    if (resp)
    {
        fragmentSize = *((Uint16*)resp->data);
    }

    return fragmentSize;
}

/**
 * @brief 进入Updater模式。
 * @details 在 App 模式下调用本命令，程序将跳转到升级模式，等待升级相关的指令。
 * @return 无回应。
 * @note 仅在 App 或 Upgrader 模式下支持本命令。
 */
void DeviceUpdateInterface::EnterUpdater()
{
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DUI_ENTER_UPDATER));
    SyncCaller syncCaller(m_retries);

    syncCaller.SendWithoutRespond(cmd);
}

/**
 * @brief 进入应用程序。
 * @details 在 Updater 模式下调用本命令，程序将跳转到应用程序。
 *  通常在升级未开始前不打算继续升级，或者在升级完成后调用。
 * @return 无回应。
 */
void DeviceUpdateInterface::EnterApplication()
{
    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DUI_ENTER_APPLICATION));
    SyncCaller  syncCaller(m_retries);

    syncCaller.SendWithoutRespond(cmd);
}

/**
 * @brief 擦除程序存储区域。
 * @details 在调用 Write 写数据之前，必须先调用本操作。对于没有擦除的区域，
 *  直接写入程序可能会造成数据混乱。只有擦除成功后，才能继续对该区域进行后续的
 *  @ref DSCP_CMD_DUI_WRITE_PROGRAM 操作。
 *  <p>设备知道自己的可擦写块大小，具体的擦写算法由设备自行决定，而不需要上位机额外干预。
 * @return 状态回应，Uint16，支持的状态有：
 *  - @ref DSCP_OK  操作成功；
 *  - @ref DUI_ERASE_CHECK_ERROR 擦除校验错误，擦除后立即检查，发现该区域所有位不全为1；
 *  - @ref DUI_ERASE_FAILED 擦除操作失败；
 * @note 不能擦除Updater自身所在的区域，否则设备变砖，除非重新烧录。
 */
bool DeviceUpdateInterface::Erase(void)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DUI_ERASE));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

/**
 * @brief 向设备写程序数据。
 * @details 升级管理程序需要对程序文件进行分片，然后针对每一分片，按序调用本操作更新程序。
 *  设备支持的最大分片大小限制参见： @ref DSCP_CMD_DUI_GET_MAX_FRAGMENT_SIZE 命令。
 * @param length Uint16，分片数据的长度，单位为字节
 * @param data[length] Byte[]，分片数据，具体长度取决于length参数。
 * @return 执行状态：
 *  - @b status Uint16，执行状态，支持的状态有：
 *      - @ref DSCP_OK 操作成功；
 *      - @ref DUI_WRITE_CHECK_ERROR 写入校验错误，写完后立即检查，发现与写入的数据不同；
 *      - @ref DUI_WRITE_FAILED 写入失败；
 *      - @ref DUI_WRITE_SIZE_ERROR 错误的数据长度；
 * @note 请确保已先正确调用 @ref DSCP_CMD_DUI_ERASE 命令。
 */
WriteProgramResult DeviceUpdateInterface::WriteProgram(Uint16 length, Byte* data, Uint16 seq)
{
//    Uint16 status = DscpStatus::Error;
    Uint8 point = 0;
    WriteProgramResult result;

    if (length != 0)
    {
        Byte* cmdData = new Byte[length + sizeof(Uint16) * 2];

        memcpy(cmdData, &seq, sizeof(Uint16));
        point += sizeof(seq);

        memcpy(cmdData+point, &length, sizeof(Uint16));
        point += sizeof(length);

        memcpy(cmdData+point,data,length);

        DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DUI_WRITE_PROGRAM, cmdData, length + sizeof(Uint16) * 2));
        if (cmdData)
        {
            delete[] cmdData;
        }
        SyncCaller syncCaller(m_retries);
//        status = syncCaller.SendWithStatus(cmd);
        DscpRespPtr resp = syncCaller.Send(cmd);
        if (resp)
        {
            result = *((WriteProgramResult*)resp->data);
        }
//        qDebug("seq %d status %d and Wish seq %d ", seq,result.status, result.seq);
    }
    return result;
}

/**
 * @brief 程序完整性检验。
 * @details 对整个目标升级程序涉及到的所有数据进行检验，只有校验成功才能跳转到应用程序。
 *  校验失败时，升级管理程序应该且只能从头（块擦除）开始尝试再次升级。
 *  如果校验失败，即使强行重启，设备仍将进入 Updater，等待下一次的升级尝试。
 * @param checksum Uint16，期望的 CRC16-CCITT 校验和数值。
 *  0 表示不需要校验，强行认为写入的程序是完整的，目标设备无需校验。
 * @return 返回状态，操作是否成功。
 */
bool DeviceUpdateInterface::CheckIntegrity(short checksum)
{
    Uint16 status = DscpStatus::Error;

    DscpCmdPtr cmd(new DscpCommand(m_addr, DSCP_CMD_DUI_CHECK_INTEGRITY, &checksum, sizeof(checksum)));
    SyncCaller syncCaller(m_retries);
    status = syncCaller.SendWithStatus(cmd);
    return (status == DscpStatus::OK);
}

EraseResult DeviceUpdateInterface:: ExpectErase(long timeout)
{
    EraseResult result = ERASE_RESULT_FINISHED;


    DscpEventPtr event = this->Expect(DSCP_EVENT_DUI_ERASE_RESULT, timeout);

    if (event != nullptr)
    {
       result = (EraseResult)(*(event->data));
    }

    return result;
}

}
}
