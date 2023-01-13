#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QTextCodec>
#include "SaveImageConfig.h"
#include "ui_SaveImageConfig.h"

SaveImageConfig &SaveImageConfig::instance()
{
    static SaveImageConfig saveImageConfig;
    return saveImageConfig;
}

void SaveImageConfig::setCameraSaveConfig(const SaveIamgeInfo &info)
{
    m_cameraSaveConfig = info;
    showParameters();
}

SaveIamgeInfo SaveImageConfig::cameraSaveConfig()
{
    m_cameraSaveConfig.FileName = ui->cameraName->text();
    return m_cameraSaveConfig;
}

void SaveImageConfig::setStatisticsSaveConfig(const SaveIamgeInfo &info)
{
    m_statisticsSaveConfig = info;
    showParameters();
}

SaveIamgeInfo SaveImageConfig::statisticsSaveConfig()
{
    m_statisticsSaveConfig.FileName = ui->statisticsName->text();
    return m_statisticsSaveConfig;
}

void SaveImageConfig::setComparisonSaveConfig(const SaveIamgeInfo &info)
{
    m_comparisonSaveConfig = info;
    showParameters();
}

SaveIamgeInfo SaveImageConfig::comparisonSaveConfig()
{
    m_comparisonSaveConfig.FileName = ui->comparisonName->text();
    return m_comparisonSaveConfig;
}

void SaveImageConfig::on_Save_clicked()
{
    saveConfig();
    QDialog::close();
}

void SaveImageConfig::on_close_clicked()
{
    QDialog::close();
}

void SaveImageConfig::on_reset_clicked()
{
    m_cameraSaveConfig.SaveDir  = "/tmp/optics_align_save_image/";
    m_cameraSaveConfig.FileName = "camera_original_";
    m_cameraSaveConfig.SaveSuffix = "jpg";

    m_statisticsSaveConfig.SaveDir  = "/tmp/optics_align_save_image/";
    m_statisticsSaveConfig.FileName = "statistical_";
    m_statisticsSaveConfig.SaveSuffix = "jpg";

    m_comparisonSaveConfig.SaveDir  = "/tmp/optics_align_save_image/";
    m_comparisonSaveConfig.FileName = "comparison_";
    m_comparisonSaveConfig.SaveSuffix = "jpg";

    showParameters();
}

SaveImageConfig::SaveImageConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveImageConfig)
{
    ui->setupUi(this);

    setMaximumSize(QSize(390,310));
    setMinimumSize(QSize(390,310));
    setWindowFlags(windowFlags()& ~Qt::WindowMinMaxButtonsHint);
    setWindowFlags(windowFlags()& Qt::WindowStaysOnTopHint);

    on_reset_clicked();

    loadConfig();
    showParameters();
}

SaveImageConfig::~SaveImageConfig()
{
    delete ui;
}

void SaveImageConfig::loadConfig()
{
    QSettings settings("imageSaveConfig.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF8"));
    if (settings.value("camera_fileName").isNull()) {
        on_reset_clicked();
        saveConfig();
    } else {
        m_cameraSaveConfig.FileName = settings.value("camera_fileName").toString();
        m_cameraSaveConfig.SaveDir = settings.value("camera_saveDir").toString();
        m_cameraSaveConfig.SaveSuffix = settings.value("camera_saveSuffix").toString();

        m_statisticsSaveConfig.FileName = settings.value("statistics_fileName").toString();
        m_statisticsSaveConfig.SaveDir = settings.value("statistics_saveDir").toString();
        m_statisticsSaveConfig.SaveSuffix = settings.value("statistics_saveSuffix").toString();

        m_comparisonSaveConfig.FileName = settings.value("comparison_fileName").toString();
        m_comparisonSaveConfig.SaveDir = settings.value("comparison_saveDir").toString();
        m_comparisonSaveConfig.SaveSuffix = settings.value("comparison_saveSuffix").toString();
    }
}

void SaveImageConfig::saveConfig()
{
    QSettings settings("imageSaveConfig.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF8"));

    m_cameraSaveConfig.FileName = ui->cameraName->text();
    settings.setValue("camera_fileName", m_cameraSaveConfig.FileName);
    settings.setValue("camera_saveDir", m_cameraSaveConfig.SaveDir);
    settings.setValue("camera_saveSuffix", m_cameraSaveConfig.SaveSuffix);

    m_statisticsSaveConfig.FileName = ui->statisticsName->text();
    settings.setValue("statistics_fileName", m_statisticsSaveConfig.FileName);
    settings.setValue("statistics_saveDir", m_statisticsSaveConfig.SaveDir);
    settings.setValue("statistics_saveSuffix", m_statisticsSaveConfig.SaveSuffix);

    m_comparisonSaveConfig.FileName = ui->comparisonName->text();
    settings.setValue("comparison_fileName", m_comparisonSaveConfig.FileName);
    settings.setValue("comparison_saveDir", m_comparisonSaveConfig.SaveDir);
    settings.setValue("comparison_saveSuffix", m_comparisonSaveConfig.SaveSuffix);
}

void SaveImageConfig::showParameters()
{
    ui->cameraDir->setText(m_cameraSaveConfig.SaveDir);
    ui->cameraName->setText(m_cameraSaveConfig.FileName);
    ui->cameraSuffix->setCurrentText(m_cameraSaveConfig.SaveSuffix);

    ui->statisticsDir->setText(m_statisticsSaveConfig.SaveDir);
    ui->statisticsName->setText(m_statisticsSaveConfig.FileName);
    ui->statisticsSuffix->setCurrentText(m_statisticsSaveConfig.SaveSuffix);

    ui->comparisonDir->setText(m_comparisonSaveConfig.SaveDir);
    ui->comparisonName->setText(m_comparisonSaveConfig.FileName);
    ui->comparisonSuffix->setCurrentText(m_comparisonSaveConfig.SaveSuffix);
}

void SaveImageConfig::on_selectCameraDir_clicked()
{
    QString title    = QStringLiteral("选择相机图像存储目录");
    QString dirName  = m_cameraSaveConfig.SaveDir;//QCoreApplication::applicationDirPath();

    dirName = QFileDialog::getExistingDirectory(this, title, dirName);
    if (dirName.isEmpty()) {
        return;
    }
    ui->cameraDir->setText(dirName);
    m_cameraSaveConfig.SaveDir = dirName;
    qDebug() << u8"选择相机图像存储目录:" << dirName;
}

void SaveImageConfig::on_selectStatisticsDir_clicked()
{
    QString title    = QStringLiteral("选择统计图像存储目录");
    QString dirName  = m_statisticsSaveConfig.SaveDir;

    dirName = QFileDialog::getExistingDirectory(this, title, dirName);
    if (dirName.isEmpty()) {
        return;
    }
    ui->statisticsDir->setText(dirName);
    m_statisticsSaveConfig.SaveDir = dirName;
    qDebug() << u8"选择统计图像存储目录:" << dirName;
}

void SaveImageConfig::on_selectComparisonDir_clicked()
{
    QString title    = QStringLiteral("选择统计图像存储目录");
    QString dirName  = m_comparisonSaveConfig.SaveDir;

    dirName = QFileDialog::getExistingDirectory(this, title, dirName);
    if (dirName.isEmpty()) {
        return;
    }
    ui->comparisonDir->setText(dirName);
    m_comparisonSaveConfig.SaveDir = dirName;
    qDebug() << u8"选择统计图像存储目录:" << dirName;
}

void SaveImageConfig::on_cameraSuffix_currentIndexChanged(const QString &arg1)
{
    m_cameraSaveConfig.SaveSuffix = arg1;
}

void SaveImageConfig::on_statisticsSuffix_currentIndexChanged(const QString &arg1)
{
    m_statisticsSaveConfig.SaveSuffix = arg1;
}

void SaveImageConfig::on_comparisonSuffix_currentIndexChanged(const QString &arg1)
{
    m_comparisonSaveConfig.SaveSuffix = arg1;
}
