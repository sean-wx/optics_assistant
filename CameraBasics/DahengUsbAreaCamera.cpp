#include <QDebug>

#ifdef GLOBAL_DAHENG
#include "DahengUsbAreaCamera.h"

DahengUsbAreaCamera::DahengUsbAreaCamera(const QString &cameraSn)
    :DahengAreaCamera()
{
    m_cameraSN = cameraSn;
    m_cameraType = "SN";
    m_cameraInfo = m_cameraSN;
}

DahengUsbAreaCamera::~DahengUsbAreaCamera()
{
    if (m_handle) {
        if (m_isGrabbing)
            stopGrabbing();

        if (m_isOpened)
            closeDevice();
    }
}

bool DahengUsbAreaCamera::enumDevices(QList<QString> &cameraInfoList)
{
    unsigned int deviceNumber = 0;
    GX_STATUS emStatus = GXUpdateDeviceList(&deviceNumber, ENUMRATE_TIME_OUT);
    GX_DEVICE_BASE_INFO *pBaseInfo; //< Pointer struct of Device info
    if(emStatus == GX_STATUS_SUCCESS) {
        if(deviceNumber > 0) {
            try {
                pBaseInfo = new GX_DEVICE_BASE_INFO[deviceNumber];
            }
            catch (std::bad_alloc &e) {
                qCritical() << QString("分配内存错误，无法分配内存，请退出此应用！");
                if (pBaseInfo != NULL) {
                    delete pBaseInfo;
                    pBaseInfo = NULL;
                }
                return false;
            }
            // Set size of function "GXGetAllDeviceBaseInfo"
            size_t deviceInfoSize = deviceNumber * sizeof(GX_DEVICE_BASE_INFO);

            // Get all device baseinfo
            emStatus = GXGetAllDeviceBaseInfo(pBaseInfo, &deviceInfoSize);
            if (emStatus != GX_STATUS_SUCCESS) {
                qWarning() << QString("获取相机设备信息失败!");
                if (pBaseInfo != NULL) {
                    delete[] pBaseInfo;
                    pBaseInfo = NULL;
                }

                // Reset device number
                deviceNumber = 0;
                return false;
            }
            qDebug() << QString("系统中扫描到%1个大恒USB相机").arg(QString::number(deviceNumber));

            for(int i=0; i<int(deviceNumber); i++) {
                QString daHengUsbCmeraSn = QString(pBaseInfo[i].szSN);
                qDebug() << QString("当前获取到的大恒USB相机SN是：%1，索引是：%2").arg(daHengUsbCmeraSn).arg(i);
                cameraInfoList.append(daHengUsbCmeraSn);
            }
            if (cameraInfoList.size() > 0) {
                return true;
            } else {
                qCritical() << QString("网络中未扫描到大恒USB相机");
            }
        }
    } else {
        qWarning() << "扫描网络中的大恒USB相机失败，错误码：" << QString::asprintf("%08X", emStatus);
    }
    return false;
}

bool DahengUsbAreaCamera::getCameraDeviceNum(unsigned int &deviceNum)
{
    QList<QString> cameraList;
    bool result = enumDevices(cameraList);
    deviceNum = cameraList.size();
    return result;
}

bool DahengUsbAreaCamera::openDevice()
{
    if (m_isOpened) {
        printStateErrorInfo(QString("打开"), QString("该相机已被打开"));
        return true;
    }

    QByteArray cameraSN = m_cameraSN.toLatin1();
    GX_OPEN_PARAM stOpenParam;
    stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
    stOpenParam.openMode   = GX_OPEN_SN;
    stOpenParam.pszContent = cameraSN.data();

    bool result = false;
    GX_STATUS resultCode = GXOpenDevice(&stOpenParam, &m_handle);
    if (GX_STATUS_SUCCESS == resultCode) {
        result = true;
        m_isOpened = true;
        m_deviceOffline = false;
    } else {
        m_isOpened = false;
    }

    printDeviceControlStateInfo(QString("打开"), result, resultCode);
    return result;
}

bool DahengUsbAreaCamera::setStreamTransferSize(unsigned int streamTransferSize)
{
    return setIntValue(GX_DS_INT_STREAM_TRANSFER_SIZE, streamTransferSize);//设置usb相机块大小为1M（1024*1024）
}

bool DahengUsbAreaCamera::setStreamTransferNumber(unsigned int streamTransferNumber)
{
    return setIntValue(GX_DS_INT_STREAM_TRANSFER_NUMBER_URB, streamTransferNumber);
}

#endif // GLOBAL_DAHENG
