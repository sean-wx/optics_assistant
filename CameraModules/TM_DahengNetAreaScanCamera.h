#ifndef TM_DAHENGNETAREASCANCAMERA_H
#define TM_DAHENGNETAREASCANCAMERA_H
#ifdef GLOBAL_DAHENG

#include <QImage>
#include <memory>
#include "AbstractCamera.h"
#include "DahengNetAreaCamera.h"

class TM_DahengNetAreaScanCamera : public AbstractCamera
{
    Q_OBJECT

public:
    explicit TM_DahengNetAreaScanCamera(const GX_DEVICE_IP_INFO &deviceInfo, int cameraId);
    virtual ~TM_DahengNetAreaScanCamera();

public:
    static bool initLib();
    static bool UnInitLib();

    /* 相机设备的相关操作 */
    bool init()       override;
    bool close()      override;
    bool start()      override;
    bool stop()       override;

    /* 读取相机状态 */
    bool isOpen()                                   override;
    bool isGrab()                                   override;
    virtual bool isAreaScan(bool &isAreaScanCamera) override;         /* 设备是否为面阵相机（面阵、线阵） */
    virtual bool isColor(bool &isColorCamera)       override;

    /* 设置相机相关参数 */
    bool setRoi(int roiX, int roiY, int roiW, int roiH) override;     /* 设置ROI相机感兴趣区域 */
    bool setGain(float gain)                        override;
    bool setGamma(float gama)                       override;
    bool setExposureTime(float exposure)            override;
    bool setTriggerSource(int triggerSource)        override;
    bool setTriggerMode(bool triggerMode)           override;
    bool setGammaEnable(bool gammaEnable)           override;

    /* 读取相机相关参数 */
    bool getRoi(int &roiX, int &roiY, int &roiW, int &roiH) override;
    bool getGain(float &gain)                       override;
    bool getGamma(float &gamma)                     override;
    bool getExposureTime(float &exposure)           override;
    bool getTriggerSource(int &triggerSource)       override;
    bool getTriggerMode(bool &triggerMode)          override;
    bool getGammaEnable(bool &gammaEnable)          override;

    /* 设置相机内部参数 */
    bool sendSoftTriggerCommand()                   override;
    int getCamerDeviceNum()                         override;
    bool setHardwareTrigger()                       override;
    bool setSoftwareTrigger()                       override;
    bool setImageBuffer(int bufferNumber)           override;

    bool setAcquisitionFrameRateEnable(bool enable) override;
    bool setAcquisitionFrameRate(float frameRate)   override;
    bool getAcquisitionFrameRateEnable(bool &enable)override;
    bool getAcquisitionFrameRate(float &frameRate)  override;

    /* 相机设备的前置操作 */
    static bool enumDevices(QMap<QString, GX_DEVICE_IP_INFO> &cameraInfoList);                 //枚举相机设备
    bool isAreaLScan();

signals:
//    void photoReceived(const QImage &photo, int mumber);

public:
    typedef enum TRIGGER_SOURCE
    {
        SOURCE_SOFRWARE            = 0,           ///< Soft Ware Trigger
        SOURCE_LINE0               = 1,           ///< Lin0 Trigger
        SOURCE_LINE1               = 2,           ///< Lin1 Trigger
        SOURCE_LINE2               = 3,           ///< Lin2 Trigger
        SOURCE_LINE3               = 4,           ///< Lin3 Trigger
    }TRIGGER_SOURCE;

private:
    /* 用于接收相机图片流的回调函数 */
    static void photoReceivedCallBack(GX_FRAME_CALLBACK_PARAM* pFrame);

    /* 用于相机掉线的回调函数 */
    static void cameraOfflineCallBack(void* pUser);

protected:
    std::shared_ptr<DahengNetAreaCamera> m_cameraHandle;

};

#endif // GLOBAL_DAHENG
#endif // TM_DAHENGNETAREASCANCAMERA_H
