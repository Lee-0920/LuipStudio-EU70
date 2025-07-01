#ifndef NT66CURVEPLUGIN_H
#define NT66CURVEPLUGIN_H

#include "ResultDetailPlugin/ResultDetailPlugin.h"
#include "LuipShare.h"
#include "oolua.h"
#include "Log.h"
#include <memory>
#include "System/Types.h"

namespace  ResultDetail
{

class NT66CurvePlugin: public ResultDetailPlugin
{
public:
    virtual ~NT66CurvePlugin();
    bool Init(log4cpp::Category* log);
    System::String GetVersion();
private:
    const System::String version = System::String("1.0.0.0");
};

}
// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C"
{
    LUIP_SHARE ResultDetail::ResultDetailPlugin *CreatePlugin();
}

#endif // NT66CURVEPLUGIN_H
