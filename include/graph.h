/**
 * @file graph.h
 * @brief Definición del Tipo de Dato Abstracto (TDA) Grafo para modelado de incompatibilidades diplomáticas.
 * @author Brayan Ceballos (bracebalDev)
 * @date 2020 (Original) - 2026 (Refactorización de Portafolio)
 * 
 * Este módulo provee las estructuras y funciones fundamentales para representar y manipular
 * un grafo no dirigido simple G = (V, E), donde los vértices representan a los delegados
 * de la convención internacional y las aristas representan las incompatibilidades diplomáticas.
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @struct Graph
 * @brief Estructura que encapsula el grafo de incompatibilidades diplomáticas.
 * 
 * Utiliza una matriz de adyacencia dinámica de tamaño (n x n) para responder consultas
 * de incompatibilidad en tiempo O(1), junto con un arreglo de grados para optimizar
 * los cálculos de conflictividad y detección de vértices aislados.
 */
typedef struct {
    int totalDelegates;             /**< Número total de vértices |V| (delegados, 1..n) */
    int totalIncompatibilities;     /**< Número total de aristas |E| (incompatibilidades) */
    int **adjacencyMatrix;          /**< Matriz de adyacencia simétrica (0 = compatible, 1 = incompatible) */
    int *degrees;                   /**< Arreglo de grados de cada vértice (índice 0..n-1) */
    int maxDegree;                  /**< Grado máximo del grafo Delta(G) */
    int isolatedCount;              /**< Cantidad de vértices aislados (grado 0) */
} Graph;

/**
 * @brief Crea e inicializa una nueva instancia de Grafo en memoria dinámica.
 * 
 * @param totalDelegates Cantidad total de delegados (vértices).
 * @return Graph* Puntero a la estructura de grafo creada, o NULL en caso de fallo de asignación.
 */
Graph* graph_create(int totalDelegates);

/**
 * @brief Libera toda la memoria dinámica reservada por el grafo.
 * 
 * @param graph Puntero al grafo a destruir.
 */
void graph_destroy(Graph *graph);

/**
 * @brief Agrega una arista de incompatibilidad no dirigida entre dos delegados.
 * 
 * @param graph Puntero al grafo.
 * @param delegateA Identificador del primer delegado (1..n).
 * @param delegateB Identificador del segundo delegado (1..n).
 * @return true Si la arista fue agregada con éxito, false si los índices son inválidos o redundantes.
 */
bool graph_add_incompatibility(Graph *graph, int delegateA, int delegateB);

/**
 * @brief Verifica si dos delegados son incompatibles entre sí en tiempo O(1).
 * 
 * @param graph Puntero al grafo.
 * @param delegateA Identificador del primer delegado (1..n).
 * @param delegateB Identificador del segundo delegado (1..n).
 * @return true Si existe una arista entre ambos delegados, false en caso contrario o entrada inválida.
 */
bool graph_are_incompatible(const Graph *graph, int delegateA, int delegateB);

/**
 * @brief Obtiene el grado de conflictividad (número de incompatibilidades) de un delegado.
 * 
 * @param graph Puntero al grafo.
 * @param delegateId Identificador del delegado (1..n).
 * @return int Grado del vértice, o -1 si el identificador es inválido.
 */
int graph_get_delegate_degree(const Graph *graph, int delegateId);

/**
 * @brief Actualiza y sincroniza las métricas de grado máximo y conteo de aislados.
 * 
 * @param graph Puntero al grafo.
 */
void graph_compute_metrics(Graph *graph);

/**
 * @brief Valida si el identificador de un delegado se encuentra dentro del rango válido [1, n].
 * 
 * @param graph Puntero al grafo.
 * @param delegateId Identificador del delegado.
 * @return true Si 1 <= delegateId <= n, false en caso contrario.
 */
bool graph_is_valid_delegate_id(const Graph *graph, int delegateId);

#endif /* GRAPH_H */
