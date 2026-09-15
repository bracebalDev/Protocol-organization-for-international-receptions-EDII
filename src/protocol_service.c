/**
 * @file protocol_service.c
 * @brief Implementación de las reglas de negocio y algoritmos de Teoría de Grafos para la ONU DAGGC.
 * @author Brayan Ceballos (bracebalDev)
 * @date 2020 (Original) - 2026 (Refactorización de Portafolio)
 */

#include "protocol_service.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool protocol_check_compatibility(const Graph *graph, int delegateA, int delegateB, bool *outAreIncompatible) {
    if (graph == NULL || outAreIncompatible == NULL) {
        return false;
    }

    if (!graph_is_valid_delegate_id(graph, delegateA) || !graph_is_valid_delegate_id(graph, delegateB)) {
        return false;
    }

    if (delegateA == delegateB) {
        /* Un delegado consigo mismo no tiene incompatibilidad diplomática */
        *outAreIncompatible = false;
        return true;
    }

    *outAreIncompatible = graph_are_incompatible(graph, delegateA, delegateB);
    return true;
}

bool protocol_find_most_conflictive(const Graph *graph, ConflictiveDelegatesResult *outResult) {
    if (graph == NULL || outResult == NULL) {
        return false;
    }

    int maxDeg = graph->maxDegree;
    int matchCount = 0;

    /* Primer pase: contar cuántos delegados tienen el grado máximo */
    for (int i = 0; i < graph->totalDelegates; i++) {
        if (graph->degrees[i] == maxDeg) {
            matchCount++;
        }
    }

    outResult->maxIncompatibilities = maxDeg;
    outResult->count = matchCount;
    outResult->delegateIds = NULL;

    if (matchCount > 0) {
        outResult->delegateIds = (int*)malloc(matchCount * sizeof(int));
        if (outResult->delegateIds == NULL) {
            return false;
        }

        int index = 0;
        for (int i = 0; i < graph->totalDelegates; i++) {
            if (graph->degrees[i] == maxDeg) {
                outResult->delegateIds[index++] = i + 1; /* Identificador 1-indexado */
            }
        }
    }

    return true;
}

bool protocol_find_isolated_delegates(const Graph *graph, IsolatedDelegatesResult *outResult) {
    if (graph == NULL || outResult == NULL) {
        return false;
    }

    int count = graph->isolatedCount;
    outResult->count = count;
    outResult->delegateIds = NULL;

    if (count > 0) {
        outResult->delegateIds = (int*)malloc(count * sizeof(int));
        if (outResult->delegateIds == NULL) {
            return false;
        }

        int index = 0;
        for (int i = 0; i < graph->totalDelegates; i++) {
            if (graph->degrees[i] == 0) {
                outResult->delegateIds[index++] = i + 1;
            }
        }
    }

    return true;
}

/**
 * @brief Función auxiliar para verificar si un delegado puede ser agregado a una mesa sin conflicto.
 */
static bool can_add_to_table(const Graph *graph, const int *tableDelegates, int currentCount, int candidateDelegate) {
    int candIdx = candidateDelegate - 1;
    for (int i = 0; i < currentCount; i++) {
        int existingIdx = tableDelegates[i] - 1;
        if (graph->adjacencyMatrix[candIdx][existingIdx] == 1) {
            return false;
        }
    }
    return true;
}

bool protocol_generate_table_distribution(const Graph *graph, ProtocolDistributionResult *outResult) {
    if (graph == NULL || outResult == NULL) {
        return false;
    }

    int n = graph->totalDelegates;
    memset(outResult, 0, sizeof(ProtocolDistributionResult));

    /* 1. Identificar y separar delegados aislados (sin incompatibilidades) */
    int isolatedCount = 0;
    int *isolatedList = (int*)malloc(n * sizeof(int));
    int nonIsolatedCount = 0;
    int *nonIsolatedList = (int*)malloc(n * sizeof(int));

    if (isolatedList == NULL || nonIsolatedList == NULL) {
        free(isolatedList);
        free(nonIsolatedList);
        return false;
    }

    for (int i = 0; i < n; i++) {
        int delegateId = i + 1;
        if (graph->degrees[i] == 0) {
            isolatedList[isolatedCount++] = delegateId;
        } else {
            nonIsolatedList[nonIsolatedCount++] = delegateId;
        }
    }

    outResult->totalIsolatedDelegates = isolatedCount;
    outResult->isolatedDelegates = isolatedList;

    /* Caso especial: si no hay delegados con conflictos (grafo nulo), 1 sola mesa basta */
    if (nonIsolatedCount == 0) {
        outResult->totalTablesRequired = 1;
        outResult->tables = (TableAssignment*)calloc(1, sizeof(TableAssignment));
        if (outResult->tables == NULL) {
            free(nonIsolatedList);
            return false;
        }

        TableAssignment *table = &outResult->tables[0];
        table->tableId = 1;
        table->capacity = n;
        table->delegates = (int*)malloc(n * sizeof(int));
        table->delegateCount = n;
        table->initialCount = 0;
        table->balancedCount = n;

        for (int i = 0; i < n; i++) {
            table->delegates[i] = isolatedList[i];
        }

        outResult->isBalanced = true;
        free(nonIsolatedList);
        return true;
    }

    /* 2. Partición en Conjuntos Independientes Maximales para vértices no aislados */
    /* Algoritmo de partición voraz de subgrafos independientes */
    bool *assigned = (bool*)calloc(n, sizeof(bool));
    if (assigned == NULL) {
        free(nonIsolatedList);
        return false;
    }

    int assignedNonIsolated = 0;
    int tablesAllocated = 0;
    int tablesCapacity = 8;
    TableAssignment *tables = (TableAssignment*)malloc(tablesCapacity * sizeof(TableAssignment));

    if (tables == NULL) {
        free(assigned);
        free(nonIsolatedList);
        return false;
    }

    while (assignedNonIsolated < nonIsolatedCount) {
        /* Redimensionar arreglo de mesas si es necesario */
        if (tablesAllocated >= tablesCapacity) {
            tablesCapacity *= 2;
            TableAssignment *newTables = (TableAssignment*)realloc(tables, tablesCapacity * sizeof(TableAssignment));
            if (newTables == NULL) {
                for (int t = 0; t < tablesAllocated; t++) {
                    free(tables[t].delegates);
                }
                free(tables);
                free(assigned);
                free(nonIsolatedList);
                return false;
            }
            tables = newTables;
        }

        TableAssignment *currentTable = &tables[tablesAllocated];
        currentTable->tableId = tablesAllocated + 1;
        currentTable->capacity = n;
        currentTable->delegates = (int*)malloc(n * sizeof(int));
        currentTable->delegateCount = 0;
        currentTable->initialCount = 0;
        currentTable->balancedCount = 0;

        if (currentTable->delegates == NULL) {
            for (int t = 0; t < tablesAllocated; t++) {
                free(tables[t].delegates);
            }
            free(tables);
            free(assigned);
            free(nonIsolatedList);
            return false;
        }

        /* Recorrer los delegados no aislados en orden natural y agregarlos al conjunto independiente actual */
        for (int i = 0; i < nonIsolatedCount; i++) {
            int candidate = nonIsolatedList[i];
            int candIdx = candidate - 1;

            if (!assigned[candIdx]) {
                if (can_add_to_table(graph, currentTable->delegates, currentTable->delegateCount, candidate)) {
                    currentTable->delegates[currentTable->delegateCount++] = candidate;
                    currentTable->initialCount++;
                    assigned[candIdx] = true;
                    assignedNonIsolated++;
                }
            }
        }

        tablesAllocated++;
    }

    free(assigned);
    free(nonIsolatedList);

    /* 3. Algoritmo de Balanceo de Delegados Libres (Vértices Aislados) */
    /* Cada delegado aislado es asignado a la mesa con menor cantidad actual de delegados */
    for (int i = 0; i < isolatedCount; i++) {
        int isolatedCandidate = isolatedList[i];

        /* Encontrar la mesa con menor número de delegados (empates resueltos por menor ID de mesa) */
        int minTableIndex = 0;
        int minDelegates = tables[0].delegateCount;

        for (int t = 1; t < tablesAllocated; t++) {
            if (tables[t].delegateCount < minDelegates) {
                minDelegates = tables[t].delegateCount;
                minTableIndex = t;
            }
        }

        /* Asignar delegado libre a la mesa elegida */
        TableAssignment *targetTable = &tables[minTableIndex];
        targetTable->delegates[targetTable->delegateCount++] = isolatedCandidate;
        targetTable->balancedCount++;
    }

    outResult->totalTablesRequired = tablesAllocated;
    outResult->tables = tables;
    outResult->isBalanced = (isolatedCount > 0);

    return true;
}

void protocol_free_conflictive_result(ConflictiveDelegatesResult *result) {
    if (result != NULL) {
        if (result->delegateIds != NULL) {
            free(result->delegateIds);
            result->delegateIds = NULL;
        }
        result->count = 0;
        result->maxIncompatibilities = 0;
    }
}

void protocol_free_isolated_result(IsolatedDelegatesResult *result) {
    if (result != NULL) {
        if (result->delegateIds != NULL) {
            free(result->delegateIds);
            result->delegateIds = NULL;
        }
        result->count = 0;
    }
}

void protocol_free_distribution_result(ProtocolDistributionResult *result) {
    if (result != NULL) {
        if (result->tables != NULL) {
            for (int i = 0; i < result->totalTablesRequired; i++) {
                if (result->tables[i].delegates != NULL) {
                    free(result->tables[i].delegates);
                }
            }
            free(result->tables);
            result->tables = NULL;
        }
        if (result->isolatedDelegates != NULL) {
            free(result->isolatedDelegates);
            result->isolatedDelegates = NULL;
        }
        result->totalTablesRequired = 0;
        result->totalIsolatedDelegates = 0;
        result->isBalanced = false;
    }
}
