#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <iomanip>

namespace fs = std::filesystem;

// Lista de palavras-chave do C++ que vou tratar como simbolos unicos
// Isso eh o que torna esse compressor especializado pra codigo-fonte
const std::vector<std::string> CPP_KEYWORDS = {
    "alignas", "alignof", "and", "and_eq", "asm", "auto",
    "bitand", "bitor", "bool", "break", "case", "catch",
    "char", "char8_t", "char16_t", "char32_t", "class", "compl",
    "concept", "const", "consteval", "constexpr", "constinit", "const_cast",
    "continue", "co_await", "co_return", "co_yield", "decltype", "default",
    "delete", "do", "double", "dynamic_cast", "else", "enum",
    "explicit", "export", "extern", "false", "float", "for",
    "friend", "goto", "if", "inline", "int", "long",
    "mutable", "namespace", "new", "noexcept", "not", "not_eq",
    "nullptr", "operator", "or", "or_eq", "private", "protected",
    "public", "register", "reinterpret_cast", "requires", "return", "short",
    "signed", "sizeof", "static", "static_assert", "static_cast", "struct",
    "switch", "template", "this", "thread_local", "throw", "true",
    "try", "typedef", "typeid", "typename", "union", "unsigned",
    "using", "virtual", "void", "volatile", "wchar_t", "while",
    "xor", "xor_eq",
    // Palavras comuns da STL
    "std", "cout", "cin", "endl", "vector", "map", "string",
    "include", "define", "ifdef", "ifndef", "endif", "pragma"
};

bool ehPalavraChave(const std::string& word) {
    return std::find(CPP_KEYWORDS.begin(), CPP_KEYWORDS.end(), word) != CPP_KEYWORDS.end();
}

// Funcao pra quebrar o texto em tokens
// Se for keyword, trata como simbolo unico
// Se nao, separa em caracteres individuais
std::vector<std::string> tokenizar(const std::string& text) {
    std::vector<std::string> tokens;
    std::string palavra_atual;
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        
        if (std::isalnum(c) || c == '_') {
            palavra_atual += c;
        } else {
            if (!palavra_atual.empty()) {
                if (ehPalavraChave(palavra_atual)) {
                    tokens.push_back(palavra_atual);
                } else {
                    for (char ch : palavra_atual) {
                        tokens.push_back(std::string(1, ch));
                    }
                }
                palavra_atual.clear();
            }
            
            tokens.push_back(std::string(1, c));
        }
    }
    
    if (!palavra_atual.empty()) {
        if (ehPalavraChave(palavra_atual)) {
            tokens.push_back(palavra_atual);
        } else {
            for (char ch : palavra_atual) {
                tokens.push_back(std::string(1, ch));
            }
        }
    }
    
    return tokens;
}

// Le o arquivo e conta quantas vezes cada simbolo aparece
void contarFrequencias(const std::string& filename, std::map<std::string, int>& frequencies) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir arquivo: " << filename << std::endl;
        return;
    }
    
    std::string linha;
    while (std::getline(file, linha)) {
        linha += '\n';
        
        auto tokens = tokenizar(linha);
        
        for (const auto& token : tokens) {
            frequencies[token]++;
        }
    }
    
    file.close();
}

/**
 * Processa todos os arquivos de um diretório recursivamente
 */
void processarDiretorio(const std::string& dirPath, std::map<std::string, int>& frequencies) {
    try {
        for (const auto& entry : fs::recursive_directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                // Processa apenas arquivos C++
                if (ext == ".cpp" || ext == ".hpp" || ext == ".h" || ext == ".cc" || ext == ".cxx") {
                    std::cout << "Processando: " << entry.path().string() << std::endl;
                    contarFrequencias(entry.path().string(), frequencies);
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Erro ao processar diretório: " << e.what() << std::endl;
    }
}

/**
 * Salva a tabela de frequências em um arquivo
 */
void salvarTabelaFrequencias(const std::map<std::string, int>& frequencies, const std::string& outputFile) {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        std::cerr << "Erro ao criar arquivo de saída: " << outputFile << std::endl;
        return;
    }
    
    // Formato: símbolo|frequência
    for (const auto& pair : frequencies) {
        std::string symbol = pair.first;
        // Escapa caracteres especiais
        if (symbol == "\n") symbol = "\\n";
        else if (symbol == "\t") symbol = "\\t";
        else if (symbol == " ") symbol = "\\s";
        else if (symbol == "|") symbol = "\\p";
        
        file << symbol << "|" << pair.second << "\n";
    }
    
    file.close();
    std::cout << "\nTabela de frequências salva em: " << outputFile << std::endl;
}

/**
 * Imprime estatísticas da tabela de frequências
 */
void imprimirEstatisticas(const std::map<std::string, int>& frequencies) {
    int totalSymbols = 0;
    int totalOccurrences = 0;
    
    for (const auto& pair : frequencies) {
        totalSymbols++;
        totalOccurrences += pair.second;
    }
    
    std::cout << "\n=== Estatísticas ===" << std::endl;
    std::cout << "Total de símbolos únicos: " << totalSymbols << std::endl;
    std::cout << "Total de ocorrências: " << totalOccurrences << std::endl;
    
    // Top 10 símbolos mais frequentes
    std::vector<std::pair<std::string, int>> sorted(frequencies.begin(), frequencies.end());
    std::sort(sorted.begin(), sorted.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::cout << "\nTop 10 símbolos mais frequentes:" << std::endl;
    std::cout << std::left << std::setw(20) << "Símbolo" 
              << std::setw(15) << "Frequência" 
              << "Porcentagem" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (int i = 0; i < std::min(10, (int)sorted.size()); ++i) {
        std::string display = sorted[i].first;
        if (display == "\n") display = "\\n";
        else if (display == "\t") display = "\\t";
        else if (display == " ") display = "[espaço]";
        
        double percentage = (sorted[i].second * 100.0) / totalOccurrences;
        std::cout << std::left << std::setw(20) << display
                  << std::setw(15) << sorted[i].second
                  << std::fixed << std::setprecision(2) << percentage << "%" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Uso: " << argv[0] << " <arquivo_ou_diretório> [arquivo_saída]" << std::endl;
        std::cout << "\nExemplos:" << std::endl;
        std::cout << "  " << argv[0] << " exemplo.cpp frequencias.txt" << std::endl;
        std::cout << "  " << argv[0] << " ./src/ frequencias.txt" << std::endl;
        return 1;
    }
    
    std::string inputPath = argv[1];
    std::string outputFile = argc > 2 ? argv[2] : "frequencias.txt";
    
    std::map<std::string, int> frequencies;
    
    std::cout << "=== Contador de Frequências de Símbolos ===" << std::endl;
    std::cout << "Analisando: " << inputPath << std::endl << std::endl;
    
    // Verifica se é arquivo ou diretório
    if (fs::is_directory(inputPath)) {
        processarDiretorio(inputPath, frequencies);
    } else if (fs::is_regular_file(inputPath)) {
        contarFrequencias(inputPath, frequencies);
    } else {
        std::cerr << "Erro: Caminho inválido: " << inputPath << std::endl;
        return 1;
    }
    
    if (frequencies.empty()) {
        std::cerr << "Nenhum símbolo encontrado!" << std::endl;
        return 1;
    }
    
    // Imprime estatísticas
    imprimirEstatisticas(frequencies);
    
    // Salva a tabela
    salvarTabelaFrequencias(frequencies, outputFile);
    
    return 0;
}

