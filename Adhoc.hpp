/*
 *  Misc. functions and classes for Text Art - Valentines 2022 
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

#include <iostream>
#include <fstream>
#include <memory>

typedef uint16_t Dimension;
typedef uint8_t Color;
typedef uint8_t Mask;
typedef unsigned char Text;
typedef std::unique_ptr<Color[]> Uptr_color;
typedef std::unique_ptr<Mask[]> Uptr_mask;
typedef std::unique_ptr<Text[]> Uptr_text;

class TextImage {
public:    
    TextImage() {}
    TextImage(Dimension w, Dimension h) : 
        w_(w), h_(h), s_(w_ * h_),
        color_(std::make_unique<Color[]>(s_)),
        text_(std::make_unique<Text[]>(s_)),
        mask_(std::make_unique<Mask[]>(s_)) {}
    auto operator=(const TextImage &rhs) -> TextImage & = delete;
    auto operator=(TextImage &&rhs) -> TextImage &  = delete;
    inline auto size() const -> const Dimension { return s_; }

private:
    Dimension w_, h_, s_;
    Uptr_color color_ = nullptr;
    Uptr_text text_ = nullptr;
    Uptr_mask mask_ = nullptr;
};


struct Coordinate {
    Dimension x, y;
};

struct Rectangle {
    Coordinate coordinate;
    Dimension w, h;
    inline auto size() const -> const Dimension { return w * h; }
};