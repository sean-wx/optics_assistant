#ifndef HAIKANGNETCAMERA_H
#define HAIKANGNETCAMERA_H
#ifdef GLOBAL_HAIKANG

#include <QString>
#include "CameraParams.h"

class HaikangNetCamera
{
public:
    explicit HaikangNetCamera(const MV_CC_DEVICE_INFO &deviceInfo);
    virtual ~HaikangNetCamera();

    /* 以下是相机设备枚举参数 */
    enum HaikangTriggerSource                  /* 相机触发源 */
    {
        TriggerSourceLine0             = MV_TRIGGER_SOURCE_LINE0,           ///< Lin0 Trigger
        TriggerSourceLine1             = MV_TRIGGER_SOURCE_LINE1,           ///< Lin1 Trigger
        TriggerSourceLine2             = MV_TRIGGER_SOURCE_LINE2,           ///< Lin2 Trigger
        TriggerSourceLine3             = MV_TRIGGER_SOURCE_LINE3,           ///< Lin3 Trigger
        TriggerSourceCounter0          = MV_TRIGGER_SOURCE_COUNTER0,        ///< Counter0 Trigger
        TriggerSourceSoftWare          = MV_TRIGGER_SOURCE_SOFTWARE,        ///< Soft Ware Trigger
        TriggerSourceFrequencyConverter= MV_TRIGGER_SOURCE_FrequencyConverter,///< Frequency Converter Trigger
    };

    enum HaikangTriggerActivation              /* 相机触发激活信号模式 */
    {
        TriggerActivationRisingEdge    = 0,                                   ///< RisingEdge
        TriggerActivationFallingEdge   = 1,                                   ///< FallingEdge
        TriggerActivationLevelHigh     = 2,                                   ///< LevelHigh
        TriggerActivationLevelLow      = 3,                                   ///< LevelLow
    };

    enum HaikangAcquisitionMode                /* 相机采集模式 */
    {
        AcquisitionModeSingleFrame    = MV_ACQ_MODE_SINGLE,                  ///< Single frame mode
        AcquisitionModeMulitiFrame    = MV_ACQ_MODE_MUTLI,                   ///< Multi frame mode
        AcquisitionModeContinuous     = MV_ACQ_MODE_CONTINUOUS,              ///< Continuous mode
    };

    typedef void (*PhotoReceivedCallBack)(unsigned char *data, MV_FRAME_OUT_INFO_EX *frameInfo, void *user);

    static bool enumDevices(QMap<QString, MV_CC_DEVICE_INFO> &cameraInfoList);
    static bool getCameraDeviceNum(unsigned int &deviceNum);

    /* 以下是设备操作接口 */
    bool createHandle();                                /* 创建相机句柄（上下文） */
    bool destroyHandle();                               /* 释放相机句柄 */
    bool openDevice();                                  /* 打开设备 */
    bool closeDevice();                                 /* 关闭设备 */
    bool startGrabbing();                               /* 开始取图 */
    bool stopGrabbing();                                /* 停止取图 */

    /* 以下是设备传输层，稳定性接口 */
    bool setImageBuffer(unsigned int imageBuffer = 16); /* 设置图片缓冲节点，用于提升性能, 解决偶发性的图片丢帧、错位的问题 */
    bool setResend(unsigned int maxResendPercent = 100,
                   unsigned int resendTimeout = 3000);  /* 设置数据丢包后重传比例和重传超时 */
    bool setGevSCPD(unsigned int gevSCPD = 50);         /* 设置延迟发包间隔 */
    bool setPacketSize(unsigned int packetSize = 1500); /* 设置包长(巨型帧) */
    bool setAutoPacketSize();                           /* 自动设置最大值包长(巨型帧) */
    bool setHeartbeatEnable(bool enable);               /* 设置心跳使能 */
    bool setHeartbeatTime(unsigned int heartbeatTime = 6000); /* 设置心跳时间 */
    bool setAcquisitionMode(HaikangAcquisitionMode acquisitionMode
                            = AcquisitionModeContinuous);   /* 设置图片采集模式为连续采集模式 */

    /* 以下是设备触发控制接口 */
    bool setHardwareTrigger(HaikangTriggerSource triggerSource = TriggerSourceLine0);/* 设置触发模式为硬件(外部)触发 */
    bool setSoftwareTrigger();                          /* 设置触发模式为软(软件)触发 */
    bool sendSoftTriggerCommand();                      /* 通过软件下发命令，触发一次拍照 */
    bool setTriggerActivation(HaikangTriggerActivation triggerActivation); /* 设置触发激活信号 */
    bool setlineDebouncerTime(unsigned int debouncerTime = 50);/* 设置线路去抖动时间 */
    bool setTriggerDelayTime(float delayTime = 0);      /* 设置触发延迟时间 */

    /* 以下是设备状态获取接口 */
    bool isDeviceAccessible();                          /* 设备是否可达（是否可以已经掉线） */
    bool isDeviceOpenned();                             /* 设备是否已经打开 */
    bool isGrabbing();                                  /* 设备是否正在进行图片取流 */
    bool isAreaScan(bool &isAreaScanCamera);            /* 设备是否为面阵相机（面阵、线阵） */
    bool isColor(bool &isColorCamera);                  /* 设备是否为彩色相机（彩色、黑白） */

    /* 以下是设备回调事件接口 */
    bool registerImageCallBack(PhotoReceivedCallBack callBack,
                               void *user);             /* 注册收到图片后的回调函数 */

    /* 以下是设备参数设置接口 */
    virtual bool setRoi(unsigned int offset_X,
                        unsigned int offset_Y,
                        unsigned int width,
                        unsigned int height);           /* 设置感兴趣区域（热点区域） */
    bool setOffset_X(unsigned int offset_X);            /* 设置横向偏移值X */
    bool setImage_Width(unsigned int width);            /* 设置图像宽度 */
    bool setImage_Height(unsigned int height);          /* 设置图像高度 */
    bool setTriggerSource(HaikangTriggerSource triggerSource);/* 设置触发源 */
    bool setTriggerMode(bool triggerMode);              /* 设置触发使能 */
    bool setExposureTime(float exposureTime);           /* 设置曝光 */
    bool setGammaEnable(bool enable);                   /* 设置伽马使能 */
    bool setGamma(float gamma);                         /* 设置伽马值 */
    bool setGain(float gain);                           /* 设置增益值 */

    /* 以下是设备参数读取接口 */
    virtual bool getRoi(unsigned int &offset_X,
                        unsigned int &offset_Y,
                        unsigned int &width,
                        unsigned int &height);          /* 读取感兴趣区域（热点区域） */
    bool getOffset_X(unsigned int &offset_X);           /* 读取横向偏移值X */
    bool getImage_Width(unsigned int &width);           /* 读取图像宽度 */
    bool getImage_Height(unsigned int &height);         /* 读取图像高度 */
    bool getTriggerSource(HaikangTriggerSource &triggerSource);/* 读取触发源 */
    bool getTriggerMode(bool &triggerMode);             /* 读取触发使能 */
    bool getExposureTime(float &exposureTime);          /* 读取曝光值 */
    bool getGammaEnable(bool &enable);                  /* 读取伽马使能 */
    bool getGamma(float &gamma);                        /* 读取伽马值 */
    bool getGain(float &gain);                          /* 读取增益值 */

    /* 以下是设置图像反转接口 */
    bool setReverse_X(bool enable = false);             /* 设置图像沿X（垂直）方向反转 */
    bool setReverse_Y(bool enable = false);             /* 设置图像沿Y（水平）方向反转 */

    /* 以下是设备通用参数设置接口 */
    /* 一般情况下不建议使用      */
    bool setIntValue(const QString &featureName, unsigned int value);
    bool setFloatValue(const QString &featureName, float value);
    bool setEnumValue(const QString &featureName, unsigned int value);
    bool setBoolVaule(const QString &featureName, bool value);
    bool setStringValue(const QString &featureName, const QString &value);

    /* 以下是设备通用参数读取接口 */
    /* 一般情况下不建议使用      */
    bool getIntValue(const QString &featureName, unsigned int &value);
    bool getFloatValue(const QString &featureName, float &value);
    bool getEnumValue(const QString &featureName, unsigned int &value);
    bool getBoolVaule(const QString &featureName, bool &value);
    bool getStringValue(const QString &featureName, QString &value);

protected:
    static QString errorNoToHex(int error);

    /* 打印控制相关的错误提示 */
    void printNotCreatDeviceHandleErrorInfo(const QString action, const QString because);
    void printStateErrorInfo(const QString action, const QString because);
    void printDeviceControlStateInfo(const QString action, bool isSuccess, int errorCode = 0);

    /* 打印相机参数配置信息 */
    bool printCameraFeatureInfo_Int(const QString &featureName, int value, const QString &action, int status);
    bool printCameraFeatureInfo_Float(const QString &featureName, float value, const QString &action, int status);
    bool printCameraFeatureInfo_Bool(const QString &featureName, bool value, const QString &action, int status);
    bool printCameraFeatureInfo_String(const QString &featureName, QString value, const QString &action, int status);

    MV_CC_DEVICE_INFO     m_deviceInfo;                             /* 相机的设备信息 */
    void                 *m_handle                  = nullptr;      /* 指向海康相机实例对象的指针， 相机实例通过海康SDK创建 */
    bool                  m_isGrabbing              = false;        /* 记录当前相机是否已开启图片取流 */
    bool                  m_isOpened                = false;        /* 记录当前相机是否已打开（已连接到实体相机） */
    QString               m_ipAddress;                              /* 当前相机的IP地址 */
};

#endif // GLOBAL_HAIKANG
#endif // HAIKANGNETCAMERA_H
