#include "cesta_basica.h"

#include <math.h>
#include <string.h>

static SituacaoPreco classificar_variacao(double variacao_percentual) {
    if (variacao_percentual > CESTA_EPSILON) {
        return SITUACAO_AUMENTO;
    }
    if (variacao_percentual < -CESTA_EPSILON) {
        return SITUACAO_QUEDA;
    }
    return SITUACAO_ESTAVEL;
}

CestaStatus cesta_analisar_produto(const ProdutoCesta *produto, ResultadoProduto *resultado) {
    if (produto == NULL || resultado == NULL) {
        return CESTA_ERR_ARGUMENTO_NULO;
    }
    if (produto->nome[0] == '\0') {
        return CESTA_ERR_NOME_VAZIO;
    }
    if (!isfinite(produto->preco_anterior) || produto->preco_anterior <= 0.0) {
        return CESTA_ERR_PRECO_ANTERIOR_INVALIDO;
    }
    if (!isfinite(produto->preco_atual) || produto->preco_atual <= 0.0) {
        return CESTA_ERR_PRECO_ATUAL_INVALIDO;
    }

    double diferenca = produto->preco_atual - produto->preco_anterior;
    double variacao = (diferenca / produto->preco_anterior) * 100.0;

    /* Evita exibicao de -0.00% quando a diferenca cai dentro da tolerancia. */
    if (fabs(variacao) <= CESTA_EPSILON) {
        variacao = 0.0;
        diferenca = 0.0;
    }

    resultado->produto = *produto;
    resultado->diferenca_absoluta = diferenca;
    resultado->variacao_percentual = variacao;
    resultado->situacao = classificar_variacao(variacao);

    return CESTA_OK;
}

CestaStatus cesta_analisar_lote(const ProdutoCesta *produtos,
                                size_t quantidade,
                                ResultadoProduto *resultados,
                                ResumoCesta *resumo) {
    if (produtos == NULL || resultados == NULL || resumo == NULL) {
        return CESTA_ERR_ARGUMENTO_NULO;
    }
    if (quantidade == 0) {
        return CESTA_ERR_LISTA_VAZIA;
    }

    memset(resumo, 0, sizeof(*resumo));
    double soma_variacoes = 0.0;

    for (size_t i = 0; i < quantidade; i++) {
        CestaStatus st = cesta_analisar_produto(&produtos[i], &resultados[i]);
        if (st != CESTA_OK) {
            return st;
        }

        resumo->total_anterior += produtos[i].preco_anterior;
        resumo->total_atual += produtos[i].preco_atual;
        soma_variacoes += resultados[i].variacao_percentual;

        switch (resultados[i].situacao) {
            case SITUACAO_AUMENTO:
                resumo->qtd_aumento++;
                break;
            case SITUACAO_QUEDA:
                resumo->qtd_queda++;
                break;
            case SITUACAO_ESTAVEL:
                resumo->qtd_estavel++;
                break;
        }
    }

    resumo->variacao_media_percentual = soma_variacoes / (double)quantidade;
    resumo->variacao_global_percentual =
        ((resumo->total_atual - resumo->total_anterior) / resumo->total_anterior) * 100.0;

    if (fabs(resumo->variacao_global_percentual) <= CESTA_EPSILON) {
        resumo->variacao_global_percentual = 0.0;
    }
    if (fabs(resumo->variacao_media_percentual) <= CESTA_EPSILON) {
        resumo->variacao_media_percentual = 0.0;
    }

    return CESTA_OK;
}

const char *cesta_situacao_texto(SituacaoPreco situacao) {
    switch (situacao) {
        case SITUACAO_AUMENTO:
            return "AUMENTO";
        case SITUACAO_QUEDA:
            return "QUEDA";
        case SITUACAO_ESTAVEL:
            return "ESTAVEL";
    }
    return "DESCONHECIDO";
}

const char *cesta_status_mensagem(CestaStatus status) {
    switch (status) {
        case CESTA_OK:
            return "Analise concluida com sucesso.";
        case CESTA_ERR_ARGUMENTO_NULO:
            return "Argumento nulo recebido.";
        case CESTA_ERR_PRECO_ANTERIOR_INVALIDO:
            return "Preco anterior deve ser um numero finito maior que zero.";
        case CESTA_ERR_PRECO_ATUAL_INVALIDO:
            return "Preco atual deve ser um numero finito maior que zero.";
        case CESTA_ERR_NOME_VAZIO:
            return "Nome do produto nao pode estar vazio.";
        case CESTA_ERR_LISTA_VAZIA:
            return "Lista de produtos nao pode estar vazia.";
    }
    return "Erro desconhecido.";
}
