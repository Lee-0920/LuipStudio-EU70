/**
 * @brief  HJ212发送队列
 * @author huangyue
 * @date   2020/11/10
 */
#ifndef HJ212SENDQUEUE_H
#define HJ212SENDQUEUE_H

#include <memory>
#include <mutex>
#include <QQueue>

#include "API/HJ212Packet.h"

namespace HJ212
{

class HJ212SendQueue
{
public:
    HJ212SendQueue(int capacity);
    ~HJ212SendQueue();

    void Add(HJ212PackPtr pack);
    bool Get(HJ212PackPtr& pack);
    void Clear();

    int Size();
    int Capacity() const;

private:
    int m_capacity;
    std::mutex m_mutex;
    QQueue<HJ212PackPtr> m_queue;
};

typedef std::shared_ptr<HJ212SendQueue> HJ212SendQueuePtr;

}

#endif // HJ212SENDQUEUE_H
