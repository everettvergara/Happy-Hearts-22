/*
 *  Image Class for text graphics
 *  Copyright (C) 2022 Everett Gaius S. Vergara
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *	
 */

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <iostream>
#include <fstream>
#include <memory>

#include "Dimensions.hpp"

namespace g80 {

    typedef uint8_t Color;
    typedef uint8_t Mask;
    typedef unsigned char Text;
    typedef std::unique_ptr<Color[]> Uptr_color;
    typedef std::unique_ptr<Mask[]> Uptr_mask;
    typedef std::unique_ptr<Text[]> Uptr_text;

    class Image {
    public:    
        Image();
        Image(Area area, Mask mask = 0x00);
        Image(const char *text, const Color color, const Mask mask = 0x00);
        Image(const char *filename);
        auto operator=(const Image &rhs) -> Image & = delete;
        auto operator=(Image &&rhs) -> Image &  = delete;

        auto raw_color() -> Uptr_color &;
        auto raw_text() -> Uptr_text &;
        auto raw_mask() -> Uptr_mask &;
        auto area() const -> const Area &;

        auto save(const char *filename) -> void;
        auto load(const char *filename) -> void;
        auto debug() -> void;

        auto fill_with_text(const char *text, const Color color) -> void;
        auto get_image(const Image &source, const Point point) -> void;
        auto and_mask(const Image &source, const Point point) -> void;
        auto or_image(const Image &source, const Point point) -> void;
        auto rotate_left() -> void;
        auto put_image(const Image &source, const Point point) -> void;
        auto show() -> void;

    private:
        Area area_;
        Uptr_color color_{nullptr};
        Uptr_text text_{nullptr};
        Uptr_mask mask_{nullptr};
    };    

}

#endif 