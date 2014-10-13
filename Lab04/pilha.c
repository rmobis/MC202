/**
 * Programa: pilha.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 27/09/2014
 *
 * Este m�dulo implementa opera��es sobre uma pilha de elementos do tipo void*.
 * A pilha � implementada como um struct com um vetor 'vetor' e um membro 'topo'
 * que indica o item no topo da pilha. Pilhas vazias possuem 'topo' igual a 0.
 * Implementa verifica��es para casos onde tetamos empilhar mais elementos em
 * uma pilha j� cheia ou quando tentamos desempilhar elementos de uma pilha j�
 * vazia.
 */

#include <stdio.h>
#include <stdlib.h>
#include "pilha.h"

/* Mensagens de erro para este m�dulo.                                     */
#define MSG_ERRO_PILHA_CHEIA "n�mero m�ximo de elementos na pilha excedido."
#define MSG_ERRO_PILHA_VAZIA "tentativa de desempilhar elemento de pilha vazia."

#define IMPRIME_ERRO(msg)  { printf("%s\n",msg); exit(0); }

/* Inicializa uma pilha. */
void CriaPilha (Pilha* p) {
	p->topo = -1;
}

/* Verifica se a pilha est� vazia. */
Boolean PilhaVazia (Pilha* p) {
	return p->topo == -1;
}

void Empilha (Pilha* p, void *elem) {
	(p->topo)++;

	if (p->topo >= MAX_ELEM_PILHA) {
		IMPRIME_ERRO(MSG_ERRO_PILHA_CHEIA);
	}

	p->vetor[p->topo] = elem;
}

/* Desempilha um elemento da pilha. */
void* Desempilha (Pilha* p) {
    if (PilhaVazia(p)) {
    	IMPRIME_ERRO(MSG_ERRO_PILHA_VAZIA);
    }

	return p->vetor[(p->topo)--];
}