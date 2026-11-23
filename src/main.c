#include "cesta_basica.h"

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool ler_linha(char *buffer, size_t tamanho) {
    if (fgets(buffer, (int)tamanho, stdin) == NULL) {
        return false;
    }
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
        buffer[--len] = '\0';
    }
    return true;
}

static bool ler_nome_produto(const char *prompt, char *destino, size_t tamanho) {
    char linha[128];
    for (;;) {
        printf("%s", prompt);
        if (!ler_linha(linha, sizeof(linha))) {
            return false;
        }
        char *inicio = linha;
        while (*inicio != '\0' && isspace((unsigned char)*inicio)) {
            inicio++;
        }
        size_t len = strlen(inicio);
        while (len > 0 && isspace((unsigned char)inicio[len - 1])) {
            inicio[--len] = '\0';
        }
        if (len == 0) {
            printf("  [AVISO] Informe um nome valido para o produto.\n");
            continue;
        }
        if (len >= tamanho) {
            len = tamanho - 1;
        }
        memcpy(destino, inicio, len);
        destino[len] = '\0';
        return true;
    }
}

static bool ler_preco_positivo(const char *prompt, double *valor) {
    char linha[128];
    for (;;) {
        printf("%s", prompt);
        if (!ler_linha(linha, sizeof(linha))) {
            return false;
        }

        /* Aceita virgula ou ponto como separador decimal. */
        for (char *p = linha; *p != '\0'; p++) {
            if (*p == ',') {
                *p = '.';
            }
        }

        errno = 0;
        char *fim = NULL;
        double parsed = strtod(linha, &fim);

        while (fim != NULL && *fim != '\0' && isspace((unsigned char)*fim)) {
            fim++;
        }

        if (errno != 0 || fim == linha || (fim != NULL && *fim != '\0') ||
            !isfinite(parsed) || parsed <= 0.0) {
            printf("  [AVISO] Valor invalido. Informe um preco maior que zero (ex.: 24.90).\n");
            continue;
        }

        *valor = parsed;
        return true;
    }
}

static void imprimir_relatorio(const ResultadoProduto *resultados,
                               size_t quantidade,
                               const ResumoCesta *resumo) {
    printf("\n================================================================================\n");
    printf("              MONITORAMENTO DE PRECOS DA CESTA BASICA (RELATORIO)               \n");
    printf("================================================================================\n");
    printf("%-24s | %10s | %10s | %10s | %-10s\n",
           "PRODUTO", "ANTERIOR", "ATUAL", "VARIACAO", "SITUACAO");
    printf("--------------------------------------------------------------------------------\n");

    for (size_t i = 0; i < quantidade; i++) {
        const ResultadoProduto *r = &resultados[i];
        printf("%-24s | R$ %7.2f | R$ %7.2f | %+9.2f%% | %-10s\n",
               r->produto.nome,
               r->produto.preco_anterior,
               r->produto.preco_atual,
               r->variacao_percentual,
               cesta_situacao_texto(r->situacao));
    }

    printf("================================================================================\n");
    printf("Total cesta (mes anterior): R$ %.2f\n", resumo->total_anterior);
    printf("Total cesta (mes atual)   : R$ %.2f\n", resumo->total_atual);
    printf("Variacao global da cesta  : %+.2f%%\n", resumo->variacao_global_percentual);
    printf("Variacao media simples    : %+.2f%%\n", resumo->variacao_media_percentual);
    printf("Resumo classificacoes     : %zu AUMENTO | %zu QUEDA | %zu ESTAVEL\n",
           resumo->qtd_aumento, resumo->qtd_queda, resumo->qtd_estavel);
    printf("================================================================================\n");
}

int main(int argc, char **argv) {
    ProdutoCesta produtos[CESTA_NUM_PRODUTOS];

    if (argc > 1 && strcmp(argv[1], "--demo") == 0) {
        const ProdutoCesta demo[CESTA_NUM_PRODUTOS] = {
            {"Arroz Tipo 1 (5kg)", 25.00, 28.50},
            {"Feijao Carioca (1kg)", 8.50, 7.65},
            {"Oleo de Soja (900ml)", 6.40, 6.40}
        };
        memcpy(produtos, demo, sizeof(produtos));
    } else if (argc > 1 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printf("Uso: %s [--demo | --help]\n", argv[0]);
        printf("  (sem argumentos)  Modo interativo para entrada de %d produtos.\n",
               CESTA_NUM_PRODUTOS);
        printf("  --demo            Executa demonstracao com 3 itens tipicos da cesta basica.\n");
        return EXIT_SUCCESS;
    } else {
        printf("====================================================\n");
        printf("     MONITORAMENTO DE PRECOS DA CESTA BASICA        \n");
        printf("====================================================\n");
        printf("Informe os dados de %d produtos para comparacao mensal.\n\n", CESTA_NUM_PRODUTOS);

        for (size_t i = 0; i < CESTA_NUM_PRODUTOS; i++) {
            printf("--- Produto %zu de %d ---\n", i + 1, CESTA_NUM_PRODUTOS);
            if (!ler_nome_produto("Nome do produto           : ",
                                  produtos[i].nome,
                                  sizeof(produtos[i].nome)) ||
                !ler_preco_positivo("Preco mes anterior (R$)   : ",
                                    &produtos[i].preco_anterior) ||
                !ler_preco_positivo("Preco mes atual (R$)      : ",
                                    &produtos[i].preco_atual)) {
                fprintf(stderr, "\n[ERRO] Entrada encerrada antes da conclusao.\n");
                return EXIT_FAILURE;
            }
            printf("\n");
        }
    }

    ResultadoProduto resultados[CESTA_NUM_PRODUTOS];
    ResumoCesta resumo;
    CestaStatus st = cesta_analisar_lote(produtos, CESTA_NUM_PRODUTOS, resultados, &resumo);
    if (st != CESTA_OK) {
        fprintf(stderr, "[ERRO] %s\n", cesta_status_mensagem(st));
        return EXIT_FAILURE;
    }

    imprimir_relatorio(resultados, CESTA_NUM_PRODUTOS, &resumo);
    return EXIT_SUCCESS;
}
