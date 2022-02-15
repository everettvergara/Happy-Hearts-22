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

    class Image {
    public:    
        Image() {}
        Image(Dimension w, Dimension h) : 
            w_(w), h_(h), s_(w_ * h_),
            color_(std::make_unique<Color[]>(s_)),
            text_(std::make_unique<Text[]>(s_)),
            mask_(std::make_unique<Mask[]>(s_)) {}
        auto operator=(const Image &rhs) -> Image & = delete;
        auto operator=(Image &&rhs) -> Image &  = delete;

        // Getters and Setters
        inline auto size() const -> const Dimension { return s_; }
        inline auto get_h() const -> const Dimension { return h_; }
        inline auto get_w() const -> const Dimension { return w_; }

        auto save_image(const char *filename, const Image &image) -> void {
            std::ofstream file (filename, std::ios::binary);
            file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            file.write(static_cast<const char *>(static_cast<const void*>(&image)), sizeof(w_) + sizeof(h_));
            file.write(static_cast<const char *>(static_cast<const void*>(color_.get())), size());
            file.write(static_cast<const char *>(static_cast<const void*>(text_.get())), size());
            file.write(static_cast<const char *>(static_cast<const void*>(mask_.get())), size());
        }

        auto load_image(const char *filename) -> void {
            if (s_) throw;
            std::ifstream file (filename, std::ios::binary);
            file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
            file.read(static_cast<char *>(static_cast<void *>(&w_)), sizeof(w_));
            file.read(static_cast<char *>(static_cast<void *>(&h_)), sizeof(h_));
            s_ = w_ * h_;
            color_ = std::make_unique<Color[]>(s_);             
            file.read(static_cast<char *>(static_cast<void *>(color_.get())), s_);
            text_ = std::make_unique<Text[]>(s_);             
            file.read(static_cast<char *>(static_cast<void *>(text_.get())), s_);
            mask_ = std::make_unique<Mask[]>(s_);             
            file.read(static_cast<char *>(static_cast<void *>(mask_.get())), s_);
        }

        auto debug() -> void {
            std::cout << "\nmask:\n";
            for (int i = 0; i < h_; ++i) {
                for (int j = 0; j < w_; ++j) {
                    std::cout << (mask_[i * w_ + j] == 0x00 ? '0' : '1');
                }
                std::cout << "\n";
            }
            std::cout << "\n\n";

            std::cout << "\ntext:\n";
            for (int i = 0; i < h_; ++i) {
                for (int j = 0; j < w_; ++j) {
                    std::cout << text_[i* w_ + j];
                }
                std::cout << "\n";
            }
            std::cout << "\n\n";    
        }

    private:
        Dimension w_{0}, h_{0}, s_{0};
        Uptr_color color_ {nullptr};
        Uptr_text text_ {nullptr};
        Uptr_mask mask_ {nullptr};
    };


    struct Coordinate {
        Dimension x, y;
    };

    struct Rectangle {
        Coordinate coordinate;
        Dimension w, h;
        inline auto size() const -> const Dimension {return w * h;}
    };

}

#endif 