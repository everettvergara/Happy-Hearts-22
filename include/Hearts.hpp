/*
 *  Heart auxilliary classes for Text Art - Valentines 2022 
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

#ifndef _HEARTS_HPP_
#define _HEARTS_HPP_

#include <cstdint>

namespace g80 {

    constexpr int SZ_DEG_GRANULARITY = 360;
    float sine[SZ_DEG_GRANULARITY], cosine[SZ_DEG_GRANULARITY];

    class SmootherSinCosTable {
    public:
        SmootherSinCosTable(uint16_t min, uint16_t max) : 
            min_(min), max_(max), cur_(min) {
        };

        auto get() const -> const uint16_t { 
            return ix_; 
        }

        auto next() -> uint16_t {
            ix_ = (ix_ + cur_) % SZ_DEG_GRANULARITY;
            cur_ += inc_;
            if (cur_ == min_ || cur_ == max_) inc_ *= -1; 
            return ix_;
        }
        
    private:
        uint16_t min_, max_, cur_, inc_{1};
        uint16_t ix_{0};
    };
}

#endif