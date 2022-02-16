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

namespace egv {

    typedef uint8_t Color;
    typedef uint8_t Mask;       // Convert Mask to bitset on final release
    typedef unsigned char Text;
    typedef std::unique_ptr<Color[]> Uptr_color;
    typedef std::unique_ptr<Mask[]> Uptr_mask;
    typedef std::unique_ptr<Text[]> Uptr_text;

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

    class Image {
    public:    
        Image() : area_({0, 0}) {}
        Image(Area area, Mask mask = 0x00) : 
            area_(area),
            color_(std::make_unique<Color[]>(area_.size())),
            text_(std::make_unique<Text[]>(area_.size())),
            mask_(std::make_unique<Mask[]>(area_.size())) {
                std::fill_n(mask_.get(), area_.size(), mask);
                std::fill_n(text_.get(), area_.size(), ' ');
                std::fill_n(color_.get(), area_.size(), 0);
        }
        Image(const char *text, const Color color, const Mask mask = 0x00) : Image({static_cast<Dimension>(strlen(text)), 1}, mask) {
            for (int i = 0; i < area_.size(); ++i) {
                color_[i] = color;
                text_[i] = text[i];
            }
        }
        Image(const char *filename) : Image() { load(filename); }
        auto operator=(const Image &rhs) -> Image & = delete;
        auto operator=(Image &&rhs) -> Image &  = delete;

        auto get_raw_color() -> Uptr_color & { return color_; }
        auto get_raw_text() -> Uptr_text & { return text_; }
        auto get_raw_mask() -> Uptr_mask & { return mask_; }

        inline auto area() const -> const Area & { return area_; }

        auto save(const char *filename) -> void {
            std::ofstream file (filename, std::ios::binary);
            file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            file.write(static_cast<const char *>(static_cast<const void*>(&area_)), sizeof(area_.width()) + sizeof(area_.height()));
            file.write(static_cast<const char *>(static_cast<const void*>(color_.get())), area_.size());
            file.write(static_cast<const char *>(static_cast<const void*>(text_.get())), area_.size());
            file.write(static_cast<const char *>(static_cast<const void*>(mask_.get())), area_.size());
        }

        auto load(const char *filename) -> void {
            if (area_.size()) throw;
            std::ifstream file (filename, std::ios::binary);
            file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            Dimension width, height;
            file.read(static_cast<char *>(static_cast<void *>(&width)), sizeof(area_.width()));
            file.read(static_cast<char *>(static_cast<void *>(&height)), sizeof(area_.height()));
            
            area_.set(width, height);
            color_ = std::make_unique<Color[]>(area_.size());             
            file.read(static_cast<char *>(static_cast<void *>(color_.get())), area_.size());
            text_ = std::make_unique<Text[]>(area_.size());             
            file.read(static_cast<char *>(static_cast<void *>(text_.get())), area_.size());
            mask_ = std::make_unique<Mask[]>(area_.size());             
            file.read(static_cast<char *>(static_cast<void *>(mask_.get())), area_.size());
        }
        
        auto debug() -> void {
            std::cout << "\nmask:\n";
            for (int i = 0; i < area_.height(); ++i) {
                for (int j = 0; j < area_.width(); ++j)
                    std::cout << (mask_[i * area_.width() + j] == 0x00 ? '0' : '1');
                std::cout << "\n";
            }
            std::cout << "\n\n";

            std::cout << "\ntext:\n";
            for (int i = 0; i < area_.height(); ++i) {
                for (int j = 0; j < area_.width(); ++j)
                    std::cout << text_[i * area_.width() + j];
                std::cout << "\n";
            }
            std::cout << "\n\n";    
        }

        auto fill_with_text(const char *text, const Color color) -> void {
            std::fill_n(color_.get(), area_.size(), color);
            for (size_t i = 0, length = strlen(text); i < area_.size(); ++i)
                text_[i] = text[i % length];
        }

        auto get_image(const Image &source, const Point point) -> void {
            int start = point.y * source.area_.width() + point.x;
            int add_vertical = source.area_.width() - area_.width();
            for (int i = 0; i < area_.size();) {
                int ci = start + i;
                color_[i] = source.color_[ci];
                text_[i] = source.text_[ci];
                mask_[i] = source.mask_[ci];
                if (++i % area_.width() == 0) start += add_vertical;
            }
        }
    
        auto and_mask(const Image &source, const Point point) -> void {
            int start = point.y * area_.width() + point.x;
            int add_vertical = area_.width() - source.area_.width();
            for (int i = 0; i < source.area_.size();) {
                mask_[start + i] &= source.mask_[i]; 
                if (++i % source.area_.width() == 0) start += add_vertical;
            }
        }

        auto or_image(const Image &source, const Point point) -> void {
            int start = point.y * area_.width() + point.x;
            int add_vertical = area_.width() - source.area_.width();
            for (int i = 0; i < source.area_.size();) {
                int ci = start + i;
                mask_[ci] |= source.mask_[i]; 
                if (mask_[ci] == 0x00) {
                    color_[ci] = source.color_[i]; 
                    text_[ci] = source.text_[i];
                } 
                if (++i % source.area_.width() == 0) start += add_vertical;
            }
        }

        auto rotate_left() -> void {
            Color t_color = color_[0];
            Text t_text = text_[0];
            Mask t_mask = mask_[0];
            int j = 1;
            for (int i = 0; i < area_.size() - 1; i = j++) {
                color_[i] = color_[j];
                mask_[i] = mask_[j];
                text_[i] = text_[j];
            }
            color_[j] = t_color;
            mask_[j] = t_mask;
            text_[j] = t_text;
        }

        auto show() -> void {
            static const size_t max_color = 8;
            static std::string c[max_color] { "\033[30m", "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m", "\033[37m" };
            std::cout << "\033[2J";
            for (int i = 0; i < area_.size(); ++i) {
                std::cout << c[color_[i]];
                if (i % area_.width() == 0) putchar(10); 
                putchar(text_[i]);
            }
            std::cout << "\033[0m";
        }

        auto put_image(const Image &source, const Point point) -> void {
            int start = point.y * area_.width() + point.x;
            int add_vertical = area_.width() - source.area_.width();
            for (int i = 0; i < source.area_.size();) {
                int ci = start + i;
                color_[ci] = source.color_[i]; 
                text_[ci] = source.text_[i];
                mask_[ci] = source.mask_[i];
                if (++i % source.area_.width() == 0) start += add_vertical;
            }
        }


    private:
        Area area_;
        Uptr_color color_{nullptr};
        Uptr_text text_{nullptr};
        Uptr_mask mask_{nullptr};
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