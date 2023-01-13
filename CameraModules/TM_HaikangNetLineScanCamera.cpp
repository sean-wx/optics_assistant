#include <QDebug>
#include <QThread>
#include <QTime>
#include <QHostAddress>
#include "TM_HaikangNetLineScanCamera.h"

#ifdef GLOBAL_HAIKANG
#include "MvCameraControl.h"
#include "MvErrorDefine.h"

TM_HaikangNetLineScanCamera::TM_HaikangNetLineScanCamera(const MV_CC_DEVICE_INFO &deviceInfo, int cameraId)
    : AbstractCamera(),
      m_cameraHandle(std::make_shared<HaikangNetLineScanCamera>(deviceInfo))
{
    m_cameraId = cameraId;
}

TM_HaikangNetLineScanCamera::~TM_HaikangNetLineScanCamera()
{

}

bool TM_HaikangNetLineScanCamera::init()
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
            && m_cameraHandle->setAcquisitionMode(HaikangNetLineScanCamera::AcquisitionModeContinuous)
            && m_cameraHandle->setHardwareTrigger()
            && m_cameraHandle->setTriggerActivation(HaikangNetLineScanCamera::TriggerActivationRisingEdge)
            && m_cameraHandle->setlineDebouncerTime(100)
            && m_cameraHandle->setTriggerDelayTime(0);
}

bool TM_HaikangNetLineScanCamera::close()
{
    bool result = m_cameraHandle->closeDevice();
    result &= m_cameraHandle->destroyHandle();
    return result;
}

bool TM_HaikangNetLineScanCamera::start()
{
    return m_cameraHandle->startGrabbing();
}

bool TM_HaikangNetLineScanCamera::stop()
{
    return m_cameraHandle->stopGrabbing();
}

bool TM_HaikangNetLineScanCamera::enumDevices(QMap<QString, MV_CC_DEVICE_INFO> &cameraInfoList)
{
    return HaikangNetLineScanCamera::enumDevices(cameraInfoList);
}

int TM_HaikangNetLineScanCamera::getCamerDeviceNum()
{
    unsigned int cameraNum = 0;
    HaikangNetLineScanCamera::getCameraDeviceNum(cameraNum);
    return cameraNum;
}

bool TM_HaikangNetLineScanCamera::setHardwareTrigger()
{
    return m_cameraHandle->setHardwareTrigger();
}

bool TM_HaikangNetLineScanCamera::setSoftwareTrigger()
{
    return m_cameraHandle->setSoftwareTrigger();
}

bool TM_HaikangNetLineScanCamera::setImageBuffer(int bufferNumber)
{
    return m_cameraHandle->setImageBuffer(bufferNumber);
}

bool TM_HaikangNetLineScanCamera::setReverse_X(bool enable)
{
    return m_cameraHandle->setReverse_X(enable);
}

bool TM_HaikangNetLineScanCamera::setReverse_Y(bool enable)
{
    return m_cameraHandle->setReverse_Y(enable);
}

bool TM_HaikangNetLineScanCamera::isOpen()
{
    return m_cameraHandle->isDeviceOpenned();
}

bool TM_HaikangNetLineScanCamera::isGrab()
{
    return m_cameraHandle->isGrabbing();
}

bool TM_HaikangNetLineScanCamera::isAreaScan(bool &isAreaScanCamera)
{
    return m_cameraHandle->isAreaScan(isAreaScanCamera);
}

bool TM_HaikangNetLineScanCamera::isColor(bool &isColorCamera)
{
    return m_cameraHandle->isColor(isColorCamera);
}

bool TM_HaikangNetLineScanCamera::sendSoftTriggerCommand()
{
    return m_cameraHandle->sendSoftTriggerCommand();
}

void TM_HaikangNetLineScanCamera::photoReceivedCallBack(unsigned char *pData, MV_FRAME_OUT_INFO_EX *pFrameInfo, void *pUser)
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

        TM_HaikangNetLineScanCamera *self = static_cast<TM_HaikangNetLineScanCamera*>(pUser);
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

        TM_HaikangNetLineScanCamera *self = static_cast<TM_HaikangNetLineScanCamera*>(pUser);
        self->photoReceived(photo, copyCost);
    }
}

bool TM_HaikangNetLineScanCamera::setRoi(int roiX, int roiY, int roiW, int roiH)
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

bool TM_HaikangNetLineScanCamera::setGain(float gain)
{
    return m_cameraHandle->setGain(gain);
}

bool TM_HaikangNetLineScanCamera::setPreampGain(float preampGain)
{
    return m_cameraHandle->setPreampGain(preampGain);
}

bool TM_HaikangNetLineScanCamera::setGamma(float gama)
{
    return m_cameraHandle->setGamma(gama);
}

bool TM_HaikangNetLineScanCamera::setExposureTime(float exposure)
{
    return m_cameraHandle->setExposureTime(exposure);
}

bool TM_HaikangNetLineScanCamera::setTriggerSource(int triggerSource)
{
    bool result = true;
    switch (triggerSource)
    {
    case SOURCE_SOFRWARE:
        result = m_cameraHandle->setTriggerSource(HaikangNetLineScanCamera::TriggerSourceSoftWare);
        break;
    case SOURCE_LINE0:
        result = m_cameraHandle->setTriggerSource(HaikangNetLineScanCamera::TriggerSourceLine0);
        break;
    case SOURCE_LINE1:
        result = m_cameraHandle->setTriggerSource(HaikangNetLineScanCamera::TriggerSourceLine1);
        break;
    case SOURCE_LINE2:
        result = m_cameraHandle->setTriggerSource(HaikangNetLineScanCamera::TriggerSourceLine2);
        break;
    case SOURCE_LINE3:
        result = m_cameraHandle->setTriggerSource(HaikangNetLineScanCamera::TriggerSourceLine3);
        break;
    default:
        qDebug() << QString("设置海康面阵相机触发源失败，错误代码是：%1").arg(triggerSource);
        break;
    }
    return result;
}

bool TM_HaikangNetLineScanCamera::setTriggerMode(bool triggerMode)
{
    return m_cameraHandle->setTriggerMode(triggerMode);
}

bool TM_HaikangNetLineScanCamera::setGammaEnable(bool gammaEnable)
{
    return m_cameraHandle->setGammaEnable(gammaEnable);
}

bool TM_HaikangNetLineScanCamera::setLineRate(float rate)
{
    unsigned int lineRate = rate;
    return m_cameraHandle->setLineRate(lineRate);
}

bool TM_HaikangNetLineScanCamera::getRoi(int &roiX, int &roiY, int &roiW, int &roiH)
{
    unsigned int x, y, width ,hight;
    bool result = m_cameraHandle->getRoi(x, y, width ,hight);
    roiX = x;
    roiY = y;
    roiW = width;
    roiH = hight;
    return result;
}

bool TM_HaikangNetLineScanCamera::getGain(float &gain)
{
    return m_cameraHandle->getGain(gain);
}

bool TM_HaikangNetLineScanCamera::getPreampGain(float &preampGain)
{
    return m_cameraHandle->getPreampGain(preampGain);
}

bool TM_HaikangNetLineScanCamera::getGamma(float &gamma)
{
    return m_cameraHandle->getGamma(gamma);
}

bool TM_HaikangNetLineScanCamera::getExposureTime(float &exposure)
{
    return m_cameraHandle->getExposureTime(exposure);
}

bool TM_HaikangNetLineScanCamera::getTriggerSource(int &triggerSource)
{
    HaikangNetLineScanCamera::HaikangTriggerSource tgiggerSourceNum;
    bool result = m_cameraHandle->getTriggerSource(tgiggerSourceNum);
    switch (tgiggerSourceNum)
    {
    case HaikangNetLineScanCamera::TriggerSourceSoftWare:
        triggerSource = SOURCE_SOFRWARE;
        break;
    case HaikangNetLineScanCamera::TriggerSourceLine0:
        triggerSource = SOURCE_LINE0;
        break;
    case HaikangNetLineScanCamera::TriggerSourceLine1:
        triggerSource = SOURCE_LINE1;
        break;
    case HaikangNetLineScanCamera::TriggerSourceLine2:
        triggerSource = SOURCE_LINE2;
        break;
    case HaikangNetLineScanCamera::TriggerSourceLine3:
        triggerSource = SOURCE_LINE3;
        break;
    default:
        break;
    }

    return result;
}

bool TM_HaikangNetLineScanCamera::getTriggerMode(bool &triggerMode)
{
    return m_cameraHandle->getTriggerMode(triggerMode);
}

bool TM_HaikangNetLineScanCamera::getGammaEnable(bool &gammaEnable)
{
    return m_cameraHandle->getGammaEnable(gammaEnable);
}

bool TM_HaikangNetLineScanCamera::getLineRate(float &rate)
{
    unsigned int lineRate = 0;
    bool result = m_cameraHandle->getLineRate(lineRate);
    rate = lineRate;
    return result;
}

bool TM_HaikangNetLineScanCamera::isAreaLScan()
{
    bool isAreaLScan = false;
    m_cameraHandle->isAreaScan(isAreaLScan);
    return isAreaLScan;
}

#endif // GLOBAL_HAIKANG
