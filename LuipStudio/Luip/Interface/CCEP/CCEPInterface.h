
#if !defined(MODBUS_CCEPINTERFACE_H)
#define MODBUS_CCEPINTERFACE_H

#include <memory>
#include <QThread>
#include <QString>
#include <QSerialPort>
#include "lua.hpp"
#include "System/Types.h"
#include "ResultManager/IResultNotifiable.h"

using System::Uint8;
using System::Uint16;
using System::String;
using Result::IResultNotifiable;

namespace Interface
{
namespace CCEP
{

enum class MagType
{
    data = 0,
    hend = 1,
};

class CCEPInterface : public QThread, public IResultNotifiable
{
public:
    static CCEPInterface *Instance();
    ~CCEPInterface();
    bool Init();
    void Start();
    void Stop();

protected:
    void run();
    void OnMeasureResultAdded(String name, ResultData::RecordData result);
    void OnCalibrateResultAdded(String name, ResultData::RecordData result);

private:
    CCEPInterface();
   void UpReportMessage(MagType type);
   Uint16 CRC(const char *pBuf, int nLen);

private:
    static std::unique_ptr<CCEPInterface> m_instance;
    bool m_isRuning;

    lua_State * m_mainState;
    lua_State * m_newState;
    QSerialPort* m_serialPort;

    bool m_dataUpdated;
};
}
}
#endif // MODBUS_CCEPINTERFACE_H
