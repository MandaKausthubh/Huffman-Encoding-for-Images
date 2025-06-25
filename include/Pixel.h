#ifndef PIXEL_H
#define PIXEL_H

#include <unordered_map>
#include <vector>

struct Pixel {
    int r, g, b;

    // Constructors
    Pixel();
    Pixel(int r, int g, int b);
};

namespace std {
    template<> struct hash<Pixel> {
        size_t operator()(const Pixel&) const;
    };
}


// Image structure to hold pixel data
struct Image {
    std::vector<Pixel> pixels;
    size_t width;
    size_t height;

    // Constructors
    Image();
    Image(size_t w, size_t h);
    Image(const std::vector<Pixel>& px, size_t w, size_t h);
};

Image ReadImageFromPNG(const char* filename);
void WriteImageToPNG(const char* filename, const Image& image);

#endif
