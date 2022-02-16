/*
 *  Dimension structs used in HappyValAnim22
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

#ifndef _DIMENSIONS_H_
#define _DIMENSIONS_H_

#include <cstdint>

namespace egv {

    typedef uint16_t Dimension;
    
    struct Point { 
        Dimension x, y; 
    };

    struct Area { 
        Area(Dimension width, Dimension height) : width_(width), height_(height) { set_size(); }

        inline auto set(Dimension width, Dimension height) -> void { width_ = width; height_ = height; set_size(); }
        inline auto width(Dimension width) -> void { width_ = width; set_size(); }
        inline auto height(Dimension height) -> void { height_ = height; set_size(); }

        inline auto width() const -> const Dimension { return width_; }
        inline auto height() const -> const Dimension { return height_; }
        inline auto width_center() const -> const Dimension { return width_ / 2; };
        inline auto height_center() const -> const Dimension { return height_ / 2; };
        inline auto size() const -> const Dimension { return size_; }

    private:
        Dimension width_, height_; 
        Dimension size_;

        inline auto set_size() -> void { size_ = width_ * height_; } 
    };

    struct Rectangle {
        Point point;
        Area area;
    };
}
    
#endif 