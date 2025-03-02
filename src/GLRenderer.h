#pragma once

struct GLViewport;

struct GLRenderer {
    using RenderProc = void (*)(GLViewport *viewport);

    virtual RenderProc getRenderProc() const = 0;
    virtual bool shouldExit() const = 0;
    virtual void setExit()  = 0;
};


struct DefaultGLRenderer : public GLRenderer
{
    struct Color {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };

    Color rgb;

    void setColor(
        unsigned char r,
        unsigned char g,
        unsigned char b) {
        rgb.r = r;
        rgb.g = g;
        rgb.b = b;
    }

    void setExit() override {
        exitCondition = true;
    }

    bool shouldExit() const override {
        return exitCondition;
    }

    RenderProc getRenderProc() const override {
        return RenderProcedure;
    }

    DefaultGLRenderer(const DefaultGLRenderer &) = default;
    DefaultGLRenderer(DefaultGLRenderer &&) = default;
    DefaultGLRenderer &operator=(const DefaultGLRenderer &) = default;
    DefaultGLRenderer &operator=(DefaultGLRenderer &&) = default;
    DefaultGLRenderer(unsigned char r, unsigned char g, unsigned char b)
        : rgb{r, g, b} {}

    DefaultGLRenderer() :
        rgb{255,0,0} {}

    static void RenderProcedure(GLViewport *viewport);

private:
    bool exitCondition = false;
};

