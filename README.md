# Compressor de Huffman - EDB2

Projeto da 1ª unidade de Estruturas de Dados 2

## Compilação

```bash
make all
```

## Uso

### 1. Gerar tabela de frequências
```bash
./bin/frequency_counter <diretório_ou_arquivo> <saída.txt>
```

Exemplo:
```bash
./bin/frequency_counter examples/ frequencias.txt
```

### 2. Comprimir arquivo
```bash
./bin/huffman_compressor -c <entrada> <saída.huf> <tabela_frequências>
```

Exemplo:
```bash
./bin/huffman_compressor -c examples/exemplo.cpp exemplo.huf frequencias.txt
```

### 3. Descomprimir arquivo
```bash
./bin/huffman_compressor -d <entrada.huf> <saída> <tabela_frequências>
```

Exemplo:
```bash
./bin/huffman_compressor -d exemplo.huf exemplo_restaurado.cpp frequencias.txt
```

## Teste Rápido

```bash
make all
./bin/frequency_counter examples/ freq.txt
./bin/huffman_compressor -c examples/exemplo.cpp teste.huf freq.txt
./bin/huffman_compressor -d teste.huf restaurado.cpp freq.txt
diff examples/exemplo.cpp restaurado.cpp
```

## Estrutura

- `src/huffman_tree.hpp` e `.cpp` - Implementação da árvore de Huffman
- `src/frequency_counter.cpp` - Programa contador de frequências
- `src/huffman_compressor.cpp` - Programa compressor/descompressor
- `examples/` - Arquivos de exemplo para teste
- `Makefile` - Compilação automatizada

## Observações

- Desenvolvido em C++17
- Otimizado para código-fonte C++ (reconhece palavras-chave)
- Taxa de compressão típica: 40-45% em código C++
