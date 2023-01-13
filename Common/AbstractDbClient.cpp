#include "AbstractDbClient.h"
#include <QCoreApplication>
#include <QUuid>
#include <QDebug>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QPoint>
#include <QTimer>

int AbstractDbClient::m_counter = 0;
QMutex AbstractDbClient::m_mutex;

AbstractDbClient::AbstractDbClient(QObject *parent)
    : QObject(parent)
{
    QSettings settings("db.ini", QSettings::IniFormat);

    if (settings.value("host").isNull()) {
        settings.setValue("host", "localhost");
    }

    if (settings.value("port").isNull()) {
        settings.setValue("port", 3306);
    }

    if (settings.value("name").isNull()) {
        settings.setValue("name", "HMI_");
    }

    if (settings.value("user").isNull()) {
        settings.setValue("user", "root");
    }

    if (settings.value("password").isNull()) {
        settings.setValue("password", "Cube1234");
    }

    settings.sync();

    QString host = settings.value("host").toString();
    int     port = settings.value("port").toInt();
    QString name = settings.value("name").toString();
    QString user = settings.value("user").toString();
    QString password = settings.value("password").toString();

    m_db = QSqlDatabase::addDatabase("QMYSQL", QUuid::createUuid().toString());
    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(name);
    m_db.setUserName(user);
    m_db.setPassword(password);
    if (!m_db.open()) {
        qFatal("Can not open database: %s", m_db.lastError().text().toStdString().c_str());
    }

    m_mutex.lock();
    m_id = ++m_counter;
    m_mutex.unlock();
}

AbstractDbClient::AbstractDbClient(QString host, QString username, QString password, int port, QString dbname, QObject *parent)
    : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QMYSQL", QUuid::createUuid().toString());
    m_db.setHostName(host);
    m_db.setPort(port);
    m_db.setDatabaseName(dbname);
    m_db.setUserName(username);
    m_db.setPassword(password);
    if (!m_db.open()) {
        qFatal("Can not open database: %s", m_db.lastError().text().toStdString().c_str());
    }

    m_mutex.lock();
    m_id = ++m_counter;
    m_mutex.unlock();
}

AbstractDbClient::~AbstractDbClient()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

void AbstractDbClient::setName(const QString &name)
{
    m_name = name;
}

QSqlDatabase AbstractDbClient::sqlDatabase() const
{
    return m_db;
}

QList<AbstractDbClient::ProfileCameraPhoto> AbstractDbClient::getProfileCameraPhotosByProfileProductId(int profileProductId){
    QList<ProfileCameraPhoto> profileCameraPhotos;
    QString condition="";
    if (profileProductId > 0){
        condition=QString(" And ProfileProductID=%1").arg(profileProductId);
    }
    QString sql(QString("SELECT * FROM ProfileCameraPhoto where 1=1 %1").arg(condition));
    QSqlQuery query(m_db);
    if (query.exec(sql)) {
        QSqlRecord record = query.record();
        while(query.next()) {
            QVariantMap map;
            for(int i = 0; i < record.count(); i++) {
                map.insert(record.fieldName(i), query.value(i));
            }
            ProfileCameraPhoto profileCameraPhoto;
            profileCameraPhoto.profileProductId = map["ProfileProductID"].toInt();
            profileCameraPhoto.cameraId         = map["CameraID"].toInt();
            profileCameraPhoto.photoId          = map["PhotoID"].toInt();
            profileCameraPhoto.offsetX          = map["OffsetX"].toInt();
            profileCameraPhoto.offsetY          = map["OffsetY"].toInt();
            profileCameraPhoto.width            = map["Width"].toInt();
            profileCameraPhoto.height           = map["Height"].toInt();
            profileCameraPhoto.exposureTime     = map["ExposureTime"].toInt();
            profileCameraPhoto.gamma            = map["Gamma"].toFloat();
            profileCameraPhoto.lineRate         = map["LineRate"].toInt();
            profileCameraPhoto.preampGain       = map["PreampGain"].toFloat();
            profileCameraPhoto.gain             = map["Gain"].toFloat();


            profileCameraPhotos.append(profileCameraPhoto);
        }
    }
    else {
        qWarning() << QString("Failed to excute sql(%1): %2").arg(sql).arg(query.lastError().text());
    }

    return profileCameraPhotos;
}

AbstractDbClient::ProfileCameraPhoto AbstractDbClient::getProfileCameraPhoto(int profileProductId, int cameraId, int photoId)
{
    ProfileCameraPhoto profileCameraPhoto;
    QString sql(QString("SELECT * FROM ProfileCameraPhoto where ProfileProductId='%1' and CameraID='%2' and PhotoID='%3'")
                .arg(profileProductId)
                .arg(cameraId)
                .arg(photoId));
    QSqlQuery query(m_db);
    if (query.exec(sql)) {
        QSqlRecord record = query.record();
        while(query.next()) {
            QVariantMap map;
            for(int i = 0; i < record.count(); i++) {
                map.insert(record.fieldName(i), query.value(i));
            }
            profileCameraPhoto.profileProductId = map["ProfileProductID"].toInt();
            profileCameraPhoto.cameraId         = map["CameraID"].toInt();
            profileCameraPhoto.photoId          = map["PhotoID"].toInt();
            profileCameraPhoto.offsetX          = map["OffsetX"].toInt();
            profileCameraPhoto.offsetY          = map["OffsetY"].toInt();
            profileCameraPhoto.width            = map["Width"].toInt();
            profileCameraPhoto.height           = map["Height"].toInt();
            profileCameraPhoto.exposureTime     = map["ExposureTime"].toInt();
            profileCameraPhoto.gamma            = map["Gamma"].toFloat();
            profileCameraPhoto.lineRate         = map["LineRate"].toInt();
            profileCameraPhoto.preampGain       = map["PreampGain"].toFloat();
            profileCameraPhoto.gain             = map["Gain"].toFloat();
        }
    }
    else {
        qWarning() << QString("Failed to excute sql(%1): %2").arg(sql).arg(query.lastError().text());
        profileCameraPhoto.cameraId = 0;
    }
    return profileCameraPhoto;
}

bool AbstractDbClient::updateProfileCameraPhoto(int profileProductId, int cameraId, int photoId, int x, int y, int w, int h)
{
    QString sql=QString("update ProfileCameraPhoto set OffsetX='%1',OffsetY='%2',Width='%3',Height='%4' where ProfileProductId='%5' and PhotoID='%6' and CameraID='%7'")
            .arg(x)
            .arg(y)
            .arg(w)
            .arg(h)
            .arg(profileProductId)
            .arg(photoId)
            .arg(cameraId);
    QSqlQuery updateExec(m_db);
    if (!updateExec.exec(sql)) {
        qWarning() << QString("Failed to excute sql(%1): %2").arg(sql).arg(updateExec.lastError().text());
        return false;
    }
    return true;
}

bool AbstractDbClient::updateProfileCameraPhoto(int profileProductId, int cameraId, int photoId, float gain, float gamma, int exposureTime, float preampGain, int lineRate)
{
    QString sql=QString("update ProfileCameraPhoto set Gain='%1',Gamma='%2',ExposureTime='%3',PreampGain='%4',LineRate='%5' where ProfileProductId='%6' and PhotoID='%7' and CameraID='%8'")
            .arg(gain)
            .arg(gamma)
            .arg(exposureTime)
            .arg(preampGain)
            .arg(lineRate)
            .arg(profileProductId)
            .arg(photoId)
            .arg(cameraId);
    QSqlQuery updateExec(m_db);
    if (!updateExec.exec(sql)) {
        qWarning() << QString("Failed to excute sql(%1): %2").arg(sql).arg(updateExec.lastError().text());
        return false;
    }
    return true;
}
