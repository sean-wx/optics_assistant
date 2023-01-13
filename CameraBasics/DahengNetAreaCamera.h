#ifndef DAHENGNETAREACAMERA_H
#define DAHENGNETAREACAMERA_H

#ifdef GLOBAL_DAHENG
#include <QMap>
#include "DahengAreaCamera.h"

class DahengNetAreaCamera : public DahengAreaCamera
{
public:
    explicit DahengNetAreaCamera(const GX_DEVICE_IP_INFO &deviceInfo);
    ~DahengNetAreaCamera();

    static bool enumDevices(QMap<QString, GX_DEVICE_IP_INFO> &cameraInfoList); //枚举相机设备
    static bool getCameraDeviceNum(unsigned int &deviceNum);

    /* 以下是设备操作接口 */
    virtual bool openDevice() override;                             /* 打开设备 */

    /* 以下是设备传输层，稳定性接口 */
    bool setGevSCPD(unsigned int gevSCPD = 50);                     /* 设置延迟发包间隔 */
    bool setPacketSize(unsigned int packetSize = 8192);             /* 设置包长(巨型帧) */
    bool setHeartbeatTime(unsigned int heartbeatTime = 6000);       /* 设置心跳时间 */
    bool setPacketSizeTimeout(unsigned int packetSizeTimeout = 20); /* 设置包超时时间20ms */
    bool setBlockTimeout(unsigned int blockTimeout = 3000);         /* 设置数据块超时时间3s */
    bool setResendTimeout(unsigned int resendTimeout = 20);         /* 设置数据重传超时时间20ms */
    bool setReSendEnable(bool enable = true);                       /* 设置数据重传使能 */
    bool setMaxWaitPacketCount(unsigned int maxWaitPacketCount = 8192); /* 设置最大等待包数8192 */

private:
    GX_DEVICE_IP_INFO          m_deviceInfo;                        //< Device DEVICE_IP_INFO
};

#endif // GLOBAL_DAHENG
#endif // DAHENGNETAREACAMERA_H
