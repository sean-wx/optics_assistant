#include <QDebug>
#include <QTimer>
#include <QEventLoop>

#ifdef GLOBAL_DAHENG
#include "DahengAreaCamera.h"

#define CALCCCPARAM_LENGTH (sizeof(VxInt16) * 9)

DahengAreaCamera::DahengAreaCamera()
{

}

DahengAreaCamera::~DahengAreaCamera()
{
    if (m_handle) {
        if (m_isGrabbing)
            stopGrabbing();

        if (m_isOpened)
            closeDevice();
    }
}

bool DahengAreaCamera::initLib()
{
    // Init GxiApi libary
    GX_STATUS emStatus = GX_STATUS_SUCCESS;
    emStatus = GXInitLib();
    if (emStatus != GX_STATUS_SUCCESS) {
        qCritical() << QString("安装大恒相机SDK驱动库失败");
        return false;
    } else {
        qDebug() << QString("安装大恒相机SDK驱动库成功");
        return true;
    }
}

bool DahengAreaCamera::UnInitLib()
{
    // Uninit GxiApi libary
    GX_STATUS emStatus = GX_STATUS_SUCCESS;
    emStatus = GXCloseLib();
    if (emStatus != GX_STATUS_SUCCESS) {
        qCritical() << QString("卸载大恒相机SDK驱动库失败");
        return false;
    } else {
        qDebug() << QString("卸载大恒相机SDK驱动库成功");
        return true;
    }
}

bool DahengAreaCamera::closeDevice()
{  
    if (!m_isOpened) {
        printStateErrorInfo(QString("关闭"), QString("该相机未被打开"));
        return true;
    }

    if (m_isGrabbing) {
        stopGrabbing();
    }

    GX_STATUS resultCode = GXCloseDevice(m_handle);
    bool result = false;
    if (GX_STATUS_SUCCESS == resultCode) {
        result = true;
        m_isOpened = false;
        m_deviceOffline = true;
    }
    printDeviceControlStateInfo(QString("关闭"), result, resultCode);
    return result;
}

bool DahengAreaCamera::startGrabbing()
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("开始采集"), QString("该相机未被打开"));
        return false;
    }

    if (m_isGrabbing) {
        printStateErrorInfo(QString("开始采集"), QString("该相机已经开启采集"));
        return true;
    }
    bool result = false;
    GX_STATUS resultCode = GXSendCommand(m_handle, GX_COMMAND_ACQUISITION_START);
    if (GX_STATUS_SUCCESS == resultCode) {
        result = true;
        m_isGrabbing = true;
    }

    printDeviceControlStateInfo(QString("开始采集"), result, resultCode);
    return result;
}

bool DahengAreaCamera::stopGrabbing()
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("停止采集"), QString("该相机未被打开"));
        return false;
    }

    if (!m_isGrabbing) {
        printStateErrorInfo(QString("停止采集"), QString("该相机已经停止采集"));
        return true;
    }

    bool result = false;
    GX_STATUS resultCode = GX_STATUS_SUCCESS;
    for (int num = 0; num <= 50; num++) {
        resultCode = GXSendCommand(m_handle, GX_COMMAND_ACQUISITION_STOP);
        if(resultCode == GX_STATUS_SUCCESS) {
            m_isGrabbing = false;
            result = true;
            break;
        }

        result = false;
        size_t size             = 0;
        char *errorInfo         = NULL;
        GX_STATUS errorCode     = resultCode;
        resultCode = GXGetLastError(&errorCode, NULL, &size);
        errorInfo = new char[size];
        if (errorInfo == NULL) {
            continue;
        }
        resultCode = GXGetLastError(&errorCode, errorInfo, &size);
        qWarning() << QString("%1%2为%3的大恒相机失败，错误号：%4，错误信息：%5")
                      .arg("停止采集")
                      .arg(m_cameraType)
                      .arg(m_cameraInfo)
                      .arg(QString::number(errorCode))
                      .arg(QString(errorInfo));
        if (errorInfo != NULL) {
            delete[] errorInfo;
            errorInfo = NULL;
        }

        resultCode = errorCode;
        QEventLoop eventloop;
        QTimer::singleShot(20, &eventloop, SLOT(quit()));
        eventloop.exec();
    }
    printDeviceControlStateInfo(QString("停止采集"), result, resultCode);
    return result;
}

bool DahengAreaCamera::setImageBuffer(unsigned int imageBuffer)
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("ImageBuffer"), QString("该相机未被打开"));
        return false;
    }

    GX_STATUS resultCode = GXSetAcqusitionBufferNumber(m_handle, imageBuffer);
    if(resultCode == GX_STATUS_SUCCESS) {
        qDebug() << QString("%1为（%2）的大恒相机%3参数（%4 = %5）成功。")
                    .arg(m_cameraType)
                    .arg(m_cameraInfo)
                    .arg("设置")
                    .arg("ImageBuffer")
                    .arg(QString::number(imageBuffer));
        return true;
    } else {
        qCritical() << QString("%1为（%2）的大恒相机%3参数（%4 = %5）失败，错误号：%6。")
                       .arg(m_cameraType)
                       .arg(m_cameraInfo)
                       .arg("设置")
                       .arg("ImageBuffer")
                       .arg(QString::number(imageBuffer))
                       .arg(QString::number(resultCode));
        return false;
    }
}

bool DahengAreaCamera::setAcquisitionMode(DahengAcquisitionMode acquisitionMode)
{
    return setEnumValue(GX_ENUM_ACQUISITION_MODE, acquisitionMode);
}

bool DahengAreaCamera::setAcquisitionFrameRateEnable(bool enable)
{
    if (enable) {
        return setEnumValue(GX_ENUM_ACQUISITION_FRAME_RATE_MODE, GX_ACQUISITION_FRAME_RATE_MODE_ON);
    } else {
        return setEnumValue(GX_ENUM_ACQUISITION_FRAME_RATE_MODE, GX_ACQUISITION_FRAME_RATE_MODE_OFF);
    }
}

bool DahengAreaCamera::setAcquisitionFrameRate(float frameRate)
{
    return setFloatValue(GX_FLOAT_ACQUISITION_FRAME_RATE, frameRate);
}

bool DahengAreaCamera::getAcquisitionFrameRateEnable(bool &enable)
{
    unsigned int frameRateEnable = 0;
    bool result = getEnumValue(GX_ENUM_ACQUISITION_FRAME_RATE_MODE, frameRateEnable);
    if (frameRateEnable == GX_ACQUISITION_FRAME_RATE_MODE_ON) {
        enable = true;
    } else {
        enable = false;
    }
    return result;
}

bool DahengAreaCamera::getAcquisitionFrameRate(float &frameRate)
{
    return getFloatValue(GX_FLOAT_ACQUISITION_FRAME_RATE, frameRate);
}

bool DahengAreaCamera::setHardwareTrigger(DahengTriggerSource triggerSource)
{
    bool result = true;
    result &= setTriggerMode(true);
    result &= setTriggerSource(triggerSource);
    return result;
}

bool DahengAreaCamera::setSoftwareTrigger()
{
    bool result = true;
    result &= setTriggerMode(true);
    result &= setTriggerSource(TriggerSourceSoftWare);
    return result;
}

bool DahengAreaCamera::sendSoftTriggerCommand()
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("发送软触发命令"), QString("该相机未被打开"));
        return false;
    }

    if (!m_isGrabbing) {
        printStateErrorInfo(QString("发送软触发命令"), QString("该相机已经停止采集"));
        return false;
    }

    GX_STATUS result = GX_STATUS_SUCCESS;
    result = GXSendCommand(m_handle, GX_COMMAND_TRIGGER_SOFTWARE);
    if(result == GX_STATUS_SUCCESS) {
        qDebug() << QString("给%1为%2的大恒相机发送软触发命令成功").arg(m_cameraType).arg(m_cameraInfo);
        return true;
    } else {
        qCritical() << QString("给%1为%12的大恒相机发送软触发命令失败，错误码：%2").arg(m_cameraType).arg(m_cameraInfo);
        return false;
    }
}

bool DahengAreaCamera::setTriggerActivation(DahengTriggerActivation triggerActivation)
{
    return setEnumValue(GX_ENUM_TRIGGER_ACTIVATION, triggerActivation);
}

bool DahengAreaCamera::setlineDebouncerTime(bool isRising, unsigned int debouncerTime)
{
    if (isRising) {
        return setFloatValue(GX_FLOAT_TRIGGER_FILTER_RAISING, debouncerTime);
    } else {
        return setFloatValue(GX_FLOAT_TRIGGER_FILTER_FALLING, debouncerTime);
    }
}

bool DahengAreaCamera::setTriggerDelayTime(float delayTime)
{
    return setFloatValue(GX_FLOAT_TRIGGER_DELAY, delayTime);
}

bool DahengAreaCamera::isDeviceAccessible()
{
    return m_deviceOffline;
}

bool DahengAreaCamera::isDeviceOpenned()
{
    return m_isOpened;
}

bool DahengAreaCamera::isGrabbing()
{
    return m_isGrabbing;
}

bool DahengAreaCamera::isAreaScan(bool &isAreaScanCamera)
{
    isAreaScanCamera = true;
    return true;
}

bool DahengAreaCamera::isColor(bool &isColorCamera)
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("获取相机颜色"), QString("该相机未被打开"));
        return false;
    }

    bool isColor = false;
    GX_STATUS emStatus = GXIsImplemented(m_handle, GX_ENUM_PIXEL_COLOR_FILTER, &isColor);
    if(emStatus == GX_STATUS_SUCCESS) {
        isColorCamera = isColor;
        if (isColor) {
            //获取彩色相机的Bayer格式
            emStatus = GXGetEnum(m_handle, GX_ENUM_PIXEL_COLOR_FILTER, &m_pixColorFilter);
            printCameraFeatureInfo_Int(GX_ENUM_PIXEL_COLOR_FILTER, m_pixColorFilter, "获取", emStatus);
        }
    }
    return printCameraFeatureInfo_String(GX_ENUM_PIXEL_COLOR_FILTER, isColorCamera ? "true" : "false", "获取", emStatus);
}

bool DahengAreaCamera::setDeviceOffline(bool &isOffline)
{
    m_deviceOffline = isOffline;
    return true;
}

bool DahengAreaCamera::registerImageCallBack(DahengAreaCamera::PhotoReceivedCallBack callBack, void *user)
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("注册图像回调函数"), QString("该相机未被打开"));
        return false;
    }

    GX_STATUS result = GXRegisterCaptureCallback(m_handle, user, callBack);
    if (GX_STATUS_SUCCESS != result) {
        qCritical() << QString("为%1为%2的大恒相机注册图像回调函数失败，错误码：%3").arg(m_cameraType).arg(m_cameraInfo).arg(result);
        return false;
    }
    qDebug() << QString("为%1为%2的大恒相机注册图像回调函数成功").arg(m_cameraType).arg(m_cameraInfo);
    return true;
}

bool DahengAreaCamera::unRegisterImageCallBack()
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("注销图像回调函数"), QString("该相机未被打开"));
        return true;
    }
    GX_STATUS result = GXUnregisterCaptureCallback(m_handle);
    if (result == GX_STATUS_SUCCESS) {
        qDebug() << QString("为%1为%2的大恒相机注销图像回调函数成功").arg(m_cameraType).arg(m_cameraInfo);
        return true;
    } else {
        qCritical() << QString("为%1为%2的大恒相机注销图像回调函数失败，错误码：%3").arg(m_cameraType).arg(m_cameraInfo).arg(result);
        return false;
    }
}

bool DahengAreaCamera::registerCameraOfflineCallBack(DahengAreaCamera::CameraOfflineCallBack callBack, void *user)
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("注册相机设备掉线回调函数"), QString("该相机未被打开"));
        return false;
    }

    GX_STATUS result = GXRegisterDeviceOfflineCallback(m_handle, user, callBack, &m_handleEventCallBack);
    if (GX_STATUS_SUCCESS != result) {
        qCritical() << QString("为%1为%2的大恒相机注册相机设备掉线回调函数失败，错误码：%3").arg(m_cameraType).arg(m_cameraInfo).arg(result);
        return false;
    }
    qDebug() << QString("为%1为%2的大恒相机注册相机设备掉线回调函数成功").arg(m_cameraType).arg(m_cameraInfo);
    return true;
}

bool DahengAreaCamera::unRegisterCameraOfflineCallBack()
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("注销相机设备掉线回调函数"), QString("该相机未被打开"));
        return true;
    }
    GX_STATUS result = GXUnregisterDeviceOfflineCallback(m_handle, m_handleEventCallBack);
    if (result == GX_STATUS_SUCCESS) {
        qDebug() << QString("为%1为%2的大恒相机注销相机设备掉线回调函数成功").arg(m_cameraType).arg(m_cameraInfo);
        return true;
    } else {
        qCritical() << QString("为%1为%2的大恒相机注销相机设备掉线回调函数失败，错误码：%3").arg(m_cameraType).arg(m_cameraInfo).arg(result);
        return false;
    }
}

bool DahengAreaCamera::setRoi(unsigned int offset_X, unsigned int offset_Y, unsigned int width, unsigned int height)
{
    bool result = true;
//    result &= setIntValue(GX_INT_OFFSET_X, 0);
//    result &= setIntValue(GX_INT_OFFSET_Y, 0);
    repairOffsetX_Y(GX_INT_OFFSET_X, 0);
    repairOffsetX_Y(GX_INT_OFFSET_Y, 0);

    result &= setIntValue(GX_INT_OFFSET_X, offset_X);
    result &= setIntValue(GX_INT_OFFSET_Y, offset_Y);
    result &= setIntValue(GX_INT_WIDTH,   width);
    result &= setIntValue(GX_INT_HEIGHT,  height);

    return result;
}

bool DahengAreaCamera::setOffset_X(unsigned int offset_X)
{
    //调用该接口之前必须处于停止采集
    return setIntValue(GX_INT_OFFSET_X, offset_X);
}

bool DahengAreaCamera::setOffset_Y(unsigned int offset_Y)
{
    //调用该接口之前必须处于停止采集
    return setIntValue(GX_INT_OFFSET_Y, offset_Y);
}

bool DahengAreaCamera::setImage_Width(unsigned int width)
{
    //调用该接口之前必须处于停止采集
    return setIntValue(GX_INT_WIDTH, width);
}

bool DahengAreaCamera::setImage_Height(unsigned int height)
{   
    //调用该接口之前必须处于停止采集
    return setIntValue(GX_INT_HEIGHT, height);
}

bool DahengAreaCamera::setTriggerSource(DahengTriggerSource triggerSource)
{
    return setEnumValue(GX_ENUM_TRIGGER_SOURCE, triggerSource);
}

bool DahengAreaCamera::setTriggerMode(bool triggerMode)
{
    // Set Trigger Mode
    if(triggerMode) {
        return setEnumValue(GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_ON);
    } else {
        return setEnumValue(GX_ENUM_TRIGGER_MODE, GX_TRIGGER_MODE_OFF);
    }
}

bool DahengAreaCamera::setExposureTime(float exposureTime)
{
    return setFloatValue(GX_FLOAT_EXPOSURE_TIME, exposureTime);
}

bool DahengAreaCamera::setGammaEnable(bool enable)
{
    m_gammaEnable = enable;
    qDebug() << QString("%1为（%2）的大恒相机%3参数（%4 = %5）成功。")
                .arg(m_cameraType)
                .arg(m_cameraInfo)
                .arg("设置")
                .arg("Gamma Enable")
                .arg(enable);
    return true;
}

bool DahengAreaCamera::setGamma(float gamma)
{
    m_gamma = gamma;
    qDebug() << QString("%1为（%2）的大恒相机%3参数（%4 = %5）成功。")
                .arg(m_cameraType)
                .arg(m_cameraInfo)
                .arg("设置")
                .arg("Gamma")
                .arg(QString::number(gamma));
    return true;
}

bool DahengAreaCamera::setGain(float gain)
{
    return setFloatValue(GX_FLOAT_GAIN, gain);
}

bool DahengAreaCamera::getRoi(unsigned int &offset_X, unsigned int &offset_Y, unsigned int &width, unsigned int &height)
{
    bool result = true;
    result &= getIntValue(GX_INT_OFFSET_X, offset_X);
    result &= getIntValue(GX_INT_OFFSET_Y, offset_Y);
    result &= getIntValue(GX_INT_WIDTH,    width);
    result &= getIntValue(GX_INT_HEIGHT,   height);
    return result;
}

bool DahengAreaCamera::getOffset_X(unsigned int &offset_X)
{
    return getIntValue(GX_INT_OFFSET_X, offset_X);
}

bool DahengAreaCamera::getOffset_Y(unsigned int &offset_Y)
{
    return getIntValue(GX_INT_OFFSET_Y, offset_Y);
}

bool DahengAreaCamera::getImage_Width(unsigned int &width)
{
    return getIntValue(GX_INT_WIDTH, width);
}

bool DahengAreaCamera::getImage_Height(unsigned int &height)
{
    return getIntValue(GX_INT_HEIGHT, height);
}

bool DahengAreaCamera::getTriggerSource(DahengTriggerSource &triggerSource)
{
    unsigned int triggerSourceValue = 0;
    bool result = getEnumValue(GX_ENUM_TRIGGER_SOURCE, triggerSourceValue);
    triggerSource = DahengTriggerSource(triggerSourceValue);
    return result;
}

bool DahengAreaCamera::getTriggerMode(bool &triggerMode)
{
    unsigned int trggerModeIndex = 0;
    if(getEnumValue(GX_ENUM_TRIGGER_MODE, trggerModeIndex)) {
        if(trggerModeIndex == GX_TRIGGER_MODE_ON) {
            triggerMode = true;
        } else {
            triggerMode = false;
        }
        return true;
    }
    return false;
}

bool DahengAreaCamera::getExposureTime(float &exposureTime)
{
    return getFloatValue(GX_FLOAT_EXPOSURE_TIME, exposureTime);
}

bool DahengAreaCamera::getGammaEnable(bool &enable)
{
    enable = m_gammaEnable;
    qDebug() << QString("%1为（%2）的大恒相机%3参数（%4 = %5）成功。")
                .arg(m_cameraType)
                .arg(m_cameraInfo)
                .arg("获取")
                .arg("Gamma Enable")
                .arg(enable);
    return true;
}

bool DahengAreaCamera::getGamma(float &gamma)
{
    gamma = m_gamma;
    qDebug() << QString("%1为（%2）的大恒相机%3参数（%4 = %5）成功。")
                .arg(m_cameraType)
                .arg(m_cameraInfo)
                .arg("获取")
                .arg("Gamma")
                .arg(QString::number(gamma));
    return true;
}

bool DahengAreaCamera::getGain(float &gain)
{
    return getFloatValue(GX_FLOAT_GAIN, gain);
}

bool DahengAreaCamera::setIntValue(int featureName, unsigned int value)
{
   if (m_isOpened) {
        GX_STATUS emStatus = GX_STATUS_SUCCESS;
        emStatus = GXSetInt(m_handle, featureName, value);
        return printCameraFeatureInfo_Int(featureName, value, "设置", emStatus);
   } else {
       qWarning() << QString("不能设置相机参数（%1），因为相机没有被打开。").arg(featureName);
       return false;
   }
}

bool DahengAreaCamera::setFloatValue(int featureName, float value)
{
    if (m_isOpened) {
         GX_STATUS emStatus = GX_STATUS_SUCCESS;
         emStatus = GXSetFloat(m_handle, featureName, value);
         return printCameraFeatureInfo_Float(featureName, value, "设置", emStatus);
    } else {
        qWarning() << QString("不能设置相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool DahengAreaCamera::setEnumValue(int featureName, unsigned int value)
{
    if (m_isOpened) {
         GX_STATUS emStatus = GX_STATUS_SUCCESS;
         emStatus = GXSetEnum(m_handle, featureName, value);
         return printCameraFeatureInfo_Int(featureName, value, "设置", emStatus);
    } else {
        qWarning() << QString("不能设置相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool DahengAreaCamera::getIntValue(int featureName, unsigned int &value)
{
    if (m_isOpened) {
        GX_STATUS emStatus = GX_STATUS_SUCCESS;
        int64_t getValue = -1;
        emStatus = GXGetInt(m_handle, featureName, &getValue);
        value = getValue;
        return printCameraFeatureInfo_Int(featureName, value, "获取", emStatus);
    } else {
        qWarning() << QString("不能获取相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool DahengAreaCamera::getFloatValue(int featureName, float &value)
{
    if (m_isOpened) {
        GX_STATUS emStatus = GX_STATUS_SUCCESS;
        double getValue = -1;
        emStatus = GXGetFloat(m_handle, featureName, &getValue);
        value = getValue;
        return printCameraFeatureInfo_Float(featureName, value, "获取", emStatus);
    } else {
        qWarning() << QString("不能获取相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool DahengAreaCamera::getEnumValue(int featureName, unsigned int &value)
{
    if (m_isOpened) {
        GX_STATUS emStatus = GX_STATUS_SUCCESS;
        int64_t getValue = -1;
        emStatus = GXGetEnum(m_handle, featureName, &getValue);
        value = getValue;
        return printCameraFeatureInfo_Int(featureName, value, "获取", emStatus);
    } else {
        qWarning() << QString("不能获取相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool DahengAreaCamera::imageProcess(GX_FRAME_CALLBACK_PARAM pstFrameBuffer, unsigned char *pImageProcess)
{
    bool colorFilter = false;
    if (!isColor(colorFilter)) {
        return false;
    }

    if (colorFilter) {
        imageTransformation(colorFilter);
        // 对 Raw8 图像进行处理
        unsigned int roiW = 0;
        unsigned int roiH = 0;
        getImage_Width(roiW);
        getImage_Height(roiH);

        DxRaw8ImgProcess((unsigned char*)pstFrameBuffer.pImgBuf, pImageProcess, roiW, roiH, &m_stColorImagePro);
        if (m_stColorImagePro.pProLut != NULL) {
            delete[] m_stColorImagePro.pProLut;
            m_stColorImagePro.pProLut = NULL;
        }
        if (m_stColorImagePro.parrCC != NULL) {
            delete[] m_stColorImagePro.parrCC;
            m_stColorImagePro.parrCC = NULL;
        }
    } else {
        imageTransformation(colorFilter);
        // 对 Mono8 图像进行处理
        unsigned int roiW = 0;
        unsigned int roiH = 0;
        getImage_Width(roiW);
        getImage_Height(roiH);

        DxMono8ImgProcess((unsigned char*)pstFrameBuffer.pImgBuf, pImageProcess, roiW, roiH, &m_stGrayImagePro);
        if (m_stGrayImagePro.pProLut != NULL) {
            delete[] m_stGrayImagePro.pProLut;
            m_stGrayImagePro.pProLut = NULL;
        }
    }
    return true;
}

void DahengAreaCamera::imageTransformation(bool colorFilter)
{
    if (colorFilter) {
        float fSharpen = 1.0;
        int64_t  nContrastParam = 0;

        double dGamma = 1;
        if (m_gammaEnable) {
            dGamma=m_gamma;
        }

        int nLightness = 0;
        //VxUint16 nLutLength;
        // 图像处理功能设置结构体参数初始化
        m_stColorImagePro.bAccelerate = false;
        m_stColorImagePro.bDefectivePixelCorrect = false;
        m_stColorImagePro.bDenoise = false;
        m_stColorImagePro.bFlip = false;
        m_stColorImagePro.bSharpness = false;
        m_stColorImagePro.fSharpFactor = fSharpen;
        m_stColorImagePro.cvType = RAW2RGB_NEIGHBOUR;
        m_stColorImagePro.emLayOut = DX_PIXEL_COLOR_FILTER(m_pixColorFilter);
        // 获取查找表长度
        VxInt32 DxStatus = DxGetLut(nContrastParam, dGamma, nLightness, NULL, &m_stColorImagePro.nLutLength);
        if (DxStatus!=DX_OK) {
            return ;
        }
        // 为查找表申请空间
        m_stColorImagePro.pProLut = new VxUint8[m_stColorImagePro.nLutLength];
        DxStatus = DxGetLut(nContrastParam, dGamma, nLightness, m_stColorImagePro.pProLut, &m_stColorImagePro.nLutLength);
        if (DxStatus != DX_OK) {
            if (m_stColorImagePro.pProLut != NULL) {
                delete[] m_stColorImagePro.pProLut;
                m_stColorImagePro.pProLut = NULL;
            }
            return ;
        }
        m_stColorImagePro.nCCBufLength = CALCCCPARAM_LENGTH;
        m_stColorImagePro.parrCC = new VxInt16[m_stColorImagePro.nCCBufLength];
        if (m_stColorImagePro.parrCC == NULL) {
            if (m_stColorImagePro.pProLut != NULL) {
                delete[] m_stColorImagePro.pProLut;
                m_stColorImagePro.pProLut = NULL;
            }
            return ;
        }

        int64_t nColorCorrect=0;
        int nSuturation=68;
        // 计算图像彩色调节数组
        DxStatus = DxCalcCCParam(nColorCorrect,nSuturation,m_stColorImagePro.parrCC,CALCCCPARAM_LENGTH);
        if(DxStatus != DX_OK) {
            if(m_stColorImagePro.pProLut != NULL) {
                delete[] m_stColorImagePro.pProLut;
                m_stColorImagePro.pProLut = NULL;
            }
            return ;
        }
        if (m_stColorImagePro.parrCC != NULL) {
            delete[] m_stColorImagePro.parrCC;
            m_stColorImagePro.parrCC = NULL;
        }
    } else {
        float fSharpen=1.0;
        int64_t  nContrastParam=0;

        double dGamma = 1;
        if (m_gammaEnable) {
            dGamma=m_gamma;
        }

        int nLightness=0;
        // 图像处理功能设置结构体参数初始化
        m_stGrayImagePro.bAccelerate = false;
        m_stGrayImagePro.bDefectivePixelCorrect = false;
        m_stGrayImagePro.bSharpness = false;
        m_stGrayImagePro.fSharpFactor = fSharpen;
        // 获取查找表长度
        VxInt32 DxStatus = DxGetLut(nContrastParam, dGamma, nLightness, NULL, &m_stGrayImagePro.nLutLength);
        if(DxStatus != DX_OK) {
            return;
        }
        // 为查找表申请空间
        m_stGrayImagePro.pProLut = new VxUint8[m_stGrayImagePro.nLutLength];
        // 计算查找表
        DxStatus = DxGetLut(nContrastParam, dGamma, nLightness, m_stGrayImagePro.pProLut, &m_stGrayImagePro.nLutLength);
        if(DxStatus != DX_OK) {
            if(m_stGrayImagePro.pProLut != NULL) {
                delete[]m_stGrayImagePro.pProLut;
                m_stGrayImagePro.pProLut = NULL;
            }
            return;
        }
    }
}

QString DahengAreaCamera::getStringByFeatureName(int featureName)
{
    QString result = "";
    switch (featureName)
    {
    case GX_INT_WIDTH:
        result = "Width";
        break;
    case GX_INT_HEIGHT:
        result = "Height";
        break;
    case GX_INT_OFFSET_X:
        result = "OffsetX";
        break;
    case GX_INT_OFFSET_Y:
        result = "OffsetY";
        break;
    case GX_FLOAT_GAIN:
        result = "Gain";
        break;
    case GX_ENUM_TRIGGER_ACTIVATION:
        result = "Trigger Activation";
        break;
    case GX_FLOAT_TRIGGER_FILTER_RAISING:
        result = "Rising DebouncerTime";
        break;
    case GX_FLOAT_TRIGGER_FILTER_FALLING:
        result = "Falling DebouncerTime";
        break;
    case GX_FLOAT_TRIGGER_DELAY:
        result = "Trigger Delay";
        break;
    case GX_FLOAT_EXPOSURE_TIME:
        result = "Exposure Time";
        break;
    case GX_ENUM_TRIGGER_SOURCE:
        result = "Trigger Source";
        break;
    case GX_ENUM_TRIGGER_MODE:
        result = "Trigger Mode";
        break;
    case GX_ENUM_PIXEL_COLOR_FILTER:
        result = "Color Filter";
        break;
    case GX_ENUM_ACQUISITION_MODE:
        result = "Acquisittion Mode";
        break;
    case GX_FLOAT_ACQUISITION_FRAME_RATE:
        result = "Acquisittion frame rate";
        break;
    case GX_ENUM_ACQUISITION_FRAME_RATE_MODE:
        result = "Acquisittion  frame rate mode";
        break;
    case GX_INT_GEV_HEARTBEAT_TIMEOUT:
        result = "Heartbeat Timeout";
        break;
    case GX_INT_GEV_PACKETSIZE:
        result = "PacketSize";
        break;
    case GX_INT_GEV_PACKETDELAY:
        result = "GevSCPD";
        break;
    case GX_DS_INT_ANNOUNCED_BUFFER_COUNT:
        result = "StreamAnnouncedBufferCount";
        break;
    case GX_DS_INT_STREAM_TRANSFER_SIZE:
        result = "StreamTransferSize";
        break;
    case GX_DS_INT_STREAM_TRANSFER_NUMBER_URB:
        result = "StreamTransferNumberUrb";
        break;
    case GX_DS_ENUM_RESEND_MODE:
        result = "ResendMode";
        break;
    case GX_DS_INT_PACKET_TIMEOUT:
        result = "PacketTimeout";
        break;
    case GX_DS_INT_BLOCK_TIMEOUT:
        result = "BlockTimeout";
        break;
    case GX_DS_INT_RESEND_TIMEOUT:
        result = "ResendTimeout";
        break;
    case GX_DS_INT_MAX_WAIT_PACKET_COUNT:
        result = "MaxWaitPacketCount";
        break;
    default:
        result = "Unknown";
        break;
    }
    return result;
}

bool DahengAreaCamera::repairOffsetX_Y(int featureName, int value)
{
    bool result = false;
    int reSetCount = 50;
    for (int num = 0; num <= reSetCount; num++)
    {
        GX_STATUS emStatus  = GXSetInt(m_handle, featureName, value);
        if(emStatus == GX_STATUS_SUCCESS)
        {
            result = true;
            qDebug() << QString("%1为（%2）的大恒相机%3参数（%4 = %5）成功。")
                        .arg(m_cameraType)
                        .arg(m_cameraInfo)
                        .arg("set")
                        .arg(getStringByFeatureName(featureName))
                        .arg(QString::number(value));
            break;
        }
        else
        {
            result = false;
            size_t size             = 0;
            char *errorInfo         = NULL;
            GX_STATUS errorCode     = emStatus;
            emStatus = GXGetLastError(&errorCode, NULL, &size);
            errorInfo = new char[size];
            if (errorInfo == NULL)
            {
                continue;
            }
            emStatus = GXGetLastError(&errorCode, errorInfo, &size);
            if (num != reSetCount)
            {
                qWarning() << QString("%1为（%2）的大恒相机%3参数（%4 = %5）失败，错误号：%6，错误信息：%7。")
                              .arg(m_cameraType)
                              .arg(m_cameraInfo)
                              .arg("set")
                              .arg(getStringByFeatureName(featureName))
                              .arg(QString::number(value))
                              .arg(QString::number(errorCode))
                              .arg(QString(QLatin1String(errorInfo)));
            }
            else
            {
                qCritical() << QString("%1为（%2）的大恒相机%3参数（%4 = %5）失败，错误号：%6，错误信息：%7。")
                               .arg(m_cameraType)
                               .arg(m_cameraInfo)
                               .arg("set")
                               .arg(getStringByFeatureName(featureName))
                               .arg(QString::number(value))
                               .arg(QString::number(errorCode))
                               .arg(QString(QLatin1String(errorInfo)));
            }
            if (errorInfo != NULL)
            {
                delete[] errorInfo;
                errorInfo = NULL;
            }
            QEventLoop eventloop;
            QTimer::singleShot(20, &eventloop, SLOT(quit()));
            eventloop.exec();
            continue;
        }
    }
    return result;
}

void DahengAreaCamera::printNotCreatDeviceHandleErrorInfo(const QString action, const QString because)
{
    qWarning() << QString("为%1为%2的大恒相机%3失败，错误原因：%4").arg(m_cameraType).arg(m_cameraInfo).arg(action).arg(because);
}

void DahengAreaCamera::printStateErrorInfo(const QString action, const QString because)
{
    qWarning() << QString("%1%2为%3的大恒相机失败，错误原因：%4").arg(action).arg(m_cameraType).arg(m_cameraInfo).arg(because);
}

void DahengAreaCamera::printDeviceControlStateInfo(const QString action, bool isSuccess, int errorCode)
{
    if(!isSuccess) {
        qCritical() << QString("%1%2为%3的大恒相机失败，错误号：%4").arg(action).arg(m_cameraType).arg(m_cameraInfo).arg(QString::number(errorCode));
    } else {
        qDebug() << QString("%1%2为%3的大恒相机成功").arg(action).arg(m_cameraType).arg(m_cameraInfo);
    }
}

bool DahengAreaCamera::printCameraFeatureInfo_Int(int featureName, int value, const QString &action, GX_STATUS status)
{
    return printCameraFeatureInfo_String(featureName, QString::number(value), action, status);
}

bool DahengAreaCamera::printCameraFeatureInfo_Float(int featureName, float value, const QString &action, GX_STATUS status)
{
    return printCameraFeatureInfo_String(featureName, QString::number(value), action, status);
}

bool DahengAreaCamera::printCameraFeatureInfo_String(int featureName, const QString value, const QString &action, GX_STATUS status)
{
    if(status == GX_STATUS_SUCCESS) {
        qDebug() << QString("%1为（%2）的大恒相机%3参数（%4 = %5）成功。")
                    .arg(m_cameraType)
                    .arg(m_cameraInfo)
                    .arg(action)
                    .arg(getStringByFeatureName(featureName))
                    .arg(value);
        return true;
    } else {
        //get error info
        size_t errorSize        = 0;
        char *errorInfo         = NULL;
        GX_STATUS currentStatus = GX_STATUS_SUCCESS;
        GX_STATUS errorCode     = status;
        currentStatus = GXGetLastError(&errorCode, NULL, &errorSize);
        errorInfo = new char[errorSize];
        if (errorInfo == NULL) {
            return false;
        }
        currentStatus = GXGetLastError(&errorCode, errorInfo, &errorSize);
        qCritical() << QString("%1为（%2）的大恒相机%3参数（%4 = %5）失败，错误号：%6，错误信息：%7。")
                       .arg(m_cameraType)
                       .arg(m_cameraInfo)
                       .arg(action)
                       .arg(getStringByFeatureName(featureName))
                       .arg(value)
                       .arg(QString::number(errorCode))
                       .arg(QString(QLatin1String(errorInfo)));
        if (errorInfo != NULL) {
            delete[] errorInfo;
            errorInfo = NULL;
        }
        return false;
    }
}

#endif // GLOBAL_DAHENG
