/**
 * Programa: polinomios.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 27/09/2014
 *
 * Este m�dulo implementa opera��es sobre polin�mios em uma vari�vel. Cada
 * polin�mio � representado por uma lista ligada circular, com n� cabe�a. Os n�s
 * da lista representam os termos n�o nulos do polin�mio em ordem crescente dos
 * seus expoentes. O n� cabe�a tem expoente '-1'.
 */

#include "polinomios.h"
#include "balloc.h"
#include <stdio.h>

/* Insere o termo '(e, c)' ap�s o n� apontado por 'p'. */
void InsereTermoAux(Polinomio p, int e, float c) {
	Polinomio novoTermo = MALLOC(sizeof(Termo));

	novoTermo->expo = e;
	novoTermo->coef = c;
	novoTermo->prox = p->prox;

	p->prox = novoTermo;
}

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
			/* S� inserimos termos n�o nulos */
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

/* Devolve a subtra��o do polin�mios 'b' de 'a'. N�o altera os polin�mios dados.
 * Termos nulos n�o s�o inseridos no resultados.
 */
Polinomio SubPolinomios(Polinomio a, Polinomio b) {
	Polinomio c, d;

	c = MultTermo(b, 0, -1);
	d = SomaPolinomios(a, c);

	LiberaPolinomio(c);

	return d;
}

/* Devolve o polin�mio 'p' multiplicado pelo termo '(e, c)'. Sup�e que 'c' n�o �
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

	/* Como 'q' aponta para o �ltimo termo da lista, retornamos 'q->prox' pois
	 * assim o endere�o retornado � o do n� cabe�a.
	 */
	return q->prox;
}

/* Devolve o produto dos polin�mios 'a' e 'b'. N�o altera os polin�mios dados. */
Polinomio MultPolinomios(Polinomio a, Polinomio b) {
	Polinomio d, e, c = CriaPolinomioNulo();

	b = b->prox;

	/* Realiza a soma de cada um dos termos do polin�mio 'b' multiplicado pelo
	 * polin�mio 'a'.
	 */
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