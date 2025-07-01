#ifndef SETSNDIALOG_H
#define SETSNDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include "UI/Frame/NumberKeyboard.h"
#include "oolua.h"
#include "System/Types.h"

namespace UI
{

struct typeChangesItem
{
    System::String type;
    System::String bindCmd = "";
    System::String unBindCmd = "";
    bool isReStart = false;
};

class SetSNDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SetSNDialog(QWidget *parent = 0);
    ~SetSNDialog();

protected:
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void DisplayInfo();

private slots:
    void SlotReadDefaultButton();
    void SlotReadInputButton();
    void SlotOkButton();
    void SlotQuitButton();

private:
    QLabel* m_manufactureTypeLabel;
    QComboBox* m_manufactureTypeBox;

    QLabel* m_modelLabel;
    QLabel* m_modelEdit;
    QLabel* m_nameLabel;
    QLabel* m_nameEdit;
    QLabel* m_mfrsLabel;
    QLabel* m_mfrsEdit;

    QLabel* m_typeChangesLabel;
    QComboBox* m_typeChangesBox;

    QLabel* m_snLabel;
    QLabel* m_charLabel;
    QLineEdit* m_snEdit;

    QPushButton* m_readDefaultButton;
    QPushButton* m_readInputButton;

    QPushButton* m_okButton;
    QPushButton* m_quitButton;
    CNumberKeyboard *m_numberKey;

    OOLUA::Script *m_lua;
    System::Uint8 m_typeChangesCount;
    std::map<System::Uint8, typeChangesItem> m_typeItems;
    bool isShowTypeChanges;
};

}

#endif // SETSNDIALOG_H
