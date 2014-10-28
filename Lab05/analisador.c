/**
 * Programa: analisador.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 06/10/2014
 *
 * Módulo de transformação de expressões na notação infixa para pós-fixa.
 */

#include "analisador.h"

#define NULL 0

/* Variáveis globais a este módulo */
char *in;  /* Cadeia e índice para expressão infixa (entrada). */
char *pos; /* Cadeia e índice para expressão pósfixa (saída). */

/* Protótipos das funções mutuamente recursivas. Optamos por retornar apenas
 * o código do erro, sem sua posição, pois esta pode ser calculada depois, como
 * explicitado mais abaixo.
 */
CodigosErro Expressao();
CodigosErro Termo();
CodigosErro Fator();
CodigosErro Primario();

/* Funções Auxiliares */
void Sai(char c);
void PulaBrancos();

/*************************************************************/
/*                 Função   principal                        */
/*************************************************************/

/* Transforma uma expressão da notação infixa para a pós-fixa. Em caso de erro,
 * devolve o código e a posição na cadeia de entrada onde o erro foi encontrado.
 */
Erro InPos(char *infixa, char *posfixa) {
	Erro e;
	char c;

	in = infixa;
	pos = posfixa;

	PulaBrancos();

	if (*in == '\0') {
		e.codigoErro = CADEIA_DE_BRANCOS;
		e.posicao = 0;
	} else {
		e.codigoErro = Expressao();

		/* A posição pode ser calculada pelo endereço final, que é onde o último
		 * caractere foi lido, subtraído do endereço inicial, dividido pelo
		 * tamanho de um char.
		 */
		e.posicao = ((int)(in - infixa)) / sizeof(char);

		c = *in;

		if (e.codigoErro == EXPR_VALIDA) {
			if (c != '\0') {
				e.codigoErro = OPERADOR_ESPERADO;
			}
		}

		/* Se tivemos algum erro, verificamos se na verdade não era um caractere
		 * inválido.
		 */
		if (e.codigoErro != EXPR_VALIDA) {
			if (
				(c < 'a' || c > 'z') &&
				c != '*' && c != '/' &&
				c != '+' && c != '-' &&
				c != '(' && c != ')' &&
				c != '^' && c != '\0'
			) {
				e.codigoErro = CARACTERE_INVALIDO;
			}
		}
	}

	Sai('\0');

	return e;
}

/*************************************************************/
/*           Funções de implementação do analisador          */
/*************************************************************/

/* Coloca o caractere 'c' no vetor de saída. */
void Sai(char c) {
	*pos = c;
	pos++;
}

/* Pula caracteres brancos. */
void PulaBrancos() {
	while (*in == ' ') {
		in++;
	}
}

/* Processa uma expressão da cadeia de entrada. */
CodigosErro Expressao() {
	CodigosErro e;
	char op = NULL;

	PulaBrancos();

	/* Operadores Unários */
	if (*in == '+') {
		op = '&';
		in++;
	} else if (*in == '-') {
		op = '~';
		in++;
	}

	e = Termo();
	if (e != EXPR_VALIDA) {
		return e;
	}

	/* Operadores Unários */
	if (op != NULL) {
		Sai(op);
	}

	PulaBrancos();

	while (*in == '+' || *in == '-') {
		op = *in;
		in++;

		e = Termo();
		if (e != EXPR_VALIDA) {
			return e;
		}

		Sai(op);

		PulaBrancos();
	}

	return EXPR_VALIDA;
}


/* Processa um termo da cadeia de entrada. */
CodigosErro Termo() {
	CodigosErro e;
	char op;

	e = Fator();
	if (e != EXPR_VALIDA) {
		return e;
	}

	PulaBrancos();

	while (*in == '*' || *in == '/') {
		op = *in;
		in++;

		e = Fator();
		if (e != EXPR_VALIDA) {
			return e;
		}

		Sai(op);

		PulaBrancos();
	}

	return EXPR_VALIDA;
}

/* Processa um fator da cadeia de entrada. */
CodigosErro Fator() {
	CodigosErro e;

	e = Primario();
	if (e != EXPR_VALIDA) {
		return e;
	}

	PulaBrancos();

	if (*in == '^') {
		in++;

		e = Fator();
		if (e != EXPR_VALIDA) {
			return e;
		}

		Sai('^');

		PulaBrancos();
	}

	return EXPR_VALIDA;
}


/* Processa um primário da cadeia de entrada. */
CodigosErro Primario() {
	CodigosErro e;

	PulaBrancos();

	if (*in >= 'a' && *in <= 'z') {
		Sai(*in);
		in++;
	} else if (*in == '(') {
		in++;

		e = Expressao();
		if (e != EXPR_VALIDA) {
			return e;
		} else if (*in != ')') {
			return FECHA_PARENTESE_ESPERADO;
		} else {
			in++;
			PulaBrancos();
		}
	} else {
		return OPERANDO_ESPERADO;
	}

	return EXPR_VALIDA;
}
