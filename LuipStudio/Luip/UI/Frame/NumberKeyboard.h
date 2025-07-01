/******************** (C) COPYRIGHT 2011 LABSUN ********************************
* File Name          : numberKeyboard.h
* Author             : WZ
* Version            : V1.0.1
* Date               : 2015/06/25
* Description        : Header for numberKeyboard.cpp file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, LABSUN SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#ifndef UI_FRAME_NUMBERKEYBOARD_H
#define UI_FRAME_NUMBERKEYBOARD_H

#include <QtWidgets>

class CNumberKeyboard : public QWidget
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
        KEY_POSITIVE,
        KEY_NEGATIVE,
        KEY_DOT,
        KEY_COLON,
        KEY_BACKSPAC,
        KEY_QUIT,
        MAX_KEY
    };

public:
    explicit CNumberKeyboard(QWidget * parent = NULL);
    ~CNumberKeyboard(void);
    static CNumberKeyboard *Instance();
    bool eventFilter(QObject *traget, QEvent *event);
    void myMove(int iFocusWidgetX, int iFocusWidgetY, int iFocusWidgetWidth, int iFocusWidgetHeight);

private slots:
    void slotKeyButtonReleased(void);

protected:
    QSize sizeHint(void) const;

private:
    int keyButtonWidth;         // 按钮的宽度
    int keyButtonHeight;        // 按钮的高度
    int rowSpace;               // 列间距
    int columnSpace;            // 行间距

    QVector<QPushButton *> keyButtonVector;
    QVector<QString> keyCodeVector;
};

#endif // UI_FRAME_NUMBERKEYBOARD_H
