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

namespace egv {

    typedef uint16_t Dimension;
    typedef uint8_t Color;
    typedef uint8_t Mask;       // Convert Mask to bitset on final release
    typedef unsigned char Text;
    typedef std::unique_ptr<Color[]> Uptr_color;
    typedef std::unique_ptr<Mask[]> Uptr_mask;
    typedef std::unique_ptr<Text[]> Uptr_text;

    constexpr int DEG_GRANULARITY = 360;
    float sine[DEG_GRANULARITY], cosine[DEG_GRANULARITY];

    class SmootherSinCosTable {
    public:
        SmootherSinCosTable(uint16_t min, uint16_t max) : min_(min), max_(max), cur_(min), inc_(1) {};
        auto get() const -> const uint16_t { return ix_; }
        auto next() -> uint16_t {
            ix_ = (ix_ + cur_) % DEG_GRANULARITY;
            cur_ += inc_;
            if (cur_ == min_ || cur_ == max_) inc_ *= -1; 
            return ix_;
        }
    private:
        uint16_t min_, max_, cur_, inc_;
        uint16_t ix_{0};
    };

    struct Point { uint16_t x, y; };
    class Dimensions { 
    public:
        Dimensions(Dimension width, Dimension height) : width_(width), height_(height) { set_size(); }
        auto get_width() -> Dimension & { return width_; }
        auto get_height() -> Dimension & { return height_; }
        auto get_cwidth() const -> const Dimension { return width_; }
        auto get_cheight() const -> const Dimension { return height_; }
        auto set_width(Dimension width) -> void { width_ = width; set_size(); }
        auto set_height(Dimension height) -> void { height_ = height; set_size(); }
        auto set(Dimension width, Dimension height) -> void { width_ = width; height_ = height; set_size(); }
        auto size() const -> const Dimension { return size_; }
        auto get_center_width() const -> const Dimension { return width_ / 2; };
        auto get_center_height() const -> const Dimension { return height_ / 2; };

    private:
        uint16_t width_, height_; 
        uint16_t size_;
        auto set_size() -> void { size_ = width_ * height_; } 
    };

    struct Rectangle {
        Point point;
        Dimensions dimensions;
    };

    class Image {
    public:    
        Image() : dimensions_({0, 0}) {}
        Image(Dimensions dimensions, Mask mask = 0x00) : 
            dimensions_(dimensions),
            color_(std::make_unique<Color[]>(dimensions_.size())),
            text_(std::make_unique<Text[]>(dimensions_.size())),
            mask_(std::make_unique<Mask[]>(dimensions_.size())) {
                std::fill_n(mask_.get(), dimensions_.size(), mask);
                std::fill_n(text_.get(), dimensions_.size(), ' ');
                std::fill_n(color_.get(), dimensions_.size(), 0);
        }
        Image(const char *text, const Color color, const Mask mask = 0x00) : Image({static_cast<Dimension>(strlen(text)), 1}, mask) {
            for (int i = 0; i < dimensions_.size(); ++i) {
                color_[i] = color;
                text_[i] = text[i];
            }
        }
        Image(const char *filename) : Image() { load(filename); }
        auto operator=(const Image &rhs) -> Image & = delete;
        auto operator=(Image &&rhs) -> Image &  = delete;

        inline auto dimensions() const -> const Dimensions & { return dimensions_; }

        auto save(const char *filename) -> void {
            std::ofstream file (filename, std::ios::binary);
            file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            file.write(static_cast<const char *>(static_cast<const void*>(&dimensions_)), sizeof(dimensions_.get_cwidth()) + sizeof(dimensions_.get_cheight()));
            file.write(static_cast<const char *>(static_cast<const void*>(color_.get())), dimensions_.size());
            file.write(static_cast<const char *>(static_cast<const void*>(text_.get())), dimensions_.size());
            file.write(static_cast<const char *>(static_cast<const void*>(mask_.get())), dimensions_.size());
        }

        auto load(const char *filename) -> void {
            if (dimensions_.size()) throw;
            std::ifstream file (filename, std::ios::binary);
            file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            Dimension width, height;
            file.read(static_cast<char *>(static_cast<void *>(&width)), sizeof(dimensions_.get_cwidth()));
            file.read(static_cast<char *>(static_cast<void *>(&height)), sizeof(dimensions_.get_cheight()));
            
            dimensions_.set(width, height);
            color_ = std::make_unique<Color[]>(dimensions_.size());             
            file.read(static_cast<char *>(static_cast<void *>(color_.get())), dimensions_.size());
            text_ = std::make_unique<Text[]>(dimensions_.size());             
            file.read(static_cast<char *>(static_cast<void *>(text_.get())), dimensions_.size());
            mask_ = std::make_unique<Mask[]>(dimensions_.size());             
            file.read(static_cast<char *>(static_cast<void *>(mask_.get())), dimensions_.size());
        }
        
        auto debug() -> void {
            std::cout << "\nmask:\n";
            for (int i = 0; i < dimensions_.get_cheight(); ++i) {
                for (int j = 0; j < dimensions_.get_cwidth(); ++j)
                    std::cout << (mask_[i * dimensions_.get_cwidth() + j] == 0x00 ? '0' : '1');
                std::cout << "\n";
            }
            std::cout << "\n\n";

            std::cout << "\ntext:\n";
            for (int i = 0; i < dimensions_.get_cheight(); ++i) {
                for (int j = 0; j < dimensions_.get_cwidth(); ++j)
                    std::cout << text_[i * dimensions_.get_cwidth() + j];
                std::cout << "\n";
            }
            std::cout << "\n\n";    
        }

        auto fill_with_text(const char *text, const Color color) -> void {
            std::fill_n(color_.get(), dimensions_.size(), color);
            for (size_t i = 0, length = strlen(text); i < dimensions_.size(); ++i)
                text_[i] = text[i % length];
        }

        auto get_image(const Image &source, const Point point) -> void {
            int start = point.y * source.dimensions_.get_cwidth() + point.x;
            int add_vertical = source.dimensions_.get_cwidth() - dimensions_.get_cwidth();
            for (int i = 0; i < dimensions_.size();) {
                int ci = start + i;
                color_[i] = source.color_[ci];
                text_[i] = source.text_[ci];
                mask_[i] = source.mask_[ci];
                if (++i % dimensions_.get_cwidth() == 0) start += add_vertical;
            }
        }
    
        auto and_mask(const Image &source, const Point point) -> void {
            int start = point.y * dimensions_.get_cwidth() + point.x;
            int add_vertical = dimensions_.get_cwidth() - source.dimensions_.get_cwidth();
            for (int i = 0; i < source.dimensions_.size();) {
                mask_[start + i] &= source.mask_[i]; 
                if (++i % source.dimensions_.get_cwidth() == 0) start += add_vertical;
            }
        }

        auto or_image(const Image &source, const Point point) -> void {
            int start = point.y * dimensions_.get_cwidth() + point.x;
            int add_vertical = dimensions_.get_cwidth() - source.dimensions_.get_cwidth();
            for (int i = 0; i < source.dimensions_.size();) {
                int ci = start + i;
                mask_[ci] |= source.mask_[i]; 
                if (mask_[ci] == 0x00) {
                    color_[ci] = source.color_[i]; 
                    text_[ci] = source.text_[i];
                } 
                if (++i % source.dimensions_.get_cwidth() == 0) start += add_vertical;
            }
        }

        auto rotate_left() -> void {
            Color t_color = color_[0];
            Text t_text = text_[0];
            Mask t_mask = mask_[0];
            int j = 1;
            for (int i = 0; i < dimensions_.size() - 1; i = j++) {
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
            for (int i = 0; i < dimensions_.size(); ++i) {
                std::cout << c[color_[i]];
                if (i % dimensions_.get_cwidth() == 0) putchar(10); 
                putchar(text_[i]);
            }
            std::cout << "\033[0m";
        }

        auto put_image(const Image &source, const Point point) -> void {
            int start = point.y * dimensions_.get_cwidth() + point.x;
            int add_vertical = dimensions_.get_cwidth() - source.dimensions_.get_cwidth();
            for (int i = 0; i < source.dimensions_.size();) {
                int ci = start + i;
                color_[ci] = source.color_[i]; 
                text_[ci] = source.text_[i];
                mask_[ci] = source.mask_[i];
                if (++i % source.dimensions_.get_cwidth() == 0) start += add_vertical;
            }
        }


    private:
        Dimensions dimensions_;
        Uptr_color color_ {nullptr};
        Uptr_text text_ {nullptr};
        Uptr_mask mask_ {nullptr};
    };


}

#endif 