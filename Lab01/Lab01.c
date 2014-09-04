/**
 * Programa: Lab01.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 03/09/2014
 */

#include <stdio.h>
#include <stdbool.h>

#define MATRIX_MAX_ORDER 20

bool readData(int *matrixOrder, int matrix[MATRIX_MAX_ORDER][MATRIX_MAX_ORDER]);
void skipLine();

int main() {
	int i, j;
	int matrixOrder;
	int matrix[MATRIX_MAX_ORDER][MATRIX_MAX_ORDER];

	if (!readData(&matrixOrder, matrix)) {
		printf("Dados inválidos\n");
		return 0;
	}



	return 0;
}

bool readData(int *matrixOrder, int matrix[MATRIX_MAX_ORDER][MATRIX_MAX_ORDER]) {
	int i, j;

	scanf("%d", matrixOrder);
	skipLine();

	for (i = 0; i < *matrixOrder; i++) {
		for (j = 0; j < *matrixOrder; j++) {

			/* A função `scanf` retorna o número de argumentos corretamente
			 * preenchidos. Caso nosso único argumento não tenha sido assim
			 * preenchido, retornamos false indicando que a leitura não foi
			 * efetuada com sucesso.
			 */
			if (scanf("%d", &matrix[i][j]) != 1) {
				return false;
			}
		}

		skipLine();
	}

	return true;
}

void skipLine() {
	scanf("%*[^\n]");
}