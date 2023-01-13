#ifndef COMM_DEF_H
#define COMM_DEF_H
#include <QPolygonF>
#include <QDebug>
#include <QObject>

typedef enum Camera_Type
{
    Camera_DahengNetAreaScan             = 0,           ///< DaHeng Net Camera
    Camera_DahengUsbAreaScan             = 1,           ///< DaHeng Usb Camera
    Camera_DalsaNetLineScan              = 2,           ///< DalsaNetLine Camera
    Camera_DalsaCameraLinkLineScan       = 3,           ///< DalsaCameraLinkLine Camera
    Camera_HaikangNetAreaScan            = 4,           ///< HaiKang Camera
    Camera_HaikangNetLineScan            = 5,           ///< HaiKang Camera
    Camera_DahuaNetLineScan              = 6,           ///< DaHua Camera
    CAMERA_DEFAULT                       = 7,           ///< Default Camera
}Camera_Type;

typedef enum InterfaceType
{
    Interface_GigE          = 0,
    Interface_USB           = 1,
    Interface_CameraLink    = 2,
}InterfaceType;

typedef enum AssessmentMethod
{
    MeanType  = 0,        // 平均值比较法
     MaxType  = 1,        // 最大值比较法
     MinType  = 2,        // 最小值比较法
}AssessmentMethod;

struct ModelData
{
                 int  Width          = 0;           // 图像宽度
                 int  Height         = 0;           // 图像高度
                 int  UpperTolerance = 5;           // 上公差
                 int  LowerTolerance = 5;           // 下公差
                 int  GridRow        = 3;           // 网格行数
                 int  GridCol        = 3;           // 网格列数
    AssessmentMethod  AssessmentType = MeanType;
           QPolygonF  ProductMarkPolygon;
       QList<QRectF>  AssessmentRect;

    friend QDebug operator <<(QDebug debug, const ModelData &modelData) {
        debug << u8"模型数据"
              << modelData.Width
              << modelData.Height
              << modelData.UpperTolerance
              << modelData.LowerTolerance
              << modelData.GridRow
              << modelData.GridCol
              << modelData.AssessmentType
              << modelData.ProductMarkPolygon
              << modelData.AssessmentRect;
        return debug;
    }
};

struct AssessmentData
{
    QRectF RectF = QRectF(0, 0, 0, 0);
        int mean = 0;       // 平均值
         int max = 0;       // 最大值
         int min = 0;       // 最小值

     friend QDebug operator <<(QDebug debug, const AssessmentData &data) {
         debug << u8"比较数据"
               << data.RectF
               << data.mean
               << data.max
               << data.min;
         return debug;
     }
};

struct ConnectData
{
              int ProductId   = 0;     // 产品配置id
               int CameraId   = 0;     // 相机配置id
                int PhotoId   = 0;     // 图号配置id
     QString ModelImageFile   = "";
   QString ModelXmlDataFile   = "";
      friend QDebug operator <<(QDebug debug, const ConnectData &data) {
          debug << u8"连接数据"
                << data.ProductId
                << data.CameraId
                << data.PhotoId
                << data.ModelImageFile
                << data.ModelXmlDataFile;
          return debug;
     }
};

struct CameraParameter
{
    int     Roi_X           = -1;
    int     Roi_Y           = -1;
    int     Roi_W           = -1;
    int     Roi_H           = -1;
    float   ExposureTime    = -1;
    float   Gamma           = -1;
    float   Gain            = -1;
    float   LineRate        = -1;
    float   PreampGain      = -1;
    bool    TriggerMode     = true;
    int     TriggerSource   = 1;
    friend QDebug operator <<(QDebug debug, const CameraParameter &data) {
        debug << u8"相机参数"
              << data.Roi_X
              << data.Roi_Y
              << data.Roi_W
              << data.Roi_H
              << data.ExposureTime
              << data.Gamma
              << data.Gain
              << data.LineRate
              << data.PreampGain
              << data.TriggerMode
              << data.TriggerSource;
        return debug;
   }
};

struct StatisticalInfo
{
                  ConnectData  ConnectConfig;
                    QPolygonF  ProductMarkPolygon;
                QList<QRectF>  AssessmentRect;
                       QImage  MouldImage;
                       QImage  RealTimeImage;
                          int  OffsetX              = 0;
                          int  OffsetY              = 0;
                          int  UpperLimit           = 5;
                          int  LowerLimit           = 5;
             AssessmentMethod  AssessmentType       = MeanType;
    QMap<int, AssessmentData>  ModelStatistical;
    QMap<int, AssessmentData>  ComparisonStatistical;
    friend QDebug operator <<(QDebug debug, const StatisticalInfo &data) {
        debug << u8"比较数据"
              << data.ConnectConfig
              << data.ProductMarkPolygon
              << data.AssessmentRect
              << data.MouldImage.size()
              << data.RealTimeImage.size()
              << data.OffsetX
              << data.OffsetY
              << data.UpperLimit
              << data.LowerLimit
              << data.AssessmentType
              << data.ModelStatistical
              << data.ComparisonStatistical;
        return debug;
   }
};

struct SaveIamgeInfo
{
    QString SaveDir     = "/tmp/optics_align_save_image/"; // 保存文件目录
    QString FileName    = 0;                               // 保存文件名字前缀
    QString SaveSuffix  = "jpg";                           // 保存文件格式
    friend QDebug operator <<(QDebug debug, const SaveIamgeInfo &data) {
        debug << u8"比较数据"
              << data.SaveDir
              << data.FileName
              << data.SaveSuffix;
        return debug;
     }
};

#endif // COMM_DEF_H
