#include "processo.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void mostrarMenuPrincipal() {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Carregar e ordenar processos\n");
    printf("2. Consultar processos por classe\n");
    printf("3. Consultar processo por ID\n");
    printf("4. Mostrar dias de tramitação\n");
    printf("5. Mostrar contagem de assuntos únicos\n");
    printf("6. Listar processos com múltiplos assuntos\n");
    printf("7. Sair\n");
    printf("======================\n");
    printf("Escolha uma opção: ");
}

void menuConsultarClasse(const Processo *processos, int quantidade) {
    int id_classe;
    printf("\nDigite o ID da classe para consulta: ");
    scanf("%d", &id_classe);
    
    int count = contarProcessosPorClasse(processos, quantidade, id_classe);
    printf("\nProcessos da classe %d: %d\n", id_classe, count);
}

void menuConsultarProcesso(const Processo *processos, int quantidade) {
    long int id;
    printf("\nDigite o ID do processo: ");
    scanf("%ld", &id);
    
    Processo *proc = buscarProcessoPorId(processos, quantidade, id);
    if (proc == NULL) {
        printf("Processo não encontrado!\n");
        return;
    }
    
    printf("\n=== Dados do Processo ===\n");
    printf("ID: %ld\n", proc->id);
    printf("Número: %s\n", proc->numero);
    printf("Data de ajuizamento: %s\n", proc->data_ajuizamento);
    printf("Classe: %d\n", proc->id_classe);
    printf("Assuntos: ");
    for (int i = 0; i < proc->num_assuntos; i++) {
        printf("%d", proc->id_assuntos[i]);
        if (i < proc->num_assuntos - 1) printf(", ");
    }
    printf("\nAno eleição: %d\n", proc->ano_eleicao);
}

void menuDiasTramitacao(const Processo *processos, int quantidade) {
    long int id;
    printf("\nDigite o ID do processo: ");
    scanf("%ld", &id);
    
    Processo *proc = buscarProcessoPorId(processos, quantidade, id);
    if (proc == NULL) {
        printf("Processo não encontrado!\n");
        return;
    }
    
    int dias = calcularDiasTramitacao(proc->timestamp);
    printf("\nO processo %ld está em tramitação há %d dias\n", proc->id, dias);
}

void menuAssuntosUnicos(const Processo *processos, int quantidade) {
    clock_t inicio = clock();
    int unicos = contarAssuntosUnicos(processos, quantidade);
    mostrarTempoProcessamento((double)(clock() - inicio) / CLOCKS_PER_SEC, 
                             "Contagem de assuntos únicos");
    printf("\nQuantidade de assuntos únicos na base: %d\n", unicos);
}

void menuMultiplosAssuntos(const Processo *processos, int quantidade) {
    clock_t inicio = clock();
    listarProcessosComMultiplosAssuntos(processos, quantidade);
    mostrarTempoProcessamento((double)(clock() - inicio) / CLOCKS_PER_SEC, 
                             "Listagem de processos com múltiplos assuntos");
}

int main() {
    Processo *processos = NULL;
    int quantidade = 0;
    int opcao = 0;
    int dados_carregados = 0;

    while (1) {
        mostrarMenuPrincipal();
        scanf("%d", &opcao);
        
        switch (opcao) {
            case 1: // Carregar e ordenar
                if (processos != NULL) {
                    liberarProcessos(processos);
                }
                quantidade = lerProcessosDeArquivo("processo_043_202409032338.csv", &processos);
                if (quantidade <= 0) {
                    printf("Erro ao carregar processos!\n");
                    break;
                }
                printf("\n%d processos carregados com sucesso!\n", quantidade);
                
                printf("\nOrdenando por ID...\n");
                ordenarPorId(processos, quantidade);
                salvarProcessosEmArquivo("ordenado_por_id.csv", processos, quantidade);
                verificarArquivo("ordenado_por_id.csv");
                
                printf("\nOrdenando por data...\n");
                ordenarPorDataAjuizamento(processos, quantidade);
                salvarProcessosEmArquivo("ordenado_por_data.csv", processos, quantidade);
                verificarArquivo("ordenado_por_data.csv");
                
                dados_carregados = 1;
                break;
                
            case 2: // Consultar por classe
                if (!dados_carregados) {
                    printf("Carregue os dados primeiro (opção 1)!\n");
                    break;
                }
                menuConsultarClasse(processos, quantidade);
                break;
                
            case 3: // Consultar por ID
                if (!dados_carregados) {
                    printf("Carregue os dados primeiro (opção 1)!\n");
                    break;
                }
                menuConsultarProcesso(processos, quantidade);
                break;
                
            case 4: // Dias de tramitação
                if (!dados_carregados) {
                    printf("Carregue os dados primeiro (opção 1)!\n");
                    break;
                }
                menuDiasTramitacao(processos, quantidade);
                break;
                
            case 5: // Assuntos únicos
                if (!dados_carregados) {
                    printf("Carregue os dados primeiro (opção 1)!\n");
                    break;
                }
                menuAssuntosUnicos(processos, quantidade);
                break;
                
            case 6: // Múltiplos assuntos
                if (!dados_carregados) {
                    printf("Carregue os dados primeiro (opção 1)!\n");
                    break;
                }
                menuMultiplosAssuntos(processos, quantidade);
                break;
                
            case 7: // Sair
                liberarProcessos(processos);
                printf("Encerrando o programa...\n");
                return 0;
                
            default:
                printf("Opção inválida!\n");
        }
    }
}