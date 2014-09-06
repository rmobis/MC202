/**
 * Programa: Lab01.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 03/09/2014
 */

#include <stdio.h>
#include <stdbool.h>

#define MATRIX_MAX_ORDER 20

void findSela(int matrixOrder, int matrix[MATRIX_MAX_ORDER][MATRIX_MAX_ORDER]);
bool readData(int *matrixOrder, int matrix[MATRIX_MAX_ORDER][MATRIX_MAX_ORDER]);
void skipLine();

int main() {
	int matrixOrder;
	int matrix[MATRIX_MAX_ORDER][MATRIX_MAX_ORDER];

	if (!readData(&matrixOrder, matrix)) {
		printf("Dados inválidos\n");
		return 0;
	}

	printf("Os pontos de sela da matriz são:\n\n");

	findSela(matrixOrder, matrix);

	return 0;
}

void findSela(int matrixOrder, int matrix[MATRIX_MAX_ORDER][MATRIX_MAX_ORDER]) {
	int i, j;
	int curItem;
	int lineMin[MATRIX_MAX_ORDER], colMax[MATRIX_MAX_ORDER];
	bool hasOneSela = false;

	for (i = 0; i < matrixOrder; i++) {
		lineMin[i] = matrix[i][0];

		for (j = 1; j < matrixOrder; j++) {
			if (matrix[i][j] < lineMin[i]) {
				lineMin[i] = matrix[i][j];
			}
		}
	}

	for (j = 0; j < matrixOrder; j++) {
		colMax[j] = matrix[0][j];

		for (i = 1; i < matrixOrder; i++) {
			if (matrix[i][j] > colMax[j]) {
				colMax[j] = matrix[i][j];
			}
		}
	}

	for (i = 0; i < matrixOrder; i++) {
		for (j = 0; j < matrixOrder; j++) {
			curItem = matrix[i][j];

			if (curItem == lineMin[i] && curItem == colMax[j]) {
				hasOneSela = true;
				printf("%4d%4d%4d\n", i, j, curItem);
			}
		}
	}

	if (!hasOneSela) {
		printf("    nenhum\n");
	}
}

bool readData(int *matrixOrder, int matrix[MATRIX_MAX_ORDER][MATRIX_MAX_ORDER]) {
	int i, j;

	scanf("%d", matrixOrder);
	skipLine();

	if (*matrixOrder < 1 || *matrixOrder > MATRIX_MAX_ORDER) {
		return false;
	}

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