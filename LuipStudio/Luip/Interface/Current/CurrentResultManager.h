#ifndef CURRENTRESULTMANAGER_H
#define CURRENTRESULTMANAGER_H
#include <memory>
#include <list>
//#include "Interface/ExternalInterface.h"
#include "CurrentResultOutputer.h"

using std::list;

namespace Interface {
namespace Current {

enum OutputType
{
    sample = 0,
    check = 1,
};

class CurrentResultManager// : public ExternalInterface
{
public:
    static CurrentResultManager *Instance();
    bool Init();
    void SetSampleRange(float min, float max);
    void SetCheckRange(float min, float max);
    void OutputSample(float consistency);
    void OutputCheck(float consistency);
    void OutputSampleCurrent(float current);
    void OutputCheckCurrent(float current);

    void OnMeasureResultUpdated();

    String getSampleDriveFile();
    String getCheckDriveFile();

private:
    void OutPut(OutputType type);

private:
    String m_sampleDriveFile;
    String m_checkDriveFile;

    float m_sampleLowLimit;           ///< 水样4-20浓度下限
    float m_sampleUpLimit;            ///< 水样4-20浓度上限
    float m_checkLowLimit;            ///< 核查样4-20浓度下限
    float m_checkUpLimit;             ///< 核查样4-20浓度上限

    CurrentResultOutputer *m_sampleOutputer;
    CurrentResultOutputer *m_checkOutputer;
    CurrentResultOutputer *m_commonOutputer;

    ChipType m_chip;

    float m_sampleCurrent;
    float m_checkCurrent;

    static std::unique_ptr<CurrentResultManager> m_instance;

};
}
}
#endif // CURRENTRESULTMANAGER_H
