/**
 * @file ui_view.c
 * @brief Implementación de la interfaz de consola visual para la Organización de Protocolo.
 * @author Brayan Ceballos (bracebalDev)
 * @date 2020 (Original) - 2026 (Refactorización de Portafolio)
 */

#include "ui_view.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

/* Códigos de escape ANSI para formato y colores en terminales compatibles */
#define ANSI_RESET   "\x1b[0m"
#define ANSI_BOLD    "\x1b[1m"
#define ANSI_DIM     "\x1b[2m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_WHITE   "\x1b[37m"
#define ANSI_BG_BLUE "\x1b[44m"

void ui_init(void) {
#if defined(_WIN32) || defined(_WIN64)
    /* Habilitar procesamiento de secuencias ANSI en Windows Terminal / CMD */
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= 0x0004; /* ENABLE_VIRTUAL_TERMINAL_PROCESSING */
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

void ui_clear_screen(void) {
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#else
    system("clear");
#endif
}

void ui_pause(void) {
    printf("\n" ANSI_DIM "Presione [Enter] para continuar..." ANSI_RESET);
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void ui_display_header(void) {
    printf(ANSI_CYAN);
    printf("========================================================================================\n");
    printf("       ORGANIZACION DE LAS NACIONES UNIDAS (ONU)  -  PROTOCOLO DIPLOMATICO              \n");
    printf("     Departamento de la Asamblea General para la Gestion de Conferencias (DAGGC)       \n");
    printf("       Sistema de Optimizacion de Recepciones y Asignacion de Mesas (ED-II)            \n");
    printf("========================================================================================\n");
    printf(ANSI_RESET);
}

void ui_display_graph_summary(const char *filename, const Graph *graph) {
    if (graph == NULL) {
        printf(ANSI_YELLOW "[ Estado: Ningun archivo de convencion cargado ]\n" ANSI_RESET);
        return;
    }

    printf(ANSI_GREEN "+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
    printf("  " ANSI_BOLD "Archivo Activo:" ANSI_RESET " %-20s | " ANSI_BOLD "Total Delegados (|V|):" ANSI_RESET " %-4d | " ANSI_BOLD "Incompatibilidades (|E|):" ANSI_RESET " %-4d\n",
           filename ? filename : "N/A", graph->totalDelegates, graph->totalIncompatibilities);
    printf("  " ANSI_BOLD "Grado Maximo (Delta):" ANSI_RESET " %-14d | " ANSI_BOLD "Delegados Aislados:" ANSI_RESET "   %-4d | " ANSI_BOLD "Delegados c/ Conflicto:" ANSI_RESET "   %-4d\n",
           graph->maxDegree, graph->isolatedCount, graph->totalDelegates - graph->isolatedCount);
    printf(ANSI_GREEN "+--------------------------------------------------------------------------------------+\n\n" ANSI_RESET);
}

void ui_display_menu(const char *filename, const Graph *graph) {
    ui_display_header();
    ui_display_graph_summary(filename, graph);

    printf(ANSI_BOLD "MENU DE OPERACIONES DEL PROTOCOLO DIPLOMATICO:\n" ANSI_RESET);
    printf("  " ANSI_CYAN "[1]" ANSI_RESET " Cargar / Cambiar archivo de Convencion (.in)\n");
    printf("  " ANSI_CYAN "[2]" ANSI_RESET " Requerimiento 1: Verificacion de Compatibilidad entre dos Delegados\n");
    printf("  " ANSI_CYAN "[3]" ANSI_RESET " Requerimiento 2: Identificacion del Delegado(s) mas Conflictivo(s)\n");
    printf("  " ANSI_CYAN "[4]" ANSI_RESET " Requerimiento 3: Listado de Delegados sin Incompatibilidades\n");
    printf("  " ANSI_CYAN "[5]" ANSI_RESET " Requerimiento 4: Distribucion Optima de Mesas con Balanceo\n");
    printf("  " ANSI_CYAN "[6]" ANSI_RESET " Utilidad: Visualizar Matriz de Adyacencia del Grafo\n");
    printf("  " ANSI_CYAN "[7]" ANSI_RESET " Salir del Sistema\n");
    printf("\n" ANSI_BOLD "Seleccione una opcion [1-7]: " ANSI_RESET);
}

void ui_display_requirement1(int delegateA, int delegateB, bool areIncompatible) {
    printf("\n" ANSI_CYAN "+--------------------------------------------------------------------------------------+\n");
    printf("| REQUERIMIENTO 1: VERIFICACION DE INCOMPATIBILIDAD DIPLOMATICA                        |\n");
    printf("+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
    printf("  " ANSI_BOLD "Delegado 1:" ANSI_RESET " %d\n", delegateA);
    printf("  " ANSI_BOLD "Delegado 2:" ANSI_RESET " %d\n\n", delegateB);

    if (areIncompatible) {
        printf("  " ANSI_BOLD ANSI_RED "[!] DICTAMEN: LOS DELEGADOS SON INCOMPATIBLES DIPLOMATICAMENTE" ANSI_RESET "\n");
        printf("  -> Motivo: Existe una arista (%d, %d) en el grafo de relaciones hostiles.\n", delegateA, delegateB);
        printf("  -> Accion de Protocolo: " ANSI_YELLOW "NO deben ubicarse en la misma mesa." ANSI_RESET "\n");
    } else {
        printf("  " ANSI_BOLD ANSI_GREEN "[OK] DICTAMEN: LOS DELEGADOS SON COMPATIBLES" ANSI_RESET "\n");
        printf("  -> Motivo: No existe registro de tension diplomatica entre ambos.\n");
        printf("  -> Accion de Protocolo: " ANSI_GREEN "Pueden compartir mesa de recepcion con total seguridad." ANSI_RESET "\n");
    }
    printf(ANSI_CYAN "+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
}

void ui_display_requirement2(const ConflictiveDelegatesResult *result) {
    printf("\n" ANSI_CYAN "+--------------------------------------------------------------------------------------+\n");
    printf("| REQUERIMIENTO 2: IDENTIFICACION DEL / LOS DELEGADOS MAS CONFLICTIVOS                  |\n");
    printf("+--------------------------------------------------------------------------------------+\n" ANSI_RESET);

    if (result == NULL || result->count == 0) {
        printf("  No hay delegados registrados o el grafo carece de vertices.\n");
        return;
    }

    if (result->maxIncompatibilities == 0) {
        printf("  " ANSI_GREEN "Ningun delegado posee conflictos diplomáticos (Todos tienen 0 incompatibilidades)." ANSI_RESET "\n");
        printf(ANSI_CYAN "+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
        return;
    }

    if (result->count == 1) {
        printf("  " ANSI_BOLD "El delegado " ANSI_RED "%d" ANSI_RESET ANSI_BOLD " es el mas conflictivo con " ANSI_RED "%d" ANSI_RESET ANSI_BOLD " incompatibilidades.\n" ANSI_RESET,
               result->delegateIds[0], result->maxIncompatibilities);
    } else {
        printf("  " ANSI_BOLD "Existen %d delegados con el maximo grado de conflictividad (" ANSI_RED "%d incompatibilidades" ANSI_RESET ANSI_BOLD "):\n" ANSI_RESET,
               result->count, result->maxIncompatibilities);
        printf("  -> Identificadores: ");
        for (int i = 0; i < result->count; i++) {
            printf(ANSI_YELLOW "%d" ANSI_RESET "%s", result->delegateIds[i], (i < result->count - 1) ? ", " : "\n");
        }
    }
    printf(ANSI_CYAN "+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
}

void ui_display_requirement3(const IsolatedDelegatesResult *result, int totalDelegates) {
    printf("\n" ANSI_CYAN "+--------------------------------------------------------------------------------------+\n");
    printf("| REQUERIMIENTO 3: LISTADO DE DELEGADOS SIN INCOMPATIBILIDADES DIPLOMATICAS            |\n");
    printf("+--------------------------------------------------------------------------------------+\n" ANSI_RESET);

    if (result == NULL || result->count == 0) {
        printf("  " ANSI_YELLOW "Todos los delegados (%d) poseen al menos una incompatibilidad diplomatica.\n" ANSI_RESET, totalDelegates);
        printf("  (No existen vertices aislados en el grafo de la convencion).\n");
    } else {
        printf("  " ANSI_GREEN "No poseen incompatibilidades con nadie los siguientes %d delegados:\n" ANSI_RESET, result->count);
        printf("  ");
        for (int i = 0; i < result->count; i++) {
            printf(ANSI_BOLD "%d" ANSI_RESET "%s", result->delegateIds[i], (i < result->count - 1) ? ", " : "\n");
        }
        printf("\n  " ANSI_DIM "Estos delegados seran aprovechados como comodines para balancear las mesas en el Req. 4." ANSI_RESET "\n");
    }
    printf(ANSI_CYAN "+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
}

void ui_display_requirement4(const ProtocolDistributionResult *result, int totalDelegates) {
    printf("\n" ANSI_CYAN "+======================================================================================+\n");
    printf("| REQUERIMIENTO 4: DISTRIBUCION OPTIMA DE MESAS Y BALANCEO DE DELEGADOS                |\n");
    printf("+======================================================================================+\n" ANSI_RESET);

    if (result == NULL || result->totalTablesRequired == 0) {
        printf("  No se ha podido generar la distribucion de mesas.\n");
        return;
    }

    printf("  " ANSI_BOLD "Numero Minimo de Mesas Requerido (Numero Cromatico):" ANSI_RESET " " ANSI_GREEN "%d mesas\n" ANSI_RESET, result->totalTablesRequired);
    printf("  " ANSI_BOLD "Total de Delegados Distribuidos:" ANSI_RESET " %d | " ANSI_BOLD "Delegados Comodines Balanceados:" ANSI_RESET " %d\n\n",
           totalDelegates, result->totalIsolatedDelegates);

    for (int t = 0; t < result->totalTablesRequired; t++) {
        const TableAssignment *table = &result->tables[t];
        printf(ANSI_BOLD ANSI_CYAN "  +--- MESA %d / CONJUNTO %d (%d delegados asignados) --------------------------+\n" ANSI_RESET,
               table->tableId, table->tableId, table->delegateCount);

        printf("  | Delegados: ");
        for (int d = 0; d < table->delegateCount; d++) {
            /* Resaltar si el delegado fue añadido durante la fase de balanceo */
            if (d >= table->initialCount) {
                printf(ANSI_GREEN "%d*" ANSI_RESET "%s", table->delegates[d], (d < table->delegateCount - 1) ? ", " : "");
            } else {
                printf("%d%s", table->delegates[d], (d < table->delegateCount - 1) ? ", " : "");
            }
        }
        printf("\n");
        printf("  | " ANSI_DIM "Composicion: %d delegados originales asignados por coloracion + %d delegados balanceados (*)" ANSI_RESET "\n",
               table->initialCount, table->balancedCount);
        printf(ANSI_CYAN "  +-----------------------------------------------------------------------------+\n\n" ANSI_RESET);
    }

    printf("  " ANSI_DIM "Leyenda: Los numeros con asterisco (*) corresponden a delegados libres integrados por balanceo." ANSI_RESET "\n");
    printf(ANSI_CYAN "+======================================================================================+\n" ANSI_RESET);
}

void ui_display_adjacency_matrix(const Graph *graph) {
    if (graph == NULL) {
        printf("Grafo no inicializado.\n");
        return;
    }

    printf("\n" ANSI_CYAN "+--------------------------------------------------------------------------------------+\n");
    printf("| MATRIZ DE ADYACENCIA DEL GRAFO DE INCOMPATIBILIDADES (|V| = %d)                      |\n", graph->totalDelegates);
    printf("+--------------------------------------------------------------------------------------+\n" ANSI_RESET);

    if (graph->totalDelegates > 35) {
        printf("  " ANSI_YELLOW "Aviso: El grafo posee %d vertices. Mostrando submatriz de los primeros 30 delegados...\n\n" ANSI_RESET, graph->totalDelegates);
    }

    int limit = (graph->totalDelegates < 30) ? graph->totalDelegates : 30;

    printf("     ");
    for (int j = 0; j < limit; j++) {
        printf("%2d ", j + 1);
    }
    printf("\n     ");
    for (int j = 0; j < limit; j++) {
        printf("---");
    }
    printf("\n");

    for (int i = 0; i < limit; i++) {
        printf("%3d |", i + 1);
        for (int j = 0; j < limit; j++) {
            if (graph->adjacencyMatrix[i][j] == 1) {
                printf(ANSI_RED " 1 " ANSI_RESET);
            } else {
                printf(ANSI_DIM " 0 " ANSI_RESET);
            }
        }
        printf("\n");
    }
    printf(ANSI_CYAN "+--------------------------------------------------------------------------------------+\n" ANSI_RESET);
}

void ui_display_success(const char *message) {
    printf("\n" ANSI_BOLD ANSI_GREEN "[EXITO] %s" ANSI_RESET "\n", message);
}

void ui_display_error(const char *message) {
    printf("\n" ANSI_BOLD ANSI_RED "[ERROR] %s" ANSI_RESET "\n", message);
}
