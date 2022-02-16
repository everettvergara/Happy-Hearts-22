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
#include <array>
#include <chrono>
#include <thread>
#include <cmath>

#include "Dimensions.hpp"
#include "Image.h"
#include "Wave.hpp"
#include "Hearts.hpp"
#include "Droplet.hpp"
#include "Misc.hpp"

using namespace g80;

namespace chr = std::chrono;
typedef chr::time_point<chr::system_clock> TimePointSysClock;
typedef chr::system_clock SysClock;
typedef std::array<Image, 3> DropletAnimation;
typedef std::array<Droplet, 100> Droplets;

constexpr int FPS = 15;
constexpr int MSPF = 1000 / FPS;
constexpr float HEART_RADIUS = 10.0f;

auto set_droplet_animation_images(const Image &screen, DropletAnimation &droplet_animation, Droplets &droplets) -> void;
auto animate_droplets(Image &screen, const DropletAnimation &droplet_animation, Droplets &droplets) -> void;
auto set_starting_wave(Image &wave, Uptr_color &wave_averages) -> void;
auto animate_wave(Image &wave, Uptr_color &wave_averages) -> void;
auto cache_sin_cos_table() -> void;
auto set_center_pos(const Image &screen, const Image &source, int cos_sin_ix, int x_dir, int y_dir) -> Point;
auto reset_wave_colors(Uptr_color &wave_averages, size_t sz_wave) -> void;
auto delay_until_mspf(const TimePointSysClock &start) -> void;

auto main(int argc, char **argv) -> int {
    
    Image screen("./asset/screen.img");
    Image marquee("./asset/marquee.img");
    Image heart("./asset/heart.img");
    Image greetings(" ~ ~ ~ ~ ~ Happy Heart's Day 2022 ~ ~ ~ ~ ~", 2, 0xff);
    Image download_at("https://github.com/everettvergara/HappyValentines2022", 3, 0xff);
    
    Image wave({screen.area().w(), SZ_WAVE_COLORS}, 0xff);
    Uptr_color wave_averages = std::make_unique<Color[]>(wave.area().size());
    
    DropletAnimation droplet_animation {
        Image({1, 5}, 0xff), 
        Image({1, 5}, 0xff), 
        Image({1, 5}, 0xff)};
    Droplets droplets;
    
    screen.put_image(greetings, {static_cast<Dimension>(screen.area().w_mid() - greetings.area().w_mid()), 0});
    screen.put_image(download_at, {static_cast<Dimension>(screen.area().w_mid() - download_at.area().w_mid()), 1});
    Image behind_heart1(heart.area());
    Image behind_heart2(heart.area());
    
    cache_sin_cos_table();
    reset_wave_colors(wave_averages, wave.area().size());
    set_droplet_animation_images(screen, droplet_animation, droplets);

    SmootherSinCosTable i(5, 30), j (1, 50);
    do {
        // Start timing ms per frame
        TimePointSysClock start {SysClock::now()};
        
        // Start of Droplet Animation
        animate_droplets(screen, droplet_animation, droplets);

        // Start of Wave Animation
        set_starting_wave(wave, wave_averages);
        animate_wave(wave, wave_averages);
        screen.put_image(wave, {0, static_cast<Dimension>(screen.area().h() - wave.area().h() - 1)});
        

        // Start of Hearts Animation
        // Rotate with smoothing function
        i.next(); j.next();
        Point point_heart1 = set_center_pos(screen, heart, i.get(), -1, -1);
        Point point_heart2 = set_center_pos(screen, heart, j.get(), 1, 1);

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

auto set_droplet_animation_images(const Image &screen, DropletAnimation &droplet_animation, Droplets &droplets) -> void {
    
    droplet_animation[0].raw_text()[0] = ' ';
    droplet_animation[0].raw_text()[1] = ' ';
    droplet_animation[0].raw_text()[2] = '.';
    droplet_animation[0].raw_text()[3] = '.';
    droplet_animation[0].raw_text()[4] = '@';

    droplet_animation[1].raw_text()[0] = ' ';
    droplet_animation[1].raw_text()[1] = ' ';
    droplet_animation[1].raw_text()[2] = '.';
    droplet_animation[1].raw_text()[3] = '.';
    droplet_animation[1].raw_text()[4] = '@';

    droplet_animation[2].raw_text()[0] = ' ';
    droplet_animation[2].raw_text()[1] = ' ';
    droplet_animation[2].raw_text()[2] = ' ';
    droplet_animation[2].raw_text()[3] = '.';
    droplet_animation[2].raw_text()[4] = '@';     

    for (auto &droplet_image : droplet_animation)
        for (int i = 0; i < 5; ++i)
            droplet_image.raw_color()[i] = 4;

    for (auto &droplet : droplets) {
        droplet.point.x = rand() % screen.area().w();
        droplet.point.y = 2 + rand() % (screen.area().h() - 10);
        droplet.animation_ix = rand() % 3;
        droplet.stepper_max = 1 + rand() % 3;
    }
}

auto animate_droplets(Image &screen, const DropletAnimation &droplet_animation, Droplets &droplets) -> void {
    int i = 0;
    for (auto &droplet : droplets) {
        droplet.animation_ix = (droplet.animation_ix + droplet.ixn);
        if (droplet.animation_ix == 3) { droplet.animation_ix = 2; droplet.ixn *= -1; }
        if (droplet.animation_ix == -1) { droplet.animation_ix = 0; droplet.ixn *= -1; }
        
        if (++droplet.stepper == droplet.stepper_max) {
            droplet.stepper = 0;
            ++droplet.point.y;
        }

        if (droplet.point.y > (screen.area().h() - 10)) {
            droplet.point.x = rand() % screen.area().w();
            droplet.point.y = 2;
            droplet.stepper_max = 1 + rand() % 3;
        }
        screen.put_image(droplet_animation[droplet.animation_ix], droplet.point);
        ++i;
    }
}

auto cache_sin_cos_table() -> void {
    float rad = 0.0f;
    for (int i = 0; i < SZ_DEG_GRANULARITY; ++i) {
        cosine[i] = cosf(rad);
        sine[i] = sinf(rad);
        rad += M_PI * 2.0 / SZ_DEG_GRANULARITY;
    }
}

auto set_center_pos(const Image &screen, const Image &source, int cos_sin_ix, int x_dir, int y_dir) -> Point {
    Point point {
        static_cast<uint16_t>(screen.area().w_mid() - source.area().w_mid() + HEART_RADIUS * cosine[cos_sin_ix] * x_dir), 
        static_cast<uint16_t>(screen.area().h_mid() - source.area().h_mid()  + HEART_RADIUS * sine[cos_sin_ix] * y_dir)
    };
    return point;
}

auto delay_until_mspf(const TimePointSysClock &start) -> void {

    TimePointSysClock end = SysClock::now();
    int delay = MSPF - chr::duration_cast<chr::milliseconds>(end - start).count();
    if (delay > 0) std::this_thread::sleep_for(chr::milliseconds(delay));
}

auto reset_wave_colors(Uptr_color &wave_averages, size_t sz_wave) -> void {
     for (int i = 0; i < sz_wave; ++i) 
        wave_averages[i] = 0;
}

auto set_starting_wave(Image &wave, Uptr_color &wave_averages) -> void {
    Dimension start = wave.area().w() * (wave.area().h() - 1);
    Dimension end = start + wave.area().w();
    Uptr_text &wave_text = wave.raw_text();
    Uptr_text &wave_color = wave.raw_color();
    for (int i = start; i < end; ++i) {
        int r = 11 + rand() % (SZ_WAVE_COLORS - 11);
        wave_text[i] = WAVE_TEXT[r];
        wave_color[i] = WAVE_PALETTE[r];
        wave_averages[i] = r;
    }
}

auto animate_wave(Image &wave, Uptr_color &wave_averages) -> void {
    Uptr_text &wave_text = wave.raw_text();
    Uptr_text &wave_color = wave.raw_color();
    Dimension sz_wave = wave.area().size();
    Dimension wave_w = wave.area().w();
    for (Dimension i = 0; i < sz_wave - wave_w; ++i) {
        int down = i + wave_w;
        int j = (wave_averages[down % sz_wave] + 
                wave_averages[(down  - 1) % sz_wave] + 
                wave_averages[(down  + 1) % sz_wave] + 
                wave_averages[(down  + wave_w) % sz_wave]) / 4.00625;
        wave_averages[i] = j;
        wave_text[i] = WAVE_TEXT[j];
        wave_color[i] = WAVE_PALETTE[j];
    }    
}