#include "HJ212Connection.h"

using namespace HJ212;

HJ212Connection::HJ212Connection(OnReceiveCallBack onReceiveCallBack, void* onReceiveParent)
{
    this->m_onReceiveParent = onReceiveParent;
    this->m_onReceive = onReceiveCallBack;
    this->m_isConnected = false;
}

HJ212Connection::~HJ212Connection()
{

}

bool HJ212Connection::Connect()
{
    return false;
}

void HJ212Connection::CloseConnection()
{

}

bool HJ212Connection::IsConnected()
{
//    QReadLocker locker(&m_wrlock);
    return m_isConnected;
}

int HJ212Connection::Send(HJ212PackPtr pack, int& errNum)
{
    (void)pack;
    (void)errNum;
    return 0;
}

int HJ212Connection::ReceiveData()
{
    return 0;
}
