/*
 登陆界面 包括360、新浪、人人登录
 作者：╰☆奋斗ing❤孩子`
 博客地址：http://blog.sina.com.cn/liang19890820
 QQ：550755606
 Qt分享、交流群：26197884

 注：请尊重原作者劳动成果，仅供学习使用，请勿盗用，违者必究！
 */

#ifndef UI_FRAME_SIGNATUREDIALOG_H
#define UI_FRAME_SIGNATUREDIALOG_H

#include <QDialog>
#include <QStackedLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QKeyEvent>
#include <QGridLayout>
#include "Common.h"
#include "DropShadowWidget.h"
#include "UI/Frame/NumberKeyboard.h"
#include "DataBasePlugin/DataBaseManager.h"
#include "UI/Frame/InputKeyBoard.h"

using namespace DataBaseSpace;

namespace UI
{

class SignatureDialog: public DropShadowWidget
{
Q_OBJECT

public:

    explicit SignatureDialog(QWidget *parent = 0);
    ~SignatureDialog();

public:
    void TranslateLanguage();
    void FillObjectText(QString text);
    void FillDescriptorText(QString text);
public slots:
    void SlotloginButton();
protected:

    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);

public:
    static int lastSignatureTime;
    static int GetLastSignatureTime()
    {
        return lastSignatureTime;
    }
    static void SetNewSignatureTime(int time_t)
    {
        lastSignatureTime = time_t;
    }    
private:

    QLabel *m_titleLabel;
    QLabel *m_logoLabel;
    QLabel *m_userLabel;
    QLabel *m_passwordLabel;
    QLabel *m_objectLabel;
    QLabel *m_descriptorLabel;
    QComboBox *m_userComboBox;
    QComboBox *m_detailsComboBox;
    QLineEdit *m_passwordLineEdit;
    QLineEdit *m_objectEdit;
    QLineEdit *m_descriptorEdit;
    QPushButton *m_loginButton;
    QPushButton *m_cancelButton;
    int m_totalClick;
    bool m_superAppear;
    CNumberKeyboard *m_SignatureDialogKeyboard;
    InputKeyBoard *m_inputKeyboard;
};

}

#endif //UI_FRAME_SIGNATUREDIALOG_H
