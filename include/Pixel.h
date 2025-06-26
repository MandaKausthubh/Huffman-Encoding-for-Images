#ifndef PIXEL_H
#define PIXEL_H

#include <unordered_map>
#include <vector>

struct Pixel {
    int r, g, b;

    // Constructors
    Pixel();
    Pixel(int r, int g, int b);

    bool operator==(const Pixel& other) const;
};

namespace std {
    template <>
    struct hash<Pixel> {
        size_t operator()(const Pixel& p) const {
            return ((std::hash<int>()(p.r)
                    ^ (std::hash<int>()(p.g) << 1)) >> 1)
                    ^ (std::hash<int>()(p.b) << 1);
        }
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

struct DualImage {
    Image MainImage;
    Image ErrorImage;

    DualImage() : MainImage(), ErrorImage() {}
    DualImage(const Image& main, const Image& error)
        : MainImage(main), ErrorImage(error) {}
    DualImage(
        const std::vector<Pixel>& mainPixels,
        const std::vector<Pixel>& errorPixel,
        size_t w, size_t h)
        : MainImage(mainPixels, w, h), ErrorImage(errorPixel, w, h) {}
};

DualImage ReadImageFromPNG(const char* filename);
void WriteImageToPNG(const char* filename, const DualImage& image);
DualImage ReadImageFromWebP(const char* filename);
void WriteImageToWebP(const char* filename, const DualImage& image, float quality = 90.0f);

#endif
