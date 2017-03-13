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
