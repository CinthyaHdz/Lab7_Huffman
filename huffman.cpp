#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <queue>
#include <vector>
#include <bitset>

using namespace std;

struct Node {
    char ch;
    int freq;
    Node *left, *right;
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        return a->freq > b->freq;
    }
};

void generateCodes(Node* root, string code, unordered_map<char, string>& huffmanCode) {
    if (!root) return;
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = code;
    }
    generateCodes(root->left, code + "0", huffmanCode);
    generateCodes(root->right, code + "1", huffmanCode);
}

void deleteTree(Node* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

int main() {
    ifstream input("entrada.txt");
    if (!input) {
        cerr << "No se pudo abrir el archivo 'entrada.txt'\n";
        return 1;
    }

    stringstream buffer;
    buffer << input.rdbuf();
    string text = buffer.str();
    input.close();

    unordered_map<char, int> freq;
    for (char c : text) freq[c]++;

    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto pair : freq) pq.push(new Node(pair.first, pair.second));

    while (pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* combined = new Node('\0', left->freq + right->freq);
        combined->left = left;
        combined->right = right;
        pq.push(combined);
    }

    Node* root = pq.top();
    unordered_map<char, string> huffmanCode;
    generateCodes(root, "", huffmanCode);

    // Mostrar frecuencias de caracteres
    cout << "Frecuencias:\n";
    for (auto pair : freq) {
        if (pair.first == ' ') cout << "' ' (espacio)";
        else if (pair.first == '\n') cout << "'\\n' (salto)";
        else cout << "'" << pair.first << "'";
        cout << ": " << pair.second << endl;
    }

    // Mostrar codigos de Huffman
    cout << "\nCodigos de Huffman:\n";
    for (auto pair : huffmanCode) {
        if (pair.first == ' ') cout << "' ' (espacio)";
        else if (pair.first == '\n') cout << "'\\n' (salto)";
        else cout << "'" << pair.first << "'";
        cout << ": " << pair.second << endl;
    }

    // Guardar codigos en archivo
    ofstream codigosOut("codigos.txt");
    for (auto pair : huffmanCode) {
        codigosOut << (pair.first == ' ' ? "[espacio]" : string(1, pair.first)) << ": " << pair.second << "\n";
    }
    codigosOut.close();

    string encoded = "";
    for (char c : text) encoded += huffmanCode[c];

    ofstream out("codificado.txt");
    out << encoded;
    out.close();

    int originalBits = text.size() * 8;
    int compressedBits = encoded.size();
    double compressionRate = (double)compressedBits / originalBits * 100;

    cout << "\nBits originales: " << originalBits << "\n";
    cout << "Bits comprimidos: " << compressedBits << "\n";
    cout << "Tasa de compresion: " << compressionRate << "%\n";

    // Paso 8: Decodificacion
    string decoded = "";
    Node* current = root;
    for (char bit : encoded) {
        if (bit == '0') current = current->left;
        else current = current->right;

        if (!current->left && !current->right) {
            decoded += current->ch;
            current = root;
        }
    }

    cout << "\nTexto decodificado (primeros 300 caracteres):\n" << decoded.substr(0, 300) << "...\n";
    deleteTree(root);
    return 0;
}
