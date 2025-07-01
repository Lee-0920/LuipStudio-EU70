#ifndef DYNAMICPASSWORD_H
#define DYNAMICPASSWORD_H

#include <QString>
#include <QDateTime>

namespace System {

static const QString luipKeyWord = "LuipAdmin@";
static const QString reagentKeyWord = "ReagentAdmin@";

class DynamicPassword
{
public:
    static QString GeneratePassword(QDateTime time = QDateTime::currentDateTime());
    static QString GenerateReagentPassword(QDateTime time = QDateTime::currentDateTime());
    static QStringList GeneratePasswordList();
    static QStringList GenerateReagentPasswordList();

protected:

    static QString CreatePassword(QString keyWord, QDateTime time = QDateTime::currentDateTime());

};

}


#endif // DYNAMICPASSWORD_H
