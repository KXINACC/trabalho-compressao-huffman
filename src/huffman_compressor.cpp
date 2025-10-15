#include "huffman_tree.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include <filesystem>

namespace fs = std::filesystem;

// Le a tabela de frequencias do arquivo
std::map<std::string, int> loadFrequencyTable(const std::string& filename) {
    std::map<std::string, int> frequencies;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir arquivo de frequências: " << filename << std::endl;
        return frequencies;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('|');
        if (pos != std::string::npos) {
            std::string symbol = line.substr(0, pos);
            int frequency = std::stoi(line.substr(pos + 1));
            
            // Desescapa caracteres especiais
            if (symbol == "\\n") symbol = "\n";
            else if (symbol == "\\t") symbol = "\t";
            else if (symbol == "\\s") symbol = " ";
            else if (symbol == "\\p") symbol = "|";
            
            frequencies[symbol] = frequency;
        }
    }
    
    file.close();
    return frequencies;
}

// Quebra o texto em tokens (igual ao frequency_counter)
std::vector<std::string> tokenize(const std::string& text, const std::map<std::string, int>& frequencies) {
    std::vector<std::string> tokens;
    std::string palavra;
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        
        if (std::isalnum(c) || c == '_') {
            palavra += c;
        } else {
            if (!palavra.empty()) {
                if (frequencies.find(palavra) != frequencies.end()) {
                    tokens.push_back(palavra);
                } else {
                    for (char ch : palavra) {
                        tokens.push_back(std::string(1, ch));
                    }
                }
                palavra.clear();
            }
            
            tokens.push_back(std::string(1, c));
        }
    }
    
    if (!palavra.empty()) {
        if (frequencies.find(palavra) != frequencies.end()) {
            tokens.push_back(palavra);
        } else {
            for (char ch : palavra) {
                tokens.push_back(std::string(1, ch));
            }
        }
    }
    
    return tokens;
}

// Funcao que comprime o arquivo
bool compressFile(const std::string& inputFile, const std::string& outputFile, 
                  const std::map<std::string, std::string>& codes,
                  const std::map<std::string, int>& frequencies) {
    // Lê o arquivo de entrada
    std::ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        std::cerr << "Erro ao abrir arquivo de entrada: " << inputFile << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();
    inFile.close();
    
    // Tokeniza o conteúdo
    auto tokens = tokenize(content, frequencies);
    
    // Gera a sequência de bits
    std::string bitString;
    for (const auto& token : tokens) {
        auto it = codes.find(token);
        if (it != codes.end()) {
            bitString += it->second;
        } else {
            std::cerr << "Aviso: Símbolo não encontrado na codificação: \"" << token << "\"" << std::endl;
            // Usa um código de escape ou ignora
        }
    }
    
    // Calcula padding necessário para completar o último byte
    int padding = (8 - (bitString.length() % 8)) % 8;
    bitString += std::string(padding, '0');
    
    // Escreve o arquivo comprimido
    std::ofstream outFile(outputFile, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Erro ao criar arquivo de saída: " << outputFile << std::endl;
        return false;
    }
    
    // Cabeçalho: tamanho original e padding
    int originalSize = content.length();
    outFile.write(reinterpret_cast<const char*>(&originalSize), sizeof(originalSize));
    outFile.write(reinterpret_cast<const char*>(&padding), sizeof(padding));
    
    // Converte a string de bits em bytes e escreve
    for (size_t i = 0; i < bitString.length(); i += 8) {
        std::bitset<8> byte(bitString.substr(i, 8));
        unsigned char c = static_cast<unsigned char>(byte.to_ulong());
        outFile.write(reinterpret_cast<const char*>(&c), 1);
    }
    
    outFile.close();
    
    // Estatísticas
    int compressedSize = fs::file_size(outputFile);
    double ratio = 1.0 - (static_cast<double>(compressedSize) / originalSize);
    
    std::cout << "\n=== Compressão Concluída ===" << std::endl;
    std::cout << "Arquivo original: " << inputFile << " (" << originalSize << " bytes)" << std::endl;
    std::cout << "Arquivo comprimido: " << outputFile << " (" << compressedSize << " bytes)" << std::endl;
    std::cout << "Taxa de compressão: " << (ratio * 100) << "%" << std::endl;
    std::cout << "Bits totais: " << bitString.length() - padding << std::endl;
    std::cout << "Padding: " << padding << " bits" << std::endl;
    
    return true;
}

// Funcao que descomprime o arquivo
bool decompressFile(const std::string& inputFile, const std::string& outputFile,
                    std::shared_ptr<HuffmanNode> root) {
    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Erro ao abrir arquivo comprimido: " << inputFile << std::endl;
        return false;
    }
    
    // Lê o cabeçalho
    int originalSize, padding;
    inFile.read(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));
    inFile.read(reinterpret_cast<char*>(&padding), sizeof(padding));
    
    // Lê os bytes comprimidos
    std::vector<unsigned char> compressedData;
    unsigned char byte;
    while (inFile.read(reinterpret_cast<char*>(&byte), 1)) {
        compressedData.push_back(byte);
    }
    inFile.close();
    
    // Converte bytes em string de bits
    std::string bitString;
    for (unsigned char c : compressedData) {
        std::bitset<8> bits(c);
        bitString += bits.to_string();
    }
    
    // Remove o padding
    if (padding > 0 && bitString.length() >= static_cast<size_t>(padding)) {
        bitString = bitString.substr(0, bitString.length() - padding);
    }
    
    // Decodifica usando a árvore
    std::string decodedContent;
    auto currentNode = root;
    
    for (char bit : bitString) {
        if (bit == '0') {
            currentNode = currentNode->left;
        } else {
            currentNode = currentNode->right;
        }
        
        // Se chegou em uma folha, adiciona o símbolo
        if (currentNode->isLeaf()) {
            decodedContent += currentNode->symbol;
            currentNode = root; // Volta para a raiz
        }
    }
    
    // Escreve o arquivo descomprimido
    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Erro ao criar arquivo de saída: " << outputFile << std::endl;
        return false;
    }
    
    outFile << decodedContent;
    outFile.close();
    
    std::cout << "\n=== Descompressão Concluída ===" << std::endl;
    std::cout << "Arquivo comprimido: " << inputFile << std::endl;
    std::cout << "Arquivo descomprimido: " << outputFile << " (" << decodedContent.length() << " bytes)" << std::endl;
    
    return true;
}

void printUsage(const char* programName) {
    std::cout << "Uso: " << programName << " <modo> <arquivo_entrada> <arquivo_saída> <tabela_frequências>" << std::endl;
    std::cout << "\nModos:" << std::endl;
    std::cout << "  -c, --compress     Comprimir arquivo" << std::endl;
    std::cout << "  -d, --decompress   Descomprimir arquivo" << std::endl;
    std::cout << "\nExemplos:" << std::endl;
    std::cout << "  Comprimir:   " << programName << " -c exemplo.cpp exemplo.huf frequencias.txt" << std::endl;
    std::cout << "  Descomprimir: " << programName << " -d exemplo.huf exemplo_restaurado.cpp frequencias.txt" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 5) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string mode = argv[1];
    std::string inputFile = argv[2];
    std::string outputFile = argv[3];
    std::string freqFile = argv[4];
    
    std::cout << "=== Compressor/Descompressor de Huffman ===" << std::endl;
    
    // Carrega a tabela de frequências
    std::cout << "Carregando tabela de frequências..." << std::endl;
    auto frequencies = loadFrequencyTable(freqFile);
    
    if (frequencies.empty()) {
        std::cerr << "Erro: Tabela de frequências vazia ou inválida!" << std::endl;
        return 1;
    }
    
    std::cout << "Símbolos carregados: " << frequencies.size() << std::endl;
    
    // Constrói a árvore de Huffman
    std::cout << "Construindo árvore de Huffman..." << std::endl;
    HuffmanTree tree;
    tree.buildTree(frequencies);
    
    if (mode == "-c" || mode == "--compress") {
        // Modo compressão
        auto codes = tree.getCodes();
        std::cout << "Códigos gerados: " << codes.size() << std::endl;
        
        return compressFile(inputFile, outputFile, codes, frequencies) ? 0 : 1;
        
    } else if (mode == "-d" || mode == "--decompress") {
        // Modo descompressão
        auto root = tree.getRoot();
        if (!root) {
            std::cerr << "Erro: Árvore de Huffman inválida!" << std::endl;
            return 1;
        }
        
        return decompressFile(inputFile, outputFile, root) ? 0 : 1;
        
    } else {
        std::cerr << "Erro: Modo inválido: " << mode << std::endl;
        printUsage(argv[0]);
        return 1;
    }
    
    return 0;
}

