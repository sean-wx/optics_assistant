#ifndef HAIKANGNETAREACAMERA_H
#define HAIKANGNETAREACAMERA_H
#ifdef GLOBAL_HAIKANG

#include "HaikangNetCamera.h"

class HaikangNetAreaCamera : public HaikangNetCamera
{
public:
    explicit HaikangNetAreaCamera(const MV_CC_DEVICE_INFO &deviceInfo);
    ~HaikangNetAreaCamera();
    bool setOffset_Y(unsigned int offset_Y);                /* 设置纵向偏移值Y */
    bool setAcquisitionFrameRateEnable(bool enable);        /* 设置采集帧率使能 */
    bool setAcquisitionFrameRate(float frameRate);          /* 设置采集帧率 */

    bool getAcquisitionFrameRateEnable(bool &enable);       /* 读取采集帧率使能 */
    bool getAcquisitionFrameRate(float &frameRate);         /* 读取采集帧率 */
    bool getOffset_Y(unsigned int &offset_Y);               /* 读取横向偏移值X */
};

#endif // GLOBAL_HAIKANG
#endif // HAIKANGNETAREACAMERA_H
