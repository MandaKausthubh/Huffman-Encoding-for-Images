#include <string>
#include <unordered_map>
#include <fstream>
#include <unordered_map>
#include <string>

#include "../include/IOUtils.h"
#include "../include/Pixel.h"

void read_from_bin_file(
    const std::string& filename,
    std::unordered_map<std::string, Pixel>& decodingTable,
    size_t& height, size_t& width,
    std::string& encodedString
) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) throw std::runtime_error("Could not open file for reading.");

    // 1. Read decoding table size
    size_t tableSize = 0;
    ifs.read(reinterpret_cast<char*>(&tableSize), sizeof(size_t));

    // 2. Read decoding table entries
    decodingTable.clear();
    for (size_t i = 0; i < tableSize; ++i) {
        size_t strLen = 0;
        ifs.read(reinterpret_cast<char*>(&strLen), sizeof(size_t));
        std::string key(strLen, '\0');
        ifs.read(&key[0], strLen);
        int r, g, b;
        ifs.read(reinterpret_cast<char*>(&r), sizeof(int));
        ifs.read(reinterpret_cast<char*>(&g), sizeof(int));
        ifs.read(reinterpret_cast<char*>(&b), sizeof(int));
        decodingTable[key] = Pixel(r, g, b);
    }

    // 3. Read dimensions
    ifs.read(reinterpret_cast<char*>(&height), sizeof(size_t));
    ifs.read(reinterpret_cast<char*>(&width), sizeof(size_t));

    // 4. Read encoded string length and content
    size_t encodedLen = 0;
    ifs.read(reinterpret_cast<char*>(&encodedLen), sizeof(size_t));
    encodedString.resize(encodedLen);
    ifs.read(&encodedString[0], encodedLen);

    ifs.close();
}


void write_to_bin_file(
    const std::string& filename,
    const std::unordered_map<std::string, Pixel>& decodingTable,
    size_t height, size_t width,
    const std::string& encodedString
) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) throw std::runtime_error("Could not open file for writing.");

    // 1. Write decoding table size
    size_t tableSize = decodingTable.size();
    ofs.write(reinterpret_cast<const char*>(&tableSize), sizeof(size_t));

    // 2. Write decoding table entries
    for (const auto& entry : decodingTable) {
        size_t strLen = entry.first.size();
        ofs.write(reinterpret_cast<const char*>(&strLen), sizeof(size_t));
        ofs.write(entry.first.data(), strLen);
        ofs.write(reinterpret_cast<const char*>(&entry.second.r), sizeof(int));
        ofs.write(reinterpret_cast<const char*>(&entry.second.g), sizeof(int));
        ofs.write(reinterpret_cast<const char*>(&entry.second.b), sizeof(int));
    }

    // 3. Write dimensions
    ofs.write(reinterpret_cast<const char*>(&height), sizeof(size_t));
    ofs.write(reinterpret_cast<const char*>(&width), sizeof(size_t));

    // 4. Write encoded string length and content
    size_t encodedLen = encodedString.size();
    ofs.write(reinterpret_cast<const char*>(&encodedLen), sizeof(size_t));
    ofs.write(encodedString.data(), encodedLen);

    ofs.close();
}
