/*
 * huffman.c - Implementação do algoritmo de Huffman.
 */

/*
 * huffman.c - Implementação do algoritmo de Huffman.
 *             Para fazer a implementação com bits verdadeiros,
 *             deixe abaixo a inclusão do arquivo "bits.h"; caso
 *             contrário, do arquivo "pseudo_bits.h".
 *
 *             Opcionalmente, poderá ser usada uma implementação
 *             independente de fila de prioridade (heap) da tarefa
 *             08. Neste caso, deverá ser submetido também o arquivo
 *             "heap.c". Caso contrário, deverá ser eliminada ou
 *             comentada a linha abaixo que inclui "heap.h".
 */

/* Número máximo de caracteres distintos */
#define MAX_CHARS 256

/* Determina o uso de pseudo-bits na compressão e decompressão. Isto é, usa-se
 * os caracteres '0' e '1' para representar os bits reais. Para usar bits reais,
 * comente a linha abaixo.
 */
#define PSEUDO_BITS

#include "heap.h"

#include <string.h>
#include <stdio.h>
#include "huffman.h"
#include "balloc.h"
#include "imprimearvore.h"


/******************************************************************************
 *                            Declarações de Tipos                            *
 ******************************************************************************/

typedef enum TipoFilho {
	Esquerdo = 0,
	Direito  = 1,
	Indeterminado
} TipoFilho;

typedef struct RegArvHuff *ArvHuff;
typedef struct RegArvHuff {
	int peso;

	/* Somente para folhas */
	char letra;

	TipoFilho tipoFilho;
	ArvHuff pai, esq, dir;
} RegArvHuff;


/******************************************************************************
 *                             Variáveis Globais                              *
 ******************************************************************************/

/* Árvore usada ao longo da execução do programa. */
ArvHuff huff;

/* Ponteiros para todas as folhas da árvore 'huff' */
ArvHuff folhasHuff[MAX_CHARS];


/******************************************************************************
 *                             Funções Auxiliares                             *
 ******************************************************************************/

/* Compara duas árvores de huffman, 'a' e 'b', e retorna < 0 caso 'a' deva estar
 * acima de 'b', > 0 caso 'b' deva estar acima de 'a' e 0 caso ambas tenham o
 * mesmo peso.
 */
int ComparaHuffman(void *a, void *b) {
	ArvHuff hA = (ArvHuff) a;
	ArvHuff hB = (ArvHuff) b;

	return hB->peso - hA->peso;
}

/* Cria uma folha da árvore; caso não haja mais memória ou o peso da folha seja
 * 0, retorna NULL.
 */
ArvHuff CriaFolha(int peso, char letra) {
	if (peso == 0) {
		return NULL;
	}

	ArvHuff f = MALLOC(sizeof(RegArvHuff));

	if (f != NULL) {
		f->peso      = peso;
		f->letra     = letra;
		f->tipoFilho = Indeterminado;

		f->esq = f->dir = f->pai = NULL;
	}

	return f;
}

/* Cria uma nova árvore, usando como filhos esquerdo e direito as árvores
 * passadas, respectivamente, em 'esq' e 'dir'; caso não haja mais memória,
 * retorna NULL.
 */
ArvHuff CombinaArvores(ArvHuff dir, ArvHuff esq) {
	ArvHuff p = MALLOC(sizeof(RegArvHuff));

	if (p != NULL) {
		p->peso = esq->peso + dir->peso;

		p->esq = esq;
		p->dir = dir;
		p->pai = NULL;

		esq->tipoFilho = Esquerdo;
		dir->tipoFilho = Direito;

		esq->pai = dir->pai = p;
	}

	return p;
}

/* Libera toda a memória ocupada pela árvore 'p'. */
void LiberaHuffmanAux(ArvHuff p) {
	if (p != NULL) {
		LiberaHuffmanAux(p->esq);
		LiberaHuffmanAux(p->dir);

		FREE(p);
	}
}


/* Retorna o i-ésimo bit na sequência de bits 'bits'. Serve de abstração para o
 * tipo de representação escolhido no topo no arquivo.
 */
short int IBit(char *bits, int n) {
	#ifdef PSEUDO_BITS
		return bits[n] - '0';
	#endif
}

/* Acrescenta à sequência de bits 'bits' o bit indicado por 'bit'. Serve de
 * abstração para o tipo de representação escolhido no topo no arquivo.
 */
void AcrescentaBit(char *bits, int n, short int bit) {
	#ifdef PSEUDO_BITS
		bits[n] = bit ? '1' : '0';
	#endif
}

/* Acrescenta à sequência de caracteres 'texto' o caractere indicado por 'c'. */
void AcrescentaChar(char *texto, int n, char c) {
	texto[n] = c;
}

/* Acrescenta à sequência de bits 'bits' os bits que representam o caractere que
 * é passado como no, na primeira chamada da função.
 */
Boolean AcrescentaCharBits(ArvHuff no, char *bits, int *numBits, int numBitsMax) {
	/* Se estamos na raiz, retornamos false caso tenhamos excedido o número
	 * máximo de bits a serem codificados.
	 */
	if (no->pai == NULL) {
		return *numBits <= numBitsMax;
	}

	/* Sobe na árvore até a raiz */
	if (!AcrescentaCharBits(no->pai, bits, numBits, numBitsMax)) {
		return false;
	}

	/* Escreve o bit que representa essa passagem na árvore */
	AcrescentaBit(bits, *numBits, no->tipoFilho == Direito);
	(*numBits)++;

	return true;
}

/* Acrescenta à sequência de caracteres 'texto' o caractere representado pela
 * sequência de bits que começa na posição 'numBits'.
 */
Boolean AcrescentaBitsChar(ArvHuff no, char *bits, int *numBits, char *texto, int *numChars, int numCharsMax) {
	/* Se chegamos a uma folha, basta adicionar esse caractere */
	if (no->esq == NULL) {
		if (*numChars == numCharsMax) {
			return false;
		}

		AcrescentaChar(texto, (*numChars)++, no->letra);

		return true;
	}

	/* Desce na árvore, lendo os bits, até acharmos uma folha */
	if (IBit(bits, (*numBits)++) == 0) {
		return AcrescentaBitsChar(no->esq, bits, numBits, texto, numChars, numCharsMax);
	} else {
		return AcrescentaBitsChar(no->dir, bits, numBits, texto, numChars, numCharsMax);
	}
}



/******************************************************************************
 *                   Implementação da Compressão de Huffman                   *
 ******************************************************************************/

/* A partir do texto 'texto' de 'n' caracteres, constrói uma árvore de Huffman
 * para compressão deste texto, na variável global 'huff' deste módulo e amarzena
 * na variável 'folhasHuff', também global a este módulo, os apontadores para as
 * folhas da árvore. Retorna true caso a construção tenha sido bem sucedida;
 * false caso contrário.
 */
Boolean ConstroiHuffman(char texto[], int n) {
	int i, j;
	int freq[MAX_CHARS];
	ArvHuff tempFolhas[MAX_CHARS];

	/* Zera todas as frequências */
	memset(freq, 0, sizeof(freq));

	/* Calcula as frequências */
	for (i = 0; i < n; i++) {
		freq[(unsigned char) texto[i]]++;
	}

	/* Cria as folhas, já com as frequências */
	j = 0;
	for (i = 0; i < MAX_CHARS; i++) {
		folhasHuff[i] = CriaFolha(freq[i], (char) i);

		if (folhasHuff[i] != NULL) {
			tempFolhas[j++] = folhasHuff[i];
		}
	}

	Heap freqHeap = CriaInicializaHeap(j, ComparaHuffman, (void **) tempFolhas);

	/* Constrói a Árvore de Huffman */
	while (TamanhoHeap(freqHeap) > 1) {
		InsereHeap(
			freqHeap,
			CombinaArvores(
				(ArvHuff) RemoveHeap(freqHeap),
				(ArvHuff) RemoveHeap(freqHeap)
			)
		);
	}

	/* A árvore comleta é o único elemento restante no heap */
	huff = (ArvHuff) RemoveHeap(freqHeap);

	LiberaHeap(freqHeap);

	return true;
}

/* Libera toda a memória ocupada pela árvore 'p'. */
void LiberaHuffman() {
	LiberaHuffmanAux(huff);
}


/* Comprime os 'n' caracteres do texto 'texto', usando as estruturas previamente
 * construídas pelo módulo, armazenando o resultado, na representação definida no
 * topo do arquivo, na variável 'bits'. No caso de a compressão ser bem sucedida,
 * retorna true e armazena em 'numBits' o total de bits na cadeia comprimida; caso
 * contrário, retorna false.
 */
Boolean Comprime(char *texto, int n, char *bits, int *numBits, int numBitsMax) {
	*numBits = 0;

	/* Comprime um char por vez */
	for (int i = 0; i < n; i++) {
		if (!AcrescentaCharBits(folhasHuff[(unsigned char) texto[i]], bits, numBits, numBitsMax)) {
			return false;
		}
	}

	return true;
}


/* Descomprime os primeiros 'numBits' bits da cadeia comprimida 'bits', usando
 * as estruturas préviamente construídas pelo módulo, e armazena o resultado, na
 * representação definida no topo do arquivo, na variável 'texto'. No caso de a
 * decompressão ser bem sucedida, retorna true e armazena em 'numChars' o total
 * de caracteres na cadeia descomprimida; caso contrário, retorna false.
 */
Boolean Descomprime(char *texto, int *numChars, char *bits, int numBits, int numCharsMax) {
	*numChars = 0;

	/* Descomprime um char por vez */
	for (int i = 0; i < numBits;) {
		if (!AcrescentaBitsChar(huff, bits, &i, texto, numChars, numCharsMax)) {
			return false;
		}
	}

	return true;
}

/******************************************************************************
 *                 Funções Auxiliares para o Módulo Principal                 *
 ******************************************************************************/

#define DESLOCA 5

char buf[10];  /* O buffer de impressão será fixo de até 9 caracteres e \0 */

ArvHuff esq(ArvHuff p, int nivel) { return p->esq; }
ArvHuff dir(ArvHuff p, int nivel) { return p->dir; }

char *info(ArvHuff p, int nivel) {
	if ((p->esq==NULL)&&(p->dir==NULL))  /* folha */
		sprintf(buf,"%4d:%c",p->peso,p->letra);
	else  /* nó interno */
		sprintf(buf,"%4d",p->peso);
	return buf;
}

void ImprimeHuffman() {
	ImprimeArvore(huff,(subarvfunc*)esq,(subarvfunc*)dir,(infofunc*)info,NULL,DESLOCA,"Árvore vazia");
}

Boolean PseudoBits() {
	#ifdef PSEUDO_BITS
		return true;
	#else
		return false;
	#endif
}