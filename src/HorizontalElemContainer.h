#pragma once

#include <vector>

#include "WindowInterface.h"
class HorizontalElemContainer : public BoundingBox {

public:
    HorizontalElemContainer(int width, int height)
    : BoundingBox(width, height, 0, 0) {

    }

    HorizontalElemContainer(int width, int height, int posx, int posy)
    : BoundingBox(width, height, posx, posy) {

    }

    void add(double percentage, ResizableInterface *window) {
        subWindows.emplace_back(std::make_pair(percentage, window));
    }

    void resize(size_t width, size_t height) override final {
        if(height > 0)
            this->height = height;

        if ( width > 0 )
            this->width = width;
    }

    void position(size_t x, size_t y) override final {
        this->setAnchorX(x);
        this->setAnchorY(y);
    }

    void update() {
        reconfigure();
    }


private:
    void reconfigure() {

        auto width = getWidth();

        //
        //  |---| |----| |-------|
        //  |   | |    | |       |
        //  |---| |----| |-------|
        //
        width -= padding * (subWindows.size() + 2);

        size_t xPos = padding;
        for ( std::size_t idx = 0; idx < subWindows.size(); idx++ ) {
            auto [percentage, window] = subWindows[idx];

            auto windowHeight = window->getHeight();

            size_t newWidth = static_cast<size_t>(double(width) * percentage);

            const auto containerPosX = getPosX();
            window->resize(newWidth, windowHeight);
            window->position(
                containerPosX + xPos, getPosY());

            xPos += newWidth + padding;
        }
    }


    std::vector<std::pair<double, ResizableInterface *>> subWindows;
    size_t padding = 10;

};
