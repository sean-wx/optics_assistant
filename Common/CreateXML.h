#ifndef CREATEXML_H
#define CREATEXML_H
#include <QDomDocument>
#include <QDomElement>
#include <iostream>
#include "comm_def.h"

class CreateXML
{
public:
    CreateXML();

    bool saveXml(const QString &fileName, const ModelData &modelData);
    ModelData loadXml(const QString &fileName);

private:
    QDomElement ReadElement(const QDomElement &element, QString name);
    bool IsExistElement(const QDomElement &element, QString name);
};

#endif // CREATEXML_H
