#include "../include/Pixel.h"
#include <vector>
#include <fstream>
#include <png++/png.hpp>
#include <webp/encode.h>
#include <webp/decode.h>

Pixel::Pixel() : r(0), g(0), b(0) {}
Pixel::Pixel(int r, int g, int b) : r(r), g(g), b(b) {}
bool Pixel::operator==(const Pixel& other) const {
    return r == other.r && g == other.g && b == other.b;
}


Image::Image() : width(0), height(0) {}
Image::Image(size_t w, size_t h) : width(w), height(h) {}
Image::Image(const std::vector<Pixel>& px, size_t w, size_t h)
    : pixels(px), width(w), height(h) {}

DualImage ReadImageFromPNG(const char *filename) {
    png::image<png::rgb_pixel> image(filename);
    size_t width = image.get_width();
    size_t height = image.get_height();
    std::vector<Pixel> pixels, errorPixels;
    pixels.reserve(width * height);
    errorPixels.reserve(width * height);

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            png::rgb_pixel px = image[y][x];
            pixels.emplace_back(px.red/10, px.green/10, px.blue/10);
            errorPixels.emplace_back(px.red % 10, px.green % 10, px.blue % 10);
        }
    }
    return DualImage(
        Image{pixels, width, height},
        Image{errorPixels, width, height});
}


void WriteImageToPNG(const char *filename, const DualImage &img) {
    png::image<png::rgb_pixel> png_img(
        img.MainImage.width,
        img.MainImage.height);

    size_t width = img.MainImage.width;
    size_t height = img.MainImage.height;

    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            const Pixel& p = img.MainImage.pixels[y * width + x];
            const Pixel& e = img.ErrorImage.pixels[y * width + x];
            png_img[y][x] = png::rgb_pixel(
                static_cast<uint8_t>(p.r*10 + e.r),
                static_cast<uint8_t>(p.g*10 + e.g),
                static_cast<uint8_t>(p.b*10 + e.b)
            );
        }
    }
    png_img.write(filename);
}


void WriteImageToWebP(const char* filename, const DualImage& image, float quality) {
    // Prepare raw RGB data
    std::vector<uint8_t> rgb;
    size_t height = image.MainImage.height;
    size_t width = image.MainImage.width;
    rgb.reserve(width * height * 3);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            const Pixel& p = image.MainImage.pixels[y * width + x];
            const Pixel& e = image.ErrorImage.pixels[y * width + x];
            rgb.push_back(static_cast<uint8_t>(p.r*10 + e.r));
            rgb.push_back(static_cast<uint8_t>(p.g*10 + e.g));
            rgb.push_back(static_cast<uint8_t>(p.b*10 + e.b));
        }
    }

    // Encode to WebP (lossy)
    uint8_t* output = nullptr;
    size_t output_size = WebPEncodeRGB(rgb.data(), width, height, width * 3, quality, &output);
    if (output_size == 0) throw std::runtime_error("WebP encoding failed.");

    // Write to file
    std::ofstream ofs(filename, std::ios::binary);
    ofs.write(reinterpret_cast<const char*>(output), output_size);
    ofs.close();

    WebPFree(output);
}





DualImage ReadImageFromWebP(const char* filename) {
    // Read file into memory
    std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
    if (!ifs) throw std::runtime_error("Could not open WebP file.");
    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(size);
    if (!ifs.read(reinterpret_cast<char*>(buffer.data()), size))
        throw std::runtime_error("Failed to read WebP file.");

    // Decode WebP
    int width = 0, height = 0;
    uint8_t* rgb = WebPDecodeRGB(buffer.data(), buffer.size(), &width, &height);
    if (!rgb) throw std::runtime_error("WebP decoding failed.");

    // Fill Image struct
    Image imgMain, imgError;
    imgMain.width = imgError.width = width;
    imgMain.height = imgError.height = height;
    imgMain.pixels.reserve(width * height);
    for (int i = 0; i < width * height; ++i) {
        imgMain.pixels.push_back(Pixel{rgb[i*3]/10, rgb[i*3+1]/10, rgb[i*3+2]/10});
        imgError.pixels.push_back(Pixel{rgb[i*3]%10, rgb[i*3+1]%10, rgb[i*3+2]%10});
    }
    WebPFree(rgb);
    return DualImage(imgMain, imgError);
}




