/**
 * Programa: base.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 30/11/2014
 *
 * Módulo de implementação das funções de manipulação de hash tables, usando uma
 * função simples de espalhamento, usando uma lista para colisões e organizando-as
 * em ordem crescente por RA.
 */

#include <stdio.h>
#include <string.h>
#include "base.h"

/* Constantes em CAPS, por favor */
#define MAX_HASHES MaxHash


/******************************************************************************
 *                            Declarações de Tipos                            *
 ******************************************************************************/

typedef struct RegLista *ImplLista;
typedef struct RegLista {
	Aluno aluno;
	ImplLista prox;
} RegLista;

typedef struct RegBase *ImplBase;
typedef struct RegBase {
	int numRegs;
	ImplLista tabela[MAX_HASHES];
} RegBase;


/******************************************************************************
 *                           Função de Espalhamento                           *
 ******************************************************************************/

int Espalha(int k) {
	return k % MAX_HASHES;
}


/******************************************************************************
 *                      Funções de manipulação de Listas                      *
 ******************************************************************************/

/* Protótipos */
void LiberaLista(ImplLista iL);

/* Devolve um ponteiro para uma nova lista. Esta lista pode ser vazia ou não,
 * dependendo dos argumentos passados.
 */
ImplLista CriaLista(Aluno *aluno, ImplLista prox) {
	ImplLista iL = MALLOC(sizeof(RegLista));

	if (aluno != NULL) {
		iL->aluno.ra   = aluno->ra;
		iL->aluno.nome = aluno->nome;
	}

	iL->prox = prox;

	return iL;
}

/* Tenta inserir o aluno 'aluno' na lista 'iL', em ordem crescente de RA. Caso
 * ele ainda não esteja na lista, insere-o e o retorna true; caso contrário,
 * retorna false.
 */
Boolean InsereLista(ImplLista iL, Aluno *aluno) {
	/* Procura o aluno que antecede o aluno a ser inserido */
	while (iL->prox != NULL) {
		if (iL->prox->aluno.ra >= aluno->ra) {
			break;
		}

		iL = iL->prox;
	}

	/* Se o aluno ainda não foi inserido, insere-o posição correta */
	if (iL->prox == NULL || iL->prox->aluno.ra != aluno->ra) {
		iL->prox = CriaLista(aluno, iL->prox);

		return true;
	}

	return false;
}

/* Tenta remover o aluno de RA 'RA' da lista 'iL'. Caso ele esteja na lista,
 * remove-o e o retorna true; caso contrário, retorna false.
 */
Boolean RemoveLista(ImplLista iL, int RA) {
	/* Procura o aluno a ser removido */
	while (iL->prox != NULL) {
		if (iL->prox->aluno.ra == RA) {
			/* Remove o nó */
			ImplLista noRemovido = iL->prox;
			iL->prox = noRemovido->prox;

			/* Libera o nó */
			noRemovido->prox = NULL;
			LiberaLista(noRemovido);

			return true;
		}

		iL = iL->prox;
	}

	return false;
}

/* Procura o aluno de RA 'RA' na lista 'iL'. Caso ele esteja na lista, o coloca
 * na variável 'aluno' e retorna true; caso contrário, retorna false.
 */
Boolean ConsultaLista(ImplLista iL, int RA, Aluno *aluno) {
	/* Pula o nó cabeça */
	iL = iL->prox;

	/* Procura o aluno */
	while (iL != NULL) {
		if (iL->aluno.ra == RA) {
			*aluno = iL->aluno;

			return true;
		}

		iL = iL->prox;
	}

	return false;
}

/* Imprime os alunos presentes na lista 'iL', um por linha e em ordem alfabética,
 * seguindo o formato "(%3d) %06d %s" onde o primeiro argumento é o número da
 * linha na tabela de hash, o segundo o RA do aluno e o terceiro o seu nome.
 */
void ImprimeLista(ImplLista iL, int linha) {
	/* Pula o nó cabeça */
	iL = iL->prox;

	/* Imprime os nós */
	while (iL != NULL) {
		printf("(%3d) %06d %s\n", linha, iL->aluno.ra, iL->aluno.nome);

		iL = iL->prox;
	}
}

/* Libera toda a memória ocupada pela lista 'iL' e os alunos nesta contidos. */
void LiberaLista(ImplLista iL) {
	if (iL->prox != NULL) {
		LiberaLista(iL->prox);
	}

	FREE(iL->aluno.nome);
	FREE(iL);
}


/******************************************************************************
 *                    Funções de manipulação da Hash Table                    *
 ******************************************************************************/

/* Devolve um ponteiro para uma nova base vazia, com a tabela devidamente
 * incializada.
 */
Base CriaBase() {
	ImplBase iB = MALLOC(sizeof(RegBase));

	iB->numRegs = 0;

	/* Inicialize a tabela */
	for (int i = 0; i < MAX_HASHES; i++) {
		iB->tabela[i] = CriaLista(NULL, NULL);
	}

	return (Base) iB;
}

/* Tenta inserir o aluno 'aluno' na base 'b', em ordem crescente de RA. Caso
 * ele ainda não esteja na base, insere-o e o retorna true; caso contrário,
 * retorna false.
 */
Boolean InsereBase(Base b, Aluno *aluno) {
	ImplBase iB = (ImplBase) b;

	int i = Espalha(aluno->ra);

	if (InsereLista(iB->tabela[i], aluno)) {
		(iB->numRegs)++;
		return true;
	}

	return false;
}

/* Tenta remover o aluno de RA 'RA' da base 'b'. Caso ele esteja na base,
 * remove-o e o retorna true; caso contrário, retorna false.
 */
Boolean RemoveBase(Base b, int RA) {
	ImplBase iB = (ImplBase) b;

	int i = Espalha(RA);

	if (RemoveLista(iB->tabela[i], RA)) {
		(iB->numRegs)--;
		return true;
	}

	return false;
}

/* Procura o aluno de RA 'RA' na base 'b'. Caso ele esteja na base, o coloca
 * na variável 'aluno' e retorna true; caso contrário, retorna false.
 */
Boolean ConsultaBase(Base b, int RA, Aluno *aluno) {
	ImplBase iB = (ImplBase) b;

	int i = Espalha(RA);

	return ConsultaLista(iB->tabela[i], RA, aluno);
}

/* Devolve o número total de alunos inseridos na base 'b'. */
int NumeroRegsBase(Base b) {
	ImplBase iB = (ImplBase) b;

	return iB->numRegs;
}

/* Imprime os alunos presentes na base 'b', um por linha e, para cada linha da
 * tabela de hash, em ordem alfabética, seguindo o formato "(%3d) %06d %s" onde
 * o primeiro argumento é o número da linha na tabela de hash, o segundo o RA do
 * aluno e o terceiro o seu nome.
 */
void ImprimeBase(Base b) {
	ImplBase iB = (ImplBase) b;

	/* Imprime todas as linhas */
	for (int i = 0; i < MAX_HASHES; i++) {
		ImprimeLista(iB->tabela[i], i);
	}
}

/* Libera toda a memória ocupada pela base 'b' e os alunos nesta contidos. */
void LiberaBase(Base b) {
	ImplBase iB = (ImplBase) b;

	/* Libera todas as linhas */
	for (int i = 0; i < MAX_HASHES; i++) {
		LiberaLista(iB->tabela[i]);
	}

	FREE(iB);
}
