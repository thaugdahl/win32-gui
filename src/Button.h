#pragma once

#include <functional>
#include <string>
#include <windows.h>
#include "WindowInterface.h"
#include "WindowClassManager.h"
#include "WindowIDHandler.h"

class Button : public WindowInterface, public DefaultResizer<Button>
{
    using WindowCallbackProcedure = LRESULT(*)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    Button(
        const std::string &placeholder,
        size_t width, size_t height,
        size_t anchor_x, size_t anchor_y)
    : DefaultResizer<Button>{width, height, anchor_x, anchor_y}, BoundingBox{width, height, anchor_x, anchor_y},
    placeholder{placeholder}
    {
        size_t ID = WindowIDHandler::getNew();
        setID(static_cast<size_t>(ID));
    }


    void setHandler(const std::function<void ()> &proc) {
        handleProc = proc;
    }


    HWND attach(HWND parent) override
    {

        HMENU id = (HMENU) getID();

        HWND handle = CreateWindow(
            TEXT("BUTTON"), // Window Class
            TEXT(placeholder.c_str()),
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            getPosX(),
            getPosY(),
            getWidth(),
            getHeight(),
            parent,
            id,
            (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
            NULL
        );

        SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR) this);

        OldProc = (WindowCallbackProcedure)SetWindowLongPtr(handle, GWLP_WNDPROC, (LONG_PTR)Button::Proc);

        setHandle(handle);

        ShowWindow(handle, 1);
        UpdateWindow(handle);

        return handle;
    }

    static LRESULT CALLBACK Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

        Button *instance = (Button *) GetWindowLongPtr(hwnd, GWLP_USERDATA);


        if ( uMsg == WM_COMMAND ) {
            if ( instance->handleProc ) {
                instance->handleProc();
            }
        }

        // The result value matters.
        return instance->OldProc(hwnd, uMsg, wParam, lParam);
    }


private:
    std::string placeholder;

    std::function<void ()> handleProc;

    WindowCallbackProcedure OldProc = nullptr;

};


