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
constexpr float HEART_RADIUS = 10.0f; 


auto cache_sin_cos_table() -> void;
auto set_center_pos(const Image &screen, const Image &source, int cos_sin_ix, int x_dir, int y_dir) -> Point;
auto delay_until_mspf(const TimePointSysClock &start) -> void;

auto main(int argc, char **argv) -> int {
    
    Image screen("/Users/everett/Documents/Codes/Projects/HappyValentines2022/screen.img");
    Image marquee("/Users/everett/Documents/Codes/Projects/HappyValentines2022/marquee.img");
    Image heart("/Users/everett/Documents/Codes/Projects/HappyValentines2022/heart.img");

    Image behind_heart1(heart.dimensions());
    Image behind_heart2(heart.dimensions());
    cache_sin_cos_table();

    SmootherSinCosTable i(5, 30);
    do {
        // Start timing ms per frame
        TimePointSysClock start {SysClock::now()};

        // Rotate with smoothing function
        i.next();
        Point point_heart1 = set_center_pos(screen, heart, i.get(), 1, -1);
        Point point_heart2 = set_center_pos(screen, heart, i.get(), -1, 1);

        // Process hearts 
        behind_heart1.get_image(screen, point_heart1);
        behind_heart2.get_image(screen, point_heart2);
        screen.and_mask(heart, point_heart1);
        screen.and_mask(heart, point_heart2);
        marquee.rotate_left();
        screen.or_image(marquee, {0, 0});

        // Show Hearts, Wave and Greetings
        screen.show();

        // Delay until mspf is reached
        delay_until_mspf(start);

        // Restore screen to original state
        screen.put_image(behind_heart2, point_heart2);
        screen.put_image(behind_heart1, point_heart1);

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

auto set_center_pos(const Image &screen, const Image &source, int cos_sin_ix, int x_dir, int y_dir) -> Point {
    Point point {
        static_cast<uint16_t>(screen.dimensions().get_center_width() - source.dimensions().get_center_width() + HEART_RADIUS * cosine[cos_sin_ix] * x_dir), 
        static_cast<uint16_t>(screen.dimensions().get_center_height() - source.dimensions().get_center_height()  + HEART_RADIUS * sine[cos_sin_ix] * y_dir)
    };
    return point;
}

auto delay_until_mspf(const TimePointSysClock &start) -> void {
    TimePointSysClock end = SysClock::now();
    int delay = MSPF - chr::duration_cast<chr::milliseconds>(end - start).count();
    if (delay > 0) std::this_thread::sleep_for(chr::milliseconds(delay));
}