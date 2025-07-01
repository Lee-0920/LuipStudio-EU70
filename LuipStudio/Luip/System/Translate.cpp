#include <QObject>
#include "Types.h"
#include "Translate.h"

namespace System
{

QString Translate::ComplexTranslate(QString string)
{
    QString symbols = " ~!@#$^&*-=_+\|()[]{}<>:;',./?0123456789";
    String array = string.toStdString();

    QString trStr;
    String temp;
    for(int i = 0; i < array.size(); i++)
    {
        char c = array.at(i);
        if(symbols.contains(QString(c)))
        {
            if(!temp.empty())
            {
                trStr += QObject::tr(temp.c_str());
                temp.clear();
            }
            trStr += c;
        }
        else
        {
            temp.push_back(c);
            if(i == array.size() - 1)
            {
                trStr += QObject::tr(temp.c_str());
            }
        }
    }

    return trStr;
}

}
