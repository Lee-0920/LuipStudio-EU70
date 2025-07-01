#include <QThread>
#include "App.h"
#include <QUuid>

namespace Lua
{

App::App()
{

}
System::String App::CreateUUID(void)
{
    return QUuid::createUuid().toString().toStdString();
}

}
