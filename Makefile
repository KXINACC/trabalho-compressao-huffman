# Makefile para o Projeto de Compressão de Huffman

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SRCDIR = src
BUILDDIR = build
BINDIR = bin

# Arquivos fonte
TREE_SRC = $(SRCDIR)/huffman_tree.cpp
COUNTER_SRC = $(SRCDIR)/frequency_counter.cpp
COMPRESSOR_SRC = $(SRCDIR)/huffman_compressor.cpp

# Arquivos objeto
TREE_OBJ = $(BUILDDIR)/huffman_tree.o
COUNTER_OBJ = $(BUILDDIR)/frequency_counter.o
COMPRESSOR_OBJ = $(BUILDDIR)/huffman_compressor.o

# Executáveis
COUNTER_BIN = $(BINDIR)/frequency_counter
COMPRESSOR_BIN = $(BINDIR)/huffman_compressor

# Alvos
.PHONY: all clean dirs test

all: dirs $(COUNTER_BIN) $(COMPRESSOR_BIN)

dirs:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(BINDIR)

# Compila o objeto da árvore de Huffman
$(TREE_OBJ): $(TREE_SRC) $(SRCDIR)/huffman_tree.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compila o contador de frequências
$(COUNTER_BIN): $(COUNTER_SRC)
	$(CXX) $(CXXFLAGS) $< -o $@
	@echo "✓ Contador de frequências compilado com sucesso!"

# Compila o compressor/descompressor
$(COMPRESSOR_BIN): $(COMPRESSOR_SRC) $(TREE_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "✓ Compressor/Descompressor compilado com sucesso!"

# Limpa arquivos compilados
clean:
	rm -rf $(BUILDDIR) $(BINDIR)
	rm -f *.huf *.txt
	@echo "✓ Arquivos limpos!"

# Teste básico
test: all
	@echo "\n=== Executando testes básicos ==="
	@echo "\n1. Gerando tabela de frequências..."
	./$(COUNTER_BIN) examples/ frequencias.txt
	@echo "\n2. Comprimindo arquivo de exemplo..."
	./$(COMPRESSOR_BIN) -c examples/exemplo.cpp examples/exemplo.huf frequencias.txt
	@echo "\n3. Descomprimindo arquivo..."
	./$(COMPRESSOR_BIN) -d examples/exemplo.huf examples/exemplo_restaurado.cpp frequencias.txt
	@echo "\n4. Comparando arquivos..."
	@diff examples/exemplo.cpp examples/exemplo_restaurado.cpp && echo "✓ Arquivos idênticos!" || echo "✗ Arquivos diferentes!"

# Informações de ajuda
help:
	@echo "Makefile para Projeto de Compressão de Huffman"
	@echo ""
	@echo "Alvos disponíveis:"
	@echo "  make all       - Compila todos os programas"
	@echo "  make clean     - Remove arquivos compilados"
	@echo "  make test      - Executa testes básicos"
	@echo "  make help      - Mostra esta mensagem"
	@echo ""
	@echo "Uso dos programas:"
	@echo "  ./bin/frequency_counter <diretório> [saída.txt]"
	@echo "  ./bin/huffman_compressor -c <entrada> <saída.huf> <frequencias.txt>"
	@echo "  ./bin/huffman_compressor -d <entrada.huf> <saída> <frequencias.txt>"

