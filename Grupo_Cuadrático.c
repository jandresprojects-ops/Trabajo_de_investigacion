#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

typedef struct
{
    long long comparaciones;
    long long escrituras;
    double tiempo_ms;
    bool ordenado_ok;
} Resultado;

// Ordenamiento Burbuja Optimizado (In-Place)
void bubble_sort(int *arr, int n, long long *comparaciones, long long *escrituras)
{
    *comparaciones = 0;
    *escrituras = 0;
    for (int i = 0; i < n - 1; i++)
    {
        bool intercambio = false;
        for (int j = 0; j < n - 1 - i; j++)
        {
            (*comparaciones)++;
            if (arr[j] > arr[j + 1])
            {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                *escrituras += 2;
                intercambio = true;
            }
        }
        if (!intercambio)
            break;
    }
}

// Ordenamiento por Inserción (In-Place)
void insertion_sort(int *arr, int n, long long *comparaciones, long long *escrituras)
{
    *comparaciones = 0;
    *escrituras = 0;
    for (int i = 1; i < n; i++)
    {
        int clave = arr[i];
        int j = i - 1;
        while (j >= 0)
        {
            (*comparaciones)++;
            if (arr[j] > clave)
            {
                arr[j + 1] = arr[j];
                (*escrituras)++;
                j--;
            }
            else
            {
                break;
            }
        }
        arr[j + 1] = clave;
        (*escrituras)++;
    }
}

bool verificar_ordenado(const int *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        if (arr[i] > arr[i + 1])
            return false;
    }
    return true;
}

// Temporizador nativo de alta precisión en Windows (milisegundos)
double obtener_tiempo_ms()
{
    LARGE_INTEGER frecuencia, contador;
    QueryPerformanceFrequency(&frecuencia);
    QueryPerformanceCounter(&contador);
    return (double)(contador.QuadPart * 1000.0) / (double)frecuencia.QuadPart;
}

Resultado medir_algoritmo(void (*algoritmo)(int *, int, long long *, long long *), const int *original, int n, int repeticiones_k)
{
    Resultado res = {0, 0, 0.0, true};
    int *copia = (int *)malloc(n * sizeof(int));
    double suma_tiempos = 0.0;

    for (int k = 0; k < repeticiones_k; k++)
    {
        memcpy(copia, original, n * sizeof(int));
        long long comp = 0, escr = 0;

        double inicio = obtener_tiempo_ms();
        algoritmo(copia, n, &comp, &escr);
        double fin = obtener_tiempo_ms();

        suma_tiempos += (fin - inicio);
        if (k == 0)
        {
            res.comparaciones = comp;
            res.escrituras = escr;
            res.ordenado_ok = verificar_ordenado(copia, n);
        }
    }

    res.tiempo_ms = suma_tiempos / repeticiones_k;
    free(copia);
    return res;
}

// Parser sencillo para extraer los arreglos enteros de datos_cuadratica.json
void procesar_json(const char *ruta_archivo)
{
    FILE *f = fopen(ruta_archivo, "rb");
    if (!f)
    {
        printf("Error: No se pudo abrir '%s'. Verifica la ruta.\n", ruta_archivo);
        return;
    }

    fseek(f, 0, SEEK_END);
    long tam = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *contenido = (char *)malloc(tam + 1);
    fread(contenido, 1, tam, f);
    contenido[tam] = '\0';
    fclose(f);

    printf("%-16s | %-6s | %-15s | %-13s | %-13s | %-12s | %s\n",
           "Dataset", "N", "Algoritmo", "Tiempo (ms)", "Comparaciones", "Escrituras", "Estado");
    printf("-----------------------------------------------------------------------------------------------------\n");

    char *cursor = contenido;
    int *buffer = (int *)malloc(20000 * sizeof(int));

    while ((cursor = strchr(cursor, '"')) != NULL)
    {
        char *fin_clave = strchr(cursor + 1, '"');
        if (!fin_clave)
            break;

        int len_clave = fin_clave - (cursor + 1);
        char nombre_dataset[64] = {0};
        if (len_clave < 63)
        {
            strncpy(nombre_dataset, cursor + 1, len_clave);
        }

        char *inicio_arr = strchr(fin_clave, '[');
        if (!inicio_arr)
            break;
        char *fin_arr = strchr(inicio_arr, ']');
        if (!fin_arr)
            break;

        int n = 0;
        char *p = inicio_arr + 1;
        while (p < fin_arr)
        {
            while (p < fin_arr && (*p == ' ' || *p == ',' || *p == '\n' || *p == '\r' || *p == '\t'))
            {
                p++;
            }
            if (p < fin_arr && ((*p >= '0' && *p <= '9') || *p == '-'))
            {
                buffer[n++] = (int)strtol(p, &p, 10);
            }
            else
            {
                p++;
            }
        }

        if (n > 0)
        {
            Resultado r_bubble = medir_algoritmo(bubble_sort, buffer, n, 5);
            printf("%-16s | %-6d | %-15s | %11.4f ms | %13lld | %12lld | %s\n",
                   nombre_dataset, n, "Bubble Sort", r_bubble.tiempo_ms,
                   r_bubble.comparaciones, r_bubble.escrituras, r_bubble.ordenado_ok ? "OK" : "ERROR");

            Resultado r_insert = medir_algoritmo(insertion_sort, buffer, n, 5);
            printf("%-16s | %-6d | %-15s | %11.4f ms | %13lld | %12lld | %s\n",
                   nombre_dataset, n, "Insertion Sort", r_insert.tiempo_ms,
                   r_insert.comparaciones, r_insert.escrituras, r_insert.ordenado_ok ? "OK" : "ERROR");
            printf("-----------------------------------------------------------------------------------------------------\n");
        }

        cursor = fin_arr + 1;
    }

    free(buffer);
    free(contenido);
}

int main()
{
    FILE *test = fopen("Data/datos_cuadratica.json", "rb");
    if (test)
    {
        fclose(test);
        procesar_json("Data/datos_cuadratica.json");
    }
    else
    {
        procesar_json("../Data/datos_cuadratica.json");
    }
    return 0;
}