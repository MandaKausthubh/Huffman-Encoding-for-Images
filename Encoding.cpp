#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <png++/png.hpp>

struct Pixel {
    int r, g, b;
    Pixel() : r(0), g(0), b(0) {}
    Pixel(int r, int g, int b) : r(r), g(g), b(b) {}
    friend std::ostream& operator<<(std::ostream& os, const Pixel& p) {
        os.write(reinterpret_cast<const char*>(&p.r), sizeof(int));
        os.write(reinterpret_cast<const char*>(&p.g), sizeof(int));
        os.write(reinterpret_cast<const char*>(&p.b), sizeof(int));
        return os;
    }
    friend std::istream& operator>>(std::istream& is, Pixel& p) {
        is.read(reinterpret_cast<char*>(&p.r), sizeof(int));
        is.read(reinterpret_cast<char*>(&p.g), sizeof(int));
        is.read(reinterpret_cast<char*>(&p.b), sizeof(int));
        return is;
    }
};

struct Image {
    std::vector<Pixel> pixels;
    size_t width;
    size_t height;
    Image() : width(0), height(0) {}
    Image(size_t w, size_t h) : width(w), height(h) {}
    Image(const std::vector<Pixel>& px, size_t w, size_t h) 
        : pixels(px), width(w), height(h) {}
};

Image read_png_to_image(const std::string& filename) {
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

void write_to_bin_file(const std::string& filename,
                       const std::unordered_map<std::string, Pixel>& decodingTable,
                       size_t height, size_t width,
                       const std::string& encodedString) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        std::cerr << "Could not open file for writing.\n";
        return;
    }

    // 1. Write decoding table size
    size_t tableSize = decodingTable.size();
    ofs.write(reinterpret_cast<const char*>(&tableSize), sizeof(size_t));

    // 2. Write decoding table: for each entry, write string length, string, then Pixel
    for (const auto& entry : decodingTable) {
        size_t strLen = entry.first.size();
        ofs.write(reinterpret_cast<const char*>(&strLen), sizeof(size_t));
        ofs.write(entry.first.c_str(), strLen);
        ofs << entry.second; // Pixel as binary
    }

    // 3. Write dimensions
    ofs.write(reinterpret_cast<const char*>(&height), sizeof(size_t));
    ofs.write(reinterpret_cast<const char*>(&width), sizeof(size_t));

    // 4. Write encoded string length and data
    size_t encodedLen = encodedString.size();
    ofs.write(reinterpret_cast<const char*>(&encodedLen), sizeof(size_t));
    ofs.write(encodedString.c_str(), encodedLen);

    ofs.close();
    std::cout << "Data written to " << filename << std::endl;
}
