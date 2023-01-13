#ifndef DAHENGUSBAREACAMERA_H
#define DAHENGUSBAREACAMERA_H

#ifdef GLOBAL_DAHENG
#include "DahengAreaCamera.h"

class DahengUsbAreaCamera : public DahengAreaCamera
{
public:
    explicit DahengUsbAreaCamera(const QString &cameraSn);
    ~DahengUsbAreaCamera();

    static bool enumDevices(QList<QString> &cameraInfoList);
    static bool getCameraDeviceNum(unsigned int &deviceNum);

    /* 以下是设备操作接口 */
    virtual bool openDevice() override;                             /* 打开设备 */

    /* 以下是设备传输层，稳定性接口 */
    bool setStreamTransferSize(unsigned int streamTransferSize
                               = 1048576);                          /* 设置传输数据块大小 */
    bool setStreamTransferNumber(unsigned int streamTransferNumber
                                 = 32);                             /* 设置传输数据块数量 */

protected:
    QString   m_cameraSN         = "";                              /* 相机的SN编码 */
};

#endif // GLOBAL_DAHENG
#endif // DAHENGUSBAREACAMERA_H
