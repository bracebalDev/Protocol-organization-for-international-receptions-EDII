/**
 * @file protocol_service.h
 * @brief Capa de Servicios de Protocolo Diplomático (Lógica de Negocio / Teoría de Grafos).
 * @author Brayan Ceballos (bracebalDev)
 * @date 2020 (Original) - 2026 (Refactorización de Portafolio)
 * 
 * Este módulo contiene la implementación algorítmica de los 4 requerimientos solicitados
 * por el Departamento de la Asamblea General para la Gestión de Conferencias (DAGGC)
 * de la Organización de las Naciones Unidas (ONU):
 * 
 * 1. Verificación de incompatibilidad entre dos delegados arbitrarios.
 * 2. Identificación del/los delegado(s) con mayor conflictividad (Grado Máximo).
 * 3. Identificación de delegados sin incompatibilidad diplomática (Vértices Aislados).
 * 4. Asignación óptima de mesas (Coloración de Grafos / Conjuntos Independientes)
 *    con balanceo de delegados libres de conflicto.
 */

#ifndef PROTOCOL_SERVICE_H
#define PROTOCOL_SERVICE_H

#include "graph.h"
#include <stdbool.h>

/**
 * @struct TableAssignment
 * @brief Estructura que representa la asignación individual de una mesa de recepción.
 */
typedef struct {
    int tableId;                /**< Número identificador de la mesa (1, 2, ...) */
    int delegateCount;          /**< Cantidad total de delegados sentados en esta mesa */
    int capacity;               /**< Capacidad reservada en memoria dinámica para el arreglo */
    int *delegates;             /**< Arreglo de identificadores de los delegados asignados */
    int initialCount;           /**< Cantidad de delegados con restricciones asignados originalmente */
    int balancedCount;          /**< Cantidad de delegados libres asignados durante el balanceo */
} TableAssignment;

/**
 * @struct ProtocolDistributionResult
 * @brief Estructura que almacena el resultado completo de la distribución de mesas.
 */
typedef struct {
    int totalTablesRequired;    /**< Número mínimo de mesas necesarias (Número Cromático chi(G)) */
    TableAssignment *tables;    /**< Arreglo de estructuras de mesas asignadas */
    int totalIsolatedDelegates; /**< Total de delegados que estaban libres de incompatibilidad */
    int *isolatedDelegates;     /**< Lista de delegados libres agrupados antes de balanceo */
    bool isBalanced;            /**< Indica si el balanceo de carga fue aplicado */
} ProtocolDistributionResult;

/**
 * @struct ConflictiveDelegatesResult
 * @brief Estructura que almacena la lista de delegados más conflictivos del evento.
 */
typedef struct {
    int maxIncompatibilities;   /**< Número máximo de conflictos registrado (Delta(G)) */
    int count;                  /**< Cantidad de delegados que empatan con dicho grado máximo */
    int *delegateIds;           /**< Arreglo con los IDs de dichos delegados */
} ConflictiveDelegatesResult;

/**
 * @struct IsolatedDelegatesResult
 * @brief Estructura que almacena los delegados libres de todo conflicto.
 */
typedef struct {
    int count;                  /**< Cantidad de delegados sin incompatibilidades */
    int *delegateIds;           /**< Arreglo de identificadores de delegados libres */
} IsolatedDelegatesResult;

/* ========================================================================= */
/* FUNCIONES DE SERVICIO DEL PROTOCOLO                                       */
/* ========================================================================= */

/**
 * @brief REQUERIMIENTO 1: Verifica si dos delegados tienen incompatibilidad diplomática.
 * 
 * @param graph Puntero constante al grafo de la convención.
 * @param delegateA Identificador del primer delegado.
 * @param delegateB Identificador del segundo delegado.
 * @param outAreIncompatible Puntero a booleano donde se almacena el resultado.
 * @return true Si la consulta se procesó válidamente, false si hubo error en los identificadores.
 */
bool protocol_check_compatibility(const Graph *graph, int delegateA, int delegateB, bool *outAreIncompatible);

/**
 * @brief REQUERIMIENTO 2: Encuentra los delegados con mayor grado de conflictividad.
 * 
 * @param graph Puntero constante al grafo.
 * @param outResult Puntero a la estructura donde se almacenarán los resultados.
 * @return true Si se resolvió con éxito, false en caso de error.
 */
bool protocol_find_most_conflictive(const Graph *graph, ConflictiveDelegatesResult *outResult);

/**
 * @brief REQUERIMIENTO 3: Lista todos los delegados que no poseen incompatibilidad alguna.
 * 
 * @param graph Puntero constante al grafo.
 * @param outResult Puntero a la estructura de delegados libres.
 * @return true Si se ejecutó con éxito, false en caso de error.
 */
bool protocol_find_isolated_delegates(const Graph *graph, IsolatedDelegatesResult *outResult);

/**
 * @brief REQUERIMIENTO 4: Realiza la partición óptima en mesas y distribuye los delegados libres.
 * 
 * Algoritmo:
 * 1. Separa los vértices aislados I = {v in V | deg(v) = 0} en un conjunto aparte.
 * 2. Aplica coloración de grafos (conjuntos independientes maximales) sobre G[V \ I].
 * 3. Asigna cada conjunto independiente a una mesa oficial (número mínimo de mesas).
 * 4. Aplica algoritmo voraz de balanceo: distribuye secuencialmente cada delegado aislado
 *    en la mesa que posea menor cantidad de comensales en ese momento.
 * 
 * @param graph Puntero constante al grafo.
 * @param outResult Puntero a la estructura receptora del plan de distribución.
 * @return true Si la distribución fue completada con éxito, false en caso de error.
 */
bool protocol_generate_table_distribution(const Graph *graph, ProtocolDistributionResult *outResult);

/**
 * @brief Libera la memoria reservada por un resultado de delegados conflictivos.
 * 
 * @param result Puntero a la estructura a liberar.
 */
void protocol_free_conflictive_result(ConflictiveDelegatesResult *result);

/**
 * @brief Libera la memoria reservada por un resultado de delegados aislados.
 * 
 * @param result Puntero a la estructura a liberar.
 */
void protocol_free_isolated_result(IsolatedDelegatesResult *result);

/**
 * @brief Libera la memoria reservada por el plan de distribución de mesas.
 * 
 * @param result Puntero a la estructura a liberar.
 */
void protocol_free_distribution_result(ProtocolDistributionResult *result);

#endif /* PROTOCOL_SERVICE_H */
