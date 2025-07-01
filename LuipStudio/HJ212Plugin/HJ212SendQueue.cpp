#include "HJ212SendQueue.h"

namespace HJ212
{

HJ212SendQueue::HJ212SendQueue(int capacity)
{
    m_capacity = capacity;
}

HJ212SendQueue::~HJ212SendQueue()
{
    Clear();
}

void HJ212SendQueue::Add(HJ212PackPtr pack)
{
    std::unique_lock<std::mutex> locker(m_mutex);
    if (m_queue.size() >= m_capacity)
        m_queue.dequeue();

    m_queue.enqueue(pack);
}

bool HJ212SendQueue::Get(HJ212PackPtr& pack)
{
    std::unique_lock<std::mutex> locker(m_mutex);
    if (m_queue.size() <= 0)
        return false;

    pack = m_queue.dequeue();
    return true;
}

void HJ212SendQueue::Clear()
{
    std::unique_lock<std::mutex> locker(m_mutex);
    m_queue.clear();
}

int HJ212SendQueue::Size()
{
    std::unique_lock<std::mutex> locker(m_mutex);
    return m_queue.size();
}

int HJ212SendQueue::Capacity() const
{
    return m_capacity;
}

}
