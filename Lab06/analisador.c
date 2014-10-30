/**
 * Programa: analisador.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 28/10/2014
 *
 * Módulo de transformação de expressões na notação infixa para uma árvore binária
 * que pode ser posteriormente percorrida em pós-ordem ou pré-ordem para se obter
 * as mesma expressões em notação pós-fixa ou pré-fixa, respectivamente.
 */

#include "analisador.h"
#include "balloc.h"

/* Variáveis globais a este módulo */
char *in;  /* Cadeia e índice para expressão infixa (entrada). */

/* Protótipos das funções mutuamente recursivas. Optamos por retornar apenas
 * o código do erro, sem sua posição, pois esta pode ser calculada depois, como
 * explicitado mais abaixo.
 */
CodigosErro Expressao(ArvBin *arv);
CodigosErro Termo(ArvBin *arv);
CodigosErro Fator(ArvBin *arv);
CodigosErro Primario(ArvBin *arv);

/* Funções Auxiliares */
void PulaBrancos();
void CriaNo(ArvBin *arv, ArvBin esq, ArvBin dir, char info);

/* Funções Auxiliares de Percursos */
void ArvPreAux(ArvBin arv, char **pre);
void ArvPosAux(ArvBin arv, char **pos);


/*************************************************************/
/*                 Função   principal                        */
/*************************************************************/

/* Transforma uma expressão da notação infixa para um árvore binária. Em caso
 * de erro, devolve o código e a posição na cadeia de entrada onde o erro foi
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

		/* A posição pode ser calculada pelo endereço final, que é onde o último
		 * caractere foi lido, subtraído do endereço inicial, dividido pelo
		 * tamanho de um char.
		 */
		e.posicao = ((int)(in - infixa)) / sizeof(char);

		c = *in;

		/* Se a expressão transformada é válida, mas não consumiu toda a cadeia
		 * de entrada, então é porque esperavamos um operador e a expressão
		 * total não é de fato válida.
		 */
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
	return e;
}

/*************************************************************/
/*           Funções de implementação do analisador          */
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

/* Processa uma expressão da cadeia de entrada. */
CodigosErro Expressao(ArvBin *arv) {
	CodigosErro e;
	char op = 0;

	/* A expressão pode começar com um operador unário */
	if (*in == '+') {
		op = '&';
		in++;
		PulaBrancos();
	} else if (*in == '-') {
		op = '~';
		in++;
		PulaBrancos();
	}

	/* A expressão sempre tem um termo, após o possível operador unário */
	e = Termo(arv);
	if (e != EXPR_VALIDA) {
		return e;
	}

	/* Se temos um operador unário, substituimos a raiz pelo operador e
	 * colocamos o termo que foi processado à direita. Usamos o nó da direita ao
	 * invés do da esquerda como de costume com os operadores binários pois
	 * assim nós poderiamos, teoricamente, ler a árvore em Inordem para obtermos
	 * novamente a expressão infixa.
	 */
	if (op != 0) {
		CriaNo(arv, NULL, (*arv), op);
	}

	/* A expressão pode ser seguida de um ou mais operadores (+ ou -) e termos.
	 * Para cada um, definimos este operador como a raiz da árvore, a raiz
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

	/* O termo sempre começa com um fator */
	e = Fator(arv);
	if (e != EXPR_VALIDA) {
		return e;
	}

	/* O termo pode ser seguido de um ou mais operadores (* ou /) e fatores.
	 * Analogamente a como fizemos na expressão, para cada fator, definimos o
	 * operador como a raiz da árvore, a raiz anterior como o filho esquerdo
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

	/* O fator sempre começa com um primário */
	e = Primario(arv);
	if (e != EXPR_VALIDA) {
		return e;
	}

	/* O primário pode ser seguido de um operador ^ e outro fator. Analogamente
	 * a como fizemos no termo, caso haja outra fator, definimos o operador como
	 * a raiz da árvore, a raiz anterior como o filho esquerdo e colocamos no
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


/* Processa um primário da cadeia de entrada. */
CodigosErro Primario(ArvBin *arv) {
	CodigosErro e;

	/* O primário pode ser um operador */
	if (*in >= 'a' && *in <= 'z') {
		CriaNo(arv, NULL, NULL, *in);

		in++;
		PulaBrancos();

	/* O primário pode ser uma expressão entre parênteses */
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

	/* Não é um primário válido */
	} else {
		return OPERANDO_ESPERADO;
	}

	return EXPR_VALIDA;
}

/* Percursos */

/* Essa função apenas chama uma função auxiliar e coloca o caractere final na
 * cadeia de caracteres para produzir uma representação pré-fixa da expressão
 * armazenada na árvore.
 */
void ArvPre(ArvBin arv, char *pre) {
	ArvPreAux(arv, &pre);
	*pre = '\0';
}

/* Essa função apenas chama uma função auxiliar e coloca o caractere final na
 * cadeia de caracteres para produzir uma representação pós-fixa da expressão
 * armazenada na árvore.
 */
void ArvPos(ArvBin arv, char *pos) {
	ArvPosAux(arv, &pos);
	*pos = '\0';
}

/* Percorre a árvore em pré-ordem, recursivamente, produzindo uma representação
 * pré-fixa da expressão nela armazenada.
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

/* Percorre a árvore em pós-ordem, recursivamente, produzindo uma representação
 * pré-fixa da expressão nela armazenada.
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

/* Percorre a árvore em pós-ordem, liberando a memória utilizada por todos os
 * seus nós.
 */
void LiberaArv(ArvBin arv) {
	if (arv == NULL) {
		return;
	}

	LiberaArv(arv->esq);
	LiberaArv(arv->dir);

	FREE(arv);
}

