#ifndef HAIKANGNETLINESCANCAMERA_H
#define HAIKANGNETLINESCANCAMERA_H
#ifdef GLOBAL_HAIKANG

#include <QList>
#include "HaikangNetCamera.h"

class HaikangNetLineScanCamera : public HaikangNetCamera
{
public:
    explicit HaikangNetLineScanCamera(const MV_CC_DEVICE_INFO &deviceInfo);
    ~HaikangNetLineScanCamera();

    /* 以下是光学参数设置接口 */
    bool setRoi(unsigned int offset_X,
                unsigned int offset_Y,
                unsigned int width,
                unsigned int height) override;   /* 设置感兴趣区域（热点区域） */
    bool setLineRate(unsigned int rate);         /* 设置线扫频率 */
    bool setPreampGain(float preampGain);        /* 设置前置放大器增益 */

    bool getRoi(unsigned int &offset_X,
                unsigned int &offset_Y,
                unsigned int &width,
                unsigned int &height);           /* 读取感兴趣区域（热点区域） */
    bool getLineRate(unsigned int &rate);        /* 读取线扫频率 */
    bool getPreampGain(float &preampGain);       /* 设置前置放大器增益 */
};

#endif // GLOBAL_HAIKANG
#endif // HAIKANGNETLINESCANCAMERA_H
