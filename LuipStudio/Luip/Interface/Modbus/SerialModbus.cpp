#include <QDebug>
#include "SerialModbus.h"
#include "Log.h"

namespace Interface
{
namespace Modbus
{

SerialModbus::SerialModbus(IFunCodeNotifiable *rw, modbus_mapping_t *mapping)
{
    this->m_serverAddr = 0;
    this->m_baud = 115200;
    this->m_parity = 'N';
    this->m_dataBit = 8;
    this->m_stopBit = 1;
    this->m_ctx = nullptr;
    this->m_headerLength = 0;
    this->m_isRuning = false;
    this->m_mapping = mapping;
    this->m_rw = rw;
}

void SerialModbus::SetAddress(int Addr)
{
    this->m_serverAddr = Addr;
}

bool SerialModbus::SetCommunicationParam(QString port, int baud, char parity,
                                         int dataBit, int stopBit)
{
    m_ctx = modbus_new_rtu(port.toStdString().c_str(), baud,
                           parity, dataBit, stopBit);

    modbus_set_slave(m_ctx, m_serverAddr);
    m_headerLength = modbus_get_header_length(m_ctx);
    modbus_set_debug(m_ctx, FALSE);

    if (modbus_connect(m_ctx) == -1)
    {
        qDebug() << "Unable to connect " << modbus_strerror(errno);

        modbus_free(m_ctx);
        return false;
    }

    return true;
}

void SerialModbus::Start()
{
    m_isRuning = true;
    start();
}

void SerialModbus::Stop()
{
    m_isRuning = false;
}

void SerialModbus::run()
{
    logger->debug("SerialModbus Thread ID = %d", QThread::currentThreadId());

    int rc;

    uint8_t *query = (uint8_t *)malloc(MODBUS_RTU_MAX_ADU_LENGTH);
    if (!query)
    {
        logger->warn("SerialModbus query is NULL");
        return;
    }
    else
    {
        memset(query, 0, sizeof(query[0])*MODBUS_RTU_MAX_ADU_LENGTH);
    }

    while(m_isRuning)
    {
        // 数据接收
        rc = modbus_receive(m_ctx, query);

        if (rc > 0)
        {
            // 功能码 、地址、寄存器数量解析
            int funCode = query[1];
            int regAddr = query[2];
            regAddr = (regAddr << 8) + query[3];

            int regNum = 0;
            if (funCode == 0x05 || funCode == 0x06)
            {
                regNum = 1;
            }
            else
            {
                regNum = query[4];
                regNum = (regNum << 8) + query[5];
            }

            // 只读寄存器操作
            m_rw->OnRegisterRead(funCode, regAddr, regNum);

            // 应答
            rc = modbus_reply(m_ctx, query, rc, m_mapping);

            // 保持寄存器操作
            m_rw->OnRegisterWrited(funCode, regAddr, regNum);

            if (rc == -1)
            {
                logger->debug("{modbus_reply} %s", modbus_strerror(errno));
            }
        }
        else if (rc == -1)
        {
            logger->debug("{modbus_receive} %s", modbus_strerror(errno));
        }

        QThread::msleep(10);
    }

    logger->debug("{SerialModbus Thread Quit the loop} %s", modbus_strerror(errno));

    free(query);
    modbus_close(m_ctx);
    modbus_free(m_ctx);
}


}

}
