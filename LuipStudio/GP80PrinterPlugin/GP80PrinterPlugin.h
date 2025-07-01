#ifndef GP80PRINTERPLUGIN_H
#define GP80PRINTERPLUGIN_H

#include "LuipShare.h"
#include "Log.h"
#include "System/Types.h"
#include <memory>
#include "GP80PrinterPlugin.h"
#include "PrinterPlugin/PrinterPlugin.h"

namespace PrinterSpace
{
class LUIP_SHARE GP80PrinterPlugin : public PrinterPlugin
{
public:
    virtual ~GP80PrinterPlugin();
    static GP80PrinterPlugin* Instance();
    virtual bool Init(log4cpp::Category* log);
    virtual System::String GetVersion();
private:
    static std::unique_ptr<GP80PrinterPlugin> m_instance;
    const System::String version = System::String("1.0.0.0");
};
}


// extern "C" 生成的导出符号没有任何修饰，方便主程序找到它
extern "C"
{
    LUIP_SHARE PrinterSpace::GP80PrinterPlugin *CreatePlugin();
}

#endif // GP80PRINTERPLUGIN_H
