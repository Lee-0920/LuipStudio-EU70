setting.ui.measureDataWindow =
{
    measureData =
    {
        {
            name = "测量数据",
            resultDataname = setting.resultFileInfo.measureRecordFile[1].name,
        },
    },
    waveData =
    {
        {
            name = "历史趋势",
            resultDataname = setting.resultFileInfo.measureRecordFile[1].name,
            profileTableName = "config.measureParam",
            rangeTableName = "config.system.rangeViewMap",
        },
    },
    calibrateCurve =
    {
        {
            name = "校准曲线",
            resultDataname = setting.resultFileInfo.calibrateRecordFile[1].name,
            profileTableName = "config.measureParam",
            curveParamName = "config.measureParam.curveParam[1]",--此表必须是profileTableName表中的子表
            profileFileName = "MeasureParamConfig.ls",
        },
    },
}
return setting.ui.measureDataWindow
