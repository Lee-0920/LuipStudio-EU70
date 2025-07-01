#include "AboutDialog.h"
#include "QGridLayout"
#include "QFileInfo"

using namespace UI;

QPushButton* CreateButton(QString btnImage, QString btnPressedImage, int size)
{
    QPushButton* btn = new QPushButton();

    QString style = QString("QPushButton{"
                            "border: none;"
                            "image: url(%1);"
                            "color: black;"
                            "background: rgb(216, 235, 247);}"
                            "QPushButton:pressed {"
                            "image: url(%2);"
                            "color: rgb(173, 115, 0);}").arg(btnImage).arg(btnPressedImage);
    btn->setStyleSheet(style);
    btn->setFixedSize(size,size);
    return btn;
}

PictureDialog::PictureDialog(QWidget *parent) : QDialog(parent)
{
#ifdef _CS_X86_WINDOWS
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
#endif

    this->resize(800, 600);
    this->setWindowModality(Qt::WindowModal);

    //设置按钮样式
    m_leftBtn = CreateButton(":/sysButton/img/sysButton/topButton.png", "img/sysButton/topButton_pressed.png", 50);
    m_rightBtn = CreateButton(":/sysButton/img/sysButton/bottomButton.png", ":/sysButton/img/sysButton/bottomButton_pressed.png", 50);
    m_enterPicBtn = CreateButton(":/sysButton/img/sysButton/downPage.png", ":/sysButton/img/sysButton/downPage_pressed.png", 36);
    m_closeBtn = CreateButton(":/sysButton/img/sysButton/close1.png", ":/sysButton/img/sysButton/close1_pressed.png", 36);
    m_leftBtn->hide();
    m_rightBtn->hide();
    m_enterPicBtn->hide();

    m_curIndx = 0;
    m_imageLabel = new QLabel();

    auto layout = new QGridLayout(this);
    layout->addWidget(m_imageLabel);
    layout->addWidget(m_closeBtn, 0, 0, Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(m_leftBtn, 0, 0, Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(m_rightBtn, 0, 0, Qt::AlignRight | Qt::AlignVCenter);
    layout->addWidget(m_enterPicBtn, 0, 0, Qt::AlignRight | Qt::AlignTop);
    layout->setContentsMargins(0, 0, 0, 0);

    connect(m_leftBtn, &QPushButton::clicked, this, [=]{
        m_curIndx--;
        RefreshPic();
    });

    connect(m_rightBtn, &QPushButton::clicked, this, [=]{
        m_curIndx++;
        RefreshPic();
    });

    connect(m_closeBtn, &QPushButton::clicked, this, [=]{ this->accept(); });
}

void PictureDialog::SetPhoto(QStringList photos)
{
    for (auto item: photos)
    {
        QImage img;
        img.load(item);
        m_pixmaps.append(QPixmap::fromImage(img));
    }

    m_curIndx = 0;
    RefreshPic();
}

void PictureDialog::RefreshPic()
{
    if (m_curIndx < 0)
        m_curIndx = m_pixmaps.size() - 1;
    else if (m_curIndx >= m_pixmaps.size())
        m_curIndx = 0;

    if (m_pixmaps.size() > m_curIndx && m_curIndx >= 0)
        m_imageLabel->setPixmap(m_pixmaps[m_curIndx]);

    if (m_curIndx > 0)
        m_leftBtn->show();
    else
        m_leftBtn->hide();

    if (m_curIndx < m_pixmaps.size() - 1)
        m_rightBtn->show();
    else
        m_rightBtn->hide();

    AfterRefreshPic();
}


//遍历图片
//第一层遍历：1.jpg \ 2.jpg \ 3.jpg
//第二层遍历：1.1.jpg \ 1.2.jpg \ 1.3.jpg
QList<AboutDialog::PicItem> AboutDialog::GetPicItems(QString path)
{
    QList<AboutDialog::PicItem> rets;

    for (int i = 0; i < 10; i++)
    {
        QString pic = QString("%1/%2.jpg").arg(path).arg(i + 1);
        QFileInfo fileInfo(pic);
        if (!fileInfo.isFile())
            break;

        QStringList subPics;
        for (int j = 0; j < 10; j++)
        {
            QString pic = QString("%1/%2.%3.jpg").arg(path).arg(i + 1).arg(j + 1);
            QFileInfo fileInfo(pic);
            if (!fileInfo.isFile())
                break;

            subPics.append(pic);
        }

        rets.append(AboutDialog::PicItem(pic, subPics));
    }
    return rets;
}

//查询是否有图片存在
//直接查询第一张图 1.jpg 是否有没有
bool AboutDialog::HavePic()
{
    QString pic = QString("%1/%2.jpg").arg(m_path).arg(1);
    QFileInfo fileInfo(pic);
    return fileInfo.isFile();
}

AboutDialog::AboutDialog(QString path, QWidget *parent) : PictureDialog(parent)
{
    //遍历目录下的图片
    m_path = path;
    m_pics = GetPicItems(m_path);

    //设置第一层界面的图片
    QStringList items;
    for (auto item: m_pics)
        items.append(item.path);
    this->SetPhoto(items);

    //显示进入第二层的按钮
    connect(m_enterPicBtn, &QPushButton::clicked, this, [=]{
        PictureDialog dig(this);
        dig.SetPhoto(m_pics[m_curIndx].subPath);
        dig.exec();
    });
}

void AboutDialog::AfterRefreshPic()
{
    m_enterPicBtn->hide();
    if (m_curIndx >= 0 && m_curIndx < m_pics.size() && m_pics[m_curIndx].subPath.size() > 0)
        m_enterPicBtn->show();
}
