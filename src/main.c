/**
 * @file main.c
 * @brief Punto de entrada principal y controlador interactivo del Sistema de Protocolo Diplomático.
 * @author Brayan Ceballos (bracebalDev)
 * @date 2020 (Original) - 2026 (Refactorización de Portafolio)
 * 
 * Proyecto: Organización de Protocolo para Recepciones Internacionales (ONU - DAGGC)
 * Asignatura: Elementos Discretos II - Universidad de Carabobo
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "graph.h"
#include "file_parser.h"
#include "protocol_service.h"
#include "ui_view.h"

#define MAX_FILENAME_LEN 256
#define INPUT_BUFFER_LEN 64

/**
 * @brief Lee una línea de la entrada estándar y elimina el salto de línea final.
 */
static bool read_user_string(char *buffer, size_t size) {
    if (fgets(buffer, (int)size, stdin) == NULL) {
        return false;
    }
    buffer[strcspn(buffer, "\r\n")] = '\0';
    return true;
}

/**
 * @brief Lee un número entero de la entrada estándar de manera segura.
 */
static bool read_user_int(int *outValue) {
    char buffer[INPUT_BUFFER_LEN];
    if (!read_user_string(buffer, sizeof(buffer))) {
        return false;
    }
    return (sscanf(buffer, "%d", outValue) == 1);
}

int main(int argc, char *argv[]) {
    ui_init();

    Graph *currentGraph = NULL;
    char currentFilename[MAX_FILENAME_LEN] = "Convencion.in";

    /* Intentar cargar archivo inicial especificado por línea de comandos o Convencion.in por defecto */
    if (argc > 1) {
        strncpy(currentFilename, argv[1], MAX_FILENAME_LEN - 1);
        currentFilename[MAX_FILENAME_LEN - 1] = '\0';
    }

    FileParseStatus initStatus = file_parser_load_convention(currentFilename, &currentGraph);
    if (initStatus != PARSE_SUCCESS) {
        /* Si falla el primer intento, dejar grafo como NULL para que el usuario lo cargue desde el menú */
        currentGraph = NULL;
    }

    int option = 0;
    bool running = true;

    while (running) {
        ui_clear_screen();
        ui_display_menu(currentGraph ? currentFilename : "Ninguno", currentGraph);

        if (!read_user_int(&option)) {
            ui_display_error("Entrada no reconocida. Por favor ingrese un numero de opcion del 1 al 7.");
            ui_pause();
            continue;
        }

        switch (option) {
            case 1: {
                /* Cargar nuevo archivo de convención */
                char newFilename[MAX_FILENAME_LEN];
                printf("\nIngrese la ruta o nombre del archivo de convencion (ej: Convencion.in): ");
                if (read_user_string(newFilename, sizeof(newFilename)) && strlen(newFilename) > 0) {
                    Graph *newGraph = NULL;
                    FileParseStatus status = file_parser_load_convention(newFilename, &newGraph);
                    if (status == PARSE_SUCCESS) {
                        if (currentGraph != NULL) {
                            graph_destroy(currentGraph);
                        }
                        currentGraph = newGraph;
                        strncpy(currentFilename, newFilename, MAX_FILENAME_LEN - 1);
                        currentFilename[MAX_FILENAME_LEN - 1] = '\0';
                        ui_display_success("Archivo de convencion cargado exitosamente.");
                    } else {
                        ui_display_error(file_parser_get_error_message(status));
                    }
                } else {
                    ui_display_error("Nombre de archivo invalido.");
                }
                ui_pause();
                break;
            }

            case 2: {
                /* Requerimiento 1: Verificación de compatibilidad */
                if (currentGraph == NULL) {
                    ui_display_error("Debe cargar primero un archivo de convencion (Opcion 1).");
                    ui_pause();
                    break;
                }

                int delegateA = 0;
                int delegateB = 0;

                printf("\n--- REQUERIMIENTO 1: CONSULTA DE COMPATIBILIDAD ---\n");
                printf("Ingrese el numero del primer delegado [1 - %d]: ", currentGraph->totalDelegates);
                if (!read_user_int(&delegateA) || !graph_is_valid_delegate_id(currentGraph, delegateA)) {
                    ui_display_error("Identificador del primer delegado invalido o fuera de rango.");
                    ui_pause();
                    break;
                }

                printf("Ingrese el numero del segundo delegado [1 - %d]: ", currentGraph->totalDelegates);
                if (!read_user_int(&delegateB) || !graph_is_valid_delegate_id(currentGraph, delegateB)) {
                    ui_display_error("Identificador del segundo delegado invalido o fuera de rango.");
                    ui_pause();
                    break;
                }

                bool areIncompatible = false;
                if (protocol_check_compatibility(currentGraph, delegateA, delegateB, &areIncompatible)) {
                    ui_display_requirement1(delegateA, delegateB, areIncompatible);
                } else {
                    ui_display_error("Ocurrio un error al evaluar la compatibilidad.");
                }
                ui_pause();
                break;
            }

            case 3: {
                /* Requerimiento 2: Delegados más conflictivos */
                if (currentGraph == NULL) {
                    ui_display_error("Debe cargar primero un archivo de convencion (Opcion 1).");
                    ui_pause();
                    break;
                }

                ConflictiveDelegatesResult result;
                if (protocol_find_most_conflictive(currentGraph, &result)) {
                    ui_display_requirement2(&result);
                    protocol_free_conflictive_result(&result);
                } else {
                    ui_display_error("No se pudo calcular la conflictividad del grafo.");
                }
                ui_pause();
                break;
            }

            case 4: {
                /* Requerimiento 3: Delegados sin incompatibilidades */
                if (currentGraph == NULL) {
                    ui_display_error("Debe cargar primero un archivo de convencion (Opcion 1).");
                    ui_pause();
                    break;
                }

                IsolatedDelegatesResult result;
                if (protocol_find_isolated_delegates(currentGraph, &result)) {
                    ui_display_requirement3(&result, currentGraph->totalDelegates);
                    protocol_free_isolated_result(&result);
                } else {
                    ui_display_error("Error al obtener la lista de delegados aislados.");
                }
                ui_pause();
                break;
            }

            case 5: {
                /* Requerimiento 4: Distribución de mesas con balanceo */
                if (currentGraph == NULL) {
                    ui_display_error("Debe cargar primero un archivo de convencion (Opcion 1).");
                    ui_pause();
                    break;
                }

                ProtocolDistributionResult result;
                if (protocol_generate_table_distribution(currentGraph, &result)) {
                    ui_display_requirement4(&result, currentGraph->totalDelegates);
                    protocol_free_distribution_result(&result);
                } else {
                    ui_display_error("Error al computar la particion y balanceo de mesas.");
                }
                ui_pause();
                break;
            }

            case 6: {
                /* Utilidad: Visualizar Matriz de Adyacencia */
                if (currentGraph == NULL) {
                    ui_display_error("Debe cargar primero un archivo de convencion (Opcion 1).");
                    ui_pause();
                    break;
                }
                ui_display_adjacency_matrix(currentGraph);
                ui_pause();
                break;
            }

            case 7: {
                /* Salir */
                printf("\nCerrando el Sistema de Protocolo de la ONU DAGGC. ¡Hasta luego!\n\n");
                running = false;
                break;
            }

            default:
                ui_display_error("Opcion fuera de rango. Seleccione un valor entre 1 y 7.");
                ui_pause();
                break;
        }
    }

    if (currentGraph != NULL) {
        graph_destroy(currentGraph);
        currentGraph = NULL;
    }

    return 0;
}
