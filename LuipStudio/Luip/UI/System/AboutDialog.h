#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

namespace UI
{
class PictureDialog : public QDialog
{
    Q_OBJECT
public:
    PictureDialog(QWidget *parent = 0);
    void SetPhoto(QStringList pics);
    virtual void AfterRefreshPic() {}

private:
    void RefreshPic();

protected:
    QPushButton* m_closeBtn;
    QPushButton* m_leftBtn;
    QPushButton* m_rightBtn;
    QPushButton* m_enterPicBtn;

    QLabel* m_imageLabel;

    int m_curIndx;
    QList<QPixmap> m_pixmaps;
};


class AboutDialog : public PictureDialog
{
    Q_OBJECT
public:
    AboutDialog(QString path, QWidget *parent = 0);
    bool HavePic();

private:
    struct PicItem
    {
        QString path;
        QStringList subPath;

        PicItem(QString path, QStringList subPath = {})
        {
            this->path = path;
            this->subPath = subPath;
        }
    };

private:
    virtual void AfterRefreshPic();
    QList<PicItem> GetPicItems(QString path);


private:
    QList<PicItem> m_pics;
    QString m_path;
};
}

#endif // ABOUTDIALOG_H
