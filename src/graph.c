/**
 * @file graph.c
 * @brief Implementación del TDA Grafo para el modelo de incompatibilidades diplomáticas.
 * @author Brayan Ceballos (bracebalDev)
 * @date 2020 (Original) - 2026 (Refactorización de Portafolio)
 */

#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Graph* graph_create(int totalDelegates) {
    if (totalDelegates <= 0) {
        return NULL;
    }

    Graph *graph = (Graph*)malloc(sizeof(Graph));
    if (graph == NULL) {
        return NULL;
    }

    graph->totalDelegates = totalDelegates;
    graph->totalIncompatibilities = 0;
    graph->maxDegree = 0;
    graph->isolatedCount = totalDelegates;

    /* Asignación de la matriz de adyacencia (n x n) */
    graph->adjacencyMatrix = (int**)malloc(totalDelegates * sizeof(int*));
    if (graph->adjacencyMatrix == NULL) {
        free(graph);
        return NULL;
    }

    for (int i = 0; i < totalDelegates; i++) {
        graph->adjacencyMatrix[i] = (int*)calloc(totalDelegates, sizeof(int));
        if (graph->adjacencyMatrix[i] == NULL) {
            /* Liberar filas asignadas previamente en caso de error */
            for (int j = 0; j < i; j++) {
                free(graph->adjacencyMatrix[j]);
            }
            free(graph->adjacencyMatrix);
            free(graph);
            return NULL;
        }
    }

    /* Asignación del arreglo de grados */
    graph->degrees = (int*)calloc(totalDelegates, sizeof(int));
    if (graph->degrees == NULL) {
        for (int i = 0; i < totalDelegates; i++) {
            free(graph->adjacencyMatrix[i]);
        }
        free(graph->adjacencyMatrix);
        free(graph);
        return NULL;
    }

    return graph;
}

void graph_destroy(Graph *graph) {
    if (graph == NULL) {
        return;
    }

    if (graph->adjacencyMatrix != NULL) {
        for (int i = 0; i < graph->totalDelegates; i++) {
            if (graph->adjacencyMatrix[i] != NULL) {
                free(graph->adjacencyMatrix[i]);
            }
        }
        free(graph->adjacencyMatrix);
        graph->adjacencyMatrix = NULL;
    }

    if (graph->degrees != NULL) {
        free(graph->degrees);
        graph->degrees = NULL;
    }

    free(graph);
}

bool graph_is_valid_delegate_id(const Graph *graph, int delegateId) {
    if (graph == NULL) {
        return false;
    }
    return (delegateId >= 1 && delegateId <= graph->totalDelegates);
}

bool graph_add_incompatibility(Graph *graph, int delegateA, int delegateB) {
    if (graph == NULL) {
        return false;
    }

    /* Validación de límites de identificadores */
    if (!graph_is_valid_delegate_id(graph, delegateA) || !graph_is_valid_delegate_id(graph, delegateB)) {
        return false;
    }

    /* Un delegado no puede ser incompatible consigo mismo (grafo simple sin lazos) */
    if (delegateA == delegateB) {
        return false;
    }

    int u = delegateA - 1;
    int v = delegateB - 1;

    /* Si la arista ya existía, evitar duplicación */
    if (graph->adjacencyMatrix[u][v] == 1) {
        return true;
    }

    /* Grafo no dirigido: arista simétrica */
    graph->adjacencyMatrix[u][v] = 1;
    graph->adjacencyMatrix[v][u] = 1;

    graph->degrees[u]++;
    graph->degrees[v]++;
    graph->totalIncompatibilities++;

    graph_compute_metrics(graph);
    return true;
}

bool graph_are_incompatible(const Graph *graph, int delegateA, int delegateB) {
    if (graph == NULL) {
        return false;
    }

    if (!graph_is_valid_delegate_id(graph, delegateA) || !graph_is_valid_delegate_id(graph, delegateB)) {
        return false;
    }

    int u = delegateA - 1;
    int v = delegateB - 1;

    return (graph->adjacencyMatrix[u][v] == 1);
}

int graph_get_delegate_degree(const Graph *graph, int delegateId) {
    if (graph == NULL || !graph_is_valid_delegate_id(graph, delegateId)) {
        return -1;
    }
    return graph->degrees[delegateId - 1];
}

void graph_compute_metrics(Graph *graph) {
    if (graph == NULL) {
        return;
    }

    int maxDeg = 0;
    int isolated = 0;

    for (int i = 0; i < graph->totalDelegates; i++) {
        int deg = graph->degrees[i];
        if (deg > maxDeg) {
            maxDeg = deg;
        }
        if (deg == 0) {
            isolated++;
        }
    }

    graph->maxDegree = maxDeg;
    graph->isolatedCount = isolated;
}
