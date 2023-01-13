#ifndef CAMERAIMAGEWINDOW_H
#define CAMERAIMAGEWINDOW_H

#include <QWidget>
#include <QResizeEvent>
#include <QMutex>
#include "ImageView.h"
#include "ImageDetection.h"

namespace Ui {
class CameraImageWindow;
}

class CameraImageWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CameraImageWindow(QWidget *parent = 0);
    ~CameraImageWindow();

    void setItemNumber(int row, int col);
    void setShowType(AssessmentMethod type);
    void setShowDiff(bool value);
    void setRulerShow(bool value);
    void setCursorShow(bool value);
    void setCenterCursor();
    void setLowerLimit(int value);
    void setUpperLimit(int value);
    void setOffsetX(int value);
    void setOffsetY(int value);
    void setRulerSpacing(int value);
    void setRatio(float ratio);
    void getCurrentViewInfo(QString &cameraSn, Camera_Type &cameraType);
    void setAutomaticMatching();
    void setPolygonShow(bool isShow);
    void setAssessRectShow(bool isShow);
    void updateColor();

signals:
    void changeCamera(const QString &cameraSn, const Camera_Type &cameraType);
    void selectCamera(const QString &cameraSn, const Camera_Type &cameraType, const StatisticalInfo &statisticalData);
    void updataCrossCursorShow(bool isShow);
    void updataRulerShow(bool isShow);
    void updataRulerSpacing(int value);
    void updataPolygonShow(bool isShow);
    void updataAssessRectShow(bool isShow);
    void updataShowType(const AssessmentMethod &type);
    void updataShowDiffData(bool isShow);
    void updataModelImageSize(int imgWidth, int imgHeight);
    void updataCameraImageSize(int imgWidth, int imgHeight);
    void updataMousePoint(const QPointF &pointF, int value);
    void updataLimit(int lowerLimit, int upperLimit);
    void updataCrossCursor(const QPointF &pointF);
    void updataOffset(const QPointF &pointF);
    void ratioChangeSignal(float ratio);
    void updataInfo(const QString &info, bool isError);
    void updataConnectData(const QString &cameraSn, const Camera_Type &type, const ConnectData &connectData);

public slots:
    void setChangeCamera(const QString &cameraSn,const Camera_Type &cameraType);
    void updataMapping(const QString &cameraSn,const Camera_Type &cameraType);
    void updataModelImageFile(const QString &cameraSn, const Camera_Type &type, const ConnectData &connectData);
    void updataCameraImage(const QString &cameraSn, const Camera_Type &type, const QImage &photo, int copyCost);
    void statisticsData(const QList<QRectF> &rectList);
    void setStatisticalData(const QString &cameraSn, const Camera_Type &type, const StatisticalInfo &statisticalData);
    void saveCameraImg(const QString &cameraSn, const Camera_Type &cameraType);
    void saveStatisticalImg(const QString &cameraSn, const Camera_Type &cameraType);
    void saveComparisonImg(const QString &cameraSn, const Camera_Type &cameraType);
    void receiveIamgeCensusData(const QString &customData, const QMap<int, AssessmentData> &censusData);\
    void receiveMousePoint(const QPointF &pointF);

private:
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    void initItem(int row, int col);
    ImageView* getImageView(const QString &cameraSn,const Camera_Type &cameraType);
    void setCameraViewFocus(ImageView *view);
    void updataUIData(ImageView *view);
    void deleteAllView();
    void setMouleImage(const QString &cameraSn,const Camera_Type &cameraType);
    QString getStrByCameraSnAndType(const QString &cameraSn,const Camera_Type &cameraType);
    void getCameraInfoByStr(const QString &cameraInfo, QString &cameraSn, Camera_Type &cameraType);
    void printMessageBoxInfo(const QString &info);

    Ui::CameraImageWindow *ui;
    int m_itemRow = 1;
    int m_itemCol = 1;
    int m_horizontalSpacing = 2;
    int m_verticalSpacing   = 2;

    QMap<int, ImageView*>   m_imageView;
    ImageDetection          m_imageDetection;
    QMutex                  m_imageMutex;
    ImageView              *m_currentView = NULL;
    QMap<Camera_Type, QMap<QString, StatisticalInfo>> m_statisticalData;
};

#endif // CAMERAIMAGEWINDOW_H
