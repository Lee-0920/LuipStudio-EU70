#ifndef UI_FRAME_MeasureResultPanel_H
#define UI_FRAME_MeasureResultPanel_H


#include <QWidget>
#include <QVBoxLayout>
#include <QVector>
#include <QString>
#include <QPushButton>
#include "ResultIterm.h"

namespace UI
{
    struct TargetIterm
    {
        QString name;
        ResultIterm * iterm;
    };

class MeasureResultPanel : public QWidget
{
    Q_OBJECT
public:
    explicit MeasureResultPanel( QList<QString> targets, QString resultType, QString unitStr, QWidget *parent = 0);
    ~MeasureResultPanel();

    void SetResultInfo(QString target, QString result, QString dateTime, QString resultType);
    void SetResultInfoTC(QString target, QString result);
    void SetResultInfoIC(QString target, QString result);
    void SetProformaResultInfo(QString result);
    void ShowFatalWarning();

    QLabel *estimateResultTimeLable;    //校准时间
    QPushButton *fatalButton;   //严重故障停机按钮

private:
    QVBoxLayout *measureResultLayout;  //测量面板布局
    QVector <TargetIterm> m_resultMap;

    int m_targetNum;
    QLabel *m_resultTypeLabel;  // 结果类型
    QLabel *m_proformaResultLabel;    // 预估数据

private slots:
    void SlotFatalButton();
};

}
#endif // UI_FRAME_MeasureResultPanel_H
