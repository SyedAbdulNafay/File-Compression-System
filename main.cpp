#include <iostream>
using namespace std;
#include <fstream>
#include <string>
#include <map>
#include <queue>

struct Node
{
    char character;
    int frequency;
    Node *left;
    Node *right;

    Node(char c, int f, Node *l, Node *r)
    {
        character = c;
        frequency = f;
        left = l;
        right = r;
    }
};

struct CompareNode
{
    bool operator()(Node *a, Node *b)
    {
        return a->frequency > b->frequency;
    }
};

string
readInputFile(const string filePath)
{
    ifstream inputFile(filePath);

    if (!inputFile.is_open())
    {
        cout << "File did not open" << endl;
        return "";
    }

    string content((istreambuf_iterator<char>(inputFile)), (istreambuf_iterator<char>()));

    inputFile.close();

    return content;
}

map<char, int> calculateFrequency(const string content)
{
    map<char, int> frequencies;

    for (char ch : content)
    {
        frequencies[ch]++;
    }

    return frequencies;
}

Node *buildHuffmanTree(const map<char, int> frequencies)
{
    priority_queue<Node *, vector<Node *>, CompareNode> pq;

    for (const auto pair : frequencies)
    {
        Node *node = new Node(pair.first, pair.second, nullptr, nullptr);
        pq.push(node);
    }
    int i = 1;
    while (pq.size() > 1)
    {
        Node *left = pq.top();
        pq.pop();
        Node *right = pq.top();
        pq.pop();

        Node *internalNode = new Node(i + '0', left->frequency + right->frequency, left, right);
        pq.push(internalNode);
        i++;
    }
    return pq.top();
}

void Display(Node *root, string prefix = "")
{
    if (root == nullptr)
        return;

    if (root->left == nullptr && root->right == nullptr)
    {
        // This is a leaf node
        cout << prefix << "Leaf: " << root->character << " (" << root->frequency << ")" << endl;
    }
    else
    {
        // This is an internal node
        cout << prefix << "Internal: " << root->frequency << endl;
        Display(root->left, prefix + "  ");
        Display(root->right, prefix + "  ");
    }
}

void generateHuffmanCodes(Node *root, const string &prefix, map<char, string> &huffmanCodes)
{
    if (root == nullptr)
        return;

    if (root->left == nullptr && root->right == nullptr)
    {
        huffmanCodes[root->character] = prefix;
    }
    else
    {
        generateHuffmanCodes(root->left, prefix + "0", huffmanCodes);
        generateHuffmanCodes(root->right, prefix + "1", huffmanCodes);
    }
}

string encodeText(const string &text, const map<char, string> &huffmanCodes)
{
    string encodeText;

    for (char ch : text)
    {
        encodeText += huffmanCodes.at(ch);
    }
    return encodeText;
}

void saveEncodedData(const string &encodedText, const map<char, string> &huffmanCodes, const string &filePath)
{
    ofstream outFile(filePath, ios::binary);
    if (!outFile.is_open())
    {
        cout << "Could not open file" << endl;
        return;
    }

    // Write the size of the Huffman map
    size_t mapSize = huffmanCodes.size();
    outFile.write(reinterpret_cast<const char *>(&mapSize), sizeof(mapSize));

    // Write the huffman code itself
    for (const auto &pair : huffmanCodes)
    {
        outFile.put(pair.first);
        size_t codeLength = pair.second.size();
        outFile.write(reinterpret_cast<const char *>(&codeLength), sizeof(codeLength));
        outFile.write(pair.second.c_str(), codeLength);
    }

    // Write the encoded text length
    size_t encodedTextLength = encodedText.length();
    outFile.write(reinterpret_cast<const char *>(&encodedTextLength), sizeof(encodedTextLength));

    // Write the encoded text
    outFile.write(encodedText.c_str(), encodedText.size());

    outFile.close();
}

int main()
{
    string filePath = "sample.txt";
    string fileContent = readInputFile(filePath);
    if (!fileContent.empty())
    {
        map<char, int> frequencies = calculateFrequency(fileContent);
        Node *root = buildHuffmanTree(frequencies);
        // Display(root);

        map<char, string> huffmanCodes;
        generateHuffmanCodes(root, "", huffmanCodes);

        cout << "\nHuffman Codes:" << endl;
        for (const auto &pair : huffmanCodes)
        {
            cout << pair.first << ": " << pair.second << endl;
        }

        string encodedText = encodeText(fileContent, huffmanCodes);
        cout << "\nEncoded Text: " << encodedText << endl;

        string outputFilePath = "encoded.bin";
        saveEncodedData(encodedText, huffmanCodes, outputFilePath);
        cout << "Encoded data saved to " << outputFilePath << endl;
    }
    return 0;
}