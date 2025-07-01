#ifndef REAGENTREMAINVECTOR_H
#define REAGENTREMAINVECTOR_H

#include <vector>
#include "Interface/Wqimc/Api/Manager/InstrumentManager_types.h"
#include "System/Types.h"

namespace Interface
{
namespace Wqimc
{
class ReagentRemainVector
{
public:
    ReagentRemainVector();
    void Push(Api::Manager::ReagentRemain reagentRemain);
    System::Uint32 Size();
    std::vector<Api::Manager::ReagentRemain> GetReagentRemainList(void);
private:
    std::vector<Api::Manager::ReagentRemain> reagentRemainList;
};
}
}
#endif // REAGENTREMAINVECTOR_H
