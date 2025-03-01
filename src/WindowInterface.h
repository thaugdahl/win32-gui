#pragma once

#include <windows.h>

class ResizableInterface {
public:
    virtual void resize(size_t x, size_t y) = 0;
    virtual void position(size_t x, size_t y) = 0;

    virtual size_t getWidth() const = 0;
    virtual size_t getHeight() const = 0;
    virtual size_t getPosX() const = 0;
    virtual size_t getPosY() const = 0;

    ~ResizableInterface() = default;
};

class InputHandlerInterface {
    virtual LRESULT handleInput(WPARAM param) = 0;
};

class BoundingBox : public ResizableInterface {
public:
    BoundingBox(size_t w, size_t h, size_t x, size_t y)
    : width{w}, height{h}, anchor_x{x}, anchor_y{y}
    {}

    ~BoundingBox() = default;

public:
    size_t getWidth() const final {
        return width;
    }


    size_t getHeight() const final {
        return height;
    }

    size_t getPosX() const final {
        return anchor_x;
    }

    size_t getPosY() const final {
        return anchor_y;
    }

protected:

    void setWidth(size_t width) {
        this->width = width;
    }

    void setHeight(size_t height) {
        this->height = height;
    }

    void setAnchorX(size_t anchor_x) {
        this->anchor_x = anchor_x;
    }

    void setAnchorY(size_t anchor_x) {
        this->anchor_x = anchor_x;
    }

    size_t width;
    size_t height;

    size_t anchor_x;
    size_t anchor_y;
};


template <class WindowType>
class DefaultResizer : virtual public BoundingBox {
public:
    DefaultResizer()
    {}

    DefaultResizer(
        size_t width, size_t height,
        size_t anchor_x, size_t anchor_y)
    : BoundingBox{width, height, anchor_x, anchor_y} {}

    void resize(size_t x, size_t y) override {

        RECT resizerRect;

        WindowType *instance = reinterpret_cast<WindowType *>(this);

        HWND handle = instance->getHandle();

        if ( GetWindowRect(instance->getHandle(), &resizerRect ) ) {
            int px = resizerRect.left;
            int py = resizerRect.top;

            int width = x;
            int height = y;

            MoveWindow(handle, px, py, width * 2, height, true);
            ShowWindow(handle, 1);

        }
    }

    void position(size_t x, size_t y) override {

        RECT resizerRect;

        WindowType *instance = reinterpret_cast<WindowType *>(this);

        HWND handle = instance->getHandle();

        if ( GetWindowRect(instance->getHandle(), &resizerRect ) ) {
            int width = resizerRect.right - resizerRect.left;
            int height = resizerRect.bottom- resizerRect.top;

            MoveWindow(handle, x, y, width * 2, height, true);
            ShowWindow(handle, 1);

        }
    }

};

class WindowInterface {
public:
    virtual HWND getHandle() {
        return handle;
    }

    virtual size_t getID() const {
        return ID;
    }

protected:
    virtual void setHandle(HWND handle) {
        this->handle = handle;
    }

    virtual void setID(size_t ID) {
        this->ID = ID;
    }

    virtual HWND attach(HWND parent) = 0;

public:
  WindowInterface(const WindowInterface &) = default;
  WindowInterface(WindowInterface &&) = default;
  WindowInterface &operator=(const WindowInterface &) = default;
  WindowInterface &operator=(WindowInterface &&) = default;

    WindowInterface() = default;

    ~WindowInterface() = default;

protected:
    HWND handle = nullptr;
    size_t ID;


};
