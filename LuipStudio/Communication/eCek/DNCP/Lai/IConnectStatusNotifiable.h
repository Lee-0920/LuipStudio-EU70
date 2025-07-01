/**
 * @file
 * @brief 通信连接上报接口。
 * @version 1.0.0
 * @author ludijun
 * @date 2016/8/25
 */

#ifndef MEASURE_ICONNECTSTATUSNOTIFIABLE_H_
#define MEASURE_ICONNECTSTATUSNOTIFIABLE_H_

/**
 * @brief 通信连接上报接口。
 * @details 观察者。
 */
class IConnectStatusNotifiable
{
public:
    IConnectStatusNotifiable() {}
    virtual ~IConnectStatusNotifiable() {}

public:
    virtual void OnConnectStatusChanged(unsigned char pollingAddr, bool status) = 0;

};

#endif  //MEASURE_ICONNECTSTATUSNOTIFIABLE_H_
