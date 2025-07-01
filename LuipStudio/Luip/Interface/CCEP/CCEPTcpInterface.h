#ifndef CCEP_TCP_INTERFACE_H
#define CCEP_TCP_INTERFACE_H

#include <memory>
#include <QThread>
#include <QString>
#include <QTcpSocket>
#include "lua.hpp"
#include "System/Types.h"
#include "ResultManager/IResultNotifiable.h"
#include "CCEPInterface.h"

using System::Uint8;
using System::Uint16;
using System::String;
using Result::IResultNotifiable;

namespace Interface
{
namespace CCEP
{

class CCEPTcpInterface : public QThread, public IResultNotifiable
{
public:
    static CCEPTcpInterface *Instance();
    ~CCEPTcpInterface();
    bool Init();
    void Start();
    void Stop();

protected:
    void run();
    void OnMeasureResultAdded(String name, ResultData::RecordData result);
    void OnCalibrateResultAdded(String name, ResultData::RecordData result);

private:
    CCEPTcpInterface();
   void UpReportMessage(MagType type);
    Uint16 CRC(const char *pBuf, int nLen);

private:
    static std::unique_ptr<CCEPTcpInterface> m_instance;
    bool m_isRuning;

    String m_ip;
    int m_port;

    lua_State * m_mainState;
    lua_State * m_newState;
    QTcpSocket* m_tcpSocket;

    bool m_dataUpdated;
};
}
}
#endif //CCEP_TCP_INTERFACE_H
