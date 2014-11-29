/**
 * Programa: trie.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 29/11/2014
 *
 * Módulo de implementação das funções de manipulação de árvores digitais (ADs)
 * através de um tipo abstrato de dados.
 */

#include <stdio.h>
#include <string.h>

#include "trie.h"
#include "balloc.h"

typedef struct RegTrie * ImplTrie;

typedef struct RegTrie {
	Boolean fim;                   // término de uma cadeia
	ImplTrie subarv[TAM_ALFABETO]; // vetor de subárvores
} RegTrie;



/******************************************************************************
 *                 Funções auxiliares para manipulação de ADs                 *
 ******************************************************************************/

/* Devolve o máximo entre dois valores inteiros. Poderiamos ter usado o macro
 * 'max' mas, por ser um macro, ele acabaria por executar parte do código duas
 * vezes, diminuindo a performance do programa.
 */
int _max(int a, int b) {
	if (a >= b) {
		return a;
	} else {
		return b;
	}
}

/* Cria e inicializa um nó, setando a marca de final de cadeia e preenchendo
 * o evetor de sub-árcores com NULL.
 */
ImplTrie criaInicializaNo() {
	ImplTrie n = MALLOC(sizeof(RegTrie));

	n->fim = false;

	/* Preenche o vetor de sub-árvores com NULL */
	memset(n->subarv, 0, sizeof(n->subarv));

	return n;
}

/* Retorna o índice em que uma letra 'l' deve ocupar no vetor de sub-árvores.
 * Assume 'l' uma letra do alfabeto padrão, minúscula.
 */
int letraIndice(char l) {
	return l - 97;
}

/* Retorna se um nó pode ser removido, após a remoção de uma cadeia. */
Boolean noLivre(ImplTrie iT) {
	return !iT->fim && numNosAD(iT) == 1;
}

/* Remove uma dada cadeira de caracteres 's' da árvore digital 't'. Caso esta
 * tenha sido corretamento removida, devolve 'true'; 'false' caso contrário.
 * 's' pode ser uma cadeia vazia.
 */
Boolean removeAux(ImplTrie iT, char *s, Boolean *removido) {
	if (iT == NULL) {
		return false;
	}

	/* Chegamos ao fim da cadeia */
	if (*s == '\0') {
		iT->fim = false;

		/* Removemos o nó, caso ele esteja livre */
		*removido = noLivre(iT);
		if (*removido) {
			liberaAD(iT);
		}

		return true;
	}

	int i = letraIndice(*s);
	if (removeAux(iT->subarv[i], s + 1, removido)) {
		if (*removido) {
			/* Se removemos um filho, atualizamos o pai de acordo */
			iT->subarv[i] = NULL;

			/* Removemos o nó, caso ele esteja livre */
			*removido = noLivre(iT);
			if (*removido) {
				liberaAD(iT);
			}
		}

		return true;
	}

	return false;
}

/* NÃO IMPLEMENTADA */
void percorreAux(ImplTrie t, funcVisita *v, char *buf, int m) {
	return;
}


/******************************************************************************
 *                    Implementação da Árvore Digital (AD)                    *
 ******************************************************************************/

/* Cria uma árvore digital vazia. */
Trie CriaAD() {
	return criaInicializaNo();
}

/* Verifica se uma dada cadeia de caracteres 's' foi inserida na árvore digital
 * 't'.
 */
Boolean consultaAD(Trie t, char *s) {
	int i;
	ImplTrie iT = (ImplTrie) t;

	while (*s != '\0') {
		i = letraIndice(*s);
		iT = iT->subarv[i];

		if (iT == NULL) {
			return false;
		}

		s++;
	}

	return iT->fim;
}

/* Insere uma dada cadeira de caracteres 's' na árvore digital 't'. Caso haja
 * inserção, devolve 'true'; 'false' caso contrário. 's' pode ser uma cadeia
 * vazia.
 */
Boolean insereAD(Trie t, char *s) {
	int i;
	ImplTrie iT = (ImplTrie) t;

	while (*s != '\0') {
		i = letraIndice(*s);

		/* Criamos os nós faltantes */
		if (iT->subarv[i] == NULL) {
			iT->subarv[i] = criaInicializaNo();
		}

		iT = iT->subarv[i];
		s++;
	}

	/* Após percorrer a árvore, fazendo as inserções necessários, chegamos ao
	 * nó que representa o final da cadeia. Se este ainda não está marcado como
	 * um final de cadeia, é porque a cadeia ainda não havia sido inserida.
	 */
	if (!iT->fim) {
		iT->fim = true;
		return true;
	}

	return false;
}

/* Devolve o número de cadeias contidas na árvore digital. */
int numCadeiasAD(Trie t) {
	ImplTrie iT = (ImplTrie) t;

	if (iT == NULL) {
		return 0;
	}

	/* O nó representa uma cadeia se estiver marcado como o final de uma */
	int n = (int) iT->fim;

	int i;
	/* Somamos com as cadeias em todos os filhos */
	for (i = 0; i < TAM_ALFABETO; i++) {
		n += numCadeiasAD(iT->subarv[i]);
	}

	return n;
}

/* Remove uma dada cadeira de caracteres 's' da árvore digital 't'. Caso esta
 * tenha sido corretamento removida, devolve 'true'; 'false' caso contrário.
 * 's' pode ser uma cadeia vazia.
 */
Boolean removeAD(Trie t, char *s) {
	Boolean removido;
	ImplTrie iT = (ImplTrie) t;

	/* Temporariamente definimos a raiz como o final de uma cadeia, para que ela
	 * não seja removida durante o processo.
	 */
	iT->fim = true;

	removido = removeAux(iT, s, &removido);

	/* Retornamos a raiz ao seu estado original */
	iT->fim = false;

	return removido;
}

/* NÃO IMPLEMENTADA */
void percorreAD(Trie t, funcVisita *v) {
	return;
}

/* Libera toda a memória alocada para a árvore digital. */
void liberaAD(Trie t) {
	ImplTrie iT = (ImplTrie) t;

	if (iT == NULL) {
		return;
	}

	int i;
	/* Liberamos todos os filhos primerio */
	for (i = 0; i < TAM_ALFABETO; i++) {
		liberaAD(iT->subarv[i]);
	}

	FREE(iT);
}


/******************************************************************************
 *              Funções para verificação da Árvore Digital (AD)               *
 ******************************************************************************/

/* Devolve o número de nós contidos na árvore digital, contando com a raiz. */
int numNosAD(Trie t) {
	ImplTrie iT = (ImplTrie) t;

	if (iT == NULL) {
		return 0;
	}

	int n = 1;

	int i;
	/* Somamos com os nós de todos os filhos */
	for (i = 0; i < TAM_ALFABETO; i++) {
		n += numNosAD(iT->subarv[i]);
	}

	return n;
}

/* Devolve a altura da árvore digital, contando com a raiz. */
int alturaAD(Trie t) {
	ImplTrie iT = (ImplTrie) t;

	if (iT == NULL) {
		return 0;
	}

	int n = 0;

	int i;
	/* Pegamos o máximo dentre as alturas de todos os filhos */
	for (i = 0; i < TAM_ALFABETO; i++) {
		n = _max(n, alturaAD(iT->subarv[i]));
	}

	return n + 1;
}


