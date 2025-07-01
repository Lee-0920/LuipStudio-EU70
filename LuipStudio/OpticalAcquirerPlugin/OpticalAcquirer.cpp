/**
 * @file
 * @brief 光学信号采集器。
 * @details
 * @version 1.0.0
 * @author kim@erchashu.com
 * @date 2016/5/13
 */
#include "Log.h"
#include "Communication/EventHandler.h"
#include "OpticalAcquirer.h"
#include "Communication/CommunicationException.h"

using namespace std;
using namespace Communication;
using namespace Communication::Dscp;

namespace Controller
{

/**
 * @brief 单泵液路控制器构造。
 */
OpticalAcquirer::OpticalAcquirer(DscpAddress addr)
    : BaseController(addr),
    IOpticalAcquire(new OpticalAcquireInterface(addr))
{

}

OpticalAcquirer::~OpticalAcquirer()
{
    if (IOpticalAcquire)
        delete IOpticalAcquire;
}

/**
 * @brief 初始化 Controller 环境。
 */
bool OpticalAcquirer::Init()
{
    IOpticalAcquire->RegisterAcquireADNotice(this);

    return true;
}

bool OpticalAcquirer::Uninit()
{
    return true;
}

void OpticalAcquirer::Register(ISignalNotifiable *handle)
{
    m_notifise.push_back(handle);
}

OpticalAcquireInterface *OpticalAcquirer::GetIOpticalAcquire()
{
    return IOpticalAcquire;
}

void OpticalAcquirer::OnReceive(DscpEventPtr event)
{
    double ref = *((unsigned long*)(event->data));
    double mea = *((unsigned long*)(event->data + 4));
    double abs = 0;
    if (0 < ref && 0 < mea)
    {
        abs = 1000 * log10(ref/mea);
    }

    if(!m_notifise.empty())
    {
        list<ISignalNotifiable*>::iterator it;
        for(it = m_notifise.begin(); it != m_notifise.end(); it++)
        {
            (*it)->OnSignalChanged("refAD", ref);
            (*it)->OnSignalChanged("meaAD", mea);
            (*it)->OnSignalChanged("abs", abs);
        }
    }
}

void OpticalAcquirer::StartSignalUpload()
{
    if (this->GetConnectStatus())
    {
        try
        {
            this->IOpticalAcquire->SetAcquireADNotifyPeriod(1);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("OpticalAcquirer::StartSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("OpticalAcquirer::StartSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("OpticalAcquirer::StartSignalUpload() => %s", e.what());
        }
    }
}

void OpticalAcquirer::StopSignalUpload()
{
    if (this->GetConnectStatus())
    {
        try
        {
            this->IOpticalAcquire->SetAcquireADNotifyPeriod(0);
        }
        catch(CommandTimeoutException e)
        {
            logger->warn("OpticalAcquirer::StopSignalUpload() => %s", e.What().c_str());
        }
        catch(ExpectEventTimeoutException e)
        {
            logger->warn("OpticalAcquirer::StopSignalUpload() => %s", e.What().c_str());
        }
        catch (std::exception e)
        {
            logger->warn("OpticalAcquirer::StopSignalUpload() => %s", e.what());
        }
    }
}

}


