#include "VncScreen.h"

using namespace VncSpace;

VncScreen::VncScreen(int width, int height)
{
    image = QImage(width, height, QImage::Format_RGB32);
    SetEmpty();
}

/**
 * @brief 清空
 */
void VncScreen::SetEmpty()
{
    image.fill(Qt::lightGray);
}

/**
 * @brief 设置画布大小
 */
void VncScreen::SetSize(int width, int height)
{
    image = QImage(width, height, QImage::Format_RGB32);
}

/**
 * @brief 覆盖一张图
 */
void VncScreen::CoverImage(QImage &pic, int x, int y)
{
    QPainter painter(&image);
    painter.drawImage(x, y, pic);
    painter.end();
}

QImage& VncScreen::GetImage()
{
    return image;
}

/**
 * @brief 拉伸图片
 * @param width  宽
 * @param height 高
 * @param keepRatio 保持比例
 */
QImage VncScreen::GetImage(int width, int height, bool keepRatio)
{
    return image.scaled(width, height, keepRatio ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}
