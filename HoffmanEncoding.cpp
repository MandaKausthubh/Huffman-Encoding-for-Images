#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

using namespace std;

// Huffman tree node
struct Node {
    char ch;
    int freq;
    shared_ptr<Node> left, right;
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    Node(shared_ptr<Node> l, shared_ptr<Node> r) : ch('\0'), freq(l->freq + r->freq), left(l), right(r) {}
};

// Comparison for min-heap
struct Compare {
    bool operator()(shared_ptr<Node> const& a, shared_ptr<Node> const& b) {
        return a->freq > b->freq;
    }
};

// Recursively build code table
void buildCodeTable(shared_ptr<Node> node, const string& str, unordered_map<char, string>& codeTable) {
    if (!node) return;
    if (!node->left && !node->right) {
        codeTable[node->ch] = str;
        return;
    }
    buildCodeTable(node->left, str + "0", codeTable);
    buildCodeTable(node->right, str + "1", codeTable);
}

// Build Huffman tree and code table from input text
void huffmanBuild(const string& text, shared_ptr<Node>& root, unordered_map<char, string>& codeTable) {
    unordered_map<char, int> freq;
    for (char ch : text) freq[ch]++;
    priority_queue<shared_ptr<Node>, vector<shared_ptr<Node>>, Compare> pq;
    for (auto& p : freq) pq.push(make_shared<Node>(p.first, p.second));
    while (pq.size() > 1) {
        auto l = pq.top(); pq.pop();
        auto r = pq.top(); pq.pop();
        pq.push(make_shared<Node>(l, r));
    }
    root = pq.top();
    buildCodeTable(root, "", codeTable);
}

// Encode input text using code table
string huffmanEncode(const string& text, const unordered_map<char, string>& codeTable) {
    string encoded;
    for (char ch : text) encoded += codeTable.at(ch);
    return encoded;
}

// Decode bit string using Huffman tree
string huffmanDecode(const string& encoded, shared_ptr<Node> root) {
    string decoded;
    auto node = root;
    for (char bit : encoded) {
        node = (bit == '0') ? node->left : node->right;
        if (!node->left && !node->right) {
            decoded += node->ch;
            node = root;
        }
    }
    return decoded;
}

int main() {
    string text = "huffman example";
    shared_ptr<Node> root;
    unordered_map<char, string> codeTable;

    // Build Huffman tree and code table
    huffmanBuild(text, root, codeTable);

    cout << "Huffman Codes:\n";
    for (auto& p : codeTable)
        cout << p.first << ": " << p.second << endl;

    // Encode
    string encoded = huffmanEncode(text, codeTable);
    cout << "\nEncoded string:\n" << encoded << endl;

    // Decode
    string decoded = huffmanDecode(encoded, root);
    cout << "\nDecoded string:\n" << decoded << endl;

    return 0;
}
