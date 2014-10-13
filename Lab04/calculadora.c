/**
 * Programa: calculadora.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 06/10/2014
 *
 * Este módulo implementa o cálculo de expressões pós-fixas sobre uma base de
 * dados formada por polinômios em uma variável.
 */

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

/* Como não podemos alterar o polinomios.h, fazemos essa pequena gambiarra e
 * declaramos essa o protótipo dessa função aqui mesmo.
 */
Polinomio CopiaPolinomio(Polinomio p);

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

/* Definição da ordem de precedência dos operadores. */
typedef enum {
	/* Temos que começar o enum em 1 pois eles será convertido para um char e o
	 * char de valor 0 é o representante do final de uma cadeia de caracteres.
	 */
	Soma = 1,
	Subtracao,
	Produto,
	MenosUnario
} OrdemOperador;


/* Desempilha um elemento, enviando uma mensagem de erro caso a pilha esteja
 * vazia.
 */
ElemPilha* VerificaDesempilha(Pilha* pilha) {
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

/* Verifica se o ponteiro aponta para um craactere que representa um operando
 * válido. Caso o valor apontado seja uma letra, este é modificado para sua
 * versão maiúscula.
 */
Boolean Operando(char *op) {
	*op = toupper(*op);

	return (*op >= 'A' && *op < ('A' + TAM_BASE));
}

/* Verifica se o ponteiro aponta para um caractere que representa um operador
 * válido. Caso o valor apontado seja, este é modificado para o equivalente do
 * tipo 'OrdemOperador'.
 */
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

/* Dado a pilha de operandos e um operador, desempilha o número de operandos
 * necessários para aquela operação, executa a operação e empilha o resultado.
 */
void ExecutaOperacao(Pilha *polPilha, OrdemOperador op) {
	Polinomio r;
	ElemPilha *a, *b;

	a = VerificaDesempilha(polPilha);

	/* Operadores unários só precisam de um operando. */
	if (op == MenosUnario) {
		r = MultTermo(a->poli, 0, -1);
	} else {
		b = VerificaDesempilha(polPilha);

		if (op == Soma) {
			r = SomaPolinomios(b->poli, a->poli);
		} else if (op == Subtracao) {
			r = SubPolinomios(b->poli, a->poli);
		} else {
			r = MultPolinomios(b->poli, a->poli);
		}

		if (b->temp) {
			LiberaPolinomio(b->poli);
		}
		FREE(b);
	}

	if (a->temp) {
		LiberaPolinomio(a->poli);
	}
	FREE(a);

	EmpilhaOperando(polPilha, r, true);
}

/* Retorna o polinômio referente à expressão dada; este polinômio devolvido é
 * sempre uma nova cópia, mesmo que a expressão seja uma variável simples.
 */
Polinomio CalcExpr(char* expr) {
	Pilha polPilha;
	ElemPilha *r;
	Polinomio p;
	int i;

	CriaPilha(&polPilha);

	i = 0;
	while (expr[i] != '\0') {
		if (Operando(expr + i)) {
			EmpilhaOperando(&polPilha, RecuperaPolinomio(expr[i]), false);
		} else if (Operador(expr + i)) {
			ExecutaOperacao(&polPilha, expr[i]);
		} else {
			IMPRIME_ERRO(MSG_ERRO_CARACTERE_INVALIDO);
		}

		i++;
	}

	r = VerificaDesempilha(&polPilha);

	if (!PilhaVazia(&polPilha)) {
		IMPRIME_ERRO(MSG_ERRO_FALTA_OPERADOR);
	}

	if (r->temp) {
		p = r->poli;
	} else {
		p = CopiaPolinomio(r->poli);
	}

	FREE(r);

	return p;
}
