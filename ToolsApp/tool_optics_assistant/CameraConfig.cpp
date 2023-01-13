#include "QMessageBox"
#include "CameraConfig.h"
#include "ui_CameraConfig.h"

CameraConfig::CameraConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraConfig)
{
    ui->setupUi(this);
    ui->pushButtonOpen->setVisible(false);
    ui->pushButtonClose->setVisible(false);
    connect(&m_cameraManager, &CameraManager::openCameraFinish, this, &CameraConfig::receiveOpenCameraFinish, Qt::DirectConnection);
    connect(&m_cameraManager, &CameraManager::photoReceived, this, &CameraConfig::photoReceived, Qt::DirectConnection);
}

CameraConfig::~CameraConfig()
{
    if (m_currentCamera != NULL) {
        m_currentCamera = NULL;
    }
    delete ui;
}

CameraParameter CameraConfig::getCameraParameter()
{
    CameraParameter config;
    config.Roi_X = ui->roiX->value();
    config.Roi_Y = ui->roiY->value();
    config.Roi_W = ui->roiWidth->value();
    config.Roi_H = ui->roiHeight->value();
    config.ExposureTime = ui->exposureTime->value();
    config.Gamma = ui->gamma->value();
    config.Gain = ui->gain->value();
    config.LineRate = ui->lineRate->value();
    config.PreampGain = ui->preampGain->value();
    config.TriggerMode = ui->TriggerMode->currentIndex() == 1 ? true : false;
    config.TriggerSource = ui->TriggerMode->currentIndex();
    emit updataCameraParameter(config);
    return config;
}

void CameraConfig::setCameraParameter(const CameraParameter &config, bool ROIEnable)
{
    updataUICameraParameter(config, ROIEnable);
    if (ROIEnable) {
        if (m_currentCamera && m_currentCamera->isOpen()) {
            m_currentCamera->setRoi(config.Roi_X, config.Roi_Y, config.Roi_W, config.Roi_H);
        }
    }
    if (m_currentCamera && m_currentCamera->isOpen()) {
        m_currentCamera->setExposureTime(config.ExposureTime);
        m_currentCamera->setGamma(config.Gamma);
        m_currentCamera->setGain(config.Gain);
        m_currentCamera->setLineRate(config.LineRate);
        m_currentCamera->setPreampGain(config.PreampGain);
    }
    if (m_currentCamera && m_currentCamera->isOpen() && m_currentCamera->isGrab()) {
        setROIButtonEnable(false);
    } else {
        setROIButtonEnable(true);
    }
}

void CameraConfig::searchCameraList(const Camera_Type &type)
{
    switch (type) {
    case Camera_DahengNetAreaScan:
        searchDaHengNetAreaCameraCameraList();
        break;
    case Camera_DahengUsbAreaScan:
        searchDaHengUSBAreaCameraCameraList();
        break;
    case Camera_HaikangNetAreaScan:
        searchHaiKangNetAreaCameraCameraList();
        break;
    case Camera_HaikangNetLineScan:
        searchHaiKangNetLineCameraCameraList();
        break;
    default:
        qDebug() << u8"暂时不支持类型" << type;
        break;
    }
}

void CameraConfig::connectCamera(const QString &cameraSn, const Camera_Type &type)
{
    switch (type) {
    case Camera_DahengNetAreaScan:
        m_cameraManager.initDaHengNetAreaScan(cameraSn);
        break;
    case Camera_DahengUsbAreaScan:
        m_cameraManager.initDahengUsbAreaScan(cameraSn);
        break;
    case Camera_HaikangNetAreaScan:
        m_cameraManager.initHaikangNetAreaScan(cameraSn);
        break;
    case Camera_HaikangNetLineScan:
        m_cameraManager.initHaikangNetLineScan(cameraSn);
        break;
    default:
        break;
    }
}

void CameraConfig::closeCamera(const QString &cameraSn, const Camera_Type &type)
{
    bool res = m_cameraManager.closeCamera(cameraSn, type);
    if (res) {
        emit updataInfo(u8"相机关闭成功。", false);
    } else {
        emit updataInfo(u8"相机关闭失败。", true);
    }
    emit closeCameraFinish(cameraSn, type, res);
}

void CameraConfig::setCurrentCamera(const QString &cameraSn, const Camera_Type &type)
{
    m_currentCamera = m_cameraManager.getCamera(cameraSn, type);
    ui->CameraSn->setText(cameraSn);
    if ((m_currentCamera != NULL) && (m_currentCamera->isGrab())) {
        setButtonStyle(ui->startAcquisition, true);
    } else {
        setButtonStyle(ui->startAcquisition, false);
    }
    readCmaeraParameter();
}

void CameraConfig::receiveOpenCameraFinish(const QString &cameraSn, const Camera_Type &type, bool isSuccess)
{
    if (isSuccess) {
        emit updataInfo(u8"相机打开成功。", false);
    } else {
        emit updataInfo(u8"相机打开失败。", true);
    }
    emit openCameraFinish(cameraSn, type, isSuccess);
}

void CameraConfig::on_startAcquisition_clicked()
{
    bool res = false;
    if(m_currentCamera && m_currentCamera->isOpen()) {
        res = m_currentCamera->start();
    }
    if(res) {
        qDebug() << QString("camera start surceuss");
        emit updataInfo(u8"相机开始采集成功。", false);
        setROIButtonEnable(false);
        setButtonStyle(ui->startAcquisition, true);
    } else {
        qDebug() << QString("camera start error");
        emit updataInfo(u8"相机开始采集失败。", true);
        setButtonStyle(ui->startAcquisition, false);
    }
}

void CameraConfig::on_stopAcquisition_clicked()
{
    bool res = false;
    if(m_currentCamera && m_currentCamera->isOpen()) {
        res = m_currentCamera->stop();
    }

    if(res)  {
        qDebug() << QString("camera stop surceuss");
        emit updataInfo(u8"相机停止采集成功。", false);
        setROIButtonEnable(true);
    } else {
        qDebug() << QString("camera stop error");
        emit updataInfo(u8"相机停止采集失败。", true);
    }
    setButtonStyle(ui->startAcquisition, false);
}

void CameraConfig::on_triggerSoftWare_clicked()
{
    bool res = false;
    if(m_currentCamera && m_currentCamera->isOpen()) {
        res = m_currentCamera->sendSoftTriggerCommand();
    }

    if(res) {
        qDebug() << QString("camera soft trigger surceuss");
        emit updataInfo(u8"相机软触发成功。", false);
    } else {
        qDebug() << QString("camera soft trigger error");
        emit updataInfo(u8"相机软触发失败。", true);
    }
}

void CameraConfig::on_TriggerMode_currentIndexChanged(const QString &arg1)
{
    bool res = false;
    if (arg1 == "on") {
        if (m_currentCamera && m_currentCamera->isOpen()) {
            res = m_currentCamera->setTriggerMode(true);
        }
    } else {
        if (m_currentCamera && m_currentCamera->isOpen()) {
            res = m_currentCamera->setTriggerMode(false);
        }
    }

    if(res) {
        qDebug() << QString("camera set trigger mode succeeded");
        emit updataInfo(u8"相机设置触发模式成功。", false);
    } else {
        qDebug() << QString("camera set trigger mode failed");
        emit updataInfo(u8"相机设置触发模式失败。", true);
    }
}

void CameraConfig::on_TriggerSource_currentIndexChanged(int index)
{
    bool res = false;
    if (m_currentCamera && m_currentCamera->isOpen()) {
        res = m_currentCamera->setTriggerSource(index);
    } if(res) {
        qDebug() << QString("Camera set trigger source succeeded");
        emit updataInfo(u8"相机设置触发源成功。", false);
    } else {
        qDebug() << QString("camera set trigger source failed");
        emit updataInfo(u8"相机设置触发源失败。", true);
    }
}

void CameraConfig::on_roiX_editingFinished()
{
    bool res = false;
    int roiX = ui->roiX->value();
    int roiY = ui->roiY->value();
    int roiW = ui->roiWidth->value();
    int roiH = ui->roiHeight->value();
    if (m_currentCamera && m_currentCamera->isOpen()) {
        res = m_currentCamera->setRoi(roiX, roiY, roiW, roiH);
    }
    if(res) {
        qDebug() << QString("Camera set ROI succeeded");
        emit updataInfo(u8"相机设置ROI成功。", false);
    } else {
        qDebug() << QString("camera set ROI failed");
        emit updataInfo(u8"相机设置ROI失败。", true);
    }
    if (m_currentCamera && m_currentCamera->isOpen()) {
        if (m_currentCamera->getRoi(roiX, roiY, roiW, roiH)) {
            ui->roiX->setValue(roiX);
            ui->roiY->setValue(roiY);
            ui->roiWidth->setValue(roiW);
            ui->roiHeight->setValue(roiH);
        }
    }
}

void CameraConfig::on_roiY_editingFinished()
{
    on_roiX_editingFinished();
}

void CameraConfig::on_roiWidth_editingFinished()
{
    on_roiX_editingFinished();
}

void CameraConfig::on_roiHeight_editingFinished()
{
    on_roiX_editingFinished();
}

void CameraConfig::on_exposureTime_editingFinished()
{
    bool res = false;
    if (m_currentCamera && m_currentCamera->isOpen()) {
        res = m_currentCamera->setExposureTime(ui->exposureTime->value());
    }
    if(res) {
        qDebug() << QString("Camera set exposureTime succeeded");
        emit updataInfo(u8"相机设置曝光时间成功。", false);
    } else {
        qDebug() << QString("camera set exposureTime failed");
        emit updataInfo(u8"相机设置曝光时间失败。", true);
    }
    if (m_currentCamera && m_currentCamera->isOpen()) {
        float value = -1;
        if (m_currentCamera->getExposureTime(value)) {
            ui->exposureTime->setValue(value);
        }
    }
}

void CameraConfig::on_gamma_editingFinished()
{
    bool res = false;
    if (m_currentCamera && m_currentCamera->isOpen()) {
        res = m_currentCamera->setGamma(ui->gamma->value());
    }
    if(res) {
        qDebug() << QString("Camera set gamma succeeded");
        emit updataInfo(u8"相机设置伽玛成功。", false);
    } else {
        qDebug() << QString("camera set gamma failed");
        emit updataInfo(u8"相机设置伽玛失败。", true);
    }
    if (m_currentCamera && m_currentCamera->isOpen()) {
        float value = -1;
        if (m_currentCamera->getGamma(value)) {
            ui->gamma->setValue(value);
        }
    }
}

void CameraConfig::on_gain_editingFinished()
{
    bool res = false;
    if (m_currentCamera && m_currentCamera->isOpen()) {
        res = m_currentCamera->setGain(ui->gain->value());
    }
    if(res) {
        qDebug() << QString("Camera set gain succeeded");
        emit updataInfo(u8"相机设置增益成功。", false);
    } else {
        qDebug() << QString("camera set gain failed");
        emit updataInfo(u8"相机设置增益失败。", true);
    }
    if (m_currentCamera && m_currentCamera->isOpen()) {
        float value = -1;
        if (m_currentCamera->getGain(value)) {
            ui->gain->setValue(value);
        }
    }
}

void CameraConfig::on_lineRate_editingFinished()
{
    bool res = false;
    if (m_currentCamera && m_currentCamera->isOpen()) {
        res = m_currentCamera->setLineRate(ui->lineRate->value());
    }
    if(res) {
        qDebug() << QString("Camera set lineRate succeeded");
        emit updataInfo(u8"相机设置行频成功。", false);
    } else {
        qDebug() << QString("camera set lineRate failed");
        emit updataInfo(u8"相机设置行频失败。", true);
    }
    if (m_currentCamera && m_currentCamera->isOpen()) {
        float value = -1;
        if (m_currentCamera->getLineRate(value)) {
            ui->lineRate->setValue(value);
        }
    }
}

void CameraConfig::on_preampGain_editingFinished()
{
    bool res = false;
    if (m_currentCamera && m_currentCamera->isOpen()) {
        res = m_currentCamera->setPreampGain(ui->preampGain->value());
    }
    if(res) {
        qDebug() << QString("Camera set preampGain succeeded");
        emit updataInfo(u8"相机设置前置增益成功。", false);
    } else {
        qDebug() << QString("camera set preampGain failed");
        emit updataInfo(u8"相机设置前置增益失败。", true);
    }
    if (m_currentCamera && m_currentCamera->isOpen()) {
        float value = -1;
        if (m_currentCamera->getPreampGain(value)) {
            ui->preampGain->setValue(value);
        }
    }
}

void CameraConfig::searchDaHengNetAreaCameraCameraList()
{
    QList<QString> cameraList;
    m_cameraManager.searchDahengNetAreaScan(cameraList);
    if (!cameraList.isEmpty()) {
        emit updataCameraList(Camera_DahengNetAreaScan, cameraList);
    }
}

void CameraConfig::searchDaHengUSBAreaCameraCameraList()
{
    QList<QString> cameraList;
    m_cameraManager.searchDahengUsbAreaScan(cameraList);
    if (!cameraList.isEmpty()) {
        emit updataCameraList(Camera_DahengUsbAreaScan, cameraList);
    }
}

void CameraConfig::searchHaiKangNetAreaCameraCameraList()
{
    QList<QString> cameraList;
    m_cameraManager.searchHaikangNetAreaScan(cameraList);
    if (!cameraList.isEmpty()) {
        emit updataCameraList(Camera_HaikangNetAreaScan, cameraList);
    }
}

void CameraConfig::searchHaiKangNetLineCameraCameraList()
{
    QList<QString> cameraList;
    m_cameraManager.searchHaikangNetLineScan(cameraList);
    if (!cameraList.isEmpty()) {
        emit updataCameraList(Camera_HaikangNetLineScan, cameraList);
    }
}

void CameraConfig::readCmaeraParameter()
{
    CameraParameter config;
    if (m_currentCamera && m_currentCamera->isOpen()) {
        m_currentCamera->getRoi(config.Roi_X, config.Roi_Y, config.Roi_W, config.Roi_H);
        m_currentCamera->getExposureTime(config.ExposureTime);
        m_currentCamera->getGamma(config.Gamma);
        m_currentCamera->getGain(config.Gain);
        m_currentCamera->getLineRate(config.LineRate);
        m_currentCamera->getPreampGain(config.PreampGain);
        m_currentCamera->getTriggerMode(config.TriggerMode);
        m_currentCamera->getTriggerSource(config.TriggerSource);
    }

    setBlockSignals(true);
    updataUICameraParameter(config, true);
    setBlockSignals(false);
}

void CameraConfig::printMessageBoxInfo(const QString &info)
{
    QMessageBox box(QMessageBox::Warning, QStringLiteral("警告!"), info);
    box.setStandardButtons(QMessageBox::Cancel);
    box.setButtonText(QMessageBox::Cancel, QStringLiteral("关闭"));
    box.exec();
}

void CameraConfig::setButtonEnable(bool isEnabled)
{
    //操作按钮
    ui->pushButtonOpen->setEnabled(!isEnabled);
    ui->pushButtonClose->setEnabled(isEnabled);
    ui->startAcquisition->setEnabled(isEnabled);
    ui->stopAcquisition->setEnabled(isEnabled);
    ui->triggerSoftWare->setEnabled(isEnabled);

    //参数按钮
    ui->exposureTime->setEnabled(isEnabled);
    ui->gamma->setEnabled(isEnabled);
    ui->gain->setEnabled(isEnabled);
    ui->lineRate->setEnabled(isEnabled);
    ui->preampGain->setEnabled(isEnabled);
    ui->TriggerMode->setEnabled(isEnabled);
    ui->TriggerSource->setEnabled(isEnabled);
}

void CameraConfig::setROIButtonEnable(bool isEnabled)
{
    ui->roiX->setEnabled(isEnabled);
    ui->roiY->setEnabled(isEnabled);
    ui->roiWidth->setEnabled(isEnabled);
    ui->roiHeight->setEnabled(isEnabled);
}

void CameraConfig::setCameraDefaultValue()
{
    ui->roiX->setValue(-1);
    ui->roiY->setValue(-1);
    ui->roiWidth->setValue(-1);
    ui->roiHeight->setValue(-1);
    ui->exposureTime->setValue(-1);
    ui->gamma->setValue(-1);
    ui->gain->setValue(-1);
    ui->lineRate->setValue(-1);
    ui->preampGain->setValue(-1);
    ui->TriggerMode->setCurrentIndex(0);
    ui->TriggerSource->setCurrentIndex(0);
}

void CameraConfig::setBlockSignals(bool value)
{
    ui->roiX->blockSignals(value);
    ui->roiY->blockSignals(value);
    ui->roiWidth->blockSignals(value);
    ui->roiHeight->blockSignals(value);
    ui->exposureTime->blockSignals(value);
    ui->gamma->blockSignals(value);
    ui->gain->blockSignals(value);
    ui->lineRate->blockSignals(value);
    ui->preampGain->blockSignals(value);
    ui->TriggerMode->blockSignals(value);
    ui->TriggerSource->blockSignals(value);
}

void CameraConfig::setButtonStyle(QPushButton *button, bool isPressed)
{
    if (button != NULL) {
        if (!isPressed) {
            button->setStyleSheet("QPushButton{border-radius:4px; padding:2px 4px; background-color: rgb(190, 190, 190);} QPushButton:hover{background-color:rgb(180,180,180)}");
        } else {
            button->setStyleSheet("QPushButton{border-radius:4px; padding:2px 4px; background-color: rgb(190, 100, 100);} QPushButton:hover{background-color:rgb(200, 100, 100)}");
        }
    }
}

void CameraConfig::updataUICameraParameter(const CameraParameter &config, bool ROIEnable)
{
    if (ROIEnable) {
        ui->roiX->setValue(config.Roi_X);
        ui->roiY->setValue(config.Roi_Y);
        ui->roiWidth->setValue(config.Roi_W);
        ui->roiHeight->setValue(config.Roi_H);
    }

    ui->exposureTime->setValue(config.ExposureTime);
    ui->gamma->setValue(config.Gamma);
    ui->gain->setValue(config.Gain);
    ui->lineRate->setValue(config.LineRate);
    ui->preampGain->setValue(config.PreampGain);
    ui->TriggerMode->setCurrentIndex(config.TriggerMode ? 1 : 0);
    ui->TriggerSource->setCurrentIndex(config.TriggerSource);
}
