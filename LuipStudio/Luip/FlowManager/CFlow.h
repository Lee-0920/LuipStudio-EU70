#ifndef FLOWMANAGER_CFLOW_H
#define FLOWMANAGER_CFLOW_H

namespace Flow
{

class CFlow
{
public:
    CFlow() {}
    virtual ~CFlow() {}
    virtual void OnStarted() {}
    virtual void Run() {}
    virtual void OnStopped() {}
};

}
#endif // FLOWMANAGER_CFLOW_H
