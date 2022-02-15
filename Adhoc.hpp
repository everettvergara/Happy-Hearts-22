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


    struct Point { uint16_t x, y; };
    class Dimensions { 
    public:
        Dimensions(Dimension width, Dimension height) : width_(width), height_(height) {}
        auto get_width() -> Dimension & { return width_; }
        auto get_height() -> Dimension & { return height_; }
        auto get_cwidth() const -> const Dimension { return width_; }
        auto get_cheight() const -> const Dimension { return height_; }
        auto set_width(Dimension width) -> void { width_ = width; set_size(); }
        auto set_height(Dimension height) -> void { height_ = height; set_size(); }
        auto set(Dimension width, Dimension height) -> void { width_ = width; height_ = height; set_size(); }
        auto size() const -> const Dimension { return size_; }
    private:
        uint16_t width_, height_; 
        uint16_t size_;
        auto set_size() -> void { size_ = width_ * height_; } 
    };

    struct Rectangle {
        Point p;
        Dimensions d;
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
            }
        auto operator=(const Image &rhs) -> Image & = delete;
        auto operator=(Image &&rhs) -> Image &  = delete;

        // Getters
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

    private:
        Dimensions dimensions_;
        Uptr_color color_ {nullptr};
        Uptr_text text_ {nullptr};
        Uptr_mask mask_ {nullptr};
    };


}

#endif 