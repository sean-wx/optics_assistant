#ifndef CAMERACONFIG_H
#define CAMERACONFIG_H

#include <QWidget>
#include <QPushButton>
#include "CameraManager.h"

namespace Ui {
class CameraConfig;
}

class CameraConfig : public QWidget
{
    Q_OBJECT

public:
    explicit CameraConfig(QWidget *parent = 0);
    ~CameraConfig();

signals:
    void updataCameraList(const Camera_Type &type, const QList<QString> &list);
    void openCameraFinish(const QString &cameraSn, const Camera_Type &type, bool isSuccess);
    void closeCameraFinish(const QString &cameraSn, const Camera_Type &type, bool isSuccess);
    void photoReceived(const QString &cameraSn, const Camera_Type &type, const QImage &photo, int copyCost);
    void updataCameraParameter(const CameraParameter &parameter);
    void updataInfo(const QString &info, bool isError);

public slots:
    void searchCameraList(const Camera_Type &type);
    void connectCamera(const QString &cameraSn, const Camera_Type &type);
    void closeCamera(const QString &cameraSn, const Camera_Type &type);
    void setCurrentCamera(const QString &cameraSn, const Camera_Type &type);
    void receiveOpenCameraFinish(const QString &cameraSn, const Camera_Type &type, bool isSuccess);
    void setCameraParameter(const CameraParameter &config, bool ROIEnable);
    CameraParameter getCameraParameter();

private slots:
    void on_startAcquisition_clicked();
    void on_stopAcquisition_clicked();
    void on_triggerSoftWare_clicked();
    void on_TriggerMode_currentIndexChanged(const QString &arg1);
    void on_TriggerSource_currentIndexChanged(int index);
    void on_roiX_editingFinished();
    void on_roiY_editingFinished();
    void on_roiWidth_editingFinished();
    void on_roiHeight_editingFinished();
    void on_exposureTime_editingFinished();
    void on_gamma_editingFinished();
    void on_gain_editingFinished();
    void on_lineRate_editingFinished();
    void on_preampGain_editingFinished();

private:
    void searchDaHengNetAreaCameraCameraList();
    void searchDaHengUSBAreaCameraCameraList();
    void searchHaiKangNetAreaCameraCameraList();
    void searchHaiKangNetLineCameraCameraList();

    void readCmaeraParameter();
    void printMessageBoxInfo(const QString &info);
    void setButtonEnable(bool isEnabled);
    void setROIButtonEnable(bool isEnabled);
    void setCameraDefaultValue();
    void setBlockSignals(bool value);
    void setButtonStyle(QPushButton *button, bool isPressed);
    void updataUICameraParameter(const CameraParameter &config, bool ROIEnable);

    Ui::CameraConfig *ui;
    CameraManager m_cameraManager;
    AbstractCamera *m_currentCamera = NULL;
};

#endif // CAMERACONFIG_H
