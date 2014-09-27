/**
 * Programa: polinomios.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 27/09/2014
 *
 * Este módulo implementa operações sobre polinômios em uma variável. Cada
 * polinômio é representado por uma lista ligada circular, com nó cabeça. Os nós
 * da lista representam os termos não nulos do polinômio em ordem crescente dos
 * seus expoentes. O nó cabeça tem expoente '-1'.
 */

#include "polinomios.h"
#include "balloc.h"
#include <stdio.h>

/* Insere o termo '(e, c)' após o nó apontado por 'p'. */
void InsereTermoAux(Polinomio p, int e, float c) {
	Polinomio novoTermo = MALLOC(sizeof(Termo));

	novoTermo->expo = e;
	novoTermo->coef = c;
	novoTermo->prox = p->prox;

	p->prox = novoTermo;
}

/* Imprime, em ordem crescente dos expoentes, os termos do não nulos do 'p'. No
 * caso do polinômio identicamente nulo, imprime as palavras "Polinômio nulo".
 */
void ImprimePolinomio(Polinomio p) {
	if (p == NULL) {
		printf("Polinômio inválido\n");
	} else if (p->prox == p) {
		printf("Polinômio nulo\n");
	} else {
		p = p->prox;
		while (p->expo != -1) {
			printf("(%2d,%5.1f) ", p->expo, p->coef);
			p = p->prox;
		}
		printf("\n");
	}
}

/* Devolve um novo polinômio identicamente nulo. */
Polinomio CriaPolinomioNulo() {
	Polinomio polinomioNulo = MALLOC(sizeof(Termo));

	polinomioNulo->expo = -1;
	polinomioNulo->prox = polinomioNulo;

	return polinomioNulo;
}


/* Libera toda a memória dinâmica ocupada por um polinômio. */
void LiberaPolinomio(Polinomio p) {
	Polinomio q;

	/* Pula o nó cabeça */
	p = p->prox;

	/* Libera todos antes do nó cabeça */
	while (p->expo != -1) {
		q = p->prox;
		FREE(p);
		p = q;
	}

	/* Libera o nó cabeça */
	FREE(p);
}

/* Verifica se o polinômio 'p' é identicamente nulo. */
Boolean PolinomioNulo(Polinomio p) {
	return p->prox == p;
}

/* Insere no polinomio 'p' o termo '(e,c)', de maneira a manter os termos
 * ordenados. Supõe que não existe ainda em 'p' um termo com expoente 'e', e que
 * 'c' não é zero.
 */
void InsereTermo(Polinomio p, int e, float c) {
	while (p->prox->expo < e && p->prox->expo != -1) {
		p = p->prox;
	}

	InsereTermoAux(p, e, c);
}

/* Devolve a soma dos polinômios 'a' e 'b'. Não altera os polinômios dados.
 * Termos nulos não são inseridos no resultados.
 */
Polinomio SomaPolinomios(Polinomio a, Polinomio b) {
	Polinomio c = CriaPolinomioNulo();

	a = a->prox;
	b = b->prox;

	/* Enquanto 'a' e 'b' ainda tiverem termos não adicionados, realiza a soma */
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
			/* Só inserimos termos não nulos */
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

	/* Como 'c' aponta para o último termo da lista, retornamos 'c->prox' pois
	 * assim o endereço retornado é o do nó cabeça.
	 */
	return c->prox;
}

/* Devolve a subtração do polinômios 'b' de 'a'. Não altera os polinômios dados.
 * Termos nulos não são inseridos no resultados.
 */
Polinomio SubPolinomios(Polinomio a, Polinomio b) {
	Polinomio c, d;

	c = MultTermo(b, 0, -1);
	d = SomaPolinomios(a, c);

	LiberaPolinomio(c);

	return d;
}

/* Devolve o polinômio 'p' multiplicado pelo termo '(e, c)'. Supõe que 'c' não é
 * nulo. Não altera o polinômio dado.
 */
Polinomio MultTermo(Polinomio p, int e, float c) {
	Polinomio q = CriaPolinomioNulo();

	p = p->prox;

	while (p->expo != -1) {
		InsereTermoAux(q, p->expo + e, p->coef * c);
		p = p->prox;
		q = q->prox;
	}

	/* Como 'q' aponta para o último termo da lista, retornamos 'q->prox' pois
	 * assim o endereço retornado é o do nó cabeça.
	 */
	return q->prox;
}

/* Devolve o produto dos polinômios 'a' e 'b'. Não altera os polinômios dados. */
Polinomio MultPolinomios(Polinomio a, Polinomio b) {
	Polinomio d, e, c = CriaPolinomioNulo();

	b = b->prox;

	/* Realiza a soma de cada um dos termos do polinômio 'b' multiplicado pelo
	 * polinômio 'a'.
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