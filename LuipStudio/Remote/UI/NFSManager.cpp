#include "NFSManager.h"
#include "VncPlugin/VncManager.h"
#include "UdpPlugin/UdpManager.h"

NFSManager::NFSManager()
{
    //导出目录
#ifdef _WIN32
    m_exportPath = Treasure::Path::GetAbsPath("../NFSData/");
#else
    m_exportPath = Treasure::Path::GetAbsPath("/mnt/userdata2/NFSData/");
#endif
    Treasure::Path::Mkdirs(m_exportPath);
}

QString NFSManager::GetExportPath()
{
    return m_exportPath;
}

void NFSManager::Check()
{
#ifdef _WIN32
    QString configPath = "./nfsd/exports";
    QString exePath    = "./nfsd/nfssrv.exe";
    bool configChange = false;

    //检查配置文件是否正确
    QString content = m_exportPath.replace("/", "\\") + " -name:nfs -alldirs -exec -maproot:0";
    QString configText = Treasure::File::ReadAll(configPath);
    if (content != configText)
    {
        configChange = true;
        Treasure::File::WriteAll(configPath, content);
    }

    //检查NFS是否有启动
    HWND nfsPro = ::FindWindowA(NULL, "haneWIN NFS Server");
    if (nfsPro && !configChange)
        return;

    //重启
    QProcess p;
    p.execute("taskkill /im nfssrv.exe /f");
    p.close();

//    QProcess::startDetached(Treasure::Path::GetAbsPath(exePath).toStdString().c_str());
//    const wchar_t *str1=L"open";
//    wchar_t str2[256];
//    Treasure::Path::GetAbsPath(exePath).toWCharArray(str2);
//    ShellExecute(NULL,str1,str2,NULL,NULL,0);
    WinExec(Treasure::Path::GetAbsPath(exePath).toStdString().c_str(), SW_SHOWMINIMIZED);

#else
    QString configPath = "/etc/exports";
    QString exePath    = "/etc/init.d/S60nfs";

    if (!Treasure::File::Exist(exePath))
    {
        trLogger->warn("NFSManager::Check not support nfs server!!!");
        return;
    }

    //检查 "/nfs" 链接是否存在
    if (!Treasure::Path::Exist("/nfs"))
    {
        trLogger->debug(QString("NFSManager::Check ln /nfs to %1").arg(m_exportPath));
        if (!Treasure::Path::Link(m_exportPath, "/nfs"))
        {
            trLogger->debug(QString("NFSManager::Check ln /nfs to %1 failed.").arg(m_exportPath));
            return;
        }
    }

    //检查配置文件是否正确
    QString content = m_exportPath.replace("\\", "/") + " *(rw,fsid=0,no_subtree_check,no_root_squash)";
    QString configText = Treasure::File::ReadAll(configPath);
    if (content != configText)
    {
        trLogger->debug(QString("NFSManager::Check rewrite %1").arg(configPath));
        Treasure::File::WriteAll(configPath, content);
        QProcess::startDetached(exePath + " reload");
    }

    //检查进程 "rpc.mountd"
    if (!Treasure::Process::IsRunning("rpc.mountd"))
    {
        trLogger->debug(QString("NFSManager::Check rpc.mountd not exist, restart nfs!"));
        QProcess::startDetached(exePath + " restart");
    }
#endif
}
