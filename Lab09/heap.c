/**
 * Programa: heap.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 22/11/2014
 *
 * M�dulo de implementa��o das fun��es de manipula��o de filas de prioridade (FP).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"
#include "balloc.h"

typedef struct {
	int tamMax;
	int tam;

	/* Fun��o de Compara��o */
	funcCompara *comp;

	/* Tamanho real do vertor ser� 'tamMax' */
	void *vetor[1];
} RegHeap, *ImplHeap;


/******************************************************************************
 *                 Fun��es auxiliares para manipula��o de FPs                 *
 ******************************************************************************/

/* Sobe de maneira recursiva um elemento, poss�velmente em desacordo com as
 * especifica��es do heap, at� que todo o heap esteja corretamento ordenado.
 */
void Sobe(ImplHeap iH, int i) {
	if (i == 0) {
		return;
	}

	int j = (i - 1) / 2;

	void *filho = Elemento((Heap) iH, i);
	void *pai   = Elemento((Heap) iH, j);

	if (iH->comp(filho, pai) > 0) {
		iH->vetor[j] = filho;
		iH->vetor[i] = pai;

		Sobe(iH, j);
	}
}

/* Desce de maneira recursiva um elemento, poss�velmente em desacordo com as
 * especifica��es do heap, at� que todo o heap esteja corretamento ordenado.
 */
void Desce(ImplHeap iH, int i) {
	if (i >= iH->tam) {
		return;
	}

	int j = (i * 2) + 1;

	void *pai      = Elemento((Heap) iH, i);
	void *filho    = Elemento((Heap) iH, j);
	void *filhoDir = Elemento((Heap) iH, j + 1);

	if (filho != NULL) {
		/* Trocamos sempre com o maior filho */
		if (filhoDir != NULL) {
			if (iH->comp(filhoDir, filho) > 0) {
				filho = filhoDir;
				j++;
			}
		}

		if (iH->comp(filho, pai) > 0) {
			iH->vetor[i] = filho;
			iH->vetor[j] = pai;

			Desce(iH, j);
		}
	}
}


/******************************************************************************
 *                  Implementa��o da Fila de Prioridade (FP)                  *
 ******************************************************************************/

/* Cria um heap vazio. */
Heap CriaHeap(int tamMax, funcCompara *comp) {
	/* Pequena hackeragem para podermos determinar o tamanho do heap de maneira
	 * din�mica, mas ainda acess�-lo atrav�s de um vetor.
	 */
	ImplHeap iH = MALLOC(sizeof(RegHeap) + ((tamMax - 1)*sizeof(void*)));

	iH->tamMax = tamMax;
	iH->tam    = 0;
	iH->comp   = comp;

	return iH;
}

/* Cria um heap vazio e o preenche com os valores de 'elems', fazendo as devidas
 * altera��es para que a ordem dos elementos obede�a �quela estabelecida para um
 * heap.
 */
Heap CriaInicializaHeap(int tamMax, funcCompara *comp, void *elems[]) {
	ImplHeap iH = (ImplHeap) CriaHeap(tamMax, comp);

	/* Copia os elementos */
	memcpy(iH->vetor, elems, iH->tamMax * sizeof(void*));
	iH->tam = iH->tamMax;

	/* "Ordena" os elementos */
	int i;
	for (i = (iH->tam - 2) / 2; i >= 0; i--) {
		Desce(iH, i);
	}

	return iH;
}

/* Retorna o tamanho do heap. */
int TamanhoHeap(Heap h) {
	return ((ImplHeap) h)->tam;
}

/* Insere um novo elemento no heap. */
void InsereHeap(Heap h, void *el) {
	ImplHeap iH = (ImplHeap) h;

	if (iH->tam == iH->tamMax) {
		printf("Estouro da FP\n");
		exit(0);
	}

	/* Insere no final do heap */
	iH->vetor[(iH->tam)++] = el;

	/* Sobe o elemento at� sua posi��o adequada */
	Sobe(iH, iH->tam - 1);
}

/* Remove o primeiro elemento do heap e retorna um ponteiro para este. */
void* RemoveHeap(Heap h) {
	ImplHeap iH = h;

	if (iH->tam == 0) {
		printf("FP vazia\n");
		exit(0);
	}

	void* ret = Elemento(iH, 0);

	/* Substitui a raiz com o �ltimo elemento */
	iH->vetor[0] = Elemento(iH, iH->tam - 1);
	(iH->tam)--;

	/* Desce o elemento at� sua posi��o adequada */
	Desce(iH, 0);

	return ret;
}

/* Libera toda a mem�ria alocada para o heap. */
void LiberaHeap(Heap h) {
	FREE(h);
}

/* Retorna um ponteiro para o 'i'-�simo elemento do heap. */
void* Elemento(Heap h, int i) {
	ImplHeap iH = (ImplHeap) h;

	if (i < 0 || i >= iH->tam) {
		return NULL;
	}

	return iH->vetor[i];
}
