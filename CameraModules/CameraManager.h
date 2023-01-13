#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QObject>
#include <QMap>
#include "AbstractCamera.h"
#include "TM_DahengNetAreaScanCamera.h"
#include "TM_DahengUsbAreaScanCamera.h"
#include "TM_HaikangNetAreaScanCamera.h"
#include "TM_HaikangNetLineScanCamera.h"
#include "comm_def.h"

/* Hk API */
#ifdef GLOBAL_HAIKANG
#include "CameraParams.h"
#endif

/* DaHeng API */
#ifdef GLOBAL_DAHENG
#include "GxIAPI.h"
#include "DxImageProc.h"
#endif

/* DaHua API */
#ifdef GLOBAL_DAHUA
#include "GenICam/System.h"
#include "Media/ImageConvert.h"
#endif

/* Dalsa Net LineScan Camera API */
#ifdef GLOBAL_DALSA_NET
#include "cordef.h"
#include "gevapi.h"
#endif

class CameraManager : public QObject
{
    Q_OBJECT
public:
    explicit CameraManager(QObject *parent = nullptr);
    ~CameraManager();

    bool searchDahengNetAreaScan(QList<QString> &cameraList);
    bool searchDahengUsbAreaScan(QList<QString> &cameraList);
    bool searchHaikangNetAreaScan(QList<QString> &cameraList);
    bool searchHaikangNetLineScan(QList<QString> &cameraList);

    bool initDaHengNetAreaScan(const QString &cameraIP);
    bool initDahengUsbAreaScan(const QString &cameraSn);
    bool initHaikangNetAreaScan(const QString &cameraIP);
    bool initHaikangNetLineScan(const QString &cameraIP);

    AbstractCamera* getCamera(const QString &cameraSn, const Camera_Type &type);
    bool closeCamera(const QString &cameraSn, const Camera_Type &type);

signals:
    void photoReceived(const QString &cameraSn, const Camera_Type &type, const QImage &photo, int copyCost);
    void openCameraFinish(const QString &cameraSn, const Camera_Type &type, bool isSuccess);

public slots:
    void cameraPhotoReceived(const QImage &photo, int copyCost);

private:
    bool getCameraInfo(const AbstractCamera* camera, QString &cameraSn, Camera_Type &type);

#ifdef GLOBAL_HAIKANG
    QMap<QString, MV_CC_DEVICE_INFO>    m_HaiKangNetCameraDeviceInfo;       /* QMap<Camera IP, MV_CC_DEVICE_INFO> */

#endif

#ifdef GLOBAL_DAHENG
    QMap<QString, GX_DEVICE_IP_INFO>    m_DaHengNetCameraDeviceInfo;        /* QMap<Camera IP, GX_DEVICE_IP_INFO> */
    QList<QString>                      m_DaHengUsbCameraDeviceId;
#endif

#ifdef GLOBAL_DAHUA
    QMap<QString, Dahua::GenICam::ICameraPtr> m_DaHuaNetCameraDeviceInfo;   /* QMap<QString, Dahua::GenICam::ICameraPtr> */
#endif

#ifdef GLOBAL_DALSA_CAMERALINK
    QList<QString>                      m_dalsaServerNames;
    QList<QString>                      m_dalsaCameraLinkServerNames;
#endif

#ifdef GLOBAL_DALSA_NET
    QMap<QString, GEV_DEVICE_INTERFACE> m_DalsaNetLineScanCameraDeviceInfo; /* QMap<Camera IP, GEV_DEVICE_INTERFACE> */
#endif
   QMap<Camera_Type, QMap<QString, AbstractCamera*>> m_openCameraList;
};

#endif // CAMERAMANAGER_H
