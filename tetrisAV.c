#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
    Trabalho: Tetris Stack - Versão Mestre
    Nome: Jonas da Rocha Lima
*/

typedef struct {
    char tipo;
    int id;
} Peca;

/* ----------- Fila Circular ----------- */
#define MAX_FILA 5

typedef struct {
    Peca itens[MAX_FILA];
    int inicio;
    int fim;
    int qtd;
} Fila;

/* ----------- Pilha ----------- */
#define MAX_PILHA 3

typedef struct {
    Peca itens[MAX_PILHA];
    int topo;
} Pilha;

/* ----------- Histórico para desfazer ----------- */
typedef struct {
    Fila fila;
    Pilha pilha;
    int valido;  
} Historico;


/* Gera peça aleatória */
Peca gerarPeca() {
    char tipos[7] = {'I','O','T','S','Z','J','L'};
    Peca p;
    p.tipo = tipos[rand() % 7];
    p.id = rand() % 10000;
    return p;
}

/* ----------- Fila ----------- */
void iniciarFila(Fila *f) {
    f->inicio = 0;
    f->fim = 0;
    f->qtd = 0;

    for (int i = 0; i < MAX_FILA; i++) {
        f->itens[f->fim] = gerarPeca();
        f->fim = (f->fim + 1) % MAX_FILA;
        f->qtd++;
    }
}

void enqueue(Fila *f, Peca x) {
    if (f->qtd == MAX_FILA) return;
    f->itens[f->fim] = x;
    f->fim = (f->fim + 1) % MAX_FILA;
    f->qtd++;
}

Peca dequeue(Fila *f) {
    Peca x = f->itens[f->inicio];
    f->inicio = (f->inicio + 1) % MAX_FILA;
    f->qtd--;
    return x;
}

void mostrarFila(Fila f) {
    printf("\nFila: ");
    for (int i = 0; i < f.qtd; i++) {
        int pos = (f.inicio + i) % MAX_FILA;
        printf("[%c %d] ", f.itens[pos].tipo, f.itens[pos].id);
    }
    printf("\n");
}

/* ----------- Pilha ----------- */
void iniciarPilha(Pilha *p) {
    p->topo = -1;
}

int push(Pilha *p, Peca x) {
    if (p->topo == MAX_PILHA - 1)
        return 0;
    p->topo++;
    p->itens[p->topo] = x;
    return 1;
}

int pop(Pilha *p, Peca *x) {
    if (p->topo == -1)
        return 0;
    *x = p->itens[p->topo];
    p->topo--;
    return 1;
}

void mostrarPilha(Pilha p) {
    printf("Pilha: ");
    for (int i = p.topo; i >= 0; i--) {
        printf("[%c %d] ", p.itens[i].tipo, p.itens[i].id);
    }
    printf("\n");
}

/* ----------- Histórico (Undo) ----------- */
void salvarHistorico(Historico *h, Fila f, Pilha p) {
    h->fila = f;
    h->pilha = p;
    h->valido = 1;
}

void desfazer(Historico *h, Fila *f, Pilha *p) {
    if (!h->valido) {
        printf("Nada para desfazer!\n");
        return;
    }
    *f = h->fila;
    *p = h->pilha;
    h->valido = 0; // só permite 1 undo
    printf("Desfeito!\n");
}

/* ----------- Trocar e Inverter ----------- */

void trocar(Fila *f, Pilha *p) {
    if (p->topo == -1) {
        printf("Pilha vazia, não tem como trocar.\n");
        return;
    }

    Peca tmp = p->itens[p->topo];
    p->itens[p->topo] = f->itens[f->inicio];
    f->itens[f->inicio] = tmp;
}

void inverter(Fila *f, Pilha *p) {
    Peca temp[MAX_FILA + MAX_PILHA];
    int total = 0;

    // guardar fila
    for (int i = 0; i < f->qtd; i++) {
        int pos = (f->inicio + i) % MAX_FILA;
        temp[total++] = f->itens[pos];
    }

    // guardar pilha
    for (int i = p->topo; i >= 0; i--) {
        temp[total++] = p->itens[i];
    }

    // limpar
    f->inicio = f->fim = 0;
    f->qtd = 0;
    p->topo = -1;

    // recolocar invertido
    for (int i = total - 1; i >= 0; i--) {
        if (f->qtd < MAX_FILA)
            enqueue(f, temp[i]);
        else
            push(p, temp[i]);
    }
}


/* ===================== MAIN ===================== */

int main() {
    srand(time(NULL));

    Fila fila;
    Pilha pilha;
    Historico hist;
    hist.valido = 0;

    iniciarFila(&fila);
    iniciarPilha(&pilha);

    int op;

    do {
        printf("\n==== TETRIS STACK MESTRE ====\n");
        printf("1 - Jogar peça\n");
        printf("2 - Reservar peça\n");
        printf("3 - Usar peça reservada\n");
        printf("4 - Trocar topo da pilha com a fila\n");
        printf("5 - Desfazer última ação\n");
        printf("6 - Inverter fila e pilha\n");
        printf("0 - Sair\n> ");
        scanf("%d", &op);

        if (op >= 1 && op <= 6)
            salvarHistorico(&hist, fila, pilha);

        if (op == 1) {
            Peca jogada = dequeue(&fila);
            printf("Jogou: %c %d\n", jogada.tipo, jogada.id);
            enqueue(&fila, gerarPeca());
        }

        else if (op == 2) {
            Peca p = dequeue(&fila);
            if (push(&pilha, p)) {
                printf("Peça reservada!\n");
                enqueue(&fila, gerarPeca());
            } else {
                printf("Pilha cheia!\n");
            }
        }

        else if (op == 3) {
            Peca p;
            if (pop(&pilha, &p)) {
                printf("Usou reserva: %c %d\n", p.tipo, p.id);
            } else {
                printf("Pilha vazia!\n");
            }
        }

        else if (op == 4) {
            trocar(&fila, &pilha);
        }

        else if (op == 5) {
            desfazer(&hist, &fila, &pilha);
        }

        else if (op == 6) {
            inverter(&fila, &pilha);
        }

        mostrarFila(fila);
        mostrarPilha(pilha);

    } while (op != 0);

    return 0;
}
