#include <QFile>
#include <QXmlStreamWriter>
#include <QMessageBox>
#include <QDateTime>
#include "CreateXML.h"

CreateXML::CreateXML()
{

}

bool CreateXML::saveXml(const QString &fileName, const ModelData &modelData)
{
    QString strFile(fileName);
    QFile file(strFile);
    if (!file.open(QFile::WriteOnly | QFile::Text)) { // 只写模式打开文件
            qDebug() << QString("Cannot write file %1(%2).").arg(strFile).arg(file.errorString());
            return false;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true); // 自动格式化
    writer.writeStartDocument("1.0", true);  // 开始文档（XML 声明）
    writer.writeProcessingInstruction("xml-stylesheet type=\"text/css\" href=\"style.css\"");  // 处理指令
    writer.writeStartElement("doc");

    writer.writeTextElement("path", fileName);

    writer.writeStartElement("size");
    writer.writeTextElement("width",QString::number(modelData.Width));
    writer.writeTextElement("height",QString::number(modelData.Height));
    writer.writeEndElement(); // End size

    writer.writeStartElement("tolerance");
    writer.writeTextElement("upper",QString::number(modelData.UpperTolerance));
    writer.writeTextElement("lower",QString::number(modelData.LowerTolerance));
    writer.writeEndElement(); // End tolerance

    writer.writeStartElement("gride");
    writer.writeTextElement("row",QString::number(modelData.GridRow));
    writer.writeTextElement("column",QString::number(modelData.GridCol));
    writer.writeEndElement(); // End gride

    writer.writeStartElement("assessment");
    writer.writeTextElement("type",QString::number(modelData.AssessmentType));
    writer.writeEndElement(); // End assessment

    writer.writeStartElement("polygon");
    for(int i = 0; i < modelData.ProductMarkPolygon.size(); i++) {
        writer.writeTextElement(QString("x%1").arg(i + 1), QString::number(modelData.ProductMarkPolygon[i].x()));
        writer.writeTextElement(QString("y%1").arg(i + 1), QString::number(modelData.ProductMarkPolygon[i].y()));
    }
    writer.writeEndElement(); // End polygon

    writer.writeStartElement("rects");
    for (int i = 0; i < modelData.AssessmentRect.size(); i++) {
        writer.writeStartElement("rect");
        writer.writeTextElement("x", QString::number(modelData.AssessmentRect[i].x()));
        writer.writeTextElement("y", QString::number(modelData.AssessmentRect[i].y()));
        writer.writeTextElement("w", QString::number(modelData.AssessmentRect[i].width()));
        writer.writeTextElement("h", QString::number(modelData.AssessmentRect[i].height()));
        writer.writeEndElement(); // End item
    }
    writer.writeEndElement(); // End rects

    writer.writeStartElement("time_labeled");
    writer.writeTextElement("save_time", QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"));
    writer.writeEndElement(); // time_labeled

    writer.writeEndDocument();  // 结束文档

    file.close();  // 关闭文件
    return true;
}

ModelData CreateXML::loadXml(const QString &fileName)
{
    ModelData res;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        std::cerr << "Error: Cannot read file " << qPrintable(fileName)
                  << ": " << qPrintable(file.errorString())
                  << std::endl;
        return res;
    }

    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument doc;
    if (!doc.setContent(&file, false, &errorStr, &errorLine, &errorColumn))
    {
        std::cerr << "Error: Parse error at line " << errorLine << ", "
                  << "column " << errorColumn << ": "
                  << qPrintable(errorStr) << std::endl;
        return res;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "doc")
    {
        qDebug() << "Error: Not a doc xml file";
        return res;
    }

    qDebug() << "open xml "<<fileName;

    QDomElement ele = ReadElement(root,"size");
    if(ele.tagName() != "doc") {
        res.Width = ReadElement(ele,"width").text().toInt();
        res.Height = ReadElement(ele,"height").text().toInt();
    }

    ele = ReadElement(root,"tolerance");
    if(ele.tagName() != "doc") {
        res.UpperTolerance = ReadElement(ele,"upper").text().toInt();
        res.LowerTolerance = ReadElement(ele,"lower").text().toInt();
    }

    ele = ReadElement(root,"gride");
    if(ele.tagName() != "doc") {
        res.GridRow = ReadElement(ele,"row").text().toInt();
        res.GridCol = ReadElement(ele,"column").text().toInt();
    }

    ele = ReadElement(root,"assessment");
    if(ele.tagName() != "doc") {
        res.AssessmentType = AssessmentMethod(ReadElement(ele,"type").text().toInt());
    }

    ele = ReadElement(root,"polygon");
    if(ele.tagName() != "doc") {
        auto children = ele.childNodes();
        for(int i = 1; i <= children.count() / 2; i++) {
            QDomElement x = ReadElement(ele, QString("x%1").arg(i));
            QDomElement y = ReadElement(ele, QString("y%1").arg(i));
            QPointF pointF(x.text().toDouble(), y.text().toDouble());
            res.ProductMarkPolygon.append(pointF);
        }
    }
    ele = ReadElement(root,"rects");
    if(ele.tagName() != "doc") {
        auto children = ele.childNodes();
        for(int i = 0; i < children.count(); i++) {
            QDomElement x = ReadElement(children.at(i).toElement(), "x");
            QDomElement y = ReadElement(children.at(i).toElement(), "y");
            QDomElement w = ReadElement(children.at(i).toElement(), "w");
            QDomElement h = ReadElement(children.at(i).toElement(), "h");
            QRectF rectF(x.text().toDouble(), y.text().toDouble(), w.text().toDouble(), h.text().toDouble());
            res.AssessmentRect.append(rectF);
        }
    }
    qDebug() << res;
    return res;
}

QDomElement CreateXML::ReadElement(const QDomElement &element,QString name)
{
    QStringList list = name.split("/");
    QDomNode child = element.firstChild();
    while (!child.isNull())
    {
        if (child.toElement().tagName() == list[0])
        {
            if(list.count()>1){
                return ReadElement(child.toElement(),name.replace(list[0]+"/",""));
            }else{
                return child.toElement();
            }
        }
        child = child.nextSibling();
    }
    return element;
}

bool CreateXML::IsExistElement(const QDomElement &element,QString name){
    QStringList list = name.split("/");
    QDomNode child = element.firstChild();
    while (!child.isNull())
    {
        if (child.toElement().tagName() == list[0])
        {
            if(list.count()>1){
                return IsExistElement(child.toElement(),name.replace(list[0]+"/",""));
            }else{
                return true;
            }
        }
        child = child.nextSibling();
    }
    return false;
}
