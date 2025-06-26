#include "Pixel.h"
#include <unordered_map>
#include <string>

void write_to_bin_file(
    const std::string& filename,
    const std::unordered_map<std::string, Pixel>& decodingTableMain,
    const std::unordered_map<std::string, Pixel>& decodingTableError,
    size_t height, size_t width,
    const std::string& encodedStringMain,
    const std::string& encodedStringError
);

void read_from_bin_file(
    const std::string& filename,
    std::unordered_map<std::string, Pixel>& decodingTableMain,
    std::unordered_map<std::string, Pixel>& decodingTableError,
    size_t& height, size_t& width,
    std::string& encodedStringMain,
    std::string& encodedStringError
);
