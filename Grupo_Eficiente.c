#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

#define MAX 100

typedef struct
{
    long comparaciones;
    long escrituras;
    double tiempo_ms;
    bool ordenado_ok;
} Resultado;

// ==========================================
// 1. MERGE SORT
// ==========================================
void mezclar(int a[], int inicio, int medio, int fin, long *comparaciones, long *escrituras)
{
    int temp[MAX];
    int i = inicio, j = medio + 1, k = inicio;

    while (i <= medio && j <= fin)
    {
        (*comparaciones)++;
        if (a[i] <= a[j])
        {
            temp[k++] = a[i++];
        }
        else
        {
            temp[k++] = a[j++];
        }
        (*escrituras)++;
    }

    while (i <= medio)
    {
        temp[k++] = a[i++];
        (*escrituras)++;
    }

    while (j <= fin)
    {
        temp[k++] = a[j++];
        (*escrituras)++;
    }

    for (i = inicio; i <= fin; i++)
    {
        a[i] = temp[i];
        (*escrituras)++;
    }
}

void mergeSortRec(int a[], int inicio, int fin, long *comparaciones, long *escrituras)
{
    if (inicio < fin)
    {
        int medio = (inicio + fin) / 2;
        mergeSortRec(a, inicio, medio, comparaciones, escrituras);
        mergeSortRec(a, medio + 1, fin, comparaciones, escrituras);
        mezclar(a, inicio, medio, fin, comparaciones, escrituras);
    }
}

void ejecutarMergeSort(int a[], int n, long *comparaciones, long *escrituras)
{
    *comparaciones = 0;
    *escrituras = 0;
    if (n > 1)
    {
        mergeSortRec(a, 0, n - 1, comparaciones, escrituras);
    }
}

// ==========================================
// 2. QUICK SORT
// ==========================================
int particionar(int a[], int bajo, int alto, long *comparaciones, long *escrituras)
{
    int pivote = a[alto];
    int i = bajo - 1;

    for (int j = bajo; j < alto; j++)
    {
        (*comparaciones)++;
        if (a[j] <= pivote)
        {
            i++;
            if (i != j)
            {
                int temp = a[i];
                a[i] = a[j];
                a[j] = temp;
                *escrituras += 2;
            }
        }
    }

    if (i + 1 != alto)
    {
        int temp = a[i + 1];
        a[i + 1] = a[alto];
        a[alto] = temp;
        *escrituras += 2;
    }

    return i + 1;
}

void quickSortRec(int a[], int bajo, int alto, long *comparaciones, long *escrituras)
{
    if (bajo < alto)
    {
        int p = particionar(a, bajo, alto, comparaciones, escrituras);
        quickSortRec(a, bajo, p - 1, comparaciones, escrituras);
        quickSortRec(a, p + 1, alto, comparaciones, escrituras);
    }
}

void ejecutarQuickSort(int a[], int n, long *comparaciones, long *escrituras)
{
    *comparaciones = 0;
    *escrituras = 0;
    if (n > 1)
    {
        quickSortRec(a, 0, n - 1, comparaciones, escrituras);
    }
}

// ==========================================
// 3. HEAP SORT
// ==========================================
void heapify(int a[], int n, int i, long *comparaciones, long *escrituras)
{
    int mayor = i;
    int izquierda = 2 * i + 1;
    int derecha = 2 * i + 2;

    if (izquierda < n)
    {
        (*comparaciones)++;
        if (a[izquierda] > a[mayor])
            mayor = izquierda;
    }

    if (derecha < n)
    {
        (*comparaciones)++;
        if (a[derecha] > a[mayor])
            mayor = derecha;
    }

    if (mayor != i)
    {
        int temp = a[i];
        a[i] = a[mayor];
        a[mayor] = temp;
        *escrituras += 2;

        heapify(a, n, mayor, comparaciones, escrituras);
    }
}

void ejecutarHeapSort(int a[], int n, long *comparaciones, long *escrituras)
{
    *comparaciones = 0;
    *escrituras = 0;
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(a, n, i, comparaciones, escrituras);

    for (int i = n - 1; i > 0; i--)
    {
        int temp = a[0];
        a[0] = a[i];
        a[i] = temp;
        *escrituras += 2;

        heapify(a, i, 0, comparaciones, escrituras);
    }
}

// ==========================================
// BENCHMARK Y EJECUCIÓN POR DATASET
// ==========================================
bool verificar_ordenado(const int *arr, int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        if (arr[i] > arr[i + 1])
            return false;
    }
    return true;
}

double obtener_tiempo_ms()
{
    LARGE_INTEGER frecuencia, contador;
    QueryPerformanceFrequency(&frecuencia);
    QueryPerformanceCounter(&contador);
    return (double)(contador.QuadPart * 1000.0) / (double)frecuencia.QuadPart;
}

Resultado medir_algoritmo(void (*algoritmo)(int *, int, long *, long *), const int *original, int n, int repeticiones_k)
{
    Resultado res = {0, 0, 0.0, true};
    int copia[MAX];
    double suma_tiempos = 0.0;

    for (int k = 0; k < repeticiones_k; k++)
    {
        memcpy(copia, original, n * sizeof(int));
        long comp = 0, escr = 0;

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
    return res;
}

void evaluar_dataset(const char *nombre_dataset, const int *datos, int n)
{
    Resultado r_merge = medir_algoritmo(ejecutarMergeSort, datos, n, 100);
    printf("%-16s | %-5d | %-13s | %10.5f ms | %13ld | %11ld | %s\n",
           nombre_dataset, n, "Merge Sort", r_merge.tiempo_ms,
           r_merge.comparaciones, r_merge.escrituras, r_merge.ordenado_ok ? "OK" : "ERROR");

    Resultado r_quick = medir_algoritmo(ejecutarQuickSort, datos, n, 100);
    printf("%-16s | %-5d | %-13s | %10.5f ms | %13ld | %11ld | %s\n",
           nombre_dataset, n, "Quick Sort", r_quick.tiempo_ms,
           r_quick.comparaciones, r_quick.escrituras, r_quick.ordenado_ok ? "OK" : "ERROR");

    Resultado r_heap = medir_algoritmo(ejecutarHeapSort, datos, n, 100);
    printf("%-16s | %-5d | %-13s | %10.5f ms | %13ld | %11ld | %s\n",
           nombre_dataset, n, "Heap Sort", r_heap.tiempo_ms,
           r_heap.comparaciones, r_heap.escrituras, r_heap.ordenado_ok ? "OK" : "ERROR");

    printf("-------------------------------------------------------------------------------------------------\n");
}

int procesar_json(const char *ruta_archivo)
{
    FILE *f = fopen(ruta_archivo, "rb");
    if (!f)
        return 0;

    fseek(f, 0, SEEK_END);
    long tam = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (tam <= 0)
    {
        fclose(f);
        return 0;
    }

    char *contenido = (char *)malloc(tam + 1);
    if (!contenido)
    {
        fclose(f);
        return 0;
    }
    fread(contenido, 1, tam, f);
    contenido[tam] = '\0';
    fclose(f);

    char *cursor = contenido;
    int buffer[MAX];
    int datasets_leidos = 0;

    while ((cursor = strchr(cursor, '"')) != NULL)
    {
        char *fin_clave = strchr(cursor + 1, '"');
        if (!fin_clave)
            break;

        int len_clave = (int)(fin_clave - (cursor + 1));
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
        while (p < fin_arr && n < MAX)
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
            evaluar_dataset(nombre_dataset, buffer, n);
            datasets_leidos++;
        }

        cursor = fin_arr + 1;
    }

    free(contenido);
    return datasets_leidos;
}

void ejecutar_respaldo_integrado()
{
    int ord20[20], inv20[20], ord100[100], inv100[100], ale100[100];
    int ale20[20] = {42, 7, 91, 15, 63, 28, 84, 3, 50, 19, 76, 11, 95, 34, 68, 2, 57, 89, 23, 47};
    int dup20[20] = {5, 2, 8, 5, 1, 9, 2, 8, 5, 1, 3, 7, 3, 9, 2, 5, 8, 1, 7, 3};

    for (int i = 0; i < 20; i++)
    {
        ord20[i] = i + 1;
        inv20[i] = 20 - i;
    }
    for (int i = 0; i < 100; i++)
    {
        ord100[i] = i + 1;
        inv100[i] = 100 - i;
        ale100[i] = ((i * 73 + 19) % 100) + 1;
    }

    evaluar_dataset("ordenado_20", ord20, 20);
    evaluar_dataset("inverso_20", inv20, 20);
    evaluar_dataset("aleatorio_20", ale20, 20);
    evaluar_dataset("duplicados_20", dup20, 20);
    evaluar_dataset("ordenado_100", ord100, 100);
    evaluar_dataset("inverso_100", inv100, 100);
    evaluar_dataset("aleatorio_100", ale100, 100);
}

int main()
{
    printf("%-16s | %-5s | %-13s | %-13s | %-13s | %-11s | %s\n",
           "Dataset", "N", "Algoritmo", "Tiempo (ms)", "Comparaciones", "Escrituras", "Estado");
    printf("-------------------------------------------------------------------------------------------------\n");

    const char *rutas[] = {
        "Data/datos_eficiente.json",
        "../Data/datos_eficiente.json",
        "datos_eficiente.json"};

    int total_leidos = 0;
    for (int i = 0; i < 3; i++)
    {
        total_leidos = procesar_json(rutas[i]);
        if (total_leidos > 0)
            break;
    }

    if (total_leidos == 0)
    {
        ejecutar_respaldo_integrado();
    }

    fflush(stdout);
    return 0;
}