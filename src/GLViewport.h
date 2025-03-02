#pragma once


#include <optional>
#include <utility>
#include <windows.h>
#include <gl/GL.h>

#include "DebugHelp.h"
#include "WindowInterface.h"
#include "GLRenderer.h"

typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC, HGLRC, const int*);

typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList,
        const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);



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


typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC, HGLRC, const int*);

typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList,
        const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);



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

// static HDC hDevContext;
// static HANDLE renderThread;

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
        *this = std::move(handle);
    }

    HandleType(const HandleT &handle) {
        destructor(m_handle);
        *this = handle;
    }

    HandleType &operator=(HandleT &&handle) {
        // Avoid self-assignment calling destructor
        if ( handle != m_handle ) {
            this->~HandleType();
            m_handle = std::forward<HandleT>(handle);
        }

        return *this;
    }

    operator bool() {
        return static_cast<bool>(m_handle);
    }

    ~HandleType() {
        if ( m_handle != nullptr ) {
            destructor(m_handle);
        }
    }

private:
    HandleT m_handle;
};

// Scoped Handle to GL Render Context
using scoped_hglrc = HandleType<HGLRC, wglDeleteContext>;


class GLViewport : public WindowInterface, public DefaultResizer<GLViewport>
{

public:
    GLViewport(size_t width, size_t height, size_t anchor_x, size_t anchor_y);

    int setup_glcontext(HWND hwnd, GLViewport *window) {
        // Get Device Context
        // Gets a handle to a device context for the client area of a window
        // hDevContext = GetDC(MainWindow::getInstance().getHandle());
        hDeviceContext = GetDC(hwnd);

        setupRenderContext();

        return true;
    }

    HWND attach(HWND parent) override;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam);

    static void init_opengl_extensions(void);
    static HGLRC init_opengl(HDC real_dc);

    bool makeContextCurrent() {
        wglMakeCurrent(hDeviceContext, NULL);
        return wglMakeCurrent(hDeviceContext, renderContext.get());
    }

    void setRenderer(std::unique_ptr<GLRenderer> &&renderer) {

        // Release the device context, so the spawned thread can obtain ownership.
        wglMakeCurrent(NULL, NULL);

        // Release the renderer if already exists
        if ( ! this->renderer ) {
            this->renderer = std::move(renderer);
            CreateThread(NULL, 0, GLViewport::RenderThreadProc, this, 0, NULL);
            return;
        }

        this->renderer->setExit();

        while ( ! exitFlag && this->renderer ) {
            // Wait for exit to finish
        }

        exitFlag = false;

        // Use RAII to clean up
        std::unique_ptr<GLRenderer> oldRenderer = std::move(this->renderer);

        this->renderer = std::move(renderer);

        CreateThread(NULL, 0, GLViewport::RenderThreadProc, this, 0, NULL);
    }

    GLRenderer * getRenderer() {
        return this->renderer.get();
    }


    void setExitFlag() {
        exitFlag = true;
    }


    void swapBuffers() {
        if ( hDeviceContext != nullptr )
            if (!SwapBuffers(hDeviceContext)) {
                fatal_error("FAiled to swap buffers");
            }
    }

    void setupRenderContext();

private:
    std::unique_ptr<GLRenderer> renderer;

    HDC hDeviceContext;

    scoped_hglrc renderContext{nullptr};

    bool exitFlag = false;

    static DWORD WINAPI RenderThreadProc(LPVOID param);


};


DWORD WINAPI RenderThreadProc(LPVOID param);
