#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define REPS 100000

int busquedaBinaria(int *arr, int n, int buscado, long *comp)
{
	int inicio = 0;
	int fin = n - 1;
	int medio;

	*comp = 0;

	while (inicio <= fin)
	{
		medio = inicio + (fin - inicio) / 2;
		(*comp)++;

		if (arr[medio] == buscado)
			return medio;

		if (buscado < arr[medio])
			fin = medio - 1;
		else
			inicio = medio + 1;
	}

	return -1;
}

int llenarArreglo(int *arr, int n)
{
	int i;

	for (i = 0; i < n; i++)
		arr[i] = i * 2;   /* ordenado, solo numeros pares */

	return 0;
}

int main()
{
	int tamanos[5] = {100, 1000, 10000, 100000, 1000000};
	char *nombres[4] = {"Inicio", "Centro", "Final", "No existe"};
	int *arr;
	int t, c, r, n;
	int objetivo, pos;
	long comp;
	long suma;
	clock_t t1, t2;
	double ns;

	printf("n\tCaso\t\tPos\tComparaciones\tTiempo(ns)\n");
	printf("-----------------------------------------------------------\n");

	for (t = 0; t < 5; t++)
	{
		n = tamanos[t];
		arr = (int *)malloc(n * sizeof(int));
		llenarArreglo(arr, n);   /* se hace ANTES del cronometro */

		for (c = 0; c < 4; c++)
		{
			if (c == 0) objetivo = arr[0];
			if (c == 1) objetivo = arr[n / 2];
			if (c == 2) objetivo = arr[n - 1];
			if (c == 3) objetivo = -1;

			suma = 0;

			t1 = clock();
			for (r = 0; r < REPS; r++)
			{
				pos = busquedaBinaria(arr, n, objetivo, &comp);
				suma += pos;   /* para que el compilador no elimine el ciclo */
			}
			t2 = clock();

			ns = ((double)(t2 - t1) / CLOCKS_PER_SEC) / REPS * 1e9;

			printf("%d\t%s\t\t%d\t%ld\t\t%.2f\n", n, nombres[c], pos, comp, ns);
		}

		free(arr);
		printf("\n");
	}

	return 0;
}
