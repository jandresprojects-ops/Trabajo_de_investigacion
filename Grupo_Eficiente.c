#include <stdio.h>

#define MAX 1000

// MERGE SORT
void mezclar(int a[], int inicio, int medio, int fin) {
    int temp[MAX];
    int i = inicio, j = medio + 1, k = inicio;

    while (i <= medio && j <= fin) {
        if (a[i] <= a[j])
            temp[k++] = a[i++];
        else
            temp[k++] = a[j++];
    }

    while (i <= medio)
        temp[k++] = a[i++];

    while (j <= fin)
        temp[k++] = a[j++];

    for (i = inicio; i <= fin; i++)
        a[i] = temp[i];
}

void mergeSort(int a[], int inicio, int fin) {
    if (inicio < fin) {
        int medio = (inicio + fin) / 2;

        mergeSort(a, inicio, medio);
        mergeSort(a, medio + 1, fin);
        mezclar(a, inicio, medio, fin);
    }
}

// QUICK SORT
int particionar(int a[], int bajo, int alto) {
    int pivote = a[alto];
    int i = bajo - 1;

    for (int j = bajo; j < alto; j++) {
        if (a[j] <= pivote) {
            i++;
            int temp = a[i];
            a[i] = a[j];
            a[j] = temp;
        }
    }

    int temp = a[i + 1];
    a[i + 1] = a[alto];
    a[alto] = temp;

    return i + 1;
}

void quickSort(int a[], int bajo, int alto) {
    if (bajo < alto) {
        int p = particionar(a, bajo, alto);

        quickSort(a, bajo, p - 1);
        quickSort(a, p + 1, alto);
    }
}

// HEAP SORT
void heapify(int a[], int n, int i) {
    int mayor = i;
    int izquierda = 2 * i + 1;
    int derecha = 2 * i + 2;

    if (izquierda < n && a[izquierda] > a[mayor])
        mayor = izquierda;

    if (derecha < n && a[derecha] > a[mayor])
        mayor = derecha;

    if (mayor != i) {
        int temp = a[i];
        a[i] = a[mayor];
        a[mayor] = temp;

        heapify(a, n, mayor);
    }
}

void heapSort(int a[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(a, n, i);

    for (int i = n - 1; i > 0; i--) {
        int temp = a[0];
        a[0] = a[i];
        a[i] = temp;

        heapify(a, i, 0);
    }
}

// MOSTRAR ARREGLO
void mostrar(int a[], int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", a[i]);

    printf("\n");
}

// PROGRAMA PRINCIPAL
int main() {
    int numeros[MAX];
    int n, opcion;

    printf("=================================\n");
    printf("   ALGORITMOS DE ORDENAMIENTO\n");
    printf("=================================\n");

    printf("Cuantos numeros deseas ingresar? ");
    scanf("%d", &n);

    if (n < 1 || n > MAX) {
        printf("Cantidad no valida.\n");
        return 1;
    }

    printf("\nIngresa los numeros:\n");

    for (int i = 0; i < n; i++) {
        scanf("%d", &numeros[i]);
    }

    printf("\nArreglo original:\n");
    mostrar(numeros, n);

    printf("\nSelecciona un algoritmo:\n");
    printf("1. Merge Sort\n");
    printf("2. Quick Sort\n");
    printf("3. Heap Sort\n");
    printf("Opcion: ");
    scanf("%d", &opcion);

    switch (opcion) {
        case 1:
            mergeSort(numeros, 0, n - 1);
            printf("\nMetodo utilizado: Merge Sort\n");
            break;

        case 2:
            quickSort(numeros, 0, n - 1);
            printf("\nMetodo utilizado: Quick Sort\n");
            break;

        case 3:
            heapSort(numeros, n);
            printf("\nMetodo utilizado: Heap Sort\n");
            break;

        default:
            printf("Opcion no valida.\n");
            return 1;
    }

    printf("\nArreglo ordenado:\n");
    mostrar(numeros, n);

    printf("\nOrdenamiento terminado correctamente.\n");

    return 0;
}