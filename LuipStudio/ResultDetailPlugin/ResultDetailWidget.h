#ifndef RESULTDETAILWIDGET_H
#define RESULTDETAILWIDGET_H

#include <QWidget>
#include "RecordDataPlugin/RecordData.h"
#include "LuipShare.h"

namespace ResultDetail
{

class LUIP_SHARE ResultDetailWidget: public QWidget
{
public:
    ResultDetailWidget(QWidget *parent = 0);
    virtual void Show(QWidget *parent, int roleType = 0);
    void SetRecordData(ResultData::RecordData recordData);
    virtual bool UpdateRecord();
    virtual void SetResultInfo(QString info);

protected:
    ResultData::RecordData m_recordData;
};

}
#endif // RESULTDETAILWIDGET_H
