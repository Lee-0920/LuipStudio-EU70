#include "ReagentRemainVector.h"
namespace Interface
{
namespace Wqimc
{
ReagentRemainVector::ReagentRemainVector()
{

}

void ReagentRemainVector::Push(Api::Manager::ReagentRemain reagentRemain)
{
    reagentRemainList.push_back(reagentRemain);
}

System::Uint32 ReagentRemainVector::Size()
{
    return reagentRemainList.size();
}

std::vector<Api::Manager::ReagentRemain> ReagentRemainVector::GetReagentRemainList(void)
{
    return reagentRemainList;
}

}
}
