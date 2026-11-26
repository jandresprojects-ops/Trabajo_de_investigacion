#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;
using namespace chrono;

mt19937 generador(12345);

struct Medidas {
    long long comparaciones = 0;
    long long swaps = 0;
};

static inline void swapCont(vector<int>& a, int i, int j, Medidas& m) {
    if (i == j) return;
    swap(a[i], a[j]);
    m.swaps++;
}

// QUICK SORT: pivote aleatorio + Lomuto
int particionQuick(vector<int>& a, int inicio, int fin, Medidas& m) {
    uniform_int_distribution<int> dist(inicio, fin);
    int indicePivote = dist(generador);
    swapCont(a, indicePivote, fin, m);

    int pivote = a[fin];
    int i = inicio;

    for (int j = inicio; j < fin; j++) {
        m.comparaciones++;
        if (a[j] < pivote) {
            swapCont(a, i, j, m);
            i++;
        }
    }
    swapCont(a, i, fin, m);
    return i;
}

void quickSort(vector<int>& a, int inicio, int fin, Medidas& m) {
    if (inicio < fin) {
        int p = particionQuick(a, inicio, fin, m);
        quickSort(a, inicio, p - 1, m);
        quickSort(a, p + 1, fin, m);
    }
}

// MERGE SORT
void combinar(vector<int>& a, vector<int>& aux,
              int inicio, int medio, int fin, Medidas& m) {
    int i = inicio;
    int j = medio + 1;
    int k = inicio;

    while (i <= medio && j <= fin) {
        m.comparaciones++;
        if (a[i] <= a[j]) aux[k++] = a[i++];
        else aux[k++] = a[j++];
    }
    while (i <= medio) aux[k++] = a[i++];
    while (j <= fin) aux[k++] = a[j++];

    for (int x = inicio; x <= fin; x++) a[x] = aux[x];
}

void mergeSort(vector<int>& a, vector<int>& aux,
               int inicio, int fin, Medidas& m) {
    if (inicio >= fin) return;
    int medio = inicio + (fin - inicio) / 2;
    mergeSort(a, aux, inicio, medio, m);
    mergeSort(a, aux, medio + 1, fin, m);
    combinar(a, aux, inicio, medio, fin, m);
}

// HEAP SORT (max-heap)
void ajustarHeap(vector<int>& a, int n, int raiz, Medidas& m) {
    int mayor = raiz;
    int izquierdo = 2 * raiz + 1;
    int derecho = 2 * raiz + 2;

    if (izquierdo < n) {
        m.comparaciones++;
        if (a[izquierdo] > a[mayor]) mayor = izquierdo;
    }
    if (derecho < n) {
        m.comparaciones++;
        if (a[derecho] > a[mayor]) mayor = derecho;
    }

    if (mayor != raiz) {
        swapCont(a, raiz, mayor, m);
        ajustarHeap(a, n, mayor, m);
    }
}

void heapSort(vector<int>& a, Medidas& m) {
    int n = (int)a.size();

    for (int i = n / 2 - 1; i >= 0; i--) ajustarHeap(a, n, i, m);
    for (int i = n - 1; i > 0; i--) {
        swapCont(a, 0, i, m);
        ajustarHeap(a, i, 0, m);
    }
}

// INSERTION SORT
void insertionSort(vector<int>& a, Medidas& m) {
    for (int i = 1; i < (int)a.size(); i++) {
        int actual = a[i];
        int j = i - 1;

        while (j >= 0) {
            m.comparaciones++;
            if (a[j] > actual) {
                a[j + 1] = a[j];
                m.swaps++;
                j--;
            } else {
                break;
            }
        }
        a[j + 1] = actual;
    }
}

vector<int> generarDatos(int n, const string& tipo) {
    vector<int> datos(n);
    for (int i = 0; i < n; i++) datos[i] = i + 1;

    if (tipo == "Aleatorios") {
        shuffle(datos.begin(), datos.end(), generador);
    } else if (tipo == "Inversos") {
        reverse(datos.begin(), datos.end());
    }
    return datos;
}

int main() {
    const vector<int> tamanos = {100, 500, 1000, 3000, 5000};
    const vector<string> tipos = {"Aleatorios", "Ordenados", "Inversos"};
    const int repeticiones = 5;

    filesystem::create_directories("resultados");

    ofstream archivo("resultados/resultados.csv");
    if (!archivo.is_open()) {
        cerr << "No se pudo crear resultados/resultados.csv\n";
        return 1;
    }

    archivo << "Tipo,N,Algoritmo,Tiempo_promedio_ms,"
               "Comparaciones_promedio,Swaps_movimientos_promedio\n";

    const string nombres[] = {"Quick Sort", "Merge Sort", "Heap Sort", "Insertion Sort"};

    for (const string& tipo : tipos) {
        for (int n : tamanos) {
            for (int algoritmo = 0; algoritmo < 4; algoritmo++) {

                double tiempoTotal = 0.0;
                Medidas totales;

                vector<int> aux;
                if (algoritmo == 1) aux.resize(n);

                for (int rep = 0; rep < repeticiones; rep++) {
                    vector<int> datos = generarDatos(n, tipo);
                    Medidas m;

                    auto inicio = steady_clock::now();

                    switch (algoritmo) {
                        case 0:
                            quickSort(datos, 0, n - 1, m);
                            break;
                        case 1:
                            mergeSort(datos, aux, 0, n - 1, m);
                            break;
                        case 2:
                            heapSort(datos, m);
                            break;
                        case 3:
                            insertionSort(datos, m);
                            break;
                    }

                    auto fin = steady_clock::now();

                    double milisegundos =
                        duration<double, milli>(fin - inicio).count();

                    if (!is_sorted(datos.begin(), datos.end())) {
                        cerr << "Error: un algoritmo no ordeno correctamente.\n";
                        return 1;
                    }

                    tiempoTotal += milisegundos;
                    totales.comparaciones += m.comparaciones;
                    totales.swaps += m.swaps;
                }

                double tiempoPromedio = tiempoTotal / repeticiones;
                double comparacionesPromedio =
                    (double)totales.comparaciones / repeticiones;
                double swapsPromedio = (double)totales.swaps / repeticiones;

                archivo << tipo << "," << n << "," << nombres[algoritmo] << ","
                        << fixed << setprecision(6) << tiempoPromedio << ","
                        << setprecision(2) << comparacionesPromedio << ","
                        << setprecision(2) << swapsPromedio << "\n";

                cout << tipo << " | N=" << n
                     << " | " << nombres[algoritmo]
                     << " | Tiempo: " << fixed << setprecision(6) << tiempoPromedio << " ms"
                     << " | Comparaciones: " << setprecision(2) << comparacionesPromedio
                     << " | Mov/Swaps: " << setprecision(2) << swapsPromedio << "\n";
            }
        }
    }

    archivo.close();
    cout << "\nPruebas terminadas. Archivo: resultados/resultados.csv\n";
    return 0;
}