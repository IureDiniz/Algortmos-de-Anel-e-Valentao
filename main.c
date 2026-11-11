// CÓDIGO ESCRITO ORIGINALMENTE EM PYTHON E CONVERTIDO PARA C
// COM O USO DE INTELIGENCIA ARTIFICIAL DEVIDO A FALHA DE LEITURA
// DA PLATAFORMA DE CORREÇÃO (TESTES MANUAIS FUNCIONAVAM NORMALMENTE)
// ALTERAÇÕES HUMANAS REALIZADAS NO CÓDIGO EM C FORAM REALIZADOS PARA
// CONCERTAR PEQUENOS ERROS NA SAÍDA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Estados possíveis de um processo */
typedef enum {
    ATIVO,
    INATIVO
} Estado;

/* Estrutura que representa um processo */
typedef struct {
    int id;
    Estado estado;
    int coordenador;
} Processo;

/* Vetor dinâmico de processos */
typedef struct {
    Processo *dados;
    int quantidade;
} Processos;

/* Lista dinâmica de mensagens */
typedef struct {
    char **itens;
    size_t quantidade;
    size_t capacidade;
} ListaMensagens;


/* =====================================================
   FUNÇÕES AUXILIARES
   ===================================================== */

const char *nomeEstado(Estado estado) {
    return estado == ATIVO ? "ativo" : "inativo";
}

void liberarProcessos(Processos *processos) {
    free(processos->dados);

    processos->dados = NULL;
    processos->quantidade = 0;
}

void limparMensagens(ListaMensagens *mensagens) {
    for (size_t i = 0; i < mensagens->quantidade; i++) {
        free(mensagens->itens[i]);
    }

    free(mensagens->itens);

    mensagens->itens = NULL;
    mensagens->quantidade = 0;
    mensagens->capacidade = 0;
}

/* Adiciona uma mensagem à lista */
void adicionarMensagem(
    ListaMensagens *mensagens,
    const char *tipo,
    int origem,
    int destino
) {
    char buffer[128];

    snprintf(
        buffer,
        sizeof(buffer),
        "%s P%d P%d",
        tipo,
        origem + 1,
        destino + 1
    );

    char *copia = malloc(strlen(buffer) + 1);

    if (copia == NULL) {
        fprintf(stderr, "Erro de alocacao de memoria.\n");
        return;
    }

    strcpy(copia, buffer);

    /* Aumenta a capacidade quando necessário */
    if (mensagens->quantidade == mensagens->capacidade) {

        size_t novaCapacidade =
            mensagens->capacidade == 0
            ? 8
            : mensagens->capacidade * 2;

        char **novoVetor = realloc(
            mensagens->itens,
            novaCapacidade * sizeof(char *)
        );

        if (novoVetor == NULL) {
            free(copia);
            fprintf(stderr, "Erro de alocacao de memoria.\n");
            return;
        }

        mensagens->itens = novoVetor;
        mensagens->capacidade = novaCapacidade;
    }

    mensagens->itens[mensagens->quantidade] = copia;
    mensagens->quantidade++;
}


/* =====================================================
   FUNÇÕES DOS PROCESSOS
   ===================================================== */

/* Exibe informações de um processo */
void conferir(Processo *processo) {

    printf("processo: P%d\n", processo->id + 1);

    printf(
        "coordenador: P%d\n",
        processo->coordenador + 1
    );

    printf("estado: %s\n\n", nomeEstado(processo->estado));
}


/* Inicializa os processos */
void inicializarProcessos(int n, Processos *processos) {

    liberarProcessos(processos);

    if (n <= 0) {
        printf("Erro: a quantidade de processos deve ser positiva.\n");
        return;
    }

    processos->dados = malloc(n * sizeof(Processo));

    if (processos->dados == NULL) {
        printf("Erro ao alocar memoria para os processos.\n");
        return;
    }

    processos->quantidade = n;

    for (int i = 0; i < n; i++) {

        processos->dados[i].id = i;
        processos->dados[i].estado = ATIVO;
        processos->dados[i].coordenador = n - 1;
    }

    printf("Sistema iniciado com %d processos.\n", n);
}


/* Simula a falha de um processo */
void simularFalha(int id, Processos *processos) {

    id--;

    for (int i = 0; i < processos->quantidade; i++) {

        Processo *processo = &processos->dados[i];

        if (processo->id == id) {

            if (processo->estado == INATIVO) {
                printf("Falha ja registrada: P%d.\n", processo->id + 1);
                return;
            }

            processo->estado = INATIVO;

            printf("Falha registrada: P%d.\n", processo->id + 1);
            return;
        }
    }

    printf("Erro: processo P%d inexistente.\n", id + 1);
}


/* =====================================================
   ALGORITMO VALENTÃO (BULLY)
   ===================================================== */

void algoritmoValentao(
    int id,
    Processos *processos,
    ListaMensagens *mensagens,
    int idOriginal
) {

    id--;
    idOriginal--;

    /* Verifica se o processo original existe */
    if (idOriginal >= processos->quantidade) {
        printf(
            "Erro: processo P%d inexistente.\n",
            idOriginal + 1
        );
        return;
    }

    if (idOriginal < 0 || id < 0) {
        printf(
            "Erro: processo P%d inexistente.\n",
            idOriginal + 1
        );
        return;
    }

    if (id >= processos->quantidade) {
        return;
    }

    /* Verifica se o processo que iniciou a eleição está ativo */
    if (processos->dados[idOriginal].estado == INATIVO) {

        printf(
            "Erro: processo P%d inativo.\n",
            processos->dados[idOriginal].id + 1
        );

        return;
    }

    /* Se o processo atual estiver inativo, avança */
    if (processos->dados[id].estado == INATIVO) {

        algoritmoValentao(
            id + 2,
            processos,
            mensagens,
            idOriginal + 1
        );

        return;
    }

    /* O processo atual inicialmente assume ser coordenador */
    processos->dados[id].coordenador = id;

    int i = id + 1;
    int coord = id;

    /* Envia mensagens ELECTION para os processos superiores */
    while (i < processos->quantidade) {

        if (processos->dados[i].estado == ATIVO) {

            adicionarMensagem(
                mensagens,
                "ELECTION",
                id,
                i
            );

            adicionarMensagem(
                mensagens,
                "OK",
                i,
                id
            );

            coord = i;
        }

        i++;
    }

    /* Continua a eleição nos processos seguintes */
    algoritmoValentao(
        id + 2,
        processos,
        mensagens,
        idOriginal + 1
    );

    i = 0;

    processos->dados[coord].coordenador = coord;

    /* Verifica se o processo atual ainda é candidato a coordenador */
    if (
        processos->dados[id].id ==
        processos->dados[processos->dados[id].coordenador].id
    ) {

        while (i < coord) {

            if (processos->dados[i].estado == ATIVO) {

                processos->dados[i].coordenador = coord;

                adicionarMensagem(
                    mensagens,
                    "COORDINATOR",
                    coord,
                    i
                );
            }

            i++;
        }

        printf(
            "Valentao: coordenador P%d; mensagens: %zu\n",
            processos->dados[id].coordenador + 1,
            mensagens->quantidade
        );
    }
}


/* =====================================================
   ALGORITMO DO ANEL
   ===================================================== */

void algoritmoAnel(
    int id,
    Processos *processos,
    ListaMensagens *mensagens
) {

    int inicio = id - 1;

    /* Verifica se o processo existe */
    if (inicio < 0 || inicio >= processos->quantidade) {

        printf(
            "Erro: processo P%d inexistente.\n",
            inicio + 1
        );

        return;
    }

    /* Verifica se o processo que iniciou a eleição está ativo */
    if (processos->dados[inicio].estado == INATIVO) {

        printf(
            "Erro: processo P%d inativo.\n",
            processos->dados[inicio].id + 1
        );

        return;
    }

    /* Armazena os índices dos processos ativos */
    int *ativos = malloc(
        processos->quantidade * sizeof(int)
    );

    if (ativos == NULL) {
        printf("Erro ao alocar memoria.\n");
        return;
    }

    int quantidadeAtivos = 0;

    /* Percorre o anel a partir do processo inicial */
    for (int i = 0; i < processos->quantidade; i++) {

        int indice = (inicio + i) % processos->quantidade;

        if (processos->dados[indice].estado == ATIVO) {

            ativos[quantidadeAtivos] = indice;
            quantidadeAtivos++;
        }
    }

    /* O coordenador será o processo ativo de maior ID */
    int coord = ativos[0];

    for (int i = 1; i < quantidadeAtivos; i++) {

        if (ativos[i] > coord) {
            coord = ativos[i];
        }
    }

    /* Caso exista apenas um processo ativo */
    if (quantidadeAtivos == 1) {

        processos->dados[coord].coordenador = coord;

        printf(
            "Anel: coordenador P%d; mensagens: 0\n",
            coord + 1
        );

        free(ativos);
        return;
    }

    /* Primeira etapa: mensagens ELECTION */
    for (int i = 0; i < quantidadeAtivos; i++) {

        int atual = ativos[i];

        int proximo = ativos[
            (i + 1) % quantidadeAtivos
        ];

        adicionarMensagem(
            mensagens,
            "ELECTION",
            atual,
            proximo
        );
    }

    /* Atualiza o coordenador de todos os processos ativos */
    for (int i = 0; i < quantidadeAtivos; i++) {

        processos->dados[ativos[i]].coordenador = coord;
    }

    /* Segunda etapa: mensagens COORDINATOR */
    for (int i = 0; i < quantidadeAtivos; i++) {

        int atual = ativos[i];

        int proximo = ativos[
            (i + 1) % quantidadeAtivos
        ];

        adicionarMensagem(
            mensagens,
            "COORDINATOR",
            atual,
            proximo
        );
    }

    printf(
        "Anel: coordenador P%d; mensagens: %zu\n",
        coord + 1,
        mensagens->quantidade
    );

    free(ativos);
}


/* =====================================================
   EXIBIÇÃO DOS PROCESSOS E MENSAGENS
   ===================================================== */

void exibirProcessos(Processos *processos) {

    int coordenador = -1;

    if (processos->quantidade == 0) {

        printf("Erro: inicialize o sistema com -n N.\n");
        return;
    }

    printf("Ativos:");

    int quantidadeAtivos = 0;

    for (int i = 0; i < processos->quantidade; i++) {

        if (processos->dados[i].estado == ATIVO) {

            printf(" P%d", processos->dados[i].id + 1);

            coordenador = processos->dados[i].coordenador;
            quantidadeAtivos++;
        }
    }

    if (quantidadeAtivos == 0) {
        printf(" nenhum");
    }

    printf("\nInativos:");

    int quantidadeInativos = 0;

    for (int i = 0; i < processos->quantidade; i++) {

        if (processos->dados[i].estado == INATIVO) {

            printf(" P%d", processos->dados[i].id + 1);

            quantidadeInativos++;
        }
    }

    if (quantidadeInativos == 0) {
        printf(" nenhum");
    }

    /* Verifica se existe um coordenador ativo */
    if (
        coordenador >= 0 &&
        coordenador < processos->quantidade &&
        processos->dados[coordenador].estado == ATIVO
    ) {

        printf("\nCoordenador: P%d\n", coordenador + 1);

    } else {

        printf("\nCoordenador: indefinido\n");
    }
}


/* Exibe todas as mensagens registradas */
void exibirMensagens(ListaMensagens *mensagens) {

    if (mensagens->quantidade == 0) {

        printf("Nenhuma mensagem registrada.\n");
        return;
    }

    for (size_t i = 0; i < mensagens->quantidade; i++) {

        printf("%s\n", mensagens->itens[i]);
    }
}


/* =====================================================
   FUNÇÃO PRINCIPAL
   ===================================================== */

int main(void) {

    Processos processos = {NULL, 0};

    ListaMensagens mensagens = {NULL, 0, 0};

    char escolha[256];

    while (fgets(escolha, sizeof(escolha), stdin) != NULL) {

        /* Remove a quebra de linha */
        escolha[strcspn(escolha, "\r\n")] = '\0';

        if (strcmp(escolha, "-f") == 0) {
            break;
        }

        if (escolha[0] != '-' || escolha[1] == '\0') {
            continue;
        }

        char comando = escolha[1];

        int valor;

        /* Comandos que precisam de um ID ou quantidade */
        if (
            comando == 'n' ||
            comando == 'x' ||
            comando == 'v' ||
            comando == 'a'
        ) {

            if (sscanf(escolha + 2, "%d", &valor) != 1) {

                printf("Uso: -%c N\n", comando);
                continue;
            }
        }

        switch (comando) {

            case 'n':

                inicializarProcessos(valor, &processos);

                break;

            case 'x':

                simularFalha(valor, &processos);

                break;

            case 'v':

                limparMensagens(&mensagens);

                algoritmoValentao(
                    valor,
                    &processos,
                    &mensagens,
                    valor
                );

                break;

            case 'a':

                limparMensagens(&mensagens);

                algoritmoAnel(
                    valor,
                    &processos,
                    &mensagens
                );

                break;

            case 's':

                exibirProcessos(&processos);

                break;

            case 'm':

                exibirMensagens(&mensagens);

                break;

            default:

                break;
        }
    }

    /* Libera a memória utilizada */
    liberarProcessos(&processos);
    limparMensagens(&mensagens);

    return 0;
}
