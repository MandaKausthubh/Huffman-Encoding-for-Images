#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <memory>
#include <unordered_map>
#include <string>
using namespace std;

#include "Pixel.h"

struct ImageNode {
    Pixel px;
    long long freq;
    shared_ptr<ImageNode> left, right;
    ImageNode(int r, int g, int b) 
        : px(r, g, b), left(nullptr), right(nullptr) {}
    ImageNode(Pixel p, int freq)
        : px(p), freq(freq), left(nullptr), right(nullptr) {}
    ImageNode(shared_ptr<ImageNode> l, shared_ptr<ImageNode> r)
        : px(), left(l), right(r) {}
};

void BuildImageCodeTable(
        shared_ptr<ImageNode> node, const string& str,
        unordered_map<Pixel, string, hash<Pixel>>& codeTableMain
);

void BuildHuffman(
        const vector<Pixel>& pixels,
        unordered_map<Pixel, string, hash<Pixel>>& codeTable,
        unordered_map<string, Pixel>& reverseCodeTable
);

string EncodeImage(
    const vector<Pixel>& pixels,
    unordered_map<Pixel, string, hash<Pixel>>& codeTable,
    unordered_map<string, Pixel>& reverseCodeTable
);

void DecodeImage(
        const string& encoded,
        vector<Pixel>& decodedPixels,
        unordered_map<string, Pixel>& reverseCodeTable
);

#endif
