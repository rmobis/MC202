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

/* Imprime uma mensagem de erro e encerra a execução do programa, retornando 1. */
void erro(char *msg) {
	printf("\n*** %s\n", msg);
	exit(1);
}

/* Pula toda entrada até o final da linha. */
void prox_linha(void) {
	scanf("%*[^\n]\n");
}

/* Inicializa 'ultraCabeca' com uma matriz esparsa de 'numLinhas' linhas e
 * 'numColunas' colunas, já alocando os nós cabeça e os vetores que apontam para
 * estes.
 */
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

/* Libera todo a memória alocada para a matriz esparsa. */
void libera(Matriz *ultraCabeca) {
	ApElemento tempElemento;
	int i;

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

/* Procura o elemento de coordenadas 'i','j' na matriz. Caso ele existe, retorna
 * em 'apApLinha' e 'apApColuna' o endereço do elemento. Caso contrário, retorna
 * o endereço de seus predecessores da linha e da coluna, respectivamente.
 */
void encontra(Matriz *ultraCabeca, int i, int j, ApElemento *apApLinha, ApElemento *apApColuna) {
	ApElemento apLinha, apColuna;

	if (i < 0 || i >= ultraCabeca->nlins) {
		erro("encontra: linha inválida");
	} else if (j < 0 || j >= ultraCabeca->ncols) {
		erro("encontra: coluna inválida");
	}

	/* Percorre a linha 'i' procurando pelo elemento, ou seu predecessor */
	apLinha = ultraCabeca->clin[i];
	apLinha = apLinha->dir;

	while (apLinha->col < j) {
		apLinha = apLinha->dir;
	}

	/* Se não encontramos o elemento, percorremos a coluna 'j' procurando seu
	 * outro predecessor.
	 */
	if (apLinha->col != j) {
		apColuna = ultraCabeca->ccol[j];
		apColuna = apColuna->ab;

		while (apColuna->lin < i) {
			apColuna = apColuna->ab;
		}
	} else {
		apColuna = apLinha;
	}

	(*apApLinha) = apLinha;
	(*apApColuna) = apColuna;
}

/* Retorna o valor de um elemento 'i','j' na matriz. */
float valor(Matriz *ultraCabeca, int i, int j) {
	ApElemento apLinha, apColuna;

	encontra(ultraCabeca, i, j, &apLinha, &apColuna);

	/* Se o elemento não foi encontrado, seu valor é 0. */
	if (apLinha != apColuna) {
		return 0;
	}

	return apLinha->val;
}

/* Remove um elemento 'elRemovido' da matriz e libera a memória usada por ele. */
void remove_elem(Matriz *ultraCabeca, ApElemento elRemovido) {
	elRemovido->ac->ab   = elRemovido->ab;
	elRemovido->ab->ac   = elRemovido->ac;
	elRemovido->dir->esq = elRemovido->esq;
	elRemovido->esq->dir = elRemovido->dir;

	FREE(elRemovido);
	ultraCabeca->nelems--;
}

/* Insere um elemento de valor 'v' e índices 'i','j', dados os elementos
 * sucessores. Assume 'v' != 0.
 */
void insere_elem(Matriz *ultraCabeca, int i, int j, float v, ApElemento apLinha, ApElemento apColuna) {
	ApElemento novoElemento;

	if (apLinha->col < j) {
		erro("insere: apLinha inconsistente");
	} else if (apColuna->lin < i) {
		erro("insere: apColuna inconsistente");
	}

	novoElemento = MALLOC(sizeof(Elemento));
	novoElemento->lin = i;
	novoElemento->col = j;
	novoElemento->val = v;

	novoElemento->esq = apLinha->esq;
	novoElemento->dir = apLinha;

	novoElemento->ac = apColuna->ac;
	novoElemento->ab = apColuna;

	apLinha->esq = novoElemento;
	apColuna->ac = novoElemento;

	ultraCabeca->nelems++;
}

void atribui(Matriz *ultraCabeca, int i, int j, float v) {
	ApElemento apLinha, apColuna;
	encontra(ultraCabeca, i, j, &apLinha, &apColuna);

	/* Elemento não existe; se "v" não é nulo, precisa encaixar: */
	if (apLinha != apColuna) {
		if (v != 0) {
			insere_elem(ultraCabeca, i, j, v, apLinha, apColuna);
		}
	/* Elemento existe; se "v" é nulo, precisa remover, senão alterar: */
	} else {
		if (v != 0) {
			apLinha->val = v;
		} else {
			remove_elem(ultraCabeca, apLinha);
		}
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
