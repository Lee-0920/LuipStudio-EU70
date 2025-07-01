/**
 * @brief Vnc图像控件,用于绘制远程桌面
 * @author huangyue
 * @date 2021-12-03
 * @note
 */

#ifndef VNCSCREEN_H
#define VNCSCREEN_H

#include <memory>
#include <QtWidgets>

namespace VncSpace
{
class VncScreen
{
public:
    VncScreen(int width, int height);

    void SetEmpty();
    void SetSize(int width, int height);
    void CoverImage(QImage& pic, int x, int y);
    QImage  GetImage(int width, int height, bool keepRatio);
    QImage& GetImage();

private:
    QImage image;
};
}
#endif // VNCSCREEN_H
