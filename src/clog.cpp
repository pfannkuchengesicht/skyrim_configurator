#include <QString>
#include <QStringList>
#include <QVariant>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QMessageBox>
#include "clog.h"

bool bLogging;

CLog::CLog()
{
    logfile = "./program.log";
    bLogging = false;
    bEnabled = false;
}

CLog::CLog(QString log)
{
    logfile = log;
    bLogging = false;
    bEnabled = false;
}

void CLog::enable()
{
    bLogging = true;
    bEnabled = true;
}

void CLog::disable()
{
    bLogging = false;
    bEnabled = false;
}

void CLog::event(int level, QString msg)
{
    if(!bEnabled)
        return;

    QDateTime timestamp;
    timestamp = timestamp.currentDateTime();

    log = new QFile(logfile);

    if(!log->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;

    QTextStream out(log);

    switch(level)
    {
        case LOGLV_NORMAL:
            out << "[INFO]     ";
            break;
        case LOGLV_WARNING:
            out << "[WARNING]  ";
            break;
        case LOGLV_ERROR:
            out << "[ERROR]    ";
            break;
        case LOGLV_CRITICAL:
            out << "[CRITICAL] ";
            break;
    }

    out << timestamp.toString("dd.MM.yyyy hh:mm:ss") << ": " << msg << "\n";

    log->close();
    delete log;
}

void CLog::event(int level, QString msg, char *func, int line)
{
    if(!bEnabled)
        return;

    QDateTime timestamp;
    timestamp = timestamp.currentDateTime();

    log = new QFile(logfile);

    if(!log->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;

    QTextStream out(log);

    switch(level)
    {
        case LOGLV_NORMAL:
            out << "[INFO]     ";
            break;
        case LOGLV_WARNING:
            out << "[WARNING]  ";
            break;
        case LOGLV_ERROR:
            out << "[ERROR]    ";
            break;
        case LOGLV_CRITICAL:
            out << "[CRITICAL] ";
            break;
    }

    out << timestamp.toString("dd.MM.yyyy hh:mm:ss") << "(" << QVariant(func).toString() << "@" << QVariant(line).toString() << "): " << msg << "\n";

    log->close();
    delete log;
}
