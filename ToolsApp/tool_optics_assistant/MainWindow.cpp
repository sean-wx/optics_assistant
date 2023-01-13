#include <QDateTime>
#include <QMessageBox>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "CameraSelect.h"
#include "CameraConfig.h"
#include "CameraImageWindow.h"
#include "ColorPalette.h"
#include "SaveImageConfig.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    CameraSelect *cameraSelect = new CameraSelect(ui->cameraSelectwidget);
    QLayout *layout = ui->cameraSelectwidget->layout();
    layout->addWidget(cameraSelect);

    CameraConfig *cameraConfig = new CameraConfig(ui->CameraConfigWidget);
    layout = ui->CameraConfigWidget->layout();
    layout->addWidget(cameraConfig);

    m_cameraImageWindow = new CameraImageWindow(ui->ImageShowWidget);
    layout = ui->ImageShowWidget->layout();
    layout->addWidget(m_cameraImageWindow);

    m_productConfig = new ProductConfig(ui->ProductConfigWidget);
    layout = ui->ProductConfigWidget->layout();
    layout->addWidget(m_productConfig);

    QList<int> lstSize;
    lstSize << 300 << this->width() - 560 << 260;
    ui->splitter->setSizes(lstSize);

    connect(cameraSelect,        &CameraSelect::changeCamera,      m_cameraImageWindow, &CameraImageWindow::setChangeCamera, Qt::DirectConnection);
    connect(m_cameraImageWindow, &CameraImageWindow::changeCamera, cameraSelect,        &CameraSelect::changeCameraFinished, Qt::DirectConnection);
    connect(cameraSelect,        &CameraSelect::changeCamera,      cameraConfig,        &CameraConfig::setCurrentCamera, Qt::DirectConnection);
    connect(m_cameraImageWindow, &CameraImageWindow::changeCamera, cameraConfig,        &CameraConfig::setCurrentCamera, Qt::DirectConnection);
    connect(cameraSelect,        &CameraSelect::changeCamera,      m_productConfig,       &ProductConfig::setCurrentCamera, Qt::DirectConnection);
    connect(m_cameraImageWindow, &CameraImageWindow::changeCamera, m_productConfig,       &ProductConfig::setCurrentCamera, Qt::DirectConnection);

    connect(m_cameraImageWindow, &CameraImageWindow::updataCrossCursorShow, this, &MainWindow::setCrossCursorShow);
    connect(m_cameraImageWindow, &CameraImageWindow::updataRulerShow,       this, &MainWindow::setRulerShow);
    connect(m_cameraImageWindow, &CameraImageWindow::updataShowDiffData,    this, &MainWindow::setShowDiffData);
    connect(m_cameraImageWindow, &CameraImageWindow::updataMousePoint,      this, &MainWindow::setMousePoint);
    connect(m_cameraImageWindow, &CameraImageWindow::updataCrossCursor,     this, &MainWindow::setCrossCursorPoint);
    connect(m_cameraImageWindow, &CameraImageWindow::updataOffset,          this, &MainWindow::setOffset);
    connect(m_cameraImageWindow, &CameraImageWindow::ratioChangeSignal,     this, &MainWindow::setRatio);
    connect(m_cameraImageWindow, &CameraImageWindow::updataLimit,           this, &MainWindow::setLimit);
    connect(m_cameraImageWindow, &CameraImageWindow::updataRulerSpacing,    this, &MainWindow::setRulerSpacing);
    connect(m_cameraImageWindow, &CameraImageWindow::updataShowType,        this, &MainWindow::setShowType);
    connect(m_cameraImageWindow, &CameraImageWindow::selectCamera,          this, &MainWindow::openOpticsConfig);
    connect(m_cameraImageWindow, &CameraImageWindow::updataPolygonShow,     this, &MainWindow::setPolygonShow);
    connect(m_cameraImageWindow, &CameraImageWindow::updataAssessRectShow,  this, &MainWindow::setAssessRectShow);
    connect(m_cameraImageWindow, &CameraImageWindow::updataModelImageSize,  this, &MainWindow::setModelImageSize);
    connect(m_cameraImageWindow, &CameraImageWindow::updataCameraImageSize, this, &MainWindow::setCameraImageSize);

    connect(cameraConfig, &CameraConfig::updataCameraList,  cameraSelect,        &CameraSelect::updataCameraList);
    connect(cameraSelect, &CameraSelect::searchCamera,      cameraConfig,        &CameraConfig::searchCameraList);
    connect(cameraSelect, &CameraSelect::connectCamera,     cameraConfig,        &CameraConfig::connectCamera, Qt::DirectConnection);
    connect(cameraConfig, &CameraConfig::openCameraFinish,  cameraSelect,        &CameraSelect::openCameraFinished, Qt::DirectConnection);
    connect(cameraSelect, &CameraSelect::closeCamera,       cameraConfig,        &CameraConfig::closeCamera, Qt::DirectConnection);
    connect(cameraConfig, &CameraConfig::closeCameraFinish, cameraSelect,        &CameraSelect::closeCameraFinished, Qt::DirectConnection);
    connect(cameraConfig, &CameraConfig::photoReceived,     m_cameraImageWindow, &CameraImageWindow::updataCameraImage, Qt::DirectConnection);

    connect(m_productConfig, &ProductConfig::requestCameraParameter,  cameraConfig,        &CameraConfig::getCameraParameter);
    connect(cameraConfig,    &CameraConfig::updataCameraParameter,    m_productConfig,       &ProductConfig::saveCameraParameterToDb);
    connect(m_productConfig, &ProductConfig::updateModelImageFile,    m_cameraImageWindow, &CameraImageWindow::updataModelImageFile, Qt::DirectConnection);
    connect(m_productConfig, &ProductConfig::updataCameraParameter,   cameraConfig,        &CameraConfig::setCameraParameter, Qt::DirectConnection);
    connect(m_productConfig, &ProductConfig::startSaveCameraImg,      m_cameraImageWindow, &CameraImageWindow::saveCameraImg);
    connect(m_productConfig, &ProductConfig::startSaveComparisonImg,  m_cameraImageWindow, &CameraImageWindow::saveComparisonImg);
    connect(m_productConfig, &ProductConfig::startSaveStatisticalImg, m_cameraImageWindow, &CameraImageWindow::saveStatisticalImg);

    // 创建图像比对工具
    m_comparisonToolWindow = new ComparisonToolWindow(NULL);
    m_comparisonToolWindow->setWindowFlags(m_comparisonToolWindow->windowFlags() | Qt::SubWindow | Qt::WindowStaysOnTopHint);
    m_comparisonToolWindow->hide();
    connect(cameraConfig,           &CameraConfig::photoReceived,           m_comparisonToolWindow, &ComparisonToolWindow::updataCameraImage);
    connect(m_comparisonToolWindow, &ComparisonToolWindow::winddowClose,    m_cameraImageWindow,    &CameraImageWindow::setStatisticalData);
    connect(m_cameraImageWindow,    &CameraImageWindow::updataConnectData,  m_productConfig,        &ProductConfig::setConnectData);

    connect(cameraConfig,           &CameraConfig::updataInfo,          this, &MainWindow::setInfoData, Qt::DirectConnection);
    connect(m_productConfig,        &ProductConfig::updataInfo,         this, &MainWindow::setInfoData, Qt::DirectConnection);
    connect(m_cameraImageWindow,    &CameraImageWindow::updataInfo,     this, &MainWindow::setInfoData, Qt::DirectConnection);
    connect(m_comparisonToolWindow, &ComparisonToolWindow::updataInfo,  this, &MainWindow::setInfoData, Qt::DirectConnection);

    //创建批量图像对比工具
    m_batchComparison = new BatchComparison();
    m_batchComparison->setWindowFlags(m_batchComparison->windowFlags() | Qt::SubWindow | Qt::WindowStaysOnTopHint);
    m_batchComparison->hide();
}

MainWindow::~MainWindow()
{
    if (m_comparisonToolWindow != NULL) {
        m_comparisonToolWindow->hide();
        m_comparisonToolWindow->close();
        delete m_comparisonToolWindow;
    }

    if (m_batchComparison != NULL) {
        m_batchComparison->hide();
        m_batchComparison->close();
        delete m_batchComparison;
    }

    if (m_productConfig != NULL) {
        delete m_productConfig;
    }

    if (m_cameraImageWindow != NULL) {
        delete m_cameraImageWindow;
    }
    delete ui;
}

void MainWindow::setInfoData(const QString &info, bool isError)
{
    // 先保存当前的文字颜色
    auto cur_text_color = ui->systemInfo->textColor();
    // 设置当前行要使用的颜色，假设为红色
    if (isError) {
        ui->systemInfo->setTextColor(Qt::red);
    } else {
        ui->systemInfo->setTextColor(Qt::black);
    }
    // 写入一行内容
    ui->systemInfo->append(QDateTime::currentDateTime().toString("MM-dd hh:mm:ss ") + info);
    // 最后恢复原来的颜色
    ui->systemInfo->setTextColor(cur_text_color);
}

void MainWindow::setCrossCursorShow(bool isShow)
{
    ui->action_showCursor->blockSignals(true);
    ui->action_showCursor->setChecked(isShow);
    ui->action_showCursor->blockSignals(false);
}

void MainWindow::setRulerShow(bool isShow)
{
    ui->action_showRuler->blockSignals(true);
    ui->action_showRuler->setChecked(isShow);
    ui->action_showRuler->blockSignals(false);
}

void MainWindow::setShowDiffData(bool isShow)
{
    ui->action_showDiff->blockSignals(true);
    ui->action_showDiff->setChecked(isShow);
    ui->action_showDiff->blockSignals(false);
}

void MainWindow::setPolygonShow(bool isShow)
{
    ui->action_showTemBox->blockSignals(true);
    ui->action_showTemBox->setChecked(isShow);
    ui->action_showTemBox->blockSignals(false);
}

void MainWindow::setAssessRectShow(bool isShow)
{
    ui->action_showComBox->blockSignals(true);
    ui->action_showComBox->setChecked(isShow);
    ui->action_showComBox->blockSignals(false);
}

void MainWindow::setMousePoint(const QPointF &pointF, int value)
{
    ui->mouseX->setText(QString::number(int(pointF.x())));
    ui->mouseY->setText(QString::number(int(pointF.y())));
    ui->grayValue->setText(QString::number(value));
}

void MainWindow::setCrossCursorPoint(const QPointF &pointF)
{
    ui->cursorX->setValue(pointF.x());
    ui->cursorY->setValue(pointF.y());
}

void MainWindow::setOffset(const QPointF &pointF)
{
    ui->offsetX->blockSignals(true);
    ui->offsetY->blockSignals(true);
    ui->offsetX->setValue(pointF.x());
    ui->offsetY->setValue(pointF.y());
    ui->offsetX->blockSignals(false);
    ui->offsetY->blockSignals(false);
}

void MainWindow::setRatio(float ratio)
{
    ui->zoomRatio->blockSignals(true);
    ui->zoomRatio->setValue(ratio*100);
    ui->zoomRatio->blockSignals(false);
}

void MainWindow::setLimit(int lowerLimit, int upperLimit)
{
    ui->lowerLimit->blockSignals(true);
    ui->lowerLimit->setValue(lowerLimit);
    ui->lowerLimit->blockSignals(false);

    ui->upperLimit->blockSignals(true);
    ui->upperLimit->setValue(upperLimit);
    ui->upperLimit->blockSignals(false);
}

void MainWindow::setModelImageSize(int imageWidth, int imageHeight)
{
    ui->leftImgW->setText(QString::number(imageWidth));
    ui->leftImgH->setText(QString::number(imageHeight));
}

void MainWindow::setCameraImageSize(int imageWidth, int imageHeight)
{
    ui->RightImgW->setText(QString::number(imageWidth));
    ui->RightImgH->setText(QString::number(imageHeight));
}

void MainWindow::setRulerSpacing(int value)
{
    ui->rulerSpacing->blockSignals(true);
    ui->rulerSpacing->setValue(value);
    ui->rulerSpacing->blockSignals(false);
}

void MainWindow::setShowType(AssessmentMethod type)
{
    ui->action_mean->blockSignals(true);
    ui->action_min->blockSignals(true);
    ui->action_max->blockSignals(true);
    ui->algorithmType->blockSignals(true);

    ui->action_mean->setChecked(false);
    ui->action_min->setChecked(false);
    ui->action_max->setChecked(false);
    if (type == MinType) {
        ui->action_min->setChecked(true);
        ui->algorithmType->setCurrentIndex(2);
    } else if (type == MaxType) {
        ui->action_max->setChecked(true);
        ui->algorithmType->setCurrentIndex(1);
    } else if (type == MeanType) {
        ui->action_mean->setChecked(true);
        ui->algorithmType->setCurrentIndex(0);
    } else {
        qDebug() << u8"未定义显示类型";
    }
    m_cameraImageWindow->setShowType(type);

    ui->action_mean->blockSignals(false);
    ui->action_min->blockSignals(false);
    ui->action_max->blockSignals(false);
    ui->algorithmType->blockSignals(false);
}

void MainWindow::openOpticsConfig(const QString &cameraSn, const Camera_Type &cameraType, const StatisticalInfo &statisticalData)
{
    m_comparisonToolWindow->setCameraSN(cameraSn);
    m_comparisonToolWindow->setCameraType(cameraType);
    on_action_openOpticsConfig_triggered();
    m_comparisonToolWindow->setStatisticalData(statisticalData);
}

void MainWindow::on_action_openOpticsConfig_triggered()
{
    // 显示图像比对工具，并移动至中心
    QPoint point = QPoint(this->pos() + this->window()->rect().center());
    m_comparisonToolWindow->move(point - m_comparisonToolWindow->window()->rect().center());
    m_comparisonToolWindow->show();
}

void MainWindow::on_action_colorPalette_triggered()
{
    QPoint point = QPoint(this->pos() + this->window()->rect().center());
    ColorPalette::instance().move(point - ColorPalette::instance().window()->rect().center());
    ColorPalette::instance().exec();
    m_cameraImageWindow->updateColor();
    this->update();
}

void MainWindow::on_action1_x_1_triggered()
{
    m_cameraImageWindow->setItemNumber(1, 1);
}

void MainWindow::on_action2_x_2_triggered()
{
    m_cameraImageWindow->setItemNumber(2, 2);
}

void MainWindow::on_action3_x_3_triggered()
{
    m_cameraImageWindow->setItemNumber(3, 3);
}

void MainWindow::on_action4_x_4_triggered()
{
    m_cameraImageWindow->setItemNumber(4, 4);
}

void MainWindow::on_zoomRatio_editingFinished()
{
    m_cameraImageWindow->setRatio(ui->zoomRatio->value() / 100);
}

void MainWindow::on_upperLimit_editingFinished()
{
    m_cameraImageWindow->setUpperLimit(ui->upperLimit->value());
}

void MainWindow::on_lowerLimit_editingFinished()
{
    m_cameraImageWindow->setUpperLimit(ui->lowerLimit->value());
}

void MainWindow::on_action_mean_triggered()
{
    setShowType(MeanType);
}

void MainWindow::on_action_max_triggered()
{
    setShowType(MaxType);
}

void MainWindow::on_action_min_triggered()
{
    setShowType(MinType);
}

void MainWindow::on_offsetX_valueChanged(int arg1)
{
    m_cameraImageWindow->setOffsetX(arg1);
}

void MainWindow::on_offsetY_valueChanged(int arg1)
{
    m_cameraImageWindow->setOffsetY(arg1);
}

void MainWindow::on_rulerSpacing_editingFinished()
{
    m_cameraImageWindow->setRulerSpacing(ui->rulerSpacing->value());
}

void MainWindow::on_algorithmType_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if (ui->algorithmType->currentIndex() == 0) {
        setShowType(MeanType);
    } else if (ui->algorithmType->currentIndex() == 1) {
        setShowType(MaxType);
    } else {
        setShowType(MinType);
    }
}

void MainWindow::on_action_upMove_triggered()
{
    ui->offsetY->setValue(ui->offsetY->value() - 1);
}

void MainWindow::on_action_downMove_triggered()
{
    ui->offsetY->setValue(ui->offsetY->value() + 1);
}

void MainWindow::on_action_leftMove_triggered()
{
    ui->offsetX->setValue(ui->offsetX->value() - 1);
}

void MainWindow::on_action_rightMove_triggered()
{
    ui->offsetX->setValue(ui->offsetX->value() + 1);
}

void MainWindow::on_action_showCursor_triggered(bool checked)
{
    m_cameraImageWindow->setCursorShow(checked);
}

void MainWindow::on_action_showCenterCursor_triggered()
{
    m_cameraImageWindow->setCenterCursor();
}

void MainWindow::on_action_showRuler_triggered(bool checked)
{
    m_cameraImageWindow->setRulerShow(checked);
}

void MainWindow::on_action_showDiff_triggered(bool checked)
{
    m_cameraImageWindow->setShowDiff(checked);
}

void MainWindow::on_action_openModelImg_triggered()
{
    m_productConfig->getMouldImageFile();
}

void MainWindow::on_action_cameraImgSave_triggered()
{
    QString cameraSn;
    Camera_Type cameraType;
    m_cameraImageWindow->getCurrentViewInfo(cameraSn, cameraType);
    if (!cameraSn.isEmpty()) {
        m_cameraImageWindow->saveCameraImg(cameraSn, cameraType);
    }
}

void MainWindow::on_action_saveStatisticalImg_triggered()
{
    QString cameraSn;
    Camera_Type cameraType;
    m_cameraImageWindow->getCurrentViewInfo(cameraSn, cameraType);
    if (!cameraSn.isEmpty()) {
        m_cameraImageWindow->saveStatisticalImg(cameraSn, cameraType);
    }
}

void MainWindow::on_action_saveComparisonImg_triggered()
{
    QString cameraSn;
    Camera_Type cameraType;
    m_cameraImageWindow->getCurrentViewInfo(cameraSn, cameraType);
    if (!cameraSn.isEmpty()) {
        m_cameraImageWindow->saveComparisonImg(cameraSn, cameraType);
    }
}

void MainWindow::on_action_automaticMatching_triggered()
{
    m_cameraImageWindow->setAutomaticMatching();
}

void MainWindow::on_action_imageSaveConfig_triggered()
{
    QPoint point = QPoint(this->pos() + this->window()->rect().center());
    SaveImageConfig::instance().move(point - SaveImageConfig::instance().window()->rect().center());
    SaveImageConfig::instance().exec();
    this->update();
}


void MainWindow::on_action_showTemBox_triggered(bool checked)
{
    m_cameraImageWindow->setPolygonShow(checked);
}

void MainWindow::on_action_showComBox_triggered(bool checked)
{
    m_cameraImageWindow->setAssessRectShow(checked);
}

void MainWindow::on_action_openBatchComparison_triggered()
{
    // 显示图像比对工具，并移动至中心
    QPoint point = QPoint(this->pos() + this->window()->rect().center());
    m_batchComparison->move(point - m_batchComparison->window()->rect().center());
    m_batchComparison->show();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,tr("光学助手 V1.0.1"),
                           tr("<h3>光学助手 V1.0.1</h3>"
                              "<p> Based on Qt 5.9.3"));
}
