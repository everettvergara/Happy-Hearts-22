#include <algorithm>
#include "Image.h"

using namespace g80;

Image::Image() : 
    area_({0, 0}) {
}

Image::Image(Area area, Mask mask) : 
    area_(area),
    color_(std::make_unique<Color[]>(area_.size())),
    text_(std::make_unique<Text[]>(area_.size())),
    mask_(std::make_unique<Mask[]>(area_.size())) {
    
    std::fill_n(mask_.get(), area_.size(), mask);
    std::fill_n(text_.get(), area_.size(), ' ');
    std::fill_n(color_.get(), area_.size(), 0);
}

Image::Image(const char *text, const Color color, const Mask mask) : 
    Image({static_cast<Dimension>(strlen(text)), 1}, mask) {
    for (int i = 0; i < area_.size(); ++i) {
        color_[i] = color;
        text_[i] = text[i];
    }
}

Image::Image(const char *filename) : Image() { 
    load(filename);
}

auto Image::raw_color() -> Uptr_color & { 
    return color_; 
}

auto Image::raw_text() -> Uptr_text & { 
    return text_; 
}

auto Image::raw_mask() -> Uptr_mask & { 
    return mask_; 
}

auto Image::area() const -> const Area & { 
    return area_; 
}

auto Image::save(const char *filename) -> void {
    std::ofstream file (filename, std::ios::binary);
    file.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    file.write(static_cast<const char *>(static_cast<const void*>(&area_)), sizeof(area_.w()) + sizeof(area_.h()));
    file.write(static_cast<const char *>(static_cast<const void*>(color_.get())), area_.size());
    file.write(static_cast<const char *>(static_cast<const void*>(text_.get())), area_.size());
    file.write(static_cast<const char *>(static_cast<const void*>(mask_.get())), area_.size());
}

auto Image::load(const char *filename) -> void {
    if (area_.size()) throw;
    std::ifstream file (filename, std::ios::binary);
    file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    
    Dimension w, h;
    file.read(static_cast<char *>(static_cast<void *>(&w)), sizeof(area_.w()));
    file.read(static_cast<char *>(static_cast<void *>(&h)), sizeof(area_.h()));
    
    area_.set(w, h);
    color_ = std::make_unique<Color[]>(area_.size());             
    file.read(static_cast<char *>(static_cast<void *>(color_.get())), area_.size());
    text_ = std::make_unique<Text[]>(area_.size());             
    file.read(static_cast<char *>(static_cast<void *>(text_.get())), area_.size());
    mask_ = std::make_unique<Mask[]>(area_.size());             
    file.read(static_cast<char *>(static_cast<void *>(mask_.get())), area_.size());
}

auto Image::debug() -> void {
    std::cout << "\nmask:\n";
    for (int i = 0; i < area_.h(); ++i) {
        for (int j = 0; j < area_.w(); ++j)
            std::cout << (mask_[i * area_.w() + j] == 0x00 ? '0' : '1');
        std::cout << "\n";
    }
    std::cout << "\n\n";

    std::cout << "\ntext:\n";
    for (int i = 0; i < area_.h(); ++i) {
        for (int j = 0; j < area_.w(); ++j)
            std::cout << text_[i * area_.w() + j];
        std::cout << "\n";
    }
    std::cout << "\n\n";    
}

auto Image::fill_with_text(const char *text, const Color color) -> void {
    std::fill_n(color_.get(), area_.size(), color);
    for (size_t i = 0, length = strlen(text); i < area_.size(); ++i)
        text_[i] = text[i % length];
}

auto Image::get_image(const Image &source, const Point point) -> void {
    int start {point.y * source.area_.w() + point.x};
    int add_vertical {source.area_.w() - area_.w()};
    for (int i = 0; i < area_.size();) {
        int ci = start + i;
        color_[i] = source.color_[ci];
        text_[i] = source.text_[ci];
        mask_[i] = source.mask_[ci];
        if (++i % area_.w() == 0) 
            start += add_vertical;
    }
}

auto Image::and_mask(const Image &source, const Point point) -> void {
    int start { point.y * area_.w() + point.x };
    int add_vertical { area_.w() - source.area_.w() };
    for (int i = 0; i < source.area_.size();) {
        mask_[start + i] &= source.mask_[i]; 
        if (++i % source.area_.w() == 0) 
            start += add_vertical;
    }
}

auto Image::or_image(const Image &source, const Point point) -> void {
    int start { point.y * area_.w() + point.x };
    int add_vertical { area_.w() - source.area_.w() };
    for (int i = 0; i < source.area_.size();) {
        int ci = start + i;
        mask_[ci] |= source.mask_[i]; 
        if (mask_[ci] == 0x00) {
            color_[ci] = source.color_[i]; 
            text_[ci] = source.text_[i];
        } 
        if (++i % source.area_.w() == 0) 
            start += add_vertical;
    }
}

auto Image::rotate_left() -> void {
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

auto Image::put_image(const Image &source, const Point point) -> void {
    int start { point.y * area_.w() + point.x };
    int add_vertical { area_.w() - source.area_.w() };
    for (int i = 0; i < source.area_.size();) {
        int ci = start + i;
        color_[ci] = source.color_[i]; 
        text_[ci] = source.text_[i];
        mask_[ci] = source.mask_[i];

        if (++i % source.area_.w() == 0) 
            start += add_vertical;
    }
}

auto Image::show() -> void {
    static const size_t max_color {8};
    static std::string c[max_color] { "\033[30m", "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m", "\033[37m" };
    
    std::cout << "\033[2J";
    for (int i = 0; i < area_.size(); ++i) {
        std::cout << c[color_[i]];
        if (i % area_.w() == 0) putchar(10); 
        putchar(text_[i]);
    }
    std::cout << "\033[0m";
}
