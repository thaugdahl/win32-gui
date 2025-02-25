#include <cassert>
#include <iostream>
#include <windows.h>
#include <string>
#include <cstdio>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <map>

#include "GLViewport.h"
#include "MainWindow.h"
#include "TextBox.h"
#include "Button.h"
#include "WindowIDHandler.h"
#include "WindowClassManager.h"


// Global variables
// const char *CLASS_NAME = "Sample Window Class";




#ifdef _WIN32
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

    MODULE_INFO.hInstance = hInstance;

    WindowIDHandler::setup();

    MainWindow mainWindow = MainWindow::getInstance();

    HWND hwnd = mainWindow.getHandle();

    if (hwnd == NULL) {
        return 0;
    }

    TextBox box{"This is a test", 200, 30, 20, 20};
    Button btn{"Click me!", 200, 30, 20, 60};

    GLViewport glvp{200, 200, 100, 100};

    box.attach(hwnd);
    btn.attach(hwnd);
    glvp.attach(hwnd);

    btn.setHandler([&box] () {

        box.setText("Text changed");

    });

    mainWindow.show(nCmdShow);

    while (mainWindow.listen()) { };

    return 0;
}
#endif

WNDPROC editProc;


