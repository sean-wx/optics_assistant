#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ComparisonToolWindow.h"
#include "CameraImageWindow.h"
#include "CameraSelect.h"
#include "ProductConfig.h"
#include "BatchComparison.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setInfoData(const QString &info, bool isError);

private slots:
    void setCrossCursorShow(bool isShow);
    void setRulerShow(bool isShow);
    void setShowDiffData(bool isShow);
    void setPolygonShow(bool isShow);
    void setAssessRectShow(bool isShow);
    void setMousePoint(const QPointF &pointF, int value);
    void setCrossCursorPoint(const QPointF &pointF);
    void setOffset(const QPointF &pointF);
    void setRatio(float ratio);
    void setLimit(int lowerLimit, int upperLimit);
    void setModelImageSize(int imageWidth, int imageHeight);
    void setCameraImageSize(int imageWidth, int imageHeight);
    void setRulerSpacing(int value);
    void setShowType(AssessmentMethod type);
    void openOpticsConfig(const QString &cameraSn, const Camera_Type &cameraType, const StatisticalInfo &statisticalData);

    void on_action_openOpticsConfig_triggered();
    void on_action_colorPalette_triggered();
    void on_action1_x_1_triggered();
    void on_action2_x_2_triggered();
    void on_action3_x_3_triggered();
    void on_action4_x_4_triggered();
    void on_zoomRatio_editingFinished();
    void on_upperLimit_editingFinished();
    void on_lowerLimit_editingFinished();
    void on_action_mean_triggered();
    void on_action_max_triggered();
    void on_action_min_triggered();
    void on_offsetX_valueChanged(int arg1);
    void on_offsetY_valueChanged(int arg1);
    void on_rulerSpacing_editingFinished();
    void on_algorithmType_currentIndexChanged(int index);
    void on_action_upMove_triggered();
    void on_action_downMove_triggered();
    void on_action_leftMove_triggered();
    void on_action_rightMove_triggered();
    void on_action_showCursor_triggered(bool checked);
    void on_action_showCenterCursor_triggered();
    void on_action_showRuler_triggered(bool checked);
    void on_action_showDiff_triggered(bool checked);
    void on_action_openModelImg_triggered();
    void on_action_cameraImgSave_triggered();
    void on_action_saveStatisticalImg_triggered();
    void on_action_saveComparisonImg_triggered();
    void on_action_automaticMatching_triggered();
    void on_action_imageSaveConfig_triggered();
    void on_action_showTemBox_triggered(bool checked);
    void on_action_showComBox_triggered(bool checked);
    void on_action_openBatchComparison_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    CameraImageWindow       *m_cameraImageWindow       = NULL;
    ProductConfig           *m_productConfig           = NULL;
    ComparisonToolWindow    *m_comparisonToolWindow    = NULL;
    BatchComparison         *m_batchComparison         = NULL;
};

#endif // MAINWINDOW_H
