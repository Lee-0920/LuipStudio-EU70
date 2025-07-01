/**
 * @brief 程序启动前的屏保
 * @param name desc
 * @return
 */
#ifndef __TREASURE_SYSTEM_SPLASHSCREEN__
#define __TREASURE_SYSTEM_SPLASHSCREEN__

#include <QSplashScreen>
#include <QMovie>
#include <QLabel>

class QPixmap;

namespace Treasure
{

class SplashScreen : public QSplashScreen
{
public:
    SplashScreen(QSize size)
    {
        m_size  = size;
        m_moive = NULL;
        m_label = NULL;
        setDisabled(true);
    }

    ~SplashScreen()
    {
        if (m_moive)
            delete m_moive;
        if (m_label)
            delete m_label;
    }

    void SetGif(QString filePath)
    {
        SetJpg(filePath);

        m_moive = new QMovie(filePath);
        m_moive->setScaledSize(m_size);

        m_label = new QLabel(this);
        m_label->setMovie(m_moive);
        m_label->setFixedSize(m_size);
        m_moive->start();
    }

    void SetJpg(QString filePath)
    {
        QPixmap pixmap(filePath);
        pixmap = pixmap.scaled(m_size);

        this->setPixmap(pixmap);
    }

private:
    QMovie *m_moive;
    QLabel *m_label;
    QSize  m_size;
};
}

#endif
