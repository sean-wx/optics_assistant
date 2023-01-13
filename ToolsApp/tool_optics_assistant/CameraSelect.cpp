#include "CameraSelect.h"
#include "ui_CameraSelect.h"
#include <QDebug>
#include <QMimeData>
#include <QDrag>

CameraSelect::CameraSelect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraSelect)
{
    ui->setupUi(this);

    m_cameraTypeName[Camera_DahengNetAreaScan]          = u8"DaHeng Net Camera";
    m_cameraTypeName[Camera_DahengUsbAreaScan]          = u8"DaHeng Usb Camera";
    m_cameraTypeName[Camera_DalsaNetLineScan]           = u8"DalsaNetLine Camera";
    m_cameraTypeName[Camera_DalsaCameraLinkLineScan]    = u8"DalsaCameraLinkLine Camera";
    m_cameraTypeName[Camera_HaikangNetAreaScan]         = u8"HaiKang Camera";
    m_cameraTypeName[Camera_HaikangNetLineScan]         = u8"HaiKang Camera";
    m_cameraTypeName[Camera_DahuaNetLineScan]           = u8"DaHua Camera";
    m_cameraTypeName[CAMERA_DEFAULT]                    = u8"Default Camera";

    m_headers << u8"类别";
    m_interfaceType << "GigE" << "USB" << "CameraLink";
    m_brandType << u8"海康线扫" << u8"海康面阵" << u8"大恒面阵" << u8"大华线扫" << u8"大华面阵" << u8"Dalsa线扫" << u8"Dalsa面阵";

    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setHeaderLabels(m_headers);     // 设置头
    ui->treeWidget->setHeaderHidden(true);
//    ui->treeWidget->setAcceptDrops(true);           // 接受拖拽内容
//    ui->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeWidget->setDragEnabled(true);           // 设置拖拽使能
    ui->treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  // 单行选中
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::Stretch); // 列平分

    connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, &CameraSelect::itemDoubleClick, Qt::DirectConnection);
    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &CameraSelect::cameraChange, Qt::DirectConnection);
    initTreeWidget();
}

CameraSelect::~CameraSelect()
{
    clearTreeData();
    delete ui;
}

void CameraSelect::openCameraFinished(const QString &cameraSn, const Camera_Type &type, bool isSuccess)
{
    CameraFrameItem *item = getCameraItem(cameraSn, type);
    if (item != NULL) {
        item->setOpenSate(isSuccess);
    }
}

void CameraSelect::closeCameraFinished(const QString &cameraSn, const Camera_Type &type, bool isSuccess)
{
    CameraFrameItem *item = getCameraItem(cameraSn, type);
    if (item) {
        item->setOpenSate(!isSuccess);
    }
}

void CameraSelect::changeCameraFinished(const QString &cameraSn, const Camera_Type &type)
{
    QTreeWidgetItem *cameraTypeitem = getTreeWidgetItem(cameraSn, type);
    if (cameraTypeitem) {
        ui->treeWidget->setCurrentItem(cameraTypeitem);
    }
}

void CameraSelect::updataCameraList(const Camera_Type &cameraType, const QList<QString> &list)
{
    clearTreeDataByType(cameraType, list);
    ui->treeWidget->collapseItem(m_cameraTree[cameraType].cameraTypeItem.TreeWidgetItem);
    for(int i = 0; i < list.size(); i++) {
        if ((!list.at(i).isEmpty()) && (!cameraISExist(list.at(i), cameraType))) {
            if (m_cameraTree[cameraType].cameraTypeItem.TreeWidgetItem == nullptr) continue;
            QTreeWidgetItem *itemChildCamera = new QTreeWidgetItem(m_cameraTree[cameraType].cameraTypeItem.TreeWidgetItem);
            CameraFrameItem *cameraItem = new CameraFrameItem(this);
            cameraItem->setCameraItem(i + 1, list.at(i));
            cameraItem->setCameraType(cameraType);
            ui->treeWidget->setItemWidget(itemChildCamera, 0, cameraItem);
            WidgetItem item;
            item.cameraFrameItem = cameraItem;
            item.TreeWidgetItem = itemChildCamera;
            m_cameraTree[cameraType].cameraItemList.append(item);

            connect(cameraItem, &CameraFrameItem::connectCamera, this, &CameraSelect::startConnectCamera, Qt::DirectConnection);
            connect(cameraItem, &CameraFrameItem::closeCamera, this, &CameraSelect::startCloseCamera, Qt::DirectConnection);
        }
    }
    for(int i = 0; i < m_cameraTree[cameraType].cameraItemList.size(); i++) {
        if (m_cameraTree[cameraType].cameraItemList.at(i).cameraFrameItem) {
            QString cameraSN = m_cameraTree[cameraType].cameraItemList.at(i).cameraFrameItem->getCameraSn();
            m_cameraTree[cameraType].cameraItemList.at(i).cameraFrameItem->setCameraItem(i + 1, cameraSN);
        }
    }
    ui->treeWidget->expandItem(m_cameraTree[cameraType].cameraTypeItem.TreeWidgetItem);
}

void CameraSelect::initTreeWidget()
{
    clearTreeData();
    for (int i = 0; i < m_interfaceType.size(); i++) {
        QTreeWidgetItem *rootItem = new QTreeWidgetItem(ui->treeWidget);
        CameraFrameItem *InterfaceType = new CameraFrameItem(this);
        InterfaceType->setInterfaceTypeItem(m_interfaceType.at(i));
        InterfaceType->setInterfaceType(getInterfaceType(m_interfaceType.at(i)));
        ui->treeWidget->setItemWidget(rootItem, 0, InterfaceType);
        ui->treeWidget->addTopLevelItem(rootItem);
        connect(InterfaceType, &CameraFrameItem::searchCamera, this, &CameraSelect::startSearchCamera, Qt::DirectConnection);

        for(int j = 0; j < m_brandType.size(); ++j) {
            Camera_Type type = getCameraType(m_interfaceType.at(i), m_brandType.at(j));
            if (type == CAMERA_DEFAULT) {
                continue;
            }
            QTreeWidgetItem *itemChild = new QTreeWidgetItem(rootItem);
            CameraFrameItem *brandType = new CameraFrameItem(this);
            brandType->setBrandTypeItem(m_brandType.at(j));
            ui->treeWidget->setItemWidget(itemChild, 0, brandType);

            m_cameraTree[type].type = type;
            m_cameraTree[type].cameraTypeItem.TreeWidgetItem = itemChild;
            m_cameraTree[type].cameraTypeItem.cameraFrameItem = brandType;
        }
    }
    ui->treeWidget->expandAll();
}

Camera_Type CameraSelect::getCameraType(const QString &Interface, const QString &brand)
{
    if ((Interface == "GigE") && (brand == "海康面阵")) {
        return Camera_HaikangNetAreaScan;
    } else if ((Interface == "GigE") && (brand == "海康线扫")){
        return Camera_HaikangNetLineScan;
    } else if ((Interface == "GigE") && (brand == "大华线扫")){
        return Camera_DahuaNetLineScan;
    } else if ((Interface == "GigE") && (brand == "大恒面阵")){
        return Camera_DahengNetAreaScan;
    } else if ((Interface == "USB") && (brand == "大恒面阵")){
        return Camera_DahengUsbAreaScan;
    } else if ((Interface == "GigE") && (brand == "Dalsa线扫")){
        return Camera_DalsaNetLineScan;
    } else if ((Interface == "CameraLink") && (brand == "Dalsa线扫")){
        return Camera_DalsaCameraLinkLineScan;
    } else {
        return CAMERA_DEFAULT;
    }
}

InterfaceType CameraSelect::getInterfaceType(const QString &Interface)
{
    if (Interface == "GigE") {
        return Interface_GigE;
    } else if (Interface == "USB") {
        return Interface_USB;
    } else {
        return Interface_CameraLink;
    }
}

CameraFrameItem *CameraSelect::getCameraItem(const QString &cameraSn, const Camera_Type &type)
{
    for(int i = 0; i < m_cameraTree[type].cameraItemList.size(); i++) {
        if ((m_cameraTree[type].cameraItemList.at(i).cameraFrameItem != nullptr) &&
            (m_cameraTree[type].cameraItemList.at(i).cameraFrameItem->getCameraSn() == cameraSn)) {
            return m_cameraTree[type].cameraItemList.at(i).cameraFrameItem;
        }
    }
    return NULL;
}

QTreeWidgetItem *CameraSelect::getTreeWidgetItem(const QString &cameraSn, const Camera_Type &type)
{
    for(int i = 0; i < m_cameraTree[type].cameraItemList.size(); i++) {
        if ((m_cameraTree[type].cameraItemList.at(i).cameraFrameItem != nullptr) &&
            (m_cameraTree[type].cameraItemList.at(i).cameraFrameItem->getCameraSn() == cameraSn)) {
            return m_cameraTree[type].cameraItemList.at(i).TreeWidgetItem;
        }
    }
    return NULL;
}

bool CameraSelect::cameraISExist(const QString &cameraSn, const Camera_Type &type)
{
    for(int i = 0; i < m_cameraTree[type].cameraItemList.size(); i++) {
        if ((m_cameraTree[type].cameraItemList.at(i).cameraFrameItem != nullptr) &&
            (m_cameraTree[type].cameraItemList.at(i).cameraFrameItem->getCameraSn() == cameraSn)) {
            return true;
        }
    }
    return false;
}

void CameraSelect::clearTreeDataByType(const Camera_Type &type)
{
    for(int i = 0; i < m_cameraTree[type].cameraItemList.size(); i++) {
        if(m_cameraTree[type].cameraItemList[i].TreeWidgetItem != nullptr) {
            ui->treeWidget->removeItemWidget(m_cameraTree[type].cameraItemList[i].TreeWidgetItem, 0);
            delete m_cameraTree[type].cameraItemList[i].TreeWidgetItem;
            m_cameraTree[type].cameraItemList[i].TreeWidgetItem = nullptr;
        }
        if (m_cameraTree[type].cameraItemList[i].cameraFrameItem != nullptr) {
            delete m_cameraTree[type].cameraItemList[i].cameraFrameItem;
            m_cameraTree[type].cameraItemList[i].cameraFrameItem = nullptr;
        }
    }
    m_cameraTree[type].cameraItemList.clear();
}

void CameraSelect::clearTreeDataByType(const Camera_Type &type, const QList<QString> &list)
{
    for(auto &it: m_cameraTree[type].cameraItemList) {
        if ((it.cameraFrameItem != NULL) && (list.contains(it.cameraFrameItem->getCameraSn()))) {
            continue;
        }
        if(it.TreeWidgetItem != nullptr) {
            ui->treeWidget->removeItemWidget(it.TreeWidgetItem, 0);
            delete it.TreeWidgetItem;
            it.TreeWidgetItem = nullptr;
        }
        if (it.cameraFrameItem != nullptr) {
            delete it.cameraFrameItem;
            it.cameraFrameItem = nullptr;
        }
        m_cameraTree[type].cameraItemList.removeOne(it);
    }
}

void CameraSelect::clearTreeData()
{
    for(QMap<Camera_Type, CameraTreeList>::iterator cameraType = m_cameraTree.begin(); cameraType != m_cameraTree.end(); cameraType++) {
        for(int i = 0; i < cameraType->cameraItemList.size(); i++) {
            if(cameraType->cameraItemList[i].TreeWidgetItem) {
                ui->treeWidget->removeItemWidget(cameraType->cameraItemList[i].TreeWidgetItem, 0);
            }
            if (cameraType->cameraItemList[i].TreeWidgetItem != nullptr) {
                delete cameraType->cameraItemList[i].TreeWidgetItem;
                cameraType->cameraItemList[i].TreeWidgetItem = nullptr;
            }
            if (cameraType->cameraItemList[i].cameraFrameItem != nullptr) {
                delete cameraType->cameraItemList[i].cameraFrameItem;
                cameraType->cameraItemList[i].cameraFrameItem = nullptr;
            }
        }
        if(cameraType->cameraTypeItem.TreeWidgetItem) {
            ui->treeWidget->removeItemWidget(cameraType->cameraTypeItem.TreeWidgetItem, 0);

        }
        if (cameraType->cameraTypeItem.TreeWidgetItem != nullptr) {
            delete cameraType->cameraTypeItem.TreeWidgetItem;
            cameraType->cameraTypeItem.TreeWidgetItem = nullptr;
        }
        if (cameraType->cameraTypeItem.cameraFrameItem != nullptr) {
            delete cameraType->cameraTypeItem.cameraFrameItem;
            cameraType->cameraTypeItem.cameraFrameItem = nullptr;
        }
    }
    ui->treeWidget->clear();
}

void CameraSelect::cameraChange(QTreeWidgetItem *item, int column)
{
    CameraFrameItem *cameraItem = dynamic_cast<CameraFrameItem*>(ui->treeWidget->itemWidget(item, column));
    if (cameraItem != NULL) {
        if (cameraItem->getItemType() == CameraFrameItem::ItemCameraType) {
            emit changeCamera(cameraItem->getCameraSn(), cameraItem->getCameraType());
            qDebug() << u8"准备切换的相机编号是：" << cameraItem->getCameraSn() << m_cameraTypeName.value(cameraItem->getCameraType());
        }
    }
}

void CameraSelect::itemDoubleClick(QTreeWidgetItem *item, int column)
{
    CameraFrameItem *cameraItem = dynamic_cast<CameraFrameItem*>(ui->treeWidget->itemWidget(item, column));
    if (cameraItem != NULL) {
        if (cameraItem->getItemType() == CameraFrameItem::ItemCameraType) {
            if(cameraItem->getOpenSate()) {
                startCloseCamera(cameraItem->getIDNumber(), cameraItem->getCameraSn(), cameraItem->getCameraType());
            } else {
                startConnectCamera(cameraItem->getIDNumber(), cameraItem->getCameraSn(), cameraItem->getCameraType());
            }
        }
    }
}

void CameraSelect::startConnectCamera(int cameraId, const QString &cameraSn, const Camera_Type &type)
{
    Q_UNUSED(cameraId);
    qDebug() << u8"开始连接相机：" << cameraSn << m_cameraTypeName.value(type);
    emit connectCamera(cameraSn, type);
//    QMetaEnum metaEnum = QMetaEnum::fromType<Camera_Type>();
//    QString str = metaEnum.valueToKey(type);
}

void CameraSelect::startCloseCamera(int cameraId, const QString &cameraSn, const Camera_Type &type)
{
    Q_UNUSED(cameraId);
    qDebug() << u8"开始断开相机：" << cameraSn << m_cameraTypeName.value(type);
    emit closeCamera(cameraSn, type);
}

void CameraSelect::startSearchCamera(const InterfaceType &interfaceType)
{
    if (interfaceType == Interface_GigE) {
        emit searchCamera(Camera_HaikangNetAreaScan);
        emit searchCamera(Camera_HaikangNetLineScan);
        emit searchCamera(Camera_DahuaNetLineScan);
        emit searchCamera(Camera_DahengNetAreaScan);
        emit searchCamera(Camera_DalsaNetLineScan);
        qDebug() << u8"刷新:" << "GigE" << u8"的相机";
    } else if (interfaceType == Interface_USB) {
        emit searchCamera(Camera_DahengUsbAreaScan);
        qDebug() << u8"刷新:" << "USB" << u8"的相机";
    } else {
        emit searchCamera(Camera_DalsaCameraLinkLineScan);
        qDebug() << u8"刷新:" << "CameraLink" << u8"的相机";
    }
}

void CameraSelect::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
}
