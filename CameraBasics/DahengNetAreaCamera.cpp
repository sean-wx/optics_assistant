#include <QDebug>

#ifdef GLOBAL_DAHENG
#include "DahengNetAreaCamera.h"

DahengNetAreaCamera::DahengNetAreaCamera(const GX_DEVICE_IP_INFO &deviceInfo)
    : DahengAreaCamera()
{
    m_deviceInfo = deviceInfo;
    m_cameraType = "IP";
    m_cameraInfo = QString(m_deviceInfo.szIP);
}

DahengNetAreaCamera::~DahengNetAreaCamera()
{
    if (m_handle) {
        if (m_isGrabbing)
            stopGrabbing();

        if (m_isOpened)
            closeDevice();
    }
}

bool DahengNetAreaCamera::enumDevices(QMap<QString, GX_DEVICE_IP_INFO> &cameraInfoList)
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
            qDebug() << QString("系统中扫描到%1个大恒网络相机").arg(QString::number(deviceNumber));

            for(int i=0; i<int(deviceNumber); i++) {
                GX_DEVICE_IP_INFO info;
                QString ip = "";
                emStatus = GXGetDeviceIPInfo(i+1, &info); //可以根据获取到的指定IP初始化相机
                if (emStatus == GX_STATUS_SUCCESS) {
                    ip = QString(info.szIP);
                    cameraInfoList.insert(ip, info);
                    qDebug() << QString("当前获取到的大恒网络相机IP是：%1，索引是：%2").arg(ip).arg(i);
                }
            }
            if (cameraInfoList.size() > 0) {
                return true;
            } else {
                qCritical() << QString("网络中未扫描到大恒网络相机");
            }
        }
    } else {
        qWarning() << "扫描网络中的大恒网络相机失败，错误码：" << QString::asprintf("%08X", emStatus);
    }
    return false;
}

bool DahengNetAreaCamera::getCameraDeviceNum(unsigned int &deviceNum)
{
    QMap<QString, GX_DEVICE_IP_INFO> cameraList;
    bool result = enumDevices(cameraList);
    deviceNum = cameraList.size();
    return result;
}

bool DahengNetAreaCamera::openDevice()
{
    if (m_isOpened) {
        printStateErrorInfo(QString("打开"), QString("该相机已被打开"));
        return true;
    }

    GX_OPEN_PARAM stOpenParam;
    stOpenParam.accessMode = GX_ACCESS_EXCLUSIVE;
    stOpenParam.openMode = GX_OPEN_IP;
    stOpenParam.pszContent = m_deviceInfo.szIP;

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

bool DahengNetAreaCamera::setGevSCPD(unsigned int gevSCPD)
{
    return setIntValue(GX_INT_GEV_PACKETDELAY, gevSCPD);
}

bool DahengNetAreaCamera::setPacketSize(unsigned int packetSize)
{
    return setIntValue(GX_INT_GEV_PACKETSIZE, packetSize);//defult 1500;
}

bool DahengNetAreaCamera::setHeartbeatTime(unsigned int heartbeatTime)
{
    return setIntValue(GX_INT_GEV_HEARTBEAT_TIMEOUT, heartbeatTime);
}

bool DahengNetAreaCamera::setPacketSizeTimeout(unsigned int packetSizeTimeout)
{
    return setIntValue(GX_DS_INT_PACKET_TIMEOUT, packetSizeTimeout);
}

bool DahengNetAreaCamera::setBlockTimeout(unsigned int blockTimeout)
{
    return setIntValue(GX_DS_INT_BLOCK_TIMEOUT, blockTimeout);
}

bool DahengNetAreaCamera::setResendTimeout(unsigned int resendTimeout)
{
    return setIntValue(GX_DS_INT_RESEND_TIMEOUT, resendTimeout);
}

bool DahengNetAreaCamera::setReSendEnable(bool enable)
{
    if (enable) {
        return setEnumValue(GX_DS_ENUM_RESEND_MODE, GX_DS_RESEND_MODE_ON);
    } else {
        return setEnumValue(GX_DS_ENUM_RESEND_MODE, GX_DS_RESEND_MODE_OFF);
    }
}

bool DahengNetAreaCamera::setMaxWaitPacketCount(unsigned int maxWaitPacketCount)
{
    return setIntValue(GX_DS_INT_MAX_WAIT_PACKET_COUNT, maxWaitPacketCount);
}

#endif // GLOBAL_DAHENG
