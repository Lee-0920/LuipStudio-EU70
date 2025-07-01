#ifndef __TREASURE_TOOL_NFSCLIENT_H__
#define __TREASURE_TOOL_NFSCLIENT_H__

#ifndef _WIN32
#include <sys/mount.h>
#endif

#include "../SystemDef.h"
#include <QProcess>

namespace Treasure
{

class NFSClient
{
private:
    enum class Status
    {
        Connect,
        DisConnect,
        Check,
        Sleep,
    };

public:
    NFSClient()
    {
        m_isConnected = false;
        m_thread = NULL;
        m_status = Status::Sleep;
        m_refreshFunc = NULL;
        m_mountFailedNum = 0;
    }

    ~NFSClient()
    {
        Uninit();
    }

    void Init()
    {
        m_thread = Treasure::ThreadPtr(new Treasure::Thread());
        m_thread->Start(ThreadFunc, this);
    }

    void Uninit()
    {
        if (m_thread)
        {
            m_thread->RequestInterruption();
            m_thread->Wait();
        }
    }

    bool IsConnected()
    {
        return m_isConnected;
    }

    void Connect(QString remoteIP, QString remotePath, QString localPath)
    {
        if (remoteIP != m_remoteIP ||
            remotePath != m_remotePath ||
            localPath != m_localPath)
        {
            m_remoteIP = remoteIP;
            m_remotePath = remotePath;
            m_localPath = localPath;
            SetStatus(Status::Connect);
        }
    }

    void Disconnect()
    {
        SetStatus(Status::DisConnect);
    }

    /**
     * @brief 设置更新参数接口
     * @note 检查挂载时，调用接口，查看参数是否有修改，不用外部调用Connect重设参数
     */
    void SetRefreshParamsFunc(std::function<void(QString& remoteIP, QString& remotePath, QString& localPath, bool& connect)> func)
    {
        QMutexLocker lock(&m_funcMutex);
        m_refreshFunc = func;
    }

private:
    void ThreadSleep(int count)
    {
        while (!m_thread->IsRequestInterruption())
        {
            QThread::msleep(1000);
            if (--count <= 0)
                break;
        }
    }

    static void ThreadFunc(NFSClient* handle)
    {
        QThread::msleep(100);
        trLogger->debug("NFSClient thread start.");
        while(!handle->m_thread->IsRequestInterruption())
        {
            auto status = handle->GetStatus();
            switch(status)
            {
            case Status::Sleep:
                handle->ThreadSleep(10);

                {
                    QMutexLocker lock(&handle->m_funcMutex);
                    if (handle->m_refreshFunc)
                    {
                        bool connect = true;
                        handle->m_refreshFunc(handle->m_remoteIP, handle->m_remotePath, handle->m_localPath, connect);
                        if (connect)
                        {
                            trLogger->debug("NFSClinet mount[%s] refresh connect.", handle->m_remoteIP.toStdString().c_str());
                            handle->SetStatusCheckBefore(Status::Sleep, Status::Connect);
                        }
                    }
                }

                break;
            case Status::Connect:
                handle->UnMount();
                if (handle->Mount())
                    handle->SetStatusCheckBefore(Status::Connect, Status::Check);
                else
                    handle->ThreadSleep(5);
                break;
            case Status::DisConnect:
                handle->UnMount();
                handle->SetStatusCheckBefore(Status::DisConnect, Status::Sleep);
                break;
            case Status::Check:
                if (!handle->CheckMount())
                    handle->SetStatusCheckBefore(Status::Check, Status::Connect);
                else
                    handle->ThreadSleep(10);
                break;
            }
        }
        trLogger->debug("NFSClient thread end.");
    }

    Status GetStatus()
    {
        QMutexLocker lock(&m_statusMutex);
        return m_status;
    }

    void SetStatus(Status status)
    {
        QMutexLocker lock(&m_statusMutex);
        m_status = status;
        //qDebug("SetStatus:%d", status);
    }

    void SetStatusCheckBefore(Status checkStatus, Status status)
    {
        QMutexLocker lock(&m_statusMutex);
        if (m_status == checkStatus)
        {
            m_status = status;
            //qDebug("SetStatus:%d", status);
        }
    }

private:
//    #define	EINVAL		22	/* Invalid argument */
//    #define	EACCES		13	/* Permission denied */
//    #define	EPERM		 1	/* Operation not permitted */
//    #define	EBUSY		16	/* Device or resource busy */
//    #define	ENOENT		 2	/* No such file or directory */
    bool Mount()
    {
        {
            QMutexLocker lock(&m_funcMutex);
            if (m_refreshFunc)
            {
                bool connect = true;
                m_refreshFunc(m_remoteIP, m_remotePath, m_localPath, connect);
                if (!connect)
                {
                    trLogger->debug("NFSClinet mount[%s] refresh disconnect.", m_remoteIP.toStdString().c_str());
                    SetStatusCheckBefore(Status::Connect, Status::Sleep);
                    return false;
                }
            }
        }

        QString remotePath = m_remoteIP + ":" + m_remotePath;
        QString localPath = m_localPath;
        QString params = QString("addr=%1,nolock,soft,fg,tcp,timeo=5,retrans=1,rsize=4096,vers=3").arg(m_remoteIP);

        if (!Treasure::Path::Mkdirs(localPath))
        {
            trLogger->debug("NFSClinet mount[%s] failed. path mkdir err(%s)", m_remoteIP.toStdString().c_str(), localPath.toStdString().c_str());
            return false;
        }

#ifdef _WIN32
        return false;
#else
        QTime time;
        time.start();

        int ret = mount(remotePath.toStdString().c_str(),
                        localPath.toStdString().c_str(),
                        "nfs",
                        MS_SYNCHRONOUS,
                        params.toStdString().c_str());

        QString consumTime = QString::number(time.elapsed()/1000.0);

        if (ret != 0)
        {
            if (m_mountFailedNum % 100 == 0)
                trLogger->debug("NFSClinet mount[%s] failed. ret=%d time:%s", m_remoteIP.toStdString().c_str(), errno, consumTime.toStdString().c_str());
            m_mountFailedNum++;
            return false;
        }

        m_mountFailedNum = 0;
        trLogger->debug("NFSClinet mount[%s] success. time:%s", m_remoteIP.toStdString().c_str(), consumTime.toStdString().c_str());
        m_isConnected = true;
        return true;
#endif
    }

    bool CheckMount()
    {
#ifdef _WIN32
        return false;
#else
        static QProcess proc;
        //打印文件系统的主设备号和次设备号 eg: 0:50
        QString shell = "mountpoint -d " + m_localPath;
        proc.start(shell.toStdString().c_str());
        if (proc.waitForFinished(10000))
        {
            QString output = proc.readAll();
            if (output.isEmpty())
            {
                trLogger->debug("NFSClinet check mount: nfs server close.");
                m_isConnected = false;
                return false;
            }
        }
        else
            trLogger->debug("NFSClinet check mount wait finish failed.");

        //检查是否修改参数
        {
            QMutexLocker lock(&m_funcMutex);
            if (m_refreshFunc)
            {
                QString remoteIP;
                QString remotePath;
                QString localPath;
                bool connect = true;
                m_refreshFunc(remoteIP, remotePath, localPath, connect);
                if (remoteIP != m_remoteIP ||
                    remotePath != m_remotePath ||
                    localPath != m_localPath ||
                    !connect)
                {
                    m_remoteIP = remoteIP;
                    m_remotePath = remotePath;
                    m_localPath = localPath;
                    trLogger->debug("NFSClinet check mount: nfs server(enable:%s) params change:[%s:%s] %s",
                                    connect ? "true" :"false",
                                    m_remoteIP.toStdString().c_str(),
                                    m_remotePath.toStdString().c_str(),
                                    m_localPath.toStdString().c_str());
                    return false;
                }
            }
        }

        m_isConnected = true;
        return true;
#endif
    }

    void UnMount()
    {
#ifdef _WIN32
        return;
#else
        m_isConnected = false;
        if (umount2(m_localPath.toStdString().c_str(), MNT_FORCE) != 0)
        {
            //trLogger->debug("NFSClinet unmount failed. error=%d", errno);
            return;
        }
        else
            trLogger->debug("NFSClinet unmount success.");
#endif
    }

private:
    std::atomic_int  m_mountFailedNum;
    std::atomic_bool m_isConnected;
    Status m_status;
    QMutex m_statusMutex;
    Treasure::ThreadPtr m_thread;

    QString m_remoteIP;
    QString m_remotePath;
    QString m_localPath;

    QMutex m_funcMutex;
    std::function<void(QString& remoteIP, QString& remotePath, QString& localPath, bool& connect)> m_refreshFunc;
};

}
#endif

