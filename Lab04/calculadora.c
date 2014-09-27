/* Módulo de cálculo de expressões pós-fixas sobre uma base de dados     */
/* formada por polinômios em uma variável.                               */

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#include "pilha.h"
#include "polinomios.h"
#include "balloc.h"
#include "boolean.h"

/* Mensagens de erro para este módulo. */

#define MSG_ERRO_NOME_INVALIDO "nome inválido para a base de polinômios."
#define MSG_ERRO_POLINOMIO_NAO_INICIALIZADO "tentativa de recuperar polinômio não inicializado."
#define MSG_ERRO_FALTA_OPERANDO "número insuficiente de operandos."
#define MSG_ERRO_FALTA_OPERADOR  "número insuficiente de operandores."
#define MSG_ERRO_CARACTERE_INVALIDO "caractere inválido."

#define IMPRIME_ERRO(msg)  {printf("Erro: %s\n",msg); exit(0); }

#define TAM_BASE 5
Polinomio vetorPoli[TAM_BASE];

Boolean Operando(char *op);

/* Inicializa polinômios com NULL, indicando que não há polinômios válidos na
 * base.
 */
void InicializaBasePolinomios() {
	int i;
	for (i = 0; i < TAM_BASE; i++) {
		vetorPoli[i] = NULL;
	}
}

/* Retorna o polinômio de nome x. */
Polinomio RecuperaPolinomio(char x) {
	if (!Operando(&x)) {
		IMPRIME_ERRO(MSG_ERRO_NOME_INVALIDO);
	} else if (vetorPoli[x - 'A'] == NULL) {
		IMPRIME_ERRO(MSG_ERRO_POLINOMIO_NAO_INICIALIZADO);
	}

	return vetorPoli[x - 'A'];
}

/* Armazena o polinômio 'p' sob o nome 'x' na base. */
void ArmazenaPolinomio(char x, Polinomio p) {
	if (!Operando(&x)) {
		IMPRIME_ERRO(MSG_ERRO_NOME_INVALIDO);
	}

	vetorPoli[x - 'A'] = p;
}

/*** Cálculo da expressão ***/

/* Definição dos elementos da pilha. Polinômios temporários devem ser liberados
 * após terem sido utilizados. Polinômios da base só são liberados quando o
 * usuário invoca libera.
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

/* Cria e empilha um nó do tipo ElemPilha. */
void EmpilhaOperando(Pilha* pilha, Polinomio poli, Boolean temp) {
	ElemPilha *elem = MALLOC(sizeof(ElemPilha));
	elem->poli = poli;
	elem->temp = temp;
	Empilha(pilha, elem);
}

/* Verifica se o ponteiro aponta para um cactere que representa um operando
 * válido. Caso o valor apontado seja uma letra, este é modificado para sua
 * versão maiúscula.
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

/* Retorna o polinômio referente à expressão dada; este polinômio devolvido é
 * sempre uma nova cópia, mesmo que a expressão seja uma variável simples.
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
