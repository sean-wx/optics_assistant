#include <QFutureWatcher>
#include <QtConcurrent>
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include "BatchComparison.h"
#include "ui_BatchComparison.h"
#include "ColorPalette.h"

BatchComparison::BatchComparison(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BatchComparison)
{
    ui->setupUi(this);
    m_threadPool.setMaxThreadCount(QThreadPool::globalInstance()->maxThreadCount());
}

BatchComparison::~BatchComparison()
{
    m_threadPool.clear();
    delete ui;
}

void BatchComparison::on_getMouldImageFile_clicked()
{
    QString title    = QStringLiteral("选择模板图像");
    QString dirName  = m_mouldImagePath;
    if (dirName.isEmpty()) {
        dirName  = QCoreApplication::applicationDirPath();
    }
    QString filePath = QFileDialog::getExistingDirectory(this, title, dirName);
    if (filePath.isEmpty()) {
        return;
    }
    m_mouldImagePath = filePath;

    ui->mouldFileinfo->clear();
    ui->mouldImageFilePath->setText(m_mouldImagePath);
    getFileInfoThread(m_mouldImagePath, true);
}

void BatchComparison::on_getComparisonFile_clicked()
{
    QString title    = QStringLiteral("选择比对图像");
    QString dirName  = m_comparisonImagePath;
    if (dirName.isEmpty()) {
        dirName  = QCoreApplication::applicationDirPath();
    }

    QString filePath = QFileDialog::getExistingDirectory(this, title, dirName);
    if (filePath.isEmpty()) {
        return;
    }
    m_comparisonImagePath = filePath;

    ui->comparisonFileinfo->clear();
    ui->comparisonFilePath->setText(m_comparisonImagePath);
    getFileInfoThread(m_comparisonImagePath, false);
}

QMap<int, BatchComparison::ImageInfo> BatchComparison::getFileInfo(const QString &filePath, bool isReadImage)
{
    QMap<int, BatchComparison::ImageInfo> imgeInfoMap;
    if (filePath.isEmpty()) return imgeInfoMap;

    // 获取所有文件名
    QDir dir(filePath);
    if (!dir.exists())
        return imgeInfoMap;

    QStringList imgNames;
    if (!dir.exists()) {
        imgNames = QStringList("");
    }

    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);
    dir.setNameFilters(QString("*.bmp;*.jpg;*.png;*.tiff;*.tif").split(";"));
    imgNames = dir.entryList();

    for (int i = 0; i < imgNames.size(); ++i) {
        ImageInfo imageInfo;
        QString picName = imgNames[i];
        imageInfo.fileName = picName;
        if (isReadImage) {
            QImage image(filePath + "/" +picName);
            imageInfo.imageWidth = image.width();
            imageInfo.imageHeight = image.height();
        }
        //图像文件名：0512-0016-34.bmp
        int firstIndex = picName.lastIndexOf("-");
        int secondIndex = picName.lastIndexOf(".");
        imageInfo.imageNumber = picName.mid(firstIndex + 1, secondIndex - firstIndex - 1).toInt();
        imgeInfoMap.insert(imageInfo.imageNumber, imageInfo);
        qDebug() << u8"图像文件：" << imageInfo.imageNumber << picName << imageInfo.imageWidth << imageInfo.imageHeight;
    }
    return imgeInfoMap;
}

void BatchComparison::getFileInfoThread(const QString &filePath, bool isMould)
{
    QFutureWatcher<QMap<int, BatchComparison::ImageInfo>> *watcher = new QFutureWatcher<QMap<int, BatchComparison::ImageInfo>>();
    watcher->setObjectName(isMould ? "mould" : "comparison");
    connect(watcher, &QFutureWatcher<QMap<int, BatchComparison::ImageInfo>>::finished, this, &BatchComparison::getFileInfoFished);
    QFuture<QMap<int, BatchComparison::ImageInfo>> future = QtConcurrent::run(&getFileInfo, filePath, true);
    watcher->setFuture(future);
}

void BatchComparison::getFileInfoFished()
{
    QFutureWatcher<QMap<int, BatchComparison::ImageInfo>> *watcher = dynamic_cast<QFutureWatcher<QMap<int, BatchComparison::ImageInfo>> *>(sender());
    if (watcher->objectName() == "mould") {
        m_mouldImgeInfoMap = watcher->future().result();
        setFileInfo(ui->mouldFileinfo, m_mouldImgeInfoMap);
    } else if (watcher->objectName() == "comparison") {
        m_comparisonImageInfoMap = watcher->future().result();
        setFileInfo(ui->comparisonFileinfo, m_comparisonImageInfoMap);
        ui->imageNumber->setValue(1);
        ui->imageFileName->setText("");
        if ((!m_comparisonImageInfoMap.isEmpty()) && (isExistByImageNumber(m_comparisonImageInfoMap, 1))) {
            ui->imageFileName->setText(m_comparisonImageInfoMap[1].fileName);
        }
    }
    watcher->deleteLater();
}

void BatchComparison::setProgressBar(float min, float max, float currentValue)
{
    ui->progressBar->setMinimum(min);
    ui->progressBar->setMaximum(max);
    ui->progressBar->setValue(currentValue);
}

void BatchComparison::setFileInfo(QTextEdit *textEdit, const QMap<int, BatchComparison::ImageInfo> &fileInfoMap)
{
    textEdit->clear();
    for(auto it:fileInfoMap) {
//       textEdit->append(QString(u8"图号：%1，文件名：%2，宽度：%3，高度：%4。").arg(it.imageNumber).arg(it.fileName).arg(it.imageWidth).arg(it.imageHeight));
       textEdit->append(QString(u8"ID:%1 %2 %3*%4").arg(it.imageNumber).arg(it.fileName).arg(it.imageWidth).arg(it.imageHeight));
    }
}

void BatchComparison::setSaveFileinfo(const QString &info, bool isError)
{
    // 先保存当前的文字颜色
    auto cur_text_color = ui->saveFileinfo->textColor();
    // 设置当前行要使用的颜色，假设为红色
    if (isError) {
        ui->saveFileinfo->setTextColor(Qt::red);
    } else {
        ui->saveFileinfo->setTextColor(Qt::black);
    }
    // 写入一行内容
    ui->saveFileinfo->append(info);
    // 最后恢复原来的颜色
    ui->saveFileinfo->setTextColor(cur_text_color);
}

bool BatchComparison::isExistByImageNumber(const QMap<int, BatchComparison::ImageInfo> &infoMap, int imageNumber)
{
    QList<int> keys = infoMap.keys();
    return infoMap.keys().contains(imageNumber);
}

void BatchComparison::startComparisonThread(int imageNumber, const QString &mouldImage, const QString &comparisonImage)
{
    m_thread = new ComparisonThread();
    connect(m_thread, &ComparisonThread::finished, this, &BatchComparison::comparisonThreadfinished);
    connect(m_thread, &ComparisonThread::updataSaveFileinfo, this, &BatchComparison::setSaveFileinfo);
    m_thread->setImageNumber(imageNumber);
    m_thread->setMouldImagePath(mouldImage);
    m_thread->setComparisonImagePath(comparisonImage);
    m_threadPool.start(m_thread);
}

void BatchComparison::startComparisonById(int imageId)
{
    if (isExistByImageNumber(m_comparisonImageInfoMap, imageId)) {
        if (isExistByImageNumber(m_mouldImgeInfoMap, imageId)) {
            QString mouldImagePath = m_mouldImagePath + "/" + m_mouldImgeInfoMap[imageId].fileName;
            QString comparisonImagePath = m_comparisonImagePath + "/" + m_comparisonImageInfoMap[imageId].fileName;
            startComparisonThread(imageId, mouldImagePath, comparisonImagePath);
        } else {
            QString info = QString(u8"ID:%1 %2 %3*%4 error:%5")
                    .arg(m_comparisonImageInfoMap[imageId].imageNumber)
                    .arg(m_comparisonImageInfoMap[imageId].fileName)
                    .arg(m_comparisonImageInfoMap[imageId].imageWidth)
                    .arg(m_comparisonImageInfoMap[imageId].imageHeight)
                    .arg(u8"无模板图像！");
            setSaveFileinfo(info, true);
            qDebug() << u8"无模板图像！" << info;
        }
    } else {
        QString info = QString(u8"ID:%1 %2 %3*%4 error:%5")
                .arg(imageId)
                .arg("fileName")
                .arg(0)
                .arg(0)
                .arg(u8"无此对比文件");
        setSaveFileinfo(info, true);
        qDebug() << u8"无此对比文件" << info;
    }
}

void BatchComparison::comparisonThreadfinished(int imageNumber, const QString &path)
{
    Q_UNUSED(imageNumber);
    Q_UNUSED(path);
    m_finishedNumber++;
    if (m_finishedNumber == m_batchComparisonNumber) {
        setProgressBar(0, m_batchComparisonNumber, m_finishedNumber);
    }
}

void BatchComparison::on_colorPalette_clicked()
{
    QPoint point = QPoint(this->pos() + this->window()->rect().center());
    ColorPalette::instance().move(point - ColorPalette::instance().window()->rect().center());
    ColorPalette::instance().exec();
}

void BatchComparison::on_imageNumber_valueChanged(int arg1)
{
    ui->imageFileName->setText(u8"无相关文件！");
    if ((!m_comparisonImageInfoMap.isEmpty()) && (isExistByImageNumber(m_comparisonImageInfoMap, arg1))) {
        ui->imageFileName->setText(m_comparisonImageInfoMap[arg1].fileName);
    }
}

void BatchComparison::on_exportImage_clicked()
{
    m_finishedNumber = 0;
    m_batchComparisonNumber = 1;
    setProgressBar(0, m_batchComparisonNumber, 0);
    ui->saveFileinfo->clear();
    startComparisonById(ui->imageNumber->value());
}

void BatchComparison::on_previous_clicked()
{
    int oldNumber =  ui->imageNumber->value();
    int number = ui->imageNumber->value() - 1;
    if (number < 1) { number = 1; }
    ui->imageNumber->setValue(number);
    if (number != oldNumber) {
        on_exportImage_clicked();
    }
}

void BatchComparison::on_next_clicked()
{
    int oldNumber =  ui->imageNumber->value();
    int number = ui->imageNumber->value() + 1;
    ui->imageNumber->setValue(number);
    if (number != oldNumber) {
        on_exportImage_clicked();
    }
}

void BatchComparison::on_exportAllImage_clicked()
{
    m_finishedNumber = 0;
    m_batchComparisonNumber = m_comparisonImageInfoMap.size();
    setProgressBar(0, m_comparisonImageInfoMap.size(), 0);

    ui->saveFileinfo->clear();
    for (auto it = m_comparisonImageInfoMap.begin(); it != m_comparisonImageInfoMap.end(); it++) {
        startComparisonById(it.key());
    }
}

void BatchComparison::on_aboutBtn_clicked()
{
    QMessageBox::about(this,tr("批量比对工具"),
                           tr("<h3>批量比对工具</h3>"
                              "<p> Based on Qt 5.9.3"));
}
