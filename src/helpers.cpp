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

#include <cstdint>

#include <ShlObj.h>

#include <QString>

#include "helpers.h"

uint32_t ipow(uint32_t base, uint32_t exp)
{
    uint32_t res = 1;
    for(uint32_t i = 0; i < exp; ++i)
    {
        res *= base;
    }

    return res;
}

QString TCHAR2QString(TCHAR in[MAX_PATH])
{
    #ifdef UNICODE
    return QString::fromUtf16((ushort*)in);
    #else
    return QString::fromLocal8Bit(in);
    #endif
}
