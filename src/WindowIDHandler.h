#pragma once

#include <windows.h>

struct WindowIDHandler {
    static HANDLE hMutex;
    static size_t nextID;
    static bool isSetup;

    static size_t getNew();

    static bool setup();
    static bool teardown();
};
