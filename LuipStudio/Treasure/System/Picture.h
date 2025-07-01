//#ifndef __TREASURE_SYSTEM_PICTURE_H__
//#define __TREASURE_SYSTEM_PICTURE_H__

//#include <memory>
//#include <QFile>
//#include <QPixmap>

//namespace Treasure
//{

//    class Picture
//    {
//    public:
//        /**
//         * @brief 压缩图片
//         * @param path 图片路径
//         * @param level 压缩倍数
//         * @param outPath 输出文件
//         * @note outPath 为空则覆盖
//         */
//        static bool Compresse(QString path, uint level, QString outPath = "", QString format = "")
//        {
//            if (path.isEmpty() || level <= 1)
//                return false;

//            if (!QFile::exists(path))
//                return false;

//            if (outPath.isEmpty())
//                outPath = path;

//            QPixmap pix(path);
//            int width = pix.width() / level;
//            int height = pix.height() / level;

//            pix = pix.scaled(QSize(width, height), Qt::KeepAspectRatio);
//            return pix.save(outPath, format.isEmpty() ? NULL : format.toStdString().c_str());
//        }

//    };

//}

//#endif
