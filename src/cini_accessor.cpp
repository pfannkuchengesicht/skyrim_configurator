#include "cini_accessor.h"

CINI_Accessor::CINI_Accessor()
{

}

CINI_Accessor::CINI_Accessor(QString file)
{
    ini = new QFile(file);

    if(!ini->open(QIODevice::ReadWrite | QIODevice::Text))
        delete this;

    QTextStream in(ini);

    while(!in.atEnd())
    {
        KeyValues += in.readLine();
    }
}

QVariant CINI_Accessor::getValue(QString key)
{
    for(int i = 0; i < KeyValues.length(); i++)
    {
        if(KeyValues.at(i).contains(key))
        {
            QStringList split = KeyValues.at(i).split("=");
            return split.at(1);
        }
    }

    return QString("0");
}

void CINI_Accessor::setValue(QString key, QVariant value)
{
    for(int i = 0; i < KeyValues.length(); i++)
    {
        if(KeyValues.at(i).contains(key))
        {
            QStringList split = KeyValues.at(i).split("=");
            KeyValues.replaceInStrings(KeyValues.at(i), split.at(0) + "=" + value.toString());
            return;
        }
    }

    KeyValues += key + "=" + value.toString();
}

void CINI_Accessor::setFile(QString file)
{
    ini = new QFile(file);

    if(!ini->open(QIODevice::ReadWrite | QIODevice::Text))
        delete this;

    QTextStream in(ini);

    while(!in.atEnd())
    {
        KeyValues += in.readLine();
    }
}

void CINI_Accessor::save()
{
    ini->seek(0);
    QTextStream out(ini);

    for(int i = 0; i < KeyValues.length(); i++)
    {
        out << KeyValues.at(i) + "\n";
    }

    ini->resize(out.pos());
}

bool CINI_Accessor::isKey(QString key)
{
    for(int i = 0; i < KeyValues.length(); i++)
    {
        if(KeyValues.at(i).contains(key))
        {
            return true;
        }
    }

    return false;
}
