/**
 * Programa: base.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 05/11/2014
 *
 * Módulo de implementação das funções de manipulação da base de alunos, usando
 * uma árvore AVL.
 */

#include <stdio.h>
#include "base.h"
#include "balloc.h"
#include "imprimearvore.h"

typedef struct AuxNoArv {
	Aluno info;                   /* registros de informação */
	struct AuxNoArv *esq, *dir;   /* sub-árvores */
	short int bal;                /* fator de balanceamento */
} NoArv, *ImplBase;


/* Protótipos das funções auxiliares */
Boolean AuxInsereBase(ImplBase *p, Aluno aluno, Boolean *altDif);
Boolean AuxRemoveBase(ImplBase *pB, int ra, Boolean *altDif);
void AuxCorrigeBalanceamentoEsq(ImplBase *pB, Boolean *altDif, Boolean insercao);
void AuxCorrigeBalanceamentoDir(ImplBase *pB, Boolean *altDif, Boolean insercao);
void AuxRotacionaEsq(ImplBase *b);
void AuxRotacionaDir(ImplBase *b);

/******************************************************************************
 *                  Funções Usadas na Impressão das Árvores                   *
 ******************************************************************************/

char cadeiaInfo[7] = "0000000"; /* conterá os dígitos do RA */
char *vazia = "Árvore vazia\n";

/* Retorna o elemento da esquerda de 'arv'. Supõe 'arv' != NULL. */
ImplBase esq(ImplBase arv, int _) {
	return arv->esq;
}

/* Retorna o elemento da direita 'arv'. Supõe 'arv' != NULL. */
ImplBase dir(ImplBase arv, int _) {
	return arv->dir;
}

/* Retorna o elemento de informação de 'arv'. Supõe 'arv' != NULL. */
char *info(ImplBase arv, int _) {
	sprintf(cadeiaInfo, "%6d", (arv->info).ra);

	return cadeiaInfo;
}

/* Imprime a base sob a forma de uma árvore 'deitada'. */
void ImprimeBase(Base *p) {
	ImplBase b = (ImplBase) *p;

	printf("\n");
	ImprimeArvore(
		b,
		(subarvfunc*) esq,
		(subarvfunc*) dir,
		(infofunc*) info,
		NULL,
		1,
		vazia
	);
	printf("\n");
}



/******************************************************************************
 *        Funções Específicas à Implementação de Base com Árvores AVL         *
 ******************************************************************************/

/* Devolve a subárvore esquerda da base 'p' se não for vazia; caso contrário
 * devolve NULL.
 */
Base SubEsq(Base *p) {
	if (*p == NULL) {
		return NULL;
	}

	return ((ImplBase) *p)->esq;
}

/* Devolve a subárvore direita da base 'p' se não for vazia; caso contrário
* devolve NULL.
*/
Base SubDir(Base *p) {
	if (*p == NULL) {
		return NULL;
	}

	return ((ImplBase) *p)->dir;
}

/* Devolve o fator de balanceamento da raiz da base: altura da sub-árvore
 * direita menos a altura da subárvore esquerda; devolve zero se a árvore é
 * vazia.
 */
int FatorBal(Base *p) {
	if (*p == NULL) {
		return 0;
	}

	ImplBase b = (ImplBase) *p;

	return AlturaBase((Base*) &b->dir) - AlturaBase((Base*) &b->esq);
}

/* Caso o registro 'aluno' não já exista na base, insere-o e retorna true; false
 * caso contrário.
 */
Boolean InsereBase(Base *p, Aluno aluno) {
	Boolean altDif;

	return AuxInsereBase((ImplBase *) p, aluno, &altDif);
}

/* Caso o registro 'aluno' não já exista na base, insere-o e retorna true; false
 * caso contrário.
 */
Boolean AuxInsereBase(ImplBase *pB, Aluno aluno, Boolean *altDif) {
	if (*pB == NULL) {
		*pB = MALLOC(sizeof(NoArv));
		(*pB)->bal = 0;
		(*pB)->info.ra = aluno.ra;
		(*pB)->info.nome = aluno.nome;
		(*pB)->esq = (*pB)->dir = NULL;

		*altDif = true;

		return true;
	}

	ImplBase b = *pB;

	/* Inserção recursiva */
	if (aluno.ra != b->info.ra) {
		if (aluno.ra < b->info.ra) {
			if (AuxInsereBase(&b->esq, aluno, altDif)) {
				AuxCorrigeBalanceamentoDir(pB, altDif, false);

				return true;
			}
		} else if (aluno.ra > b->info.ra) {
			if (AuxInsereBase(&b->dir, aluno, altDif)) {
				AuxCorrigeBalanceamentoEsq(pB, altDif, false);

				return true;
			}
		}
	}

	return false;
}

/* Tenta remover um registro com 'ra'. Caso a remoção tenha sido bem sucedida,
 * retorna true; false caso contrário.
 */
Boolean RemoveBase(Base *p, int ra) {
	Boolean altDif;

	return AuxRemoveBase((ImplBase*) p, ra, &altDif);
}

/* Tenta remover um registro com 'ra'. Caso a remoção tenha sido bem sucedida,
 * retorna true; false caso contrário.
 */
Boolean AuxRemoveBase(ImplBase *pB, int ra, Boolean *altDif) {
	if (*pB == NULL) {
		return false;
	}

	ImplBase b = *pB;

	/* Remoção recursiva */
	if (ra < b->info.ra) {
		if (AuxRemoveBase(&b->esq, ra, altDif)) {
			AuxCorrigeBalanceamentoEsq(pB, altDif, true);

			return true;
		}
	} else if (ra > b->info.ra) {
		if (AuxRemoveBase(&b->dir, ra, altDif)) {
			AuxCorrigeBalanceamentoDir(pB, altDif, true);

			return true;
		}
	} else {
		/* Se uma das sub-árvores é NULA, basta substituir o elemento pela outra
		 * sub-árvore, não necessariamente não vazia.
		 */
		if (b->esq == NULL || b->dir == NULL) {
			*pB = b->esq != NULL ? b->esq : b->dir;
			FREE(b->info.nome);
			FREE(b);

			*altDif = true;
		} else {
			Aluno tmpAluno = b->info;

			/* Pega o elemento seguinte ao que queremos remover; ou seja, o
			 * mínimo da sub-árvore direita.
			 */
			ImplBase *pBPai = pB;
			ImplBase *pBD = &b->dir;
			while ((*pBD)->esq != NULL) {
				pBPai = pBD;
				pBD = &(*pBD)->esq;
			}

			/* Troca os alunos nos elementos */
			((ImplBase) *pB)->info = (*pBD)->info;
			(*pBD)->info = tmpAluno;

			/* Agora remove o aluno, que está numa folha */
			AuxRemoveBase(pBD, ra, altDif);
			AuxCorrigeBalanceamentoEsq(pBPai, altDif, true);
		}

		return true;
	}

	return false;
}

/* Corrige o balanceamento da árvore, fazendo as rotações necessárias. */
void AuxCorrigeBalanceamentoEsq(ImplBase *pB, Boolean *altDif, Boolean remocao) {
	if (*altDif) {
		if ((*pB)->bal == 1) {
			/* Caso tenhamos que fazer um caso Right-Left, primeiro
			 * fazemos uma rotação para a direita, transformando-o
			 * em um caso Right-Right.
			 */
			if ((*pB)->dir->bal == -1) {
				AuxRotacionaDir(&(*pB)->dir);
			}

			AuxRotacionaEsq(pB);

			*altDif = false;
		} else {
			/* Verificamos se alteramos a altura da arvore. */
			*altDif = ((*pB)->bal == -((int) remocao));

			/* Corrige o fator de balanceamento */
			(*pB)->bal++;
		}
	}
}

/* Corrige o balanceamento da árvore, fazendo as rotações necessárias. */
void AuxCorrigeBalanceamentoDir(ImplBase *pB, Boolean *altDif, Boolean remocao) {
	if (*altDif) {
		if ((*pB)->bal == -1) {
			/* Caso tenhamos que fazer um caso Left-Right, primeiro
			 * fazemos uma rotação para a esquerda, transformando-o
			 * em um caso Left-Left.
			 */
			if ((*pB)->esq->bal == 1) {
				AuxRotacionaEsq(&(*pB)->esq);
			}

			AuxRotacionaDir(pB);

			*altDif = false;
		} else {
			/* Verificamos se alteramos a altura da arvore. */
			*altDif = ((*pB)->bal == ((int) remocao));

			/* Corrige o fator de balanceamento */
			(*pB)->bal--;
		}
	}
}

/* Realiza a rotação para a esquerda entre um nó e seu filho. */
void AuxRotacionaEsq(ImplBase *b) {
	ImplBase tmpBase = *b;

	*b = tmpBase->dir;
	tmpBase->dir = (*b)->esq;
	(*b)->esq = tmpBase;

	(*b)->bal = 0;
	tmpBase->bal = FatorBal((Base *) &tmpBase);
}

/* Realiza a rotação para a direita entre um nó e seu filho. */
void AuxRotacionaDir(ImplBase *b) {
	ImplBase tmpBase = *b;

	*b = tmpBase->esq;
	tmpBase->esq = (*b)->dir;
	(*b)->dir = tmpBase;

	(*b)->bal = 0;
	tmpBase->bal = FatorBal((Base *) &tmpBase);
}


/******************************************************************************
 *  Funções "Herdadas" da Implementação de Base com Árvores Binárias Simples  *
 ******************************************************************************/

/* Cria uma base vazia. */
Base CriaBase() {
	return NULL;
}

/* Consulta na base por um registro com 'ra'. Caso esse registro exista coloca
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
void PercorreBase(Base *p, TipoVisita Visita) {
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