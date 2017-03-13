#ifndef CINI_ACCESSOR_H
#define CINI_ACCESSOR_H

#include <QFile>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QTextStream>
#include <QMessageBox>

class CINI_Accessor
{
private:
    QFile *ini;
    QStringList KeyValues;
public:
    CINI_Accessor();
    CINI_Accessor(QString file);
    bool isKey(QString key);
    QVariant getValue(QString key);
    void setValue(QString key, QVariant value);
    void setFile(QString file);
    void save();
    ~CINI_Accessor()
    {
        ini->close();
        delete ini;
    }
};

#endif // CINI_ACCESSOR_H
