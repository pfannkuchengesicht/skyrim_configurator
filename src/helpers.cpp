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
