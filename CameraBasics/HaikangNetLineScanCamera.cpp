#include <QDebug>
#include "HaikangNetLineScanCamera.h"
#ifdef GLOBAL_HAIKANG

HaikangNetLineScanCamera::HaikangNetLineScanCamera(const MV_CC_DEVICE_INFO &deviceInfo)
    : HaikangNetCamera(deviceInfo)
{

}

HaikangNetLineScanCamera::~HaikangNetLineScanCamera()
{

}

bool HaikangNetLineScanCamera::setRoi(unsigned int offset_X, unsigned int offset_Y, unsigned int width, unsigned int height)
{
    Q_UNUSED(offset_Y)
    bool ret = true;
    ret &= setIntValue("OffsetX", 0);

    ret &= setIntValue("OffsetX", offset_X);
    ret &= setIntValue("Width",   width);
    ret &= setIntValue("Height",  height);

    return ret;
}

bool HaikangNetLineScanCamera::setLineRate(unsigned int rate)
{
    return setIntValue("AcquisitionLineRate", rate);
}

bool HaikangNetLineScanCamera::setPreampGain(float preampGain)
{
    return setEnumValue("PreampGain", preampGain * 1000);
}

bool HaikangNetLineScanCamera::getRoi(unsigned int &offset_X, unsigned int &offset_Y, unsigned int &width, unsigned int &height)
{
    bool ret = true;
    offset_Y = 0;
    ret &= getIntValue("OffsetX", offset_X);
    ret &= getIntValue("Width",   width);
    ret &= getIntValue("Height",  height);
    return ret;
}

bool HaikangNetLineScanCamera::getLineRate(unsigned int &rate)
{
    return getIntValue("AcquisitionLineRate", rate);
}

bool HaikangNetLineScanCamera::getPreampGain(float &preampGain)
{
    unsigned int gain = 0;
    bool result = getEnumValue("PreampGain", gain);
    preampGain = gain;
    return result;
}

#endif // GLOBAL_HAIKANG
