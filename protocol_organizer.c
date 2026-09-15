/**
 * @file protocol_organizer.c
 * @brief Sistema de Organización de Protocolo para Recepciones Internacionales (ONU - DAGGC)
 * @author Brayan Ceballos (bracebalDev)
 * @date 2020 (Original) - 2026 (Refactorización y Rediseño de Portafolio)
 * 
 * Asignatura: Elementos Discretos II (Teoría de Grafos)
 * Universidad de Carabobo - Facultad Experimental de Ciencias y Tecnología (FACYT)
 * Departamento de Computación
 * 
 * DESCRIPCIÓN:
 * Aplicación en lenguaje ANSI C / C99 que modela y resuelve la distribución óptima de
 * delegados diplomáticos en recepciones oficiales internacionales utilizando Teoría de Grafos:
 * 
 * - Grafo no dirigido G = (V, E):
 *     V: Delegados asistentes (identificados 1..n).
 *     E: Incompatibilidades diplomáticas (relaciones rotas/hostiles).
 * 
 * REQUERIMIENTOS IMPLEMENTADOS:
 * 1. Verificación de Incompatibilidad Diplomática entre dos delegados (O(1)).
 * 2. Identificación del/los Delegado(s) más Conflictivo(s) (Grado Máximo Delta(G)).
 * 3. Listado de Delegados sin Incompatibilidad Diplomática (Vértices Aislados).
 * 4. Asignación Mínima de Mesas (Coloración de Grafos / Conjuntos Independientes)
 *    y Algoritmo Voraz de Balanceo de Comensales.
 * 
 * COMPILACIÓN RÁPIDA (ANSI C / GCC / Clang / MSVC):
 *   gcc -Wall -Wextra -O2 protocol_organizer.c -o protocol_organizer
 * 
 * EJECUCIÓN:
 *   ./protocol_organizer [archivo_entrada.in]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

/* ========================================================================= */
/* CONSTANTES Y CONFIGURACIÓN DE FORMATO VISUAL                              */
/* ========================================================================= */

#define MAX_LINE_BUFFER   512
#define MAX_FILENAME_LEN  256
#define INPUT_BUFFER_LEN  64

/* Secuencias de escape ANSI para estilo de terminal */
#define ANSI_RESET   "\x1b[0m"
#define ANSI_BOLD    "\x1b[1m"
#define ANSI_DIM     "\x1b[2m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_RED     "\x1b[31m"

/* ========================================================================= */
/* ESTRUCTURAS DE DATOS (MODELO TEÓRICO)                                     */
/* ========================================================================= */

/**
 * @struct Graph
 * @brief Tipo de Dato Abstracto (TDA) que encapsula el Grafo de Incompatibilidades.
 */
typedef struct {
    int totalDelegates;             /**< Cantidad total de vértices |V| (1..n) */
    int totalIncompatibilities;     /**< Cantidad total de aristas |E| */
    int **adjacencyMatrix;          /**< Matriz de adyacencia dinámica n x n */
    int *degrees;                   /**< Arreglo de grados de cada delegado */
    int maxDegree;                  /**< Grado máximo del grafo Delta(G) */
    int isolatedCount;              /**< Cantidad de delegados sin conflictos */
} Graph;

/**
 * @struct TableAssignment
 * @brief Estructura de una mesa oficial de recepción.
 */
typedef struct {
    int tableId;                    /**< Identificador de la mesa (1, 2, ...) */
    int delegateCount;              /**< Total de comensales sentados */
    int *delegates;                 /**< Lista de identificadores de delegados */
    int initialCount;               /**< Delegados asignados por coloración inicial */
    int balancedCount;              /**< Delegados libres asignados por balanceo */
} TableAssignment;

/**
 * @struct ProtocolDistributionResult
 * @brief Estructura con la solución completa de la distribución de mesas.
 */
typedef struct {
    int totalTablesRequired;        /**< Número mínimo de mesas requeridas */
    TableAssignment *tables;        /**< Arreglo de mesas */
    int totalIsolatedDelegates;     /**< Total de delegados comodines/aislados */
    int *isolatedDelegates;         /**< Lista de delegados aislados */
} ProtocolDistributionResult;

/**
 * @struct ConflictiveDelegatesResult
 * @brief Estructura con el listado de delegados con mayor conflictividad.
 */
typedef struct {
    int maxIncompatibilities;       /**< Grado máximo registrado */
    int count;                      /**< Cantidad de delegados que empatan en el máximo */
    int *delegateIds;               /**< IDs de los delegados */
} ConflictiveDelegatesResult;

/**
 * @struct IsolatedDelegatesResult
 * @brief Estructura con el listado de delegados libres de conflicto.
 */
typedef struct {
    int count;                      /**< Total de delegados aislados */
    int *delegateIds;               /**< IDs de los delegados */
} IsolatedDelegatesResult;

/* ========================================================================= */
/* FUNCIONES DEL TDA GRAFO                                                   */
/* ========================================================================= */

/**
 * @brief Inicializa una nueva instancia de Grafo dinámico.
 */
Graph* graph_create(int totalDelegates) {
    if (totalDelegates <= 0) return NULL;

    Graph *graph = (Graph*)malloc(sizeof(Graph));
    if (graph == NULL) return NULL;

    graph->totalDelegates = totalDelegates;
    graph->totalIncompatibilities = 0;
    graph->maxDegree = 0;
    graph->isolatedCount = totalDelegates;

    graph->adjacencyMatrix = (int**)malloc(totalDelegates * sizeof(int*));
    if (graph->adjacencyMatrix == NULL) {
        free(graph);
        return NULL;
    }

    for (int i = 0; i < totalDelegates; i++) {
        graph->adjacencyMatrix[i] = (int*)calloc(totalDelegates, sizeof(int));
        if (graph->adjacencyMatrix[i] == NULL) {
            for (int j = 0; j < i; j++) free(graph->adjacencyMatrix[j]);
            free(graph->adjacencyMatrix);
            free(graph);
            return NULL;
        }
    }

    graph->degrees = (int*)calloc(totalDelegates, sizeof(int));
    if (graph->degrees == NULL) {
        for (int i = 0; i < totalDelegates; i++) free(graph->adjacencyMatrix[i]);
        free(graph->adjacencyMatrix);
        free(graph);
        return NULL;
    }

    return graph;
}

/**
 * @brief Libera la memoria ocupada por el grafo.
 */
void graph_destroy(Graph *graph) {
    if (graph == NULL) return;

    if (graph->adjacencyMatrix != NULL) {
        for (int i = 0; i < graph->totalDelegates; i++) {
            if (graph->adjacencyMatrix[i] != NULL) free(graph->adjacencyMatrix[i]);
        }
        free(graph->adjacencyMatrix);
    }
    if (graph->degrees != NULL) free(graph->degrees);
    free(graph);
}

/**
 * @brief Verifica si un identificador de delegado está dentro del rango válido [1, n].
 */
bool graph_is_valid_delegate_id(const Graph *graph, int delegateId) {
    if (graph == NULL) return false;
    return (delegateId >= 1 && delegateId <= graph->totalDelegates);
}

/**
 * @brief Recalcula las métricas de grado máximo y número de aislados.
 */
void graph_compute_metrics(Graph *graph) {
    if (graph == NULL) return;

    int maxDeg = 0;
    int isolated = 0;
    for (int i = 0; i < graph->totalDelegates; i++) {
        int deg = graph->degrees[i];
        if (deg > maxDeg) maxDeg = deg;
        if (deg == 0) isolated++;
    }
    graph->maxDegree = maxDeg;
    graph->isolatedCount = isolated;
}

/**
 * @brief Agrega una arista de incompatibilidad no dirigida entre dos delegados.
 */
bool graph_add_incompatibility(Graph *graph, int delegateA, int delegateB) {
    if (graph == NULL) return false;
    if (!graph_is_valid_delegate_id(graph, delegateA) || !graph_is_valid_delegate_id(graph, delegateB)) return false;
    if (delegateA == delegateB) return false;

    int u = delegateA - 1;
    int v = delegateB - 1;

    if (graph->adjacencyMatrix[u][v] == 1) return true; /* Arista ya existente */

    graph->adjacencyMatrix[u][v] = 1;
    graph->adjacencyMatrix[v][u] = 1;
    graph->degrees[u]++;
    graph->degrees[v]++;
    graph->totalIncompatibilities++;

    graph_compute_metrics(graph);
    return true;
}

/**
 * @brief Consulta de incompatibilidad en O(1).
 */
bool graph_are_incompatible(const Graph *graph, int delegateA, int delegateB) {
    if (graph == NULL) return false;
    if (!graph_is_valid_delegate_id(graph, delegateA) || !graph_is_valid_delegate_id(graph, delegateB)) return false;
    return (graph->adjacencyMatrix[delegateA - 1][delegateB - 1] == 1);
}

/* ========================================================================= */
/* LECTURA Y PARSEO ROBUSTO DE ARCHIVOS                                      */
/* ========================================================================= */

static char* trim_whitespace(char *str) {
    if (str == NULL) return NULL;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

/**
 * @brief Carga y parsea el archivo de convención respetando el formato de la DAGGC.
 */
Graph* load_convention_file(const char *filename, char *errorMessage, size_t errorMsgSize) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        snprintf(errorMessage, errorMsgSize, "No se pudo abrir el archivo '%s'.", filename);
        return NULL;
    }

    char lineBuffer[MAX_LINE_BUFFER];
    int totalDelegates = 0;
    int totalIncompatibilities = 0;
    bool headerFound = false;

    while (fgets(lineBuffer, sizeof(lineBuffer), file) != NULL) {
        char *trimmed = trim_whitespace(lineBuffer);
        if (strlen(trimmed) == 0 || trimmed[0] == '#') continue;

        if (sscanf(trimmed, "%d %d", &totalDelegates, &totalIncompatibilities) == 2) {
            headerFound = true;
            break;
        } else {
            fclose(file);
            snprintf(errorMessage, errorMsgSize, "Encabezado invalido. Se esperaba 'n m'.");
            return NULL;
        }
    }

    if (!headerFound || totalDelegates <= 0 || totalIncompatibilities < 0) {
        fclose(file);
        snprintf(errorMessage, errorMsgSize, "Dimensiones de delegados o aristas invalidas en encabezado.");
        return NULL;
    }

    Graph *graph = graph_create(totalDelegates);
    if (graph == NULL) {
        fclose(file);
        snprintf(errorMessage, errorMsgSize, "Fallo de memoria al asignar estructuras del grafo.");
        return NULL;
    }

    int edgesRead = 0;
    while (edgesRead < totalIncompatibilities && fgets(lineBuffer, sizeof(lineBuffer), file) != NULL) {
        char *trimmed = trim_whitespace(lineBuffer);
        if (strlen(trimmed) == 0 || trimmed[0] == '#') continue;

        int u = 0, v = 0;
        int parsed = 0;

        char *openP = strchr(trimmed, '(');
        char *comma = strchr(trimmed, ',');
        char *closeP = strchr(trimmed, ')');

        if (openP && comma && closeP && (openP < comma) && (comma < closeP)) {
            parsed = sscanf(openP, "(%d,%d)", &u, &v);
            if (parsed != 2) parsed = sscanf(openP, "(%d , %d)", &u, &v);
        } else {
            parsed = sscanf(trimmed, "%d %d", &u, &v);
        }

        if (parsed != 2) {
            graph_destroy(graph);
            fclose(file);
            snprintf(errorMessage, errorMsgSize, "Formato incorrecto en linea de incompatibilidad: '%s'.", trimmed);
            return NULL;
        }

        if (!graph_is_valid_delegate_id(graph, u) || !graph_is_valid_delegate_id(graph, v)) {
            graph_destroy(graph);
            fclose(file);
            snprintf(errorMessage, errorMsgSize, "Delegado (%d, %d) fuera de rango [1, %d].", u, v, totalDelegates);
            return NULL;
        }

        graph_add_incompatibility(graph, u, v);
        edgesRead++;
    }

    fclose(file);
    graph_compute_metrics(graph);
    return graph;
}

/* ========================================================================= */
/* LÓGICA DE PROTOCOLO DIPLOMÁTICO (REQUERIMIENTOS 1, 2, 3 Y 4)              */
/* ========================================================================= */

/**
 * @brief REQUERIMIENTO 1: Verificación de incompatibilidad.
 */
bool protocol_req1_check(const Graph *graph, int delegateA, int delegateB, bool *outAreIncompatible) {
    if (graph == NULL || outAreIncompatible == NULL) return false;
    if (!graph_is_valid_delegate_id(graph, delegateA) || !graph_is_valid_delegate_id(graph, delegateB)) return false;

    if (delegateA == delegateB) {
        *outAreIncompatible = false;
        return true;
    }

    *outAreIncompatible = graph_are_incompatible(graph, delegateA, delegateB);
    return true;
}

/**
 * @brief REQUERIMIENTO 2: Identificación del/los delegado(s) más conflictivo(s).
 */
bool protocol_req2_most_conflictive(const Graph *graph, ConflictiveDelegatesResult *outResult) {
    if (graph == NULL || outResult == NULL) return false;

    int maxDeg = graph->maxDegree;
    int count = 0;
    for (int i = 0; i < graph->totalDelegates; i++) {
        if (graph->degrees[i] == maxDeg) count++;
    }

    outResult->maxIncompatibilities = maxDeg;
    outResult->count = count;
    outResult->delegateIds = (int*)malloc(count * sizeof(int));
    if (outResult->delegateIds == NULL && count > 0) return false;

    int idx = 0;
    for (int i = 0; i < graph->totalDelegates; i++) {
        if (graph->degrees[i] == maxDeg) {
            outResult->delegateIds[idx++] = i + 1;
        }
    }
    return true;
}

/**
 * @brief REQUERIMIENTO 3: Listado de delegados sin incompatibilidad alguna.
 */
bool protocol_req3_isolated(const Graph *graph, IsolatedDelegatesResult *outResult) {
    if (graph == NULL || outResult == NULL) return false;

    int count = graph->isolatedCount;
    outResult->count = count;
    outResult->delegateIds = (int*)malloc((count > 0 ? count : 1) * sizeof(int));
    if (outResult->delegateIds == NULL && count > 0) return false;

    int idx = 0;
    for (int i = 0; i < graph->totalDelegates; i++) {
        if (graph->degrees[i] == 0) {
            outResult->delegateIds[idx++] = i + 1;
        }
    }
    return true;
}

static bool can_join_table(const Graph *graph, const int *tableDelegates, int count, int candidate) {
    int candIdx = candidate - 1;
    for (int i = 0; i < count; i++) {
        if (graph->adjacencyMatrix[candIdx][tableDelegates[i] - 1] == 1) return false;
    }
    return true;
}

/**
 * @brief REQUERIMIENTO 4: Partición óptima en mesas con balanceo de comensales libres.
 */
bool protocol_req4_distribution(const Graph *graph, ProtocolDistributionResult *outResult) {
    if (graph == NULL || outResult == NULL) return false;

    int n = graph->totalDelegates;
    memset(outResult, 0, sizeof(ProtocolDistributionResult));

    /* 1. Separar delegados aislados y no aislados */
    int isolatedCount = 0;
    int *isolatedList = (int*)malloc(n * sizeof(int));
    int nonIsolatedCount = 0;
    int *nonIsolatedList = (int*)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        int dId = i + 1;
        if (graph->degrees[i] == 0) isolatedList[isolatedCount++] = dId;
        else nonIsolatedList[nonIsolatedCount++] = dId;
    }

    outResult->totalIsolatedDelegates = isolatedCount;
    outResult->isolatedDelegates = isolatedList;

    if (nonIsolatedCount == 0) {
        /* Grafo nulo: todos compatibles entre sí, basta 1 mesa */
        outResult->totalTablesRequired = 1;
        outResult->tables = (TableAssignment*)calloc(1, sizeof(TableAssignment));
        outResult->tables[0].tableId = 1;
        outResult->tables[0].delegates = (int*)malloc(n * sizeof(int));
        outResult->tables[0].delegateCount = n;
        outResult->tables[0].initialCount = 0;
        outResult->tables[0].balancedCount = n;
        for (int i = 0; i < n; i++) outResult->tables[0].delegates[i] = isolatedList[i];
        free(nonIsolatedList);
        return true;
    }

    /* 2. Coloración voraz por partición de conjuntos independientes */
    bool *assigned = (bool*)calloc(n, sizeof(bool));
    int assignedCount = 0;
    int tablesCount = 0;
    int tablesCapacity = 8;
    TableAssignment *tables = (TableAssignment*)malloc(tablesCapacity * sizeof(TableAssignment));

    while (assignedCount < nonIsolatedCount) {
        if (tablesCount >= tablesCapacity) {
            tablesCapacity *= 2;
            tables = (TableAssignment*)realloc(tables, tablesCapacity * sizeof(TableAssignment));
        }

        TableAssignment *currTable = &tables[tablesCount];
        currTable->tableId = tablesCount + 1;
        currTable->delegates = (int*)malloc(n * sizeof(int));
        currTable->delegateCount = 0;
        currTable->initialCount = 0;
        currTable->balancedCount = 0;

        for (int i = 0; i < nonIsolatedCount; i++) {
            int cand = nonIsolatedList[i];
            int candIdx = cand - 1;
            if (!assigned[candIdx] && can_join_table(graph, currTable->delegates, currTable->delegateCount, cand)) {
                currTable->delegates[currTable->delegateCount++] = cand;
                currTable->initialCount++;
                assigned[candIdx] = true;
                assignedCount++;
            }
        }
        tablesCount++;
    }

    free(assigned);
    free(nonIsolatedList);

    /* 3. Balanceo voraz: asignar delegados libres a las mesas con menor cantidad */
    for (int i = 0; i < isolatedCount; i++) {
        int iso = isolatedList[i];
        int minIdx = 0;
        int minCount = tables[0].delegateCount;
        for (int t = 1; t < tablesCount; t++) {
            if (tables[t].delegateCount < minCount) {
                minCount = tables[t].delegateCount;
                minIdx = t;
            }
        }
        tables[minIdx].delegates[tables[minIdx].delegateCount++] = iso;
        tables[minIdx].balancedCount++;
    }

    outResult->totalTablesRequired = tablesCount;
    outResult->tables = tables;
    return true;
}

/* ========================================================================= */
/* FUNCIONES DE LIMPIEZA DE MEMORIA                                          */
/* ========================================================================= */

void free_conflictive_result(ConflictiveDelegatesResult *res) {
    if (res && res->delegateIds) {
        free(res->delegateIds);
        res->delegateIds = NULL;
    }
}

void free_isolated_result(IsolatedDelegatesResult *res) {
    if (res && res->delegateIds) {
        free(res->delegateIds);
        res->delegateIds = NULL;
    }
}

void free_distribution_result(ProtocolDistributionResult *res) {
    if (res) {
        if (res->tables) {
            for (int i = 0; i < res->totalTablesRequired; i++) {
                if (res->tables[i].delegates) free(res->tables[i].delegates);
            }
            free(res->tables);
            res->tables = NULL;
        }
        if (res->isolatedDelegates) {
            free(res->isolatedDelegates);
            res->isolatedDelegates = NULL;
        }
        res->totalTablesRequired = 0;
    }
}

/* ========================================================================= */
/* INTERFAZ DE USUARIO VISUAL (UI PRESENTATION LAYER)                        */
/* ========================================================================= */

void init_console_ui(void) {
#if defined(_WIN32) || defined(_WIN64)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= 0x0004;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

void clear_screen(void) {
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#else
    system("clear");
#endif
}

void pause_console(void) {
    printf("\n" ANSI_DIM "Presione [Enter] para continuar..." ANSI_RESET);
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void print_banner(void) {
    printf(ANSI_CYAN);
    printf("========================================================================================\n");
    printf("       ORGANIZACION DE LAS NACIONES UNIDAS (ONU)  -  PROTOCOLO DIPLOMATICO              \n");
    printf("     Departamento de la Asamblea General para la Gestion de Conferencias (DAGGC)       \n");
    printf("       Sistema de Optimizacion de Recepciones y Asignacion de Mesas (ED-II)            \n");
    printf("========================================================================================\n");
    printf(ANSI_RESET);
}

void print_summary(const char *filename, const Graph *graph) {
    if (graph == NULL) {
        printf(ANSI_YELLOW "[ Estado: Ningun archivo de convencion cargado ]\n\n" ANSI_RESET);
        return;
    }
    printf(ANSI_GREEN "+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
    printf("  " ANSI_BOLD "Archivo Activo:" ANSI_RESET " %-20s | " ANSI_BOLD "Total Delegados (|V|):" ANSI_RESET " %-4d | " ANSI_BOLD "Incompatibilidades (|E|):" ANSI_RESET " %-4d\n",
           filename ? filename : "N/A", graph->totalDelegates, graph->totalIncompatibilities);
    printf("  " ANSI_BOLD "Grado Maximo (Delta):" ANSI_RESET " %-14d | " ANSI_BOLD "Delegados Aislados:" ANSI_RESET "   %-4d | " ANSI_BOLD "Delegados c/ Conflicto:" ANSI_RESET "   %-4d\n",
           graph->maxDegree, graph->isolatedCount, graph->totalDelegates - graph->isolatedCount);
    printf(ANSI_GREEN "+--------------------------------------------------------------------------------------+\n\n" ANSI_RESET);
}

void show_menu(const char *filename, const Graph *graph) {
    print_banner();
    print_summary(filename, graph);
    printf(ANSI_BOLD "MENU DE OPCIONES:\n" ANSI_RESET);
    printf("  " ANSI_CYAN "[1]" ANSI_RESET " Cargar / Cambiar archivo de Convencion (.in)\n");
    printf("  " ANSI_CYAN "[2]" ANSI_RESET " Requerimiento 1: Verificacion de Compatibilidad entre dos Delegados\n");
    printf("  " ANSI_CYAN "[3]" ANSI_RESET " Requerimiento 2: Identificacion del Delegado(s) mas Conflictivo(s)\n");
    printf("  " ANSI_CYAN "[4]" ANSI_RESET " Requerimiento 3: Listado de Delegados sin Incompatibilidades\n");
    printf("  " ANSI_CYAN "[5]" ANSI_RESET " Requerimiento 4: Distribucion Optima de Mesas con Balanceo\n");
    printf("  " ANSI_CYAN "[6]" ANSI_RESET " Utilidad: Visualizar Matriz de Adyacencia del Grafo\n");
    printf("  " ANSI_CYAN "[7]" ANSI_RESET " Salir del Programa\n");
    printf("\n" ANSI_BOLD "Seleccione una opcion [1-7]: " ANSI_RESET);
}

/* ========================================================================= */
/* CONTROLADOR PRINCIPAL Y BUCLE DE EJECUCIÓN                                */
/* ========================================================================= */

int main(int argc, char *argv[]) {
    init_console_ui();

    Graph *currentGraph = NULL;
    char currentFilename[MAX_FILENAME_LEN] = "Convencion.in";
    char errorMsg[256] = {0};

    if (argc > 1) {
        strncpy(currentFilename, argv[1], sizeof(currentFilename) - 1);
    }

    currentGraph = load_convention_file(currentFilename, errorMsg, sizeof(errorMsg));

    char inputBuf[INPUT_BUFFER_LEN];
    int option = 0;
    bool running = true;

    while (running) {
        clear_screen();
        show_menu(currentGraph ? currentFilename : "Ninguno", currentGraph);

        if (fgets(inputBuf, sizeof(inputBuf), stdin) == NULL || sscanf(inputBuf, "%d", &option) != 1) {
            printf(ANSI_RED "\n[ERROR] Opcion invalida. Ingrese un numero del 1 al 7.\n" ANSI_RESET);
            pause_console();
            continue;
        }

        switch (option) {
            case 1: {
                char newFilename[MAX_FILENAME_LEN];
                printf("\nIngrese el nombre del archivo (ej: Convencion.in): ");
                if (fgets(newFilename, sizeof(newFilename), stdin)) {
                    newFilename[strcspn(newFilename, "\r\n")] = '\0';
                    char err[256] = {0};
                    Graph *g = load_convention_file(newFilename, err, sizeof(err));
                    if (g != NULL) {
                        if (currentGraph) graph_destroy(currentGraph);
                        currentGraph = g;
                        strncpy(currentFilename, newFilename, sizeof(currentFilename) - 1);
                        printf(ANSI_GREEN "\n[EXITO] Archivo cargado correctamente.\n" ANSI_RESET);
                    } else {
                        printf(ANSI_RED "\n[ERROR] %s\n" ANSI_RESET, err);
                    }
                }
                pause_console();
                break;
            }

            case 2: {
                if (!currentGraph) {
                    printf(ANSI_RED "\n[ERROR] Debe cargar un archivo primero.\n" ANSI_RESET);
                    pause_console();
                    break;
                }
                int da = 0, db = 0;
                printf("\n--- REQUERIMIENTO 1: VERIFICACION DE INCOMPATIBILIDAD ---\n");
                printf("Ingrese el primer delegado [1 - %d]: ", currentGraph->totalDelegates);
                if (fgets(inputBuf, sizeof(inputBuf), stdin) && sscanf(inputBuf, "%d", &da) == 1 && graph_is_valid_delegate_id(currentGraph, da)) {
                    printf("Ingrese el segundo delegado [1 - %d]: ", currentGraph->totalDelegates);
                    if (fgets(inputBuf, sizeof(inputBuf), stdin) && sscanf(inputBuf, "%d", &db) == 1 && graph_is_valid_delegate_id(currentGraph, db)) {
                        bool areIncomp = false;
                        protocol_req1_check(currentGraph, da, db, &areIncomp);
                        printf("\n" ANSI_CYAN "+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
                        if (areIncomp) {
                            printf("  " ANSI_BOLD ANSI_RED "[!] LOS DELEGADOS %d Y %d SON INCOMPATIBLES DIPLOMATICAMENTE." ANSI_RESET "\n", da, db);
                            printf("  -> Accion de Protocolo: " ANSI_YELLOW "Deben asignarse a mesas diferentes." ANSI_RESET "\n");
                        } else {
                            printf("  " ANSI_BOLD ANSI_GREEN "[OK] LOS DELEGADOS %d Y %d SON COMPATIBLES." ANSI_RESET "\n", da, db);
                            printf("  -> Accion de Protocolo: " ANSI_GREEN "Pueden sentarse en la misma mesa." ANSI_RESET "\n");
                        }
                        printf(ANSI_CYAN "+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
                    } else {
                        printf(ANSI_RED "\n[ERROR] Segundo delegado invalido.\n" ANSI_RESET);
                    }
                } else {
                    printf(ANSI_RED "\n[ERROR] Primer delegado invalido.\n" ANSI_RESET);
                }
                pause_console();
                break;
            }

            case 3: {
                if (!currentGraph) {
                    printf(ANSI_RED "\n[ERROR] Debe cargar un archivo primero.\n" ANSI_RESET);
                    pause_console();
                    break;
                }
                ConflictiveDelegatesResult res;
                if (protocol_req2_most_conflictive(currentGraph, &res)) {
                    printf("\n" ANSI_CYAN "+--------------------------------------------------------------------------------------+\n");
                    printf("| REQUERIMIENTO 2: DELEGADOS MAS CONFLICTIVOS                                          |\n");
                    printf("+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
                    if (res.count == 1) {
                        printf("  El delegado " ANSI_RED "%d" ANSI_RESET " es el mas conflictivo con " ANSI_RED "%d" ANSI_RESET " incompatibilidades.\n",
                               res.delegateIds[0], res.maxIncompatibilities);
                    } else {
                        printf("  Existen %d delegados con el maximo de incompatibilidades (" ANSI_RED "%d" ANSI_RESET "):\n  -> ", res.count, res.maxIncompatibilities);
                        for (int i = 0; i < res.count; i++) {
                            printf(ANSI_YELLOW "%d" ANSI_RESET "%s", res.delegateIds[i], (i < res.count - 1) ? ", " : "\n");
                        }
                    }
                    printf(ANSI_CYAN "+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
                    free_conflictive_result(&res);
                }
                pause_console();
                break;
            }

            case 4: {
                if (!currentGraph) {
                    printf(ANSI_RED "\n[ERROR] Debe cargar un archivo primero.\n" ANSI_RESET);
                    pause_console();
                    break;
                }
                IsolatedDelegatesResult res;
                if (protocol_req3_isolated(currentGraph, &res)) {
                    printf("\n" ANSI_CYAN "+--------------------------------------------------------------------------------------+\n");
                    printf("| REQUERIMIENTO 3: DELEGADOS SIN INCOMPATIBILIDADES                                    |\n");
                    printf("+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
                    if (res.count == 0) {
                        printf("  Todos los delegados poseen al menos una incompatibilidad diplomatica.\n");
                    } else {
                        printf("  No poseen incompatibilidades con nadie los siguientes delegados:\n  -> ");
                        for (int i = 0; i < res.count; i++) {
                            printf(ANSI_BOLD "%d" ANSI_RESET "%s", res.delegateIds[i], (i < res.count - 1) ? ", " : "\n");
                        }
                    }
                    printf(ANSI_CYAN "+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
                    free_isolated_result(&res);
                }
                pause_console();
                break;
            }

            case 5: {
                if (!currentGraph) {
                    printf(ANSI_RED "\n[ERROR] Debe cargar un archivo primero.\n" ANSI_RESET);
                    pause_console();
                    break;
                }
                ProtocolDistributionResult res;
                if (protocol_req4_distribution(currentGraph, &res)) {
                    printf("\n" ANSI_CYAN "+======================================================================================+\n");
                    printf("| REQUERIMIENTO 4: DISTRIBUCION OPTIMA DE MESAS (CON BALANCEO)                         |\n");
                    printf("+======================================================================================+\n" ANSI_RESET);
                    printf("  " ANSI_BOLD "Se requieren %d conjuntos / mesas minimas." ANSI_RESET "\n\n", res.totalTablesRequired);

                    for (int t = 0; t < res.totalTablesRequired; t++) {
                        TableAssignment *tb = &res.tables[t];
                        printf(ANSI_BOLD ANSI_CYAN "  Conjunto %d / Mesa %d (%d delegados):" ANSI_RESET "\n  ", tb->tableId, tb->tableId, tb->delegateCount);
                        for (int d = 0; d < tb->delegateCount; d++) {
                            if (d >= tb->initialCount) {
                                printf(ANSI_GREEN "%d*" ANSI_RESET "%s", tb->delegates[d], (d < tb->delegateCount - 1) ? ", " : "\n");
                            } else {
                                printf("%d%s", tb->delegates[d], (d < tb->delegateCount - 1) ? ", " : "\n");
                            }
                        }
                        printf("  " ANSI_DIM "-> Base: %d delegados con restricciones + %d libres balanceados (*)" ANSI_RESET "\n\n", tb->initialCount, tb->balancedCount);
                    }
                    printf("  " ANSI_DIM "(*) Delegados comodines integrados para balancear tamano de mesas." ANSI_RESET "\n");
                    printf(ANSI_CYAN "+======================================================================================+\n" ANSI_RESET);
                    free_distribution_result(&res);
                }
                pause_console();
                break;
            }

            case 6: {
                if (!currentGraph) {
                    printf(ANSI_RED "\n[ERROR] Debe cargar un archivo primero.\n" ANSI_RESET);
                    pause_console();
                    break;
                }
                int limit = (currentGraph->totalDelegates < 30) ? currentGraph->totalDelegates : 30;
                printf("\n" ANSI_CYAN "+--- MATRIZ DE ADYACENCIA (|V| = %d) ---------------------------------------------------+\n" ANSI_RESET, currentGraph->totalDelegates);
                printf("     ");
                for (int j = 0; j < limit; j++) printf("%2d ", j + 1);
                printf("\n     ");
                for (int j = 0; j < limit; j++) printf("---");
                printf("\n");
                for (int i = 0; i < limit; i++) {
                    printf("%3d |", i + 1);
                    for (int j = 0; j < limit; j++) {
                        if (currentGraph->adjacencyMatrix[i][j]) printf(ANSI_RED " 1 " ANSI_RESET);
                        else printf(ANSI_DIM " 0 " ANSI_RESET);
                    }
                    printf("\n");
                }
                printf(ANSI_CYAN "+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
                pause_console();
                break;
            }

            case 7: {
                printf("\nSaliendo del Sistema de Protocolo. ¡Hasta luego!\n\n");
                running = false;
                break;
            }

            default:
                printf(ANSI_RED "\n[ERROR] Opcion fuera de rango.\n" ANSI_RESET);
                pause_console();
                break;
        }
    }

    if (currentGraph) graph_destroy(currentGraph);
    return 0;
}
