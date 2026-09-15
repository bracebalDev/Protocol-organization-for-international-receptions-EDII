/**
 * @file ui_view.h
 * @brief Interfaz de usuario por consola con diseño visual avanzado y formateo estructurado.
 * @author Brayan Ceballos (bracebalDev)
 * @date 2020 (Original) - 2026 (Refactorización de Portafolio)
 * 
 * Ofrece una interfaz de usuario pulida y profesional en terminal con:
 * - Banners institucionales ONU / DAGGC
 * - Cajas delimitadas con caracteres ASCII / UTF-8
 * - Resúmenes visuales de métricas del grafo
 * - Tablas y tarjetas formateadas para la visualización de resultados
 */

#ifndef UI_VIEW_H
#define UI_VIEW_H

#include "graph.h"
#include "protocol_service.h"
#include <stdbool.h>

/**
 * @brief Inicializa la capa de UI (configura terminal y codificación si aplica).
 */
void ui_init(void);

/**
 * @brief Limpia la pantalla de la terminal de manera multiplataforma.
 */
void ui_clear_screen(void);

/**
 * @brief Pausa la ejecución esperando que el usuario presione Enter.
 */
void ui_pause(void);

/**
 * @brief Imprime el encabezado visual principal del sistema ONU - DAGGC.
 */
void ui_display_header(void);

/**
 * @brief Imprime el menú interactivo de opciones.
 * 
 * @param filename Nombre del archivo actualmente cargado (o "Ninguno").
 * @param graph Puntero al grafo cargado (o NULL).
 */
void ui_display_menu(const char *filename, const Graph *graph);

/**
 * @brief Muestra el resumen de métricas del grafo cargado.
 * 
 * @param filename Nombre del archivo.
 * @param graph Puntero al grafo.
 */
void ui_display_graph_summary(const char *filename, const Graph *graph);

/**
 * @brief Renderiza el resultado del Requerimiento 1 (Consulta de Incompatibilidad).
 * 
 * @param delegateA Primer delegado consultado.
 * @param delegateB Segundo delegado consultado.
 * @param areIncompatible true si existe conflicto diplomático, false si son compatibles.
 */
void ui_display_requirement1(int delegateA, int delegateB, bool areIncompatible);

/**
 * @brief Renderiza el resultado del Requerimiento 2 (Delegados más Conflictivos).
 * 
 * @param result Estructura con la información de los delegados con mayor grado.
 */
void ui_display_requirement2(const ConflictiveDelegatesResult *result);

/**
 * @brief Renderiza el resultado del Requerimiento 3 (Delegados sin Incompatibilidades).
 * 
 * @param result Estructura con la lista de delegados aislados.
 * @param totalDelegates Cantidad total de delegados en la convención.
 */
void ui_display_requirement3(const IsolatedDelegatesResult *result, int totalDelegates);

/**
 * @brief Renderiza el resultado del Requerimiento 4 (Distribución y Balanceo de Mesas).
 * 
 * @param result Estructura con las mesas asignadas y estadísticas de balanceo.
 * @param totalDelegates Total de delegados del evento.
 */
void ui_display_requirement4(const ProtocolDistributionResult *result, int totalDelegates);

/**
 * @brief Muestra la matriz de adyacencia del grafo de forma visual y legible.
 * 
 * @param graph Puntero al grafo.
 */
void ui_display_adjacency_matrix(const Graph *graph);

/**
 * @brief Muestra mensaje de éxito o información destacada.
 * 
 * @param message Texto a presentar.
 */
void ui_display_success(const char *message);

/**
 * @brief Muestra mensaje de error visualmente destacado.
 * 
 * @param message Texto del error.
 */
void ui_display_error(const char *message);

#endif /* UI_VIEW_H */
