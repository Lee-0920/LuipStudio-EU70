#if !defined(UI_FRAME_UPDATEWIDGETMANAGER_H)
#define UI_FRAME_UPDATEWIDGETMANAGER_H

#include "IUpdateWidgetNotifiable.h"
#include <list>
#include <memory>
#include <QObject>
#include <QMetaType>

Q_DECLARE_METATYPE(UI::UpdateEvent)
Q_DECLARE_METATYPE(System::String)

namespace UI
{

class UpdateWidgetManager : public QObject
{
    Q_OBJECT
public:
    UpdateWidgetManager();
    ~UpdateWidgetManager();
    static UpdateWidgetManager* Instance();
    void Init();
    void Register(IUpdateWidgetNotifiable *handle);
    void Update(UpdateEvent event, System::String message = "");

signals:
    void UpdateWidget(UI::UpdateEvent event, System::String message);

public slots:
    void SlotUpdateWidget(UI::UpdateEvent event, System::String message = "");

private:
    std::list<IUpdateWidgetNotifiable*> m_notifise;
    static std::unique_ptr<UpdateWidgetManager> m_instance;
};


}
#endif // UI_FRAME_UPDATEWIDGETMANAGER_H
