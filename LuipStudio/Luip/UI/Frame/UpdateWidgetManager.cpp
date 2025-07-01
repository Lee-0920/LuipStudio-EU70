#include "UpdateWidgetManager.h"
#include "Log.h"
#include <QThread>

using namespace std;

namespace UI
{
unique_ptr<UpdateWidgetManager> UpdateWidgetManager::m_instance(nullptr);

UpdateWidgetManager::UpdateWidgetManager()
{

}

UpdateWidgetManager::~UpdateWidgetManager()
{

}

UpdateWidgetManager* UpdateWidgetManager::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new UpdateWidgetManager);
    }

    return m_instance.get();
}

void UpdateWidgetManager::Init()
{
    qRegisterMetaType<UI::UpdateEvent>("UI::UpdateEvent");
    qRegisterMetaType<System::String>("System::String");
    QObject::connect(this, SIGNAL(UpdateWidget(UI::UpdateEvent,System::String)), this,
            SLOT(SlotUpdateWidget(UI::UpdateEvent,System::String)), Qt::BlockingQueuedConnection);
}

void UpdateWidgetManager::Register(IUpdateWidgetNotifiable *handle)
{
    m_notifise.push_back(handle);
}

void UpdateWidgetManager::Update(UI::UpdateEvent event, System::String message)
{
    emit UpdateWidget(event, message);
}

void UpdateWidgetManager::SlotUpdateWidget(UI::UpdateEvent event, System::String message)
{
    for(std::list<IUpdateWidgetNotifiable *>::iterator it = m_notifise.begin(); it != m_notifise.end(); it++)
    {
        if (*it != NULL)
        {
            (*it)->OnUpdateWidget(event, message);
        }
    }
}

}
