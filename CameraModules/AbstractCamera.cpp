#include <QDebug>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include "AbstractCamera.h"

AbstractCamera::AbstractCamera()
    : QObject()
{

}

bool AbstractCamera::isOpen()
{
    return false;
}

bool AbstractCamera::isGrab()
{
    return false;
}

bool AbstractCamera::isAreaScan(bool &isAreaScanCamera)
{
    Q_UNUSED(isAreaScanCamera);
    return false;
}

bool AbstractCamera::isColor(bool &isColorCamera)
{
    Q_UNUSED(isColorCamera);
    return false;
}

bool AbstractCamera::setParameters()
{
    return false;
}

bool AbstractCamera::setRoi(int roiX, int roiY, int roiW, int roiH)
{
    Q_UNUSED(roiX);
    Q_UNUSED(roiY);
    Q_UNUSED(roiW);
    Q_UNUSED(roiH);
    return false;
}

bool AbstractCamera::setLineRate(float cameraLineRate)
{
    Q_UNUSED(cameraLineRate);
    return false;
}

bool AbstractCamera::setPreampGain(float cameraPreampGain)
{
    Q_UNUSED(cameraPreampGain);
    return false;
}

bool AbstractCamera::setGain(float cameraGain)
{
    Q_UNUSED(cameraGain);
    return false;
}

bool AbstractCamera::setGamma(float cameraGama)
{
    Q_UNUSED(cameraGama);
    return false;
}

bool AbstractCamera::setExposureTime(float cameraExposure)
{
    Q_UNUSED(cameraExposure);
    return false;
}

bool AbstractCamera::setTriggerSource(int cameraTriggerSource)
{
    Q_UNUSED(cameraTriggerSource);
    return false;
}

bool AbstractCamera::setTriggerMode(bool cameraTriggerMode)
{
    Q_UNUSED(cameraTriggerMode);
    return false;
}

bool AbstractCamera::setGammaEnable(bool cameraGammaEnable)
{
    Q_UNUSED(cameraGammaEnable);
    return false;
}

bool AbstractCamera::readParameters()
{
    return false;
}

bool AbstractCamera::getRoi(int &roiX, int &roiY, int &roiW, int &roiH)
{
    Q_UNUSED(roiX);
    Q_UNUSED(roiY);
    Q_UNUSED(roiW);
    Q_UNUSED(roiH);
    return false;
}

bool AbstractCamera::getLineRate(float &cameraLineRate)
{
    Q_UNUSED(cameraLineRate);
    return false;
}

bool AbstractCamera::getPreampGain(float &cameraPreampGain)
{
    Q_UNUSED(cameraPreampGain);
    return false;
}

bool AbstractCamera::getGain(float &cameraGain)
{
    Q_UNUSED(cameraGain);
    return false;
}

bool AbstractCamera::getGamma(float &cameraGamma)
{
    Q_UNUSED(cameraGamma);
    return false;
}

bool AbstractCamera::getExposureTime(float &cameraExposure)
{
    Q_UNUSED(cameraExposure);
    return false;
}

bool AbstractCamera::getTriggerSource(int &cameraTriggerSource)
{
    Q_UNUSED(cameraTriggerSource);
    return false;
}

bool AbstractCamera::getTriggerMode(bool &cameraTriggerMode)
{
    Q_UNUSED(cameraTriggerMode);
    return false;
}

bool AbstractCamera::getGammaEnable(bool &cameraGammaEnable)
{
    Q_UNUSED(cameraGammaEnable);
    return false;
}

bool AbstractCamera::sendSoftTriggerCommand()
{
    return false;
}

int AbstractCamera::getCamerDeviceNum()
{
    return 0;
}

bool AbstractCamera::setHardwareTrigger()
{
    return false;
}

bool AbstractCamera::setSoftwareTrigger()
{
    return false;
}

bool AbstractCamera::setImageBuffer(int bufferNumber)
{
    Q_UNUSED(bufferNumber);
    return false;
}

bool AbstractCamera::setHeartbeatTime(int heartbeatTime)
{
    Q_UNUSED(heartbeatTime);
    return false;
}

bool AbstractCamera::setPacketSize(int packetSize)
{
    Q_UNUSED(packetSize);
    return false;
}

bool AbstractCamera::setReverse_X(bool enable)
{
    Q_UNUSED(enable);
    return false;
}

bool AbstractCamera::setReverse_Y(bool enable)
{
    Q_UNUSED(enable);
    return false;
}

bool AbstractCamera::setAcquisitionFrameRateEnable(bool enable)
{
    Q_UNUSED(enable);
    return false;
}

bool AbstractCamera::setAcquisitionFrameRate(float frameRate)
{
    Q_UNUSED(frameRate);
    return false;
}

bool AbstractCamera::getAcquisitionFrameRateEnable(bool &enable)
{
    Q_UNUSED(enable);
    return false;
}

bool AbstractCamera::getAcquisitionFrameRate(float &frameRate)
{
    Q_UNUSED(frameRate);
    return false;
}

QString AbstractCamera::toString() const
{
    return QString("");
}
