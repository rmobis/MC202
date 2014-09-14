/**
 * Programa: Lab01.c
 * Autor: Raphael Mobis Tacla   RA: 157104
 * Disciplina: MC202            Turma: F
 * Data: 03/09/2014
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

#include <stdio.h>
#include <stdbool.h>

#define MATRIX_MAX_ORDER 20

void findCriticalPoint(int matrixOrder, int matrix[MATRIX_MAX_ORDER][MATRIX_MAX_ORDER]);
bool readData(int *matrixOrder, int matrix[MATRIX_MAX_ORDER][MATRIX_MAX_ORDER]);
void skipLine();

int main() {
	int matrixOrder;
	int matrix[MATRIX_MAX_ORDER][MATRIX_MAX_ORDER];

	if (!readData(&matrixOrder, matrix)) {
		printf("Dados inv�lidos\n");
		return 0;
	}

	findCriticalPoint(matrixOrder, matrix);

	return 0;
}

/**
 * Encontra os pontos de sela na matriz e imprime suas posi��es e valores.
 */
void findCriticalPoint(int matrixOrder, int matrix[MATRIX_MAX_ORDER][MATRIX_MAX_ORDER]) {
	int i, j, curItem;
	bool hasCriticalPoint = false;
	int lineMin[MATRIX_MAX_ORDER], colMax[MATRIX_MAX_ORDER];

	printf("Os pontos de sela da matriz s�o:\n\n");

	/* Encontramos o valor m�nimo de cada linha */
	for (i = 0; i < matrixOrder; i++) {
		lineMin[i] = matrix[i][0];

		for (j = 1; j < matrixOrder; j++) {
			if (matrix[i][j] < lineMin[i]) {
				lineMin[i] = matrix[i][j];
			}
		}
	}

	/* Encontramos o valor m�ximo de cada coluna; n�s poderiamos ter feito isso
	 * juntamente com a procura dos valores m�nimos de cada linha, no loop de
	 * cima, trocando os valores das linhas pelos valores das colunas, e isso
	 * diminuiria o n�mero de opera��os que ter�amos de fazer, mas acabaria
	 * tornando o c�digo em geral mais lento. Isso ocorreria pois deixariamos
	 * de acessar a mem�ria de maneira continua e n�o poderiamos fazer uso do
	 * cache do processador. Testes com matrizes de ordem 400 mostraram que
	 * fazendo os loops separados, h� um ganho de cerca de 10% de performance.
	 *
	 * Mais: http://stackoverflow.com/q/9936132/1661358
	 */
	for (j = 0; j < matrixOrder; j++) {
		colMax[j] = matrix[0][j];

		for (i = 1; i < matrixOrder; i++) {
			if (matrix[i][j] > colMax[j]) {
				colMax[j] = matrix[i][j];
			}
		}
	}

	/* Comparamos cada elemento com os valores m�nimo e m�ximo de sua linha e
	 * coluna, respectivamente, para verificar se existe um ponto de sela.
	 */
	for (i = 0; i < matrixOrder; i++) {
		for (j = 0; j < matrixOrder; j++) {
			curItem = matrix[i][j];

			if (curItem == lineMin[i] && curItem == colMax[j]) {
				printf("%4d%4d%4d\n", i, j, curItem);
				hasCriticalPoint = true;
			}
		}
	}

	if (!hasCriticalPoint) {
		printf("    nenhum\n");
	}
}

/**
 * Faz a leitura dos dados necess�rios para o problema e salva nas vari�veis
 * recebidas como argumento. Retorna um booleano indicando se a leitura foi bem
 * sucedida ou n�o.
 */
bool readData(int *matrixOrder, int matrix[MATRIX_MAX_ORDER][MATRIX_MAX_ORDER]) {
	int i, j;

	scanf("%d", matrixOrder);
	skipLine();

	if (*matrixOrder < 1 || *matrixOrder > MATRIX_MAX_ORDER) {
		return false;
	}

	for (i = 0; i < *matrixOrder; i++) {
		for (j = 0; j < *matrixOrder; j++) {

			/* A fun��o `scanf` retorna o n�mero de argumentos corretamente
			 * preenchidos. Caso nosso �nico argumento n�o tenha sido assim
			 * preenchido, retornamos false indicando que a leitura n�o foi
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

/**
 * Pula toda entrada at� o final da linha.
 */
void skipLine() {
	scanf("%*[^\n]\n");
}