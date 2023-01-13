#include "CameraFrameItem.h"
#include "ui_CameraFrameItem.h"

CameraFrameItem::CameraFrameItem(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CameraFrameItem)
{
    ui->setupUi(this);
    this->setFrameStyle(QFrame::NoFrame);
}

CameraFrameItem::~CameraFrameItem()
{
    delete ui;
}

void CameraFrameItem::setBackgroundColor(const QColor &color)
{
    QPalette pal(this->palette());
    pal.setColor(QPalette::Background, color);
    this->setAutoFillBackground(true);
    this->setBackgroundRole(QPalette::Window);
    this->setPalette(pal);
    this->show();
}

void CameraFrameItem::setOpenSate(bool isOpen)
{
    if (isOpen) {
        ui->connect->setText(u8"断开");
    } else {
        ui->connect->setText(u8"连接");
    }
    ui->openSate->setChecked(isOpen);
}

bool CameraFrameItem::getOpenSate()
{
    return ui->openSate->isChecked() ? true : false;
}

void CameraFrameItem::setBtnSearchVisible(bool enabled)
{
    ui->search->setVisible(enabled);
}

void CameraFrameItem::setBtnConnectVisible(bool enabled)
{
    ui->connect->setVisible(enabled);
}

void CameraFrameItem::setLabIDNumberVisible(bool enabled)
{
    ui->IDNumber->setVisible(enabled);
    ui->IDTitel->setVisible(enabled);
}

void CameraFrameItem::setLabCameraSnVisible(bool enabled)
{
    ui->CameraSn->setVisible(enabled);
}

void CameraFrameItem::setOpenSateVisible(bool enabled)
{
    ui->openSate->setVisible(enabled);
    ui->label->setVisible(enabled);
}

void CameraFrameItem::setIDNumber(int id)
{
    ui->IDNumber->setText(QString::number(id));
}

int CameraFrameItem::getIDNumber()
{
    return ui->IDNumber->text().toInt();
}

void CameraFrameItem::setCameraSn(const QString &cameraSn)
{
    ui->CameraSn->setText(cameraSn);
}

QString CameraFrameItem::getCameraSn()
{
    return ui->CameraSn->text();
}

void CameraFrameItem::setInterfaceTypeItem(const QString &interfaceType)
{
    setCameraSn(interfaceType);
    setBtnSearchVisible(true);
    setBtnConnectVisible(false);
    setLabIDNumberVisible(false);
    setLabCameraSnVisible(true);
    setOpenSateVisible(false);
    setOpenSateVisible(false);
    m_itemType = ItemInterfaceType;
}

void CameraFrameItem::setBrandTypeItem(const QString &brandType)
{
    setCameraSn(brandType);
    setBtnSearchVisible(false);
    setBtnConnectVisible(false);
    setLabIDNumberVisible(false);
    setLabCameraSnVisible(true);
    setOpenSateVisible(false);
    m_itemType = ItemBrandType;
}

void CameraFrameItem::setCameraItem(int id, const QString &cameraSn)
{
    setCameraSn(cameraSn);
    setIDNumber(id);
    setBtnSearchVisible(false);
    setBtnConnectVisible(true);
    setLabIDNumberVisible(true);
    setLabCameraSnVisible(true);
    setOpenSateVisible(true);
    m_itemType = ItemCameraType;
}

void CameraFrameItem::setCameraType(const Camera_Type &type)
{
    m_cameraType = type;
}

void CameraFrameItem::setInterfaceType(const InterfaceType &interfaceType)
{
    m_interfaceType = interfaceType;
}

void CameraFrameItem::on_search_clicked()
{
    emit searchCamera(m_interfaceType);
}

void CameraFrameItem::on_connect_clicked()
{
    if (!ui->openSate->isChecked()) {
        emit connectCamera(this->getIDNumber(), this->getCameraSn(), m_cameraType);
    } else {
        emit closeCamera(this->getIDNumber(), this->getCameraSn(), m_cameraType);
    }
}
