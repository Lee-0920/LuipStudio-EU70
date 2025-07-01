#include "VncManager.h"
#include "VncClientWidget.h"

using namespace VncSpace;


void VncManager::Init(Treasure::Logger* logger)
{
    Treasure::Logger::Instance(logger);
}

void VncManager::Uninit()
{
    foreach(auto item, this->clients)
    {
        item->Uninit();
    }
    this->clients.clear();
}

bool VncManager::Contains(QString ip)
{
    foreach(auto item, this->clients)
    {
        if (item->GetIP() == ip)
            return true;
    }
    return false;
}

VncClientWidgetPtr VncManager::Add(QString ip, int port, AdaptMode mode)
{
    auto client = VncClientWidgetPtr(new VncClientWidget());
    if (client->Connect(ip, port))
    {
        client->SetAdaptMode(mode);
    }

    this->clients.append(client);
    return client;
}

void VncManager::SetAdaptMode(AdaptMode mode)
{
    foreach(auto item, this->clients)
    {
        item->SetAdaptMode(mode);
    }
}

void VncManager::Show(QString ip)
{
    foreach(auto item, this->clients)
    {
        if (item->GetIP() == ip)
        {
            item->show();
            item->StartUpdate();
            return;
        }
    }
}

bool VncManager::IsConnected(QString ip)
{
    foreach(auto item, this->clients)
    {
        if (item->GetIP() == ip)
        {
            return item->IsConnected();
        }
    }
    return false;
}

bool VncManager::ReConnect(QString ip)
{
    foreach(auto item, this->clients)
    {
        if (item->GetIP() == ip)
        {
            auto port = item->GetPort();
            return item->Connect(ip, port);
        }
    }
    return false;
}

void VncManager::Remove(QString ip)
{
    foreach(auto item, this->clients)
    {
        if (item->GetIP() == ip)
        {
            this->clients.removeOne(item);
            return;
        }
    }
}

void VncManager::RemoveAll()
{
    this->clients.clear();
}

void VncManager::HideAll()
{
    foreach(auto item, this->clients)
    {
        item->hide();
        item->StopUpdate();
    }
}

