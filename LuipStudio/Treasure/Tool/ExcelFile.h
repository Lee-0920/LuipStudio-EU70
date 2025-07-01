/**
 * @brief Excel表格操作
 * @author huangyue
 * @date 2021-02-04
 * @note 1、需要先给QT安装上QtXlsx模块：https://blog.csdn.net/c3060911030/article/details/51560239
 *       2、工程pro文件增加模块定义xlsx
 *
 *       注意事项：请按照顺序调用接口
 *       1、SetTiltles （可选）
 *       2、SetColumnHeaders （可选）
 *       3、SetColumnWidths （可选）
 *       4、SetItem/SetRowItems/SetRows
 *       5、Save
 *
 *       说明：
 *       a、支持无title、一行或多行title
 *       b、支持无列名、一行列名、两行列名（主列名+子列名，会自动合并单元格）
 *       c、默认列名样式和单元格样式，如果想自定义，可以直接设置 headerFormat 和 itemFormat
 *       d、为了展现好看的效果，请自己设置每一列的列宽
 *
 *       样式：
 *       ----------------------------------------------
 *       |                  title0                    |
 *       ----------------------------------------------
 *       |       header0       |        header1       |
 *       ----------------------------------------------
 *       | subhead0 | subhead1 |  subhead0 | subhead1 |
 *       ----------------------------------------------
 *       |          |          |           |          |
 *       ----------------------------------------------
 *          ...         ...         ...         ...
 *       ----------------------------------------------
 *
 */
#ifndef __TREASURE_TOOL_EXCEL__
#define __TREASURE_TOOL_EXCEL__

#include <QObject>
#include <QVector>
#include <QFileInfo>

#include <QtXlsx/xlsxdocument.h>
#include <QtXlsx/xlsxformat.h>

namespace Treasure
{

class ExcelFile
{
public:
    //两层列头
    struct ColumnHeader
    {
        ColumnHeader(QString name = "", QStringList subNames = {}):name(name),subNames(subNames){}
        QString name;           //大列名（第一行，合并表格）
        QStringList subNames;   //子列名
    };

public:
    ExcelFile()
    {
        m_columnHeaderRowNum = 0;
        m_titleRowNum = 0;

        //设置默认的表头样式
        headerFormat.setFontBold(true);
        headerFormat.setFontColor(QColor(Qt::white));
        headerFormat.setFontSize(14);
        headerFormat.setBorderStyle(QXlsx::Format::BorderThin);
        headerFormat.setBorderColor(QColor("#d4d4d4"));
        headerFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
        headerFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);
        headerFormat.setPatternBackgroundColor(QColor("#008eca"));

        //设置title的表头样式
        titleFormat = headerFormat;
        titleFormat.setFontSize(16);

        //设置默认的内容项样式
        itemFormat.setFontBold(false);
        itemFormat.setFontColor(QColor(Qt::black));
        itemFormat.setFontSize(12);
        itemFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    }

    /**
     * @brief 设置多行title
     */
    void SetTiltles(QStringList titles)
    {
        m_titleRowNum = titles.size();

        for (int i = 0; i < m_titleRowNum; i++)
        {
            QString key = GetExcelItemKey(i, 0);
            m_xlsx.write(key, titles[i], titleFormat);
        }
    }

    /**
     * @brief 设置列名（只有一行表头）
     * @param names 列名数组
     */
    void SetColumnHeaders(QStringList names)
    {
        //列头的起始行需要依据title的行数而定
        int columnHeadStartIndex = m_titleRowNum;

        //设置列头
        for(int i = 0; i < names.size(); i++)
        {
            QString key = GetExcelItemKey(columnHeadStartIndex, i);
            m_xlsx.write(key, names[i], headerFormat);
        }
        m_columnHeaderRowNum = 1;

        //合并title
        int sumOfColumn = names.size();
        MergeTitleRow(sumOfColumn);
    }


    /**
     * @brief 设置列名（两行表头）
     * @param headers 列头数组
     */
    void SetColumnHeaders(QVector<ColumnHeader> headers)
    {
        //列头的起始行需要依据title的行数而定
        int columnHeadStartIndex = m_titleRowNum;

        int columnOffset = 0;
        foreach (ColumnHeader header, headers)
        {
            //先填写第一行的内容
            QString key = GetExcelItemKey(columnHeadStartIndex, columnOffset);
            m_xlsx.write(key, header.name);

            int subNum = header.subNames.size();
            //如果此列头没有子列，则需要合并上下两行
            if (subNum <= 0)
            {
                QString mergeKey = GetExcelMergeRowKey(columnOffset, 0, 1);
                m_xlsx.mergeCells(mergeKey, headerFormat);

                columnOffset++;
            }
            //如果只有一个子列，那就上下两格填值
            if (subNum == 1)
            {
                key = GetExcelItemKey(columnHeadStartIndex + 1, columnOffset);
                m_xlsx.write(key, header.subNames[0]);

                columnOffset++;
            }
            //如果第二行有多列，则需要合并第一行
            else
            {
                QString mergeKey = GetExcelMergeColumnKey(0, columnOffset, columnOffset + subNum - 1);
                m_xlsx.mergeCells(mergeKey, headerFormat);

                for(int i = 0; i < subNum; i++)
                {
                    key = GetExcelItemKey(1, columnOffset + i);
                    m_xlsx.write(key, header.subNames[i], headerFormat);
                }
                columnOffset += subNum;
            }
        }

        m_columnHeaderRowNum = 2;

        //合并title
        int sumOfColumn = columnOffset;
        MergeTitleRow(sumOfColumn);
    }


    /**
     * @brief 设置列宽
     * @param widths 列宽数组
     */
    void SetColumnWidths(QVector<int> widths)
    {
        for(int i = 0; i < widths.size(); i++)
        {
            //在Excel中列的下标从1开始
            m_xlsx.setColumnWidth(i + 1, widths[i]);
        }
    }


    /**
     * @brief 设置项内容
     * @param rowIndex 行号
     * @param columnIndex 列号
     * @param content 内容
     */
    void SetItem(int rowIndex, int columnIndex, QString content)
    {
        if (rowIndex < 0 || columnIndex < 0)
            return;

        //列序需要增加表头的行数
        rowIndex += m_columnHeaderRowNum + m_titleRowNum;
        QString key = GetExcelItemKey(rowIndex, columnIndex);
        m_xlsx.write(key, content, itemFormat);
    }

    /**
     * @brief 设置一行内容
     * @param rowIndex 行号
     * @param rowContent 行内容
     */
    void SetRowItems(int rowIndex, QStringList rowContent)
    {
        for(int i = 0; i < rowContent.size(); i++)
        {
            SetItem(rowIndex, i, rowContent[i]);
        }
    }


    /**
     * @brief 设置多行内容
     * @param rowStartIndex 开始行号
     * @param rowsContent 行内容集合
     */
    void SetRows(int rowStartIndex, QVector<QStringList> rowsContent)
    {
        for(int i = 0; i < rowsContent.size(); i++)
        {
            SetRowItems(rowStartIndex + i, rowsContent[i]);
        }
    }


    /**
     * @brief 保存文件
     * @param path 文件路径
     */
    bool Save(QString path)
    {
        //删除旧文件
        QFileInfo file(path);
        if(file.exists())
            remove(path.toStdString().c_str());

        return m_xlsx.saveAs(path);
    }

private:
    /**
     * @brief 数字转字母
     * @param num 0到25表示A-Z
     */
    QString NumberToWord(int num)
    {
        if (num < 0 || num > 25)
            return "";
        char word[2];
        word[0] = 'A' + num;
        word[1] = '\0';
        return QString(word);
    }


    /**
     * @brief 获取Excel的空格Key，比如A1
     * @param rowIndex 行号
     * @param columnIndex 列号
     */
    QString GetExcelItemKey(int rowIndex, int columnIndex)
    {
        if (rowIndex < 0 || columnIndex < 0)
            return "";

        //列表达：A-Z表示第0~25列，26及以上用AA~AZ
        int first = columnIndex / 26;
        int second = columnIndex % 26;
        QString columnWord = NumberToWord(first - 1) + NumberToWord(second);

        //行表达：从1开始
        QString rowWord = QString::number(rowIndex + 1);
        return columnWord + rowWord;
    }


    /**
     * @brief 获取Excel合并一行中多列表格
     * @param rowIndex 行号
     * @param columnStartIndex 开始列号
     * @param columnEndIndex 结束列号
     */
    QString GetExcelMergeColumnKey(int rowIndex, int columnStartIndex, int columnEndIndex)
    {
        QString fromWord = GetExcelItemKey(rowIndex, columnStartIndex);
        QString toWord = GetExcelItemKey(rowIndex, columnEndIndex);
        return fromWord + ":" + toWord;
    }


    /**
     * @brief 获取Excel合并一列中多行表格
     * @param columnIndex 行号
     * @param rowStartIndex 开始列号
     * @param rowEndIndex 结束列号
     */
    QString GetExcelMergeRowKey(int columnIndex, int rowStartIndex, int rowEndIndex)
    {
        QString fromWord = GetExcelItemKey(rowStartIndex, columnIndex);
        QString toWord = GetExcelItemKey(rowEndIndex, columnIndex);
        return fromWord + ":" + toWord;
    }

    /**
     * @brief title行合并
     */
    void MergeTitleRow(int sumOfColumns)
    {
        if (sumOfColumns > 1)
        {
            for (int i = 0; i < m_titleRowNum; i++)
            {
                QString mergeKey = GetExcelMergeColumnKey(i, 0, sumOfColumns - 1);
                m_xlsx.mergeCells(mergeKey, titleFormat);
            }
        }
    }


public:
    QXlsx::Format titleFormat;      //title样式
    QXlsx::Format headerFormat;     //表头样式
    QXlsx::Format itemFormat;       //内容项样式

private:
    int m_columnHeaderRowNum;     //列名所占用的行数
    int m_titleRowNum;            //title所占的行数
    QXlsx::Document m_xlsx;

};


}
#endif
