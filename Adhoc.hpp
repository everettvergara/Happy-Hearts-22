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

#ifndef _ADHOC_VAL22_
#define _ADHOC_VAL22_

#include <iostream>
#include <fstream>
#include <memory>
#include <array>

#include "Dimensions.hpp"
#include "Image.h"

namespace g80 {
    constexpr int SZ_WAVE_COLORS = 15; 
    Color WAVE_PALETTE[SZ_WAVE_COLORS] {0, 0, 0, 0, 0, 4, 4, 4, 5, 5, 6, 6, 6, 6, 6};
    Text WAVE_TEXT[SZ_WAVE_COLORS] {' ', ' ', ' ', ' ', ' ', '.', '.', '^', '^', '*', '#', '#', '#', '#', '#'};

    constexpr int SZ_DEG_GRANULARITY = 360;
    float sine[SZ_DEG_GRANULARITY], cosine[SZ_DEG_GRANULARITY];

    class SmootherSinCosTable {
    public:
        SmootherSinCosTable(uint16_t min, uint16_t max) : min_(min), max_(max), cur_(min), inc_(1) {};
        auto get() const -> const uint16_t { return ix_; }
        auto next() -> uint16_t {
            ix_ = (ix_ + cur_) % SZ_DEG_GRANULARITY;
            cur_ += inc_;
            if (cur_ == min_ || cur_ == max_) inc_ *= -1; 
            return ix_;
        }
    private:
        uint16_t min_, max_, cur_, inc_;
        uint16_t ix_{0};
    };

    struct Droplet {
        Point point{0, 0};
        int animation_ix{0};
        int ixn{1};
        int stepper{0};
        int stepper_max{3};
    };
}

#endif 