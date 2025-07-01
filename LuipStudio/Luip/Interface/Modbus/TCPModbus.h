#ifndef TCPMODBUS_H
#define TCPMODBUS_H

#include <QThread>
#include <memory>
#include "modbus.h"

/* Constants defined by configure.ac */
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# ifndef _MSC_VER
# include <stdint.h>
# else
# include "stdint.h"
# endif
#endif

#include "IFunCodeNotifiable.h"


namespace Interface
{
namespace Modbus
{

class TCPModbus : public QThread
{
public:
    explicit TCPModbus(IFunCodeNotifiable* rw, modbus_mapping_t * mapping);

    bool Init(modbus_mapping_t *mapping);
    bool Start();
    void Stop();

protected:
    void run();

private:
    modbus_t * m_ctx;
    modbus_mapping_t * m_mapping;
    int m_headerLength;
    IFunCodeNotifiable* m_rw;
    bool m_isRuning;

};

}
}

#endif // TCPMODBUS_H
