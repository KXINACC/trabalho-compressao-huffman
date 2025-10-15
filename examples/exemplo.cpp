#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Classe para representar um estudante
class Estudante {
private:
    string nome;
    int idade;
    double nota;

public:
    Estudante(const string& n, int i, double nt) 
        : nome(n), idade(i), nota(nt) {}
    
    string getNome() const { return nome; }
    int getIdade() const { return idade; }
    double getNota() const { return nota; }
    
    void setNota(double nt) { nota = nt; }
    
    void exibir() const {
        cout << "Nome: " << nome << endl;
        cout << "Idade: " << idade << endl;
        cout << "Nota: " << nota << endl;
    }
};

// Função para calcular média
double calcularMedia(const vector<Estudante>& estudantes) {
    if (estudantes.empty()) {
        return 0.0;
    }
    
    double soma = 0.0;
    for (const auto& estudante : estudantes) {
        soma += estudante.getNota();
    }
    
    return soma / estudantes.size();
}

// Função para encontrar melhor aluno
Estudante* encontrarMelhorAluno(vector<Estudante>& estudantes) {
    if (estudantes.empty()) {
        return nullptr;
    }
    
    Estudante* melhor = &estudantes[0];
    for (auto& estudante : estudantes) {
        if (estudante.getNota() > melhor->getNota()) {
            melhor = &estudante;
        }
    }
    
    return melhor;
}

// Função para ordenar estudantes por nota
void ordenarPorNota(vector<Estudante>& estudantes) {
    sort(estudantes.begin(), estudantes.end(), 
         [](const Estudante& a, const Estudante& b) {
             return a.getNota() > b.getNota();
         });
}

int main() {
    vector<Estudante> turma;
    
    // Adiciona estudantes
    turma.push_back(Estudante("João Silva", 20, 8.5));
    turma.push_back(Estudante("Maria Santos", 19, 9.2));
    turma.push_back(Estudante("Pedro Oliveira", 21, 7.8));
    turma.push_back(Estudante("Ana Costa", 20, 9.5));
    turma.push_back(Estudante("Carlos Souza", 22, 8.0));
    
    cout << "=== Sistema de Gerenciamento de Estudantes ===" << endl;
    cout << endl;
    
    // Exibe todos os estudantes
    cout << "Lista de Estudantes:" << endl;
    for (const auto& estudante : turma) {
        estudante.exibir();
        cout << "---" << endl;
    }
    
    // Calcula e exibe média
    double media = calcularMedia(turma);
    cout << "Média da turma: " << media << endl;
    cout << endl;
    
    // Encontra melhor aluno
    Estudante* melhor = encontrarMelhorAluno(turma);
    if (melhor != nullptr) {
        cout << "Melhor aluno:" << endl;
        melhor->exibir();
        cout << endl;
    }
    
    // Ordena e exibe ranking
    ordenarPorNota(turma);
    cout << "Ranking de Notas:" << endl;
    int posicao = 1;
    for (const auto& estudante : turma) {
        cout << posicao++ << "º lugar: " << estudante.getNome() 
             << " - Nota: " << estudante.getNota() << endl;
    }
    
    return 0;
}

