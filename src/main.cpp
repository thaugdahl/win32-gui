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
#include "HorizontalElemContainer.h"
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
    Button btn2{"Click me as well please!", 200, 30, 20, 90};
    Button btn3{"Change color on left", 200, 30, 20, 120};
    Button btn4{"Change color on right", 200, 30, 20, 150};

    HorizontalElemContainer container(1000, 40, 400, 40);

    container.position(400, 40);


    auto windowWidth = mainWindow.getWidth();
    auto windowHeight = mainWindow.getHeight();

    GLViewport glvp{windowWidth - 300, windowHeight - 80, 250, 20};
    GLViewport glvp2{windowWidth / 2 - 150, windowHeight - 80, 250, 20};

    container.add(0.5, &glvp);
    container.add(0.5, &glvp2);

    mainWindow.show(nCmdShow);

    box.attach(hwnd);
    btn.attach(hwnd);
    btn2.attach(hwnd);
    btn3.attach(hwnd);
    btn4.attach(hwnd);
    glvp.attach(hwnd);
    glvp2.attach(hwnd);

    btn.setHandler([&] () {
        box.setText("Text changed");

        container.resize(windowWidth - 300, windowHeight - 80);
        container.position(300, 20);
        container.update();
        mainWindow.redraw();
    });

    btn2.setHandler([&]() {
        box.setText("Yipee!");

        std::unique_ptr<GLRenderer> renderer = std::make_unique<DefaultGLRenderer>(0,0,255);
        std::unique_ptr<GLRenderer> renderer2 = std::make_unique<DefaultGLRenderer>(0,255,0);
        glvp.setRenderer(std::move(renderer));
        glvp2.setRenderer(std::move(renderer2));
	});

    btn3.setHandler([&]() {
        GLRenderer *renderer = glvp.getRenderer();
        DefaultGLRenderer *adjRenderer = dynamic_cast<DefaultGLRenderer *>(renderer);

        if ( adjRenderer ) {
            std::string text = box.getText();

            // Parse color
            DefaultGLRenderer::Color color{0,0,0};


            std::stringstream ss{text};
            std::string token;

            auto isDigit = [](char c) {
                return c >= '0' && c <= '9';
            };

            auto res = !!std::getline(ss, token, ',');
            if ( res && isDigit(token.front()) ) { color.r = std::stoi(token); }
            res = !!std::getline(ss, token, ',');
            if ( res && isDigit(token.front()) ) { color.g = std::stoi(token); }
            res = !!std::getline(ss, token, ',');
            if ( res && isDigit(token.front()) ) { color.b = std::stoi(token); }

            adjRenderer->setColor(color.r, color.g, color.b);
        }
    });
btn4.setHandler([&]() {
        GLRenderer *renderer = glvp2.getRenderer();
        DefaultGLRenderer *adjRenderer = dynamic_cast<DefaultGLRenderer *>(renderer);

        if ( adjRenderer ) {
            std::string text = box.getText();

            // Parse color
            DefaultGLRenderer::Color color{0,0,0};


            std::stringstream ss{text};
            std::string token;

            auto isDigit = [](char c) {
                return c >= '0' && c <= '9';
            };

            auto res = !!std::getline(ss, token, ',');
            if ( res && isDigit(token.front()) ) { color.r = std::stoi(token); }
            res = !!std::getline(ss, token, ',');
            if ( res && isDigit(token.front()) ) { color.g = std::stoi(token); }
            res = !!std::getline(ss, token, ',');
            if ( res && isDigit(token.front()) ) { color.b = std::stoi(token); }

            adjRenderer->setColor(color.r, color.g, color.b);
        }
    });



    while (mainWindow.listen()) { };


    WindowIDHandler::teardown();

    return 0;
}
#endif

WNDPROC editProc;


