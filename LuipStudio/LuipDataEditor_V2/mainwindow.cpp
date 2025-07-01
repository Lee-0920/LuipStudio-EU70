#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ResultManager/ResultManager.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include "AboutDialog.h"
#include "FilterDialog.h"
#include "RecordDataPlugin/RecordData.h"
#include "LuaEngine/LuaEngine.h"
#include <QDateTime>

using namespace Result;
using namespace ResultData;
using namespace Lua;
using namespace OOLUA;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->currentSelectedIndex = -1;
    this->strFilePath = "";

    String name;
    LuaEngine* luaEngine = LuaEngine::Instance();
    Script* lua = luaEngine->GetEngine();
    lua_State * state = luaEngine->GetThreadState();
    luaEngine->GetLuaValue(state, "setting.resultFileInfo.measureRecordFile[1].name", name);
    m_resultFiles = ResultManager::Instance()->GetMeasureRecordFile(name);
    m_recordFields =  m_resultFiles->GetRecordFields();
//    qDebug()<<"Parameter Name = "<<QString::fromStdString(name);

    QStringList textList;
    RecordField recordField;
    m_recordFields->ResetFieldPos();
    while(this->m_recordFields->GetNextField(recordField))
    {
        String name = recordField.name;
        m_exportFields.push_back(name);

        String text = recordField.text;
        textList.push_back(text.c_str());
    }
    m_recordFields->ResetFieldPos();
    qDebug()<<textList;

    this->labelFilePath = new QLabel("未命名");
    this->statusBar()->addWidget(this->labelFilePath);
    this->labelResultsNum = new QLabel("总数：0");
    this->statusBar()->addPermanentWidget(this->labelResultsNum);
    this->labelCurrentSelected = new QLabel("当前：0");
    this->statusBar()->addPermanentWidget(this->labelCurrentSelected);
    this->labelRecordTimeFormat = new QLabel("时间：UTC");
    this->statusBar()->addPermanentWidget(this->labelRecordTimeFormat);

    // 记录视图初始化
    ui->recordView->horizontalHeader()->setDefaultSectionSize(80);
//    ui->recordView->horizontalHeader()->setFixedHeight(38); // 设置表头的高度
//    ui->recordView->setEditTriggers(QAbstractItemView::NoEditTriggers); // 将表格变为禁止编辑
    ui->recordView->setSelectionBehavior(QAbstractItemView::SelectRows); // 设置表格为整行选择
    ui->recordView->verticalHeader()->hide(); // 隐藏列表头

    this->recordModel = new QStandardItemModel();
    this->recordModel->setHorizontalHeaderLabels(textList);
    connect(this->recordModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onTableItemChanged(QStandardItem*)));

    ui->recordView->setModel(this->recordModel);

    // 设置列宽，setModel() 之后才生效
    ui->recordView->adjustSize();

    this->loadLocalFile();
    this->updateRecordView();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAbout()
{
    AboutDialog dialog;
    dialog.exec();
}

// 加载文件到内存
bool MainWindow::loadLocalFile()
{
    bool ret = false;

    m_resultFiles->MoveToFirst();
    while(m_resultFiles->GetRemainNum() > 0)
    {
        m_resultFiles->MoveNext();
        RecordData* recordData = new RecordData(m_recordFields->GetFieldsSize());
        recordData->SetData(m_resultFiles->GetCurrentRecord().GetData(), m_recordFields->GetFieldsSize());
        m_resultRecords.push_back(recordData);
    }
    m_resultFiles->MoveToFirst();

    return ret;
}

// 把内存数据保存到文件中
bool MainWindow::saveLocalFile()
{
    bool ret = false;

    m_resultFiles->ClearRecord();
    for (const auto& record : m_resultRecords)
    {
        m_resultFiles->AddRecord(*record);
    }
    m_resultFiles->Save();

    ret = true;
    return ret;
}

void MainWindow::updateRecordView()
{
    recordModel->removeRows(0, recordModel->rowCount());
    for (const auto& record : m_resultRecords)
    {
        recordModel->appendRow(createRecordRow(record));
    }
}

void MainWindow::updateTotalRecordNum()
{
    this->labelResultsNum->setText(QString("总数：%1").arg(m_resultRecords.size()));
}

void MainWindow::updateCurrentSelected()
{
    this->labelCurrentSelected->setText(QString("当前：%1").arg(currentSelectedIndex + 1));
}

void MainWindow::updateTimeFormat()
{
    if (this->resultTimeSpec == Qt::UTC)
        this->labelRecordTimeFormat->setText("时间：UTC");
    else if (this->resultTimeSpec == Qt::LocalTime)
        this->labelRecordTimeFormat->setText("时间：本地");
    else
        this->labelRecordTimeFormat->setText("时间：未知");
}

// 使用指定的数据创建表中的一行UI
QList<QStandardItem *> MainWindow::createRecordRow(const RecordData* record)
{
    QList<QStandardItem *> recordRow;
    QStandardItem * item;
    RecordData recordData = *record;

    for (std::vector<String>::iterator iter = m_exportFields.begin(); iter != m_exportFields.end(); ++iter)
    {
        RecordField resultfield;
        if (this->m_recordFields->GetField(*iter, resultfield))
        {
            QString dataStr;
            switch((FieldType)resultfield.type)
            {
                case FieldType::Bool:
                    {
                        bool ret;
                        recordData.GetFieldData(resultfield.start, ret);//获取结果文件的数据
                        dataStr = QString::number(ret);
                    }
                    break;
                case FieldType::Byte:
                    {
                        Byte ret;
                        recordData.GetFieldData(resultfield.start, ret);
                        dataStr = QString::number(ret);
                    }
                    break;
                case FieldType::Int:
                    {
                        int ret;
                        recordData.GetFieldData(resultfield.start, ret);
                        dataStr = QString::number(ret);
                    }
                    break;
                case FieldType::Float:
                    {
                        float ret;
                        recordData.GetFieldData(resultfield.start, ret);
                        dataStr = QString::number(ret);
                    }
                    break;
                case FieldType::Double:
                    {
                        double ret;
                        recordData.GetFieldData(resultfield.start, ret);
                        dataStr = QString::number(ret);
                    }
                    break;
                case FieldType::IntArray:
                    {
                        IntArray array(resultfield.size / sizeof(int));
                        recordData.GetIntArray(resultfield.start, array);
                    }
                    break;
                case FieldType::Time:
                    {
                        int ret;
                        recordData.GetFieldData(resultfield.start, ret);
                        dataStr = QDateTime::fromTime_t(ret).toString("yyyy-MM-dd hh:mm:ss");
                    }
                    break;
                case FieldType::Enum:
                    {
                        Byte ret;
                        String str;
                        recordData.GetFieldData(resultfield.start, ret);
                        str = resultfield.option.at(ret);
                        dataStr = QString::fromStdString(str);
                    }
                    break;
            }

            recordRow.push_back(item = new QStandardItem(dataStr));
                                            item->setTextAlignment(Qt::AlignCenter);
        }
    }

    return recordRow;
}

// 数据表格中有编辑，及时更新数据到内存中
void MainWindow::onTableItemChanged(QStandardItem *item)
{
     qDebug("Item (%d, %d) changed to: %s", item->row(), item->column(), qPrintable(item->text()));
    int rowIndex = item->row();
    RecordData* record = m_resultRecords[item->row()];
    record->ResetPos();

    QString text = item->text();

    String itStr = m_exportFields[item->column()];
    RecordField resultfield;
    if (this->m_recordFields->GetField(itStr, resultfield))
    {
        switch((FieldType)resultfield.type)
        {
            case FieldType::Bool:
                {
                    bool ret = text.toInt();
                    record->SetPos(resultfield.start);
                    record->PushBool(ret);
                }
                break;
            case FieldType::Byte:
                {
                    Byte ret = text.toInt();
                    record->SetPos(resultfield.start);
                    record->PushByte(ret);
                }
                break;
            case FieldType::Int:
                {
                    int ret = text.toInt();
                    record->SetPos(resultfield.start);
                    record->PushInt(ret);
                }
                break;
            case FieldType::Float:
                {
                    float ret = text.toFloat();
                    record->SetPos(resultfield.start);
                    record->PushFloat(ret);
                }
                break;
            case FieldType::Double:
                {
                    double ret= text.toDouble();
                    record->SetPos(resultfield.start);
                    record->PushDouble(ret);
                }
                break;
            case FieldType::IntArray:
                {
//                    IntArray array(resultfield.size / sizeof(int));
//                    recordData.GetIntArray(resultfield.start, array);
                }
                break;
            case FieldType::Time:
                {
                    QDateTime time = QDateTime::fromString(text, "yyyy-MM-dd hh:mm:ss");
                    record->SetPos(resultfield.start);
                    record->PushInt(time.toTime_t());
                }
                break;
            case FieldType::Enum:
                {
                    String str = text.toStdString();
                    Byte ret = 0;
                    for(ret = 0; ret < resultfield.option.size(); ret++)
                    {
                        if(str == resultfield.option[ret])
                        {
                            break;
                        }
                    }
                    record->SetPos(resultfield.start);
                    record->PushByte(ret);
                }
                break;
        }

    }

    // 立即更新该行记录到UI，给用户实时反馈
    recordModel->insertRow(rowIndex, createRecordRow(m_resultRecords[item->row()]));
    recordModel->removeRow(rowIndex + 1);
}

void MainWindow::onSave()
{
    this->saveLocalFile();
}

void MainWindow::onCopy()
{
    QModelIndexList list = ui->recordView->selectionModel()->selectedRows();
    if (!list.empty())
    {
        int idxSelected = list.last().row();
//        qDebug("Last Selected: %d", idxSelected);
        RecordData* record = new RecordData(m_recordFields->GetFieldsSize());
        record->SetData(m_resultRecords[idxSelected]->GetData(), m_recordFields->GetFieldsSize());
        m_resultRecords.insert(m_resultRecords.begin() + idxSelected + 1, record);
        this->recordModel->insertRow(idxSelected + 1, createRecordRow(record));
        ui->recordView->selectRow(idxSelected + 1);
        updateTotalRecordNum();
    }
}

void MainWindow::onInsert()
{
    QModelIndexList list = ui->recordView->selectionModel()->selectedRows();
    RecordData* recordData = new RecordData(m_recordFields->GetFieldsSize());

    if (!list.empty())
    {
        int idxSelected = list.last().row();
//        qDebug("Last Selected: %d", idxSelected);
        m_resultRecords.insert(m_resultRecords.begin() + idxSelected, recordData);
        this->recordModel->insertRow(idxSelected, createRecordRow(recordData));
        ui->recordView->selectRow(idxSelected);
        updateTotalRecordNum();
    }
    else
    {
        m_resultRecords.push_back(recordData);
        this->recordModel->appendRow(createRecordRow(recordData));
        ui->recordView->selectRow(0);
        updateTotalRecordNum();
    }
}

void MainWindow::onDelete()
{
//    // 必须从后往前删，不然会混乱
    std::vector<int> selecteds;
    for (const QModelIndex& index : ui->recordView->selectionModel()->selectedRows())
    {
        selecteds.push_back(index.row());
    }
    std::sort(selecteds.begin(), selecteds.end(), std::greater<int>());

    for (int i : selecteds)
    {
        recordModel->removeRow(i);
        RecordData* recordData = m_resultRecords[i];
        m_resultRecords.erase(m_resultRecords.begin() + i);
        delete recordData;
    }
    updateTotalRecordNum();

    updateRecordView();
}

void MainWindow::onCurrentChange(const QModelIndex &index)
{
    currentSelectedIndex = index.row();
    updateCurrentSelected();
}
