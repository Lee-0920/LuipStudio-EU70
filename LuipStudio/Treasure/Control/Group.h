/**
 * @brief 控件组绑定
 * @author huangyue
 * @date 2021-02-03
 * @note 用于将多个控件绑定在一组，便于操作
 */

#ifndef __TREASURE_CONTROL_GROUP__
#define __TREASURE_CONTROL_GROUP__

#include <QMap>
#include <QVector>
#include <QString>

#include "../System/Global.h"

namespace Treasure
{
    template<class T>
    class Group
    {
    private:

        /**
         * @brief 获取控件模板的所有组
         */
        static QMap<QString, QVector<T*>>* GetGroupMap()
        {
            QMap<QString, QVector<T*>>* groupMap = Global<QMap<QString, QVector<T*>>>::Instance();
            return groupMap;
        }

        /**
         * @brief 获取组
         * @param groupName 组名
         * @param array 控件列表
         */
        static bool GetGroup(const QString& groupName, QVector<T*>& array)
        {
            QMap<QString, QVector<T*>>* groupMap = GetGroupMap();
            auto group = groupMap->find(groupName);
            if (group == groupMap->end())
                return false;

            array = *group;
            return true;
        }

        /**
         * @brief 添加组
         * @param groupName 组名
         */
        static QVector<T*>& AddGroup(const QString& groupName)
        {
            QMap<QString, QVector<T*>>* groupMap = GetGroupMap();
            auto group = groupMap->find(groupName);
            if (group == groupMap->end())
                group = groupMap->insert(groupName, QVector<T*>());

            return *group;
        }

        /**
         * @brief 删除组
         * @param groupName 组名
         */
        static void RemoveGroup(const QString& groupName)
        {
            QMap<QString, QVector<T*>>* groupMap = GetGroupMap();
            if (groupMap->contains(groupName))
                groupMap->remove(groupName);
        }


    public:

        /**
         * @brief 增加组控件
         * @param groupName 组名
         * @param control 控件指针
         */
        static void AddControl(const QString& groupName, T* control)
        {
            QVector<T*>& array = AddGroup(groupName);
            if (array.indexOf(control) >= 0)
                return;

            array.append(control);
        }

        /**
         * @brief 删除组控件
         * @param groupName 组名
         * @param control 控件指针
         */
        static void RemoveControl(const QString& groupName, T* control)
        {
            QVector<T*> array;
            if (!GetGroup(groupName, array))
                return;

            int index = array.indexOf(control);
            if (index >= 0)
                array.removeAt(index);

            if (array.size() <= 0)
                RemoveGroup(groupName);
        }

        /**
         * @brief 获取组控件集合
         * @param groupName 组名
         */
        static QVector<T*> GetControls(const QString& groupName)
        {
            QVector<T*> array;
            if (!GetGroup(groupName, array))
                return array;
            return array;
        }

    private:

        /**
         * @brief 控件操作
         * @param groupName 组名
         * @param ignoreControl 忽略的控件
         * @param func 操作函数
         */
        static void ControlsOperate(const QString& groupName, T* ignoreControl, std::function<void(T*)> func)
        {
            QVector<T*> array = GetControls(groupName);
            for(int i = 0; i < array.size(); i++)
            {
                T* item = array[i];
                if (item == ignoreControl)
                    continue;

                func(item);
            }
        }

    public:

        static void SetCheck(const QString& groupName, bool checked = true, T* ignoreControl = NULL)
        {
            auto func = [&checked](T* item)
            {
                item->setChecked(checked);
            };
            ControlsOperate(groupName, ignoreControl, func);
        }

        static void SetEnable(const QString& groupName, bool enable = true, T* ignoreControl = NULL)
        {
            auto func = [&enable](T* item)
            {
                item->setEnabled(enable);
            };
            ControlsOperate(groupName, ignoreControl, func);
        }

        static void SetShow(QString groupName, bool isShow = true, T* ignoreControl = NULL)
        {
            auto func = [&isShow](T* item)
            {
                if (isShow)
                    item->show();
                else
                    item->hide();
            };
            ControlsOperate(groupName, ignoreControl, func);
        }

    };


}


#endif
