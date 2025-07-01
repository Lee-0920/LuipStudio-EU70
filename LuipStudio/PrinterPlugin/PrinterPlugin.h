#ifndef PRINTERPLUGIN_H
#define PRINTERPLUGIN_H

#include "LuipShare.h"
#include "Log.h"
#include "System/Types.h"
#include <memory>

namespace PrinterSpace
{
class LUIP_SHARE PrinterPlugin
{
public:
    virtual ~PrinterPlugin();
    static PrinterPlugin* Instance();
    virtual bool Init(log4cpp::Category* log);
    virtual System::String GetVersion();
private:
    static std::unique_ptr<PrinterPlugin> m_instance;
    const System::String version = System::String("1.0.0.0");
};
}

#endif // PRINTERPLUGIN_H
