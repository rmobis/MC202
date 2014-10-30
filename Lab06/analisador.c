/**
 * Programa: analisador.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 28/10/2014
 *
 * M�dulo de transforma��o de express�es na nota��o infixa para uma �rvore bin�ria
 * que pode ser posteriormente percorrida em p�s-ordem ou pr�-ordem para se obter
 * as mesma express�es em nota��o p�s-fixa ou pr�-fixa, respectivamente.
 */

#include "analisador.h"
#include "balloc.h"

/* Vari�veis globais a este m�dulo */
char *in;  /* Cadeia e �ndice para express�o infixa (entrada). */

/* Prot�tipos das fun��es mutuamente recursivas. Optamos por retornar apenas
 * o c�digo do erro, sem sua posi��o, pois esta pode ser calculada depois, como
 * explicitado mais abaixo.
 */
CodigosErro Expressao(ArvBin *arv);
CodigosErro Termo(ArvBin *arv);
CodigosErro Fator(ArvBin *arv);
CodigosErro Primario(ArvBin *arv);

/* Fun��es Auxiliares */
void PulaBrancos();
void CriaNo(ArvBin *arv, ArvBin esq, ArvBin dir, char info);

/* Fun��es Auxiliares de Percursos */
void ArvPreAux(ArvBin arv, char **pre);
void ArvPosAux(ArvBin arv, char **pos);


/*************************************************************/
/*                 Fun��o   principal                        */
/*************************************************************/

/* Transforma uma express�o da nota��o infixa para um �rvore bin�ria. Em caso
 * de erro, devolve o c�digo e a posi��o na cadeia de entrada onde o erro foi
 * encontrado.
 */
Erro InArv(char *infixa, ArvBin *arv) {
	Erro e;
	char c;

	in = infixa;
	*arv = NULL;

	PulaBrancos();

	if (*in == '\0') {
		e.codigoErro = CADEIA_DE_BRANCOS;
		e.posicao = 0;
	} else {
		e.codigoErro = Expressao(arv);

		/* A posi��o pode ser calculada pelo endere�o final, que � onde o �ltimo
		 * caractere foi lido, subtra�do do endere�o inicial, dividido pelo
		 * tamanho de um char.
		 */
		e.posicao = ((int)(in - infixa)) / sizeof(char);

		c = *in;

		/* Se a express�o transformada � v�lida, mas n�o consumiu toda a cadeia
		 * de entrada, ent�o � porque esperavamos um operador e a express�o
		 * total n�o � de fato v�lida.
		 */
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
	return e;
}

/*************************************************************/
/*           Fun��es de implementa��o do analisador          */
/*************************************************************/

void CriaNo(ArvBin *arv, ArvBin esq, ArvBin dir, char info) {
	(*arv) = MALLOC(sizeof(NoArvBin));

	(*arv)->esq  = esq;
	(*arv)->dir  = dir;
	(*arv)->info = info;
}

/* Pula caracteres brancos. */
void PulaBrancos() {
	while (*in == ' ') {
		in++;
	}
}

/* Processa uma express�o da cadeia de entrada. */
CodigosErro Expressao(ArvBin *arv) {
	CodigosErro e;
	char op = 0;

	/* A express�o pode come�ar com um operador un�rio */
	if (*in == '+') {
		op = '&';
		in++;
		PulaBrancos();
	} else if (*in == '-') {
		op = '~';
		in++;
		PulaBrancos();
	}

	/* A express�o sempre tem um termo, ap�s o poss�vel operador un�rio */
	e = Termo(arv);
	if (e != EXPR_VALIDA) {
		return e;
	}

	/* Se temos um operador un�rio, substituimos a raiz pelo operador e
	 * colocamos o termo que foi processado � direita. Usamos o n� da direita ao
	 * inv�s do da esquerda como de costume com os operadores bin�rios pois
	 * assim n�s poderiamos, teoricamente, ler a �rvore em Inordem para obtermos
	 * novamente a express�o infixa.
	 */
	if (op != 0) {
		CriaNo(arv, NULL, (*arv), op);
	}

	/* A express�o pode ser seguida de um ou mais operadores (+ ou -) e termos.
	 * Para cada um, definimos este operador como a raiz da �rvore, a raiz
	 * anterior como o filho esquerdo e colocamos no filho direito o termo a ser
	 * processado.
	 */
	while (*in == '+' || *in == '-') {
		CriaNo(arv, (*arv), NULL, *in);

		in++;
		PulaBrancos();

		e = Termo(&(*arv)->dir);
		if (e != EXPR_VALIDA) {
			return e;
		}
	}

	return EXPR_VALIDA;
}


/* Processa um termo da cadeia de entrada. */
CodigosErro Termo(ArvBin *arv) {
	CodigosErro e;

	/* O termo sempre come�a com um fator */
	e = Fator(arv);
	if (e != EXPR_VALIDA) {
		return e;
	}

	/* O termo pode ser seguido de um ou mais operadores (* ou /) e fatores.
	 * Analogamente a como fizemos na express�o, para cada fator, definimos o
	 * operador como a raiz da �rvore, a raiz anterior como o filho esquerdo
	 * e colocamos no filho direito o fator a ser processado.
	 */
	while (*in == '*' || *in == '/') {
		CriaNo(arv, (*arv), NULL, *in);

		in++;
		PulaBrancos();

		e = Fator(&(*arv)->dir);
		if (e != EXPR_VALIDA) {
			return e;
		}
	}

	return EXPR_VALIDA;
}

/* Processa um fator da cadeia de entrada. */
CodigosErro Fator(ArvBin *arv) {
	CodigosErro e;

	/* O fator sempre come�a com um prim�rio */
	e = Primario(arv);
	if (e != EXPR_VALIDA) {
		return e;
	}

	/* O prim�rio pode ser seguido de um operador ^ e outro fator. Analogamente
	 * a como fizemos no termo, caso haja outra fator, definimos o operador como
	 * a raiz da �rvore, a raiz anterior como o filho esquerdo e colocamos no
	 * filho direito o novo fator a ser processado.
	 *  */
	if (*in == '^') {
		CriaNo(arv, (*arv), NULL, *in);

		in++;
		PulaBrancos();

		e = Fator(&(*arv)->dir);
		if (e != EXPR_VALIDA) {
			return e;
		}
	}

	return EXPR_VALIDA;
}


/* Processa um prim�rio da cadeia de entrada. */
CodigosErro Primario(ArvBin *arv) {
	CodigosErro e;

	/* O prim�rio pode ser um operador */
	if (*in >= 'a' && *in <= 'z') {
		CriaNo(arv, NULL, NULL, *in);

		in++;
		PulaBrancos();

	/* O prim�rio pode ser uma express�o entre par�nteses */
	} else if (*in == '(') {
		in++;
		PulaBrancos();

		e = Expressao(arv);
		if (e != EXPR_VALIDA) {
			return e;
		} else if (*in != ')') {
			return FECHA_PARENTESE_ESPERADO;
		} else {
			in++;
			PulaBrancos();
		}

	/* N�o � um prim�rio v�lido */
	} else {
		return OPERANDO_ESPERADO;
	}

	return EXPR_VALIDA;
}

/* Percursos */

/* Essa fun��o apenas chama uma fun��o auxiliar e coloca o caractere final na
 * cadeia de caracteres para produzir uma representa��o pr�-fixa da express�o
 * armazenada na �rvore.
 */
void ArvPre(ArvBin arv, char *pre) {
	ArvPreAux(arv, &pre);
	*pre = '\0';
}

/* Essa fun��o apenas chama uma fun��o auxiliar e coloca o caractere final na
 * cadeia de caracteres para produzir uma representa��o p�s-fixa da express�o
 * armazenada na �rvore.
 */
void ArvPos(ArvBin arv, char *pos) {
	ArvPosAux(arv, &pos);
	*pos = '\0';
}

/* Percorre a �rvore em pr�-ordem, recursivamente, produzindo uma representa��o
 * pr�-fixa da express�o nela armazenada.
 */
void ArvPreAux(ArvBin arv, char **pre) {
	if (arv == NULL) {
		return;
	}

	**pre = arv->info;
	(*pre)++;

	ArvPreAux(arv->esq, pre);
	ArvPreAux(arv->dir, pre);
}

/* Percorre a �rvore em p�s-ordem, recursivamente, produzindo uma representa��o
 * pr�-fixa da express�o nela armazenada.
 */
void ArvPosAux(ArvBin arv, char **pos) {
	if (arv == NULL) {
		return;
	}

	ArvPosAux(arv->esq, pos);
	ArvPosAux(arv->dir, pos);

	**pos = arv->info;
	(*pos)++;
}

/* Percorre a �rvore em p�s-ordem, liberando a mem�ria utilizada por todos os
 * seus n�s.
 */
void LiberaArv(ArvBin arv) {
	if (arv == NULL) {
		return;
	}

	LiberaArv(arv->esq);
	LiberaArv(arv->dir);

	FREE(arv);
}

