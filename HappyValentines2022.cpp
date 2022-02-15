/*
 *  Text Art for Valentines 2022
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
#include <chrono>
#include <thread>
#include <cmath>

#include "Adhoc.hpp"
#include "Misc.hpp"

using namespace egv;

namespace chr = std::chrono;
typedef chr::time_point<chr::system_clock> TimePointSysClock;
typedef chr::system_clock SysClock;

constexpr int FPS = 15;
constexpr int MSPF = 1000 / FPS;
constexpr int DEG_GRANULARITY = 360;
constexpr float HEART_RADIUS = 10.0f; 

float sine[DEG_GRANULARITY], cosine[DEG_GRANULARITY];

auto cache_sin_cos_table() -> void;
auto set_center_pos(const Image &screen, int cos_sin_ix, int x_dir, int y_dir) -> Point;
auto delay_until_mspf(const TimePointSysClock &start) -> void;

auto main(int argc, char **argv) -> int {
    
    Image screen("/Users/everett/Documents/Codes/Projects/HappyValentines2022/screen.img");
    Image marquee("/Users/everett/Documents/Codes/Projects/HappyValentines2022/marquee.img");
    Image heart("/Users/everett/Documents/Codes/Projects/HappyValentines2022/heart.img");
    Image behind_heart1(heart.dimensions());
    Image behind_heart2(heart.dimensions());
    cache_sin_cos_table();

    int i = 0;
    do {
        // Start timing ms per frame
        TimePointSysClock start {SysClock::now()};


        // Process Hearts
        Point point_heart1 = set_center_pos(screen, i, 1, -1);
        Point point_heart2 = set_center_pos(screen, i, 1, -1);
        behind_heart1.get_image(screen, point_heart1);
        behind_heart2.get_image(screen, point_heart2);
        screen.and_mask(heart, point_heart1);
        // screen.and_mask(heart, point_heart2);
        // marquee.rotate_left();
        // screen.or_image(heart, point_heart1);
        // screen.or_image(heart, point_heart2);

        // Show Hearts, Wave and Greetings
        // screen.show();
        screen.debug();
        return 0;
        // Delay until mspf is reached
        delay_until_mspf(start);

        screen.put_image(behind_heart1, point_heart1);
        screen.put_image(behind_heart2, point_heart2);
    } while(!is_key_pressed());
}

auto cache_sin_cos_table() -> void {
    float rad = 0.0f;
    for (int i = 0; i < DEG_GRANULARITY; ++i) {
        cosine[i] = cosf(rad);
        sine[i] = sinf(rad);
        rad += M_PI * 2.0 / DEG_GRANULARITY;
    }
}

auto set_center_pos(const Image &screen, int cos_sin_ix, int x_dir, int y_dir) -> Point {
    Point point {
        static_cast<uint16_t>(screen.dimensions().get_center_width() + HEART_RADIUS * cosine[cos_sin_ix] * x_dir), 
        static_cast<uint16_t>(screen.dimensions().get_center_width() + HEART_RADIUS * sine[cos_sin_ix] * y_dir)
    };
    return point;
}

auto delay_until_mspf(const TimePointSysClock &start) -> void {
    TimePointSysClock end = SysClock::now();
    int delay = MSPF - chr::duration_cast<chr::milliseconds>(end - start).count();
    if (delay > 0) std::this_thread::sleep_for(chr::milliseconds(delay));
}