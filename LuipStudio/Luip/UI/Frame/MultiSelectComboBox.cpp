#include "MultiSelectComboBox.h"
#include "QCheckBox"
#include <QScrollBar>

MultiSelectComboBox::MultiSelectComboBox(QWidget *parent) :
    QComboBox(parent),comboBoxStatus(HidenPopup)
{
    QFont font;
    font.setPointSize(18);

    // 设置 QListWidget 作为下拉菜单
    listWidget = new QListWidget(this);
    listWidget->resize(400,40);
    listWidget->setFont(font);

    setModel(listWidget->model());
    setView(listWidget);

    // 启用多选模式
    listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    //启用垂直滚动条
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //设置滚动条宽度
    listWidget->verticalScrollBar()->setStyleSheet("QScrollBar{width:38px;}");

    // 连接信号槽，当选项改变时更新显示
//    connect(listWidget, &QListWidget::itemChanged, this, &MultiSelectComboBox::SlotUpdateSelectedItems);

    // 设置 QLineEdit 作为显示区域
    lineEdit = new QMyEdit(this);
    font.setPointSize(13);
    lineEdit->setFixedSize(350,37);
    lineEdit->setReadOnly(true); // 设置为只读
    lineEdit->setFont(font);
    lineEdit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
//    lineEdit->setStyleSheet("QLineEdit { background-color:rgb(220,220,220); }");

    this->setLineEdit(lineEdit);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
//    this->setEditable(true);
    connect(this, SIGNAL(ButtonClicked()), this, SLOT(SlotShowCombox()));
    connect(lineEdit, SIGNAL(LineEditClicked()), this, SLOT(SlotShowCombox()));
    connect(this, SIGNAL(BlankAreaClicked(int)), this, SLOT(SlotBlankAreaClicked(int)));
    connect(this, SIGNAL(UpdateLineEdit()), this, SLOT(SlotUpdateSelectedItems()));
}

MultiSelectComboBox::~MultiSelectComboBox()
{

}

// 添加选项
void MultiSelectComboBox::myAddItem(const QString &text, const QVariant &userData)
{
    QFont font;
    font.setPointSize(18);

    QListWidgetItem *item = new QListWidgetItem(listWidget);
    QCheckBox* checkBox = new QCheckBox(text, this);
    checkBox->setObjectName("checkboxone");
    checkBox->setFixedHeight(40);
    checkBox->setFont(font);
    listWidget->addItem(item);    
    listWidget->setItemWidget(item, checkBox);
    connect(checkBox, &QCheckBox::stateChanged, this, &MultiSelectComboBox::SlotStateChange);
}

void MultiSelectComboBox::myAddItems(const QStringList &textList)
{
    for (int i = 0; i < textList.size(); ++i)
    {
        myAddItem(textList[i], QVariant());
    }
}

// 获取选中的项
QList<QString> MultiSelectComboBox::SelectedItemsData() const
{
    QList<QString> selectedData;
    for (int i = 0; i < listWidget->count(); ++i)
    {
        QWidget* widget = listWidget->itemWidget(listWidget->item(i));
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget);
        if(checkBox->isChecked())
        {
            selectedData.append(checkBox->text());
        }
    }
    return selectedData;
}

void MultiSelectComboBox::SetItemsData(QList<QString> &list)
{
    for(int i = 0; i < listWidget->count(); ++i)
    {
        QWidget* widget = listWidget->itemWidget(listWidget->item(i));
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget);
        if(list.contains(checkBox->text()))
        {
            checkBox->setChecked(true);
        }
        else
        {
            checkBox->setChecked(false);
        }
    }
}

void MultiSelectComboBox::SetAllItem(bool status)
{
    for (int i = 0; i < listWidget->count(); ++i)
    {
        QWidget* widget = listWidget->itemWidget(listWidget->item(i));
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget);
        checkBox->setChecked(status);
    }
}

void MultiSelectComboBox::SlotUpdateSelectedItems()
{
    QStringList selectedItems;
    for (int i = 0; i < listWidget->count(); ++i)
    {
        QWidget* widget = listWidget->itemWidget(listWidget->item(i));
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget);
        if(checkBox->isChecked())
        {
            selectedItems.append(checkBox->text());
        }
    }
    lineEdit->setText(selectedItems.join(",")); // 显示选中的项
}

void MultiSelectComboBox::SlotStateChange()
{
    QStringList selectedItems;
    for (int i = 0; i < listWidget->count(); ++i)
    {
        QWidget* widget = listWidget->itemWidget(listWidget->item(i));
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget);
        if(checkBox->isChecked())
        {
            selectedItems.append(checkBox->text());
        }
    }
    lineEdit->setText(selectedItems.join(", ")); // 显示选中的项
}

void MultiSelectComboBox::SlotShowCombox()
{
#ifdef _CS_ARM_LINUX
    int y0 = 0;
#endif
#ifdef _CS_X86_WINDOWS
    int y0 = 170;
#endif
    int viewHight = 600 - QCursor::pos().y() + y0;
    if(listWidget->count() * 40 > viewHight)
    {
        listWidget->viewport()->setMinimumHeight(viewHight);
        listWidget->viewport()->setMaximumHeight(viewHight);
        QComboBox::setMaxVisibleItems(viewHight/40);
    }    
    //正常显示菜单
    if(comboBoxStatus == HidenPopup)
    {
        QComboBox::showPopup();
#ifdef _CS_ARM_LINUX
        //Linux系统抬起会触发hide()事件
        comboBoxStatus = IgnoreNextPopup;
#endif
#ifdef _CS_X86_WINDOWS
        comboBoxStatus = ShowPopup;
#endif
    }
    //点击显示框后收起，本次忽略菜单显示
    else if(comboBoxStatus == IgnoreNextPopup)
    {
        comboBoxStatus = HidenPopup;
    }
}

void MultiSelectComboBox::SlotBlankAreaClicked(int index)
{
    if(comboBoxStatus == ShowPopup)
    {
        QWidget* widget = listWidget->itemWidget(listWidget->item(index));
        QCheckBox* checkBox = qobject_cast<QCheckBox*>(widget);
        if(checkBox->isChecked())
        {
            checkBox->setChecked(false);
        }
        else
        {
            checkBox->setChecked(true);
        }
        emit UpdateLineEdit();
    }
}

void MultiSelectComboBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit ButtonClicked();
    }
//    QComboBox::mousePressEvent(event);
//    qDebug() << "mouse PressEvent:" << event->type();
}

void MultiSelectComboBox::hidePopup()
{
    if(comboBoxStatus == ShowPopup)
    {
        //点击显示框及菜单之外区域，收起菜单
        if(!listWidget->viewport()->contentsRect().contains(listWidget->viewport()->mapFromGlobal(QCursor::pos()))
                && !lineEdit->contentsRect().contains(lineEdit->mapFromGlobal(QCursor::pos())))
        {
            QComboBox::hidePopup();
            comboBoxStatus = HidenPopup;
        }
        //点击显示框，收起菜单，并忽略下次菜单显示(原因：同时触发combobox的hide() & lineedit的clicked信号)
        else if(lineEdit->contentsRect().contains(lineEdit->mapFromGlobal(QCursor::pos())))
        {
            QComboBox::hidePopup();
            comboBoxStatus = IgnoreNextPopup;
        }
        else
        {
            for(int i = 0;i < this->count(); ++i)
            {
                if(listWidget->itemWidget(listWidget->item(i))->contentsRect().contains(listWidget->itemWidget(listWidget->item(i))->mapFromGlobal(QCursor::pos())))
                {
                     emit BlankAreaClicked(i);
                }
            }
        }
    }
    #ifdef _CS_ARM_LINUX
    //Linux系统下忽略按压抬起hide()事件
    else if(comboBoxStatus == IgnoreNextPopup)
    {
        comboBoxStatus = ShowPopup;
    }
    #endif
}



