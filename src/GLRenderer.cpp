#include "GLRenderer.h"

#include "GLViewport.h"

void DefaultGLRenderer::RenderProcedure(GLViewport *viewport) {

    DefaultGLRenderer *renderer = static_cast<DefaultGLRenderer *>(viewport->getRenderer());

    if ( ! renderer) return;

      glDisable(GL_DITHER);


    auto color = renderer->rgb;
    float r = float(color.r) / 255.0f;
    float g = float(color.g) / 255.0f;
    float b = float(color.b) / 255.0f;

      // glClearColor(1.f, 0.f, 1.f, 1.0f);

      viewport->makeContextCurrent();
      glClearColor(r, g, b, 1.0f);

      while (!renderer->shouldExit()) {
        glViewport(0, 0, 1000, 600);
        glClear(GL_COLOR_BUFFER_BIT);

        viewport->swapBuffers();
      }

    viewport->setExitFlag();
    }

