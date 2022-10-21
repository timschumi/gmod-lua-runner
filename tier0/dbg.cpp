#include "dbg.h"

#include <cstdarg>
#include <cstdio>

void Msg(char const* format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void Warning(char const* format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

int DevMsg(int, char const* format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    // TODO: Figure out what this returns.
    return 0;
}

int DevWarning(int, char const* format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    // TODO: Figure out what this returns.
    return 0;
}
