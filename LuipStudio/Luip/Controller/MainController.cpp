
#include "MainController.h"

using namespace std;
using namespace Controller::API;


namespace Controller
{

MainController::MainController(DscpAddress addr) :
        BaseController(addr), IInstrumentInfo(new InstrumentInfoInterface(addr))
{

}

MainController::~MainController()
{
    if (IInstrumentInfo)
        delete IInstrumentInfo;
}

/**
 * @brief 初始化 LiquidController 环境。
 */
bool MainController::Init()
{
    return true;
}

void MainController::OnReceive(DscpEventPtr event)
{
    (void)event;
}

void MainController::Stop()
{

}
}
