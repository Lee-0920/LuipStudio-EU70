#ifndef CHECKSCREEN_H
#define CHECKSCREEN_H

#include <QObject>
#include <memory>
#include <iostream>

namespace System
{
class CheckScreen : public QObject
{
    Q_OBJECT
public:
    static CheckScreen *Instance();
    explicit CheckScreen(QObject *parent = 0);
    ~CheckScreen();
    void Init();
    bool isVNC();
    bool isFNS();
    bool isADC();
    bool isRelay();

private:
    static std::unique_ptr<CheckScreen> m_instance;
    bool m_checkVNC;
    bool m_checkFNS;
    bool m_checkADC;
    bool m_checkRelay;
};
}

#endif // CHECKSCREEN_H
