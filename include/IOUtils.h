#include "Pixel.h"
#include <unordered_map>
#include <string>

void write_to_bin_file(const std::string& filename,
                       const std::unordered_map<std::string, Pixel>& decodingTable,
                       size_t height, size_t width,
                       const std::string& encodedString);
void read_from_bin_file(...); // Optional, for completeness
