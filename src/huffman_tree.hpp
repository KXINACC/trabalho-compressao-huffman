#ifndef HUFFMAN_TREE_HPP
#define HUFFMAN_TREE_HPP

#include <string>
#include <map>
#include <memory>
#include <queue>
#include <vector>

// Estrutura do no da arvore
struct HuffmanNode {
    std::string symbol;      
    int frequency;           
    std::shared_ptr<HuffmanNode> left;
    std::shared_ptr<HuffmanNode> right;
    
    HuffmanNode(const std::string& sym, int freq)
        : symbol(sym), frequency(freq), left(nullptr), right(nullptr) {}
    
    bool isLeaf() const {
        return left == nullptr && right == nullptr;
    }
};

// Comparador pro heap - pega sempre o menor
struct NodeComparator {
    bool operator()(const std::shared_ptr<HuffmanNode>& a, 
                   const std::shared_ptr<HuffmanNode>& b) const {
        return a->frequency > b->frequency; 
    }
};

class HuffmanTree {
private:
    std::shared_ptr<HuffmanNode> raiz;
    std::map<std::string, std::string> codigos; 
    
    // Gera os codigos percorrendo a arvore
    // Complexidade: O(n) onde n e o numero de nos
    void generateCodes(std::shared_ptr<HuffmanNode> node, const std::string& code);
    
public:
    HuffmanTree() : raiz(nullptr) {}
    
    // Monta a arvore usando o algoritmo de Huffman
    // Usa priority_queue pra pegar sempre os 2 menores
    // Complexidade: O(k log k) onde k eh numero de simbolos
    void buildTree(const std::map<std::string, int>& frequencies);
    
    const std::map<std::string, std::string>& getCodes() const { return codigos; }
    
    std::shared_ptr<HuffmanNode> getRoot() const { return raiz; }
    
    void printTree(std::shared_ptr<HuffmanNode> node = nullptr, int depth = 0) const;
    
    void printCodes() const;
};

#endif // HUFFMAN_TREE_HPP

