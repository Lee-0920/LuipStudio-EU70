#ifndef RESULTDETAILMANAGERPROXY_H
#define RESULTDETAILMANAGERPROXY_H

#include "oolua.h"
namespace ResultDetail
{
class ResultDetailManagerProxy
{
public:
    ResultDetailManagerProxy();
    static void Proxy(OOLUA::Script * lua);
};
}
#endif // RESULTDETAILMANAGERPROXY_H
