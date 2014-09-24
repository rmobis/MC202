/**
* Programa: Lab03.c
* Autor: Raphael Mobis Tacla   RA: 157104
* Disciplina: MC202            Turma: F
* Data: 17/09/2014
*
* Implementação do de uma estrutura de matriz esparsa. As linhas e colunas da
* matriz são numeradas de 0 a 'nlins'-1 e 0 a 'ncols'-1, respectivamente, onde
* 'nlins' e 'ncols' são o número de linhas e colunas na matriz, respectivamente.
*
* Cada termo não nulo da matriz possui apontadores 'ac', 'ab', 'dir' e 'esq' que
* apontam, respectivamente, para os termos acima, abaixo, à direita e à esquerda
* deste.
*
* Toda linha possuí um nó cabeça, de indíce 'i','ncols', onde 'i' é o índice da
* linha. Analogamente, toda coluna possui um nó cabeça, de indíce 'nlins','j',
* onde 'j' é o índice da coluna. O nó super cabeça, de índice 'nlins','ncols' é
* o cabeça da coluna de cabeças das linhas e da linha de cabeças das colunas.
*
* O ponteiro principal da matriz possui dois vetores 'clin' e 'ccol' que possuem
* respectivamente apontadores para os nós cabeça de todas linhas e colunas. Além
* disso, esse ponteiro também mantém uma referência ao número total de elementos
* existentes na matriz.
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
	char c;
	do {
		scanf("%c", &c);
	} while (c != '\n' && c != EOF);
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
	A->nelems = 0;
	A->nlins  = numLinhas;
	A->ncols  = numColunas;
	A->clin   = (ApElemento*) MALLOC((numLinhas + 1) * sizeof(ApElemento));
	A->ccol   = (ApElemento*) MALLOC((numColunas + 1) * sizeof(ApElemento));


	/* Cria a 'super' cabeça. */
	superCabeca = (ApElemento) MALLOC(sizeof(Elemento));
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
		tempCabeca = (ApElemento) MALLOC(sizeof(Elemento));
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
		tempCabeca = (ApElemento) MALLOC(sizeof(Elemento));
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
	ApElemento p, q;
	int i;

	/* Rodando a lista para a direita, apagamos sempre o elemento da esquerda. */
	for (i = 0; i <= A->nlins; i++) {
		/* Pulamos até que o nó cabeça da linha não seja o da esquerda. */
		p = A->clin[i]->dir;

		while (p->col != A->ncols) {
			q = p->dir;
			FREE(p);
			p = q;
		}

		/* Libera o nó cabeça da linha. */
		FREE(p);
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
	apLinha = A->clin[i]->dir;

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

	novoElemento->ac->ab   =
	novoElemento->ab->ac   =
	novoElemento->esq->dir =
	novoElemento->dir->esq = novoElemento;

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
	/*int ip, jp;  Índices do elemento anterior. */
	int k;

	scanf("%d %d %d", &m, &n, &d); prox_linha();
	inicializa(a, m, n);
	/*ip = -1; jp = n-1;*/
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

/* Coloca em 'T' a matriz transposta de matriz 'A'. */
void transpoe(Matriz *A, Matriz *T) {
	int i;
	ApElemento apLinha;

	inicializa(T, A->ncols, A->nlins);

	/* Insere em 'T' todos os elementos de 'A', invertendo as linhas com as
	 * colunas.
	 */
	for (i = 0; i < A->nlins; i++) {
		apLinha = A->clin[i]->dir;

		while (apLinha->col != A->ncols) {
			insere_elem(T, apLinha->col, i, apLinha->val, T->clin[apLinha->col], T->ccol[i]);
			apLinha = apLinha->dir;
		}
	}
}

/* Coloca em 'S' a soma das matrizes 'A' e 'B', que devem ter todas as mesmas
 * dimensões.
 */
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


		/* Percorremos as linha de A e B usando o paradigma de intercalação. */
		while (apLinhaA->col != A->ncols || apLinhaB->col != B->ncols) {
			if (apLinhaA->col < apLinhaB->col) {
				insere_elem(S, apLinhaA->lin, apLinhaA->col, apLinhaA->val, S->clin[i], S->ccol[apLinhaA->col]);
				apLinhaA = apLinhaA->dir;
			} else if (apLinhaA->col > apLinhaB->col) {
				insere_elem(S, apLinhaB->lin, apLinhaB->col, apLinhaB->val, S->clin[i], S->ccol[apLinhaB->col]);
				apLinhaB = apLinhaB->dir;
			} else {
				/* Não inserimos valores nulos. */
				if (apLinhaA->val + apLinhaB->val != 0) {
					insere_elem(S, apLinhaA->lin, apLinhaA->col, apLinhaA->val + apLinhaB->val, S->clin[i], S->ccol[apLinhaA->col]);
				}

				apLinhaA = apLinhaA->dir;
				apLinhaB = apLinhaB->dir;
			}
		}
	}
}

/* Retorna na matriz 'P' o produto das matrizes 'A' e 'B'. O número de colunas
 * de 'A' deve ser igual ao número de linhas de 'B'.
 */
void multiplica(Matriz *A, Matriz *B, Matriz *P) {
	ApElemento apLinhaA, apColunaB;
	float valorElemento;
	int i, j;

	if (A->ncols != B->nlins) {
		erro("multiplica: tamanhos invalidos");
	}

	inicializa(P, A->nlins, B->ncols);

	for (i = 0; i < A->nlins; i++) {

		for (j = 0; j < B->ncols; j++) {
			valorElemento = 0;
			apLinhaA  = A->clin[i]->dir;
			apColunaB = B->ccol[j]->ab;

			/* Percorremos a linha de A e a coluna de B usando o paradigma de
			 * intercalação.
			 */
			while (apLinhaA->col != A->ncols && apColunaB->lin != B->nlins) {
				if (apLinhaA->col < apColunaB->lin) {
					apLinhaA = apLinhaA->dir;
				} else if (apLinhaA->col > apColunaB->lin) {
					apColunaB = apColunaB->ab;
				} else {
					valorElemento += apLinhaA->val * apColunaB->val;
					apLinhaA = apLinhaA->dir;
					apColunaB = apColunaB->ab;
				}
			}

			/* Não inserimos valores nulos. */
			if (valorElemento != 0) {
				insere_elem(P, i, j, valorElemento, P->clin[i], P->ccol[j]);
			}
		}
	}
}
