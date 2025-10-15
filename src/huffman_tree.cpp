#include "huffman_tree.hpp"
#include <iostream>
#include <iomanip>

void HuffmanTree::construirArvore(const std::map<std::string, int>& frequencies) {
    if (frequencies.empty()) {
        std::cerr << "Erro: Tabela de frequencias vazia!" << std::endl;
        return;
    }
    
    // Cria o heap com os nos
    std::priority_queue<std::shared_ptr<HuffmanNode>, 
                       std::vector<std::shared_ptr<HuffmanNode>>, 
                       NodeComparator> heap;
    
    // Coloca todos os simbolos no heap
    for (const auto& pair : frequencies) {
        auto no_atual = std::make_shared<HuffmanNode>(pair.first, pair.second);
        heap.push(no_atual);
    }
    
    // Monta a arvore de baixo pra cima
    // sempre juntando os 2 menores
    while (heap.size() > 1) {
        auto esquerda = heap.top();
        heap.pop();
        
        auto direita = heap.top();
        heap.pop();
        
        // Cria no pai com soma das frequencias
        auto pai = std::make_shared<HuffmanNode>("", esquerda->frequency + direita->frequency);
        pai->left = esquerda;
        pai->right = direita;
        
        heap.push(pai);
    }
    
    raiz = heap.top();
    
    codigos.clear();
    gerarCodigos(raiz, "");
}

void HuffmanTree::gerarCodigos(std::shared_ptr<HuffmanNode> node, const std::string& code) {
    if (!node) return;
    
    if (node->ehFolha()) {
        codigos[node->symbol] = code.empty() ? "0" : code;
        return;
    }
    
    // Desce na arvore: esquerda = 0, direita = 1
    gerarCodigos(node->left, code + "0");
    gerarCodigos(node->right, code + "1");
}

void HuffmanTree::imprimirArvore(std::shared_ptr<HuffmanNode> node, int depth) const {
    if (!node) {
        node = raiz;
        if (!node) return;
    }
    
    std::string indent(depth * 2, ' ');
    
    if (node->ehFolha()) {
        std::cout << indent << "Folha: \"" << node->symbol 
                  << "\" (freq: " << node->frequency << ")" << std::endl;
    } else {
        std::cout << indent << "Nó interno (freq: " << node->frequency << ")" << std::endl;
        if (node->left) {
            std::cout << indent << "├─ Esquerda (0):" << std::endl;
            imprimirArvore(node->left, depth + 1);
        }
        if (node->right) {
            std::cout << indent << "└─ Direita (1):" << std::endl;
            imprimirArvore(node->right, depth + 1);
        }
    }
}

void HuffmanTree::imprimirCodigos() const {
    std::cout << "\n=== Codigos de Huffman Gerados ===" << std::endl;
    std::cout << std::left << std::setw(20) << "Simbolo" 
              << std::setw(15) << "Codigo" 
              << "Tamanho" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (const auto& pair : codigos) {
        std::string display = pair.first;
        if (display == "\n") display = "\\n";
        else if (display == "\t") display = "\\t";
        else if (display == " ") display = "[espaco]";
        
        std::cout << std::left << std::setw(20) << display
                  << std::setw(15) << pair.second
                  << pair.second.length() << " bits" << std::endl;
    }
}

