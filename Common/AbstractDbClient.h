#ifndef ABSTRACTDBCLIENT_H
#define ABSTRACTDBCLIENT_H

#include <QObject>
#include <QMutex>
#include <QSqlDatabase>
class AbstractDbClient : public QObject
{
    Q_OBJECT

public:
    explicit AbstractDbClient(QObject *parent = nullptr);
    explicit AbstractDbClient(QString host,QString username,QString password,int port, QString name,QObject *parent = nullptr);
    virtual ~AbstractDbClient();

    /* 对应数据库表 profilecameraphoto */
    struct ProfileCameraPhoto
    {
        int profileProductId = -1;
        int cameraId         = -1;
        int photoId          = -1;
        int offsetX          = -1;
        int offsetY          = -1;
        int width            = -1;
        int height           = -1;
        int exposureTime     = -1;
        float gamma          = -1;
        int lineRate         = -1;
        float preampGain     = -1;
        float gain           = -1;

        ProfileCameraPhoto() {}
    };

    void setName(const QString &name);
    QSqlDatabase sqlDatabase() const;

    /**
     * @brief getProfileCameraPhotosByProfileProductId  通过产品号获得光学配置参数
     * @param profileProductId 产品号
     * @return QList<ProfileCameraPhoto> 光学参数配置结果
     */
    QList<ProfileCameraPhoto> getProfileCameraPhotosByProfileProductId(int profileProductId);

    /**
     * @brief getProfileCameraPhoto 获取指定产品、相机、图号参数
     * @param profileProductId 产品
     * @param cameraId 相机
     * @param photoId 图号
     * @return
     */
    ProfileCameraPhoto getProfileCameraPhoto(int profileProductId, int cameraId, int photoId);

    /**
     * @brief updateProfileCameraPhoto 更新指定产品、相机、图号的ROI参数
     * @param profileProductId
     * @param cameraId
     * @param photoId
     * @param x
     * @param y
     * @param w
     * @param h
     * @return
     */
    bool updateProfileCameraPhoto(int profileProductId, int cameraId, int photoId, int x, int y, int w, int h);

    /**
     * @brief updateProfileCameraPhoto更新指定产品、相机、图号的拍照参数
     * @param profileProductId
     * @param cameraId
     * @param photoId
     * @param gain
     * @param gamma
     * @param exposureTime
     * @param preampGain
     * @param lineRate
     * @return
     */
    bool updateProfileCameraPhoto(int profileProductId, int cameraId, int photoId, float gain, float gamma, int exposureTime, float preampGain, int lineRate);

    QSqlDatabase    m_db;
    int             m_id;
    QString         m_name;

    static int      m_counter;
    static QMutex   m_mutex;
public slots:
};

#endif // ABSTRACTDBCLIENT_H
