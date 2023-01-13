#include <QDebug>
#include <QHostAddress>
#include "HaikangNetCamera.h"

#ifdef GLOBAL_HAIKANG
#include "MvCameraControl.h"
#include "MvErrorDefine.h"

HaikangNetCamera::HaikangNetCamera(const MV_CC_DEVICE_INFO &deviceInfo)
{
    memcpy(&m_deviceInfo, &deviceInfo, sizeof(deviceInfo));
    m_ipAddress = QHostAddress(deviceInfo.SpecialInfo.stGigEInfo.nCurrentIp).toString();
}

HaikangNetCamera::~HaikangNetCamera()
{
    if (m_handle) {
        if (m_isGrabbing)
            stopGrabbing();

        if (m_isOpened)
            closeDevice();
    }
}

bool HaikangNetCamera::enumDevices(QMap<QString, MV_CC_DEVICE_INFO> &cameraInfoList)
{
    MV_CC_DEVICE_INFO_LIST deviceInfoList;
    deviceInfoList.nDeviceNum = 0;

    int ret = MV_CC_EnumDevices(MV_GIGE_DEVICE, &deviceInfoList);
    if (MV_OK == ret) {
        qDebug() << QString("网络中扫描到%1个海康网络相机").arg(deviceInfoList.nDeviceNum);
        for (quint32 ii = 0; ii < deviceInfoList.nDeviceNum; ++ii) {
            MV_CC_DEVICE_INFO deviceInfo;
            memset(&deviceInfo, 0, sizeof(MV_CC_DEVICE_INFO));
            memcpy(&deviceInfo, deviceInfoList.pDeviceInfo[ii], sizeof(MV_CC_DEVICE_INFO));
            QString ip = QHostAddress(deviceInfo.SpecialInfo.stGigEInfo.nCurrentIp).toString();
            cameraInfoList[ip] = deviceInfo;
            qDebug() << QString("当前获取到的海康网络相机ip是：%1").arg(ip);
        }

        if (cameraInfoList.size() > 0) {
            return true;
        } else {
            qCritical() << QString("网络中未扫描到海康网络相机");
            return false;
        }
    } else {
        qCritical() << QString("扫描网络中的海康相机失败，错误码：%1").arg(errorNoToHex(ret));
        return false;
    }
}

bool HaikangNetCamera::getCameraDeviceNum(unsigned int &deviceNum)
{
    bool result = false;
    QMap<QString, MV_CC_DEVICE_INFO> cameraInfoList;
    result = enumDevices(cameraInfoList);
    deviceNum = cameraInfoList.size();
    return result;
}

bool HaikangNetCamera::createHandle()
{
    if (m_handle != NULL) {
        printNotCreatDeviceHandleErrorInfo(QString("创建句柄"), QString("该句柄已经创建"));
        return true;
    }
    int ret = MV_CC_CreateHandle(&m_handle, &m_deviceInfo);
    if (MV_OK != ret) {
        qCritical() << QString("为IP为%1的海康相机创建句柄失败，错误码：%2").arg(m_ipAddress).arg(errorNoToHex(ret));
        return false;
    }
    qDebug() << QString("为IP为%1的海康相机创建句柄成功").arg(m_ipAddress);
    return true;
}

bool HaikangNetCamera::destroyHandle()
{
    if (m_handle == NULL) {
        printNotCreatDeviceHandleErrorInfo(QString("释放句柄"), QString("该句柄未被创建"));
        return true;
    }
    int ret = MV_CC_DestroyHandle(m_handle);
    if (MV_OK != ret) {
        qCritical() << QString("为IP为%1的海康相机释放句柄失败，错误码：%2").arg(m_ipAddress).arg(errorNoToHex(ret));
        return false;
    }
    m_handle = nullptr;
    qDebug() << QString("为IP为%1的海康相机释放句柄成功").arg(m_ipAddress);
    return true;
}

bool HaikangNetCamera::openDevice()
{
    if (m_handle == NULL) {
        qWarning() << QString("打开IP为%1的海康相机失败，错误原因：该相机句柄未被创建").arg(m_ipAddress);
        m_isOpened = false;
        return false;
    }
    if (m_isOpened) {
        printStateErrorInfo(QString("打开"), QString("该相机已被打开"));
        return true;
    }
    bool result = false;
    int resultCode = MV_CC_OpenDevice(m_handle);
    if (MV_OK == resultCode) {
        result = true;
        m_isOpened = true;
    } else {
        m_isOpened = false;
    }
    printDeviceControlStateInfo(QString("打开"), result, resultCode);
    return result;
}

bool HaikangNetCamera::closeDevice()
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("关闭"), QString("该相机未被打开"));
        return true;
    }

    if (m_isGrabbing) {
        stopGrabbing();
    }

    int resultCode = MV_CC_CloseDevice(m_handle);
    bool result = false;
    if (MV_OK == resultCode) {
        result = true;
        m_isOpened = false;
    }
    printDeviceControlStateInfo(QString("关闭"), result, resultCode);
//    if (result) {
//        result &= destroyHandle(); //释放句柄，便于下次再打开
//    }
    return result;
}

bool HaikangNetCamera::startGrabbing()
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("开始采集"), QString("该相机未被打开"));
        return false;
    }

    if (m_isGrabbing) {
        printStateErrorInfo(QString("开始采集"), QString("该相机已经开启采集"));
        return true;
    }

    /* 2000万像素相机偶尔出现MV_CC_StartGrabbing调用失败的情况，当失败时，重试10次 */
    bool result = false;
    int resultCode = MV_OK;
    int maxTryTimes = 10;
    do {
        resultCode = MV_CC_StartGrabbing(m_handle);
        if (MV_OK == resultCode) {
            result = true;
            m_isGrabbing = true;
            break;
        } else {
            result = false;
            qWarning() << QString("%1IP为%2的海康相机失败，错误号：%3").arg(QString("开始采集")).arg(m_ipAddress).arg(errorNoToHex(resultCode));
        }
    } while (maxTryTimes-- > 0);
    printDeviceControlStateInfo(QString("开始采集"), result, resultCode);
    return result;
}

bool HaikangNetCamera::stopGrabbing()
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
    int resultCode = MV_CC_StopGrabbing(m_handle);
    if (MV_OK == resultCode) {
        m_isGrabbing = false;
        result = true;
    }
    printDeviceControlStateInfo(QString("停止采集"), result, resultCode);
    return result;
}

bool HaikangNetCamera::setImageBuffer(unsigned int imageBuffer)
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("设置图像缓冲区数量"), QString("该相机未被打开"));
        return false;
    }

    // 设置图像缓冲区数量，以解决偶发性的图片丢帧、错位的问题
    int ret = MV_CC_SetImageNodeNum(m_handle, imageBuffer);
    if (MV_OK != ret) {
        qCritical() << QString("给IP为%1的海康网口相机，设置图像缓冲区数量失败, 错误码：%2").arg(m_ipAddress).arg(errorNoToHex(ret));
        return false;
    } else {
        qDebug() << QString("给IP为%1的海康网口相机，设置图像缓冲区数量为%2成功").arg(m_ipAddress).arg(imageBuffer);
        return true;
    }
}

bool HaikangNetCamera::setResend(unsigned int maxResendPercent, unsigned int resendTimeout)
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("设置重发模式"), QString("该相机未被打开"));
        return false;
    }

    int ret = MV_GIGE_SetResend(m_handle, 1, maxResendPercent, resendTimeout);
    if (MV_OK != ret) {
        qCritical() << QString("给IP为%1的海康网口相机设置重发失败, 错误码：%2")
                       .arg(m_ipAddress).arg(errorNoToHex(ret));
        return false;
    } else {
        qDebug() << QString("给IP为%1的海康网口相机相机设置重发成功").arg(m_ipAddress);
        return true;
    }
}

bool HaikangNetCamera::setGevSCPD(unsigned int gevSCPD)
{
    return setIntValue("GevSCPD", gevSCPD);
}

bool HaikangNetCamera::setPacketSize(unsigned int packetSize)
{
    return setIntValue("GevSCPSPacketSize", packetSize);
}

bool HaikangNetCamera::setAutoPacketSize()
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("设置自动包长"), QString("该相机未被打开"));
        return false;
    }

    int nPacketSize = MV_CC_GetOptimalPacketSize(m_handle);
    return setIntValue("GevSCPSPacketSize", nPacketSize);
}

bool HaikangNetCamera::setHeartbeatEnable(bool enable)
{
    return setBoolVaule("GevGVCPHeartbeatDisable", !enable);
}

bool HaikangNetCamera::setHeartbeatTime(unsigned int heartbeatTime)
{
    return setIntValue("GevHeartbeatTimeout", heartbeatTime);
}

bool HaikangNetCamera::setAcquisitionMode(HaikangAcquisitionMode acquisitionMode)
{
    return setEnumValue("AcquisitionMode", acquisitionMode);
}

bool HaikangNetCamera::setHardwareTrigger(HaikangTriggerSource triggerSource)
{
    bool ret = true;
    ret &= setEnumValue("TriggerMode", 1);
    ret &= setEnumValue("TriggerSource", triggerSource);
    return ret;
}

bool HaikangNetCamera::setSoftwareTrigger()
{
    bool ret = true;
    ret &= setEnumValue("TriggerMode", 1);
    ret &= setEnumValue("TriggerSource", MV_TRIGGER_SOURCE_SOFTWARE);
    return ret;
}

bool HaikangNetCamera::sendSoftTriggerCommand()
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("发送软触发命令"), QString("该相机未被打开"));
        return false;
    }

    if (!m_isGrabbing) {
        printStateErrorInfo(QString("发送软触发命令"), QString("该相机已经停止采集"));
        return false;
    }

    int ret = MV_CC_SetCommandValue(m_handle, "TriggerSoftware");
    if (MV_OK == ret) {
        qDebug() << QString("给IP为%1的海康网口相机发送软触发命令成功").arg(m_ipAddress);
        return true;
    } else {
        qCritical() << QString("给IP为%1的海康网口相机发送软触发命令失败，错误码：%2").arg(m_ipAddress).arg(errorNoToHex(ret));
        return false;
    }
}

bool HaikangNetCamera::setTriggerActivation(HaikangTriggerActivation triggerActivation)
{
    return setEnumValue("TriggerActivation", triggerActivation);
}

bool HaikangNetCamera::setlineDebouncerTime(unsigned int debouncerTime)
{
    //老4K和面阵相机
    bool result = setIntValue("LineDebouncerTime", debouncerTime);
    if (!result) {
        //新4K相机配置
        result = setIntValue("LineDebouncerTimeNs", debouncerTime);
    }
    return result;
}

bool HaikangNetCamera::setTriggerDelayTime(float delayTime)
{
    return setFloatValue("TriggerDelay", delayTime);
}

bool HaikangNetCamera::isDeviceAccessible()
{
    if (!m_isOpened) {
        printStateErrorInfo(QString("获取相机状态"), QString("该相机未被打开"));
        return false;
    }

    if (MV_OK != MV_CC_IsDeviceAccessible(&m_deviceInfo, 1)) {
        return false;
    }

    return true;
}

bool HaikangNetCamera::isDeviceOpenned()
{
    return m_isOpened;
}

bool HaikangNetCamera::isGrabbing()
{
    return m_isGrabbing;
}

bool HaikangNetCamera::isAreaScan(bool &isAreaScanCamera)
{
    unsigned int iCameraType = 0;
    bool result = getEnumValue("DeviceScanType", iCameraType);

    if(result) {
        if(iCameraType == 1) {
            isAreaScanCamera = false;
        } else {
            isAreaScanCamera = true;
        }
    }
    return result;
}

bool HaikangNetCamera::isColor(bool &isColorCamera)
{
    QString strDeviceName = "";
    bool result = getStringValue("DeviceModelName", strDeviceName);
    if(strDeviceName.right(3).contains("M", Qt::CaseSensitive)) {
        isColorCamera = false;
    } else {
        isColorCamera = true;
    }
    return result;
}

bool HaikangNetCamera::registerImageCallBack(PhotoReceivedCallBack callBack, void *user)
{
    if (m_handle == NULL) {
        printNotCreatDeviceHandleErrorInfo(QString("注册回调函数"), QString("相机句柄未被创建"));
        return false;
    }
    int ret = MV_CC_RegisterImageCallBackEx(m_handle, callBack, user);
    if (MV_OK != ret) {
        qCritical() << QString("为IP为%1的海康相机注册回调函数失败，错误码：%2").arg(m_ipAddress).arg(errorNoToHex(ret));
        return false;
    }
    qDebug() << QString("为IP为%1的海康相机注册回调函数成功").arg(m_ipAddress);
    return true;
}

bool HaikangNetCamera::setRoi(unsigned int offset_X, unsigned int offset_Y, unsigned int width, unsigned height)
{
    bool result = true;
    result &= setIntValue("OffsetX", 0);
    result &= setIntValue("OffsetY", 0);

    result &= setIntValue("OffsetX", offset_X);
    result &= setIntValue("OffsetY", offset_Y);
    result &= setIntValue("Width",   width);
    result &= setIntValue("Height",  height);

    return result;
}

bool HaikangNetCamera::setOffset_X(unsigned int offset_X)
{
    return setIntValue("OffsetX", offset_X);
}

bool HaikangNetCamera::setImage_Width(unsigned int width)
{
    return setIntValue("Height", width);
}

bool HaikangNetCamera::setImage_Height(unsigned int height)
{
    return setIntValue("Width", height);
}

bool HaikangNetCamera::setTriggerSource(HaikangTriggerSource triggerSource)
{
    return setEnumValue("TriggerSource", triggerSource);
}

bool HaikangNetCamera::setTriggerMode(bool triggerMode)
{
    bool result = true;
    if (triggerMode) {
        result = setEnumValue("TriggerMode", 1);
    } else {
        result = setEnumValue("TriggerMode", 0);
    }

    return result;
}

bool HaikangNetCamera::setExposureTime(float exposureTime)
{
    return setFloatValue("ExposureTime", exposureTime);
}

bool HaikangNetCamera::setGammaEnable(bool enable)
{
    return setBoolVaule("GammaEnable", enable);
}

bool HaikangNetCamera::setGamma(float gamma)
{
    return setFloatValue("Gamma", gamma);
}

bool HaikangNetCamera::setGain(float gain)
{
    return setFloatValue("Gain", gain);
}

bool HaikangNetCamera::getRoi(unsigned int &offset_X, unsigned int &offset_Y, unsigned int &width, unsigned int &height)
{
    bool result = true;
    result &= getIntValue("OffsetX", offset_X);
    result &= getIntValue("OffsetY", offset_Y);
    result &= getIntValue("Width",   width);
    result &= getIntValue("Height",  height);
    return result;
}

bool HaikangNetCamera::getOffset_X(unsigned int &offset_X)
{
    return getIntValue("OffsetX", offset_X);
}

bool HaikangNetCamera::getImage_Width(unsigned int &width)
{
    return getIntValue("Width", width);
}

bool HaikangNetCamera::getImage_Height(unsigned int &height)
{
    return getIntValue("Height", height);
}

bool HaikangNetCamera::getTriggerSource(HaikangTriggerSource &triggerSource)
{
    unsigned int triggerSourceValue = 0;
    bool result = getEnumValue("TriggerSource", triggerSourceValue);
    triggerSource = HaikangTriggerSource(triggerSourceValue);
    return result;
}

bool HaikangNetCamera::getTriggerMode(bool &triggerMode)
{
    unsigned int cameraTriggerModee = 0;
    bool result = getEnumValue("TriggerMode", cameraTriggerModee);

    if(cameraTriggerModee == 1) {
        triggerMode = true;
    } else {
        triggerMode = false;
    }

    return result;
}

bool HaikangNetCamera::getExposureTime(float &exposureTime)
{
    return getFloatValue("ExposureTime", exposureTime);
}

bool HaikangNetCamera::getGammaEnable(bool &enable)
{
    return getBoolVaule("GammaEnable", enable);
}

bool HaikangNetCamera::getGamma(float &gamma)
{
    return getFloatValue("Gamma", gamma);
}

bool HaikangNetCamera::getGain(float &gain)
{
    return getFloatValue("Gain", gain);
}

bool HaikangNetCamera::setReverse_X(bool enable)
{
    return setBoolVaule("ReverseX", enable);
}

bool HaikangNetCamera::setReverse_Y(bool enable)
{
    return setBoolVaule("ReverseY", enable);
}

bool HaikangNetCamera::setIntValue(const QString &featureName, unsigned int value)
{
    if (m_isOpened) {
        int ret = MV_CC_SetIntValue(m_handle, featureName.toStdString().c_str(), value);
        return printCameraFeatureInfo_Int(featureName, value, "设置", ret);
    } else {
        qWarning() << QString("不能设置相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool HaikangNetCamera::setFloatValue(const QString &featureName, float value)
{
    if (m_isOpened) {
        int ret = MV_CC_SetFloatValue(m_handle, featureName.toStdString().c_str(), value);
        return printCameraFeatureInfo_Float(featureName, value, "设置", ret);
    } else {
        qWarning() << QString("不能设置相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool HaikangNetCamera::setEnumValue(const QString &featureName, unsigned int value)
{
    if (m_isOpened) {
        int ret = MV_CC_SetEnumValue(m_handle, featureName.toStdString().c_str(), value);
        return printCameraFeatureInfo_Int(featureName, value, "设置", ret);
    } else {
        qWarning() << QString("不能设置相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool HaikangNetCamera::setBoolVaule(const QString &featureName, bool value)
{
    if (m_isOpened) {
        int ret = MV_CC_SetBoolValue(m_handle, featureName.toStdString().c_str(), value);
        return printCameraFeatureInfo_Bool(featureName, value, "设置", ret);
    } else {
        qWarning() << QString("不能设置相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool HaikangNetCamera::setStringValue(const QString &featureName, const QString &value)
{
    if (m_isOpened) {
        int ret = MV_CC_SetStringValue(m_handle, featureName.toStdString().c_str(), value.toStdString().c_str());
        return printCameraFeatureInfo_String(featureName, value, "设置", ret);
    } else {
        qWarning() << QString("不能设置相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool HaikangNetCamera::getIntValue(const QString &featureName, unsigned int &value)
{
    if (m_isOpened) {
        MVCC_INTVALUE getParam;
        memset(&getParam, 0, sizeof(MVCC_INTVALUE));
        int result = MV_CC_GetIntValue(m_handle, featureName.toStdString().c_str(), &getParam);
        value = -1;
        if (MV_OK == result) {
            value = getParam.nCurValue;
        }
        return printCameraFeatureInfo_Int(featureName, value, "获取", result);
    } else {
        qWarning() << QString("不能获取相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool HaikangNetCamera::getFloatValue(const QString &featureName, float &value)
{
    if (m_isOpened) {
        MVCC_FLOATVALUE getParam;
        memset(&getParam, 0, sizeof(MVCC_FLOATVALUE));
        int result = MV_CC_GetFloatValue(m_handle, featureName.toStdString().c_str(), &getParam);
        value = -1;
        if (MV_OK == result) {
            value = getParam.fCurValue;
        }
        return printCameraFeatureInfo_Float(featureName, value, "获取", result);
    } else {
        qWarning() << QString("不能获取相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool HaikangNetCamera::getEnumValue(const QString &featureName, unsigned int &value)
{
    if (m_isOpened) {
        MVCC_ENUMVALUE getParam;
        memset(&getParam, 0, sizeof(MVCC_ENUMVALUE));
        int result = MV_CC_GetEnumValue(m_handle, featureName.toStdString().c_str(), &getParam);
        value = -1;
        if (MV_OK == result) {
            value = getParam.nCurValue;
        }
        return printCameraFeatureInfo_Int(featureName, value, "获取", result);
    } else {
        qWarning() << QString("不能获取相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool HaikangNetCamera::getBoolVaule(const QString &featureName, bool &value)
{
    if (m_isOpened) {
        value = false;
        int result = MV_CC_GetBoolValue(m_handle, featureName.toStdString().c_str(), &value);
        return printCameraFeatureInfo_Bool(featureName, value, "获取", result);
    } else {
        qWarning() << QString("不能获取相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

bool HaikangNetCamera::getStringValue(const QString &featureName, QString &value)
{
    if (m_isOpened) {
        MVCC_STRINGVALUE getParam;
        memset(&getParam, 0, sizeof(MVCC_STRINGVALUE));
        int result = MV_CC_GetStringValue(m_handle, featureName.toStdString().c_str(), &getParam);
        value = QString::number(-1);
        if (MV_OK == result) {
            value = QString(getParam.chCurValue);
        }
        return printCameraFeatureInfo_String(featureName, value, "获取", result);
    } else {
        qWarning() << QString("不能获取相机参数（%1），因为相机没有被打开。").arg(featureName);
        return false;
    }
}

QString HaikangNetCamera::errorNoToHex(int error)
{
    return QString::number(error, 16).toUpper();
}

void HaikangNetCamera::printNotCreatDeviceHandleErrorInfo(const QString action, const QString because)
{
    qWarning() << QString("为IP为%1的海康相机%2失败，错误原因：%3").arg(m_ipAddress).arg(action).arg(because);
}

void HaikangNetCamera::printStateErrorInfo(const QString action, const QString because)
{
    qWarning() << QString("%1IP为%2的海康相机失败，错误原因：%3").arg(action).arg(m_ipAddress).arg(because);
}

void HaikangNetCamera::printDeviceControlStateInfo(const QString action, bool isSuccess, int errorCode)
{
    if(!isSuccess) {
        qCritical() << QString("%1IP为%2的海康相机失败，错误号：%3").arg(action).arg(m_ipAddress).arg(errorNoToHex(errorCode));
    } else {
        qDebug() << QString("%1IP为%2的海康相机成功").arg(action).arg(m_ipAddress);
    }
}

bool HaikangNetCamera::printCameraFeatureInfo_Int(const QString &featureName, int value, const QString &action, int status)
{
    return printCameraFeatureInfo_String(featureName, QString::number(value), action, status);
}

bool HaikangNetCamera::printCameraFeatureInfo_Float(const QString &featureName, float value, const QString &action, int status)
{
    return printCameraFeatureInfo_String(featureName, QString::number(value), action, status);
}

bool HaikangNetCamera::printCameraFeatureInfo_Bool(const QString &featureName, bool value, const QString &action, int status)
{
    return printCameraFeatureInfo_String(featureName, value ? "true":"false", action, status);
}

bool HaikangNetCamera::printCameraFeatureInfo_String(const QString &featureName, QString value, const QString &action, int status)
{
    if(status == MV_OK) {
        qDebug() << QString("IP为（%1）的海康相机%2参数（%3 = %4）成功。")
                    .arg(m_ipAddress)
                    .arg(action)
                    .arg(featureName)
                    .arg(value);
        return true;
    } else {
        qCritical() << QString("IP为（%1）的海康相机%2参数（%3 = %4）失败，错误号：%5。")
                      .arg(m_ipAddress)
                      .arg(action)
                      .arg(featureName)
                      .arg(value)
                      .arg(errorNoToHex(status));
        return false;
    }
}

#endif // GLOBAL_HAIKANG
