#ifndef CESTA_BASICA_H
#define CESTA_BASICA_H

#include <stddef.h>

#define CESTA_NUM_PRODUTOS 3
#define CESTA_NOME_MAX 64
#define CESTA_EPSILON 1e-9

typedef enum {
    SITUACAO_AUMENTO = 1,
    SITUACAO_QUEDA = -1,
    SITUACAO_ESTAVEL = 0
} SituacaoPreco;

typedef enum {
    CESTA_OK = 0,
    CESTA_ERR_ARGUMENTO_NULO = 1,
    CESTA_ERR_PRECO_ANTERIOR_INVALIDO = 2,
    CESTA_ERR_PRECO_ATUAL_INVALIDO = 3,
    CESTA_ERR_NOME_VAZIO = 4,
    CESTA_ERR_LISTA_VAZIA = 5
} CestaStatus;

typedef struct {
    char nome[CESTA_NOME_MAX];
    double preco_anterior;
    double preco_atual;
} ProdutoCesta;

typedef struct {
    ProdutoCesta produto;
    double diferenca_absoluta;
    double variacao_percentual;
    SituacaoPreco situacao;
} ResultadoProduto;

typedef struct {
    double total_anterior;
    double total_atual;
    double variacao_global_percentual;
    double variacao_media_percentual;
    size_t qtd_aumento;
    size_t qtd_queda;
    size_t qtd_estavel;
} ResumoCesta;

CestaStatus cesta_analisar_produto(const ProdutoCesta *produto, ResultadoProduto *resultado);
CestaStatus cesta_analisar_lote(const ProdutoCesta *produtos,
                                size_t quantidade,
                                ResultadoProduto *resultados,
                                ResumoCesta *resumo);
const char *cesta_situacao_texto(SituacaoPreco situacao);
const char *cesta_status_mensagem(CestaStatus status);

#endif
