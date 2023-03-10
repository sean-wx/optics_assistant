#include <QTime>
#include <QDebug>
#include <QThread>
#include "TM_DahengNetAreaScanCamera.h"
#ifdef GLOBAL_DAHENG

TM_DahengNetAreaScanCamera::TM_DahengNetAreaScanCamera(const GX_DEVICE_IP_INFO &deviceInfo, int cameraId)
    : AbstractCamera(),
      m_cameraHandle(std::make_shared<DahengNetAreaCamera>(deviceInfo))
{
    m_cameraId = cameraId;
}

TM_DahengNetAreaScanCamera::~TM_DahengNetAreaScanCamera()
{

}

bool TM_DahengNetAreaScanCamera::initLib()
{
    return DahengNetAreaCamera::initLib();
}

bool TM_DahengNetAreaScanCamera::UnInitLib()
{
    return DahengNetAreaCamera::UnInitLib();
}

bool TM_DahengNetAreaScanCamera::init()
{
    bool ret = m_cameraHandle->openDevice()
            && m_cameraHandle->setImageBuffer(8)
            && m_cameraHandle->registerImageCallBack(photoReceivedCallBack, this)
            && m_cameraHandle->registerCameraOfflineCallBack(cameraOfflineCallBack, this)
            && m_cameraHandle->setPacketSize(8192)
            && m_cameraHandle->setGevSCPD(50)
            && m_cameraHandle->setHeartbeatTime(10000)
            && m_cameraHandle->setAcquisitionMode(DahengNetAreaCamera::AcquisitionModeContinuous)
            && m_cameraHandle->setHardwareTrigger()
            && m_cameraHandle->setTriggerActivation(DahengNetAreaCamera::TriggerActivationRisingEdge)
            && m_cameraHandle->setlineDebouncerTime(50)
            && m_cameraHandle->setTriggerDelayTime(0)
            && m_cameraHandle->setReSendEnable(true)
            && m_cameraHandle->setPacketSizeTimeout(20)
            && m_cameraHandle->setBlockTimeout(3000)
            && m_cameraHandle->setResendTimeout(20)
            && m_cameraHandle->setAcquisitionFrameRateEnable(true)
            && m_cameraHandle->setAcquisitionFrameRate(2)
            && m_cameraHandle->setMaxWaitPacketCount(8192);
    return ret;
}

bool TM_DahengNetAreaScanCamera::close()
{
    bool result = m_cameraHandle->unRegisterImageCallBack();
    result &= m_cameraHandle->unRegisterCameraOfflineCallBack();
    result &= m_cameraHandle->closeDevice();
    return result;
}

bool TM_DahengNetAreaScanCamera::start()
{
    return m_cameraHandle->startGrabbing();
}

bool TM_DahengNetAreaScanCamera::stop()
{
    return m_cameraHandle->stopGrabbing();
}

bool TM_DahengNetAreaScanCamera::isOpen()
{
    return m_cameraHandle->isDeviceOpenned();
}

bool TM_DahengNetAreaScanCamera::isGrab()
{
    return m_cameraHandle->isGrabbing();
}

bool TM_DahengNetAreaScanCamera::isAreaScan(bool &isAreaScanCamera)
{
    return m_cameraHandle->isAreaScan(isAreaScanCamera);
}

bool TM_DahengNetAreaScanCamera::isColor(bool &isColorCamera)
{
    return m_cameraHandle->isColor(isColorCamera);
}

bool TM_DahengNetAreaScanCamera::setRoi(int roiX, int roiY, int roiW, int roiH)
{
    bool ret = true;
    /* ??????ROI ??????ROI???????????????????????????????????? */
    if (m_cameraHandle->isGrabbing()) {
        bool stopCameraGrabbing = m_cameraHandle->stopGrabbing();
        if (stopCameraGrabbing) {
            qCritical("?????????ROI??????????????????????????????");
            ret &= false;
        }
    }
    ret &= m_cameraHandle->setRoi(static_cast<unsigned int>(roiX),
                                  static_cast<unsigned int>(roiY),
                                  static_cast<unsigned int>(roiW),
                                  static_cast<unsigned int>(roiH));
//    ret &= m_cameraHandle->startGrabbing();
    return ret;
}

bool TM_DahengNetAreaScanCamera::setGain(float gain)
{
    return m_cameraHandle->setGain(gain);
}

bool TM_DahengNetAreaScanCamera::setGamma(float gama)
{
    return m_cameraHandle->setGamma(gama);
}

bool TM_DahengNetAreaScanCamera::setExposureTime(float exposure)
{
    return m_cameraHandle->setExposureTime(exposure);
}

bool TM_DahengNetAreaScanCamera::setTriggerSource(int triggerSource)
{
    bool result = true;
    switch (triggerSource)
    {
    case SOURCE_SOFRWARE:
        result = m_cameraHandle->setTriggerSource(DahengNetAreaCamera::TriggerSourceSoftWare);
        break;
    case SOURCE_LINE0:
        result = m_cameraHandle->setTriggerSource(DahengNetAreaCamera::TriggerSourceLine0);
        break;
    case SOURCE_LINE1:
        result = m_cameraHandle->setTriggerSource(DahengNetAreaCamera::TriggerSourceLine1);
        break;
    case SOURCE_LINE2:
        result = m_cameraHandle->setTriggerSource(DahengNetAreaCamera::TriggerSourceLine2);
        break;
    case SOURCE_LINE3:
        result = m_cameraHandle->setTriggerSource(DahengNetAreaCamera::TriggerSourceLine3);
        break;
    default:
        qDebug() << QString("????????????????????????????????????????????????????????????%1").arg(triggerSource);
        break;
    }
    return result;
}

bool TM_DahengNetAreaScanCamera::setTriggerMode(bool triggerMode)
{
    return m_cameraHandle->setTriggerMode(triggerMode);
}

bool TM_DahengNetAreaScanCamera::setGammaEnable(bool gammaEnable)
{
    return m_cameraHandle->setGammaEnable(gammaEnable);
}

bool TM_DahengNetAreaScanCamera::getRoi(int &roiX, int &roiY, int &roiW, int &roiH)
{
    unsigned int x, y, width ,hight;
    bool result = m_cameraHandle->getRoi(x, y, width ,hight);
    roiX = x;
    roiY = y;
    roiW = width;
    roiH = hight;
    return result;
}

bool TM_DahengNetAreaScanCamera::getGain(float &gain)
{
    return m_cameraHandle->getGain(gain);
}

bool TM_DahengNetAreaScanCamera::getGamma(float &gamma)
{
    return m_cameraHandle->getGamma(gamma);
}

bool TM_DahengNetAreaScanCamera::getExposureTime(float &exposure)
{
    return m_cameraHandle->getExposureTime(exposure);
}

bool TM_DahengNetAreaScanCamera::getTriggerSource(int &triggerSource)
{
    DahengAreaCamera::DahengTriggerSource tgiggerSourceNum;
    bool result = m_cameraHandle->getTriggerSource(tgiggerSourceNum);
    switch (tgiggerSourceNum)
    {
    case DahengAreaCamera::TriggerSourceSoftWare:
        triggerSource = SOURCE_SOFRWARE;
        break;
    case DahengAreaCamera::TriggerSourceLine0:
        triggerSource = SOURCE_LINE0;
        break;
    case DahengAreaCamera::TriggerSourceLine1:
        triggerSource = SOURCE_LINE1;
        break;
    case DahengAreaCamera::TriggerSourceLine2:
        triggerSource = SOURCE_LINE2;
        break;
    case DahengAreaCamera::TriggerSourceLine3:
        triggerSource = SOURCE_LINE3;
        break;
    default:
        break;
    }

    return result;
}

bool TM_DahengNetAreaScanCamera::getTriggerMode(bool &triggerMode)
{
    return m_cameraHandle->getTriggerMode(triggerMode);
}

bool TM_DahengNetAreaScanCamera::getGammaEnable(bool &gammaEnable)
{
    return m_cameraHandle->getGammaEnable(gammaEnable);
}

bool TM_DahengNetAreaScanCamera::sendSoftTriggerCommand()
{
    return m_cameraHandle->sendSoftTriggerCommand();
}

bool TM_DahengNetAreaScanCamera::enumDevices(QMap<QString, GX_DEVICE_IP_INFO> &cameraInfoList)
{
    return DahengNetAreaCamera::enumDevices(cameraInfoList);
}

bool TM_DahengNetAreaScanCamera::isAreaLScan()
{
    bool isAreaLScan = false;
    m_cameraHandle->isAreaScan(isAreaLScan);
    return isAreaLScan;
}

int TM_DahengNetAreaScanCamera::getCamerDeviceNum()
{
    unsigned int cameraNum = 0;
    DahengNetAreaCamera::getCameraDeviceNum(cameraNum);
    return cameraNum;
}

bool TM_DahengNetAreaScanCamera::setHardwareTrigger()
{
    return m_cameraHandle->setHardwareTrigger();
}

bool TM_DahengNetAreaScanCamera::setSoftwareTrigger()
{
    return m_cameraHandle->setSoftwareTrigger();
}

bool TM_DahengNetAreaScanCamera::setImageBuffer(int bufferNumber)
{
    return m_cameraHandle->setImageBuffer(bufferNumber);
}

bool TM_DahengNetAreaScanCamera::setAcquisitionFrameRateEnable(bool enable)
{
    return m_cameraHandle->setAcquisitionFrameRateEnable(enable);
}

bool TM_DahengNetAreaScanCamera::setAcquisitionFrameRate(float frameRate)
{
    return m_cameraHandle->setAcquisitionFrameRate(frameRate);
}

bool TM_DahengNetAreaScanCamera::getAcquisitionFrameRateEnable(bool &enable)
{
    return m_cameraHandle->getAcquisitionFrameRateEnable(enable);
}

bool TM_DahengNetAreaScanCamera::getAcquisitionFrameRate(float &frameRate)
{
    return m_cameraHandle->getAcquisitionFrameRate(frameRate);
}

void TM_DahengNetAreaScanCamera::photoReceivedCallBack(GX_FRAME_CALLBACK_PARAM *pFrame)
{
    qDebug() << QString("???????????????????????????????????????ID???%1???").arg(QString::number(pFrame->nFrameID));
    if (pFrame->status == 0)    //????????????????????????0???????????????
    {
        //???????????????????????????
        qDebug() << "??????????????????" << __FUNCTION__ << "??????????????????????????????ID???" << QThread::currentThread();
        qDebug() << "???????????????????????????"  << QString::asprintf("%p", pFrame->pImgBuf);
        // Convert RAW8 or RAW16 image to RGB24 image
        if (pFrame->nPixelFormat==GX_PIXEL_FORMAT_MONO8)
        {
            // Step1: ??????????????????
            uint imageSize = pFrame->nWidth * pFrame->nHeight;
            QImage photo(pFrame->nWidth, pFrame->nHeight, QImage::Format_Grayscale8);
            QTime copyTime;
            copyTime.start();
            memcpy(photo.bits(), pFrame->pImgBuf,  imageSize);
            qDebug() << "???????????????????????????" << copyTime.elapsed();

            QTime setGammaTime;
            setGammaTime.start();
            unsigned char* pImageProcess = photo.bits();
            TM_DahengNetAreaScanCamera *self = static_cast<TM_DahengNetAreaScanCamera *>(pFrame->pUserParam);
            self->m_cameraHandle->imageProcess(*pFrame, pImageProcess);
            QImage photo2(pFrame->nWidth, pFrame->nHeight, QImage::Format_Grayscale8);
            memcpy(photo2.bits(), pImageProcess,  imageSize);
            int copyCost = setGammaTime.elapsed() + copyTime.elapsed();
            self->photoReceived(photo2, copyCost);
            qDebug() << "?????????????????????????????????????????????" << QSize(pFrame->nWidth, pFrame->nHeight);
            qDebug() << "??????????????????????????????Gamma???????????????" << setGammaTime.elapsed();
        }
        else //????????????
        {
            uint imageSize = pFrame->nWidth * pFrame->nHeight;
            QImage photo(pFrame->nWidth, pFrame->nHeight, QImage::Format_RGB888);
            QTime copyTime;
            copyTime.start();
            memcpy(photo.bits(),pFrame->pImgBuf,imageSize);
            qDebug() << "???????????????????????????" << copyTime.elapsed();

            QTime setGammaTime;
            setGammaTime.start();
            unsigned char* pImageProcess = photo.bits();
            TM_DahengNetAreaScanCamera *self = static_cast<TM_DahengNetAreaScanCamera *>(pFrame->pUserParam);
            self->m_cameraHandle->imageProcess(*pFrame, pImageProcess);
            QImage photo2(pFrame->nWidth, pFrame->nHeight, QImage::Format_RGB888);
            memcpy(photo2.bits(), pImageProcess, imageSize * 3);
            int copyCost = setGammaTime.elapsed() + copyTime.elapsed();
            self->photoReceived(photo2, copyCost);
            qDebug() << "?????????????????????????????????????????????" << QSize(pFrame->nWidth, pFrame->nHeight);
            qDebug() << "??????????????????????????????Gamma???????????????" << setGammaTime.elapsed();
        }
    }
    else
    {
        qWarning() << "????????????????????????????????????????????????????????????" << QSize(pFrame->nWidth, pFrame->nHeight);
    }
    return;
}

void TM_DahengNetAreaScanCamera::cameraOfflineCallBack(void *pUser)
{
    //??????????????????????????????????????????????????????????????????????????????????????????????????????
    TM_DahengNetAreaScanCamera *self = static_cast<TM_DahengNetAreaScanCamera *>(pUser);
    bool deviceOffline = true;
    self->m_cameraHandle->setDeviceOffline(deviceOffline);
    qCritical() << QString("???????????????????????????????????????????????????????????????%1???").arg("true");

    return;
}

#endif // GLOBAL_DAHENG
