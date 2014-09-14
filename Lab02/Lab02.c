/**
 * Programa: Lab02.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 13/09/2014
 *
 * L� uma matriz quadrada de ordem N, onde 0 < N < 21, e encontra seus pontos de
 * sela. A entrada � dada por uma linha inicial, contendo o valor inteiro de N,
 * seguida de N linhas contendo N inteiros, cada uma representando uma linha da
 * matriz. Todas as linhas aceitam coment�rios ap�s os dados. Caso a entrada n�o
 * se adeque ao formato especificado ou o valor de N esteja fora dos limites
 * estabelecidos, a saida � "Dados inv�lidos." Caso contr�rio, � "Os pontos de
 * sela da matriz s�o:", seguido de uma linha vazia e, para cada ponto de sela,
 * sua posi��o na matriz seguida de seu valor, em ordem de linha e coluna, todos
 * alinhados com 4 caracteres. Caso n�o haja nenhum ponto de sela, o programa
 * imprime "    nenhum". A sa�da sempre termina com um caractere de nova linha.
 */

/* Este m�dulo implementa opera��es sobre polin�mios em uma vari�vel.   */
/* Cada polin�mio � representado por uma lista ligada circular, com n�  */
/* cabe�a. Os n�s da lista representam os termos n�o nulos do polin�mio */
/* em ordem crescente dos seus expoentes. O n� cabe�a tem expoente '-1'.*/

#include "polinomios.h"
#include "balloc.h"
#include <stdio.h>

/*-----------------------  Fun��o auxiliar  ----------------------------*/


/* Insere o termo '(e,c)' ap�s o n� apontado por 'p'. */
void InsereTermoAux(Polinomio p, int e, float c) {
	Polinomio novoTermo = MALLOC(sizeof(Termo));

	novoTermo->expo = e;
	novoTermo->coef = c;
	novoTermo->prox = p->prox;

	p->prox = novoTermo;
}



/*---------------- Implementa��o das fun��es da interface --------------*/


/* Imprime, em ordem crescente dos expoentes, os termos do n�o nulos do 'p'. No
 * caso do polin�mio identicamente nulo, imprime as palavras "Polin�mio nulo".
 */
void ImprimePolinomio(Polinomio p) {
	if (p == NULL) {
		printf("Polin�mio inv�lido\n");
	} else if (p->prox == p) {
		printf("Polin�mio nulo\n");
	} else {
		p = p->prox;
		while (p->expo != -1) {
			printf("(%2d,%5.1f) ", p->expo, p->coef);
			p = p->prox;
		}
		printf("\n");
	}
}

/* Devolve um novo polin�mio identicamente nulo. */
Polinomio CriaPolinomioNulo() {
	Polinomio polinomioNulo = MALLOC(sizeof(Termo));

	polinomioNulo->expo = -1;
	polinomioNulo->prox = polinomioNulo;

	return polinomioNulo;
}


/* Libera toda a mem�ria din�mica ocupada por um polin�mio. */
void LiberaPolinomio(Polinomio p) {
	Polinomio q;

	/* Pula o n� cabe�a */
	p = p->prox;

	/* Libera todos antes do n� cabe�a */
	while (p->expo != -1) {
		q = p->prox;
		FREE(p);
		p = q;
	}

	/* Libera o n� cabe�a */
	FREE(p);
}

/* Verifica se o polin�mio 'p' � identicamente nulo. */
Boolean PolinomioNulo(Polinomio p) {
	return p->prox == p;
}

/* Insere no polinomio 'p' o termo '(e,c)', de maneira a manter os termos
 * ordenados. Sup�e que n�o existe ainda em 'p' um termo com expoente 'e', e que
 * 'c' n�o � zero.
 */
void InsereTermo(Polinomio p, int e, float c) {
	while (p->prox->expo < e && p->prox->expo != -1) {
		p = p->prox;
	}

	InsereTermoAux(p, e, c);
}

/* Devolve a soma dos polin�mios 'a' e 'b'. N�o altera os polin�mios dados.
 * Termos nulos n�o s�o inseridos no resultados.
 */
Polinomio SomaPolinomios(Polinomio a, Polinomio b) {
	Polinomio c = CriaPolinomioNulo();

	a = a->prox;
	b = b->prox;

	/* Enquanto 'a' e 'b' ainda tiverem termos n�o adicionados, realiza a soma */
	while (a->expo != -1 && b->expo != -1) {
		if (a->expo < b->expo) {
			InsereTermoAux(c, a->expo, a->coef);
			a = a->prox;
			c = c->prox;
		} else if (a->expo > b->expo) {
			InsereTermoAux(c, b->expo, b->coef);
			b = b->prox;
			c = c->prox;
		} else {
			if (a->coef + b->coef != 0) {
				InsereTermoAux(c, a->expo, a->coef + b->coef);
				c = c->prox;
			}
			a = a->prox;
			b = b->prox;
		}
	}

	/* Adiciona os termos restantes de 'a' */
	while (a->expo != -1) {
		InsereTermoAux(c, a->expo, a->coef);
		a = a->prox;
		c = c->prox;
	}

	/* Adiciona os termos restantes de 'b' */
	while (b->expo != -1) {
		InsereTermoAux(c, b->expo, b->coef);
		b = b->prox;
		c = c->prox;
	}

	/* Como 'c' aponta para o �ltimo termo da lista, retornamos 'c->prox' pois
	 * assim o endere�o retornado � o do n� cabe�a.
	 */
	return c->prox;
}


/* Devolve o polin�mio 'p' multiplicado pelo termo '(e,c)'. Sup�e que 'c' n�o �
 * nulo. N�o altera o polin�mio dado.
 */
Polinomio MultTermo(Polinomio p, int e, float c) {
	Polinomio q = CriaPolinomioNulo();

	p = p->prox;

	while (p->expo != -1) {
		InsereTermoAux(q, p->expo + e, p->coef * c);
		p = p->prox;
		q = q->prox;
	}

	return q->prox;
}

/* Devolve o produto dos polin�mios 'a' e 'b'. N�o altera os polin�mios dados. */
Polinomio MultPolinomios(Polinomio a, Polinomio b) {
	Polinomio d, e, c = CriaPolinomioNulo();

	b = b->prox;

	while (b->expo != -1) {
		d = MultTermo(a, b->expo, b->coef);
		e = c;
		c = SomaPolinomios(c, d);

		LiberaPolinomio(d);
		LiberaPolinomio(e);
		b = b->prox;
	}

	return c;
}
