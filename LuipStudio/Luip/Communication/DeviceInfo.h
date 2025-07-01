#if !defined(CONTROLNET_STACK_DEVICEINFO_H)
#define CONTROLNET_STACK_DEVICEINFO_H

#include "ControllerPlugin/API/Code/DeviceInfoInterface.h"
#include "Communication/eCek/Common/Types.h"
#include "Communication/eCek/DNCP/App/DscpDevice.h"

void DeviceInfo_GetType(DscpDevice* dscp, Byte* data, Uint16 len);
void DeviceInfo_SetType(DscpDevice* dscp, Byte* data, Uint16 len);
void DeviceInfo_GetModel(DscpDevice* dscp, Byte* data, Uint16 len);
void DeviceInfo_SetModel(DscpDevice* dscp, Byte* data, Uint16 len);
void DeviceInfo_GetSn(DscpDevice* dscp, Byte* data, Uint16 len);
void DeviceInfo_SetSn(DscpDevice* dscp, Byte* data, Uint16 len);
void DeviceInfo_GetManuFacturer(DscpDevice* dscp, Byte* data, Uint16 len);
void DeviceInfo_SetManuFacturer(DscpDevice* dscp, Byte* data, Uint16 len);
void DeviceInfo_GetManufDate(DscpDevice* dscp, Byte* data, Uint16 len);
void DeviceInfo_SetManufDate(DscpDevice* dscp, Byte* data, Uint16 len);
void DeviceInfo_GetSoftwareVersion(DscpDevice* dscp, Byte* data, Uint16 len);
void DeviceInfo_GetSoftwareLable(DscpDevice* dscp, Byte* data, Uint16 len);
void DeviceInfo_GetHardwareVersion(DscpDevice* dscp, Byte* data, Uint16 len);

#define CMD_TABLE_DEVICE_INFO \
    DSCP_CMD_ENTRY(DSCP_CMD_DII_GET_TYPE, DeviceInfo_GetType), \
    DSCP_CMD_ENTRY(DSCP_CMD_DII_SET_TYPE, DeviceInfo_SetType), \
    DSCP_CMD_ENTRY(DSCP_CMD_DII_GET_MODEL, DeviceInfo_GetModel), \
    DSCP_CMD_ENTRY(DSCP_CMD_DII_SET_MODEL, DeviceInfo_SetModel), \
    DSCP_CMD_ENTRY(DSCP_CMD_DII_GET_SN, DeviceInfo_GetSn), \
    DSCP_CMD_ENTRY(DSCP_CMD_DII_SET_SN, DeviceInfo_SetSn), \
    DSCP_CMD_ENTRY(DSCP_CMD_DII_GET_MANUFACTURER, DeviceInfo_GetManuFacturer), \
    DSCP_CMD_ENTRY(DSCP_CMD_DII_SET_MANUFACTURER, DeviceInfo_SetManuFacturer), \
    DSCP_CMD_ENTRY(DSCP_CMD_DII_GET_MANUF_DATE, DeviceInfo_GetManufDate), \
    DSCP_CMD_ENTRY(DSCP_CMD_DII_SET_MANUF_DATE, DeviceInfo_SetManufDate), \
    DSCP_CMD_ENTRY(DSCP_CMD_DII_GET_SOFTWARE_VERSION, DeviceInfo_GetSoftwareVersion), \
    DSCP_CMD_ENTRY(DSCP_CMD_DII_GET_SOFTWARE_LABEL, DeviceInfo_GetSoftwareLable), \
    DSCP_CMD_ENTRY(DSCP_CMD_DII_GET_HARDWARE_VERSION, DeviceInfo_GetHardwareVersion)

#endif // CONTROLNET_STACK_DEVICEINFO_H
