#ifndef PRINTERMANAGER_H
#define PRINTERMANAGER_H

#include "PrinterPlugin/Printer.h"
#include <memory>

namespace PrinterSpace
{

class PrinterManager
{
public:
    ~PrinterManager();
    static PrinterManager* Instance();
    bool Init();
    void Uninit();
    void LoadPlugin();
    void SetMeasureDataPrinter(Printer* printer);
    void SetCalibrateDataPrinter(Printer* printer);
    Printer* GetMeasureDataPrinter(void);
    Printer* GetCalibrateDataPrinter(void);

private:
    static std::unique_ptr<PrinterManager> m_instance;
    Printer* m_measureDataPrinter;
    Printer* m_calibrateDataPrinter;
};

}
#endif // PRINTERMANAGER_H
