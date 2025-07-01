#ifndef UI_FRAME_METHODTITLEWIDGET_H
#define UI_FRAME_METHODTITLEWIDGET_H

#include <QWidget>
#include <QtWidgets>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAction>
#include <QMenu>
#include "ToolButton.h"
#include "PushButton.h"
#include "UI/Frame/LoginDialog.h"
#include "System/LockScreen/ILockScreenNotifiable.h"

using System::ILockScreenNotifiable;

namespace UI
{

class MethodTitleWidget : public QWidget ,public ILockScreenNotifiable
{
    Q_OBJECT
public:
    explicit MethodTitleWidget(QWidget *parent = 0);
    void TranslateLanguage();
    void SetupUIWidget();
    void OnLockScreen();

signals:

    void MethodTurnPage(int currentPage);
    void ScreenAlreadyLock();
    void ShowMin();
    void CloseWidget();
    void MethodQuit();

public slots:
    void SlotTurnPage(QString currentPage);   
private:
    void ChangeBottomStatus();    
private:
    QLabel *m_softwareTitle; //标题
    ToolButton *m_closeButton; //关闭
    QList<ToolButton *> m_buttonList;

    QVBoxLayout *mainLayout;
    QHBoxLayout *titleLayout;
    QHBoxLayout *buttonLayoutTemp;    
};

}

#endif // UI_FRAME_TITLEWIDGET_H
