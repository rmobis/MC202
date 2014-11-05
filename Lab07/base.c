/**
 * Programa: base.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 02/11/2014
 *
 * M�dulo de implementa��o das fun��es de manipula��o da base de alunos, usando
 * uma �rvore bin�ria de busca.
 */

#include <stdio.h>
#include "base.h"
#include "balloc.h"

typedef struct AuxNoArv {
	Aluno info;                   /* registros de informa��o */
	struct AuxNoArv *esq, *dir;   /* sub-�rvores */
} NoArv, *ImplBase;


Base CriaBase() {
	return NULL;
}

/* Caso o registro 'aluno' n�o j� exista na base, insere-o e retorna true; false
 * caso contr�rio.
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

	/* Inser��o recursiva */
	if (aluno.ra < b->info.ra) {
		return InsereBase((Base*) &b->esq, aluno);
	} else if (aluno.ra > b->info.ra) {
		return InsereBase((Base*) &b->dir, aluno);
	} else {
		return false;
	}
}

/* Consulta na base por um registra com 'ra'. Caso esse registro exista coloca
 * as informa��es em 'a' e retorna true; false caso contr�rio.
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

/* Tenta remover um registro com 'ra'. Caso a remo��o tenha sido bem sucedida,
 * retorna true; false caso contr�rio.
 */
Boolean RemoveBase(Base *p, int ra) {
	if (*p == NULL) {
		return false;
	}

	ImplBase b = (ImplBase) *p;

	/* Remo��o recursiva */
	if (ra < b->info.ra) {
		return RemoveBase((Base*) &b->esq, ra);
	} else if (ra > b->info.ra) {
		return RemoveBase((Base*) &b->dir, ra);
	} else {
		/* Se uma das sub-�rvores � NULA, basta substituir o elemento pela outra
		 * sub-�rvore, n�o necessariamente n�o vazia.
		 */
		if (b->esq == NULL || b->dir == NULL) {
			*p = (Base) (b->esq != NULL ? b->esq : b->dir);
			FREE(b->info.nome);
			FREE(b);
		} else {
			Aluno tmpAluno = b->info;

			/* Pega o elemento seguinte ao que queremos remover; ou seja, o
			 * m�nimo da sub-�rvore direita.
			 */
			ImplBase *pB = &b->dir;
			while ((*pB)->esq != NULL) {
				pB = &(*pB)->esq;
			}

			/* Troca os alunos nos elementos */
			((ImplBase) *p)->info = (*pB)->info;
			(*pB)->info = tmpAluno;

			/* Agora remove o aluno, que est� numa folha */
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

	/* Retornamos a altura da sub-�rvore mais alta, acresida de 1 */
	return 1 + (alturaEsq > alturaDir ? alturaEsq : alturaDir);
}

/* Devolve o n�mero de n�s da base 'p'. */
int NumeroNosBase(Base *p) {
	if (*p == NULL) {
		return 0;
	}

	ImplBase b = (ImplBase) *p;

	return 1 + NumeroNosBase((Base*) &b->esq) + NumeroNosBase((Base*) &b->dir);
}


/* Executa um percurso inordem na base, invocando a fun��o 'Visita' para todos
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

/* Libera todos os n�s da base apontada por 'p', bem como todas as cadeias que
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
