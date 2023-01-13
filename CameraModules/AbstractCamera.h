#ifndef ABSTRACTCAMERA_H
#define ABSTRACTCAMERA_H

#include <QObject>

class AbstractCamera : public QObject
{
    Q_OBJECT

public:
    explicit AbstractCamera();
    ~AbstractCamera() = default;

    /** @brief 初始化相机步骤：1.创建相机上下文环境、2.打开相机设备、3.打开相机取图功能 */
    virtual bool init()  = 0;

    /** @brief 关闭相机设备/关闭与实体相机之间的连接，取消对相机的独占权 */
    virtual bool close() = 0;

    /** @brief 开始取图 */
    virtual bool start() = 0;

    /** @brief 停止取图 */
    virtual bool stop()  = 0;

    /** @brief 返回相机的ID */
    inline int getId() const { return m_cameraId; }

    /** @brief 读取相机状态 */
    virtual bool isOpen();                                                  /* 判断当前相机是否打开 */
    virtual bool isGrab();                                                  /* 判断当前相机是否采集 */
    virtual bool isAreaScan(bool &isAreaScanCamera);                        /* 设备是否为面阵相机（面阵、线阵） */
    virtual bool isColor(bool &isColorCamera);                              /* 设备是否为彩色相机（彩色、黑白） */

    /** @brief 设置相机相关参数 */
    virtual bool setParameters();                                           /* 读取所有参数 */
    virtual bool setRoi(int roiX, int roiY, int roiW, int roiH);            /* 设置ROI参数，必须停止取图 */
    virtual bool setLineRate(float lineRate);                               /* 设置行频, 线扫相机独有 */
    virtual bool setPreampGain(float preampGain);                           /* 海康线扫相机独有增益 */
    virtual bool setGain(float gain);                                       /* 设置增益 */
    virtual bool setGamma(float gama);                                      /* 设置伽玛 */
    virtual bool setExposureTime(float exposure);                           /* 设置曝光 */
    virtual bool setTriggerSource(int triggerSource);                       /* 设置触发源 */
    virtual bool setTriggerMode(bool triggerMode);                          /* 设置触发模式开关,On or Off */
    virtual bool setGammaEnable(bool gammaEnable);                          /* 设置伽玛使能*/

    /** @brief 读取相机相关参数 */
    virtual bool readParameters();                                          /* 读取所有参数 */
    virtual bool getRoi(int &roiX, int &roiY, int &roiW, int &roiH);        /* 读取ROI参数 */
    virtual bool getLineRate(float &lineRate);                              /* 读取行频, 线扫相机独有 */
    virtual bool getPreampGain(float &preampGain);                          /* 海康线扫相机独有增益 */
    virtual bool getGain(float &gain);                                      /* 读取增益 */
    virtual bool getGamma(float &gamma);                                    /* 读取伽玛 */
    virtual bool getExposureTime(float &exposure);                          /* 读取曝光 */
    virtual bool getTriggerSource(int &triggerSource);                      /* 读取触发源 */
    virtual bool getTriggerMode(bool &triggerMode);                         /* 读取触发模式开关,On or Off */
    virtual bool getGammaEnable(bool &gammaEnable);                         /* 读取伽玛使能 */

    /** @brief 设置相机内部参数 */
    virtual bool sendSoftTriggerCommand();                                  /* 软触发一次 */
    virtual int  getCamerDeviceNum();                                       /* 获取枚举相机数量 */
    virtual bool setHardwareTrigger();                                      /* 切换到硬件触发*/
    virtual bool setSoftwareTrigger();                                      /* 切换到软件触发 */
    virtual bool setImageBuffer(int bufferNumber);                          /* 设置图像buffer */
    virtual bool setHeartbeatTime(int heartbeatTime);                       /* 设置心跳时间 */
    virtual bool setPacketSize(int packetSize);                             /* 设置包间隔 */

    virtual bool setReverse_X(bool enable = false);                         /* 设置图像沿X（垂直）方向反转 */
    virtual bool setReverse_Y(bool enable = false);                         /* 设置图像沿Y（水平）方向反转 */

    virtual bool setAcquisitionFrameRateEnable(bool enable);                /* 设置采集帧率使能 */
    virtual bool setAcquisitionFrameRate(float frameRate);                  /* 设置采集帧率 */
    virtual bool getAcquisitionFrameRateEnable(bool &enable);               /* 读取采集帧率使能 */
    virtual bool getAcquisitionFrameRate(float &frameRate);                 /* 读取采集帧率 */

    /** @brief 返回描述本相机的字符串 */
    QString toString() const;

public:
    int m_profileProductId = 1;
    int m_cameraId = 1;

signals:
    /***********************************************************************
     *  @signal photoReceived
     *  @brief 当相机完成取图，此信号将被发送
     *         相机管理理模块（CameraManager）应该绑定此信号
     *  @param photo 相机取出的原图
    ***********************************************************************/
    void photoReceived(const QImage &photo, int copyCost);
};

#endif // ABSTRACTCAMERA_H
