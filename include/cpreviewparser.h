/*
    Skyrim Configurator
    Copyright (C) 2017  Roy Freytag

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CPREVIEWPARSER_H
#define CPREVIEWPARSER_H

#include <QXmlStreamReader>
#include <QList>

class CPreview
{
private:
    QString element;
    QString imgA;
    QString imgB;
    bool split;
    int value_min;
    int value_max;
public:
    CPreview()
    {
        element = "";
        imgA = "";
        imgB = "";
        split = false;
        value_min = 0;
        value_max = 0;
    }

    CPreview(QString e, int v_min, int v_max, QString p1, QString p2)
    {
        element = e;
        imgA = p1;
        imgB = p2;
        split = true;
        value_min = v_min;
        value_max = v_max;
    }

    CPreview(QString e, int v_min, int v_max, QString p1)
    {
        element = e;
        imgA = p1;
        imgB = "";
        split = false;
        value_min = v_min;
        value_max = v_max;
    }

    CPreview(QString e, int v_min, int v_max)
    {
        element = e;
        imgA = "";
        imgB = "";
        split = false;
        value_min = v_min;
        value_max = v_max;
    }

    CPreview(QString e)
    {
        element = e;
        imgA = "";
        imgB = "";
        split = false;
        value_min = 0;
        value_max = 0;
    }

    void addImgA(QString p1)
    {
        imgA = p1;
    }

    void addImgB(QString p1)
    {
        imgB = p1;
        split = true;
    }

    void setElement(QString e) { element = e; }
    void setSplit(bool s) { split = s; }
    void setValueMin(int v) { value_min = v; }
    void setValueMax(int v) { value_max = v; }
    QString getImgA() { return imgA; }
    QString getImgB() { return imgB; }
    bool isSplit() { return split; }
    int getValueMin() { return value_min; }
    int getValueMax() { return value_max; }
    QString getElement() { return element; }
};

class CPreviewParser
{
private:
    QXmlStreamReader *xml;
    QList<CPreview*> previews;
public:
    CPreviewParser();
    CPreviewParser(QIODevice* dev);
    void setDevice(QIODevice* dev);
    void parseImages(QString e, bool s, int value_min, int value_max);
    void parsePreview(QString e, bool s);
    void parse();
    CPreview* getPreview(QString element, int value);
};

#endif // CPREVIEWPARSER_H
