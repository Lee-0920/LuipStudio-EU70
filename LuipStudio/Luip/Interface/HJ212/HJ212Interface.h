
#if !defined(MODBUS_HJ212INTERFACE_H)
#define MODBUS_HJ212INTERFACE_H

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
namespace HJ212
{

enum class HJ212Type
{
    None = 0,
    V2005 = 1,
    V2017_Dbs = 2,
    V2017_Wry = 3,
};

enum class HJ212DataType
{
    HourData = 0,
    MinuteData = 1,
    RealTimeDate = 2,
};

class HJ212Interface : public QThread, public IResultNotifiable
{
public:
    static HJ212Interface *Instance();
    ~HJ212Interface();
    bool Init();
    void Start();
    void Stop();

protected:
    void run();
    void OnMeasureResultAdded(String name, ResultData::RecordData result);
    void OnCalibrateResultAdded(String name, ResultData::RecordData result);

private:
    HJ212Interface();
    void UpReportData();
    Uint16 CRC(const char *pBuf, int nLen);

private:
    static std::unique_ptr<HJ212Interface> m_instance;
    bool m_isRuning;

    HJ212Type m_hj212Type;
    HJ212DataType m_hj212DataType;
    int m_hj212Interval;
    int m_decimalNum;
    int m_hj212CommunicationMode;

    lua_State * m_mainState;
    lua_State * m_newState;
    QSerialPort* m_serialPort;

    bool m_dataUpdated;
};
}
}
#endif // MODBUS_HJ212INTERFACE_H
