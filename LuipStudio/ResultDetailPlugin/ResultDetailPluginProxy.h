#ifndef RESULTDETAILPLUGINPROXY_H
#define RESULTDETAILPLUGINPROXY_H

#include "ResultDetailWidget.h"
#include "MeasureResultDetailWidget.h"
#include "CalibrateResultDetailWidget.h"
#include "LuipShare.h"
#include "oolua.h"

using ResultDetail::ResultDetailWidget;
using ResultDetail::MeasureResultDetailWidget;
using ResultDetail::CalibrateResultDetailWidget;

OOLUA_PROXY(LUIP_SHARE, ResultDetailWidget)
OOLUA_PROXY_END

OOLUA_PROXY(LUIP_SHARE, MeasureResultDetailWidget, ResultDetailWidget)
OOLUA_PROXY_END

OOLUA_PROXY(LUIP_SHARE, CalibrateResultDetailWidget, ResultDetailWidget)
OOLUA_PROXY_END

namespace ResultDetail
{
class ResultDetailPluginProxy
{
public:
    ResultDetailPluginProxy();
    static void Proxy();
};
}
#endif // RESULTDETAILPLUGINPROXY_H
