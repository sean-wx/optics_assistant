#include "HaikangNetAreaCamera.h"
#ifdef GLOBAL_HAIKANG

HaikangNetAreaCamera::HaikangNetAreaCamera(const MV_CC_DEVICE_INFO &deviceInfo)
    : HaikangNetCamera (deviceInfo)
{

}

HaikangNetAreaCamera::~HaikangNetAreaCamera()
{

}

bool HaikangNetAreaCamera::setOffset_Y(unsigned int offset_Y)
{
    return setIntValue("OffsetY", offset_Y);
}

bool HaikangNetAreaCamera::setAcquisitionFrameRateEnable(bool enable)
{
    return setBoolVaule("AcquisitionFrameRateEnable", enable);
}

bool HaikangNetAreaCamera::setAcquisitionFrameRate(float frameRate)
{
    return setFloatValue("AcquisitionFrameRate", frameRate);
}

bool HaikangNetAreaCamera::getAcquisitionFrameRateEnable(bool &enable)
{
    return getBoolVaule("AcquisitionFrameRateEnable", enable);
}

bool HaikangNetAreaCamera::getAcquisitionFrameRate(float &frameRate)
{
    return getFloatValue("AcquisitionFrameRate", frameRate);
}

bool HaikangNetAreaCamera::getOffset_Y(unsigned int &offset_Y)
{
    return getIntValue("OffsetY", offset_Y);
}

#endif // GLOBAL_HAIKANG
