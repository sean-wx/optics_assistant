#include <QHostAddress>
#include "CameraManager.h"

/* Hk API */
#ifdef HAIKANG
#include "MvCameraControl.h"
#endif

/* DaHua API */
#ifdef DAHUA
#include "GenICam/GigE/GigECamera.h"
using namespace Dahua::GenICam;
using namespace Dahua::Infra;
#endif

/* Dalsa CameraLink API */
#ifdef DALSA_CAMERALINK
#include "SapClassBasic.h"
#endif

CameraManager::CameraManager(QObject *parent) : QObject(parent)
{
    /* DaHeng API */
#ifdef GLOBAL_DAHENG
    TM_DahengUsbAreaScanCamera::initLib();
#endif

    /* 安装Dalsa相机API驱动库 */
#ifdef GLOBAL_DALSA_NET
    DalsaNetLineScanCamera::initLib();
#endif
}

CameraManager::~CameraManager()
{
    for(auto cameraType = m_openCameraList.begin(); cameraType != m_openCameraList.end(); cameraType++) {
        for(auto it = cameraType.value().begin(); it != cameraType.value().end(); it++) {
            if (it.value() !=  NULL) {
                delete it.value();
                it.value() = NULL;
            }
        }
    }
    m_openCameraList.clear();

    /* DaHeng API */
#ifdef GLOBAL_DAHENG
    TM_DahengUsbAreaScanCamera::UnInitLib();
#endif

    /* 安装Dalsa相机API驱动库 */
#ifdef GLOBAL_DALSA_NET
    DalsaNetLineScanCamera::UnInitLib();
#endif
}

bool CameraManager::searchDahengNetAreaScan(QList<QString> &cameraList)
{
    cameraList.clear();
    m_DaHengNetCameraDeviceInfo.clear();
    DahengNetAreaCamera::enumDevices(m_DaHengNetCameraDeviceInfo);
    if (!m_DaHengNetCameraDeviceInfo.isEmpty()) {
        for (auto camera = m_DaHengNetCameraDeviceInfo.begin(); camera != m_DaHengNetCameraDeviceInfo.end(); camera++) {
            cameraList.append(camera.key());
        }
    }
    if (cameraList.isEmpty()) {
        return false;
    } else {
        return true;
    }
}

bool CameraManager::searchDahengUsbAreaScan(QList<QString> &cameraList)
{
    cameraList.clear();
    m_DaHengUsbCameraDeviceId.clear();
    DahengUsbAreaCamera::enumDevices(m_DaHengUsbCameraDeviceId);
    if (!m_DaHengUsbCameraDeviceId.isEmpty()) {
        for (auto camera: m_DaHengUsbCameraDeviceId) {
            cameraList.append(camera);
        }
    }
    if (cameraList.isEmpty()) {
        return false;
    } else {
        return true;
    }
}

bool CameraManager::searchHaikangNetAreaScan(QList<QString> &cameraList)
{
    cameraList.clear();
    m_HaiKangNetCameraDeviceInfo.clear();
    HaikangNetAreaCamera::enumDevices(m_HaiKangNetCameraDeviceInfo);
    if (!m_HaiKangNetCameraDeviceInfo.isEmpty()) {
        for (auto camera = m_HaiKangNetCameraDeviceInfo.begin(); camera != m_HaiKangNetCameraDeviceInfo.end(); camera++) {
            cameraList.append(camera.key());
        }
    }
    if (cameraList.isEmpty()) {
        return false;
    } else {
        return true;
    }
}

bool CameraManager::searchHaikangNetLineScan(QList<QString> &cameraList)
{
    return searchHaikangNetAreaScan(cameraList);
}

bool CameraManager::initDaHengNetAreaScan(const QString &cameraIP)
{
    bool res = false;
    if (!m_openCameraList[Camera_DahengNetAreaScan][cameraIP]) {
        if (!m_DaHengNetCameraDeviceInfo.isEmpty()) {
            GX_DEVICE_IP_INFO info;
            for (auto di: m_DaHengNetCameraDeviceInfo) {
                if (cameraIP == QString(di.szIP)) {
                    memcpy(&info, &di, sizeof(GX_DEVICE_IP_INFO));
                    break;
                }
            }
            AbstractCamera *camera(new TM_DahengNetAreaScanCamera(info, 1));
            connect(camera, &TM_DahengNetAreaScanCamera::photoReceived, this, &CameraManager::cameraPhotoReceived);
            m_openCameraList[Camera_DahengNetAreaScan][cameraIP] = camera;
        }
    }
    if(m_openCameraList[Camera_DahengNetAreaScan][cameraIP]->isOpen()) {
        res = true;
    } else {
        res = m_openCameraList[Camera_DahengNetAreaScan][cameraIP]->init();
    }

    QString cameraSn = "";
    Camera_Type type = CAMERA_DEFAULT;
    if (getCameraInfo(m_openCameraList[Camera_DahengNetAreaScan][cameraIP], cameraSn, type)) {
        emit openCameraFinish(cameraSn, type, res);
    }

    return res;
}

bool CameraManager::initDahengUsbAreaScan(const QString &cameraSn)
{
    bool res = false;
    QString cameraName = "";
    if (!m_openCameraList[Camera_DahengUsbAreaScan][cameraSn]) {
        if (!m_DaHengUsbCameraDeviceId.isEmpty()) {
            for (int i = 0; i < m_DaHengUsbCameraDeviceId.size(); i++) {
                if (m_DaHengUsbCameraDeviceId.at(i) == cameraSn) {
                    cameraName = m_DaHengUsbCameraDeviceId.at(i);
                    break;
                }
            }
            if (!cameraName.isEmpty()) {
                AbstractCamera *camera(new TM_DahengUsbAreaScanCamera(cameraName, 1));
                connect(camera, &TM_DahengUsbAreaScanCamera::photoReceived, this, &CameraManager::cameraPhotoReceived);
                m_openCameraList[Camera_DahengUsbAreaScan][cameraName] = camera;
            }
        }
    } else {
        cameraName = cameraSn;
    }
    if(m_openCameraList[Camera_DahengUsbAreaScan][cameraName]->isOpen()) {
        res = true;
    } else {
        res = m_openCameraList[Camera_DahengUsbAreaScan][cameraName]->init();
    }

    Camera_Type type = CAMERA_DEFAULT;
    if (getCameraInfo(m_openCameraList[Camera_DahengUsbAreaScan][cameraName], cameraName, type)) {
        emit openCameraFinish(cameraSn, type, res);
    }

    return res;
}

bool CameraManager::initHaikangNetAreaScan(const QString &cameraIP)
{
    bool res = false;
    if (!m_openCameraList[Camera_HaikangNetAreaScan][cameraIP]) {
        if (!m_HaiKangNetCameraDeviceInfo.isEmpty()) {
            MV_CC_DEVICE_INFO info;
            for (auto di: m_HaiKangNetCameraDeviceInfo) {
                if (cameraIP == QHostAddress(di.SpecialInfo.stGigEInfo.nCurrentIp).toString()) {
                    memcpy(&info, &di, sizeof(MV_CC_DEVICE_INFO));
                    break;
                }
            }
            AbstractCamera *camera(new TM_HaikangNetAreaScanCamera(info, 1));
            connect(camera, &TM_HaikangNetAreaScanCamera::photoReceived, this, &CameraManager::cameraPhotoReceived);
            m_openCameraList[Camera_HaikangNetAreaScan][cameraIP] = camera;
        }
    }
    if(m_openCameraList[Camera_HaikangNetAreaScan][cameraIP]->isOpen()) {
        res = true;
    } else {
        res = m_openCameraList[Camera_HaikangNetAreaScan][cameraIP]->init();
    }

    QString cameraSn = "";
    Camera_Type type = CAMERA_DEFAULT;
    if (getCameraInfo(m_openCameraList[Camera_HaikangNetAreaScan][cameraIP], cameraSn, type)) {
        emit openCameraFinish(cameraSn, type, res);
    }

    return res;
}

bool CameraManager::initHaikangNetLineScan(const QString &cameraIP)
{
    bool res = false;
    if (!m_openCameraList[Camera_HaikangNetLineScan][cameraIP]) {
        if (!m_HaiKangNetCameraDeviceInfo.isEmpty()) {
            MV_CC_DEVICE_INFO info;
            for (auto di: m_HaiKangNetCameraDeviceInfo) {
                if (cameraIP == QHostAddress(di.SpecialInfo.stGigEInfo.nCurrentIp).toString()) {
                    memcpy(&info, &di, sizeof(MV_CC_DEVICE_INFO));
                    break;
                }
            }
            AbstractCamera *camera(new TM_HaikangNetLineScanCamera(info, 1));
            connect(camera, &TM_HaikangNetLineScanCamera::photoReceived, this, &CameraManager::cameraPhotoReceived);
            m_openCameraList[Camera_HaikangNetLineScan][cameraIP] = camera;
        }
    }
    if(m_openCameraList[Camera_HaikangNetLineScan][cameraIP]->isOpen()) {
        res = true;
    } else {
        res = m_openCameraList[Camera_HaikangNetLineScan][cameraIP]->init();
    }

    QString cameraSn = "";
    Camera_Type type = CAMERA_DEFAULT;
    if (getCameraInfo(m_openCameraList[Camera_HaikangNetLineScan][cameraIP], cameraSn, type)) {
        emit openCameraFinish(cameraSn, type, res);
    }

    return res;
}

AbstractCamera *CameraManager::getCamera(const QString &cameraSn, const Camera_Type &type)
{
    return m_openCameraList[type][cameraSn];
}

bool CameraManager::closeCamera(const QString &cameraSn, const Camera_Type &type)
{
    if (m_openCameraList[type][cameraSn] != NULL) {
        if (m_openCameraList[type][cameraSn]->isOpen()) {
            if (m_openCameraList[type][cameraSn]->isGrab()) {
                m_openCameraList[type][cameraSn]->stop();
            }
            bool res = m_openCameraList[type][cameraSn]->close();
            if (res) {
                delete m_openCameraList[type][cameraSn];
                m_openCameraList[type][cameraSn] = NULL;
            }
            return res;
        }
    }
    return false;
}

void CameraManager::cameraPhotoReceived(const QImage &photo, int copyCost)
{
    AbstractCamera *camera = dynamic_cast<AbstractCamera*>(sender());
    if (camera != NULL) {
        QString cameraSn = "";
        Camera_Type type = CAMERA_DEFAULT;
        if (getCameraInfo(camera, cameraSn, type)) {
            emit photoReceived(cameraSn, type, photo, copyCost);
        }
    }
}

bool CameraManager::getCameraInfo(const AbstractCamera *camera, QString &cameraSn, Camera_Type &type)
{
    for(auto cameraType = m_openCameraList.begin(); cameraType != m_openCameraList.end(); cameraType++) {
        for(auto it = cameraType.value().begin(); it != cameraType.value().end(); it++) {
            if (it.value() == camera) {
                cameraSn = it.key();
                type = cameraType.key();
                return true;
            }
        }
    }
    return false;
}
