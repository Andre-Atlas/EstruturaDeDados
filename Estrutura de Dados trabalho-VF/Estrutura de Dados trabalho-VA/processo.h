#ifndef PROCESSO_H
#define PROCESSO_H

#include <time.h>

typedef struct {
    long int id;
    char numero[30];
    char data_ajuizamento[30];
    time_t timestamp;
    int id_classe;
    int id_assuntos[5]; // Agora suporta até 5 assuntos
    int num_assuntos;   // Contador de assuntos
    int ano_eleicao;
} Processo;

// Funções principais
int lerProcessosDeArquivo(const char *nomeArquivo, Processo **processos);
void ordenarPorId(Processo *processos, int quantidade);
void ordenarPorDataAjuizamento(Processo *processos, int quantidade);
int contarProcessosPorClasse(const Processo *processos, int quantidade, int id_classe);
int contarAssuntosUnicos(const Processo *processos, int quantidade);
void listarProcessosComMultiplosAssuntos(const Processo *processos, int quantidade);
int calcularDiasTramitacao(time_t timestamp_ajuizamento);
Processo* buscarProcessoPorId(const Processo *processos, int quantidade, long int id);

// Funções auxiliares
int salvarProcessosEmArquivo(const char *nomeArquivo, const Processo *processos, int quantidade);
void verificarArquivo(const char *nomeArquivo);
void liberarProcessos(Processo *processos);
void mostrarTempoProcessamento(double tempo, const char *operacao);

// Funções de parsing e conversão
int parseLinhaProcesso(const char *linha, Processo *processo);
time_t converterParaTimestamp(const char *data_ajuizamento);

#endif