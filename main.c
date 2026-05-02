/*
 * Disciplina : Estruturas de Dados
 * Aluno: Guilherme Caleffi Caleffi Pellens Pereira | RGM: 38400227 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * TipoEvento: categorias de eventos criticos monitorados pela cidade.
 * Usado como campo do no da AVL para identificar a natureza do ocorrido.
 */
typedef enum { ACIDENTE, SEMAFORO, ENERGIA, ALAGAMENTO, INCENDIO } TipoEvento;

/*
 * StatusEvento: ciclo de vida de um evento.
 * - ATIVO:    evento em andamento, requer atencao das equipes.
 * - RESOLVIDO: evento encerrado; somente eventos RESOLVIDOS podem ser removidos.
 */
typedef enum { ATIVO, RESOLVIDO } StatusEvento;

/*
 * DataHora: timestamp de registro do evento.
 * Armazenado como campos inteiros separados para facilitar a exibicao formatada.
 */
typedef struct {
    int dia, mes, ano;
    int hora, minuto;
} DataHora;

/*
 * Evento: no da Arvore AVL.
 *
 * Campos de dados do evento:
 *   id         - chave primaria da AVL; criterio de ordenacao da arvore.
 *   tipo       - categoria do evento (enum TipoEvento).
 *   severidade - nivel de urgencia de 1 (baixo) a 5 (critico).
 *   dataHora   - momento do registro.
 *   regiao     - localizacao do evento na cidade (string de ate 49 chars).
 *   status     - ATIVO ou RESOLVIDO.
 *
 * Campos de controle da AVL:
 *   alt        - altura do no; -1 para NULL, 0 para folha.
 *                Usada para calcular o Fator de Balanceamento (FB = h_esq - h_dir).
 *   esq / dir  - ponteiros para os filhos esquerdo e direito.
 */
typedef struct Evento {
    int id;
    TipoEvento tipo;
    int severidade;
    DataHora dataHora;
    char regiao[50];
    StatusEvento status;
    int alt;
    struct Evento *esq;
    struct Evento *dir;
} Evento;

/* Contador global de rotacoes realizadas desde o inicio da execucao.
 * Incrementado em rotacaoLL e rotacaoRR (cada rotacao dupla chama duas simples,
 * portanto conta como 2). Exibido nas metricas do sistema. */
int total_rotacoes = 0;

/*
 * altura: retorna a altura armazenada no no.
 * Convencao: no NULL tem altura -1, folha tem altura 0.
 * Evita desreferenciar ponteiro nulo ao consultar filhos.
 */
int altura(Evento *n) {
    return (n == NULL) ? -1 : n->alt;
}

/*
 * maior: retorna o maior entre dois inteiros.
 * Usada para calcular a altura de um no a partir de seus filhos.
 */
int maior(int a, int b) {
    return (a > b) ? a : b;
}

/*
 * fatorB: calcula o Fator de Balanceamento (FB) de um no.
 * FB = altura(filho esquerdo) - altura(filho direito).
 * Em uma AVL valida, todo no tem -1 <= FB <= 1.
 * FB >= 2  => subarvore esquerda mais pesada (requer rotacao LL ou LR).
 * FB <= -2 => subarvore direita mais pesada  (requer rotacao RR ou RL).
 */
int fatorB(Evento *n) {
    if (n == NULL) return 0;
    return altura(n->esq) - altura(n->dir);
}

/*
 * atualizaAltura: recalcula e armazena a altura de um no com base em seus filhos.
 * Deve ser chamada APOS qualquer modificacao nos filhos do no (insercao,
 * remocao ou rotacao) para manter o campo 'alt' consistente.
 * Altura = max(h_esq, h_dir) + 1.
 */
void atualizaAltura(Evento *n) {
    if (n != NULL) {
        n->alt = maior(altura(n->esq), altura(n->dir)) + 1;
    }
}


/* As rotacoes restauram a propriedade de balanceamento apos insercoes e remocoes.
 * Sao quatro casos possiveis, identificados pelo FB do no desbalanceado e
 * pelo FB de seu filho mais pesado:
 *
 *   FB(raiz) >=  2 e FB(esq) >= 0  =>  Rotacao LL (simples direita)
 *   FB(raiz) <= -2 e FB(dir) <= 0  =>  Rotacao RR (simples esquerda)
 *   FB(raiz) >=  2 e FB(esq) <  0  =>  Rotacao LR (dupla: RR no filho, LL na raiz)
 *   FB(raiz) <= -2 e FB(dir) >  0  =>  Rotacao RL (dupla: LL no filho, RR na raiz) */

/*
 * rotacaoLL: rotacao simples para a direita.
 * Aplicada quando o desbalanceamento esta na subarvore esquerda-esquerda.
 * Retorna o novo no raiz (x) apos a rotacao.
 */
Evento* rotacaoLL(Evento *y) {
    total_rotacoes++;
    Evento *x  = y->esq;   /* novo no raiz */
    Evento *T2 = x->dir;   /* subarvore que muda de pai */

    x->dir = y;   /* y desce para a direita de x */
    y->esq = T2;  /* T2 passa a ser filho esquerdo de y */

    /* Atualiza alturas de baixo para cima (y primeiro, pois agora e filho de x) */
    atualizaAltura(y);
    atualizaAltura(x);
    return x;
}

/*
 * rotacaoRR: rotacao simples para a esquerda.
 * Aplicada quando o desbalanceamento esta na subarvore direita-direita.
 * Retorna o novo no raiz (y) apos a rotacao.
 */
Evento* rotacaoRR(Evento *x) {
    total_rotacoes++;
    Evento *y  = x->dir;   /* novo no raiz */
    Evento *T2 = y->esq;   /* subarvore que muda de pai */

    y->esq = x;   /* x desce para a esquerda de y */
    x->dir = T2;  /* T2 passa a ser filho direito de x */

    atualizaAltura(x);
    atualizaAltura(y);
    return y;
}

/*
 * rotacaoLR: rotacao dupla esquerda-direita.
 * Aplicada quando o desbalanceamento esta na subarvore esquerda-direita.
 * Etapa 1: rotacao RR no filho esquerdo (endireita o "ziguezague").
 * Etapa 2: rotacao LL na raiz.
 */
Evento* rotacaoLR(Evento *z) {
    z->esq = rotacaoRR(z->esq);  /* etapa 1: RR no filho esquerdo */
    return rotacaoLL(z);          /* etapa 2: LL na raiz */
}

/*
 * rotacaoRL: rotacao dupla direita-esquerda.
 * Aplicada quando o desbalanceamento esta na subarvore direita-esquerda.
 * Etapa 1: rotacao LL no filho direito.
 * Etapa 2: rotacao RR na raiz.
 */
Evento* rotacaoRL(Evento *z) {
    z->dir = rotacaoLL(z->dir);  /* etapa 1: LL no filho direito */
    return rotacaoRR(z);          /* etapa 2: RR na raiz */
}

/*
 * criarEvento: aloca e inicializa um novo no Evento.
 * Todo evento nasce com status ATIVO, altura 0 e filhos NULL (no folha).
 * Retorna ponteiro para o no criado, ou NULL em caso de falha na alocacao.
 */
Evento* criarEvento(int id, TipoEvento tipo, int severidade, DataHora dh, const char* regiao) {
    Evento* novo = (Evento*)malloc(sizeof(Evento));
    if (!novo) {
        printf("Erro: falha na alocacao de memoria.\n");
        return NULL;
    }
    novo->id         = id;
    novo->tipo       = tipo;
    novo->severidade = severidade;
    novo->dataHora   = dh;
    strcpy(novo->regiao, regiao);
    novo->status = ATIVO;   /* todo evento comeca como ATIVO */
    novo->alt    = 0;       /* folha recem-inserida tem altura 0 */
    novo->esq    = NULL;
    novo->dir    = NULL;
    return novo;
}

/*
 * inserirAVL: insere um novo no na AVL mantendo o balanceamento.
 *
 * Algoritmo (recursivo):
 *   1. Descida BST: compara IDs para ir para esq ou dir.
 *   2. Caso base: posicao vazia encontrada, insere o no.
 *   3. Subida: atualiza altura e verifica FB.
 *   4. Se |FB| >= 2, identifica o caso (LL/RR/LR/RL) e aplica a rotacao.
 *
 * Em caso de ID duplicado, libera o no novo e retorna a raiz sem modificacao.
 * Retorna a raiz da subarvore (possivelmente nova apos rotacao).
 */
Evento* inserirAVL(Evento *raiz, Evento *novo) {
    /* caso base: posicao vazia — insere aqui */
    if (raiz == NULL) return novo;

    /* descida BST pela chave (id) */
    if (novo->id < raiz->id)
        raiz->esq = inserirAVL(raiz->esq, novo);
    else if (novo->id > raiz->id)
        raiz->dir = inserirAVL(raiz->dir, novo);
    else {
        /* ID duplicado: nao insere e evita vazamento de memoria */
        printf("Erro: ID %d ja existe!\n", novo->id);
        free(novo);
        return raiz;
    }

    /* atualiza altura do no atual apos a insercao no filho */
    atualizaAltura(raiz);
    int fb = fatorB(raiz);

    /* verifica e corrige desbalanceamento */
    if (fb >= 2  && fatorB(raiz->esq) >= 0) return rotacaoLL(raiz); /* caso LL */
    if (fb <= -2 && fatorB(raiz->dir) <= 0) return rotacaoRR(raiz); /* caso RR */
    if (fb >= 2  && fatorB(raiz->esq) <  0) return rotacaoLR(raiz); /* caso LR */
    if (fb <= -2 && fatorB(raiz->dir) >  0) return rotacaoRL(raiz); /* caso RL */

    return raiz; /* no balanceado, retorna sem rotacao */
}

/*
 * buscarPorID: busca um no pelo ID usando a propriedade BST.
 * Complexidade: O(log n) gracas ao balanceamento da AVL.
 * Retorna ponteiro para o no encontrado, ou NULL se nao existir.
 */
Evento* buscarPorID(Evento *raiz, int id) {
    if (raiz == NULL || raiz->id == id) return raiz;
    if (id < raiz->id) return buscarPorID(raiz->esq, id);
    return buscarPorID(raiz->dir, id);
}

/*
 * minimo: retorna o no com menor ID na subarvore.
 * Usado na remocao com dois filhos para encontrar o sucessor em-ordem
 * (menor elemento da subarvore direita), que substitui o no removido.
 */
Evento* minimo(Evento* raiz) {
    Evento* atual = raiz;
    while (atual->esq != NULL) atual = atual->esq;
    return atual;
}

/*
 * removerAVL: remove um no da AVL com rebalanceamento.
 *
 * Regra de negocio: apenas eventos com status RESOLVIDO podem ser removidos.
 * Tentativa de remover evento ATIVO exibe erro e cancela a operacao.
 *
 * Algoritmo (recursivo):
 *   1. Descida BST ate encontrar o no alvo.
 *   2. Verificacao de status (so RESOLVIDO).
 *   3. Remocao BST:
 *      - 0 ou 1 filho: substitui pelo filho existente (ou NULL).
 *      - 2 filhos: copia os dados do sucessor em-ordem (minimo da direita)
 *                  e remove o sucessor recursivamente.
 *   4. Subida: atualiza altura e aplica rotacoes se necessario.
 *
 * O parametro 'sucesso' (ponteiro) informa ao chamador se a remocao ocorreu.
 * Retorna a raiz da subarvore apos a operacao.
 */
Evento* removerAVL(Evento* raiz, int id, int* sucesso) {
    if (raiz == NULL) return raiz; /* ID nao encontrado */

    /* descida BST */
    if (id < raiz->id) {
        raiz->esq = removerAVL(raiz->esq, id, sucesso);
    } else if (id > raiz->id) {
        raiz->dir = removerAVL(raiz->dir, id, sucesso);
    } else {
        /* no encontrado — verifica regra de negocio */
        if (raiz->status == ATIVO) {
            printf("Erro: Apenas eventos RESOLVIDOS podem ser removidos.\n");
            *sucesso = 0;
            return raiz;
        }

        *sucesso = 1;

        if ((raiz->esq == NULL) || (raiz->dir == NULL)) {
            /* caso com 0 ou 1 filho */
            Evento *temp = raiz->esq ? raiz->esq : raiz->dir;
            if (temp == NULL) {
                /* sem filhos: remove a folha */
                temp  = raiz;
                raiz  = NULL;
            } else {
                /* um filho: copia o filho para o no atual e libera o filho */
                *raiz = *temp;
            }
            free(temp);
        } else {
            /* dois filhos: substitui pelos dados do sucessor em-ordem */
            Evento* temp = minimo(raiz->dir);
            raiz->id         = temp->id;
            raiz->tipo       = temp->tipo;
            raiz->severidade = temp->severidade;
            raiz->dataHora   = temp->dataHora;
            strcpy(raiz->regiao, temp->regiao);
            raiz->status = temp->status;
            /* remove o sucessor (que tem no maximo um filho direito) */
            raiz->dir = removerAVL(raiz->dir, temp->id, sucesso);
        }
    }

    if (raiz == NULL) return raiz;

    /* rebalanceamento na subida (mesmo criterio da insercao) */
    atualizaAltura(raiz);
    int fb = fatorB(raiz);

    if (fb >= 2  && fatorB(raiz->esq) >= 0) return rotacaoLL(raiz);
    if (fb >= 2  && fatorB(raiz->esq) <  0) return rotacaoLR(raiz);
    if (fb <= -2 && fatorB(raiz->dir) <= 0) return rotacaoRR(raiz);
    if (fb <= -2 && fatorB(raiz->dir) >  0) return rotacaoRL(raiz);

    return raiz;
}

/*
 * imprimirEvento: exibe os dados de um evento formatados em uma unica linha.
 * Converte os enums para strings legiveis (tipos e status).
 */
void imprimirEvento(Evento *e) {
    if (!e) return;
    const char* tipos[]   = {"Acidente", "Semaforo", "Energia", "Alagamento", "Incendio"};
    const char* statusStr = (e->status == ATIVO) ? "ATIVO" : "RESOLVIDO";
    printf("[ID:%d] %-10s | Sev: %d | Regiao: %-20s | Status: %-9s | %02d/%02d/%d %02d:%02d\n",
           e->id, tipos[e->tipo], e->severidade, e->regiao, statusStr,
           e->dataHora.dia, e->dataHora.mes, e->dataHora.ano,
           e->dataHora.hora, e->dataHora.minuto);
}

/*
 * listarPorSeveridade: percorre toda a arvore em-ordem e imprime eventos
 * ATIVOS cuja severidade esteja no intervalo [min, max].
 * Complexidade: O(n) — precisa visitar todos os nos pois a arvore e
 * ordenada por ID, nao por severidade.
 */
void listarPorSeveridade(Evento* raiz, int min, int max) {
    if (raiz == NULL) return;
    listarPorSeveridade(raiz->esq, min, max);
    if (raiz->status == ATIVO && raiz->severidade >= min && raiz->severidade <= max)
        imprimirEvento(raiz);
    listarPorSeveridade(raiz->dir, min, max);
}

/*
 * relatorioPorRegiao: percorre a arvore em-ordem e imprime eventos
 * ATIVOS da regiao especificada.
 * O percurso em-ordem garante que os resultados saiam ordenados por ID.
 * Complexidade: O(n).
 */
void relatorioPorRegiao(Evento* raiz, const char* regiao) {
    if (raiz == NULL) return;
    relatorioPorRegiao(raiz->esq, regiao);
    if (raiz->status == ATIVO && strcmp(raiz->regiao, regiao) == 0)
        imprimirEvento(raiz);
    relatorioPorRegiao(raiz->dir, regiao);
}

/*
 * buscarPorIntervaloID: lista todos os eventos cujo ID esteja em [minID, maxID].
 * Aproveita a propriedade BST para podar ramos desnecessarios:
 *   - So desce para a esquerda se minID < raiz->id (pode haver nos menores).
 *   - So desce para a direita se maxID > raiz->id (pode haver nos maiores).
 * Complexidade: O(log n + k), onde k e o numero de resultados.
 */
void buscarPorIntervaloID(Evento* raiz, int minID, int maxID) {
    if (raiz == NULL) return;
    if (minID < raiz->id) buscarPorIntervaloID(raiz->esq, minID, maxID);
    if (raiz->id >= minID && raiz->id <= maxID) imprimirEvento(raiz);
    if (maxID > raiz->id) buscarPorIntervaloID(raiz->dir, minID, maxID);
}

/*
 * contarNos: conta o total de nos na arvore via percurso pos-ordem.
 * Retorna 0 para arvore vazia.
 */
int contarNos(Evento* raiz) {
    if (raiz == NULL) return 0;
    return 1 + contarNos(raiz->esq) + contarNos(raiz->dir);
}

/*
 * contarAtivos: conta apenas os eventos com status ATIVO.
 * Percorre toda a arvore pois o status nao afeta a estrutura da AVL.
 */
int contarAtivos(Evento* raiz) {
    if (raiz == NULL) return 0;
    int ativo = (raiz->status == ATIVO) ? 1 : 0;
    return ativo + contarAtivos(raiz->esq) + contarAtivos(raiz->dir);
}

/*
 * somaFatorB: soma os valores absolutos dos fatores de balanceamento de
 * todos os nos. Dividida pelo total de nos, fornece o FB medio da arvore.
 * Em uma AVL perfeita, todos os FBs sao 0 e a media e 0.00.
 */
int somaFatorB(Evento* raiz) {
    if (raiz == NULL) return 0;
    return abs(fatorB(raiz)) + somaFatorB(raiz->esq) + somaFatorB(raiz->dir);
}

/*
 * exibirMetricas: consolida e imprime as metricas de desempenho da AVL.
 *   - Altura: indica o pior caso de busca (O(altura)).
 *   - Total de nos: tamanho atual da arvore.
 *   - Eventos ativos: quantos eventos ainda requerem atencao.
 *   - FB medio: quanto a arvore desvia do balanceamento perfeito (0.00).
 *   - Rotacoes: quantas restruturacoes foram necessarias desde o inicio.
 */
void exibirMetricas(Evento* raiz) {
    int nos = contarNos(raiz);
    printf("\n=== METRICAS DA ARVORE ===\n");
    printf("Altura total da arvore         : %d\n", altura(raiz));
    printf("Numero total de nos            : %d\n", nos);
    printf("Eventos Ativos                 : %d\n", contarAtivos(raiz));
    if (nos > 0) {
        float mediaFB = (float)somaFatorB(raiz) / nos;
        printf("Fator de Balanceamento medio   : %.2f\n", mediaFB);
    }
    printf("Total de rotacoes realizadas   : %d\n", total_rotacoes);
}

/*
 * liberarArvore: percurso pos-ordem que libera todos os nos alocados.
 * Deve ser chamada ao encerrar o programa para evitar vazamento de memoria.
 * Pos-ordem garante que os filhos sejam liberados antes do pai.
 */
void liberarArvore(Evento* r) {
    if (!r) return;
    liberarArvore(r->esq);
    liberarArvore(r->dir);
    free(r);
}

/*
 * lerInteiroValido: le e valida um inteiro dentro do intervalo [min, max].
 * Oferece ate 3 tentativas ao usuario antes de cancelar a operacao.
 * Limpa o buffer de entrada em caso de entrada nao numerica.
 * Retorna o valor valido lido, ou -1 se todas as tentativas falharem.
 */
int lerInteiroValido(const char* msg, int min, int max) {
    int val;
    int tentativas = 3;
    while (tentativas--) {
        printf("%s", msg);
        if (scanf("%d", &val) != 1) {
            printf("Entrada invalida. Digite um numero.\n");
            while (getchar() != '\n'); /* descarta o restante da linha invalida */
            val = min - 1;            /* forca rejeicao na verificacao abaixo */
        }
        if (val >= min && val <= max) return val;
        printf("Valor fora do intervalo permitido (%d a %d).\n", min, max);
    }
    printf("Numero de tentativas excedido. Operacao cancelada.\n");
    return -1; /* sinal de cancelamento para o chamador */
}

/*
 * menuCadastros: gerencia as operacoes de escrita na arvore.
 *   Opcao 1 - Registrar: coleta todos os dados, valida e insere na AVL.
 *   Opcao 2 - Resolver:  altera o status de ATIVO para RESOLVIDO.
 *   Opcao 3 - Remover:   verifica existencia, verifica status e remove da AVL.
 *
 * Recebe ponteiro para ponteiro da arvore pois insercao e remocao podem
 * alterar a raiz (apos rotacoes, a raiz pode mudar).
 */
void menuCadastros(Evento** arvore) {
    int sub;
    printf("\n--- CADASTROS ---\n");
    printf("1. Registrar novo evento\n");
    printf("2. Alterar status para RESOLVIDO\n");
    printf("3. Remover evento resolvido\n");
    printf("0. Voltar\n");
    printf("Escolha: ");
    if (scanf("%d", &sub) != 1) { while(getchar()!='\n'); return; }

    if (sub == 1) {
        /* --- Registro de novo evento --- */
        int id, tipo, sev;
        char regiao[50];
        DataHora dh;

        id   = lerInteiroValido("ID do evento (>0): ", 1, 999999);
        if (id == -1) return;

        tipo = lerInteiroValido("Tipo (0-Acidente, 1-Semaforo, 2-Energia, 3-Alagamento, 4-Incendio): ", 0, 4);
        if (tipo == -1) return;

        sev  = lerInteiroValido("Severidade (1-5): ", 1, 5);
        if (sev == -1) return;

        printf("Data e Hora (DD MM AAAA HH MM): ");
        if (scanf("%d %d %d %d %d", &dh.dia, &dh.mes, &dh.ano, &dh.hora, &dh.minuto) != 5) {
            printf("Data/hora invalida.\n");
            while(getchar()!='\n');
            return;
        }

        /* validacao dos limites de data e hora */
        if (dh.dia  < 1   || dh.dia  > 31  ||
            dh.mes  < 1   || dh.mes  > 12  ||
            dh.ano  < 2000 || dh.ano > 2100 ||
            dh.hora < 0   || dh.hora > 23  ||
            dh.minuto < 0 || dh.minuto > 59) {
            printf("Data ou hora fora do intervalo valido.\n");
            return;
        }

        printf("Regiao: ");
        if (scanf("%49s", regiao) != 1) { while(getchar()!='\n'); return; }

        Evento* novo = criarEvento(id, (TipoEvento)tipo, sev, dh, regiao);
        if (novo) {
            *arvore = inserirAVL(*arvore, novo);
            printf("Evento registrado e arvore balanceada!\n");
        }

    } else if (sub == 2) {
        /* --- Alterar status para RESOLVIDO --- */
        int id = lerInteiroValido("ID do evento a resolver: ", 1, 999999);
        if (id == -1) return;
        Evento* ev = buscarPorID(*arvore, id);
        if (ev) {
            if (ev->status == RESOLVIDO) {
                printf("Evento ja esta RESOLVIDO.\n");
            } else {
                ev->status = RESOLVIDO; /* atualizacao direta no no; nao quebra a AVL */
                printf("Status alterado para RESOLVIDO.\n");
            }
        } else {
            printf("Evento nao encontrado.\n");
        }

    } else if (sub == 3) {
        /* --- Remover evento (somente RESOLVIDO) --- */
        int id = lerInteiroValido("ID do evento a remover: ", 1, 999999);
        if (id == -1) return;

        /* busca previa para dar mensagem de erro mais clara */
        Evento* ev = buscarPorID(*arvore, id);
        if (!ev) {
            printf("Evento nao encontrado.\n");
            return;
        }

        int sucesso = 0;
        *arvore = removerAVL(*arvore, id, &sucesso);
        if (sucesso) printf("Evento removido e arvore rebalanceada.\n");
    }
}

/*
 * menuAtualizacoes: altera a severidade de um evento ATIVO.
 * A operacao e feita diretamente no no (sem reinsercao) porque a severidade
 * nao e a chave de ordenacao da AVL — apenas o ID o e.
 */
void menuAtualizacoes(Evento* arvore) {
    int id, sev;
    printf("\n--- ATUALIZACOES ---\n");
    printf("Atualizar severidade de evento ativo\n");

    id = lerInteiroValido("ID do evento: ", 1, 999999);
    if (id == -1) return;

    Evento* ev = buscarPorID(arvore, id);
    if (!ev) {
        printf("Evento nao encontrado.\n");
        return;
    }
    if (ev->status == RESOLVIDO) {
        printf("Evento ja esta resolvido. Nao e possivel alterar severidade.\n");
        return;
    }

    sev = lerInteiroValido("Nova severidade (1-5): ", 1, 5);
    if (sev == -1) return;
    ev->severidade = sev;
    printf("Severidade atualizada para %d.\n", sev);
}

/*
 * menuConsultas: agrupa as quatro modalidades de consulta.
 *   1. Intervalo de severidade — percurso completo O(n).
 *   2. Relatorio por regiao    — percurso completo O(n), saida em-ordem.
 *   3. Intervalo de IDs        — busca com poda BST O(log n + k).
 *   4. Busca por ID exato      — busca BST O(log n).
 */
void menuConsultas(Evento* arvore) {
    int sub;
    printf("\n--- CONSULTAS ---\n");
    printf("1. Eventos ativos por intervalo de severidade\n");
    printf("2. Relatorio de ativos por regiao\n");
    printf("3. Eventos por intervalo de ID\n");
    printf("4. Buscar evento por ID\n");
    printf("0. Voltar\n");
    printf("Escolha: ");
    if (scanf("%d", &sub) != 1) { while(getchar()!='\n'); return; }

    if (sub == 1) {
        int min, max;
        min = lerInteiroValido("Severidade minima (1-5): ", 1, 5);
        if (min == -1) return;
        max = lerInteiroValido("Severidade maxima (1-5): ", min, 5);
        if (max == -1) return;
        printf("\n--- Eventos com severidade entre %d e %d ---\n", min, max);
        listarPorSeveridade(arvore, min, max);

    } else if (sub == 2) {
        char regiao[50];
        printf("Nome da regiao: ");
        if (scanf("%49s", regiao) != 1) { while(getchar()!='\n'); return; }
        printf("\n--- Eventos ativos na regiao: %s ---\n", regiao);
        relatorioPorRegiao(arvore, regiao);

    } else if (sub == 3) {
        int min, max;
        min = lerInteiroValido("ID minimo: ", 1, 999999);
        if (min == -1) return;
        max = lerInteiroValido("ID maximo: ", min, 999999);
        if (max == -1) return;
        printf("\n--- Eventos com ID entre %d e %d ---\n", min, max);
        buscarPorIntervaloID(arvore, min, max);

    } else if (sub == 4) {
        int id = lerInteiroValido("ID do evento: ", 1, 999999);
        if (id == -1) return;
        Evento* ev = buscarPorID(arvore, id);
        if (ev) imprimirEvento(ev);
        else printf("Evento nao encontrado.\n");
    }
}

/*
 * main: ponto de entrada do programa.
 * Inicializa a arvore como NULL (vazia) e entra no loop principal do menu.
 * O loop continua ate o usuario escolher a opcao 0 (Sair).
 * Ao sair, libera toda a memoria alocada dinamicamente antes de encerrar.
 */
int main() {
    Evento* arvore = NULL; /* arvore comeca vazia */
    int opcao;

    do {
        printf("\n========================================\n");
        printf("  GESTAO DE EVENTOS CRITICOS - SMART CITY\n");
        printf("========================================\n");
        printf("1. Cadastros (Registrar / Resolver / Remover)\n");
        printf("2. Atualizar Severidade\n");
        printf("3. Consultas e Relatorios\n");
        printf("4. Metricas do Sistema\n");
        printf("0. Sair\n");
        printf("Escolha: ");

        if (scanf("%d", &opcao) != 1) {
            while(getchar() != '\n'); /* limpa entrada invalida */
            opcao = -1;              /* forca o caso default */
        }

        switch (opcao) {
            case 1: {
                menuCadastros(&arvore); /* passa endereco da raiz (pode mudar) */
                break;
            }
            case 2: {
                menuAtualizacoes(arvore); /* so leitura/update de campo; raiz nao muda */
                break;
            }
            case 3: {
                menuConsultas(arvore);
                break;
            }
            case 4: {
                exibirMetricas(arvore);
                break;
            }
            case 0: {
                printf("Encerrando sistema...\n");
                break;
            }
            default: {
                printf("Opcao invalida. Tente novamente.\n");
                break;
            }
        }
    } while (opcao != 0);

    liberarArvore(arvore); /* libera a memoria antes de encerrar */
    return 0;
}