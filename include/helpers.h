#ifndef HELPERS_H
#define HELPERS_H

#include <cstdint>
#include <QString>
#include <ShlObj.h>

uint32_t ipow(uint32_t base, uint32_t exp);

QString TCHAR2QString(TCHAR in[MAX_PATH]);

#endif // HELPERS_H
