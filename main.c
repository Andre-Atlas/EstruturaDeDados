#include "processo.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// como compilar: ctrl + J
// gcc -Wall -O3 -o programa main.c processo.c
// ./programa.exe

void mostrarMenuPrincipal() {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Carregar e ordenar processos\n");
    printf("2. Consultar processos por classe\n");
    printf("3. Consultar processo por ID\n");
    printf("4. Mostrar dias de tramitacao\n");
    printf("5. Mostrar contagem de assuntos unicos\n");
    printf("6. Listar processos com multiplos assuntos\n");
    printf("7. Sair\n");
    printf("======================\n");
    printf("Escolha uma opcao: ");
}

void menuConsultarClasse(const Processo *processos, int tamanho) {
    int id_classe;
    printf("\nDigite o ID da classe para consulta: ");
    scanf("%d", &id_classe);
    
    int count = contarProcessosPorClasse(processos, tamanho, id_classe);
    printf("\nProcessos da classe %d: %d\n", id_classe, count);
}

void menuConsultarProcesso(const Processo *processos, int tamanho) {
    long int id;
    printf("\nDigite o ID do processo: ");
    scanf("%ld", &id);
    
    Processo *proc = buscarProcessoPorId(processos, tamanho, id);
    if (proc == NULL) {
        printf("Processo nao encontrado!\n");
        return;
    }
    
    printf("\n=== Dados do Processo ===\n");
    printf("ID: %ld\n", proc->id);
    printf("Numero: %s\n", proc->numero);
    printf("Data de ajuizamento: %s\n", proc->data_ajuizamento);
    printf("Classe: %d\n", proc->id_classe);
    printf("Assuntos: ");
    for (int i = 0; i < proc->num_assuntos; i++) {
        printf("%d", proc->id_assuntos[i]);
        if (i < proc->num_assuntos - 1) printf(", ");
    }
    printf("\nAno eleicao: %d\n", proc->ano_eleicao);
}

void menuDiasTramitacao(const Processo *processos, int tamanho) {
    long int id;
    printf("\nDigite o ID do processo: ");
    scanf("%ld", &id);
    
    Processo *proc = buscarProcessoPorId(processos, tamanho, id);
    if (proc == NULL) {
        printf("Processo não encontrado!\n");
        return;
    }
    
    int dias = calcularDiasTramitacao(proc->timestamp);
    printf("\nO processo %ld esta em tramitacao ha %d dias\n", proc->id, dias);
}

void menuAssuntosUnicos(const Processo *processos, int tamanho) {
    clock_t inicio = clock();
    int unicos = contarAssuntosUnicos(processos, tamanho);
    mostrarTempoProcessamento((double)(clock() - inicio) / CLOCKS_PER_SEC, 
                             "Contagem de assuntos unicos");
    printf("\nQuantidade de assuntos unicos na base: %d\n", unicos);
}

void menuMultiplosAssuntos(const Processo *processos, int tamanho) {
    clock_t inicio = clock();
    listarProcessosComMultiplosAssuntos(processos, tamanho);
    mostrarTempoProcessamento((double)(clock() - inicio) / CLOCKS_PER_SEC, 
                             "Listagem de processos com multiplos assuntos");
}

int main() {
    Processo *processos = NULL;
    int tamanho = 0;
    int opcao = 0;
    int dados_carregados = 0;

    while (1) {
        mostrarMenuPrincipal();
        scanf("%d", &opcao);
        
        switch (opcao) {
            case 1: 
                if (processos != NULL) {
                    liberarProcessos(processos);
                }
                tamanho = lerProcessosDeArquivo("processo_043_202409032338.csv", &processos);
                if (tamanho <= 0) {
                    printf("Erro ao carregar processos!\n");
                    break;
                }
                printf("\n%d processos carregados com sucesso!\n", tamanho);
                
                printf("\nOrdenando por ID...\n");
                ordenarPorId(processos, tamanho);
                salvarProcessosEmArquivo("ordenado_por_id.csv", processos, tamanho);
                verificarArquivo("ordenado_por_id.csv");
                
                printf("\nOrdenando por data...\n");
                ordenarPorDataAjuizamento(processos, tamanho);
                salvarProcessosEmArquivo("ordenado_por_data.csv", processos, tamanho);
                verificarArquivo("ordenado_por_data.csv");
                
                dados_carregados = 1;
                break;
                
            case 2: 
                if (!dados_carregados) {
                    printf("Carregue os dados primeiro (opcao 1)!\n");
                    break;
                }
                menuConsultarClasse(processos, tamanho);
                break;
                
            case 3: 
                if (!dados_carregados) {
                    printf("Carregue os dados primeiro (opcao 1)!\n");
                    break;
                }
                menuConsultarProcesso(processos, tamanho);
                break;
                
            case 4: 
                if (!dados_carregados) {
                    printf("Carregue os dados primeiro (opcao 1)!\n");
                    break;
                }
                menuDiasTramitacao(processos, tamanho);
                break;
                
            case 5: 
                if (!dados_carregados) {
                    printf("Carregue os dados primeiro (opcao 1)!\n");
                    break;
                }
                menuAssuntosUnicos(processos, tamanho);
                break;
                
            case 6: 
                if (!dados_carregados) {
                    printf("Carregue os dados primeiro (opcao 1)!\n");
                    break;
                }
                menuMultiplosAssuntos(processos, tamanho);
                break;
                
            case 7: 
                liberarProcessos(processos);
                printf("Encerrando o programa...\n");
                return 0;
                
            default:
                printf("opcao invalida!\n");
        }
    }
}
