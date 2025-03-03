#pragma once

#include <string>
#include <windows.h>
#include "WindowInterface.h"
#include "WindowClassManager.h"
#include "WindowIDHandler.h"

class TextBox : public WindowInterface, public InputHandlerInterface, public DefaultResizer<TextBox>
{
    using WindowCallbackProcedure = LRESULT(*)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    TextBox(
        const std::string &placeholder,
        size_t width, size_t height,
        size_t anchor_x, size_t anchor_y)
    : DefaultResizer<TextBox>{width, height, anchor_x, anchor_y}, BoundingBox{width, height, anchor_x, anchor_y},
    placeholder{placeholder}
    {
        size_t ID = WindowIDHandler::getNew();
        setID(static_cast<size_t>(ID));
    }

    std::string getText() const {

        char buff[1024];
        GetWindowText(getHandle(), buff, 1024);

        return std::string{buff};
    }

    void setText(const std::string &text) {
        SendMessage(handle, WM_SETTEXT, 0, (LPARAM)text.c_str());
    }


    HWND attach(HWND parent) override
    {
        HWND handle = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            TEXT("EDIT"), // Window Class
            TEXT(placeholder.c_str()),
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
            getPosX(),
            getPosY(),
            getWidth(),
            getHeight(),
            parent,
            (HMENU) getID(),
            (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
            NULL
        );

        SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR) this);

        OldProc = (WindowCallbackProcedure)SetWindowLongPtr(handle, GWLP_WNDPROC, (LONG_PTR)TextBox::Proc);

        setHandle(handle);

        ShowWindow(handle, 1);
        UpdateWindow(handle);

        UpdateWindow(parent);

        return handle;
    }

    LRESULT handleInput(WPARAM param) override {
        if ( param == VK_RETURN ) {
            MessageBox(NULL, "The TextBox overrode default behavior!", "Notification", MB_OK);
        }

        return 0;
    }


    static LRESULT CALLBACK Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {


        TextBox *instance = (TextBox *) GetWindowLongPtr(hwnd, GWLP_USERDATA);


        if ( uMsg == WM_KEYDOWN ) {
            instance->handleInput(wParam);

        }

        return instance->OldProc(hwnd, uMsg, wParam, lParam);
    }


private:
    std::string placeholder;

    WindowCallbackProcedure OldProc;

};


