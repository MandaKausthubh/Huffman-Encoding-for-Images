#include <queue>
#include <vector>
#include <string>
#include <memory>
#include "../include/Pixel.h"

using namespace std;

struct ImageNode {
    Pixel px;
    long long freq;
    shared_ptr<ImageNode> left, right;
    ImageNode(Pixel p) : px(p), left(nullptr), right(nullptr) {}
    ImageNode(shared_ptr<ImageNode> l, shared_ptr<ImageNode> r)
        : px(Pixel(0, 0, 0)), left(l), right(r) {}
};

struct Comparte {
    bool operator()(shared_ptr<ImageNode> const& a, shared_ptr<ImageNode> const& b) {
        return a->freq > b->freq;
    }
};

void BuildImageCodeTable(
        shared_ptr<ImageNode> node, const string& str,
        unordered_map<Pixel, string, hash<Pixel>>& codeTable
    ) {
    if (!node) return;
    if (!node->left && !node->right) {
        codeTable[node->px] = str;
        return;
    }
    BuildImageCodeTable(node->left, str + "0", codeTable);
    BuildImageCodeTable(node->right, str + "1", codeTable);
}

void BuildHuffman(
        const vector<Pixel>& pixels,
        unordered_map<Pixel, string, hash<Pixel>>& codeTable,
        unordered_map<string, Pixel>& reverseCodeTable
    ) {

    // Priority queue to build the Huffman tree
    priority_queue<shared_ptr<ImageNode>, vector<shared_ptr<ImageNode>>, Comparte> pq;

    // Count frequency of each pixel
    unordered_map<Pixel, long long, hash<Pixel>> freqMap;
    for (const auto& px : pixels) {
        freqMap[px]++;
    }

    // Create nodes for each pixel
    for (const auto& pair : freqMap) {
        shared_ptr<ImageNode> node = make_shared<ImageNode>(pair.first);
        node->freq = pair.second;
        pq.push(node);
    }

    // Build the Huffman tree
    while (pq.size() > 1) {
        auto left = pq.top(); pq.pop();
        auto right = pq.top(); pq.pop();
        auto newNode = make_shared<ImageNode>(left, right);
        newNode->freq = left->freq + right->freq;
        pq.push(newNode);
    }

    // Generate the code table
    if (!pq.empty()) {
        BuildImageCodeTable(pq.top(), "", codeTable);
        for (const auto& pair : codeTable) {
            reverseCodeTable[pair.second] = pair.first;
        }
    }
}

string EncodeImage(
    const vector<Pixel>& pixels,
    unordered_map<Pixel, string, hash<Pixel>>& codeTable,
    unordered_map<string, Pixel>& reverseCodeTable
) {
    BuildHuffman(pixels, codeTable, reverseCodeTable);
    string EncodedString;
    for (const auto& px : pixels) {
        EncodedString += codeTable[px];
    }
    return EncodedString;
}

void DecodeImage(
        const string& encoded,
        vector<Pixel>& decodedPixels,
        unordered_map<string, Pixel>& reverseCodeTable
    ) {

    string currentCode;
    for (char bit : encoded) {
        currentCode += bit;
        if (reverseCodeTable.find(currentCode) != reverseCodeTable.end()) {
            decodedPixels.push_back(reverseCodeTable[currentCode]);
            currentCode.clear();
        }
    }
}
