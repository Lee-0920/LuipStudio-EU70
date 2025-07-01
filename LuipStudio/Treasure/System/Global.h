/**
 * @brief 全局单例模板
 * @author huangyue
 * @date 2021-02-03
 * @note 用法：
 *       T* record = Global<T>::Instance();
 *       获取全局单例类型指针，程序关闭时自动释放
 */
#ifndef __TREASURE_SYSTEM_GLOBAL__
#define __TREASURE_SYSTEM_GLOBAL__

#include <memory>
#include <QMutex>

namespace Treasure
{
    template<class T>
    class Global
    {

    public:
        static T *Instance()
        {
            if (nullptr == m_instance)
            {
                QMutexLocker locker(&m_instanceMutex);
                if (nullptr == m_instance)
                    m_instance.reset(new T);
            }
            return m_instance.get();
        }

    private:
        static QMutex m_instanceMutex;
        static std::unique_ptr<T> m_instance;
    };


    template<class T> QMutex Global<T>::m_instanceMutex;
    template<class T> std::unique_ptr<T> Global<T>::m_instance(nullptr);
}

//单例宏
#define TREASURE_SINGELTON(OBJ_CLASS)                   \
    friend Treasure::Global<OBJ_CLASS>;                 \
                                                        \
public:                                                 \
    static OBJ_CLASS *Instance()                        \
    {                                                   \
        return Treasure::Global<OBJ_CLASS>::Instance(); \
    }

#endif
