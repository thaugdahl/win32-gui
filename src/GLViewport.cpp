#include "GLViewport.h"

wglCreateContextAttribsARB_type* wglCreateContextAttribsARB;
wglChoosePixelFormatARB_type *wglChoosePixelFormatARB;
GLViewport::GLViewport(size_t width, size_t height, size_t anchor_x,
                       size_t anchor_y)
    : DefaultResizer<GLViewport>{width, height, anchor_x, anchor_y},
      BoundingBox{width, height, anchor_x, anchor_y} {
  size_t ID = WindowIDHandler::getNew();
  setID(static_cast<size_t>(ID));
}

HWND GLViewport::attach(HWND parent) {
  HMENU id = (HMENU)getID();

  std::string className = "GLVIEWPORT";

    auto classDescriptor =
        WindowClassBuilder::builder()
            .callback(GLViewport::WindowProc)
            .className(className)
            .addStyle(CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS)
            .build();

    ATOM classAtom = WindowClassManager::addClass(classDescriptor);

  HINSTANCE parentHinstance =
      (HINSTANCE)GetWindowLongPtr(parent, GWLP_HINSTANCE);

  HWND handle = CreateWindow(
      // className.c_str(),
      reinterpret_cast<LPCSTR>(classAtom),
      NULL,
      // TEXT("GLViewport"),
      WS_CHILD | WS_VISIBLE, getPosX(),
      getPosY(), getWidth(), getHeight(), parent, NULL, parentHinstance, NULL);

  if (handle == nullptr) {
    fatal_error("Failed to create window?");
    return nullptr;
  }

  SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)this);

  setHandle(handle);

  setup_glcontext(handle, this);
  ShowWindow(handle, 1);
  UpdateWindow(handle);

  ShowWindow(handle, 1);
  UpdateWindow(handle);

  return handle;
}

LRESULT CALLBACK GLViewport::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam) {
  switch (uMsg) {
  case WM_CREATE: {
    break;
  }
  default:
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

  return 0;
}
static void init_opengl_extensions(void) {
  // Before we can load extensions, we need a dummy OpenGL context, created
  // using a dummy window. We use a dummy window because you can only set the
  // pixel format for a window once. For the real window, we want to use
  // wglChoosePixelFormatARB (so we can potentially specify options that aren't
  // available in PIXELFORMATDESCRIPTOR), but we can't load and use that before
  // we have a context. WNDCLASSA window_class = { 0 };
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

  PIXELFORMATDESCRIPTOR pfd = {0};
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
    // fatal_error("Failed to set the pixel format.");

    DWORD lastError = GetLastError();
    std::string err =
        "Failed to set the pixel format: " + std::to_string(int(lastError));
    fatal_error(err.c_str());
  }

  HGLRC dummy_context = wglCreateContext(dummy_dc);
  if (!dummy_context) {
    fatal_error("Failed to create a dummy OpenGL rendering context.");
  }

  if (!wglMakeCurrent(dummy_dc, dummy_context)) {
    fatal_error("Failed to activate dummy OpenGL rendering context.");
  }

  wglCreateContextAttribsARB =
      (wglCreateContextAttribsARB_type *)wglGetProcAddress(
          "wglCreateContextAttribsARB");
  wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type *)wglGetProcAddress(
      "wglChoosePixelFormatARB");

  wglMakeCurrent(dummy_dc, 0);
  wglDeleteContext(dummy_context);
  ReleaseDC(MainWindow::getInstance().getHandle(), dummy_dc);
  // DestroyWindow(dummy_window);
}

static HGLRC init_opengl(HDC real_dc) {
  init_opengl_extensions();

  // Now we can choose a pixel format the modern way, using
  // wglChoosePixelFormatARB.
  int pixel_format_attribs[] = {WGL_DRAW_TO_WINDOW_ARB,
                                GL_TRUE,
                                WGL_SUPPORT_OPENGL_ARB,
                                GL_TRUE,
                                WGL_DOUBLE_BUFFER_ARB,
                                GL_TRUE,
                                WGL_ACCELERATION_ARB,
                                WGL_FULL_ACCELERATION_ARB,
                                WGL_PIXEL_TYPE_ARB,
                                WGL_TYPE_RGBA_ARB,
                                WGL_COLOR_BITS_ARB,
                                32,
                                WGL_DEPTH_BITS_ARB,
                                24,
                                WGL_STENCIL_BITS_ARB,
                                8,
                                0};

  int pixel_format;
  UINT num_formats;
  wglChoosePixelFormatARB(real_dc, pixel_format_attribs, 0, 1, &pixel_format,
                          &num_formats);
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
      WGL_CONTEXT_MAJOR_VERSION_ARB,
      3,
      WGL_CONTEXT_MINOR_VERSION_ARB,
      3,
      WGL_CONTEXT_PROFILE_MASK_ARB,
      WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
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

  GLViewport &viewport = *((GLViewport *)param);

  MessageBox(NULL, "Hello from render thread", "ERROR", MB_OK | MB_ICONERROR);
  SIZE canvas_size;
  scoped_hglrc simpleRenderContext{wglCreateContext(dc)};

  if (!wglMakeCurrent(hDevContext, simpleRenderContext.get())) {
    return false;
  };

  wglCreateContextAttribsARB =
      (wglCreateContextAttribsARB_type *)wglGetProcAddress(
          "wglCreateContextAttribsARB");

  int const create_attribs[] = {WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                                WGL_CONTEXT_MINOR_VERSION_ARB, 1, 0};

  scoped_hglrc renderContext{
      wglCreateContextAttribsARB(hDevContext, NULL, create_attribs)};

  if (!renderContext) {
    return EXIT_FAILURE;
  }

  wglMakeCurrent(hDevContext, renderContext.get());

  size_t width = viewport.getWidth();
  size_t height = viewport.getHeight();
  size_t vx = viewport.getPosX();
  size_t vy = viewport.getPosY();

  glDisable(GL_DITHER);

  glClearColor(1.f, 0.f, 0.f, 1.0f);

  while (true) {
    glViewport(vx, vy, width, height);
    glClear(GL_COLOR_BUFFER_BIT);

    SwapBuffers(hDevContext);
  }

  return EXIT_SUCCESS;
}

