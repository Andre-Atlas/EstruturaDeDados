#include "processo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

void mostrarTempoProcessamento(double tempo, const char *operacao) {
    printf("[TEMPO] %s: %.4f segundos\n", operacao, tempo);
}

time_t converterParaTimestamp(const char *data_ajuizamento) {
    struct tm tm = {0};
    if (sscanf(data_ajuizamento, "%d-%d-%d %d:%d:%d", 
               &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
               &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6) {
        return -1;
    }
    
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    tm.tm_isdst = -1;
    
    return mktime(&tm);
}

int parseLinhaProcesso(const char *linha, Processo *processo) {
    char temp[256] = {0};
    size_t i = 0, j = 0;
    int campo = 0;
    int dentroAspas = 0;
    int dentroChaves = 0;
    
    memset(processo, 0, sizeof(Processo));
    
    while (linha[i] != '\0' && campo < 6) {
        if (linha[i] == '"') {
            dentroAspas = !dentroAspas;
            i++;
            continue;
        }
        
        if (linha[i] == '{') {
            dentroChaves = 1;
            i++;
            continue;
        }
        
        if (linha[i] == '}') {
            dentroChaves = 0;
            i++;
            continue;
        }
        
        if (linha[i] == ',' && !dentroAspas && !dentroChaves) {
            switch (campo) {
                case 0: 
                    processo->id = atol(temp); 
                    break;
                case 1: 
                    strncpy(processo->numero, temp, sizeof(processo->numero)-1); 
                    break;
                case 2: 
                    strncpy(processo->data_ajuizamento, temp, sizeof(processo->data_ajuizamento)-1);
                    processo->timestamp = converterParaTimestamp(temp);
                    break;
                case 3: 
                    processo->id_classe = atoi(temp); 
                    break;
                case 4:
                    processo->num_assuntos = 0;
                    char *token = strtok(temp, "{},");
                    while (token != NULL && processo->num_assuntos < 5) {
                        if (isdigit(token[0])) {
                            processo->id_assuntos[processo->num_assuntos++] = atoi(token);
                        }
                        token = strtok(NULL, "{},");
                    }
                    break;
            }
            campo++;
            j = 0;
            memset(temp, 0, sizeof(temp));
            i++;
            continue;
        }
        
        if (j < sizeof(temp)-1) {
            temp[j++] = linha[i];
        }
        i++;
    }
    
    processo->ano_eleicao = atoi(temp);
    return 0;
}

int lerProcessosDeArquivo(const char *nomeArquivo, Processo **processos) {
    clock_t inicio = clock();
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir arquivo %s: %s\n", nomeArquivo, strerror(errno));
        return -1;
    }

    char linha[1024];
    int tamanho = 0;

    if (!fgets(linha, sizeof(linha), arquivo)) {
        fprintf(stderr, "Arquivo vazio ou cabeçalho faltando\n");
        fclose(arquivo);
        return -1;
    }

    while (fgets(linha, sizeof(linha), arquivo)) {
        tamanho++;
    }

    if (tamanho == 0) {
        fprintf(stderr, "Nenhum processo encontrado no arquivo\n");
        fclose(arquivo);
        return 0;
    }

    rewind(arquivo);
    fgets(linha, sizeof(linha), arquivo);

       *processos = (Processo *)malloc(tamanho * sizeof(Processo));
    if (!*processos) {
        fprintf(stderr, "Erro ao alocar memoria para %d processos\n", tamanho);
        fclose(arquivo);
        return -1;
    }

        for (int i = 0; i < tamanho; i++) {
        if (!fgets(linha, sizeof(linha), arquivo)) {
            fprintf(stderr, "Erro ao ler linha %d\n", i+2);
            tamanho = i;
            break;
        }
        if (parseLinhaProcesso(linha, &(*processos)[i])) {
            fprintf(stderr, "Erro ao parsear linha %d\n", i+2);
            tamanho = i;
            break;
        }
    }

    fclose(arquivo);
    mostrarTempoProcessamento((double)(clock() - inicio) / CLOCKS_PER_SEC, "Leitura do arquivo");
    return tamanho;
}

void ordenarPorId(Processo *processos, int tamanho) {
    clock_t inicio = clock();
    
    for (int i = 0; i < tamanho - 1; i++) {
        for (int j = 0; j < tamanho - i - 1; j++) {
            if (processos[j].id > processos[j + 1].id) {
                Processo temp = processos[j];
                processos[j] = processos[j + 1];
                processos[j + 1] = temp;
            }
        }
    }
    mostrarTempoProcessamento((double)(clock() - inicio) / CLOCKS_PER_SEC, "Ordenacao por ID");
}

void ordenarPorDataAjuizamento(Processo *processos, int tamanho) {
    clock_t inicio = clock();
    
    for (int i = 0; i < tamanho - 1; i++) {
        for (int j = 0; j < tamanho - i - 1; j++) {
            if (processos[j].timestamp < processos[j + 1].timestamp) {
                Processo temp = processos[j];
                processos[j] = processos[j + 1];
                processos[j + 1] = temp;
            }
        }
    }
    mostrarTempoProcessamento((double)(clock() - inicio) / CLOCKS_PER_SEC, "Ordenacao por data");
}

int contarProcessosPorClasse(const Processo *processos, int tamanho, int id_classe) {
    int count = 0;
    for (int i = 0; i < tamanho; i++) {
        if (processos[i].id_classe == id_classe) {
            count++;
        }
    }
    return count;
}

int contarAssuntosUnicos(const Processo *processos, int tamanho) {
    if (tamanho <= 0) return 0;
    
    
    int max_assuntos = tamanho * 5; 
    int *assuntos = (int *)malloc(max_assuntos * sizeof(int));
    if (!assuntos) {
        fprintf(stderr, "Erro ao alocar memoria para assuntos\n");
        return -1;
    }
    
    
    int total_assuntos = 0;
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < processos[i].num_assuntos; j++) {
            if (total_assuntos < max_assuntos) {
                assuntos[total_assuntos++] = processos[i].id_assuntos[j];
            }
        }
    }
    
    if (total_assuntos == 0) {
        free(assuntos);
        return 0;
    }
    
    
    for (int i = 0; i < total_assuntos - 1; i++) {
        for (int j = 0; j < total_assuntos - i - 1; j++) {
            if (assuntos[j] > assuntos[j + 1]) {
                int temp = assuntos[j];
                assuntos[j] = assuntos[j + 1];
                assuntos[j + 1] = temp;
            }
        }
    }
    
    
    int unicos = 1;
    for (int i = 1; i < total_assuntos; i++) {
        if (assuntos[i] != assuntos[i-1]) {
            unicos++;
        }
    }
    
    free(assuntos);
    return unicos;
}

void listarProcessosComMultiplosAssuntos(const Processo *processos, int tamanho) {
    printf("\n=== Processos com multiplos assuntos ===\n");
    int total_multiplos = 0;
    
    
    for (int i = 0; i < tamanho; i++) {
        if (processos[i].num_assuntos > 1) {
            total_multiplos++;
        }
    }
    
    if (total_multiplos == 0) {
        printf("Nenhum processo com multiplos assuntos encontrado.\n");
        return;
    }
    
    
    for (int i = 0; i < tamanho; i++) {
        if (processos[i].num_assuntos > 1) {
            printf("ID: %ld | Numero: %s | Data: %s | Assuntos: ", 
                  processos[i].id, 
                  processos[i].numero,
                  processos[i].data_ajuizamento);
            
            for (int j = 0; j < processos[i].num_assuntos; j++) {
                printf("%d", processos[i].id_assuntos[j]);
                if (j < processos[i].num_assuntos - 1) printf(", ");
            }
            printf("\n");
        }
    }
    
    printf("\nTotal de processos com multiplos assuntos: %d\n", total_multiplos);
}

Processo* buscarProcessoPorId(const Processo *processos, int tamanho, long int id) {
    for (int i = 0; i < tamanho; i++) {
        if (processos[i].id == id) {
            return (Processo*)&processos[i];
        }
    }
    return NULL;
}

int calcularDiasTramitacao(time_t timestamp_ajuizamento) {
    if (timestamp_ajuizamento == -1) return -1;
    
    time_t agora = time(NULL);
    double segundos = difftime(agora, timestamp_ajuizamento);
    return (int)(segundos / (60 * 60 * 24));
}

int salvarProcessosEmArquivo(const char *nomeArquivo, const Processo *processos, int tamanho) {
    clock_t inicio = clock();
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (!arquivo) {
        fprintf(stderr, "Erro ao criar arquivo %s: %s\n", nomeArquivo, strerror(errno));
        return -1;
    }
    
    fprintf(arquivo, "\"id\",\"numero\",\"data_ajuizamento\",\"id_classe\",\"id_assunto\",\"ano_eleicao\"\n");
    
    for (int i = 0; i < tamanho; i++) {
       
        char assuntos_str[256] = "{";
        for (int j = 0; j < processos[i].num_assuntos; j++) {
            char temp[20];
            sprintf(temp, "%d", processos[i].id_assuntos[j]);
            strcat(assuntos_str, temp);
            if (j < processos[i].num_assuntos - 1) {
                strcat(assuntos_str, ",");
            }
        }
        strcat(assuntos_str, "}");
        
        if (fprintf(arquivo, "\"%ld\",\"%s\",%s,{%d},%s,%d\n",
                processos[i].id,
                processos[i].numero,
                processos[i].data_ajuizamento,
                processos[i].id_classe,
                assuntos_str,
                processos[i].ano_eleicao) < 0) {
            fclose(arquivo);
            return -1;
        }
    }
    
    fclose(arquivo);
    mostrarTempoProcessamento((double)(clock() - inicio) / CLOCKS_PER_SEC, "Gravacao do arquivo");
    return 0;
}

void verificarArquivo(const char *nomeArquivo) {
    FILE *f = fopen(nomeArquivo, "r");
    if (f == NULL) {
        printf("[ERRO] Arquivo %s não foi criado!\n", nomeArquivo);
        return;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    
    if (size > 0) {
        printf("[OK] Arquivo %s criado com sucesso (%ld bytes)\n", nomeArquivo, size);
    } else {
        printf("[ERRO] Arquivo %s está vazio!\n", nomeArquivo);
    }
}

void liberarProcessos(Processo *processos) {
    if (processos != NULL) {
        free(processos);
    }
}
