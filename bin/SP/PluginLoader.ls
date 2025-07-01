--[[
 * @brief 驱动器相关文件载入的条目。
--]]

package.path = [[?;?.lua;?.ls;?;./SP/Common/?.ls;./SP/Controller/?.ls;]]
require("ExceptionHandler")
require("ReagentRemainManager")
require("MaterialLifeManager")
require("PeristalticPump")
require("LCPeristalticPump")
require("Operator")
require("ControllerCreater")


CreateControllers()     -- 创建各个驱动控制器对象，加入全局表
CreatePumps()           -- 创建各个液路的泵对象
CreateOperator() 	-- 创建各个液路、温度、信号操作方法的泵对象

if config.system.printer.enable == true then
    setting.ui.measureDataPrint.measure.printer = GP80Printer.new()
    PrinterManager.Instance():SetMeasureDataPrinter(setting.ui.measureDataPrint.measure.printer)
end
