#pragma once

#include <windows.h>
#include <string>
#include "WindowClassManager.h"
#include "WindowInterface.h"

class MainWindow : public BoundingBox {
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
        static MainWindow window{"Sample Window Class", 1920, 1080};

        return window;
    }

    bool show(bool cmd) {
        return ShowWindow(hwnd, cmd);
    }

    HWND getHandle() const {
        return hwnd;
    }

    void resize(size_t x, size_t y) override {
        setWidth(x);
        setHeight(y);
    }
    void position(size_t x, size_t y) override {
        setAnchorX(x);
        setAnchorY(y);
    }

    void redraw() {
        HWND handle = getHandle();
        // InvalidateRect(handle, NULL, true);
        RedrawWindow(handle, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
        // RedrawWindow(handle, NULL, NULL, RDW_ERASE);
        // UpdateWindow(handle);
    }


private:


    MainWindow(std::string className,
         int width, int height)
        : title{"Main Window"},
        BoundingBox{1920, 1080, 0,0}
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
            CW_USEDEFAULT, CW_USEDEFAULT, getWidth(), getHeight(),
            NULL,
            NULL,
            MODULE_INFO.hInstance,
            NULL
        );
    }

    HWND hwnd;
    std::string title;
    HANDLE windowID = 0;

};
