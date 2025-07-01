#ifndef UI_FRAME_ROWHIDDENHANDLER_H
#define UI_FRAME_ROWHIDDENHANDLER_H

#include <memory>
#include <QWidget>
#include <QTableWidget>
#include "MQtableWidget.h"

namespace UI
{

struct TableWidgetInfo
{
    int userRow;
    int hiddenRow;
    int perPageRow;
    int realRow;
};

class RowHiddenHandler
{

public:
    static RowHiddenHandler *Instance();
    void CreateRow(MQtableWidget * table, TableWidgetInfo& info);
    void CreateRow(QTableWidget * table, TableWidgetInfo& info);
    void AdjustRowHidden(MQtableWidget * table, TableWidgetInfo& info);
    void AdjustRowHidden(QTableWidget * table, TableWidgetInfo& info);

private:
    static std::unique_ptr<RowHiddenHandler> m_instance;

};

}

#endif // UI_FRAME_ROWHIDDENHANDLER_H
