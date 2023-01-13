#ifndef CAMERAFRAMEITEM_H
#define CAMERAFRAMEITEM_H

#include <QFrame>
#include "comm_def.h"

namespace Ui {
class CameraFrameItem;
}

class CameraFrameItem : public QFrame
{
    Q_OBJECT

public:
    explicit CameraFrameItem(QWidget *parent = 0);
    ~CameraFrameItem();

    enum ItemType
    {
        ItemDefaultType   = 0,
        ItemInterfaceType = 1,
        ItemBrandType     = 2,
        ItemCameraType    = 3,
    };

    void setBackgroundColor(const QColor &color);

    void setOpenSate(bool isOpen);
    bool getOpenSate();

    void setIDNumber(int id);
    int getIDNumber();

    void setCameraSn(const QString &cameraSn);
    QString getCameraSn();

    void setInterfaceTypeItem(const QString &interfaceType);
    void setBrandTypeItem(const QString &brandType);
    void setCameraItem(int id, const QString &cameraSn);

    ItemType getItemType() { return m_itemType; }

    void setCameraType(const Camera_Type &type);
    Camera_Type getCameraType() { return m_cameraType; }

    void setInterfaceType(const InterfaceType &interfaceType);
    InterfaceType getInterfaceType() { return m_interfaceType; }

signals:
    void connectCamera(int cameraId, const QString &cameraSn, const Camera_Type &type);
    void closeCamera(int cameraId, const QString &cameraSn, const Camera_Type &type);
    void searchCamera(const InterfaceType &interfaceType);

private slots:
    void on_search_clicked();
    void on_connect_clicked();

private:
    void setBtnSearchVisible(bool enabled);
    void setBtnConnectVisible(bool enabled);
    void setLabIDNumberVisible(bool enabled);
    void setLabCameraSnVisible(bool enabled);
    void setOpenSateVisible(bool enabled);

    Ui::CameraFrameItem     *ui;
    ItemType                 m_itemType      = ItemDefaultType;
    Camera_Type              m_cameraType    = CAMERA_DEFAULT;
    InterfaceType            m_interfaceType = Interface_GigE;
};

#endif // CAMERAFRAMEITEM_H
