#pragma once

#include <windows.h>
#include <string>
#include "WindowClassManager.h"

class MainWindow {
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);



public:
    bool listen() {
        MSG msg = {};
        bool gotMessage = GetMessage(&msg, NULL, 0, 0);

        if (gotMessage) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return gotMessage;
    }

    static MainWindow& getInstance() {
        static MainWindow window{"Sample Window Class"};

        return window;
    }

    bool show(bool cmd) {
        return ShowWindow(hwnd, cmd);
    }

    HWND getHandle() const {
        return hwnd;
    }

private:

    MainWindow(std::string className)
        : title{"Main Window"}
    {

        auto classDescriptor = WindowClassBuilder::builder()
            .callback(MainWindow::WindowProc)
            .className(className)
            .build();

        WindowClassManager::addClass(classDescriptor);

        const char* cpClassName = WindowClassManager::getName(className);

        // Create the main window

        hwnd = CreateWindowEx(
            0,
            cpClassName,
            title.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 1000, 600,
            NULL,
            NULL,
            MODULE_INFO.hInstance,
            NULL
        );
    }

    HWND hwnd;
    std::string title;
    HANDLE windowID = 0;
    HANDLE b1ID;
    HANDLE b2ID;
    HANDLE e1ID;

};
