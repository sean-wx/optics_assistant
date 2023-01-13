#ifndef CAMERASELECT_H
#define CAMERASELECT_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QMap>
#include <QDropEvent>
#include "CameraFrameItem.h"
#include "comm_def.h"

namespace Ui {
class CameraSelect;
}

class CameraSelect : public QWidget
{
    Q_OBJECT

public:
    explicit CameraSelect(QWidget *parent = 0);
    ~CameraSelect();

    struct CameraInfo
    {
            int number      = 0;
        QString cameraSn    = "";
        friend QDebug operator <<(QDebug debug, const CameraInfo &info) {
            debug << u8"模型数据"
                  << info.number
                  << info.cameraSn;
            return debug;
        }
    };

    struct WidgetItem
    {
        QTreeWidgetItem *TreeWidgetItem     = NULL;
        CameraFrameItem *cameraFrameItem    = NULL;
        bool operator ==(const WidgetItem &item) const{
            if((item.cameraFrameItem == this->cameraFrameItem)
            && (item.TreeWidgetItem == this->TreeWidgetItem)) {
                return true;
            }
            return false;
        }
    };

    struct CameraTreeList
    {
                    Camera_Type  type           = CAMERA_DEFAULT;      // 相机类别
                     WidgetItem  cameraTypeItem;                       // 相机类别item
              QList<WidgetItem>  cameraItemList;                       // 相机item列表
    };

signals:
    void changeCamera(const QString &cameraSn, const Camera_Type &type);
    void connectCamera(const QString &cameraSn, const Camera_Type &type);
    void closeCamera(const QString &cameraSn, const Camera_Type &type);
    void searchCamera(const Camera_Type &cameraType);

public slots:
    void openCameraFinished(const QString &cameraSn, const Camera_Type &type, bool isSuccess);
    void closeCameraFinished(const QString &cameraSn, const Camera_Type &type, bool isSuccess);
    void changeCameraFinished(const QString &cameraSn, const Camera_Type &type);
    void updataCameraList(const Camera_Type &cameraType, const QList<QString> &list);

private slots:
    void startConnectCamera(int cameraId, const QString &cameraSn, const Camera_Type &type);
    void startCloseCamera(int cameraId, const QString &cameraSn, const Camera_Type &type);
    void startSearchCamera(const InterfaceType &interfaceType);
    void itemDoubleClick(QTreeWidgetItem *item, int column);
    void cameraChange(QTreeWidgetItem *item, int column);

private:
    void mouseMoveEvent(QMouseEvent *event);
    void initTreeWidget();

    Camera_Type getCameraType(const QString &Interface, const QString &brand);
    InterfaceType getInterfaceType(const QString &Interface);
    CameraFrameItem* getCameraItem(const QString &cameraSn, const Camera_Type &type);
    QTreeWidgetItem* getTreeWidgetItem(const QString &cameraSn, const Camera_Type &type);
    bool cameraISExist(const QString &cameraSn, const Camera_Type &type);

    void clearTreeDataByType(const Camera_Type &type);
    void clearTreeDataByType(const Camera_Type &type, const QList<QString> &list);
    void clearTreeData();

    Ui::CameraSelect *ui;

    QStringList          m_headers;
    QStringList          m_interfaceType;
    QStringList          m_brandType;
    QMap<Camera_Type, QString>          m_cameraTypeName;
    QMap<Camera_Type, CameraInfo>       m_camerMap;
    QMap<Camera_Type, CameraTreeList>   m_cameraTree;
    bool m_isPress = false;
};

#endif // CAMERASELECT_H
