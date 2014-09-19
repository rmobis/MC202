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

/* Inicializa 'A' com uma matriz esparsa de 'numLinhas' linhas e
 * 'numColunas' colunas, já alocando os nós cabeça e os vetores que apontam para
 * estes.
 */
void inicializa(Matriz *A, int numLinhas, int numColunas) {
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

	superCabeca->esq    =
	superCabeca->dir    =
	superCabeca->ac     =
	superCabeca->ab     =
	A->clin[numLinhas]  =
	A->ccol[numColunas] = superCabeca;


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

		tempCabeca->esq    =
		tempCabeca->dir    =
		tempCabeca->ab->ac =
		tempCabeca->ac->ab =
		A->clin[i]         = tempCabeca;
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
		A->ccol[i]           = tempCabeca;
	}
}

/* Libera todo a memória alocada para a matriz esparsa. */
void libera(Matriz *A) {
	ApElemento tempElemento;
	int i;

	/* Rodando a lista para a direita, apagamos sempre o elemento da esquerda. */
	for (i = 0; i < A->nlins; i++) {
		/* Pulamos até que o nó cabeça da linha não seja o da esquerda. */
		tempElemento = A->clin[i]->dir->dir;

		while (tempElemento->col != A->ncols) {
			FREE(tempElemento->esq);
		}

		/* Libera o nó cabeça da linha. */
		FREE(tempElemento);
	}

	FREE(A->clin);
	FREE(A->ccol);
	FREE(A);
}

/* Procura o elemento de coordenadas 'i','j' na matriz. Caso ele existe, retorna
 * em 'apApLinha' e 'apApColuna' o endereço do elemento. Caso contrário, retorna
 * o endereço de seus predecessores da linha e da coluna, respectivamente.
 */
void encontra(Matriz *A, int i, int j, ApElemento *apApLinha, ApElemento *apApColuna) {
	ApElemento apLinha, apColuna;

	if (i < 0 || i >= A->nlins) {
		erro("encontra: linha inválida");
	} else if (j < 0 || j >= A->ncols) {
		erro("encontra: coluna inválida");
	}

	/* Percorre a linha 'i' procurando pelo elemento, ou seu predecessor */
	apLinha = A->clin[i];
	apLinha = apLinha->dir;

	while (apLinha->col < j) {
		apLinha = apLinha->dir;
	}

	/* Se não encontramos o elemento, percorremos a coluna 'j' procurando seu
	 * outro predecessor.
	 */
	if (apLinha->col != j) {
		apColuna = A->ccol[j];
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
float valor(Matriz *A, int i, int j) {
	ApElemento apLinha, apColuna;

	encontra(A, i, j, &apLinha, &apColuna);

	/* Se o elemento não foi encontrado, seu valor é 0. */
	if (apLinha != apColuna) {
		return 0;
	}

	return apLinha->val;
}

/* Remove um elemento 'elRemovido' da matriz e libera a memória usada por ele. */
void remove_elem(Matriz *A, ApElemento elRemovido) {
	elRemovido->ac->ab   = elRemovido->ab;
	elRemovido->ab->ac   = elRemovido->ac;
	elRemovido->dir->esq = elRemovido->esq;
	elRemovido->esq->dir = elRemovido->dir;

	FREE(elRemovido);
	A->nelems--;
}

/* Insere um elemento de valor 'v' e índices 'i','j', dados os elementos
 * sucessores. Assume 'v' != 0.
 */
void insere_elem(Matriz *A, int i, int j, float v, ApElemento apLinha, ApElemento apColuna) {
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

	A->nelems++;
}

void atribui(Matriz *A, int i, int j, float v) {
	ApElemento apLinha, apColuna;
	encontra(A, i, j, &apLinha, &apColuna);

	/* Se o elemento não existe, o inserimos. */
	if (apLinha != apColuna) {
		if (v != 0) {
			insere_elem(A, i, j, v, apLinha, apColuna);
		}

	/* Se o elemento já existe, alteramos seu valor; caso este seja 0, o
	 * removemos
	 */
	} else {
		if (v != 0) {
			apLinha->val = v;
		} else {
			remove_elem(A, apLinha);
		}
	}
}

/* Lê uma matriz e coloca-a em "a". A próxima linha  do arquivo de entrada deve
 * conter três inteiros "nlins", "ncols", "nelems" (número de linhas, de
 * colunas, e de elementos não nulos da matriz). Em seguida devem vir "nelems"
 * linhas do arquivo, cada qual contendo os índices "lin" e "col" e o valor
 * "val" de um elemento não nulo da matriz.
 *
 * NOTA: Essa função está tão zoada que eu achei melhor nem tocar nela, vai que
 * eu quebro ela...
 */
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

/* Imprime os elementos da matriz "a", no formato esperado por "le_matriz".
 *
 * NOTA: Essa função está tão zoada que eu achei melhor nem tocar nela, vai que
 * eu quebro ela...
 */
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

void transpoe(Matriz *A, Matriz *T) {
	int i;
	ApElemento apLinha;

	inicializa(T, A->ncols, A->nlins);

	for (i = 0; i < A->nlins; i++) {
		apLinha = A->clin[i]->dir;

		while (apLinha->col != A->ncols) {
			insere_elem(T, apLinha->col, i, apLinha->val, T->clin[apLinha->col], T->ccol[i]);
			apLinha = apLinha->dir;
		}
	}
}

void soma(Matriz *A, Matriz *B, Matriz *S) {
	ApElemento apLinhaA, apLinhaB;
	int i;

	if (A->nlins != B->nlins || A->ncols != B->ncols) {
		erro("soma: matrizes com tamanhos diferentes");
	}

	inicializa(S, A->nlins, A->ncols);

	for (i = 0; i < A->nlins; i++) {
		apLinhaA = A->clin[i]->dir;
		apLinhaB = B->clin[i]->dir;

		while (apLinhaA->col != A->ncols || apLinhaB->col != B->ncols) {
			if (apLinhaA->col < apLinhaB->col) {
				insere_elem(S, apLinhaA->lin, apLinhaA->col, apLinhaA->val, S->clin[i], S->clin[j]);
				apLinhaA = apLinhaA->dir;
			} else if (apLinhaA->col > apLinhaB->col) {
				insere_elem(S, apLinhaB->lin, apLinhaB->col, apLinhaB->val, S->clin[i], S->clin[j]);
				apLinhaB = apLinhaB->dir;
			} else {
				insere_elem(S, apLinhaA->lin, apLinhaA->col, apLinhaA->val + apLinhaB->val, S->clin[i], S->clin[j]);
				apLinhaA = apLinhaA->dir;
				apLinhaB = apLinhaB->dir;
			}
		}
	}
}

void multiplica(Matriz *A, Matriz *B, Matriz *P) {
	int i, j;

	if (A->ncols != B->nlins) {
		erro("multiplica: tamanhos invalidos");
	}

	inicializa(P, A->nlins, B->ncols);

	for (i = 0; i < A->nlins; i++) {
		apLinhaA = A->clin[i]->dir;

		for (j = 0; j < B->ncols; j++)
			apColunaB = B->ccol[j]->ab;

			while (apLinhaA->col != A->ncols && apLinhaB->col != B->ncols) {
				if (apLinhaA->col < apColunaB->lin) {
					apLinhaA = apLinhaA->dir;
				} else if (apLinhaA->col > apColunaB->lin) {
					apLinhaB = apLinhaB->ab;
				} else {
					insere_elem(P, apLinhaA->lin, apLinhaA->col, apLinhaA->val * apColunaB->val, P->clin[i], P->clin[j]);
					apLinhaA = apLinhaA->dir;
					apLinhaB = apLinhaB->ab;
				}
			}
		}
	}
}
