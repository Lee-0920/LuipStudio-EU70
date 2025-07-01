#ifndef UI_USERCHANGEMANAGER_IUSERCHANGENOTIFAIABLE_H
#define UI_USERCHANGEMANAGER_IUSERCHANGENOTIFAIABLE_H

#include <QString>

namespace UI {

class IUserChangeNotifiable
{
public:
    IUserChangeNotifiable() {}
    virtual ~IUserChangeNotifiable() {}

public:
    virtual void OnUserChange() = 0;
};

}
#endif // UI_USERCHANGEMANAGER_IUSERCHANGENOTIFAIABLE_H
