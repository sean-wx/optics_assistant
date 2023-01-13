#include <QGridLayout>
#include <QThread>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QDateTime>
#include <QMutexLocker>
#include <QMessageBox>
#include "CameraImageWindow.h"
#include "ui_CameraImageWindow.h"
#include "CreateXML.h"
#include "SaveImageConfig.h"

CameraImageWindow::CameraImageWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraImageWindow)
{
    ui->setupUi(this);
//    setDragEnabled(true);
    setItemNumber(1, 1);
    setCameraViewFocus(NULL);
    connect(&m_imageDetection, &ImageDetection::updataIamgeCensusData, this, &CameraImageWindow::receiveIamgeCensusData, Qt::DirectConnection);
}

CameraImageWindow::~CameraImageWindow()
{
    deleteAllView();
    delete ui;
}

void CameraImageWindow::setItemNumber(int row, int col)
{
    m_itemRow = row;
    m_itemCol = col;
    initItem(m_itemRow, m_itemCol);
}

void CameraImageWindow::setShowType(AssessmentMethod type)
{
    if (m_currentView != NULL) {
        m_currentView->setShowType(type);
    }
}

void CameraImageWindow::setShowDiff(bool value)
{
    if (m_currentView != NULL) {
        m_currentView->setShowDiff(value);
    }
}

void CameraImageWindow::setRulerShow(bool value)
{
    if (m_currentView != NULL) {
        m_currentView->setRulerShow(value);
    }
}

void CameraImageWindow::setCursorShow(bool value)
{
    if (m_currentView != NULL) {
        m_currentView->setCrossCursorShow(value);
    }
}

void CameraImageWindow::setCenterCursor()
{
    if (m_currentView != NULL) {
        m_currentView->drawCenterCrossCursorAction();
    }
}

void CameraImageWindow::setLowerLimit(int value)
{
    if (m_currentView != NULL) {
        m_currentView->setLowerLimit(value);
        m_statisticalData[m_currentView->getCameraType()][m_currentView->getCameraSn()].LowerLimit = value;
    }
}

void CameraImageWindow::setUpperLimit(int value)
{
    if (m_currentView != NULL) {
        m_currentView->setUpperLimit(value);
        m_statisticalData[m_currentView->getCameraType()][m_currentView->getCameraSn()].UpperLimit = value;
    }
}

void CameraImageWindow::setOffsetX(int value)
{
    if (m_currentView != NULL) {
        m_currentView->setOffsetX(value);
        m_statisticalData[m_currentView->getCameraType()][m_currentView->getCameraSn()].OffsetX = value;
    }
}

void CameraImageWindow::setOffsetY(int value)
{
    if (m_currentView != NULL) {
        m_currentView->setOffsetY(value);
        m_statisticalData[m_currentView->getCameraType()][m_currentView->getCameraSn()].OffsetY = value;
    }
}

void CameraImageWindow::setRulerSpacing(int value)
{
    if (m_currentView != NULL) {
        m_currentView->setRulerSpacing(value);
        m_currentView->update();
    }
}

void CameraImageWindow::setRatio(float ratio)
{
    if (m_currentView != NULL) {
        m_currentView->setScale(ratio);
    }
}

void CameraImageWindow::getCurrentViewInfo(QString &cameraSn, Camera_Type &cameraType)
{
    if (m_currentView != NULL) {
        cameraSn = m_currentView->getCameraSn();
        cameraType = m_currentView->getCameraType();
    }
}

void CameraImageWindow::setAutomaticMatching()
{
    QString cameraSn;
    Camera_Type type;
    if (m_currentView != NULL) {
        cameraSn = m_currentView->getCameraSn();
        type = m_currentView->getCameraType();

        if (m_statisticalData[type][cameraSn].MouldImage.isNull() || m_statisticalData[type][cameraSn].RealTimeImage.isNull()) {
            return;
        }
        m_currentView->setAssessRectShow(false);
        QRect polygonRect = m_imageDetection.GetPolygonToRect(m_statisticalData[type][cameraSn].ProductMarkPolygon);
        QImage modelImg = m_statisticalData[type][cameraSn].MouldImage.copy(polygonRect);
        QRect templateRect;
        float similarity = 0;
        m_imageDetection.templateMatch(m_statisticalData[type][cameraSn].RealTimeImage, modelImg, templateRect, similarity);
        if (similarity > 0.) {
            m_statisticalData[type][cameraSn].OffsetX = templateRect.x() - polygonRect.x();
            m_statisticalData[type][cameraSn].OffsetY = templateRect.y() - polygonRect.y();

            m_currentView->setOffsetX(m_statisticalData[type][cameraSn].OffsetX);
            m_currentView->setOffsetY(m_statisticalData[type][cameraSn].OffsetY);
        }
        qDebug() << u8"自动定位的偏差：" << templateRect.x() - polygonRect.x() << templateRect.y() - polygonRect.y();
        emit updataInfo(QString(u8"匹配相似度：").append(QString::number(similarity)), false);
        emit updataInfo(QString(u8"自动定位的偏差：(%1, %2)").
                        arg(QString::number(templateRect.x() - polygonRect.x())).
                        arg(QString::number(templateRect.y() - polygonRect.y())),
                        false);
        printMessageBoxInfo(QString(u8"定位完成，坐标为（%1，%2），相似度为 %3。")
                            .arg(templateRect.x())
                            .arg(templateRect.y())
                            .arg(similarity));
        m_currentView->setAssessRectShow(true);
    }
}

void CameraImageWindow::setPolygonShow(bool isShow)
{
    if (m_currentView != NULL) {
        m_currentView->setPolygonShow(isShow);
    }
}

void CameraImageWindow::setAssessRectShow(bool isShow)
{
    if (m_currentView != NULL) {
        m_currentView->setAssessRectShow(isShow);
    }
}

void CameraImageWindow::updateColor()
{
    for(auto cameraView = m_imageView.begin(); cameraView != m_imageView.end(); cameraView++) {
        if (cameraView.value() == NULL) continue;
        cameraView.value()->updateColor();
    }
}

void CameraImageWindow::setChangeCamera(const QString &cameraSn,const Camera_Type &cameraType)
{
    ImageView *view = getImageView(cameraSn, cameraType);
    if (view != NULL) {
        setCameraViewFocus(view);
    }
}

void CameraImageWindow::updataMapping(const QString &cameraSn, const Camera_Type &cameraType)
{
    ImageView *view = dynamic_cast<ImageView*>(sender());
    if (view != NULL) {
        for(auto cameraView = m_imageView.begin(); cameraView != m_imageView.end(); cameraView++) {
            if (cameraView.value() == NULL) continue;
            if ((cameraView.value()->getCameraType() == cameraType) &&
                (cameraView.value()->getCameraSn() == cameraSn) &&
                (cameraView.value() != view)) {
                cameraView.value()->setCameraSn("");
                cameraView.value()->setCameraType(CAMERA_DEFAULT);
                cameraView.value()->cleraView();
            }
        }
        view->setAssessRectShow(false);
        view->setPixmap(QPixmap::fromImage(m_statisticalData[cameraType][cameraSn].MouldImage));
        view->setOffsetX(m_statisticalData[cameraType][cameraSn].OffsetX);
        view->setOffsetY(m_statisticalData[cameraType][cameraSn].OffsetY);
        view->setPolygon(m_statisticalData[cameraType][cameraSn].ProductMarkPolygon);
        view->setAssessRectShow(true);
        view->setAssessRect(m_statisticalData[cameraType][cameraSn].AssessmentRect);
        view->onPresetImage();
        emit updataInfo(u8"切换窗口成功。" , false);
    }
}

void CameraImageWindow::updataModelImageFile(const QString &cameraSn, const Camera_Type &type, const ConnectData &connectData)
{
    m_statisticalData[type][cameraSn].ConnectConfig = connectData;
    m_statisticalData[type][cameraSn].MouldImage = QImage(connectData.ModelImageFile);
    m_statisticalData[type][cameraSn].MouldImage = m_statisticalData[type][cameraSn].MouldImage.convertToFormat(QImage::Format_Grayscale8);
    m_statisticalData[type][cameraSn].RealTimeImage = m_statisticalData[type][cameraSn].MouldImage;
    m_statisticalData[type][cameraSn].ProductMarkPolygon.clear();
    m_statisticalData[type][cameraSn].AssessmentRect.clear();
    m_statisticalData[type][cameraSn].ModelStatistical.clear();
    m_statisticalData[type][cameraSn].ComparisonStatistical.clear();
    setMouleImage(cameraSn, type);
}

void CameraImageWindow::updataCameraImage(const QString &cameraSn, const Camera_Type &type, const QImage &photo, int copyCost)
{
    Q_UNUSED(copyCost);
    ImageView *view = getImageView(cameraSn, type);
    if (view != NULL) {
        m_imageMutex.lock();
        m_statisticalData[type][cameraSn].RealTimeImage = photo.convertToFormat(QImage::Format_Grayscale8);
        m_imageMutex.unlock();

//        view->setAssessRectShow(false);
//        QRect polygonRect = m_imageDetection.GetPolygonToRect(m_statisticalData[type][cameraSn].ProductMarkPolygon);
//        QImage modelImg = m_statisticalData[type][cameraSn].MouldImage.copy(polygonRect);
//        QRect templateRect;
//        float similarity = 0;
//        m_imageDetection.templateMatch(m_statisticalData[type][cameraSn].RealTimeImage, modelImg, templateRect, similarity);
//        if (similarity > 0.6) {
//            m_statisticalData[type][cameraSn].OffsetX = templateRect.x() - polygonRect.x();
//            m_statisticalData[type][cameraSn].OffsetY = templateRect.y() - polygonRect.y();

//            view->setOffsetX(m_statisticalData[type][cameraSn].OffsetX);
//            view->setOffsetY(m_statisticalData[type][cameraSn].OffsetY);
//        }
//        qDebug() << u8"自动定位的偏差：" << templateRect.x() - polygonRect.x() << templateRect.y() - polygonRect.y();
//        view->setAssessRectShow(true);

        view->setPixmap(QPixmap::fromImage(photo));
        emit updataCameraImageSize(photo.width(), photo.height());
    }
}

void CameraImageWindow::statisticsData(const QList<QRectF> &rectList)
{
    ImageView *view = dynamic_cast<ImageView*>(sender());
    QList<QRectF> list = rectList;
    if (view != NULL) {
        Camera_Type type = view->getCameraType();
        QString cameraSn = view->getCameraSn();
//        m_statisticalData[type][cameraSn].AssessmentRect = rectList;
        m_statisticalData[type][cameraSn].OffsetX = view->offsetX();
        m_statisticalData[type][cameraSn].OffsetY = view->offsetY();

        m_imageMutex.lock();
        QImage threadImage = m_statisticalData[type][cameraSn].RealTimeImage;
        m_imageMutex.unlock();

        m_imageDetection.getIamgeCensusDataThread(getStrByCameraSnAndType(cameraSn, type), threadImage, list);
    }
}

void CameraImageWindow::setStatisticalData(const QString &cameraSn, const Camera_Type &type, const StatisticalInfo &statisticalData)
{
    m_imageMutex.lock();
    m_statisticalData[type][cameraSn] = statisticalData;
    m_imageMutex.unlock();

    ImageView *view = getImageView(cameraSn, type);
    if (view != NULL) {
        view->setAssessRectShow(false);

        view->setPixmap(QPixmap::fromImage(statisticalData.MouldImage));
        view->setPolygon(statisticalData.ProductMarkPolygon);
        view->setAssessRect(statisticalData.AssessmentRect);
        view->setOffsetX(statisticalData.OffsetX);
        view->setOffsetY(statisticalData.OffsetY);
        view->setUpperLimit(statisticalData.UpperLimit);
        view->setLowerLimit(statisticalData.LowerLimit);
        view->setShowType(statisticalData.AssessmentType);
        view->drawCenterCrossCursorAction();

        view->setAssessRectShow(true);
        view->setOffsetX(statisticalData.OffsetX);
        updataUIData(view);
        emit updataConnectData(cameraSn, type, statisticalData.ConnectConfig);
    }
}

void CameraImageWindow::saveCameraImg(const QString &cameraSn, const Camera_Type &cameraType)
{
    SaveIamgeInfo saveConfig = SaveImageConfig::instance().cameraSaveConfig();

    QString file = saveConfig.FileName.append(QString("%1_").arg(cameraSn)).append(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")).append(".").append(saveConfig.SaveSuffix);
    QDir dir(saveConfig.SaveDir);
    if (!dir.exists()) {
        dir.mkdir(saveConfig.SaveDir);
    }

    if ((!file.isEmpty()) && (!m_statisticalData[cameraType][cameraSn].RealTimeImage.isNull())) {
        if (!file.right(6).contains(".", Qt::CaseSensitive)) {
            file += ".jpg";
        }
        qDebug() << u8"导出相机原图" << saveConfig.SaveDir + file;
        emit updataInfo(QString(u8"导出相机原图(%1)成功。").arg(saveConfig.SaveDir + file), false);
        m_statisticalData[cameraType][cameraSn].RealTimeImage.save(saveConfig.SaveDir + file);
        printMessageBoxInfo(QString(u8"导出相机原图成功。"));
    }
}

void CameraImageWindow::saveStatisticalImg(const QString &cameraSn, const Camera_Type &cameraType)
{
    SaveIamgeInfo saveConfig = SaveImageConfig::instance().statisticsSaveConfig();

    QString file = saveConfig.FileName.append(QString("%1_").arg(cameraSn)).append(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")).append(".").append(saveConfig.SaveSuffix);
    QDir dir(saveConfig.SaveDir);
    if (!dir.exists()) {
        dir.mkdir(saveConfig.SaveDir);
    }

    QImage markImg = m_imageDetection.paintMarkPolygon(m_statisticalData[cameraType][cameraSn].RealTimeImage,
                                                       m_statisticalData[cameraType][cameraSn].ProductMarkPolygon,
                                                       m_statisticalData[cameraType][cameraSn].OffsetX,
                                                       m_statisticalData[cameraType][cameraSn].OffsetY);
    QImage comparisonRectkImg = m_imageDetection.paintComparisonRect(markImg, m_statisticalData[cameraType][cameraSn].AssessmentRect);
    QImage comparisonResultImg = m_imageDetection.paintComparisonResult(comparisonRectkImg,
                                                                        m_statisticalData[cameraType][cameraSn],
                                                                        m_currentView->showType(),
                                                                        false,
                                                                        m_statisticalData[cameraType][cameraSn].UpperLimit,
                                                                        m_statisticalData[cameraType][cameraSn].LowerLimit);

    if ((!file.isEmpty()) && (!comparisonResultImg.isNull())) {
        if (!file.right(6).contains(".", Qt::CaseSensitive)) {
            file += ".jpg";
        }
        qDebug() << u8"导出统计结果" << saveConfig.SaveDir + file;
        emit updataInfo(QString("u8导出统计结果图(%1)成功。").arg(saveConfig.SaveDir + file), false);
        comparisonResultImg.save(saveConfig.SaveDir + file);
        printMessageBoxInfo(QString(u8"导出统计结果图成功。"));
    }
}

void CameraImageWindow::saveComparisonImg(const QString &cameraSn, const Camera_Type &cameraType)
{
    SaveIamgeInfo saveConfig = SaveImageConfig::instance().comparisonSaveConfig();

    QString file = saveConfig.FileName.append(QString("%1_").arg(cameraSn)).append(QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss")).append(".").append(saveConfig.SaveSuffix);
    QDir dir(saveConfig.SaveDir);
    if (!dir.exists()) {
        dir.mkdir(saveConfig.SaveDir);
    }

    QImage markImg = m_imageDetection.paintMarkPolygon(m_statisticalData[cameraType][cameraSn].RealTimeImage,
                                                       m_statisticalData[cameraType][cameraSn].ProductMarkPolygon,
                                                       m_statisticalData[cameraType][cameraSn].OffsetX,
                                                       m_statisticalData[cameraType][cameraSn].OffsetY);
    QImage comparisonRectkImg = m_imageDetection.paintComparisonRect(markImg, m_statisticalData[cameraType][cameraSn].AssessmentRect);
    QImage comparisonResultImg = m_imageDetection.paintComparisonResult(comparisonRectkImg,
                                                                        m_statisticalData[cameraType][cameraSn],
                                                                        m_currentView->showType(),
                                                                        true,
                                                                        m_statisticalData[cameraType][cameraSn].UpperLimit,
                                                                        m_statisticalData[cameraType][cameraSn].LowerLimit);

    if ((!file.isEmpty()) && (!comparisonResultImg.isNull())) {
        if (!file.right(6).contains(".", Qt::CaseSensitive)) {
            file += ".jpg";
        }
        qDebug() << u8"导出比对结果" << saveConfig.SaveDir + file;
        emit updataInfo(QString(u8"导出比对结果图(%1)成功。").arg(saveConfig.SaveDir + file), false);
        comparisonResultImg.save(saveConfig.SaveDir + file);
        printMessageBoxInfo(QString(u8"导出比对结果图成功。"));
    }
}

void CameraImageWindow::receiveIamgeCensusData(const QString &customData, const QMap<int, AssessmentData> &censusData)
{
    QString cameraSn = "";
    Camera_Type type;
    getCameraInfoByStr(customData, cameraSn, type);
    if (!cameraSn.isEmpty()) {
        ImageView *view = getImageView(cameraSn, type);
        if (view != NULL) {
            m_statisticalData[type][cameraSn].ComparisonStatistical = censusData;
            view->setStatisticalData(m_statisticalData[type][cameraSn].ModelStatistical, censusData);
        }
    }
}

void CameraImageWindow::receiveMousePoint(const QPointF &pointF)
{
    ImageView *view = dynamic_cast<ImageView*>(sender());
    if (view != NULL) {
        int value = 0;
        if (!m_statisticalData[view->getCameraType()][view->getCameraSn()].RealTimeImage.isNull()) {
            QPoint point;
            point.setX(pointF.x());
            point.setY(pointF.y());
            value = m_imageDetection.GetPixelByPoint(m_statisticalData[view->getCameraType()][view->getCameraSn()].RealTimeImage, point);
        }
        emit updataMousePoint(pointF, value);
    }
}

void CameraImageWindow::initItem(int row, int col)
{
    QGridLayout* layout = qobject_cast<QGridLayout*>(this->layout());
    if(!layout) {
        return;
    }

    deleteAllView();
    m_imageView.clear();

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            ImageView *imgView = new ImageView(this);
            connect(imgView, &ImageView::updataCamera, this, &CameraImageWindow::updataMapping, Qt::DirectConnection);
            connect(imgView, &ImageView::updataOffset, this, &CameraImageWindow::updataOffset, Qt::DirectConnection);
            connect(imgView, &ImageView::updataCrossCursor, this, &CameraImageWindow::updataCrossCursor, Qt::DirectConnection);
            connect(imgView, &ImageView::ratioChangeSignal, this, &CameraImageWindow::ratioChangeSignal, Qt::DirectConnection);
            connect(imgView, &ImageView::updataMousePoint, this, &CameraImageWindow::receiveMousePoint, Qt::DirectConnection);
            connect(imgView, &ImageView::updataCrossCursorShow, this, &CameraImageWindow::updataCrossCursorShow, Qt::DirectConnection);
            connect(imgView, &ImageView::updataRulerShow, this, &CameraImageWindow::updataRulerShow, Qt::DirectConnection);
            connect(imgView, &ImageView::updataShowDiffData, this, &CameraImageWindow::updataShowDiffData, Qt::DirectConnection);
            connect(imgView, &ImageView::requestStatisticsData, this, &CameraImageWindow::statisticsData, Qt::DirectConnection);

            imgView->systemType(ImageView::CameraImage);
            layout->addWidget(imgView, i, j);
            m_imageView[i * col + j + 1] = imgView;
        }
    }
}

ImageView *CameraImageWindow::getImageView(const QString &cameraSn,const Camera_Type &cameraType)
{
    for(auto view = m_imageView.begin(); view != m_imageView.end(); view++) {
        if (view.value() == NULL) continue;
        if ((view.value()->getCameraType() == cameraType) && (view.value()->getCameraSn() == cameraSn)) {
            return view.value();
        }
    }
    return NULL;
}

void CameraImageWindow::setCameraViewFocus(ImageView *view)
{
    if ((m_itemCol == 1) && (m_itemRow == 1)) {
        m_currentView = m_imageView[1];
        updataUIData(view);
    } else {
        for(auto& cameraView : m_imageView) {
            if (!cameraView) continue;
            if(cameraView == view) {
                m_currentView = view;
                updataUIData(view);
                cameraView->setStyleSheet("background-color: rgb(104,104,104); border:2px solid blue;");
            } else {
                cameraView->setStyleSheet("background-color: rgb(104,104,104); border:2px solid rgb(104,104,104);");
            }
        }
    }
}

void CameraImageWindow::updataUIData(ImageView *view)
{
    if (view != NULL) {
        emit updataOffset(QPointF(view->offsetX(), view->offsetY()));
        emit updataCrossCursor(view->crossCursorPos());
        emit updataMousePoint(QPoint(0, 0), 0);
        emit ratioChangeSignal(view->getScale());
        emit updataLimit(view->lowerLimit(), view->upperLimit());
        emit updataCrossCursorShow(view->crossCursorShow());
        emit updataRulerShow(view->rulerShow());
        emit updataPolygonShow(view->polygonShow());
        emit updataAssessRectShow(view->assessRectShow());
        emit updataRulerSpacing(view->rulerSpacing());
        emit updataShowType(view->showType());
        emit updataShowDiffData(view->showDiff());
        emit updataModelImageSize(view->pixmap().width(), view->pixmap().height());
    }
}

void CameraImageWindow::deleteAllView()
{
    QGridLayout* layout = qobject_cast<QGridLayout*>(this->layout());
    if(!layout) {
        return;
    }

    foreach (auto obj, m_imageView) {
        layout->removeWidget(obj);
        if (obj != NULL) {
            delete obj;
        }
        obj = NULL;
    }
    m_imageView.clear();
    m_currentView = NULL;
}

void CameraImageWindow::setMouleImage(const QString &cameraSn, const Camera_Type &cameraType)
{
    CreateXML xml;
    ModelData data;
    if(QFile::exists(m_statisticalData[cameraType][cameraSn].ConnectConfig.ModelXmlDataFile)) {
        data = xml.loadXml(m_statisticalData[cameraType][cameraSn].ConnectConfig.ModelXmlDataFile);

        m_statisticalData[cameraType][cameraSn].ProductMarkPolygon = data.ProductMarkPolygon;
        m_statisticalData[cameraType][cameraSn].AssessmentRect = data.AssessmentRect;
        m_statisticalData[cameraType][cameraSn].OffsetX = 0;
        m_statisticalData[cameraType][cameraSn].OffsetY = 0;
        m_statisticalData[cameraType][cameraSn].UpperLimit = data.UpperTolerance;
        m_statisticalData[cameraType][cameraSn].LowerLimit = data.LowerTolerance;

//        m_imageMutex.lock();
        QImage threadImg = m_statisticalData[cameraType][cameraSn].MouldImage;
        m_statisticalData[cameraType][cameraSn].ModelStatistical = m_imageDetection.getIamgeCensusDataThread(threadImg, data.AssessmentRect);
//        m_imageMutex.unlock();

        emit updataInfo(u8"导入模板文件成功" , false);
    } else {
        emit updataInfo(u8"导入模板文件失败，XML文件不存在！", true);
    }

    ImageView *view = getImageView(cameraSn, cameraType);
    if (view != NULL) {
        view->cleraView();
        view->setPixmap(QPixmap::fromImage(m_statisticalData[cameraType][cameraSn].MouldImage));
        view->setPolygon(data.ProductMarkPolygon);
        view->setAssessRect(data.AssessmentRect);
        view->setUpperLimit(data.UpperTolerance);
        view->setLowerLimit(data.LowerTolerance);
        view->setShowType(data.AssessmentType);
        view->setOffsetX(0);
        view->setOffsetY(0);
        view->drawCenterCrossCursorAction();
        view->onPresetImage();
        updataUIData(view);
    }
}

QString CameraImageWindow::getStrByCameraSnAndType(const QString &cameraSn, const Camera_Type &cameraType)
{
    return cameraSn + "%_%" + QString::number(cameraType);
}

void CameraImageWindow::getCameraInfoByStr(const QString &cameraInfo, QString &cameraSn, Camera_Type &cameraType)
{
    if ((!cameraInfo.isEmpty()) && (cameraInfo.contains("%_%"))) {
        QStringList list = cameraInfo.split("%_%");
        cameraSn = list.at(0);
        cameraType = Camera_Type(QString(list.at(1)).toInt());
    }
}

void CameraImageWindow::printMessageBoxInfo(const QString &info)
{
    QMessageBox box(QMessageBox::Information, QStringLiteral("提示!"), info);
    box.setStandardButtons(QMessageBox::Cancel);
    box.setButtonText(QMessageBox::Cancel, QStringLiteral("关闭"));
    box.setWindowFlags(box.windowFlags() | Qt::SubWindow | Qt::WindowStaysOnTopHint);
    box.exec();
}

void CameraImageWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        for(auto& view : m_imageView) {
            if (!view) continue;
            if(view->rect().contains(view->mapFromParent(event->pos()))) {
                if (view->getCameraType() != CAMERA_DEFAULT) {
                    QString cameraSn = view->getCameraSn();
                    Camera_Type type = view->getCameraType();
                    m_statisticalData[type][cameraSn].OffsetX = view->offsetX();
                    m_statisticalData[type][cameraSn].OffsetY = view->offsetY();
                    m_statisticalData[type][cameraSn].UpperLimit = view->upperLimit();
                    m_statisticalData[type][cameraSn].LowerLimit = view->lowerLimit();
                    m_statisticalData[type][cameraSn].AssessmentType = view->showType();
                    emit selectCamera(cameraSn, type, m_statisticalData[type][cameraSn]);
                }
            }
        }
    }
    QWidget::mouseDoubleClickEvent(event);
}

void CameraImageWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        for(auto& view : m_imageView) {
            if (!view) continue;
            if(view->rect().contains(view->mapFromParent(event->pos()))) {
                setCameraViewFocus(view);
                if (view->getCameraType() != CAMERA_DEFAULT) {
                    m_currentView = view;
                    updataUIData(view);
                    emit changeCamera(view->getCameraSn(), view->getCameraType());
                    qDebug() << this << u8"准备切换的相机编号是：" << view->getCameraSn() << view->getCameraType();
                }
                break;
            }
        }
//        ImageView *view = dynamic_cast<ImageView*>(this->childAt(event->pos()));
//        ImageView *view = static_cast<ImageView*>(ui->gridLayout->itechildAt(event->pos()));
//        if(view) {
//            view->setStyleSheet("background-color: rgb(104,104,104); border:2px solid rgb(193, 125, 17)");
//        }
    }
    QWidget::mousePressEvent(event);
}
