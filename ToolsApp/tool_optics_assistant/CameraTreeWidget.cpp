#include <QDrag>
#include <QMimeData>
#include "CameraTreeWidget.h"
#include "CameraFrameItem.h"

CameraTreeWidget::CameraTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    setDragEnabled(true);
//    setAcceptDrops(true);
//    setDropIndicatorShown(true);
}

//void CameraTreeWidget::dragEnterEvent(QDragEnterEvent *event)
//{
//    if (event->mimeData()->hasFormat(QString("camera"))) {
//        if (event->source() == this) {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        } else {
//            event->acceptProposedAction();
//        }
//    }

//    QTreeWidget::dragEnterEvent(event);
//    return;
//}

//void CameraTreeWidget::mouseMoveEvent(QMouseEvent *event)
//{
//    if (event->buttons() & Qt::LeftButton) {
//        QDrag *drag = new QDrag(this);
//        QMimeData *mimeData = new QMimeData();
//        QByteArray data;
//        QDataStream dataStream(&data, QIODevice::WriteOnly);

//        QTreeWidgetItem *item = this->currentItem();
//        CameraFrameItem *camera = NULL;
//        camera= dynamic_cast<CameraFrameItem*>(this->itemWidget(item, 0));
//        // 找到拖动的项
//        if (!camera) {
//            QTreeWidget::mouseMoveEvent(event);
//            return;
//        }

//        dataStream << camera->getCameraSn() << camera->getCameraType();
//        mimeData->setData(QString("camera"), data);
//        qDebug() << camera->getCameraSn() << camera->getCameraType();
//        drag->setMimeData(mimeData);
//        drag->setHotSpot(QPoint(0, 0));

//        drag->exec();
//        QTreeWidget::mouseMoveEvent(event);
//        return;
//    }
//}

//void CameraTreeWidget::dragMoveEvent(QDragMoveEvent *event)
//{
//    qDebug() << 11111 << 2344;
//    if (event->mimeData()->hasFormat(QString("camera"))) {
////        event->setDropAction(Qt::MoveAction);
////        event->accept();
////    } else {
////        event->ignore();
////    }
//        if (event->source() == this) {
//            event->setDropAction(Qt::MoveAction);
//            event->accept();
//        } else {
//            event->acceptProposedAction();
//        }
//    } else {
//        event->ignore();
//    }
//    QTreeWidget::dragMoveEvent(event);
//}

void CameraTreeWidget::startDrag(Qt::DropActions /*supportedActions*/)
{
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData();
    QByteArray data;
    QDataStream dataStream(&data, QIODevice::WriteOnly);

    QTreeWidgetItem *item = this->currentItem();
    CameraFrameItem *camera = NULL;
    camera = dynamic_cast<CameraFrameItem*>(this->itemWidget(item, 0));
    // 找到拖动的项
    if (!camera || camera->getItemType() != CameraFrameItem::ItemCameraType) {
        return;
    }

    dataStream << camera->getCameraSn() << camera->getCameraType();
    mimeData->setData(QString("camera"), data);
    qDebug() << this << u8"正在拖动的相机:" << camera->getCameraSn() << camera->getCameraType();
    drag->setMimeData(mimeData);
    drag->setHotSpot(QPoint(0, 0));

    if (drag->exec(Qt::MoveAction) == Qt::MoveAction) {

    }
}
