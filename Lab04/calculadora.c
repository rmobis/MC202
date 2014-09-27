/* M�dulo de c�lculo de express�es p�s-fixas sobre uma base de dados     */
/* formada por polin�mios em uma vari�vel.                               */

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "pilha.h"
#include "polinomios.h"
#include "balloc.h"
#include "boolean.h"

/* Mensagens de erro para este m�dulo. */

#define MSG_ERRO_NOME_INVALIDO "nome inv�lido para a base de polin�mios."
#define MSG_ERRO_POLINOMIO_NAO_INICIALIZADO "tentativa de recuperar polin�mio n�o inicializado."
#define MSG_ERRO_FALTA_OPERANDO "n�mero insuficiente de operandos."
#define MSG_ERRO_FALTA_OPERADOR  "n�mero insuficiente de operandores."
#define MSG_ERRO_CARACTERE_INVALIDO "caractere inv�lido."

#define IMPRIME_ERRO(msg)  {printf("Erro: %s\n",msg); exit(0); }

#define TAM_BASE 5
Polinomio vetorPoli[TAM_BASE];

Boolean Operando(char *op);

/* Inicializa polin�mios com NULL, indicando que n�o h� polin�mios v�lidos na
 * base.
 */
void InicializaBasePolinomios() {
	int i;
	for (i = 0; i < TAM_BASE; i++) {
		vetorPoli[i] = NULL;
	}
}

/* Retorna o polin�mio de nome x. */
Polinomio RecuperaPolinomio(char x) {
	if (!Operando(&x)) {
		IMPRIME_ERRO(MSG_ERRO_NOME_INVALIDO);
	} else if (vetorPoli[x - 'A'] == NULL) {
		IMPRIME_ERRO(MSG_ERRO_POLINOMIO_NAO_INICIALIZADO);
	}

	return vetorPoli[x - 'A'];
}

/* Armazena o polin�mio 'p' sob o nome 'x' na base. */
void ArmazenaPolinomio(char x, Polinomio p) {
	if (!Operando(&x)) {
		IMPRIME_ERRO(MSG_ERRO_NOME_INVALIDO);
	}

	vetorPoli[x - 'A'] = p;
}

/*** C�lculo da express�o ***/

/* Defini��o dos elementos da pilha. Polin�mios tempor�rios devem ser liberados
 * ap�s terem sido utilizados. Polin�mios da base s� s�o liberados quando o
 * usu�rio invoca libera.
 */
typedef struct ElemPilha {
	Polinomio poli;
	Boolean temp;
} ElemPilha;

typedef enum {
	Soma,
	Subtracao,
	Produto,
	MenosUnario
} OrdemOperador;


/* Desempilha um elemento, enviando uma mensagem de erro caso a pilha esteja
 * vazia.
 */
ElemPilha* VerificaDesempilha (Pilha* pilha) {
	if (PilhaVazia(pilha)) {
		IMPRIME_ERRO(MSG_ERRO_FALTA_OPERANDO);
	}

	return (ElemPilha*) Desempilha(pilha);
}

/* Cria e empilha um n� do tipo ElemPilha. */
void EmpilhaOperando(Pilha* pilha, Polinomio poli, Boolean temp) {
	ElemPilha *elem = MALLOC(sizeof(ElemPilha));
	elem->poli = poli;
	elem->temp = temp;
	Empilha(pilha, elem);
}

/* Verifica se o ponteiro aponta para um cactere que representa um operando
 * v�lido. Caso o valor apontado seja uma letra, este � modificado para sua
 * vers�o mai�scula.
 */
Boolean Operando(char *op) {
 	*op = toupper(*op);

	return (*op >= 'A' && *op < ('A' + TAM_BASE));
}

Boolean Operador(char *opChar) {
	if (*opChar == '+') {
		*opChar = (char) Soma;
	} else if (*opChar == '-') {
		*opChar = (char) Subtracao;
	} else if (*opChar == '*') {
		*opChar = (char) Produto;
	} else if (*opChar == '~') {
		*opChar = (char) MenosUnario;
	} else {
		return false;
	}

	return true;
}

void ExecutaOperacao(Pilha *polPilha, OrdemOperador op) {
	Polinomio r;
	ElemPilha *a, *b;

	a = VerificaDesempilha(polPilha);

	if (op == MenosUnario) {
		r = MultTermo(a->poli, 0, -1);
	} else {
		b = VerificaDesempilha(polPilha);

		if (op == Soma) {
			r = SomaPolinomios(a->poli, b->poli);
		} else if (op == Subtracao) {
			r = SubPolinomios(a->poli, b->poli);
		} else if (op == Produto) {
			r = MultPolinomios(a->poli, b->poli);
		}

		if (b->temp) {
			LiberaPolinomio(b->poli);
			FREE(b);
		}
	}

	if (a->temp) {
		LiberaPolinomio(b->poli);
		FREE(b);
	}

	EmpilhaOperando(polPilha, r, true);
}

/* Retorna o polin�mio referente � express�o dada; este polin�mio devolvido �
 * sempre uma nova c�pia, mesmo que a express�o seja uma vari�vel simples.
 */
Polinomio CalcExpr(char* expr) {
	Pilha *polPilha = NULL, *opPilha = NULL;
	OrdemOperador prevOp;
	ElemPilha *r;
	int i;

	CriaPilha(polPilha);
	CriaPilha(opPilha);

	i = 0;
	while (expr[i] != '\0') {
		if (Operando(&expr[i])) {
			EmpilhaOperando(polPilha, RecuperaPolinomio(expr[i]), false);
		} else if (Operador(&expr[i])) {
			if (PilhaVazia(opPilha)) {
				Empilha(opPilha, &expr[i]);
			} else {
				prevOp = *((OrdemOperador*)Desempilha(opPilha));

				if (prevOp >= expr[i]) {
					ExecutaOperacao(polPilha, prevOp);
				} else {
					Empilha(opPilha, &prevOp);
				}

				Empilha(opPilha, &expr[i]);
			}
		} else {
			IMPRIME_ERRO(MSG_ERRO_CARACTERE_INVALIDO);
		}

		i++;
	}

	while (!PilhaVazia(opPilha)) {
		ExecutaOperacao(polPilha, *((OrdemOperador*)Desempilha(opPilha)));
	}

	r = VerificaDesempilha(polPilha);

	if (!r->temp) {
		IMPRIME_ERRO(MSG_ERRO_FALTA_OPERADOR);
	}

	return r->poli;
}
