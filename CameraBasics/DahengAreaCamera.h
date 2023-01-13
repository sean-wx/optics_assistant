#ifndef DAHENGAREACAMERA_H
#define DAHENGAREACAMERA_H

#ifdef GLOBAL_DAHENG
#include <QString>
#include "GxIAPI.h"
#include "DxImageProc.h"

#define ENUMRATE_TIME_OUT       200

class DahengAreaCamera
{
public:
    explicit DahengAreaCamera();
    virtual ~DahengAreaCamera();

    /* 以下是相机设备枚举参数 */
    enum DahengTriggerSource                  /* 相机触发源 */
    {
        TriggerSourceLine0             = GX_TRIGGER_SOURCE_LINE0,             ///< Lin0 Trigger
        TriggerSourceLine1             = GX_TRIGGER_SOURCE_LINE1,             ///< Lin1 Trigger
        TriggerSourceLine2             = GX_TRIGGER_SOURCE_LINE2,             ///< Lin2 Trigger
        TriggerSourceLine3             = GX_TRIGGER_SOURCE_LINE3,             ///< Lin3 Trigger
        TriggerSourceCounter0          = GX_TRIGGER_SOURCE_COUNTER2END,       ///< Counter0 Trigger
        TriggerSourceSoftWare          = GX_TRIGGER_SOURCE_SOFTWARE,          ///< Soft Ware Trigger
    };

    enum DahengTriggerActivation              /* 相机触发激活信号模式 */
    {
        TriggerActivationFallingEdge   = GX_TRIGGER_ACTIVATION_FALLINGEDGE,   ///< FallingEdge
        TriggerActivationRisingEdge    = GX_TRIGGER_ACTIVATION_RISINGEDGE,    ///< RisingEdge
    };

    enum DahengAcquisitionMode                /* 相机采集模式 */
    {
        AcquisitionModeSingleFrame    = GX_ACQ_MODE_SINGLE_FRAME,            ///< Single frame mode
        AcquisitionModeMulitiFrame    = GX_ACQ_MODE_MULITI_FRAME,            ///< Multi frame mode
        AcquisitionModeContinuous     = GX_ACQ_MODE_CONTINUOUS,              ///< Continuous mode
    };

    typedef void (*PhotoReceivedCallBack)(GX_FRAME_CALLBACK_PARAM* pFrame);
    typedef void (*CameraOfflineCallBack)(void* pUser);

    static bool initLib();
    static bool UnInitLib();

    /* 以下是设备操作接口 */
    virtual bool openDevice() = 0;                      /* 打开设备 */
    bool closeDevice();                                 /* 关闭设备 */
    bool startGrabbing();                               /* 开始取图 */
    bool stopGrabbing();                                /* 停止取图 */

    /* 以下是设备传输层，稳定性接口 */
    bool setImageBuffer(unsigned int imageBuffer = 8);  /* 设置图像buffer */
    bool setAcquisitionMode(DahengAcquisitionMode acquisitionMode
                            = AcquisitionModeContinuous);/* 设置图片采集模式为连续采集模式 */
    bool setAcquisitionFrameRateEnable(bool enable);    /* 设置采集帧率使能 */
    bool setAcquisitionFrameRate(float frameRate);      /* 设置采集帧率 */
    bool getAcquisitionFrameRateEnable(bool &enable);   /* 读取采集帧率使能 */
    bool getAcquisitionFrameRate(float &frameRate);     /* 读取采集帧率 */

    /* 以下是设备触发控制接口 */
    bool setHardwareTrigger(DahengTriggerSource triggerSource = TriggerSourceLine0);/* 设置触发模式为硬件(外部)触发 */
    bool setSoftwareTrigger();                          /* 设置触发模式为软(软件)触发 */
    bool sendSoftTriggerCommand();                      /* 通过软件下发命令，触发一次拍照 */
    bool setTriggerActivation(DahengTriggerActivation triggerActivation); /* 设置触发激活信号 */
    bool setlineDebouncerTime(bool isRising,
                              unsigned int debouncerTime = 50);/* 设置线路去抖动时间，需要传入设置信号的类型 */
    bool setTriggerDelayTime(float delayTime = 0);      /* 设置触发延迟时间 */

    /* 以下是设备状态获取接口 */
    bool isDeviceAccessible();                          /* 设备是否可达（是否可以已经掉线） */
    bool isDeviceOpenned();                             /* 设备是否已经打开 */
    bool isGrabbing();                                  /* 设备是否正在进行图片取流 */
    bool isAreaScan(bool &isAreaScanCamera);            /* 设备是否为面阵相机（面阵、线阵） */
    bool isColor(bool &isColorCamera);                  /* 设备是否为彩色相机（彩色、黑白） */

    bool setDeviceOffline(bool &isOffline);             /* 设置相机设备离线状态 */

    /* 以下是设备回调事件接口 */
    bool registerImageCallBack(PhotoReceivedCallBack callBack,
                               void *user);             /* 注册收到图片后的回调函数 */
    bool unRegisterImageCallBack();                     /* 注销收到图片后的回调函数 */

    bool registerCameraOfflineCallBack(CameraOfflineCallBack callBack,
                                       void *user);     /* 注册注册相机设备掉线的回调函数 */
    bool unRegisterCameraOfflineCallBack();             /* 注销注册相机设备掉线的回调函数 */

    /* 以下是设备参数设置接口 */
    bool setRoi(unsigned int offset_X,
                unsigned int offset_Y,
                unsigned int width,
                unsigned int height);                   /* 设置感兴趣区域（热点区域） */
    bool setOffset_X(unsigned int offset_X);            /* 设置横向偏移值X */
    bool setOffset_Y(unsigned int offset_Y);            /* 设置纵向偏移值Y */
    bool setImage_Width(unsigned int width);            /* 设置图像宽度 */
    bool setImage_Height(unsigned int height);          /* 设置图像高度 */
    bool setTriggerSource(DahengTriggerSource triggerSource);/* 设置触发源 */
    bool setTriggerMode(bool triggerMode);              /* 设置触发使能 */
    bool setExposureTime(float exposureTime);           /* 设置曝光值 */
    bool setGammaEnable(bool enable);                   /* 设置伽马使能 */
    bool setGamma(float gamma);                         /* 设置伽马值 */
    bool setGain(float gain);                           /* 设置增益值 */

    /* 以下是设备参数读取接口 */
    bool getRoi(unsigned int &offset_X,
                unsigned int &offset_Y,
                unsigned int &width,
                unsigned int &height);                  /* 读取感兴趣区域（热点区域） */
    bool getOffset_X(unsigned int &offset_X);           /* 读取横向偏移值X */
    bool getOffset_Y(unsigned int &offset_X);           /* 读取纵向偏移值Y */
    bool getImage_Width(unsigned int &width);           /* 读取图像宽度 */
    bool getImage_Height(unsigned int &height);         /* 读取图像高度 */
    bool getTriggerSource(DahengTriggerSource &triggerSource);/* 读取触发源 */
    bool getTriggerMode(bool &triggerMode);             /* 读取触发使能 */
    bool getExposureTime(float &exposureTime);          /* 读取曝光值 */
    bool getGammaEnable(bool &enable);                  /* 读取伽马使能 */
    bool getGamma(float &gamma);                        /* 读取伽马值 */
    bool getGain(float &gain);                          /* 读取增益值 */

    /* 以下是设备通用参数设置接口 */
    /* 一般情况下不建议使用      */
    bool setIntValue(int featureName, unsigned int value);
    bool setFloatValue(int featureName, float value);
    bool setEnumValue(int featureName, unsigned int value);

    /* 以下是设备通用参数读取接口 */
    /* 一般情况下不建议使用      */
    bool getIntValue(int featureName, unsigned int &value);
    bool getFloatValue(int featureName, float &value);
    bool getEnumValue(int featureName, unsigned int &value);

    /* 用于进行图像Gamma计算函数 */
    bool imageProcess(GX_FRAME_CALLBACK_PARAM pstFrameBuffer, unsigned char* pImageProcess);
    void imageTransformation(bool colorFilter);

protected:
    /* 通过参数获取名称 */
    QString getStringByFeatureName(int featureName);
    bool repairOffsetX_Y (int featureName, int value);

    /* 打印控制相关的错误提示 */
    void printNotCreatDeviceHandleErrorInfo(const QString action, const QString because);
    void printStateErrorInfo(const QString action, const QString because);
    void printDeviceControlStateInfo(const QString action, bool isSuccess, int errorCode = 0);

    /* 打印相机参数配置信息 */
    bool printCameraFeatureInfo_Int(int featureName, int value, const QString &action, GX_STATUS status);
    bool printCameraFeatureInfo_Float(int featureName, float value, const QString &action, GX_STATUS status);
    bool printCameraFeatureInfo_String(int featureName, const QString value, const QString &action, GX_STATUS status);

    MONO_IMG_PROCESS            m_stGrayImagePro;                         /* 灰度图像处理方法结构体指针 */
    COLOR_IMG_PROCESS           m_stColorImagePro;                        /* 彩色图像处理方法结构体指针 */

    QString                     m_cameraType              = "SN";         /* 相机的接口类别 */
    QString                     m_cameraInfo              = "";           /* 相机的打印信息 */

    GX_DEV_HANDLE               m_handle                  = nullptr;      /* 指向大恒相机实例对象的指针， 相机实例通过大恒SDK创建 */
    GX_EVENT_CALLBACK_HANDLE    m_handleEventCallBack;                    /* 记录当前相机设备掉线回调函数句柄 */
    bool                        m_isGrabbing              = false;        /* 记录当前相机是否已开启图片取流 */
    bool                        m_isOpened                = false;        /* 记录当前相机是否已打开（已连接到实体相机） */
    bool                        m_deviceOffline           = false;        /* 记录当前相机是否掉线（已连接到实体相机） */
    bool                        m_gammaEnable             = false;        /* 记录相机的伽玛使能（已连接到实体相机） */
    float                       m_gamma                   = 1;            /* 记录当前相机的伽玛参数（已连接到实体相机） */
    int64_t                     m_pixColorFilter          = -1;           /* 记录当前彩色相机的Bayer格式 */
};

#endif // GLOBAL_DAHENG
#endif // DAHENGAREACAMERA_H
