#include <string>
#include <unordered_map>
#include <fstream>
#include <unordered_map>
#include <string>

#include "../include/Pixel.h"
#include "../include/IOUtils.h"



std::vector<uint8_t> pack_bitstring(const std::string& bitstr) {
    std::vector<uint8_t> bytes((bitstr.size() + 7) / 8, 0);
    for (size_t i = 0; i < bitstr.size(); ++i) {
        if (bitstr[i] == '1') {
            bytes[i / 8] |= (1 << (7 - (i % 8)));
        }
    }
    return bytes;
}

std::string unpack_bitstring(const std::vector<uint8_t>& bytes, size_t bit_count) {
    std::string bitstr;
    bitstr.reserve(bit_count);
    for (size_t i = 0; i < bit_count; ++i) {
        uint8_t byte = bytes[i / 8];
        uint8_t bit = (byte >> (7 - (i % 8))) & 1;
        bitstr.push_back(bit ? '1' : '0');
    }
    return bitstr;
}

void read_from_bin_file(
    const std::string& filename,
    std::unordered_map<std::string, Pixel>& decodingTableMain,
    std::unordered_map<std::string, Pixel>& decodingTableError,
    size_t& height, size_t& width,
    std::string& encodedStringMain,
    std::string& encodedStringError
) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) throw std::runtime_error("Could not open file for reading.");

    // 1. Read decoding table sizes
    size_t tableSizeMain = 0;
    size_t tableSizeError = 0;
    ifs.read(reinterpret_cast<char*>(&tableSizeMain), sizeof(size_t));
    ifs.read(reinterpret_cast<char*>(&tableSizeError), sizeof(size_t));

    // 2. Read main decoding table entries
    decodingTableMain.clear();
    for (size_t i = 0; i < tableSizeMain; ++i) {
        size_t strLen = 0;
        ifs.read(reinterpret_cast<char*>(&strLen), sizeof(size_t));
        std::string key(strLen, '\0');
        ifs.read(&key[0], strLen);
        int r, g, b;
        ifs.read(reinterpret_cast<char*>(&r), sizeof(int));
        ifs.read(reinterpret_cast<char*>(&g), sizeof(int));
        ifs.read(reinterpret_cast<char*>(&b), sizeof(int));
        decodingTableMain[key] = Pixel(r, g, b);
    }

    // 3. Read error decoding table entries
    decodingTableError.clear();
    for (size_t i = 0; i < tableSizeError; ++i) {
        size_t strLen = 0;
        ifs.read(reinterpret_cast<char*>(&strLen), sizeof(size_t));
        std::string key(strLen, '\0');
        ifs.read(&key[0], strLen);
        int r, g, b;
        ifs.read(reinterpret_cast<char*>(&r), sizeof(int));
        ifs.read(reinterpret_cast<char*>(&g), sizeof(int));
        ifs.read(reinterpret_cast<char*>(&b), sizeof(int));
        decodingTableError[key] = Pixel(r, g, b);
    }

    // 4. Read dimensions
    ifs.read(reinterpret_cast<char*>(&height), sizeof(size_t));
    ifs.read(reinterpret_cast<char*>(&width), sizeof(size_t));

    // 5. Read encoded bit-strings as packed bits
    // Main
    size_t bitCountMain = 0, byteCountMain = 0;
    ifs.read(reinterpret_cast<char*>(&bitCountMain), sizeof(size_t));
    ifs.read(reinterpret_cast<char*>(&byteCountMain), sizeof(size_t));
    std::vector<uint8_t> packedMain(byteCountMain, 0);
    ifs.read(reinterpret_cast<char*>(packedMain.data()), byteCountMain);
    encodedStringMain = unpack_bitstring(packedMain, bitCountMain);

    // Error
    size_t bitCountError = 0, byteCountError = 0;
    ifs.read(reinterpret_cast<char*>(&bitCountError), sizeof(size_t));
    ifs.read(reinterpret_cast<char*>(&byteCountError), sizeof(size_t));
    std::vector<uint8_t> packedError(byteCountError, 0);
    ifs.read(reinterpret_cast<char*>(packedError.data()), byteCountError);
    encodedStringError = unpack_bitstring(packedError, bitCountError);

    ifs.close();
}


void write_to_bin_file(
    const std::string& filename,
    const std::unordered_map<std::string, Pixel>& decodingTableMain,
    const std::unordered_map<std::string, Pixel>& decodingTableError,
    size_t height, size_t width,
    const std::string& encodedStringMain,
    const std::string& encodedStringError
) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) throw std::runtime_error("Could not open file for writing.");

    // 1. Write decoding table sizes
    size_t tableSizeMain = decodingTableMain.size();
    size_t tableSizeError = decodingTableError.size();
    ofs.write(reinterpret_cast<const char*>(&tableSizeMain), sizeof(size_t));
    ofs.write(reinterpret_cast<const char*>(&tableSizeError), sizeof(size_t));

    // 2. Write main decoding table entries
    for (const auto& entry : decodingTableMain) {
        size_t strLen = entry.first.size();
        ofs.write(reinterpret_cast<const char*>(&strLen), sizeof(size_t));
        ofs.write(entry.first.data(), strLen);
        ofs.write(reinterpret_cast<const char*>(&entry.second.r), sizeof(int));
        ofs.write(reinterpret_cast<const char*>(&entry.second.g), sizeof(int));
        ofs.write(reinterpret_cast<const char*>(&entry.second.b), sizeof(int));
    }

    // 3. Write error decoding table entries
    for (const auto& entry : decodingTableError) {
        size_t strLen = entry.first.size();
        ofs.write(reinterpret_cast<const char*>(&strLen), sizeof(size_t));
        ofs.write(entry.first.data(), strLen);
        ofs.write(reinterpret_cast<const char*>(&entry.second.r), sizeof(int));
        ofs.write(reinterpret_cast<const char*>(&entry.second.g), sizeof(int));
        ofs.write(reinterpret_cast<const char*>(&entry.second.b), sizeof(int));
    }

    // 4. Write dimensions
    ofs.write(reinterpret_cast<const char*>(&height), sizeof(size_t));
    ofs.write(reinterpret_cast<const char*>(&width), sizeof(size_t));

    // 5. Write encoded bit-strings as packed bits
    // Main
    size_t bitCountMain = encodedStringMain.size();
    std::vector<uint8_t> packedMain = pack_bitstring(encodedStringMain);
    ofs.write(reinterpret_cast<const char*>(&bitCountMain), sizeof(size_t)); // number of bits
    size_t byteCountMain = packedMain.size();
    ofs.write(reinterpret_cast<const char*>(&byteCountMain), sizeof(size_t)); // number of bytes
    ofs.write(reinterpret_cast<const char*>(packedMain.data()), byteCountMain);

    // Error
    size_t bitCountError = encodedStringError.size();
    std::vector<uint8_t> packedError = pack_bitstring(encodedStringError);
    ofs.write(reinterpret_cast<const char*>(&bitCountError), sizeof(size_t)); // number of bits
    size_t byteCountError = packedError.size();
    ofs.write(reinterpret_cast<const char*>(&byteCountError), sizeof(size_t)); // number of bytes
    ofs.write(reinterpret_cast<const char*>(packedError.data()), byteCountError);

    ofs.close();
}
