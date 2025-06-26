#include "../include/Pixel.h"
#include <vector>
#include <png++/png.hpp>

Pixel::Pixel() : r(0), g(0), b(0) {}
Pixel::Pixel(int r, int g, int b) : r(r), g(g), b(b) {}
bool Pixel::operator==(const Pixel& other) const {
    return r == other.r && g == other.g && b == other.b;
}


Image::Image() : width(0), height(0) {}
Image::Image(size_t w, size_t h) : width(w), height(h) {}
Image::Image(const std::vector<Pixel>& px, size_t w, size_t h)
    : pixels(px), width(w), height(h) {}

Image ReadImageFromPNG(const char *filename) {
    png::image<png::rgb_pixel> image(filename);
    size_t width = image.get_width();
    size_t height = image.get_height();
    std::vector<Pixel> pixels;
    pixels.reserve(width * height);

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            png::rgb_pixel px = image[y][x];
            pixels.emplace_back(px.red, px.green, px.blue);
        }
    }
    return Image{pixels, width, height};
}


void WriteImageToPNG(const char *filename, const Image &img) {
    png::image<png::rgb_pixel> png_img(img.width, img.height);

    for (size_t y = 0; y < img.height; ++y) {
        for (size_t x = 0; x < img.width; ++x) {
            const Pixel& p = img.pixels[y * img.width + x];
            png_img[y][x] = png::rgb_pixel(
                static_cast<uint8_t>(p.r),
                static_cast<uint8_t>(p.g),
                static_cast<uint8_t>(p.b)
            );
        }
    }
    png_img.write(filename);
}

