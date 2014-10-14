/**
* Programa: analisador.c
* Autor: Raphael Mobis Tacla   RA: 157104
* Disciplina: MC202            Turma: F
* Data: 06/10/2014
*
* M�dulo de transforma��o de express�es na nota��o infixa para p�s-fixa.
*/

#include "analisador.h"

#define NULL 0

/* Vari�veis globais a este m�dulo */
char *in;  /* Cadeia e �ndice para express�o infixa (entrada). */
char *pos; /* Cadeia e �ndice para express�o p�sfixa (sa�da). */

/* Prot�tipos das fun��es mutuamente recursivas. Optamos por retornar apenas
 * o c�digo do erro, sem sua posi��o, pois esta pode ser calculada depois, como
 * explicitado mais abaixo.
 */
CodigosErro Expressao();
CodigosErro Termo();
CodigosErro Fator();
CodigosErro Primario();

/* Fun��es Auxiliares */
void Sai(char c);
void PulaBrancos();

/*************************************************************/
/*                 Fun��o   principal                        */
/*************************************************************/

/* Transforma uma express�o da nota��o infixa para a p�s-fixa. Em caso de erro,
* devolve o c�digo e a posi��o na cadeia de entrada onde o erro foi encontrado.
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

		/* A posi��o pode ser calculada pelo endere�o final, que � onde o �ltimo
		 * caractere foi lido, subtra�do do endere�o inicial, dividido pelo
		 * tamanho de um char.
		 */
		e.posicao = ((int)(in - infixa)) / sizeof(char);

		c = *in;

		if (e.codigoErro == EXPR_VALIDA) {
			if (c != '\0') {
				e.codigoErro = OPERADOR_ESPERADO;
			}
		}

		/* Se tivemos algum erro, verificamos se na verdade n�o era um caractere
		 * inv�lido.
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
/*           Fun��es de implementa��o do analisador          */
/*************************************************************/

/* Coloca o caractere 'c' no vetor de sa�da. */
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

/* Processa uma express�o da cadeia de entrada. */
CodigosErro Expressao() {
	CodigosErro e;
	char op = NULL;

	PulaBrancos();

	/* Operadores Un�rios */
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

	/* Operadores Un�rios */
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


/* Processa um prim�rio da cadeia de entrada. */
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
