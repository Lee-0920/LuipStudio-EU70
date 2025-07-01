#include <QString>
#include <QThread>
#include "Log.h"
#include "ProgramFileParser.h"
#include "ControllerPlugin/API/DeviceStatusInterface.h"
#include "ControllerPlugin/API/Code/DeviceUpdateInterface.h"
#include "DeviceUpdator.h"
#include "Communication/CommunicationException.h"
#include "AlarmManager/AlarmManager.h"
#include "DeviceUpdator.h"
#include "System/MessageDigest.h"
#include "Communication/eCek/DNCP/App/DscpSysDefine.h"
#include <QDateTime>
#include "System/Uncompress.h"
#include <QFile>
#include "Setting/Environment.h"
#include <QFileInfo>

using namespace Measure;
using namespace Controller::API;
using namespace Communication;

namespace Updater
{

DeviceUpdater::DeviceUpdater(DscpAddress addr, String fileName, IUpdateProgress *progress, bool isDeleteZip)
{
    m_updateInterface = new DeviceUpdateInterface(addr);
    m_fileName =  fileName;
    m_progress = progress;
    m_isDeleteZip = isDeleteZip;
}

DeviceUpdater::~DeviceUpdater()
{
    if (m_updateInterface)
        delete m_updateInterface;
}

String DeviceUpdater::GetFileName(void)
{
   return m_fileName;
}

String DeviceUpdater::GetUnzipDestDirectory(void)
{
    return m_destDirectory;
}

bool DeviceUpdater::Update(void)
{
    bool ret = FALSE;

    QFileInfo original(m_fileName.c_str());
    QString UpdaterFilePath;

    if (original.suffix() != "hex")
    {
        QString destName = original.completeBaseName();
        QString destDirectory = Configuration::Environment::Instance()->GetUnzipPath().c_str();
        destDirectory += "/" + destName;
        destDirectory += "/";
        m_destDirectory = destDirectory.toStdString();
        QStringList listPic;
        if (false == Uncompress::UnZipToDirectory(m_fileName, m_destDirectory, listPic, "LuipAdmin@2017"))
        {
            logger->debug("升级包: %s 解压失败", m_fileName.c_str());
            if (m_isDeleteZip)
            {
                QFile::remove(m_fileName.c_str());
            }
            return false;
        }
        if (m_isDeleteZip)
        {
            QFile::remove(m_fileName.c_str());
        }

        QStringList resultList= listPic.filter(".hex");

        if (resultList.size() > 0)
        {
            UpdaterFilePath = resultList.at(0);
        }
    }
    else
    {
        UpdaterFilePath = m_fileName.c_str();
        m_destDirectory = "";
    }

    vector<ProgramSegment*>  segments;
    unsigned char *data;
    ProgramFileParser updateFileParse(UpdaterFilePath.toStdString());
    String UpgradeModeS;//升级文件的所升级模式
    String UpgradeTypeS;
    RunMode SubControllerMustMode, SubControllerCurMode;//子控板必须运行的模式，和子控器当前模式
    unsigned short  maxSegmentSize = 0;
    unsigned short  segmentTxtimes = 0,lastsegmentTxLen = 0 , i = 0;
    unsigned short chksum = 0;
    float EraseTimes, writeProgramTimes;
    WriteProgramResult result;
    unsigned short seqErrorRetry = 0;

    if(TRUE == updateFileParse.ParseHeader())
    {
        if (TRUE == updateFileParse.ParseProgram())
        {
            updateFileParse.GetVersion();
            UpgradeTypeS = updateFileParse.GetType();
            //获取升级文件的所升级模式
            UpgradeModeS = updateFileParse.GetMode();
            if ("application" == UpgradeModeS)
            {
                SubControllerMustMode = RunMode::Updater;
                EraseTimes = 60;
            }
            else if ("updater" == UpgradeModeS)
            {
                SubControllerMustMode =  RunMode::Application;
                EraseTimes = 30;
            }
            segments = updateFileParse.GetSegmentData();
            if(1 == segments.size())//只支持单片数据段的升级文件升级
            {
                data = segments.at(0)->GetData();

                try
                {
                    //注意：下位机的Boot和APP 的maxSegmentSize需要一样，因为现在获取到的
                    //maxSegmentSize是当前模式下的，不管是Boot还是APP。
                    maxSegmentSize = m_updateInterface->GetMaxFragmentSize();
                }
                catch(CommandTimeoutException e)
                {
                    AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
                    return false;
                }
                catch (std::exception e)
                {
                    logger->warn("查询 Updater 支持的最大分片大小std异常");
                    AlarmManager::Instance()->AddExceptionAlarm(e);
                    return false;
                }
                if (maxSegmentSize < 256)
                {
                    segmentTxtimes =  segments.at(0)->GetLength() / maxSegmentSize;
                    lastsegmentTxLen = segments.at(0)->GetLength() % maxSegmentSize;
                    writeProgramTimes = (segmentTxtimes + 1) * 0.055;
                    m_progress->OnUpdaterTotalTimes(UpgradeTypeS, writeProgramTimes + EraseTimes + 2);
                    try
                    {
                        //获取子控板当前运行的模式
                        SubControllerCurMode = m_updateInterface->GetRunMode();
                        if(SubControllerCurMode == RunMode::Upgrader)
                        {
                            SubControllerCurMode = RunMode::Application;
                        }
                    }
                    catch(CommandTimeoutException e)
                    {
                        AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
                        return false;
                    }
                    catch (std::exception e)
                    {
                        logger->warn("获取子控板当前运行的模式std异常");
                        AlarmManager::Instance()->AddExceptionAlarm(e);
                        return false;
                    }

                    if(SubControllerMustMode != SubControllerCurMode)
                    {
                        //让子控板进入对应的模式
                        if (RunMode::Updater == SubControllerMustMode)
                        {
                            m_updateInterface->EnterUpdater();
                        }
                        else
                        {
                            m_updateInterface->EnterApplication();
                        }
                        QThread::sleep(2);//等待子控板进入对应得状态
                        try
                        {
                         //检查运行模式，是否进入必要的模式
                            SubControllerCurMode = m_updateInterface->GetRunMode();
                        }
                        catch(CommandTimeoutException e)
                        {
                            AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
                            return false;
                        }
                        catch (std::exception e)
                        {
                            logger->warn("查询程序当前的运行模式std异常");
                            AlarmManager::Instance()->AddExceptionAlarm(e);
                            return false;
                        }
                    }
                    if(SubControllerMustMode == SubControllerCurMode)
                    {  
                        m_progress->OnProgressed("Erase", EraseTimes);
                        try
                        {
                             if(true == m_updateInterface->Erase())
                             {
                                 EraseResult result;
                                 result = m_updateInterface->ExpectErase(EraseTimes * 1000);
                                 if (result != ERASE_RESULT_FINISHED)
                                 {
                                     logger->warn("擦除校验错误.");
                                     return false;
                                 }
                             }
                             else
                             {
                                logger->warn("擦除失败.");
                                return false;
                             }
                        }
                        catch(CommandTimeoutException e)
                        {
                            AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
                            return false;
                        }
                        catch(ExpectEventTimeoutException e)
                        {
                            (void)e;
                            logger->warn("无擦除事件回应.");
                        }
                        catch (std::exception e)
                        {
                            logger->warn("擦除std异常");
                            AlarmManager::Instance()->AddExceptionAlarm(e);
                            return false;
                        }
                        //向设备写程序数据。
                        m_progress->OnProgressed("WriteProgram", writeProgramTimes);
                        for(i = 0; i < segmentTxtimes; )
                        {
                            chksum = MessageDigest_Crc16Ccitt(chksum, data + i * maxSegmentSize, maxSegmentSize);

                            try
                            {
                                result = m_updateInterface->WriteProgram(maxSegmentSize, data + i * maxSegmentSize, i);
                            }
                            catch(CommandTimeoutException e)
                            {
                                AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
                                logger->warn("向设备写程序片%d数据命令超时", i);
                                ret = FALSE;
                                result.status = DSCP_TIMEOUT;
                                break;

                            }
                            catch (std::exception e)
                            {
                                logger->warn("升级时std异常");
                                AlarmManager::Instance()->AddExceptionAlarm(e);
                                ret = FALSE;
                                result.status = DSCP_TIMEOUT;
                                break;
                            }

                            if ( DSCP_OK == result.status)
                            {
                                i++;
                                seqErrorRetry = 0;
                            }
                            else if(DUI_WRITE_SEQ_ERROR  == result.status)
                            {
                                seqErrorRetry++;
                                logger->warn("向设备写程序片%d数据片错误。下位机期待写片%d数据。该片第%d次错误。", i, result.seq, seqErrorRetry);
                                i = result.seq;
                                if(seqErrorRetry >= 5)
                                {
                                    ret = FALSE;
                                    break;
                                }
                            }
                            else
                            {
                                logger->warn("向设备写程序片%d数据片错误。", i);
                                ret = FALSE;
                                break;
                            }
                        }
                        if (DSCP_OK == result.status && lastsegmentTxLen != 0)
                        {
                            chksum = MessageDigest_Crc16Ccitt(chksum, data + i * maxSegmentSize, lastsegmentTxLen);
                            try
                            {
                                result = m_updateInterface->WriteProgram(lastsegmentTxLen, data + i * maxSegmentSize, i);
                            }
                            catch(CommandTimeoutException e)
                            {
                                AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
                                logger->warn("向设备写程序片%d数据命令超时", i);
                                ret = FALSE;
                                result.status = DSCP_TIMEOUT;
                            }
                            catch (std::exception e)
                            {
                                logger->warn("升级时std异常");
                                AlarmManager::Instance()->AddExceptionAlarm(e);
                                ret = FALSE;
                                result.status = DSCP_TIMEOUT;
                            }
                            if(DUI_WRITE_SEQ_ERROR  == result.status)
                            {
                                result.status = DSCP_OK;
                            }
                        }
                        if (DSCP_OK == result.status)
                        {
                            try
                            {
//                                qDebug("chksum:%d", chksum);
                                ret = m_updateInterface->CheckIntegrity(chksum);
                            }
                            catch(CommandTimeoutException e)
                            {
                                AlarmManager::Instance()->AddCommunicationExceptionAlarm(e);
                                logger->warn("程序完整性检验命令超时");
                                ret = FALSE;
                            }
                            catch (std::exception e)
                            {
                                logger->warn("程序完整性检验std异常");
                                AlarmManager::Instance()->AddExceptionAlarm(e);
                                ret = FALSE;
                            }
                            if(TRUE == ret)
                            {
                                if ("updater" == UpgradeModeS)//如果升级的升级程序，则试运行下升级程序
                                {
                                    m_updateInterface->EnterUpdater();
                                    QThread::sleep(2);//等待子控板进入对应得状态
                                }
                                m_updateInterface->EnterApplication();
//                                m_progress->OnUpdaterTotalTimes("Idle", 0);
//                                m_progress->OnProgressed("Idle", 0);
                            }
                            else
                            {
                                logger->warn("程序完整性检验错误");
                            }

                        }
                        else
                        {
                           logger->warn("向设备写程序数据失败");
                        }
                    }
                    else
                    {
                        logger->warn("需升级板卡运行模式错误,当前模式为 %d.", SubControllerCurMode);
                    }
                }
                else
                {
                    logger->warn("下位机支持的最大分片大于等于256,当前分片大小为 %d .", maxSegmentSize);
                }
            }
            else
            {
               logger->warn("只支持升级文件数据段为1的文件, 当前数据段个数 %d.", segments.size());
            }
        }
        else
        {
            logger->warn("升级文件 %s 打开失败.", UpdaterFilePath.toLatin1().data());
        }
    }
    else
    {
        logger->warn("升级文件 %s 头解析失败.", UpdaterFilePath.toLatin1().data());
    }
    return ret;
}


}

