#ifndef CSKYRIMCONFIG_H
#define CSKYRIMCONFIG_H

#include <QString>

#include "cini_parse.h"

enum
{
    SKYRIM_MAIN,
    SKYRIM_USER
};

class CSkyrimConfig
{
private:
    CINI_Parse *skyrim_main;
    CINI_Parse *skyrim_user;
    QString base_path;
    QVariant def;
public:
    CSkyrimConfig();
    ~CSkyrimConfig()
    {
        delete skyrim_main;
        delete skyrim_user;
    }

    QString getValue(QString key);
    void setValue(QString key, QString value, unsigned int file);
    void setValue(QString key, bool value, unsigned int file);
    void setValue(QString key, double value, unsigned int file);
    void setValue(QString key, int value, unsigned int file);
    void setValue(QString key, float value, unsigned int file);
    void startSection(QString section);
    void endSection();
    void save();
};

#endif // CSKYRIMCONFIG_H
