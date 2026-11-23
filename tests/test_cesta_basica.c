#include "../src/cesta_basica.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static int aprox_igual(double a, double b, double tol) {
    return fabs(a - b) <= tol;
}

static void test_classificacao_aumento(void) {
    ProdutoCesta p = {"Arroz 5kg", 25.00, 28.50};
    ResultadoProduto r;

    assert(cesta_analisar_produto(&p, &r) == CESTA_OK);
    assert(r.situacao == SITUACAO_AUMENTO);
    assert(aprox_igual(r.diferenca_absoluta, 3.50, 1e-6));
    assert(aprox_igual(r.variacao_percentual, 14.0, 1e-6));
    assert(strcmp(cesta_situacao_texto(r.situacao), "AUMENTO") == 0);
}

static void test_classificacao_queda(void) {
    ProdutoCesta p = {"Feijao 1kg", 8.50, 7.65};
    ResultadoProduto r;

    assert(cesta_analisar_produto(&p, &r) == CESTA_OK);
    assert(r.situacao == SITUACAO_QUEDA);
    assert(aprox_igual(r.diferenca_absoluta, -0.85, 1e-6));
    assert(aprox_igual(r.variacao_percentual, -10.0, 1e-6));
    assert(strcmp(cesta_situacao_texto(r.situacao), "QUEDA") == 0);
}

static void test_classificacao_estavel(void) {
    ProdutoCesta p = {"Oleo de Soja", 6.40, 6.40};
    ResultadoProduto r;

    assert(cesta_analisar_produto(&p, &r) == CESTA_OK);
    assert(r.situacao == SITUACAO_ESTAVEL);
    assert(r.diferenca_absoluta == 0.0);
    assert(r.variacao_percentual == 0.0);
    assert(strcmp(cesta_situacao_texto(r.situacao), "ESTAVEL") == 0);
}

static void test_epsilon_precisao_flutuante(void) {
    /* 0.1 + 0.2 em IEEE-754 difere de 0.3 por ~5.55e-17; deve ser ESTAVEL. */
    ProdutoCesta p = {"Item Precisao", 0.3, 0.1 + 0.2};
    ResultadoProduto r;

    assert(cesta_analisar_produto(&p, &r) == CESTA_OK);
    assert(r.situacao == SITUACAO_ESTAVEL);
    assert(r.variacao_percentual == 0.0);
}

static void test_validacoes_erro(void) {
    ProdutoCesta p_zero = {"Arroz", 0.0, 10.0};
    ProdutoCesta p_neg_ant = {"Arroz", -5.0, 10.0};
    ProdutoCesta p_neg_atu = {"Arroz", 10.0, 0.0};
    ProdutoCesta p_vazio = {"", 10.0, 12.0};
    ResultadoProduto r;

    assert(cesta_analisar_produto(NULL, &r) == CESTA_ERR_ARGUMENTO_NULO);
    assert(cesta_analisar_produto(&p_zero, NULL) == CESTA_ERR_ARGUMENTO_NULO);
    assert(cesta_analisar_produto(&p_zero, &r) == CESTA_ERR_PRECO_ANTERIOR_INVALIDO);
    assert(cesta_analisar_produto(&p_neg_ant, &r) == CESTA_ERR_PRECO_ANTERIOR_INVALIDO);
    assert(cesta_analisar_produto(&p_neg_atu, &r) == CESTA_ERR_PRECO_ATUAL_INVALIDO);
    assert(cesta_analisar_produto(&p_vazio, &r) == CESTA_ERR_NOME_VAZIO);
}

static void test_analise_lote_e_resumo(void) {
    ProdutoCesta lote[CESTA_NUM_PRODUTOS] = {
        {"Arroz 5kg", 25.00, 28.50},   /* +14.00% */
        {"Feijao 1kg", 8.50, 7.65},    /* -10.00% */
        {"Oleo 900ml", 6.50, 6.50}     /*   0.00% */
    };
    ResultadoProduto resultados[CESTA_NUM_PRODUTOS];
    ResumoCesta resumo;

    assert(cesta_analisar_lote(lote, CESTA_NUM_PRODUTOS, resultados, &resumo) == CESTA_OK);
    assert(aprox_igual(resumo.total_anterior, 40.00, 1e-6));
    assert(aprox_igual(resumo.total_atual, 42.65, 1e-6));
    assert(aprox_igual(resumo.variacao_global_percentual, 6.625, 1e-6));
    assert(aprox_igual(resumo.variacao_media_percentual, 4.0 / 3.0, 1e-6));
    assert(resumo.qtd_aumento == 1);
    assert(resumo.qtd_queda == 1);
    assert(resumo.qtd_estavel == 1);

    assert(cesta_analisar_lote(lote, 0, resultados, &resumo) == CESTA_ERR_LISTA_VAZIA);
}

int main(void) {
    test_classificacao_aumento();
    test_classificacao_queda();
    test_classificacao_estavel();
    test_epsilon_precisao_flutuante();
    test_validacoes_erro();
    test_analise_lote_e_resumo();

    printf("[OK] Todos os testes unitarios de cesta_basica passaram.\n");
    return 0;
}
