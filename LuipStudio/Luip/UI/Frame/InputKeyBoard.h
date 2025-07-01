#ifndef INPUTKEYBOARD_H
#define INPUTKEYBOARD_H

#include <QFile>
#include <QDialog>
#include <QMultiMap>
#include <QRegExp>
#include <QEvent>
#include <QList>
#include <QPushButton>
#include <QtWidgets>
#include <memory>

class InputKeyBoard : public QDialog
{
    Q_OBJECT
    enum
    {
        KEY_0 = 0,
        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,
        KEY_numberAt,           // @
        KEY_numberColon,        // :
        KEY_numberPlus,         // +
        KEY_numberMinus,        // -
        KEY_numberMultiply,     // *
        KEY_numberDivide,       // /
        KEY_numberBackSpace,    // 退格
        KEY_numberSpace,        // 空格
        KEY_numberPeriod,       // .
        KEY_numberSymbol,       // 符号
        KEY_numberReturn,       // 返回
        KEY_numberEnter,        // 换行

        KEY_a,
        KEY_b,
        KEY_c,
        KEY_d,
        KEY_e,
        KEY_f,
        KEY_g,
        KEY_h,
        KEY_i,
        KEY_j,
        KEY_k,
        KEY_l,
        KEY_m,
        KEY_n,
        KEY_o,
        KEY_p,
        KEY_q,
        KEY_r,
        KEY_s,
        KEY_t,
        KEY_u,
        KEY_v,
        KEY_w,
        KEY_x,
        KEY_y,
        KEY_z,
        KEY_en,                     // 中英
        KEY_Enter,                  // 回车
        KEY_number,                 // 数字123
        KEY_CapsLock,               // 大小写锁定
        KEY_Space,                  // 空格
        KEY_Comma,                  // ,
        KEY_Period,                 // .
        KEY_prePage,                // 前一页
        KEY_nextPage,               // 下一页
        KEY_symbol,                 // 符号

        KEY_BackSpace,              // 退格
        KEY_Hanzi1,                 // 汉字选择1
        KEY_Hanzi2,                 // 汉字选择2
        KEY_Hanzi3,                 // 汉字选择3
        KEY_Hanzi4,                 // 汉字选择4
        KEY_Hanzi5,                 // 汉字选择5
        KEY_Hanzi6,                 // 汉字选择6

        KEY_SymbolAsciiTilde,       // ~
        KEY_SymbolQuoteLeft,        // `
        KEY_SymbolExclam,           // "!"
        KEY_SymbolAt,               // @
        KEY_SymbolNumberSign,       // #
        KEY_SymbolDollar,           // $
        KEY_SymbolPercent,          // %
        KEY_SymbolAsciiCircum,      // ^
        KEY_SymbolAmpersand,        // "&"
        KEY_SymbolAsterisk,         // *
        KEY_SymbolParentLeft,       // (
        KEY_SymbolParentRight,      // )
        KEY_SymbolUnderscore,       // _
        KEY_SymbolMinus,            // -
        KEY_SymbolPlus,             // +
        KEY_SymbolEqual,            // =
        KEY_SymbolBraceLeft,        // {
        KEY_SymbolBraceRight,       // }
        KEY_SymbolBracketLeft,      // [
        KEY_SymbolBracketRight,     // ]
        KEY_SymbolBar,              // |
        KEY_SymbolBackslash,        // \"\"
        KEY_SymbolColon,            // ":"
        KEY_SymbolSeicolon,         // ;
        KEY_SymbolQuoteDbl,         // "
        KEY_SymbolApostrophe,       // '
        KEY_SymbolLess,             // <
        KEY_SymbolGreater,          // >
        KEY_SymbolComma,            // ,
        KEY_SymbolBackSpace,        // BackSpace
        KEY_SymbolPeriod,           // .
        KEY_SymbolPeriodZh,         // 。
        KEY_SymbolQuestion,         // ?
        KEY_SymbolSlash,            // /
        KEY_SymbolLessZh,           // 《
        KEY_SymbolGreaterZh,        // 》
        KEY_Symbolx1,               // 预留1
        KEY_Symbolx2,               // 预留2
        KEY_Symbolx3,               // 预留3
        KEY_SymbolReturn,           // 返回

        KEY_QUIT,                   // 退出

        MAX_KEY
    };

public:
    ~InputKeyBoard();
    void matching(QString field);
    void initDialog();
    void setInputMethod();
    void changePage(int index);
    void prePage();
    void nextPage();
    ///static InputKeyBoard *getInstance(QWidget * parent = 0);
    static InputKeyBoard *Instance();

///private:
    InputKeyBoard(QWidget * parent = 0);
    void myMove(int iFocusWidgetX, int iFocusWidgetY, int iFocusWidgetWidth, int iFocusWidgetHeight);

protected:
    ///void showEvent(QShowEvent * event);
    bool eventFilter(QObject *target, QEvent *event);

signals:
    void signalSendString(QString str);

private slots:
    void slotKeyButtonReleased(void);

private:
    static std::unique_ptr<InputKeyBoard> m_instance;

    QKeyEvent *event;
    QList<QString> pinyinList;

    QWidget *charWidget;                    // 字符键盘
    QWidget *numberWidget;                  // 数字键盘
    QWidget *symbolWidget;                  // 标点符号键盘
    QWidget *topRowWidget;                  // 汉字选择界面
    QLineEdit *inputLineEdit;               // 汉字输入时拼音显示框
    QVector<QPushButton *> keyButtonVector; // 按键
    QVector<QString> keyCodeVector;         // 按键码
    int keyButtonWidth;                     // 按钮的宽度
    int keyButtonHeight;                    // 按钮的高度
    int rowSpace;                           // 列间距
    int columnSpace;                        // 行间距
    int pageCount;                          // 第几页
    int topRowHeight;                       // 汉字选择界面高度

    bool isLower;                           // 小写标志
    bool isChinese;                         // 中文标志
    bool isNumber;                          // 数字输入状态标志
    bool isSymbol;                          // 标点符号输入状态标志
};

#endif // INPUTKEYBOARD_H
