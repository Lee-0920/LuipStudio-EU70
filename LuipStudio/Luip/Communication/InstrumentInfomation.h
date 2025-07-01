#if !defined(CONTROLNET_STACK_INSTRUMENTINFOINSTACK_H)
#define CONTROLNET_STACK_INSTRUMENTINFOINSTACK_H

#include "ControllerPlugin/API/Code/InstrumentInfoInterface.h"
#include "Communication/eCek/Common/Types.h"
#include "Communication/eCek/DNCP/App/DscpDevice.h"

void InstrumentInfomation_GetType(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_SetType(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_GetModel(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_SetModel(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_GetSn(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_SetSn(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_GetUUID(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_SetUUID(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_GetManuFacturer(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_SetManuFacturer(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_GetManufDate(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_SetManufDate(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_GetSystemTime(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_SetSystemTime(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_GetSoftwareVersion(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_GetSoftwareLable(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_GetPlatFormVersion(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_GetPlatFormLable(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_GetInstrumentName(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_GetMeasureParameter(DscpDevice* dscp, Byte* data, Uint16 len);
void InstrumentInfomation_GetMeasureParameterCode(DscpDevice* dscp, Byte* data, Uint16 len);

#define CMD_TABLE_INSTRUMENT_INFO \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_TYPE, InstrumentInfomation_GetType), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_SET_TYPE, InstrumentInfomation_SetType), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_MODEL, InstrumentInfomation_GetModel), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_SET_MODEL, InstrumentInfomation_SetModel), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_SN, InstrumentInfomation_GetSn), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_SET_SN, InstrumentInfomation_SetSn), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_UUID, InstrumentInfomation_GetUUID), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_SET_UUID, InstrumentInfomation_SetUUID), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_MANUFACTURER, InstrumentInfomation_GetManuFacturer), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_SET_MANUFACTURER, InstrumentInfomation_SetManuFacturer), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_MANUF_DATE, InstrumentInfomation_GetManufDate), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_SET_MANUF_DATE, InstrumentInfomation_SetManufDate), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_SYSTEM_TIME, InstrumentInfomation_GetSystemTime), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_SET_SYSTEM_TIME, InstrumentInfomation_SetSystemTime), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_SOFTWARE_VERSION, InstrumentInfomation_GetSoftwareVersion), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_SOFTWARE_LABEL, InstrumentInfomation_GetSoftwareLable), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_PLATFORM_VERSION, InstrumentInfomation_GetPlatFormVersion), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_PLATFORM_LABEL, InstrumentInfomation_GetPlatFormLable), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_INSTRUMENT_NAME, InstrumentInfomation_GetInstrumentName), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_MEASURE_PARAMETER, InstrumentInfomation_GetMeasureParameter), \
    DSCP_CMD_ENTRY(DSCP_CMD_III_GET_MEASURE_PARAMETER_CODE, InstrumentInfomation_GetMeasureParameterCode)

#endif // CONTROLNET_STACK_INSTRUMENTINFOINSTACK_H
