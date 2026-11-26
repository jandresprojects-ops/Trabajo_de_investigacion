#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

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

void llenarArreglo(int *arr, int n)
{
	for (int i = 0; i < n; i++)
		arr[i] = i * 2;
}

FILE *abrirJSON()
{
	const char *rutas[] = {
		"../Data/busqueda_binaria.json",
		"../Data/datos_busqueda_lineal_O2.json",
		"../Data/datos_busqueda_lineal_O2_2.json",
		"Data/busqueda_binaria.json",
		"Data/datos_busqueda_lineal_O2.json",
		"Data/datos_busqueda_lineal_O2_2.json",
		"../busqueda_binaria.json",
		"../datos_busqueda_lineal_O2.json",
		"../datos_busqueda_lineal_O2_2.json",
		"busqueda_binaria.json",
		"datos_busqueda_lineal_O2.json",
		"datos_busqueda_lineal_O2_2.json"};

	for (int i = 0; i < 12; i++)
	{
		FILE *f = fopen(rutas[i], "r");
		if (f)
		{
			printf("Leyendo archivo: %s\n\n", rutas[i]);
			return f;
		}
	}
	return NULL;
}

int main()
{
	FILE *f = abrirJSON();
	if (!f)
	{
		printf("ERROR: No se encontro el JSON. Asegurate de guardar el contenido en Data/busqueda_binaria.json\n");
		return 1;
	}

	char linea[256];
	char alg_actual[32] = "";
	char caso_actual[32] = "";
	int n_actual = 0, n_anterior = -1;

	int *arr = NULL;
	int *objetivos_rand = (int *)malloc(REPS * sizeof(int));
	int r, objetivo, pos;
	long comp, comp_total;
	volatile long suma = 0;

	LARGE_INTEGER freq, t1, t2;
	QueryPerformanceFrequency(&freq);
	srand(42);

	printf("%-10s %-12s %-10s %-15s %s\n", "n", "Caso", "Pos", "Comparaciones", "Tiempo(ns)");
	printf("------------------------------------------------------------------\n");

	while (fgets(linea, sizeof(linea), f))
	{
		char *p;

		if ((p = strstr(linea, "\"alg\":")))
		{
			sscanf(p, "\"alg\": \"%31[^\"]\"", alg_actual);
		}
		else if ((p = strstr(linea, "\"n\":")))
		{
			sscanf(p, "\"n\": %d", &n_actual);
		}
		else if ((p = strstr(linea, "\"caso\":")))
		{
			sscanf(p, "\"caso\": \"%31[^\"]\"", caso_actual);

			if (strcmp(alg_actual, "binaria") == 0)
			{
				if (n_actual != n_anterior)
				{
					free(arr);
					arr = (int *)malloc(n_actual * sizeof(int));
					llenarArreglo(arr, n_actual);

					for (r = 0; r < REPS; r++)
					{
						int idx = ((rand() << 15) ^ rand()) % n_actual;
						objetivos_rand[r] = arr[idx];
					}
					n_anterior = n_actual;
				}

				int es_promedio = (strcmp(caso_actual, "promedio") == 0);
				if (strcmp(caso_actual, "inicio") == 0)
					objetivo = arr[0];
				else if (strcmp(caso_actual, "centro") == 0)
					objetivo = arr[(n_actual - 1) / 2];
				else if (strcmp(caso_actual, "final") == 0)
					objetivo = arr[n_actual - 1];
				else
					objetivo = -1;

				comp_total = 0;

				QueryPerformanceCounter(&t1);
				for (r = 0; r < REPS; r++)
				{
					int target = es_promedio ? objetivos_rand[r] : objetivo;
					pos = busquedaBinaria(arr, n_actual, target, &comp);
					comp_total += comp;
					suma += pos;
				}
				QueryPerformanceCounter(&t2);

				double ns = ((double)(t2.QuadPart - t1.QuadPart) * 1e9) / (freq.QuadPart * REPS);
				long comp_promedio = comp_total / REPS;

				printf("%-10d %-12s %-10d %-15ld %.2f\n",
					   n_actual,
					   caso_actual,
					   es_promedio ? 0 : pos,
					   comp_promedio,
					   ns);
			}
		}
	}

	free(arr);
	free(objetivos_rand);
	fclose(f);

	return 0;
}