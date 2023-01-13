#ifndef BATCHCOMPARISON_H
#define BATCHCOMPARISON_H

#include <QMainWindow>
#include <QMap>
#include <QThreadPool>
#include <QTextEdit>
#include "ComparisonThread.h"

namespace Ui {
class BatchComparison;
}

class BatchComparison : public QMainWindow
{
    Q_OBJECT

public:
    explicit BatchComparison(QWidget *parent = 0);
    ~BatchComparison();

    struct ImageInfo
    {
        QString fileName    = "";
        int     imageNumber = 0;
        int     imageWidth  = 0;
        int     imageHeight = 0;
        ImageInfo() {}
    };

private:
    static QMap<int, BatchComparison::ImageInfo> getFileInfo(const QString &filePath, bool isReadImage);
    void getFileInfoThread(const QString &filePath, bool isMould);
    void getFileInfoFished();
    void setProgressBar(float min, float max, float currentValue);
    void setFileInfo(QTextEdit *textEdit, const QMap<int, ImageInfo> &fileInfoMap);
    void setSaveFileinfo(const QString &info, bool isError);
    bool isExistByImageNumber(const QMap<int, ImageInfo> &infoMap, int imageNumber);
    void startComparisonThread(int imageNumber, const QString &mouldImage, const QString &comparisonImage);
    void startComparisonById(int imageId);

public slots:
    void comparisonThreadfinished(int imageNumber, const QString &path);

private slots:
    void on_getMouldImageFile_clicked();
    void on_getComparisonFile_clicked();
    void on_colorPalette_clicked();
    void on_imageNumber_valueChanged(int arg1);
    void on_exportImage_clicked();
    void on_previous_clicked();
    void on_next_clicked();
    void on_exportAllImage_clicked();

    void on_aboutBtn_clicked();

private:
    Ui::BatchComparison *ui;

    int         m_finishedNumber        = 0;
    int         m_batchComparisonNumber = 0;
    QString     m_mouldImagePath        = "";
    QString     m_comparisonImagePath   = "";
    QString     m_currentImage          = "";
    QThreadPool m_threadPool;
    ComparisonThread    *m_thread       = nullptr;
    QMap<int, ImageInfo> m_mouldImgeInfoMap;
    QMap<int, ImageInfo> m_comparisonImageInfoMap;
};

#endif // BATCHCOMPARISON_H
