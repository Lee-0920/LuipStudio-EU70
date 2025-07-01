/**
 * @brief C++反射
 * @author huangyue
 * @date 2021-04-02
 * @note 需要进行二次开发，说明：
 *       1. 必须继承Reflection类
 *       2. 必须SetMembers和SetTableNodes接口，样例如下
 *
 *          template <typename TYPE, typename... TYPES>
 *          bool SetMembers(QStringList &names, int index, OOLUA::Table &table, TYPE &arg, TYPES &...args)
 *
 *          template <typename TYPE>
 *          bool SetMembers(QStringList &names, int index, OOLUA::Table &table, TYPE &arg)
 *
 *          template <typename TYPE, typename... TYPES>
 *          bool SetTableNodes(QStringList &names, int index, QString &tablePath, TYPE &arg, TYPES &...args)
 *
 *          template <typename TYPE>
 *          bool SetTableNodes(QStringList &names, int index, QString &tablePath, TYPE &arg)
 */

#ifndef __TREASURE_SYSTEM_REFLECTION__
#define __TREASURE_SYSTEM_REFLECTION__

#include <memory>
#include <QMap>
#include <QStringList>

namespace Treasure
{

/******************************************************
 * 注册类成员
 * eg:
 * struct Test
 * {
 *      string A;
 *      string B;
 *      REFLECTION_REGISTER(A, B)
 * };
 *
 * 1. ReflectionRegisterCheck 用于判断是否有添加此注册宏
 * 2. ReflectionTableToObject 表格转类\结构体
 * 3. ReflectionObjectToTable 类\结构体转表格
 ******************************************************/
#define REFLECTION_REGISTER(...)                                                             \
    bool ReflectionRegisterCheck()                                                           \
    {                                                                                        \
        return true;                                                                         \
    }                                                                                        \
    template <typename HANDLE_TYPE, typename TABLE_TYPE>                                     \
    bool ReflectionTableToObject(HANDLE_TYPE &handle, TABLE_TYPE &table, QStringList &names) \
    {                                                                                        \
        QStringList standardNames = Treasure::Reflection::StringSplit(#__VA_ARGS__);         \
        names = Treasure::Reflection::CompleteStringList(names, standardNames);              \
        return handle.SetMembers(names, 0, table, __VA_ARGS__);                              \
    }                                                                                        \
    template <typename HANDLE_TYPE, typename TABLE_TYPE>                                     \
    bool ReflectionObjectToTable(HANDLE_TYPE &handle, TABLE_TYPE &table, QStringList &names) \
    {                                                                                        \
        QStringList standardNames = Treasure::Reflection::StringSplit(#__VA_ARGS__);         \
        names = Treasure::Reflection::CompleteStringList(names, standardNames);              \
        return handle.SetTableNodes(names, 0, table, __VA_ARGS__);                           \
    }

/******************************************************
 * 重命名类成员名
 * eg:
 * struct Test
 * {
 *      string A;
 *      string B;
 *      REFLECTION_REGISTER(A, B)
 *      REFLECTION_REGISTER_RENAME("a", "b")  //A、B的顺序要一致
 * };
 ******************************************************/
#define REFLECTION_REGISTER_RENAME(...)                         \
    QStringList ReflectionRenameMembers()                       \
    {                                                           \
        return Treasure::Reflection::StringSplit(#__VA_ARGS__); \
    }

/******************************************************
 * 类成员名默认值（当赋值失败，或者待转换的文本中没找到此名时，使用默认值，而不是直接返回失败）
 * eg:
 * struct Test
 * {
 *      string A;
 *      string B;
 *      REFLECTION_REGISTER(A, B)
 *      REFLECTION_REGISTER_RENAME("a", "b")  //A、B的顺序要一致
 *      REFLECTION_REGISTER_DEFAULT_VALUE("a=my", "b=name")
 * };
 ******************************************************/
#define REFLECTION_REGISTER_DEFAULT_VALUE(...)                     \
    QMap<QString, QString> ReflectionDefaultValues()               \
    {                                                              \
        return Treasure::Reflection::MapStringSplit(#__VA_ARGS__); \
    }

    class Reflection
    {
    public:
        enum class ParaType
        {
            Enum,
            Int,
            Float,
            Double,
            Char,
            Bool,
            LongLong,
            String,
            QString,
            Else,
        };

        /**
         * @brief 获取参数类型
         */
        template <typename TYPE>
        static ParaType GetParaType()
        {
            if (std::is_enum<TYPE>::value)
                return ParaType::Enum;
            if (typeid(TYPE) == typeid(int))
                return ParaType::Int;
            if (typeid(TYPE) == typeid(float))
                return ParaType::Float;
            if (typeid(TYPE) == typeid(double))
                return ParaType::Double;
            if (typeid(TYPE) == typeid(char))
                return ParaType::Char;
            if (typeid(TYPE) == typeid(bool))
                return ParaType::Bool;
            if (typeid(TYPE) == typeid(long long))
                return ParaType::LongLong;
            if (typeid(TYPE) == typeid(std::string))
                return ParaType::String;
            if (typeid(TYPE) == typeid(QString))
                return ParaType::QString;

            return ParaType::Else;
        }

    public:
        /**
         * @brief 字符串分割
         * @param str 成员名称集合，格式为 xxx,yy,zz
         * @param Separator 分隔符
         */
        static QStringList StringSplit(std::string str, char Separator = ',')
        {
            QStringList array = QString::fromStdString(str).split(Separator);
            for (int i = 0; i < array.size(); i++)
                array[i] = array[i].trimmed();
            return array;
        }

        /**
         * @brief 字符串分割并转为key-value
         * @param str 成员名称集合，格式为 xxx=ooo,yy=ooo,zz=ooo
         * @param Separator 分隔符
         */
        static QMap<QString, QString> MapStringSplit(std::string str, char Separator = ',')
        {
            QStringList array = StringSplit(str, Separator);
            QMap<QString, QString> ret;
            foreach (auto item, array)
            {
                QStringList keyValue = StringSplit(item.toStdString(), '=');
                if (keyValue.size() != 2)
                    continue;
                if (!ret.contains(keyValue[0]))
                    ret.insert(keyValue[0], keyValue[1]);
            }
            return ret;
        }

        /**
         * @brief 补全字符串列表
         * @param names 待补全的列表
         * @param standardNames 标准列表
         */
        static QStringList CompleteStringList(const QStringList &names, const QStringList &standardNames)
        {
            QStringList array = names;
            if (names.size() <= standardNames.size())
            {
                for (int i = names.size(); i < (int)standardNames.size(); i++)
                    array.push_back(standardNames[i]);
            }
            return array;
        }

    protected:
        /*****************************************************
         *
         * 默认值管理
         *
         * **************************************************/

#define GET_DEFAULT_VALUE_PRE    \
    if (!maps.contains(name))    \
        return false;            \
    QString svalue = maps[name];

        static bool GetDefaultValue(QMap<QString, QString> maps, QString name, int &value)
        {
            GET_DEFAULT_VALUE_PRE
            bool isSuccess = false;
            value = svalue.toInt(&isSuccess);
            return isSuccess;
        }

        static bool GetDefaultValue(QMap<QString, QString> maps, QString name, float &value)
        {
            GET_DEFAULT_VALUE_PRE
            bool isSuccess = false;
            value = svalue.toFloat(&isSuccess);
            return isSuccess;
        }

        static bool GetDefaultValue(QMap<QString, QString> maps, QString name, double &value)
        {
            GET_DEFAULT_VALUE_PRE
            bool isSuccess = false;
            value = svalue.toDouble(&isSuccess);
            return isSuccess;
        }

        static bool GetDefaultValue(QMap<QString, QString> maps, QString name, char &value)
        {
            GET_DEFAULT_VALUE_PRE
            if (svalue.length() == 3 && svalue[0] == '\'' && svalue[svalue.length() - 1] == '\'')
            {
                value = svalue.mid(1, svalue.length() - 2).toStdString().c_str()[0];
                return true;
            }
            return false;
        }

        static bool GetDefaultValue(QMap<QString, QString> maps, QString name, bool &value)
        {
            GET_DEFAULT_VALUE_PRE
            svalue = svalue.toLower();
            if (svalue == "true")
            {
                value = true;
                return true;
            }
            else if (svalue == "false")
            {
                value = false;
                return true;
            }
            return false;
        }

        static bool GetDefaultValue(QMap<QString, QString> maps, QString name, long long &value)
        {
            GET_DEFAULT_VALUE_PRE
            bool isSuccess = false;
            value = svalue.toLongLong(&isSuccess);
            return isSuccess;
        }

        static bool GetDefaultValue(QMap<QString, QString> maps, QString name, QString &value)
        {
            GET_DEFAULT_VALUE_PRE
            if (svalue[0] == '\"' && svalue[svalue.length() - 1] == '\"')
            {
                value = svalue.mid(1, svalue.length() - 2);
                return true;
            }
            return false;
        }

        static bool GetDefaultValue(QMap<QString, QString> maps, QString name, std::string &value)
        {
            GET_DEFAULT_VALUE_PRE
            if (svalue[0] == '\"' && svalue[svalue.length() - 1] == '\"')
            {
                value = svalue.mid(1, svalue.length() - 2).toStdString();
                return true;
            }
            return false;
        }

        template <typename TYPE>
        static bool GetDefaultValue(QMap<QString, QString> maps, QString name, QList<TYPE> &value)
        {
            GET_DEFAULT_VALUE_PRE
            value.clear();
            return true;
        }

        template <typename TYPE>
        static bool GetDefaultValue(QMap<QString, QString> maps, QString name, QVector<TYPE> &value)
        {
            GET_DEFAULT_VALUE_PRE
            value.clear();
            return true;
        }

    public :
        //判断类中是否有添加反射注册接口
        template <typename T>
        struct HasConverFunction
        {
            template <typename TT>
            static char func(decltype(&TT::ReflectionRegisterCheck));

            template <typename TT>
            static int func(...);

            const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
        };

    public:
        /******************************************************
         *
         * 获取类成员重命名数组
         *
         ******************************************************/

        //判断类中是否有添加重命名接口定义
        template <typename T>
        struct HasRenameFunction
        {
            template <typename TT>
            static char func(decltype(&TT::ReflectionRenameMembers));
            template <typename TT>
            static int func(...);
            const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
        };

        template <typename T, typename std::enable_if<HasRenameFunction<T>::has, int>::type = 0>
        QStringList LoadRenameArray(T &obj)
        {
            return obj.ReflectionRenameMembers();
        }

        template <typename T, typename std::enable_if<!HasRenameFunction<T>::has, int>::type = 0>
        QStringList LoadRenameArray(T &obj)
        {
            (void)obj;
            return QStringList();
        }

        /******************************************************
         *
         * 获取类成员默认值
         *
         ******************************************************/
        //判断类中是否有添加类成员默认值接口定义
        template <typename T>
        struct HasDefaultValueFunction
        {
            template <typename TT>
            static char func(decltype(&TT::ReflectionDefaultValues));
            template <typename TT>
            static int func(...);
            const static bool has = (sizeof(func<T>(NULL)) == sizeof(char));
        };

        template <typename T, typename std::enable_if<HasDefaultValueFunction<T>::has, int>::type = 0>
        QMap<QString, QString> LoadDefaultValuesMap(T &obj)
        {
            return obj.ReflectionDefaultValues();
        }

        template <typename T, typename std::enable_if<!HasDefaultValueFunction<T>::has, int>::type = 0>
        QMap<QString, QString> LoadDefaultValuesMap(T &obj)
        {
            (void)obj;
            return QMap<QString, QString>();
        }
    };

}

#endif
