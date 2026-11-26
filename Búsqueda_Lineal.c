#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NT 1000      /* objetivos aleatorios para el caso promedio */
#define TRIALS 5

long cmpL;           /* comparaciones de la busqueda lineal   */
long cmpB;           /* comparaciones de la busqueda binaria  */
long sink;           /* evita que el compilador borre las busquedas */

int busquedaLineal(int *arr, int n, int buscado)
{
	int i;
	cmpL = 0;
	for (i = 0; i < n; i++)
	{
		cmpL++;
		if (arr[i] == buscado)
			return i;
	}
	return -1;
}

int busquedaBinaria(int *arr, int n, int buscado)
{
	int ini = 0, fin = n - 1, med;
	cmpB = 0;
	while (ini <= fin)
	{
		med = ini + (fin - ini) / 2;
		cmpB++;
		if (arr[med] == buscado)
			return med;
		if (buscado < arr[med])
			fin = med - 1;
		else
			ini = med + 1;
	}
	return -1;
}

int comparar(const void *a, const void *b)
{
	return *(int *)a - *(int *)b;
}

double ahora()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1e9 + ts.tv_nsec;
}

int main()
{
	int tam[7] = {10, 100, 1000, 10000, 100000, 1000000, 10000000};
	char *caso[5] = {"inicio", "centro", "final", "no_existe", "promedio"};
	int t, c, r, i, j, n, aux, reps, tr;
	int *lin, *ord, *objs;
	volatile int obj;
	long suma, cmp;
	double t1, t2, tot, tsort;

	srand(12345);
	printf("alg\tn\tcaso\tcomparaciones\ttiempo_ns\n");

	for (t = 0; t < 7; t++)
	{
		n = tam[t];
		lin = (int *)malloc(n * sizeof(int));
		ord = (int *)malloc(n * sizeof(int));
		objs = (int *)malloc(NT * sizeof(int));

		/* datos generados ANTES de cronometrar: lin desordenado, ord ordenado */
		for (i = 0; i < n; i++)
			lin[i] = i * 2;
		for (i = n - 1; i > 0; i--)
		{
			j = ((long)rand() * 32768 + rand()) % (i + 1);
			aux = lin[i]; lin[i] = lin[j]; lin[j] = aux;
		}
		for (i = 0; i < n; i++)
			ord[i] = i * 2;
		for (i = 0; i < NT; i++)
			objs[i] = lin[((long)rand() * 32768 + rand()) % n];

		/* tiempo de ordenar con qsort (copia del arreglo desordenado) */
		aux = 0;
		tsort = 0;
		for (tr = 0; tr < 3; tr++)
		{
			int *cp = (int *)malloc(n * sizeof(int));
			for (i = 0; i < n; i++) cp[i] = lin[i];
			t1 = ahora();
			qsort(cp, n, sizeof(int), comparar);
			t2 = ahora();
			tsort += (t2 - t1) / 3.0;
			free(cp);
		}
		printf("qsort\t%d\tordenar\t0\t%.1f\n", n, tsort);

		for (c = 0; c < 5; c++)
		{
			/* ---------- lineal ---------- */
			cmp = 0;
			if (c == 0) obj = lin[0];
			if (c == 1) obj = lin[n / 2];
			if (c == 2) obj = lin[n - 1];
			if (c == 3) obj = -1;
			if (c < 4)
			{
				busquedaLineal(lin, n, obj);
				cmp = cmpL;
				reps = 200000000 / (cmp + 20);
			}
			else
			{
				suma = 0;
				for (i = 0; i < NT; i++)
				{
					busquedaLineal(lin, n, objs[i]);
					suma += cmpL;
				}
				cmp = suma / NT;
				reps = 200000000 / (cmp + 20) / 1;
			}
			if (reps < 5) reps = 5;
			if (reps > 20000000) reps = 20000000;

			tot = 0;
			for (tr = 0; tr < TRIALS; tr++)
			{
				suma = 0;
				t1 = ahora();
				for (r = 0; r < reps; r++)
				{
					if (c == 4) obj = objs[r % NT];
					suma += busquedaLineal(lin, n, obj);
				}
				t2 = ahora();
				tot += (t2 - t1) / reps;
			}
			printf("lineal\t%d\t%s\t%ld\t%.2f\n", n, caso[c], cmp, tot / TRIALS);
			sink += suma;

			/* ---------- binaria (solo comparacion) ---------- */
			if (c == 3 || c == 4)
			{
				if (c == 3) obj = -1;
				else obj = objs[0];
				busquedaBinaria(ord, n, obj);
				cmp = cmpB;
				if (c == 4)
				{
					suma = 0;
					for (i = 0; i < NT; i++)
					{
						busquedaBinaria(ord, n, objs[i]);
						suma += cmpB;
					}
					cmp = suma / NT;
				}
				reps = 20000000;
				tot = 0;
				for (tr = 0; tr < TRIALS; tr++)
				{
					suma = 0;
					t1 = ahora();
					for (r = 0; r < reps; r++)
					{
						if (c == 4) obj = objs[r % NT];
						suma += busquedaBinaria(ord, n, obj);
					}
					t2 = ahora();
					tot += (t2 - t1) / reps;
				}
				printf("binaria\t%d\t%s\t%ld\t%.2f\n", n, caso[c], cmp, tot / TRIALS);
				sink += suma;
			}
		}
		free(lin); free(ord); free(objs);
	}
	return 0;
}
