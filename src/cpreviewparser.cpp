#include "cpreviewparser.h"
#include <QMessageBox>
#include <QVariant>

CPreviewParser::CPreviewParser()
{
}

CPreviewParser::CPreviewParser(QIODevice *dev)
{
    if(dev == NULL)
        delete this;

    xml = new QXmlStreamReader(dev);
}

void CPreviewParser::setDevice(QIODevice *dev)
{
    if(dev != NULL)
        return;

    if(xml)
        delete xml;

    xml = new QXmlStreamReader(dev);
}

void CPreviewParser::parseImages(QString e, bool s, int value_min, int value_max)
{
    CPreview *tmp = new CPreview();
    tmp->setSplit(s);
    tmp->setElement(e);
    tmp->setValueMin(value_min);
    tmp->setValueMax(value_max);

    while(xml->readNextStartElement())
    {
        if(xml->name() == "img")
        {
            if(xml->attributes().value("type").toString() == "0")
            {
                tmp->addImgA(xml->readElementText());
            }
            else if(xml->attributes().value("type").toString() == "1")
            {
                tmp->addImgB(xml->readElementText());
            }
        }
        else xml->skipCurrentElement();
    }

    previews += tmp;
}

void CPreviewParser::parse()
{
    while(xml->readNextStartElement())
    {
        if(xml->name() == "previews")
        {
            continue;
        }
        else if(xml->name() == "preview")
        {
            parsePreview(xml->attributes().value("element").toString(), xml->attributes().value("split").toString().toInt());
        }
        else xml->skipCurrentElement();
    }
}

CPreview *CPreviewParser::getPreview(QString element, int value)
{
    QList<CPreview*>::iterator i = previews.begin();
    while(i != previews.end())
    {
        CPreview *tmp = *i;

        if(!tmp)
            continue;

        if(tmp->getElement() == element)
        {
            if(value >= tmp->getValueMin() && value <= tmp->getValueMax())
            {
                return tmp;
            }
        }
        ++i;
    }

    return NULL;
}

void CPreviewParser::parsePreview(QString e, bool s)
{
    int min, max;
    QString val;
    while(xml->readNextStartElement())
    {
        if(xml->name() == "images")
        {
            if((val = xml->attributes().value("value").toString()).isEmpty())
            {
                min = xml->attributes().value("value_min").toString().toInt();
                max = xml->attributes().value("value_max").toString().toInt();
            }
            else
            {
                min = max = val.toInt();
            }

            parseImages(e, s, min, max);
        }
        else xml->skipCurrentElement();
    }
}
