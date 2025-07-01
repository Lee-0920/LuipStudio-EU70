/**
 * @file
 * @brief 屏幕亮度控制。
 * @details
 * @version 1.0.0
 * @author xingfan
 * @date 2016/10/20
 */

#if !defined(SYSTEM_SCREENPROTECTION_BANKLIGHTCONTROL_H_)
#define SYSTEM_SCREENPROTECTION_BANKLIGHTCONTROL_H_


namespace System
{
namespace Screen
{


class BanklightControl
{
public:
    bool SetBrightness(unsigned char valve);
    bool SetPower(unsigned char valve);
    bool SetFbBlank(unsigned char mode);
};

}
}
#endif /* SYSTEM_SCREENPROTECTION_BANKLIGHTCONTROL_H_ */
