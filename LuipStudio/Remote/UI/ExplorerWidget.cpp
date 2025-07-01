#include "ExplorerWidget.h"

ExplorerWidget::ExplorerWidget(QWidget *parent) : QWidget(parent)
{
    m_thread = NULL;
    m_basePath = "";
    m_curPath = "";

    InitWidget();
    this->setObjectName("explorerWidgetv");
    this->setAttribute(Qt::WA_StyledBackground);

    connect(m_exportBtn, &QPushButton::clicked, this, &ExplorerWidget::OnExportClick);
    connect(m_returnBtn, &QPushButton::clicked, this, &ExplorerWidget::OnReturnClick);
    connect(m_delBtn, &QPushButton::clicked, this, &ExplorerWidget::OnDelClick);
    connect(m_importBtn, &QPushButton::clicked, this, &ExplorerWidget::OnImportClick);
    connect(m_objTable, &QTableWidget::doubleClicked, this, &ExplorerWidget::OnTableItemClick);
    connect(this, &ExplorerWidget::SignalExportEnd, this, [=](bool result){
        Style::Instance()->messageBox->Show("提示", result ? "目录导出成功!" : "目录导出失败!");
        m_importBtn->setEnabled(true);
        m_exportBtn->setEnabled(true);
        m_exportBtn->setText("导出到U盘");
    }, Qt::BlockingQueuedConnection);
    connect(this, &ExplorerWidget::SignalImportEnd, this, [=](bool result, QString msg){
        if (result)
            Style::Instance()->messageBox->Show("提示", "导出升级包成功!");
        else
            Style::Instance()->messageBox->Show("提示", msg.isEmpty() ? "导出升级包失败!" : msg);
        m_importBtn->setEnabled(true);
        m_exportBtn->setEnabled(true);
        m_importBtn->setText("导入升级包");
        this->SetPath(m_basePath + "/Updater");
    }, Qt::BlockingQueuedConnection);
}

ExplorerWidget::~ExplorerWidget()
{
    if (m_thread)
    {
        m_thread->Wait();
        m_thread = NULL;
    }
}

void ExplorerWidget::InitWidget()
{
    m_pathEdit = Style::CreateLineEdit();
    m_pathEdit->setEnabled(false);
    m_pathEdit->setText("导出目录");

    m_objTable = Style::CreateTableWidget({"类型", "名称", "时间", "大小"});
    m_objTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    m_returnBtn = Style::CreateButton("返回上一层");
    m_delBtn = Style::CreateButton("删除选定项","","Del");
    m_importBtn = Style::CreateButton("导入升级包");
    m_exportBtn = Style::CreateButton("导出到U盘");

    auto btnGrid = new QHBoxLayout();
    btnGrid->addWidget(m_returnBtn, 0, Qt::AlignLeft);
    btnGrid->addWidget(m_delBtn, 0, Qt::AlignLeft);
    btnGrid->addStretch();
    btnGrid->addWidget(m_importBtn, 0, Qt::AlignRight);
    btnGrid->addWidget(m_exportBtn, 0, Qt::AlignRight);

    auto grid = new QVBoxLayout(this);
    grid->addWidget(m_pathEdit);
    grid->addWidget(m_objTable);
    grid->addLayout(btnGrid);

    this->hide();
}

void ExplorerWidget::OnImportClick()
{
    //检查U盘是否存在
    if (!Treasure::File::Exist("/dev/sda1"))
    {
        Style::Instance()->messageBox->Show("提示", "U盘设备不存在!");
        return;
    }

    m_exportBtn->setEnabled(false);
    m_importBtn->setEnabled(false);
    m_importBtn->setText("导入中...");

    auto func = [=]()
    {
        QString toPath = m_basePath + "/Updater/";
        QString srcPath = "/mnt/usbdisk/sda1/Updater/";

        if (!Treasure::Path::Exist(srcPath))
        {
            emit this->SignalImportEnd(false, "U盘中没有Updater目录");
            return;
        }

        trLogger->debug("ExplorerWidget::OnImportClick import [%s] to [%s]", srcPath.toStdString().c_str(), toPath.toStdString().c_str());

        bool check = Treasure::Path::Copy(srcPath, toPath);
        QThread::msleep(300);
        emit this->SignalImportEnd(check, "");
    };

    if (m_thread)
    {
        m_thread->Wait();
        m_thread = NULL;
    }
    m_thread = Treasure::ThreadPtr(Treasure::Thread::CreatThread(func));
}

void ExplorerWidget::OnExportClick()
{
    //检查U盘是否存在
    if (!Treasure::File::Exist("/dev/sda1"))
    {
        Style::Instance()->messageBox->Show("提示", "U盘设备不存在!");
        return;
    }

    m_importBtn->setEnabled(false);
    m_exportBtn->setEnabled(false);
    m_exportBtn->setText("导出中...");

    auto func = [=]()
    {
        //导出的目录
        QString curPath = m_curPath;
        QString basePath = "/mnt/usbdisk/sda1/exports/";
        QString srcPath = curPath;
        QString toPath = basePath + curPath.replace(m_basePath, "");
        trLogger->debug("ExplorerWidget::OnExportClick export [%s] to [%s]", srcPath.toStdString().c_str(), toPath.toStdString().c_str());

        bool check = Treasure::Path::Copy(srcPath, toPath);
        QThread::msleep(300);
        emit this->SignalExportEnd(check);
    };

    if (m_thread)
    {
        m_thread->Wait();
        m_thread = NULL;
    }
    m_thread = Treasure::ThreadPtr(Treasure::Thread::CreatThread(func));
}

void ExplorerWidget::OnTableItemClick(const QModelIndex &index)
{
    if (index.row() < 0)
        return;

    QString name = m_objTable->item(index.row(), 1)->text();
    QString path = m_curPath + '/' + name;
    if (QDir(path).exists())
        this->SetPath(path);
}

void ExplorerWidget::OnDelClick()
{
    auto msg = Style::Instance()->messageBox;
    auto array = m_objTable->selectedItems();
    if (array.size() <= 0)
    {
        msg->Show("提示", "请先选中一项!");
        return;
    }

    QString name = m_objTable->item(array[0]->row(), 1)->text();
    QString path = m_curPath + '/' + name;

    msg->Show("提示", QString("是否删除：%1!").arg(name), true);
    if (!msg->IsConfirm())
        return;

    Treasure::Path::Remove(path);
    this->SetPath(m_curPath);
}

void ExplorerWidget::OnReturnClick()
{
    if (m_basePath == m_curPath)
        return;
    auto path = m_curPath.left(m_curPath.lastIndexOf('/'));
    this->SetPath(path);
}

static QString FormatShowPath(QString basePath, QString curPath)
{
    QString outStr = "导出目录";
    if (basePath == curPath)
        return outStr;

    QString tmp = curPath.remove(0, basePath.length());
    auto array = tmp.split('/');
    foreach (auto item, array)
    {
        if (item.isEmpty())
            continue;
        outStr = outStr + " > " + item;
    }
    return outStr;
}

void ExplorerWidget::SetPath(QString path)
{
    m_curPath = path;
    m_pathEdit->setText(FormatShowPath(m_basePath, m_curPath));

    auto objArray = Treasure::Path::GetEntryInfos(path);
    m_objTable->setRowCount(objArray.size());
    for (int i = 0; i < objArray.size(); i++)
    {
        bool isFile  = objArray[i].isFile();
        QString name = objArray[i].fileName();
        QString time = objArray[i].lastModified().toString("yyyy-MM-dd hh:mm:ss");
        QString size = "";
        if (isFile)
        {
            float num = Treasure::Math::UnitConver(objArray[i].size(), Treasure::Math::Unit::Byte, Treasure::Math::Unit::KB);
            size = QString("%1 KB").arg(num);
        }

        Style::InsertTableWidgetItem(m_objTable, i, 0, "", isFile ? "" : "dir.svg");
        Style::InsertTableWidgetItem(m_objTable, i, 1, name);
        Style::InsertTableWidgetItem(m_objTable, i, 2, time);
        Style::InsertTableWidgetItem(m_objTable, i, 3, size);
    }
}

void ExplorerWidget::Show(QString path)
{
    m_basePath = path.replace("\\", "/");
    this->SetPath(m_basePath);
    this->show();
}
