#include <QFutureWatcher>
#include <QPainter>
#include <QEventLoop>
#include <QtConcurrent>
#include <QDebug>
#include "ImageDetection.h"
#include "ColorPalette.h"

ImageDetection::ImageDetection()
{

}

void ImageDetection::getIamgeCensusDataFished()
{
    QFutureWatcher<QMap<int, AssessmentData>> *watcher = dynamic_cast<QFutureWatcher<QMap<int, AssessmentData>> *>(sender());
    m_statisticalData = watcher->future().result();
    if (!watcher->objectName().isEmpty()) {
        emit updataIamgeCensusData(watcher->objectName(), watcher->future().result());
    }
    watcher->deleteLater();
}

void ImageDetection::GetPixel(QImage *image, const QRect &rect, int &max, int &min, int &mean)
{
    int width = image->width();
    int height = image->height();
    // 判断图像是否越界
    int x = rect.x() < 0 ? 0 : rect.x();
    int y = rect.y() < 0 ? 0 : rect.y();
    int convX = rect.x() < 0 ? rect.width() + rect.x() : rect.width();
    int convY = rect.x() < 0 ? rect.height() + rect.y() : rect.height();
    min = 255;
    if((x+convX) >= width)  x=x-(x+convX-width);
    if((y+convY) >= height) y=y-(y+convY-height);
    double totalbgcolor=0;
    if(y<0 || x<0) return;
    for(int y0=y;y0<(convY+y)&&y0<height;y0++){
        uchar *line=image->scanLine(y0);
        for(int x0=x;x0<(convX+x)&&x0<width;x0++){
            totalbgcolor+=*(line+x0);
            if (*(line+x0) > max) max = *(line+x0);
            if (*(line+x0) < min) min = *(line+x0);
        }
    }
    if(convX * convY == 0) {
        mean = 0;
    } else {
        mean = totalbgcolor / (long double)(convX * convY);
    }
}

int ImageDetection::GetPixelByPoint(QImage &image, const QPoint &point)
{
    if (!image.isNull()) {
        if ((point.x() >= 0) && (point.x() <= image.width()) && (point.y() >= 0) && (point.y() <= image.height())) {
            uchar *line = image.scanLine(point.y());
            return *(line + point.x());
        } else {
            return 0;
        }
    } else {
       return 0;
    }
}

QMap<int, AssessmentData> ImageDetection::getIamgeCensusData(QImage &image, QList<QRectF> &rectList)
{
    QMap<int, AssessmentData> dataMap;
    for (int i = 0; i < rectList.size(); i++) {
        AssessmentData data;
        QRect rect;
        rect.setX(rectList[i].x());
        rect.setY(rectList[i].y());
        rect.setWidth(rectList[i].width());
        rect.setHeight(rectList[i].height());
        // 判断是否存在图像越界，完全越界之后直接给0；
        if (((rectList[i].topLeft().x() < 0) && (rectList[i].bottomRight().x() < 0)) ||
            ((rectList[i].topLeft().y() < 0) && (rectList[i].bottomRight().y() < 0)) ||
            ((rectList[i].topLeft().x() > image.width()) && (rectList[i].bottomRight().x() > image.width())) ||
            ((rectList[i].topLeft().y() > image.height()) && (rectList[i].bottomRight().y() > image.height()))) {
            data.max = data.min = data.mean = 0;
        } else {
            GetPixel(&image, rect, data.max, data.min, data.mean);
        }

        data.RectF = rectList[i];
        dataMap[i + 1] = data;
    }
    //qDebug() << tr(u8"获得的区块数据统计结果: ") << dataMap;
    return dataMap;
}

QMap<int, AssessmentData> ImageDetection::getIamgeCensusDataThread(QImage &image, QList<QRectF> &rectList)
{
    QEventLoop eventloop;
    QFutureWatcher<QMap<int, AssessmentData>> *watcher = new QFutureWatcher<QMap<int, AssessmentData>>();
    watcher->setObjectName("");
    connect(watcher, &QFutureWatcher<QMap<int, AssessmentData>>::finished, this, &ImageDetection::getIamgeCensusDataFished);
    connect(watcher, &QFutureWatcher<QMap<int, AssessmentData>>::finished, &eventloop, &QEventLoop::quit);

    QFuture<QMap<int, AssessmentData>> future = QtConcurrent::run(&getIamgeCensusData, image, rectList);
    watcher->setFuture(future);
    eventloop.exec();

    return m_statisticalData;
}

void ImageDetection::getIamgeCensusDataThread(const QString &customData, QImage &image, QList<QRectF> &rectList)
{
    QFutureWatcher<QMap<int, AssessmentData>> *watcher = new QFutureWatcher<QMap<int, AssessmentData>>();
    watcher->setObjectName(customData);
    connect(watcher, &QFutureWatcher<QMap<int, AssessmentData>>::finished, this, &ImageDetection::getIamgeCensusDataFished);
    QFuture<QMap<int, AssessmentData>> future = QtConcurrent::run(&getIamgeCensusData, image, rectList);
    watcher->setFuture(future);
}

void ImageDetection::saveImageThread(QImage &picture, QString &fileName)
{
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
    connect(watcher, &QFutureWatcher<void>::finished, watcher, &QFutureWatcher<void>::deleteLater);

    QFuture<void> future = QtConcurrent::run(&saveImage, picture, fileName);
    watcher->setFuture(future);
}

QImage ImageDetection::paintMarkPolygon(QImage &image, const QPolygonF &polygonF, float offsetX, float offsetY)
{
    QImage img = image;
    img = image.convertToFormat(QImage::Format_RGB32);
    QRect rect = GetPolygonToRect(polygonF);
    QPen pen = ColorPalette::instance().getMarkPen();
    pen .setWidthF(rect.width() / 100.0);
    QPainter painter(&img);
    painter.setPen(pen);
    painter.setBrush(QBrush(qRgba(60, 200, 60, 50), Qt::NoBrush));

    if (polygonF.size() > 2) {
        for (int i = 1; i < polygonF.size(); i++) {
            painter.drawLine(polygonF.at(i - 1) + QPointF(offsetX, offsetY), polygonF.at(i) + QPointF(offsetX, offsetY));
        }
        painter.drawLine(polygonF.at(polygonF.size() - 1) + QPointF(offsetX, offsetY), polygonF.at(0) + QPointF(offsetX, offsetY));
    }
    return img;
}

QImage ImageDetection::paintComparisonRect(QImage &image, const QList<QRectF> &rectList)
{
    QImage img = image;
    img = image.convertToFormat(QImage::Format_RGB32);
    QPen pen = ColorPalette::instance().getRectPen();
    QPainter painter(&img);
    for (int i = 0; i < rectList.size(); i++) {
        pen .setWidthF(rectList.at(i).width() * 0.05);
        painter.setPen(pen);
        painter.setBrush(QBrush(qRgba(60, 200, 60, 50), Qt::NoBrush));
        painter.drawRect(rectList.at(i));
    }
    return img;
}

QImage ImageDetection::paintComparisonResult(QImage &image, const StatisticalInfo &statisticalInfo, const AssessmentMethod &type, bool isDiff, int upperLimit, int lowerLimit)
{
    QImage img = image;
    img = image.convertToFormat(QImage::Format_RGB32);
    QPainter painter(&img);

    for (auto item = statisticalInfo.ComparisonStatistical.begin(); item != statisticalInfo.ComparisonStatistical.end(); item++) {
        QString info = "defult";
        QRectF rectF;
        QFont font;
        if (isDiff) {
            //绘制绘制差异值
            painter.setPen(ColorPalette::instance().getWithinThresholdPen());
            font = painter.font();
            font.setPointSize(item->RectF.width() * 0.2);
            int diff = 0;
            if (type == MinType) {
                diff = item->min - statisticalInfo.ModelStatistical[item.key()].min;
            } else if (type == MaxType) {
                diff = item->max - statisticalInfo.ModelStatistical[item.key()].max;
            } else {
                diff = item->mean - statisticalInfo.ModelStatistical[item.key()].mean;
            }
            if ((diff > upperLimit) || ((diff < -lowerLimit)))
            {
                painter.setPen(ColorPalette::instance().getBeyondThresholdPen());
                painter.setBrush(QBrush(qRgba(60, 200, 60, 50), Qt::NoBrush));
                font = painter.font();
                font.setPointSize(item->RectF.width() * 0.2);
            }
            info = QString::number(diff);
        } else {
            //绘制统计数据
            painter.setPen(ColorPalette::instance().getWithinThresholdPen());
            font = painter.font();
            font.setPointSize(item->RectF.width() * 0.2);
            if (type == MinType) {
                info = QString::number(item->min);
            } else if (type == MaxType) {
                info = QString::number(item->max);
            } else {
                info = QString::number(item->mean);
            }
        }
        rectF = QRectF(item->RectF.topLeft() + QPointF(3, 3), item->RectF.bottomRight());
        painter.setBrush(QBrush(qRgba(60, 200, 60, 50), Qt::NoBrush));
        painter.setFont(font);
        painter.drawText(rectF, Qt::AlignLeft|Qt::AlignTop, info);
    }
    return img;
}

Mat ImageDetection::qImageTocvMat(const QImage &image)
{
    cv::Mat mat;
    qDebug() << image.format();
    switch((int)image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

QImage ImageDetection::matToQImage(const Mat &InputMat)
{
    switch (InputMat.type())
    {
    // 8位4通道
    case CV_8UC4:
    {
        QImage image(InputMat.data, InputMat.cols, InputMat.rows, InputMat.step, QImage::Format_RGB32);
        return image;
    }
    // 8位3通道
    case CV_8UC3:
    {
        QImage image(InputMat.data, InputMat.cols, InputMat.rows, InputMat.step, QImage::Format_RGB888 );
        return image.rgbSwapped();
    }
    // 8位单通道
    case CV_8UC1:
    {
        static QVector<QRgb>  sColorTable;
        // only create our color table once
        if (sColorTable.isEmpty())
        {
            for ( int i = 0; i < 256; ++i )
                sColorTable.push_back( qRgb( i, i, i ) );
        }
        QImage image(InputMat.data, InputMat.cols, InputMat.rows, InputMat.step, QImage::Format_Indexed8 );
        image.setColorTable( sColorTable );
        return image;
    }

    default:
        qDebug("Image format is not supported: depth=%d and %d channels\n", InputMat.depth(), InputMat.channels());
        break;
    }
    return QImage();
}

void ImageDetection::templateMatch(const QImage &img, const QImage &modelImg, QRect &rect, float &similarity)
{
    cv::Mat image = qImageTocvMat(img.convertToFormat(QImage::Format_Grayscale8));
    cv::Mat templ = qImageTocvMat(modelImg.convertToFormat(QImage::Format_Grayscale8));
    int result_cols =  image.cols - templ.cols + 1;
    int result_rows = image.rows - templ.rows + 1;
    if ((result_cols < 0) || (result_rows < 0)) {
        return;
    }
    cv::Mat result;
    qDebug() << u8"模板匹配结果图像宽高：" << result_cols << result_rows << image.cols << templ.cols;

    result.create(result_cols, result_rows, CV_32FC1);
    matchTemplate(image, templ, result,  CV_TM_CCOEFF_NORMED);
//    normalize(result, result, 0, 1, NORM_MINMAX, -1, cv::Mat());
//    imshow(u8"实例图片",result);
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
    cv::Point matchLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    matchLoc.x = maxLoc.x;
    matchLoc.y = maxLoc.y;
//    cv::Rect rz;
//    rz.x=matchLoc.x,rz.y=matchLoc.y,rz.width=templ.cols,rz.height=templ.rows;
//    rectangle(image, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar(0, 255, 0), 2, 8, 0);
    rect = QRect(maxLoc.x, maxLoc.y, templ.cols, templ.rows);
    similarity = maxVal;
    qDebug() << u8"模型匹配的位置：" << rect << u8"相似度为：" << similarity;
}

void ImageDetection::templateMatch(const QImage &img, const QImage &modelImg, const QRect &inRect, QRect &rect, float &similarity)
{
    cv::Mat image = qImageTocvMat(img.convertToFormat(QImage::Format_Grayscale8));
    cv::Mat templ = qImageTocvMat(modelImg.convertToFormat(QImage::Format_Grayscale8));
    cv::Rect roi = cv::Rect(inRect.x(), inRect.y(), inRect.width(), inRect.height());

    cv::Mat roiImage = templ(roi);
    int result_cols =  image.cols - roiImage.cols + 1;
    int result_rows = image.rows - roiImage.rows + 1;
    cv::Mat result;
    qDebug() << u8"模板匹配结果图像宽高：" << result_cols << result_rows << roiImage.cols << roiImage.cols;

    result.create(result_cols, result_rows, CV_32FC1);
    matchTemplate(image, roiImage, result,  CV_TM_CCOEFF_NORMED);
//    normalize(result, result, 0, 1, NORM_MINMAX, -1, cv::Mat());
//    imshow(u8"实例图片",result);
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
    cv::Point matchLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    matchLoc.x = maxLoc.x;
    matchLoc.y = maxLoc.y;
    rect = QRect(maxLoc.x, maxLoc.y, templ.cols, templ.rows);
    similarity = maxVal;
    qDebug() << u8"模型匹配的位置：" << rect << u8"相似度为：" << similarity;
}

void ImageDetection::saveImage(QImage &picture, QString &fileName)
{
    QFileInfo fileInfo(fileName);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        dir.mkdir(dir.absolutePath());
    }

    picture.save(fileName);
}

QRect ImageDetection::GetPolygonToRect(const QPolygonF &polygonF)
{
    int x1 =-1,y1 =-1,x2 =-1,y2 =-1;
    for(int i = 0; i < polygonF.size(); i++){
        if(polygonF.at(i).x() < x1 || x1 == -1){
            x1 = polygonF.at(i).x();
        }
        if(polygonF.at(i).y() < y1 || y1 == -1){
            y1 = polygonF.at(i).y();
        }
        if(polygonF.at(i).x() > x2 || x2 == -1){
            x2 = polygonF.at(i).x();
        }
        if(polygonF.at(i).y() > y2 || y2 == -1){
            y2 = polygonF.at(i).y();
        }
    }
    return QRect(x1, y1, x2 - x1, y2 - y1);
}

QList<QRectF> ImageDetection::getOffsetRect(const QList<QRectF> &rectList, int offsetX, int offsetY)
{
    QList<QRectF> rectfList;
    for (int i = 0; i < rectList.size(); i++) {
        QRectF rect(rectList.at(i).x() + offsetX, rectList.at(i).y() + offsetY, rectList.at(i).width(), rectList.at(i).height());
        rectfList.append(rect);
    }
    return rectfList;
}
