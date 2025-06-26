#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include "include/Huffman.h"
#include "include/IOUtils.h"
#include "include/Pixel.h"

using namespace std;

int main() {
    string inputFileName, outputFileName, binFileName;

    cout << "Enter input PNG file name: ";
    cin >> inputFileName;

    cout << "Enter output PNG file name: ";
    cin >> outputFileName;

    cout << "Enter binary file name: ";
    cin >> binFileName;

    // Read the image from the PNG file
    Image img = ReadImageFromPNG(inputFileName.c_str());
    if (img.pixels.empty()) {
        cerr << "Error: Could not read image from " << inputFileName << endl;
        return 1;
    }

    // Create a frequency map for the pixels
    unordered_map<string, Pixel> reverseCodeTable;
    unordered_map<Pixel, string, hash<Pixel>> codeTable;
    BuildHuffman(img.pixels, codeTable, reverseCodeTable);
    if (codeTable.empty()) {
        cerr << "Error: Could not build Huffman tree." << endl;
        return 1;
    }

    // Encode the image using Huffman coding
    string encodedImage = EncodeImage(img.pixels, codeTable, reverseCodeTable);
    if (encodedImage.empty()) {
        cerr << "Error: Could not encode image." << endl;
        return 1;
    }
    write_to_bin_file(binFileName, reverseCodeTable, img.height, img.width, encodedImage);


    // Decode the image back to pixels
    vector<Pixel> decodedPixels;
    read_from_bin_file(binFileName, reverseCodeTable, img.height, img.width, encodedImage);
    DecodeImage(encodedImage, decodedPixels, reverseCodeTable);
    if (decodedPixels.empty()) {
        cerr << "Error: Could not decode image." << endl;
        return 1;
    }
    // Create a new Image object with the decoded pixels
    Image decodedImage(decodedPixels, img.width, img.height);
    WriteImageToPNG(outputFileName.c_str(), decodedImage);

    cout << "Image decoded and saved to " << outputFileName << endl;
    cout << "Image encoded and saved to " << binFileName << endl;
    cout << "Exiting program..." << endl;

    return 0;
}
