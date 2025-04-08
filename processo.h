#ifndef PROCESSO_H
#define PROCESSO_H

#include <time.h>

typedef struct {
    long int id;
    char numero[30];
    char data_ajuizamento[30];
    time_t timestamp;
    int id_classe;
    int id_assuntos[5]; 
    int num_assuntos; 
    int ano_eleicao;
} Processo;

int lerProcessosDeArquivo(const char *nomeArquivo, Processo **processos);
void ordenarPorId(Processo *processos, int tamanho);
void ordenarPorDataAjuizamento(Processo *processos, int tamanho);
int contarProcessosPorClasse(const Processo *processos, int tamanho, int id_classe);
int contarAssuntosUnicos(const Processo *processos, int tamanho);
void listarProcessosComMultiplosAssuntos(const Processo *processos, int tamanho);
int calcularDiasTramitacao(time_t timestamp_ajuizamento);
Processo* buscarProcessoPorId(const Processo *processos, int tamanho, long int id);
int salvarProcessosEmArquivo(const char *nomeArquivo, const Processo *processos, int tamanho);
void verificarArquivo(const char *nomeArquivo);
void liberarProcessos(Processo *processos);
void mostrarTempoProcessamento(double tempo, const char *operacao);
int parseLinhaProcesso(const char *linha, Processo *processo);
time_t converterParaTimestamp(const char *data_ajuizamento);

#endif
