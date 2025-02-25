#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <map>

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

    WindowClassDescriptor build() {
        return result;
    }

private:
    WindowClassBuilder() {}

    WindowClassDescriptor result;
};

class WindowClassManager {
public:
    static WNDCLASS* addClass(WindowClassDescriptor &descriptor) {
        WNDCLASS wc = {};

        wc.lpfnWndProc = descriptor.callback;
        wc.hInstance = MODULE_INFO.hInstance;
        wc.lpszClassName = addName(descriptor.className);

        classes.emplace_back(wc);

        RegisterClass(&classes.back());


        return &classes.back();
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
    static std::map<std::string,std::string> classNames;
};
