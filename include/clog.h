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
