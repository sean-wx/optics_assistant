#include <QMessageBox>
#include <QFileDialog>
#include "ProductConfig.h"
#include "ui_ProductConfig.h"
#include "AbstractDbClient.h"

ProductConfig::ProductConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductConfig)
{
    ui->setupUi(this);
}

ProductConfig::~ProductConfig()
{
    delete ui;
}

ConnectData ProductConfig::getConnectData()
{
    ConnectData data;
    data.CameraId = ui->productNumber->value();
    data.CameraId = ui->cameraNumber->value();
    data.PhotoId = ui->imageNumber->value();
    data.ModelImageFile = ui->mouldImageFilePath->text();
    data.ModelXmlDataFile = ui->comparisonRegionData->text();
    return data;
}

void ProductConfig::setCurrentCamera(const QString &cameraSn, const Camera_Type &type)
{
    //先保存参数
    m_connectDataList[m_cameratype][m_cameraSn].ProductId = ui->productNumber->value();
    m_connectDataList[m_cameratype][m_cameraSn].CameraId = ui->cameraNumber->value();
    m_connectDataList[m_cameratype][m_cameraSn].PhotoId = ui->imageNumber->value();
    m_connectDataList[m_cameratype][m_cameraSn].ModelImageFile = ui->mouldImageFilePath->text();
    m_connectDataList[m_cameratype][m_cameraSn].ModelXmlDataFile = ui->comparisonRegionData->text();

    //读取并显示新参数
    m_cameraSn = cameraSn;
    m_cameratype = type;
    ui->productNumber->setValue(m_connectDataList[type][cameraSn].ProductId);
    ui->cameraNumber->setValue(m_connectDataList[type][cameraSn].CameraId);
    ui->imageNumber->setValue(m_connectDataList[type][cameraSn].PhotoId);
    ui->mouldImageFilePath->setText(m_connectDataList[type][cameraSn].ModelImageFile);
    ui->comparisonRegionData->setText(m_connectDataList[type][cameraSn].ModelXmlDataFile);
}

void ProductConfig::saveCameraParameterToDb(const CameraParameter &parameter)
{
    int cameraNumber = ui->cameraNumber->value();
    int imageNumber = ui->imageNumber->value();
    int productNumber = ui->productNumber->value();

    QMessageBox box(QMessageBox::Warning, QStringLiteral("确认操作"), QStringLiteral("确认保存数据库?"));
    box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    box.setButtonText(QMessageBox::Ok, QStringLiteral("确定"));
    box.setButtonText(QMessageBox::Cancel, QStringLiteral("取消"));
    if (box.exec() == QMessageBox::Ok) {
        qDebug()  << "正在保存到数据库中的数据: " << productNumber << cameraNumber << imageNumber << parameter;
        AbstractDbClient db;
        bool ret = db.updateProfileCameraPhoto(productNumber,
                                               cameraNumber,
                                               imageNumber,
                                               parameter.Gain,
                                               parameter.Gamma,
                                               parameter.ExposureTime,
                                               parameter.PreampGain,
                                               parameter.LineRate);
        if (ui->roiIsEnable->currentText() == u8"是") {
            ret &= db.updateProfileCameraPhoto(productNumber,
                                               cameraNumber,
                                               imageNumber,
                                               parameter.Roi_X,
                                               parameter.Roi_Y,
                                               parameter.Roi_W,
                                               parameter.Roi_H);
        }
        if (ret) {
            emit updataInfo(QStringLiteral("相机参数保存至数据库成功!"), false);
        } else {
            emit updataInfo(QStringLiteral("相机参数保存至数据库失败，请检查条件？"), true);
        }
    }
}

void ProductConfig::getMouldImageFile()
{
    on_getMouldImageFile_clicked();
}

void ProductConfig::setConnectData(const QString &cameraSn, const Camera_Type &type, const ConnectData &connectData)
{
    m_connectDataList[type][cameraSn].ModelImageFile = connectData.ModelImageFile;
    ui->mouldImageFilePath->setText(connectData.ModelImageFile);
    if (QFileInfo(connectData.ModelXmlDataFile).exists()) {
        m_connectDataList[type][cameraSn].ModelXmlDataFile = connectData.ModelXmlDataFile;
        ui->comparisonRegionData->setText(connectData.ModelXmlDataFile);
    }
}

void ProductConfig::on_loadingData_clicked()
{
    AbstractDbClient db;
    int cameraNumber = ui->cameraNumber->value();
    int imageNumber = ui->imageNumber->value();
    int productNumber = ui->productNumber->value();
    AbstractDbClient::ProfileCameraPhoto profileCameraPhoto = db.getProfileCameraPhoto(productNumber, cameraNumber, imageNumber);
    CameraParameter parameter;
    if (profileCameraPhoto.cameraId != -1) {
        parameter.Roi_X = profileCameraPhoto.offsetX;
        parameter.Roi_Y = profileCameraPhoto.offsetY;
        parameter.Roi_W = profileCameraPhoto.width;
        parameter.Roi_H = profileCameraPhoto.height;
        parameter.ExposureTime = profileCameraPhoto.exposureTime;
        parameter.Gamma = profileCameraPhoto.gamma;
        parameter.Gain = profileCameraPhoto.gain;
        parameter.LineRate = profileCameraPhoto.lineRate;
        parameter.PreampGain = profileCameraPhoto.preampGain;
        parameter.TriggerMode = true;
        parameter.TriggerSource = 1;
        qDebug()  << "数据库读取到的数据: " << parameter;

        emit updataCameraParameter(parameter, ui->roiIsEnable->currentText() == u8"是" ? true : false);
        emit updataInfo(QStringLiteral("加载相机配置参数成功。"), false);
    } else {
        emit updataCameraParameter(parameter, ui->roiIsEnable->currentText() == u8"是" ? true : false);
        emit updataInfo(QStringLiteral("加载相机配置参数失败，请检查条件？"), true);
    }
}

void ProductConfig::on_saveData_clicked()
{
    emit requestCameraParameter();
}

void ProductConfig::on_getMouldImageFile_clicked()
{
    QString six      = QString("*.bmp *.jpg *.png *.tiff *.tif");
    QString title    = QStringLiteral("模板图像");
    QString type     = QStringLiteral("图像文件(%1)").arg(six);
    QString dirName  = m_connectDataList[m_cameratype][m_cameraSn].ModelImageFile;
    if (dirName.isEmpty()) {
        dirName  = QCoreApplication::applicationDirPath();
    }

    QString mouldImageFile = QFileDialog::getOpenFileName(this, title, dirName, type);
    if (mouldImageFile.isEmpty()) {
        return;
    }
    m_mouldImageFile = mouldImageFile;
    ui->mouldImageFilePath->setText(mouldImageFile);
    emit updataInfo(QStringLiteral("开始导入模板文件......"), false);

    QString modelDataFile = mouldImageFile.left(mouldImageFile.lastIndexOf(".") + 1).append("xml");
    if (QFileInfo(modelDataFile).exists()) {
        ui->comparisonRegionData->setText(modelDataFile);
    } else {
        emit updataInfo(QString("模板XML数据（%1）不存在！").arg(modelDataFile), true);
        ui->comparisonRegionData->clear();
    }

    emit updateModelImageFile(m_cameraSn, m_cameratype, getConnectData());
}

void ProductConfig::on_saveStatisticalImg_clicked()
{
    if (!m_cameraSn.isEmpty()) {
        emit startSaveStatisticalImg(m_cameraSn, m_cameratype);
    }
}

void ProductConfig::on_saveComparisonImg_clicked()
{
    if (!m_cameraSn.isEmpty()) {
        emit startSaveComparisonImg(m_cameraSn, m_cameratype);
    }
}

void ProductConfig::on_saveImageBtn_clicked()
{
    if (!m_cameraSn.isEmpty()) {
        emit startSaveCameraImg(m_cameraSn, m_cameratype);
    }
}
