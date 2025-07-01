#ifndef VNC_MANAGER_H
#define VNC_MANAGER_H

#include <map>
#include <memory>
#include <mutex>

#include <QMap>
#include <QString>

#include "Treasure/LuipShare.h"
#include "Treasure/SystemDef.h"

#include "VncClientWidget.h"

namespace VncSpace
{

class LUIP_SHARE VncManager
{
    TREASURE_SINGELTON(VncManager)

public:
    void Init(Treasure::Logger* logger);
    void Uninit();

    bool Contains(QString ip);
    bool IsConnected(QString ip);
    bool ReConnect(QString ip);

    void Show(QString ip);
    void HideAll();

    void Remove(QString ip);
    void RemoveAll();
    VncClientWidgetPtr Add(QString ip, int port = 5900, AdaptMode mode = AdaptMode::AspectRation);
    void SetAdaptMode(AdaptMode mode);  //设置适配模式


private:
    QList<VncClientWidgetPtr> clients;
};

}

#endif
