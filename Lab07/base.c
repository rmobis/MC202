/**
 * Programa: base.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 02/11/2014
 *
 * Módulo de implementação das funções de manipulação da base de alunos, usando
 * uma árvore binária de busca.
 */

#include <stdio.h>
#include "base.h"
#include "balloc.h"

typedef struct AuxNoArv {
	Aluno info;                   /* registros de informação */
	struct AuxNoArv *esq, *dir;   /* sub-árvores */
} NoArv, *ImplBase;


Base CriaBase() {
	return NULL;
}

/* Caso o registro 'aluno' não já exista na base, insere-o e retorna true; false
 * caso contrário.
 */
Boolean InsereBase(Base *p, Aluno aluno) {
	if (*p == NULL) {
		*p = MALLOC(sizeof(NoArv));
		((ImplBase) *p)->info.ra = aluno.ra;
		((ImplBase) *p)->info.nome = aluno.nome;
		((ImplBase) *p)->esq = ((ImplBase) *p)->dir = NULL;

		return true;
	}

	ImplBase b = (ImplBase) *p;

	/* Inserção recursiva */
	if (aluno.ra < b->info.ra) {
		return InsereBase((Base*) &b->esq, aluno);
	} else if (aluno.ra > b->info.ra) {
		return InsereBase((Base*) &b->dir, aluno);
	} else {
		return false;
	}
}

/* Consulta na base por um registra com 'ra'. Caso esse registro exista coloca
 * as informações em 'a' e retorna true; false caso contrário.
 */
Boolean ConsultaBase(Base *p, int ra, Aluno *aluno) {
	if (*p == NULL) {
		return false;
	}

	ImplBase b = (ImplBase) *p;

	/* Busca recursiva */
	if (ra < b->info.ra) {
		return ConsultaBase((Base*) &b->esq, ra, aluno);
	} else if (ra > b->info.ra) {
		return ConsultaBase((Base*) &b->dir, ra, aluno);
	} else {
		*aluno = b->info;
		return true;
	}
}

/* Tenta remover um registro com 'ra'. Caso a remoção tenha sido bem sucedida,
 * retorna true; false caso contrário.
 */
Boolean RemoveBase(Base *p, int ra) {
	if (*p == NULL) {
		return false;
	}

	ImplBase b = (ImplBase) *p;

	/* Remoção recursiva */
	if (ra < b->info.ra) {
		return RemoveBase((Base*) &b->esq, ra);
	} else if (ra > b->info.ra) {
		return RemoveBase((Base*) &b->dir, ra);
	} else {
		/* Se uma das sub-árvores é NULA, basta substituir o elemento pela outra
		 * sub-árvore, não necessariamente não vazia.
		 */
		if (b->esq == NULL || b->dir == NULL) {
			*p = (Base) (b->esq != NULL ? b->esq : b->dir);
			FREE(b->info.nome);
			FREE(b);
		} else {
			Aluno tmpAluno = b->info;

			/* Pega o elemento seguinte ao que queremos remover; ou seja, o
			 * mínimo da sub-árvore direita.
			 */
			ImplBase *pB = &b->dir;
			while ((*pB)->esq != NULL) {
				pB = &(*pB)->esq;
			}

			/* Troca os alunos nos elementos */
			((ImplBase) *p)->info = (*pB)->info;
			(*pB)->info = tmpAluno;

			/* Agora remove o aluno, que está numa folha */
			RemoveBase((Base*) pB, ra);
		}

		return true;
	}
}

/* Devolve a altura da base 'p'. */
int AlturaBase(Base *p) {
	if (*p == NULL) {
		return 0;
	}

	ImplBase b = (ImplBase) *p;

	int alturaEsq = AlturaBase((Base*) &b->esq);
	int alturaDir = AlturaBase((Base*) &b->dir);

	/* Retornamos a altura da sub-árvore mais alta, acresida de 1 */
	return 1 + (alturaEsq > alturaDir ? alturaEsq : alturaDir);
}

/* Devolve o número de nós da base 'p'. */
int NumeroNosBase(Base *p) {
	if (*p == NULL) {
		return 0;
	}

	ImplBase b = (ImplBase) *p;

	return 1 + NumeroNosBase((Base*) &b->esq) + NumeroNosBase((Base*) &b->dir);
}


/* Executa um percurso inordem na base, invocando a função 'Visita' para todos
 * os elementos.
 */
void PercorreBase(Base *p, void(*Visita)(Aluno*)) {
	if (*p == NULL) {
		return;
	}

	ImplBase b = (ImplBase) *p;

	PercorreBase((Base*) &b->esq, Visita);
	Visita(&b->info);
	PercorreBase((Base*) &b->dir, Visita);
}

/* Libera todos os nós da base apontada por 'p', bem como todas as cadeias que
 * guardam os nomes.
 */
void LiberaBase(Base *p) {
	if (*p == NULL) {
		return;
	}

	ImplBase b = (ImplBase) *p;

	LiberaBase((Base*) &b->esq);
	LiberaBase((Base*) &b->dir);

	FREE(b->info.nome);
	FREE(b);
}
