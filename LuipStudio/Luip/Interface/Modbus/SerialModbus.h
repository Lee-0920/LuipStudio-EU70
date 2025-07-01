
#if !defined(MODBUS_SERIALMODBUS_H)
#define MODBUS_SERIALMODBUS_H

#include <QThread>
#include <QString>
#include "IFunCodeNotifiable.h"
#include "modbus.h"

namespace Interface
{
namespace Modbus
{

class SerialModbus : public QThread
{
public:
    explicit SerialModbus(IFunCodeNotifiable* rw, modbus_mapping_t * mapping);
    void SetAddress(int Addr);
    bool SetCommunicationParam(QString port, int baud, char parity, int dataBit, int stopBit);
    void Start();
    void Stop();

protected:
    void run();

private:
    int m_serverAddr;
    QString m_device;
    int m_baud;
    char m_parity;
    int m_dataBit;
    int m_stopBit;
    modbus_t * m_ctx;
    modbus_mapping_t * m_mapping;
    int m_headerLength;
    IFunCodeNotifiable* m_rw;
    bool m_isRuning;
};
}
}
#endif // MODBUS_SERIALMODBUS_H
