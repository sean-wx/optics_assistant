#ifndef PRODUCTCONFIG_H
#define PRODUCTCONFIG_H

#include <QWidget>
#include "comm_def.h"

namespace Ui {
class ProductConfig;
}

class ProductConfig : public QWidget
{
    Q_OBJECT

public:
    explicit ProductConfig(QWidget *parent = 0);
    ~ProductConfig();

    ConnectData getConnectData();

signals:
    void updateModelImageFile(const QString &cameraSn, const Camera_Type &type, const ConnectData &connectData);
    void requestCameraParameter();
    void updataCameraParameter(const CameraParameter &parameter, bool ROIEnable);
    void startSaveCameraImg(const QString &cameraSn, const Camera_Type &cameraType);
    void startSaveStatisticalImg(const QString &cameraSn, const Camera_Type &cameraType);
    void startSaveComparisonImg(const QString &cameraSn, const Camera_Type &cameraType);
    void updataInfo(const QString &info, bool isError);

public slots:
    void setCurrentCamera(const QString &cameraSn, const Camera_Type &type);
    void saveCameraParameterToDb(const CameraParameter &parameter);
    void getMouldImageFile();
    void setConnectData(const QString &cameraSn, const Camera_Type &type, const ConnectData &connectData);

private slots:
    void on_loadingData_clicked();
    void on_saveData_clicked();
    void on_getMouldImageFile_clicked();
    void on_saveStatisticalImg_clicked();
    void on_saveComparisonImg_clicked();
    void on_saveImageBtn_clicked();

private:
    Ui::ProductConfig *ui;
    QString            m_cameraSn    = "";
    Camera_Type        m_cameratype  = CAMERA_DEFAULT;
    QString            m_mouldImageFile = "";

    QMap<Camera_Type, QMap<QString, ConnectData>> m_connectDataList;
};

#endif // PRODUCTCONFIG_H
