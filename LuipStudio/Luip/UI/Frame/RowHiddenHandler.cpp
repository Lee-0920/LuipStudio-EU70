
#include "RowHiddenHandler.h"
using namespace std;

namespace UI
{
unique_ptr<RowHiddenHandler> RowHiddenHandler::m_instance(nullptr);

RowHiddenHandler* RowHiddenHandler::Instance()
{
    if (!m_instance)
    {
        m_instance.reset(new RowHiddenHandler);
    }

    return m_instance.get();
}

void RowHiddenHandler::CreateRow(MQtableWidget *table, TableWidgetInfo &info)
{
    if (info.userRow < info.perPageRow)     //用户实际行数小于整页行数9行
    {
        info.realRow = info.perPageRow;
    }
    else
    {
        info.realRow = info.userRow;
    }

    if (info.hiddenRow > 0) //总隐藏行数大于0
    {
        table->setRowCount(info.realRow + info.hiddenRow); //比实际行数增加总隐藏行数，用于隐藏后补空，凑够整页
    }
    else
    {
        table->setRowCount(info.realRow);
    }
}

void RowHiddenHandler::CreateRow(QTableWidget *table, TableWidgetInfo &info)
{
    if (info.userRow < info.perPageRow)     //用户实际行数小于整页行数9行
    {
        info.realRow = info.perPageRow;
    }
    else
    {
        info.realRow = info.userRow;
    }

    if (info.hiddenRow > 0) //总隐藏行数大于0
    {
        table->setRowCount(info.realRow + info.hiddenRow); //比实际行数增加总隐藏行数，用于隐藏后补空，凑够整页
    }
    else
    {
        table->setRowCount(info.realRow);
    }
}

void RowHiddenHandler::AdjustRowHidden(MQtableWidget *table, TableWidgetInfo &info)
{
    if (info.hiddenRow != 0)
    {
        int hiddenRowCnt = 0;
        for(int i = 0;i < info.realRow; i++)//行
        {
            if (table->isRowHidden(i))  // 检查哪一行被隐藏
            {
                hiddenRowCnt++;                         // 统计被隐藏的行数
            }
        }

        if (hiddenRowCnt > 0)
        {
            if (info.realRow - hiddenRowCnt < info.perPageRow)       // 隐藏的行数小于完整页行数时
            {
                int inShowRow = 0;
                inShowRow = info.perPageRow - (info.realRow - hiddenRowCnt);     // 计算要组成完整页行数，需增加的行数

                for(int i = info.realRow; i < info.realRow + inShowRow; i++)//行
                {
                    table->setRowHidden(i, false);     // 显示需增加的行数
                }

                for(int i = info.realRow + inShowRow; i < table->rowCount(); i++)//行
                {
                    table->setRowHidden(i, true);      // 隐藏不行需增加的行数
                }
            }
            else
            {
                for(int i = info.realRow; i < table->rowCount(); i++)//行
                {
                    table->setRowHidden(i, true);      //隐藏所有增加的行数
                }
            }
        }
        else
        {
            for(int i = info.realRow; i < table->rowCount(); i++)//行
            {
                table->setRowHidden(i, true);      //隐藏所有增加的行数
            }
        }
    }
}

void RowHiddenHandler::AdjustRowHidden(QTableWidget *table, TableWidgetInfo &info)
{
    if (info.hiddenRow != 0)
    {
        int hiddenRowCnt = 0;
        for(int i = 0;i < info.realRow; i++)//行
        {
            if (table->isRowHidden(i))  // 检查哪一行被隐藏
            {
                hiddenRowCnt++;                         // 统计被隐藏的行数
            }
        }

        if (hiddenRowCnt > 0)
        {
            if (info.realRow - hiddenRowCnt < info.perPageRow)       // 隐藏的行数小于完整页行数时
            {
                int inShowRow = 0;
                inShowRow = info.perPageRow - (info.realRow - hiddenRowCnt);     // 计算要组成完整页行数，需增加的行数

                for(int i = info.realRow; i < info.realRow + inShowRow; i++)//行
                {
                    table->setRowHidden(i, false);     // 显示需增加的行数
                }

                for(int i = info.realRow + inShowRow; i < table->rowCount(); i++)//行
                {
                    table->setRowHidden(i, true);      // 隐藏不行需增加的行数
                }
            }
            else
            {
                for(int i = info.realRow; i < table->rowCount(); i++)//行
                {
                    table->setRowHidden(i, true);      //隐藏所有增加的行数
                }
            }
        }
        else
        {
            for(int i = info.realRow; i < table->rowCount(); i++)//行
            {
                table->setRowHidden(i, true);      //隐藏所有增加的行数
            }
        }
    }
}

}
