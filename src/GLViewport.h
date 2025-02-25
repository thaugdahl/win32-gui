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
wglCreateContextAttribsARB_type* wglCreateContextAttribsARB;

typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList,
        const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
wglChoosePixelFormatARB_type *wglChoosePixelFormatARB;


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
    GLViewport(
        size_t width, size_t height,
        size_t anchor_x, size_t anchor_y)
    : DefaultResizer<GLViewport>{width, height, anchor_x, anchor_y}, BoundingBox{width, height, anchor_x, anchor_y} {
        size_t ID = WindowIDHandler::getNew();
        setID(static_cast<size_t>(ID));
    }

friend int setup_glcontext(HWND hwnd, GLViewport *window) {
    // Get Device Context
    // Gets a handle to a device context for the client area of a window
    // hDevContext = GetDC(MainWindow::getInstance().getHandle());
    hDevContext = GetDC(hwnd);

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

    HWND attach(HWND parent) override
    {
        HMENU id = (HMENU) getID();

        std::string className = "GLVIEWPORT";

        if (WindowClassManager::getName(className) == nullptr) {
			auto classDescriptor = WindowClassBuilder::builder()
				.callback(GLViewport::WindowProc)
				.className(className)
                .addStyle(CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS)
				.build();

			WindowClassManager::addClass(classDescriptor);
        }



        HWND handle = CreateWindow(
            TEXT(className.c_str()),
            TEXT("GLViewport"),
            WS_CHILD | WS_VISIBLE,
            getPosX(),
            getPosY(),
            getWidth(),
            getHeight(),
            parent,
            NULL,
            (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE),
            NULL
        );



        SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR) this);

        setHandle(handle);

        setup_glcontext(handle, this);
        ShowWindow(handle, 1);
        UpdateWindow(handle);


        ShowWindow(handle, 1);
        UpdateWindow(handle);

        return handle;

    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch ( uMsg ) {
            case WM_CREATE:
            {
            }
        }

        return 0;
    }


};


void fatal_error(const char *text) {
    MessageBox(NULL, text, "ERROR", MB_OK | MB_ICONERROR);
}

static void
init_opengl_extensions(void)
{
    // Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
    // We use a dummy window because you can only set the pixel format for a window once. For the
    // real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
    // that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
    // have a context.
    // WNDCLASSA window_class = { 0 };
    //     window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    //     window_class.lpfnWndProc = DefWindowProcA;
    //     window_class.hInstance = GetModuleHandle(0);
    //     window_class.lpszClassName = "Dummy_WGL_djuasiodwa";

    // if (!RegisterClassA(&window_class)) {
    //     fatal_error("Failed to register dummy OpenGL window.");
    // }

    // HWND dummy_window = CreateWindowExA(
    //     0,
    //     window_class.lpszClassName,
    //     "Dummy OpenGL Window",
    //     0,
    //     CW_USEDEFAULT,
    //     CW_USEDEFAULT,
    //     CW_USEDEFAULT,
    //     CW_USEDEFAULT,
    //     0,
    //     0,
    //     window_class.hInstance,
    //     0);

    // if (!dummy_window) {
    //     fatal_error("Failed to create dummy OpenGL window.");
    // }


    HDC dummy_dc = GetDC(MainWindow::getInstance().getHandle());

    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.cColorBits = 32;
        pfd.cAlphaBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;

    int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
    if (!pixel_format) {
        fatal_error("Failed to find a suitable pixel format.");
    }
    if (!SetPixelFormat(dummy_dc, pixel_format, &pfd)) {
        fatal_error("Failed to set the pixel format.");
    }

    HGLRC dummy_context = wglCreateContext(dummy_dc);
    if (!dummy_context) {
        fatal_error("Failed to create a dummy OpenGL rendering context.");
    }

    if (!wglMakeCurrent(dummy_dc, dummy_context)) {
        fatal_error("Failed to activate dummy OpenGL rendering context.");
    }

    wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)wglGetProcAddress(
        "wglCreateContextAttribsARB");
    wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)wglGetProcAddress(
        "wglChoosePixelFormatARB");

    wglMakeCurrent(dummy_dc, 0);
    wglDeleteContext(dummy_context);
    ReleaseDC(MainWindow::getInstance().getHandle(), dummy_dc);
    // DestroyWindow(dummy_window);
}

static HGLRC
init_opengl(HDC real_dc)
{
    init_opengl_extensions();

    // Now we can choose a pixel format the modern way, using wglChoosePixelFormatARB.
    int pixel_format_attribs[] = {
        WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        WGL_STENCIL_BITS_ARB,       8,
        0
    };

    int pixel_format;
    UINT num_formats;
    wglChoosePixelFormatARB(real_dc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
    if (!num_formats) {
        fatal_error("Failed to set the OpenGL 3.3 pixel format.");
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(real_dc, pixel_format, sizeof(pfd), &pfd);
    if (!SetPixelFormat(real_dc, pixel_format, &pfd)) {
        fatal_error("Failed to set the OpenGL 3.3 pixel format.");
    }

    // Specify that we want to create an OpenGL 3.3 core profile context
    int gl33_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    HGLRC gl33_context = wglCreateContextAttribsARB(real_dc, 0, gl33_attribs);
    if (!gl33_context) {
        fatal_error("Failed to create OpenGL 3.3 context.");
    }

    if (!wglMakeCurrent(real_dc, gl33_context)) {
        fatal_error("Failed to activate OpenGL 3.3 rendering context.");
    }

    return gl33_context;
}

DWORD WINAPI RenderThreadProc(LPVOID param) {
    HDC dc = hDevContext;

    HGLRC glrc = init_opengl(dc);


    GLViewport &viewport = *((GLViewport *) param);


	MessageBox(NULL, "Hello from render thread", "ERROR", MB_OK | MB_ICONERROR);
    SIZE canvas_size;
    scoped_hglrc simpleRenderContext{wglCreateContext(dc)};

    if (! wglMakeCurrent(hDevContext, simpleRenderContext.get())) {
        return false;
    };

    wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type *) wglGetProcAddress("wglCreateContextAttribsARB");


    int const create_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 1,
        0
    };

    scoped_hglrc renderContext{wglCreateContextAttribsARB(hDevContext, NULL, create_attribs)};

    if ( ! renderContext ) {
        return EXIT_FAILURE;
    }

    wglMakeCurrent(hDevContext, renderContext.get()) ;

    size_t width = viewport.getWidth();
    size_t height = viewport.getHeight();
    size_t vx = viewport.getPosX();
    size_t vy = viewport.getPosY();


    glDisable(GL_DITHER);

    glClearColor(1.f,0.f,0.f, 1.0f);

    while ( true ) {
		glViewport(vx, vy, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        SwapBuffers(hDevContext);
    }

    return EXIT_SUCCESS;
}
