#ifndef CLOG_H
#define CLOG_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QTime>

enum
{
    LOGLV_NORMAL,
    LOGLV_WARNING,
    LOGLV_ERROR,
    LOGLV_CRITICAL
};

class CLog
{
private:
    bool bEnabled;
    QString logfile;
    QFile *log;
public:
    CLog();
    CLog(QString log);
    void enable();
    void disable();
    void event(int level, QString msg);
    void event(int level, QString msg, char *func, int line);

    ~CLog()
    {
        log->close();
        delete log;
    }
};

static CLog *skrmclog = new CLog("./skrm_conf.log");

#endif // CLOG_H
