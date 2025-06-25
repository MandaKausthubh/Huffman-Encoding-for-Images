#ifndef PNG_UTILS_H
#define PNG_UTILS_H

#include <vector>
#include <string>

struct Pixel {
    int r, g, b;
    Pixel(int r, int g, int b) : r(r), g(g), b(b) {}
};

std::vector<std::vector<int>> loadPNG(
    const std::string& filename,
    size_t& width,
    size_t& height
);

void savePNG(
    const std::string& filename,
    const std::vector<std::vector<int>>& pixels,
    size_t width,
    size_t height
);

#endif
