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

#ifndef _DIMENSIONS_HPP_
#define _DIMENSIONS_HPP_

#include <cstdint>

namespace g80 {

    typedef uint16_t Dimension;
    
    struct Point { 
        Dimension x, y; 
    };

    struct Area { 
        Area(Dimension w, Dimension h) : w_(w), h_(h) { 
            set_size(); 
        }

        inline auto set(Dimension w, Dimension h) -> void { 
            w_ = w; 
            h_ = h; 
            set_size(); 
        }

        inline auto w(Dimension w) -> void { 
            w_ = w; 
            set_size(); 
        }

        inline auto h(Dimension h) -> void { 
            h_ = h; 
            set_size(); 
        }

        inline auto w() const -> const Dimension { 
            return w_; 
        }

        inline auto h() const -> const Dimension { 
            return h_; 
        }

        inline auto w_mid() const -> const Dimension { 
            return w_ / 2; 
        };

        inline auto h_mid() const -> const Dimension { 
            return h_ / 2; 
        };

        inline auto size() const -> const Dimension { 
            return size_; 
        }

    private:
        Dimension w_, h_; 
        Dimension size_;

        inline auto set_size() -> void { 
            size_ = w_ * h_; 
        } 
    };

    struct Rectangle { 
        Point point; 
        Area area; 
    };
}

#endif 