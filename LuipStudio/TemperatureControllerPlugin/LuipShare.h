#ifndef LUIP_SHARE_H
#define LUIP_SHARE_H

#include <QtCore/qglobal.h>

// 定义BIL_SHARE，使用者可以不用再处理符号的导入和导出细节
#ifdef WIN32
    #ifdef BIL_EXPORT
        # define LUIP_SHARE Q_DECL_EXPORT
    #else
        # define LUIP_SHARE Q_DECL_IMPORT
    #endif
#else
    # define LUIP_SHARE
#endif

#endif // LUIP_SHARE_H

