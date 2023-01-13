#ifndef SAVEIMAGECONFIG_H
#define SAVEIMAGECONFIG_H

#include <QDialog>
#include "comm_def.h"

namespace Ui {
class SaveImageConfig;
}

class SaveImageConfig : public QDialog
{
    Q_OBJECT

public:
    static SaveImageConfig &instance();
    void setCameraSaveConfig(const SaveIamgeInfo &info);
    SaveIamgeInfo cameraSaveConfig();

    void setStatisticsSaveConfig(const SaveIamgeInfo &info);
    SaveIamgeInfo statisticsSaveConfig();

    void setComparisonSaveConfig(const SaveIamgeInfo &info);
    SaveIamgeInfo comparisonSaveConfig();

private slots:
    void on_reset_clicked();
    void on_selectCameraDir_clicked();
    void on_selectStatisticsDir_clicked();
    void on_selectComparisonDir_clicked();
    void on_cameraSuffix_currentIndexChanged(const QString &arg1);
    void on_statisticsSuffix_currentIndexChanged(const QString &arg1);
    void on_comparisonSuffix_currentIndexChanged(const QString &arg1);
    void on_Save_clicked();
    void on_close_clicked();

private:
    explicit SaveImageConfig(QWidget *parent = 0);
    ~SaveImageConfig();

    void loadConfig();
    void saveConfig();
    void showParameters();

    Ui::SaveImageConfig *ui;

    SaveIamgeInfo m_cameraSaveConfig;
    SaveIamgeInfo m_statisticsSaveConfig;
    SaveIamgeInfo m_comparisonSaveConfig;
};

#endif // SAVEIMAGECONFIG_H
