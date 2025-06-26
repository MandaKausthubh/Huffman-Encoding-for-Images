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
    DualImage img = ReadImageFromWebP(inputFileName.c_str());
    if (img.MainImage.pixels.empty() || img.ErrorImage.pixels.empty()) {
        cerr << "Error: Could not read image from " << inputFileName << endl;
        return 1;
    }

    // Create a frequency map for the pixels
    unordered_map<string, Pixel> reverseCodeTable;
    unordered_map<Pixel, string, hash<Pixel>> codeTable;
    unordered_map<Pixel, string, hash<Pixel>> errorCodeTable;
    unordered_map<string, Pixel> errorReverseCodeTable;

    BuildHuffman(
        img.MainImage.pixels,
        codeTable,
        reverseCodeTable
    );
    BuildHuffman(
        img.ErrorImage.pixels,
        errorCodeTable,
        errorReverseCodeTable
    );
    if (codeTable.empty()) {
        cerr << "Error: Could not build Huffman tree." << endl;
        return 1;
    }

    // Encode the image using Huffman coding
    string encodedImage = EncodeImage(
                                img.MainImage.pixels,
                                codeTable,
                                reverseCodeTable);
    string encodedErrorImage = EncodeImage(
        img.ErrorImage.pixels,
        errorCodeTable,
        errorReverseCodeTable);

    if (encodedImage.empty()) {
        cerr << "Error: Could not encode image." << endl;
        return 1;
    }
    write_to_bin_file(
        binFileName,
        reverseCodeTable,
        errorReverseCodeTable,
        img.MainImage.height, img.MainImage.width,
        encodedImage,
        encodedErrorImage
    );


    // Decode the image back to pixels
    vector<Pixel> decodedPixels;
    vector<Pixel> decodedErrorPixels;
    unordered_map<string, Pixel> reverseCodeTableError_testing;
    unordered_map<string, Pixel> reverseCodeTableMain_testing;

    read_from_bin_file(
        binFileName,
        reverseCodeTableMain_testing,
        reverseCodeTableError_testing,
        img.MainImage.height, img.MainImage.width,
        encodedImage, encodedErrorImage
    );

    DecodeImage(encodedImage, decodedPixels, reverseCodeTableMain_testing);
    DecodeImage(encodedErrorImage, decodedErrorPixels, reverseCodeTableError_testing);
    if (decodedPixels.empty() || decodedErrorPixels.empty()) {
        cerr << "Error: Could not decode image." << endl;
        return 1;
    }

    WriteImageToPNG(outputFileName.c_str(), DualImage(
        Image(decodedPixels, img.MainImage.width, img.MainImage.height),
        Image(decodedErrorPixels, img.ErrorImage.width, img.ErrorImage.height)
    ));


    cout << "Image decoded and saved to " << outputFileName << endl;
    cout << "Image encoded and saved to " << binFileName << endl;
    cout << "Exiting program..." << endl;

    return 0;
}
