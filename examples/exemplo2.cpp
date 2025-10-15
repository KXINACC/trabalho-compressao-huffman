#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

// Algoritmo de busca binária
int buscarBinariamente(const std::vector<int>& arr, int target) {
    int left = 0;
    int right = arr.size() - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    return -1;
}

// Algoritmo de ordenação QuickSort
void ordenacaoRapida(std::vector<int>& arr, int low, int high) {
    if (low < high) {
        int pivot = arr[high];
        int i = low - 1;
        
        for (int j = low; j < high; j++) {
            if (arr[j] < pivot) {
                i++;
                std::swap(arr[i], arr[j]);
            }
        }
        
        std::swap(arr[i + 1], arr[high]);
        int pi = i + 1;
        
        ordenacaoRapida(arr, low, pi - 1);
        ordenacaoRapida(arr, pi + 1, high);
    }
}

int main() {
    std::vector<int> numeros = {64, 34, 25, 12, 22, 11, 90};
    
    std::cout << "Array original: ";
    for (int num : numeros) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    
    ordenacaoRapida(numeros, 0, numeros.size() - 1);
    
    std::cout << "Array ordenado: ";
    for (int num : numeros) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    
    int busca = 25;
    int resultado = buscarBinariamente(numeros, busca);
    
    if (resultado != -1) {
        std::cout << "Elemento " << busca << " encontrado na posição " << resultado << std::endl;
    } else {
        std::cout << "Elemento não encontrado" << std::endl;
    }
    
    return 0;
}

