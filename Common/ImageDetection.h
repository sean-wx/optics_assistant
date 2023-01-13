#ifndef IMAGEDETECTION_H
#define IMAGEDETECTION_H
#include <QImage>
#include <QObject>
#include <QRect>
#include "comm_def.h"
#include "opencv2/opencv.hpp"
using namespace cv;

class ImageDetection : public QObject
{
    Q_OBJECT
public:
    ImageDetection();

    // 获取图像指定区域的平均值、最大值、最小值
    static void GetPixel(QImage *image, const QRect &rect, int &max, int &min, int &mean);
    // 通过点获取图像指定位置的像素值
    int GetPixelByPoint(QImage &image, const QPoint &point);
    static QMap<int, AssessmentData> getIamgeCensusData(QImage &image, QList<QRectF> &rectList);
    void getIamgeCensusDataFished();
    QMap<int, AssessmentData> getIamgeCensusDataThread(QImage &image, QList<QRectF> &rectList);
    void getIamgeCensusDataThread(const QString &customData, QImage &image, QList<QRectF> &rectList);
    void saveImageThread(QImage &picture, QString &fileName);

    QImage paintMarkPolygon(QImage &image, const QPolygonF &polygonF, float offsetX, float offsetY);
    QImage paintComparisonRect(QImage &image, const QList<QRectF> &rectList);
    QImage paintComparisonResult(QImage &image, const StatisticalInfo &statisticalInfo, const AssessmentMethod &type, bool isDiff, int upperLimit, int lowerLimit);

    cv::Mat qImageTocvMat(const QImage &image);
    QImage matToQImage(const cv::Mat& InputMat);
    void templateMatch(const QImage &img, const QImage &modelImg, QRect &rect, float &similarity);
    void templateMatch(const QImage &img, const QImage &modelImg, const QRect &inRect, QRect &rect, float &similarity);

    QRect GetPolygonToRect(const QPolygonF &polygonF);
    QList<QRectF> getOffsetRect(const QList<QRectF> &rectList, int offsetX, int offsetY);

signals:
    void updataIamgeCensusData(const QString &customData, const QMap<int, AssessmentData> &censusData);

private:
    QMap<int, AssessmentData> m_statisticalData;
    static void saveImage(QImage &picture, QString &fileName);
};

#endif // IMAGEDETECTION_H
