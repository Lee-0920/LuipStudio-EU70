#ifndef RESULTDETAILPLUGIN_H
#define RESULTDETAILPLUGIN_H

#include "LuipShare.h"
#include "Log.h"
#include "System/Types.h"
#include <memory>
#include "ResultDetailWidget.h"
namespace ResultDetail
{
class LUIP_SHARE ResultDetailPlugin
{
public:
    virtual ~ResultDetailPlugin();
    static ResultDetailPlugin* Instance();
    virtual bool Init(log4cpp::Category* log);
    virtual System::String GetVersion();
private:
    static std::unique_ptr<ResultDetailPlugin> m_instance;
    const System::String version = System::String("1.0.0.0");
};
}
#endif // RESULTDETAILPLUGIN_H
