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
