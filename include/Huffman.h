#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "Pixel.h"
#include <unordered_map>
#include <string>
#include <vector>

struct ImageNode { ... };
void build_image_code_table(...);
std::string huffman_encode(...);
std::vector<Pixel> huffman_decode(...);

#endif
