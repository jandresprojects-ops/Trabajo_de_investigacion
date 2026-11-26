import json
import time
import copy
from typing import List, Tuple, Dict, Any

# ==============================================================================
# 1. METADATOS TEÓRICOS EXTRAÍDOS DEL REPORTE TÉCNICO
# ==============================================================================
REPORTE_TEORICO = {
    "grupo": "Grupo Cuadrático",
    "descripcion": "Algoritmos iterativos in-place con complejidad O(n²) en caso promedio y peor caso.",
    "algoritmos": {
        "Bubble Sort": {
            "mejor_caso": "Ω(n)",
            "caso_promedio": "Θ(n²)",
            "peor_caso": "O(n²)",
            "memoria_auxiliar": "Θ(1)",
            "estable": True,
            "nota_ingenieria": "Descartar en producción frente a Insertion Sort; ejecuta hasta 3x más escrituras."
        },
        "Insertion Sort": {
            "mejor_caso": "Ω(n)",
            "caso_promedio": "Θ(n²)",
            "peor_caso": "O(n²)",
            "memoria_auxiliar": "Θ(1)",
            "estable": True,
            "nota_ingenieria": "Óptimo para n <= 64 o arreglos casi ordenados por alta localidad espacial en caché L1."
        }
    }
}

# ==============================================================================
# 2. IMPLEMENTACIÓN INSTRUMENTADA DE LOS ALGORITMOS (IN-PLACE)
# ==============================================================================
def bubble_sort(arr: List[int]) -> Tuple[List[int], int]:
    """
    Ordenamiento Burbuja optimizado con bandera booleana.
    Retorna el arreglo ordenado y el conteo exacto de comparaciones entre claves.
    """
    n = len(arr)
    comparaciones = 0
    for i in range(n - 1):
        intercambiado = False
        for j in range(n - i - 1):
            comparaciones += 1
            if arr[j] > arr[j + 1]:
                arr[j], arr[j + 1] = arr[j + 1], arr[j]
                intercambiado = True
        if not intercambiado:
            break
    return arr, comparaciones


def insertion_sort(arr: List[int]) -> Tuple[List[int], int]:
    """
    Ordenamiento por Inserción mediante desplazamientos secuenciales.
    Retorna el arreglo ordenado y el conteo exacto de comparaciones entre claves.
    """
    n = len(arr)
    comparaciones = 0
    for i in range(1, n):
        clave = arr[i]
        j = i - 1
        while j >= 0:
            comparaciones += 1
            if arr[j] > clave:
                arr[j + 1] = arr[j]
                j -= 1
            else:
                break
        arr[j + 1] = clave
    return arr, comparaciones


# ==============================================================================
# 3. MOTOR DE BENCHMARK Y EVALUACIÓN EXPERIMENTAL
# ==============================================================================
def ejecutar_benchmark(ruta_json: str, repeticiones_k: int = 5) -> None:
    # Cargar los arreglos desde datos_prueba.json
    with open(ruta_json, "r", encoding="utf-8") as f:
        datasets: Dict[str, List[int]] = json.load(f)

    algoritmos_fn = {
        "Bubble Sort": bubble_sort,
        "Insertion Sort": insertion_sort
    }

    print("=" * 95)
    print(f"EVALUACIÓN EXPERIMENTAL: {REPORTE_TEORICO['grupo'].upper()}")
    print(f"Descripción: {REPORTE_TEORICO['descripcion']}")
    print("=" * 95)

    # Mostrar ficha técnica teórica del reporte
    for nombre, info in REPORTE_TEORICO["algoritmos"].items():
        print(f"\n[{nombre}]")
        print(f"  • Cotas Asintóticas -> Mejor: {info['mejor_caso']} | Promedio: {info['caso_promedio']} | Peor: {info['peor_caso']}")
        print(f"  • Memoria Auxiliar  -> {info['memoria_auxiliar']} | Estable: {'Sí' if info['estable'] else 'No'}")
        print(f"  • Criterio Técnico  -> {info['nota_ingenieria']}")

    print("\n" + "=" * 95)
    print(f"{'Dataset':<18} | {'n':<6} | {'Algoritmo':<15} | {'Tiempo Prom (ms)':<18} | {'Comparaciones':<15}")
    print("-" * 95)

    # Evaluar cada conjunto de datos del JSON
    for nombre_dataset, datos_originales in datasets.items():
        n = len(datos_originales)
        
        for nombre_alg, funcion_sort in algoritmos_fn.items():
            tiempos_ms = []
            comparaciones_registradas = 0

            for _ in range(repeticiones_k):
                # Se entrega una copia idéntica en cada corrida para evitar efectos de estado previo
                copia_datos = datos_originales.copy()
                
                inicio = time.perf_counter()
                arreglo_ordenado, comparaciones = funcion_sort(copia_datos)
                fin = time.perf_counter()

                # Verificación de integridad del ordenamiento
                assert arreglo_ordenado == sorted(datos_originales), f"Error de ordenamiento en {nombre_alg}"

                tiempos_ms.append((fin - inicio) * 1000.0)
                comparaciones_registradas = comparaciones

            tiempo_promedio = sum(tiempos_ms) / repeticiones_k
            print(f"{nombre_dataset:<18} | {n:<6} | {nombre_alg:<15} | {tiempo_promedio:<18.4f} | {comparaciones_registradas:<15,}")
        print("-" * 95)


if __name__ == "__main__":
    # Asegúrate de que 'datos_prueba.json' esté en el mismo directorio que este script
    ejecutar_benchmark("datos_prueba.json", repeticiones_k=5)