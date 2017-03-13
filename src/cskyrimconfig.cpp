#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QDir>
#include <QDateTime>
#include <QHash>
#include "cskyrimconfig.h"
#include "clog.h"
#include "helpers.h"

extern bool bLogging;

CSkyrimConfig::CSkyrimConfig()
{
    def = "NULL";
    QMessageBox msgError;
    msgError.setIcon(QMessageBox::Critical);
    msgError.setWindowTitle("File access error!");

    bLogging?skrmclog->enable():skrmclog->disable();

    skrmclog->event(LOGLV_NORMAL ,"searching skyrim config directory");

    TCHAR szPath[MAX_PATH];
    SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szPath);
    base_path = TCHAR2QString(szPath);
    base_path.append("\\My Games\\Skyrim\\");

    skrmclog->event(LOGLV_NORMAL, "found Skyrim config dir: \""+base_path+"\"");

    QFileInfo *conf_file = new QFileInfo();

    conf_file->setFile(base_path, "SkyrimPrefs.ini");

    if(!conf_file->exists() || !conf_file->isWritable())
    {
        msgError.setStandardButtons(QMessageBox::Ok);
        msgError.setDefaultButton(QMessageBox::Ok);
        msgError.setText("Couldn't open "+base_path+"SkyrimPrefs.ini!");
        msgError.exec();
        skrmclog->event(LOGLV_ERROR, "Couldn't open "+base_path+"SkyrimPrefs.ini!");
        exit(2);
    }

    conf_file->setFile(base_path, "Skyrim.ini");

    if(!conf_file->exists() || !conf_file->isWritable())
    {
        msgError.setStandardButtons(QMessageBox::Ok);
        msgError.setDefaultButton(QMessageBox::Ok);
        msgError.setText("Couldn't open "+base_path+"Skyrim.ini!");
        skrmclog->event(LOGLV_ERROR, "Couldn't open "+base_path+"Skyrim.ini!");
        msgError.exec();
        exit(2);
    }

    delete conf_file;

    skyrim_main = new CINI_Parse(base_path + "SkyrimPrefs.ini", true);
    skyrim_user = new CINI_Parse(base_path + "Skyrim.ini", true);
}

QString CSkyrimConfig::getValue(QString key)
{
    QVariant val;
    if( (val = skyrim_user->getValue(key).toString()) != "NULL" )
        return val.toString();

    if( (val = skyrim_main->getValue(key).toString()) != "NULL" )
        return val.toString();

    return QString("NULL");
}

void CSkyrimConfig::setValue(QString key, QString value, unsigned int file)
{
    QVariant qv(value);

    if(skyrim_main->getValue(key).toString() != "NULL")
    {
        skyrim_main->setValue(key, qv);
    }

    if(skyrim_user->getValue(key).toString() != "NULL")
    {
        skyrim_user->setValue(key, qv);
    }

    if(file == SKYRIM_MAIN)
    {
        skyrim_main->setValue(key, qv);
    }
    else if(file == SKYRIM_USER)
    {
        skyrim_user->setValue(key, qv);
    }
    else
    {
        skyrim_user->setValue(key, qv);
    }
}

void CSkyrimConfig::setValue(QString key, bool value, unsigned int file)
{
    this->setValue(key, QVariant( (value ? 1 : 0) ).toString(), file);
}

void CSkyrimConfig::setValue(QString key, double value, unsigned int file)
{
    this->setValue(key, QVariant( value ).toString(), file);
}

void CSkyrimConfig::setValue(QString key, int value, unsigned int file)
{
    this->setValue(key, QVariant( value ).toString(), file);
}

void CSkyrimConfig::setValue(QString key, float value, unsigned int file)
{
    this->setValue(key, QVariant( value ).toString(), file);
}

void CSkyrimConfig::startSection(QString section)
{
    skyrim_main->setNode("[" + section + "]");
    skyrim_user->setNode("[" + section + "]");
}

void CSkyrimConfig::save()
{
    bool worked;
    QDateTime curTime;
    curTime = curTime.currentDateTime();
    uint curTimeStamp = curTime.toTime_t();
    QFile backup_file(base_path + "SkyrimPrefs.ini");
    worked = backup_file.copy(base_path + "SkyrimPrefs.ini.backup_" + QVariant(curTimeStamp).toString());
    backup_file.close();

    skrmclog->event((worked?LOGLV_NORMAL:LOGLV_ERROR), "copying file \"SkyrimPrefs.ini\" to \"SkyrimPrefs.ini.backup_"+QVariant(curTimeStamp).toString()+"\":"+backup_file.errorString());

    backup_file.setFileName(base_path + "Skyrim.ini");
    worked = backup_file.copy(base_path + "Skyrim.ini.backup_" + QVariant(curTimeStamp).toString());

    skrmclog->event((worked?LOGLV_NORMAL:LOGLV_ERROR), "copying file \"Skyrim.ini\" to \"Skyrim.ini.backup_"+QVariant(curTimeStamp).toString()+"\":"+backup_file.errorString());

    backup_file.close();

    skyrim_main->save();
    skyrim_user->save();

    skrmclog->event(LOGLV_NORMAL, "saved Skyrim.ini and SkyrimPrefs.ini");
}

void CSkyrimConfig::endSection()
{
    //skyrim_main->endGroup();
    //skyrim_user->endGroup();
}
