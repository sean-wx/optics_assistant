#include <QDebug>
#include <QThread>
#include <QTime>
#include <QHostAddress>
#include "TM_HaikangNetAreaScanCamera.h"

#ifdef GLOBAL_HAIKANG
#include "MvCameraControl.h"
#include "MvErrorDefine.h"

TM_HaikangNetAreaScanCamera::TM_HaikangNetAreaScanCamera(const MV_CC_DEVICE_INFO &deviceInfo, int cameraId)
    : AbstractCamera(),
      m_cameraHandle(std::make_shared<HaikangNetAreaCamera>(deviceInfo))
{
    m_cameraId = cameraId;
}

TM_HaikangNetAreaScanCamera::~TM_HaikangNetAreaScanCamera()
{

}

bool TM_HaikangNetAreaScanCamera::init()
{
    return m_cameraHandle->createHandle()
            && m_cameraHandle->registerImageCallBack(photoReceivedCallBack, this)
            && m_cameraHandle->openDevice()
            && m_cameraHandle->setImageBuffer(16)
            && m_cameraHandle->setAutoPacketSize()
            && m_cameraHandle->setGevSCPD(50)
            && m_cameraHandle->setResend(100, 3000)
            && m_cameraHandle->setHeartbeatEnable(true)
            && m_cameraHandle->setHeartbeatTime(10000)
            && m_cameraHandle->setAcquisitionMode(HaikangNetAreaCamera::AcquisitionModeContinuous)
            && m_cameraHandle->setHardwareTrigger()
            && m_cameraHandle->setTriggerActivation(HaikangNetAreaCamera::TriggerActivationRisingEdge)
            && m_cameraHandle->setlineDebouncerTime(100)
            && m_cameraHandle->setAcquisitionFrameRateEnable(true)
            && m_cameraHandle->setAcquisitionFrameRate(2)
            && m_cameraHandle->setTriggerDelayTime(0);
}

bool TM_HaikangNetAreaScanCamera::close()
{
    bool result = m_cameraHandle->closeDevice();
    result &= m_cameraHandle->destroyHandle();
    return result;
}

bool TM_HaikangNetAreaScanCamera::start()
{
    return m_cameraHandle->startGrabbing();
}

bool TM_HaikangNetAreaScanCamera::stop()
{
    return m_cameraHandle->stopGrabbing();
}

bool TM_HaikangNetAreaScanCamera::isOpen()
{
    return m_cameraHandle->isDeviceOpenned();
}

bool TM_HaikangNetAreaScanCamera::isGrab()
{
    return m_cameraHandle->isGrabbing();
}

bool TM_HaikangNetAreaScanCamera::isAreaScan(bool &isAreaScanCamera)
{
    return m_cameraHandle->isAreaScan(isAreaScanCamera);
}

bool TM_HaikangNetAreaScanCamera::isColor(bool &isColorCamera)
{
    return m_cameraHandle->isColor(isColorCamera);
}

bool TM_HaikangNetAreaScanCamera::setRoi(int roiX, int roiY, int roiW, int roiH)
{
    bool ret = true;
    /* 设置ROI 设置ROI，设置之前要关闭相机取流 */
    if (m_cameraHandle->isGrabbing()) {
        bool stopCameraGrabbing = m_cameraHandle->stopGrabbing();
        if (stopCameraGrabbing) {
            qCritical("在设置ROI时，停止相机取流失败");
            ret &= false;
        }
    }
    ret &= m_cameraHandle->setRoi(static_cast<unsigned int>(roiX),
                                  static_cast<unsigned int>(roiY),
                                  static_cast<unsigned int>(roiW),
                                  static_cast<unsigned int>(roiH));
    ret &= m_cameraHandle->startGrabbing();
    return ret;
}

bool TM_HaikangNetAreaScanCamera::setGain(float gain)
{
    return m_cameraHandle->setGain(gain);
}

bool TM_HaikangNetAreaScanCamera::setGamma(float gama)
{
    return m_cameraHandle->setGamma(gama);
}

bool TM_HaikangNetAreaScanCamera::setExposureTime(float exposure)
{
    return m_cameraHandle->setExposureTime(exposure);
}

bool TM_HaikangNetAreaScanCamera::setTriggerSource(int triggerSource)
{
    bool result = true;
    switch (triggerSource)
    {
    case SOURCE_SOFRWARE:
        result = m_cameraHandle->setTriggerSource(HaikangNetAreaCamera::TriggerSourceSoftWare);
        break;
    case SOURCE_LINE0:
        result = m_cameraHandle->setTriggerSource(HaikangNetAreaCamera::TriggerSourceLine0);
        break;
    case SOURCE_LINE1:
        result = m_cameraHandle->setTriggerSource(HaikangNetAreaCamera::TriggerSourceLine1);
        break;
    case SOURCE_LINE2:
        result = m_cameraHandle->setTriggerSource(HaikangNetAreaCamera::TriggerSourceLine2);
        break;
    case SOURCE_LINE3:
        result = m_cameraHandle->setTriggerSource(HaikangNetAreaCamera::TriggerSourceLine3);
        break;
    default:
        qDebug() << QString("设置海康面阵相机触发源失败，错误代码是：%1").arg(triggerSource);
        break;
    }
    return result;
}

bool TM_HaikangNetAreaScanCamera::setTriggerMode(bool triggerMode)
{
    return m_cameraHandle->setTriggerMode(triggerMode);
}

bool TM_HaikangNetAreaScanCamera::setGammaEnable(bool gammaEnable)
{
    return m_cameraHandle->setGammaEnable(gammaEnable);
}

bool TM_HaikangNetAreaScanCamera::getRoi(int &roiX, int &roiY, int &roiW, int &roiH)
{
    unsigned int x, y, width ,hight;
    bool result = m_cameraHandle->getRoi(x, y, width ,hight);
    roiX = x;
    roiY = y;
    roiW = width;
    roiH = hight;
    return result;
}

bool TM_HaikangNetAreaScanCamera::getGain(float &gain)
{
    return m_cameraHandle->getGain(gain);
}

bool TM_HaikangNetAreaScanCamera::getGamma(float &gamma)
{
    return m_cameraHandle->getGamma(gamma);
}

bool TM_HaikangNetAreaScanCamera::getExposureTime(float &exposure)
{
    return m_cameraHandle->getExposureTime(exposure);
}

bool TM_HaikangNetAreaScanCamera::getTriggerSource(int &triggerSource)
{
    HaikangNetAreaCamera::HaikangTriggerSource tgiggerSourceNum;
    bool result = m_cameraHandle->getTriggerSource(tgiggerSourceNum);
    switch (tgiggerSourceNum)
    {
    case HaikangNetAreaCamera::TriggerSourceSoftWare:
        triggerSource = SOURCE_SOFRWARE;
        break;
    case HaikangNetAreaCamera::TriggerSourceLine0:
        triggerSource = SOURCE_LINE0;
        break;
    case HaikangNetAreaCamera::TriggerSourceLine1:
        triggerSource = SOURCE_LINE1;
        break;
    case HaikangNetAreaCamera::TriggerSourceLine2:
        triggerSource = SOURCE_LINE2;
        break;
    case HaikangNetAreaCamera::TriggerSourceLine3:
        triggerSource = SOURCE_LINE3;
        break;
    default:
        break;
    }

    return result;
}

bool TM_HaikangNetAreaScanCamera::getTriggerMode(bool &triggerMode)
{
    return m_cameraHandle->getTriggerMode(triggerMode);
}

bool TM_HaikangNetAreaScanCamera::getGammaEnable(bool &gammaEnable)
{
    return m_cameraHandle->getGammaEnable(gammaEnable);
}

bool TM_HaikangNetAreaScanCamera::sendSoftTriggerCommand()
{
    return m_cameraHandle->sendSoftTriggerCommand();
}

bool TM_HaikangNetAreaScanCamera::enumDevices(QMap<QString, MV_CC_DEVICE_INFO> &cameraInfoList)
{
    return HaikangNetAreaCamera::enumDevices(cameraInfoList);
}

bool TM_HaikangNetAreaScanCamera::isAreaLScan()
{
    bool isAreaLScan = false;
    m_cameraHandle->isAreaScan(isAreaLScan);
    return isAreaLScan;
}

int TM_HaikangNetAreaScanCamera::getCamerDeviceNum()
{
    unsigned int cameraNum = 0;
    HaikangNetAreaCamera::getCameraDeviceNum(cameraNum);
    return cameraNum;
}

bool TM_HaikangNetAreaScanCamera::setHardwareTrigger()
{
    return m_cameraHandle->setHardwareTrigger();
}

bool TM_HaikangNetAreaScanCamera::setSoftwareTrigger()
{
    return m_cameraHandle->setSoftwareTrigger();
}

bool TM_HaikangNetAreaScanCamera::setImageBuffer(int bufferNumber)
{
    return m_cameraHandle->setImageBuffer(bufferNumber);
}

bool TM_HaikangNetAreaScanCamera::setReverse_X(bool enable)
{
    return m_cameraHandle->setReverse_X(enable);
}

bool TM_HaikangNetAreaScanCamera::setReverse_Y(bool enable)
{
    return m_cameraHandle->setReverse_Y(enable);
}

bool TM_HaikangNetAreaScanCamera::setAcquisitionFrameRateEnable(bool enable)
{
    return m_cameraHandle->setAcquisitionFrameRateEnable(enable);
}

bool TM_HaikangNetAreaScanCamera::setAcquisitionFrameRate(float frameRate)
{
    return m_cameraHandle->setAcquisitionFrameRate(frameRate);
}

bool TM_HaikangNetAreaScanCamera::getAcquisitionFrameRateEnable(bool &enable)
{
    return m_cameraHandle->getAcquisitionFrameRateEnable(enable);
}

bool TM_HaikangNetAreaScanCamera::getAcquisitionFrameRate(float &frameRate)
{
    return m_cameraHandle->getAcquisitionFrameRate(frameRate);
}

void TM_HaikangNetAreaScanCamera::photoReceivedCallBack(unsigned char *pData, MV_FRAME_OUT_INFO_EX *pFrameInfo, void *pUser)
{
    qDebug() << "当前函数名：" << __FUNCTION__ << "海康相机图像回调线程ID：" << QThread::currentThreadId();
    qDebug() << "海康相机图像指针：" << QString::asprintf("%p", pData);

    if(pFrameInfo->enPixelType == PixelType_Gvsp_Mono8)
    {
        // Step1: 如果是灰度图
        uint size = pFrameInfo->nWidth * pFrameInfo->nHeight;
        QImage photo(pFrameInfo->nWidth, pFrameInfo->nHeight, QImage::Format_Grayscale8);

        QTime t;
        t.start();

        memcpy(photo.bits(), pData, size);

        int copyCost = t.elapsed();
        qDebug() << "拷贝黑白图像时间：" << copyCost;
        qDebug() << "海康相机接收的黑白图像大小是：" << QSize(pFrameInfo->nWidth, pFrameInfo->nHeight);

        TM_HaikangNetAreaScanCamera *self = static_cast<TM_HaikangNetAreaScanCamera*>(pUser);
        self->photoReceived(photo, copyCost);
    }
    else
    {
        // TODO
        // Step2: 如果是彩色图
        uint size = pFrameInfo->nWidth * pFrameInfo->nHeight;
        QImage photo(pFrameInfo->nWidth, pFrameInfo->nHeight, QImage::Format_RGB888);

        QTime t;
        t.start();

        memcpy(photo.bits(), pData, size * 3);

        int copyCost = t.elapsed();
        qDebug() << "拷贝彩色图像时间：" << copyCost;
        qDebug() << "海康相机接收的彩色图像大小是：" << QSize(pFrameInfo->nWidth, pFrameInfo->nHeight);

        TM_HaikangNetAreaScanCamera *self = static_cast<TM_HaikangNetAreaScanCamera*>(pUser);
        self->photoReceived(photo, copyCost);
    }
}

#endif // GLOBAL_HAIKANG
