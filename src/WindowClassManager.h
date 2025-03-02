#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include "DebugHelp.h"

#ifdef _WIN32
struct MyStoredWin32Info {
    HINSTANCE hInstance;
};
#endif
static MyStoredWin32Info MODULE_INFO;

using WindowProcSignature = LRESULT (*)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
struct WindowClassDescriptor {
    WindowProcSignature callback;
    std::string className;
    UINT style = 0;
};

class WindowClassBuilder {
public:
    static WindowClassBuilder builder() {
        return WindowClassBuilder{};
    }

    WindowClassBuilder& callback(WindowProcSignature callback) {
        result.callback = callback;
        return *this;
    }

    WindowClassBuilder& className(const std::string &className_) {
        result.className = className_;
        return *this;
    }

    WindowClassBuilder& addStyle(UINT style) {
        result.style |= style;
        return *this;
    }

    WindowClassBuilder& removeStyle(UINT style) {
        result.style &= ~style;
        return *this;
    }


    WindowClassDescriptor build() {
        return result;
    }

private:
    WindowClassBuilder() {}

    WindowClassDescriptor result;
};

class WindowClassManager {
public:
    static ATOM addClass(WindowClassDescriptor &descriptor) {

        // Avoid creating twice
        if (getName(descriptor.className)) {
            return classAtoms[descriptor.className];
        }



        WNDCLASS wc = {};

        wc.lpfnWndProc = descriptor.callback;
        wc.hInstance = MODULE_INFO.hInstance;
        wc.lpszClassName = addName(descriptor.className);
        wc.style = descriptor.style;

        // This is needed to color the background on erasure.
        wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        classes.emplace_back(wc);

        ATOM classAtom = RegisterClass(&wc);


        classAtoms[descriptor.className] = classAtom;

        return classAtom;
    }

    static const char* getName(const std::string& name) {
        if (std::find(classNames.begin(), classNames.end(), std::make_pair(name,name)) != classNames.end()) {
            return classNames[name].c_str();
        }

        return nullptr;
    }

private:
    static const char * addName(const std::string &name) {

        const char* res = getName(name);
        if (res != nullptr) {
            return res;
        }

        classNames[name] = name;
        return (classNames[name].c_str());
    }

private:
    static std::vector<WNDCLASS> classes;
    static std::map<std::string, ATOM> classAtoms;
    static std::map<std::string,std::string> classNames;
};
