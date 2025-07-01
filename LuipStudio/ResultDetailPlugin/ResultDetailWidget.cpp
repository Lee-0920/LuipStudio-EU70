#include "ResultDetailWidget.h"
namespace ResultDetail
{
ResultDetailWidget::ResultDetailWidget( QWidget *parent):
    QWidget(parent)
{

}

void ResultDetailWidget::SetRecordData(ResultData::RecordData recordData)
{
    m_recordData = recordData;
}

void ResultDetailWidget::Show(QWidget *parent, int roleType)
{
    (void)parent;
    (void)roleType;
}

bool ResultDetailWidget::UpdateRecord()
{
    return true;
}

void ResultDetailWidget::SetResultInfo(QString info)
{
    (void)info;
}
}
