#ifndef UI_FRAME_MUlTISELECTCOMBOX_H
#define UI_FRAME_MUlTISELECTCOMBOX_H
#include <QApplication>
#include <QComboBox>
#include <QListWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QDebug>
#include <QtGui>
#include <QWidget>
#include <QMouseEvent>
#include "UI/Frame/QMyEdit.h"

class MultiSelectComboBox : public QComboBox
{
    Q_OBJECT

enum ComboBoxStatus
{
    HidenPopup,     //菜单隐藏状态
    ShowPopup,      //菜单展示状态
    IgnoreNextPopup,//忽略展示状态(需要通过该状态避免收起与展示冲突)
};

public:
    explicit MultiSelectComboBox(QWidget *parent = 0);
    ~MultiSelectComboBox();
	
    //添加一条选项
    void myAddItem(const QString &text, const QVariant &userData);
    //添加多条选项
    void myAddItems(const QStringList &textList);
    //获取选中的项
    QList<QString> SelectedItemsData() const;
    //设置内容选中
    void SetItemsData(QList<QString> &list);
    //全选or全不选
    void SetAllItem(bool);

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    void hidePopup() override;
signals:
    void ButtonClicked();
    void BlankAreaClicked(int);
    void UpdateLineEdit();
private slots:
    //更新显示区域
    void SlotUpdateSelectedItems();
    //更新显示区域
    void SlotStateChange();
    //显示菜单
    void SlotShowCombox();
    //点击空白
    void SlotBlankAreaClicked(int);
private:
    QListWidget *listWidget;
    QMyEdit *lineEdit;
    ComboBoxStatus comboBoxStatus;
};

#endif // UI_FRAME_MUlTISELECTCOMBOX_H
