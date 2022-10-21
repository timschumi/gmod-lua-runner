#pragma once

extern "C" {
void Msg(char const* format, ...);
void Warning(char const* format, ...);
int DevMsg(int level, char const* format, ...);
int DevWarning(int level, char const* format, ...);
}
