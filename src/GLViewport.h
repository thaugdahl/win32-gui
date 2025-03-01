#pragma once

#include "Win32ErrorPrinter.h"
#include "WindowInterface.h"
#include "WindowClassManager.h"
#include "WindowIDHandler.h"
#include "MainWindow.h"
#include <string>
#include <utility>
#include <windows.h>
#include <gl/GL.h>
#include <iostream>

typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC, HGLRC, const int*);

typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList,
        const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);

#include "DebugHelp.h"


// See https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt for all values
#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B

// See https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt for all values

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001


#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

static HDC hDevContext;

static HANDLE renderThread;

template <class HandleT, BOOL (*destructor)(HandleT)>
struct HandleType
{
    HandleT get() const {
        return m_handle;
    }

    void set(HandleT handle) {
        m_handle = handle;
    }

    HandleType(HandleT &&handle) {
        m_handle = std::forward<HandleT>(handle);
    }

    operator bool() {
        return static_cast<bool>(m_handle);
    }

    ~HandleType() {
        destructor(m_handle);
    }

private:
    HandleT m_handle;
};

// Scoped Handle to GL Render Context
using scoped_hglrc = HandleType<HGLRC, wglDeleteContext>;

DWORD WINAPI RenderThreadProc(LPVOID param);

class GLViewport : public WindowInterface, public DefaultResizer<GLViewport>
{

public:
  GLViewport(size_t width, size_t height, size_t anchor_x, size_t anchor_y);

  friend int setup_glcontext(HWND hwnd, GLViewport *window) {
    // Get Device Context
    // Gets a handle to a device context for the client area of a window
    // hDevContext = GetDC(MainWindow::getInstance().getHandle());
    hDevContext = GetDC(hwnd);

    fatal_error("Ops?");

    // PIXELFORMATDESCRIPTOR pfd = { sizeof(pfd), 1 };
    // IXELFORMATDESCRIPTOR pfd = { 0 };
    // fd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    // fd.nVersion = 1;
    // fd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    // fd.iPixelType = PFD_TYPE_RGBA;
    // fd.cColorBits = 24;
    // fd.cRedBits = 8;
    // fd.cGreenBits = 8;
    // fd.cBlueBits = 8;
    // fd.cAlphaBits = 8;
    // fd.cDepthBits = 24;


    // nt pixelFormat = ChoosePixelFormat(hDevContext, &pfd);
    // f ( ! pixelFormat ) {
    //    MessageBox(NULL, "Failed to choose pixel format", "ERROR", MB_OK | MB_ICONERROR);
    //    PrintWin32Error();
    //    return false;
    //

    // ool pixelFormatSet = SetPixelFormat(hDevContext, pixelFormat, &pfd);
    // f ( ! pixelFormatSet ) {
    //    MessageBox(NULL, "Failed to set pixel format", "ERROR", MB_OK | MB_ICONERROR);
    //    PrintWin32Error();
    //    return false;
    //

    // nt activePixelFormatIndex = GetPixelFormat(hDevContext);

    // f ( ! activePixelFormatIndex ) {
    //    MessageBox(NULL, "Failed to get pixel format", "ERROR", MB_OK | MB_ICONERROR);
    //    PrintWin32Error();
    //    return false;
    //

    // f ( ! DescribePixelFormat(hDevContext, activePixelFormatIndex, sizeof pfd, &pfd)) {
    //    MessageBox(NULL, "Failed to describe pixel format", "ERROR", MB_OK | MB_ICONERROR);
    //    PrintWin32Error();
    //    return false;
    //

    // f (( pfd.dwFlags & PFD_SUPPORT_OPENGL ) != PFD_SUPPORT_OPENGL ) {
    //    MessageBox(NULL, "Failed to align flags ", "ERROR", MB_OK | MB_ICONERROR);
    //    PrintWin32Error();
    //    return false;
    //

    // essageBox(NULL, "Creating render thread", "ERROR", MB_OK | MB_ICONERROR);
    renderThread = CreateThread(NULL, 0, RenderThreadProc, window, 0, NULL);
    if ( renderThread == NULL ) return false;

    return true;
}

HWND attach(HWND parent) override;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                   LPARAM lParam);
};

static void init_opengl_extensions(void);

static HGLRC init_opengl(HDC real_dc);

DWORD WINAPI RenderThreadProc(LPVOID param);
