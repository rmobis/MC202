/**
* Programa: Lab03.c
* Autor: Raphael Mobis Tacla   RA: 157104
* Disciplina: MC202            Turma: F
* Data: 17/09/2014
*
*
*/

#include <stdio.h>
#include "matrizes.h"
#include "balloc.h"

/* StudlyCase rules */
#define ApElemento ap_elemento
#define Elemento elemento
#define Matriz matriz

void erro(char *msg) {
	printf("\n*** %s\n", msg);
	exit(1);
}

/**
 * Pula toda entrada até o final da linha.
 */
void prox_linha(void) {
	scanf("%*[^\n]\n");
}

void inicializa(Matriz *ultraCabeca, int numLinhas, int numColunas) {
	int i;
	ApElemento superCabeca, tempCabeca;

	if (numLinhas < 1 || numLinhas > LCMAX) {
		erro("inicializa: número de linhas inválido");
	} else if (numColunas < 1 || numColunas > LCMAX) {
		erro("inicializa: número de colunas inválido");
	}

	/* Cria a 'ultra' cabeça. */
	ultraHead->nelems = 0;
	ultraHead->nlins  = numLinhas;
	ultraHead->ncols  = numColunas;
	ultraHead->clin   = MALLOC((numLinhas + 1) * sizeof(ApElemento));
	ultraHead->ccol   = MALLOC((numColunas + 1) * sizeof(ApElemento));


	/* Cria a 'super' cabeça. */
	superCabeca = MALLOC(sizeof(Elemento));
	if (superCabeca == NULL) {
		erro("inicializa: memória esgotada");
	}

	superCabeca->lin = numLinhas;
	superCabeca->col = numColunas;
	superCabeca->val = 0;

	superCabeca->esq              =
	superCabeca->dir              =
	superCabeca->ac               =
	superCabeca->ab               =
	ultraCabeca->clin[numLinhas]  =
	ultraCabeca->ccol[numColunas] = superCabeca;


	/* Cria cabeças das linhas. */
	for (i = 0; i < numLinhas; i++) {
		tempCabeca = MALLOC(sizeof(Elemento));
		if (tempCabeca == NULL) {
			erro("inicializa: memória esgotada");
		}

		tempCabeca->lin = i;
		tempCabeca->col = numColunas;
		tempCabeca->val = 0;

		tempCabeca->ac = superCabeca->ac;
		tempCabeca->ab = superCabeca;

		tempCabeca->esq      =
		tempCabeca->dir      =
		tempCabeca->ab->ac   =
		tempCabeca->ac->ab   =
		ultraCabeca->clin[i] = tempCabeca;
	}

	/* Cria cabeças das colunas. */
	for (i = 0; i < numColunas; i++) {
		tempCabeca = MALLOC(sizeof(Elemento));
		if (tempCabeca == NULL) {
			erro("inicializa: memória esgotada");
		}

		tempCabeca->lin = numLinhas;
		tempCabeca->col = i;
		tempCabeca->val = 0;

		tempCabeca->esq = superCabeca->esq;
		tempCabeca->dir = superCabeca;

		tempCabeca->ac       =
		tempCabeca->ab       =
		tempCabeca->esq->dir =
		tempCabeca->dir->esq =
		ultraCabeca->ccol[i] = tempCabeca;
	}
}

void libera(Matriz *ultraCabeca) {
	int i;
	ApElemento tempElemento;

	/* Rodando a lista para a direita, apagamos sempre o elemento da esquerda. */
	for (i = 0; i < ultraCabeca->nlins; i++) {
		/* Pulamos até que o nó cabeça da linha não seja o da esquerda. */
		tempElemento = ultraCabeca->clin[i]->dir->dir;

		while (tempElemento->col != ultraCabeca->ncols) {
			FREE(tempElemento->esq);
		}

		/* Libera o nó cabeça da linha. */
		FREE(tempElemento);
	}

	FREE(ultraCabeca->clin);
	FREE(ultraCabeca->ccol);
	FREE(ultraCabeca);
}

void encontra(matriz *ultraCabeca, int i, int j, ApElemento *apApLinha, ApElemento *apApColuna) {
	/* Função auxiliar: se o elemento "a[i,j]" existe,
	devolve seu endereço em "*ppl" e "*ppc".  Caso contrário,
	devolve nessas variáveis os endereços dos seus sucessores
	imediatos na linha e coluna (possivelmente as respectivas
	cabeças). */

	ApElemento apLinha, apColuna;

	if (i < 0 || i >= ultraCabeca->nlins) {
		erro("encontra: linha inválida");
	} else if (j < 0 || j >= ultraCabeca->ncols) {
		erro("encontra: coluna inválida");
	}

	/* Procura elemento "pl" na linha "i": */
	pl = a->clin[i];
	pl = pl->dir; while (pl->col < j) { pl = pl->dir; }
	if (pl->col == j)
	{ pc = pl; }
	else
	{ /* Procura elemento "pc" na coluna "j": */
		pc = a->ccol[j];
		pc = pc->ab; while (pc->lin < i) { pc =  pc->ab; }
		/* Consistência: */
		if (pc == pl) { erro("encontra: apontadores pirados"); }
	}
	if ((pl->lin != i) || (pl->col < j)) { erro("encontra: pl inconsistente"); }
	if ((pc->col != j) || (pc->lin < i)) { erro("encontra: pc inconsistente"); }
	(*ppl) = pl; (*ppc) = pc;
}

float valor(matriz *a, int i, int j) {
	ApElemento pl, pc;
	encontra(a, i, j, &pl, &pc);
	if (pl != pc)
	{ return 0; }
	else
	{ return pl->val; }
}

void remove_elem(matriz *a, ApElemento r) {
	/* Função auxiliar: elimina um elemento "r" da matriz "a". */
	erro("!!!COMPLETAR");
	FREE(r);
	a->nelems--;
}

void insere_elem(matriz *a, int i, int j, float v, ApElemento pl, ApElemento pc) {
	/* Função auxiliar: insere um elemento de valor "v" e índices "[i,j]",
	dadas as posições dos sucessores do elemento "a[i,j]" na linha e
	coluna, respectivamente (possivelmente cabeças).  Supõe que o
	elemento não está na matriz, e que "v != 0". */
	ApElemento r;
	if (v == 0) { erro("insere: elemento nulo"); }
	if (pl->col < j) { erro("insere: pl inconsistente"); }
	if (pc->lin < i) { erro("insere: pc inconsistente"); }
	r = (ApElemento)MALLOC(sizeof(elemento));
	r->lin = i;
	r->col = j;
	r->val = v;
	/* Insere o elemento nas listas da linha e da coluna: */
	erro("!!!COMPLETAR");
	a->nelems++;
}

void atribui(matriz *a, int i, int j, float v) {

	ApElemento pl, pc;
	encontra(a, i, j, &pl, &pc);
	if (pl != pc) {
	/* Elemento não existe; se "v" não é nulo, precisa encaixar: */
		if (v != 0.0) { insere_elem(a, i, j, v, pl, pc); }
	} else {
	/* Elemento existe; se "v" é nulo, precisa remover, senão alterar: */
		if (v != 0.0) { pl->val = v; } else { remove_elem(a, pl); }
	}
}

void le_matriz(matriz *a) {
	int m, n, d;
	int i, j; float v;
	int ip, jp; /* Índices do elemento anterior. */
	int k;

	scanf("%d %d %d", &m, &n, &d); prox_linha();
	inicializa(a, m, n);
	ip = -1; jp = n-1;
	for (k = 0; k < d; k++)
	{ scanf("%d %d %f", &i, &j, &v); prox_linha();
		if (v != 0) { insere_elem(a, i, j, v, a->clin[i], a->ccol[j]); }
	}
}

void imprime_matriz(matriz *a) {
	int i;
	printf("%d %d %d\n", a->nlins, a->ncols, a->nelems);
	for (i = 0; i < a->nlins; i++)
	{ elemento *p = a->clin[i]->dir;
		while (p->col < a->ncols)
		{ printf("  %2d %2d %8.2f\n", p->lin, p->col, p->val);
			p = p->dir;
		}
	}
}

void transpoe(matriz *a, matriz *t) {
	int i;
	inicializa(t, a->ncols, a->nlins);
	erro("!!!COMPLETAR");
}

void soma(matriz *a, matriz *b, matriz *s) {
	int i, j; float v;
	/* Verifica se possuem mesmas dimensões: */
	if ((a->nlins != b->nlins) || (a->ncols != b->ncols))
		erro("soma: matrizes com tamanhos diferentes");
	/* Inicializa matriz "s" com mesmas dimensoes de "a" e "b": */
	inicializa(s, a->nlins, a->ncols);
	erro("!!!COMPLETAR");
}

void multiplica(matriz *a, matriz *b, matriz *p) {
	int i, j;
	if ((a->ncols) != (b->nlins)) { erro("multiplica: tamanhos invalidos"); }
	inicializa(p, a->nlins, b->ncols);
	erro("!!!COMPLETAR");
}
