#pragma once

#include <windows.h>
#include <string>

inline void fatal_error(const char *text) {

    auto errCode = GetLastError();

    char msgBuf[512];
    DWORD dwChars;

    dwChars = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errCode, 0, msgBuf, 512, NULL);
    std::string err = std::string{ text } + std::string{ " " } + std::to_string( errCode ) + ": " + msgBuf;

    MessageBox(NULL, err.c_str(), "ERROR", MB_OK | MB_ICONERROR);
}
