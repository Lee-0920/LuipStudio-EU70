/**
 * @brief APP相关操作
 * @author huangyue
 * @date 2021-03-24
 * @note
 */

#ifndef __TREASURE_SYSTEM_APP__
#define __TREASURE_SYSTEM_APP__

#include <QDir>
#include <QFont>
#include <QScreen>
#include <QLockFile>
#include <QTextCodec>
#include <QApplication>
#include <QStyleFactory>
#include <QDesktopWidget>

namespace Treasure
{

class App : public QApplication
{
public:
    App(int &argc, char **argv) : QApplication(argc, argv)
    {
    }

    //@brief 设置QSS样式文件
    bool SetQss(QString filePath)
    {
        QFile qss(filePath);
        if (!qss.open(QFile::ReadOnly))
            return false;

        qApp->setStyleSheet(qss.readAll());
        qss.close();
        return true;
    }

    //@brief 设置窗口样式
    void SetStyle(QString style)
    {
        QApplication::setStyle(QStyleFactory::create(style));
    }

    //@brief 设置文本编码
    void SetTextCodec(QString code = "UTF-8")
    {
        QTextCodec::setCodecForLocale(QTextCodec::codecForName(code.toStdString().c_str()));
    }

    //@brief 设置字体
#ifdef _WIN32
    void SetFont(QString name, int size = 12)
#else
    void SetFont(QString name, int size = 8)
#endif
    {
        QFont font(name, size);
        this->setFont(font);
    }

    /**
     * @brief 获取main参
     * @param index 第几个
     * @param sdefault 找不到时返回的默认值
     */
    QString GetArgv(int index, QString sdefault = "")
    {
        QStringList array = this->arguments();
        if (index >= array.size())
            return sdefault;

        return array[index];
    }

    /**
     * @brief 不显示鼠标
     */
    void RemoveCursor()
    {
        this->setOverrideCursor(Qt::BlankCursor);
    }

    static QPixmap GetDesktopScreen()
    {
        QScreen* screen = QGuiApplication::primaryScreen();
        QDesktopWidget* desktop = QApplication::desktop();
        QRect rect = desktop->screenGeometry();
        QPixmap mFullScreen = screen->grabWindow(desktop->winId(), 0, 0, rect.width(), rect.height());
        return mFullScreen;
    }
};



/**
 * @brief 单例APP
 * @note 用于判断当前是否已经有此APP存在
 */
class SignalApp : public App
{
public:
    SignalApp(int &argc, char **argv, QString appId) : App(argc, argv)
    {
        //创建临时文件
        QString path = QDir::temp().absoluteFilePath(appId + ".signallock");
        m_lockFile = new QLockFile(path);

        //判断程序是否已经打开过
        m_isRunning = !m_lockFile->tryLock();
    }

    ~SignalApp()
    {
        if (m_lockFile->isLocked())
        {
            m_lockFile->unlock();
            delete  m_lockFile;
        }
    }

    /**
     * @brief 是否已经有启动的程序
     */
    bool IsRunning() const
    {
        return m_isRunning;
    }



private:
    QLockFile* m_lockFile;
    bool       m_isRunning;
};


}

#endif
