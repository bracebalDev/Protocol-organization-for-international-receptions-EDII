/**
 * @file file_parser.c
 * @brief Implementación del parser robusto de archivos de entrada de convención.
 * @author Brayan Ceballos (bracebalDev)
 * @date 2020 (Original) - 2026 (Refactorización de Portafolio)
 */

#include "file_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_BUFFER 512

/**
 * @brief Elimina espacios en blanco iniciales y finales de una cadena.
 */
static char* trim_whitespace(char *str) {
    if (str == NULL) return NULL;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

FileParseStatus file_parser_load_convention(const char *filename, Graph **outGraph) {
    if (filename == NULL || outGraph == NULL) {
        return PARSE_ERROR_FILE_NOT_FOUND;
    }

    *outGraph = NULL;

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return PARSE_ERROR_FILE_NOT_FOUND;
    }

    char lineBuffer[MAX_LINE_BUFFER];
    int totalDelegates = 0;
    int totalIncompatibilities = 0;

    /* 1. Leer encabezado: n m */
    bool headerFound = false;
    while (fgets(lineBuffer, sizeof(lineBuffer), file) != NULL) {
        char *trimmed = trim_whitespace(lineBuffer);
        if (strlen(trimmed) == 0 || trimmed[0] == '#') {
            continue; /* Ignorar líneas vacías o comentarios */
        }

        if (sscanf(trimmed, "%d %d", &totalDelegates, &totalIncompatibilities) == 2) {
            headerFound = true;
            break;
        } else {
            fclose(file);
            return PARSE_ERROR_INVALID_HEADER;
        }
    }

    if (!headerFound) {
        fclose(file);
        return PARSE_ERROR_INVALID_HEADER;
    }

    if (totalDelegates <= 0 || totalIncompatibilities < 0) {
        fclose(file);
        return PARSE_ERROR_INVALID_DIMENSIONS;
    }

    /* 2. Crear instancia del Grafo */
    Graph *graph = graph_create(totalDelegates);
    if (graph == NULL) {
        fclose(file);
        return PARSE_ERROR_MEMORY_FAILURE;
    }

    /* 3. Leer las m aristas de incompatibilidad */
    int edgesRead = 0;
    while (edgesRead < totalIncompatibilities && fgets(lineBuffer, sizeof(lineBuffer), file) != NULL) {
        char *trimmed = trim_whitespace(lineBuffer);
        if (strlen(trimmed) == 0 || trimmed[0] == '#') {
            continue;
        }

        int delegateA = 0;
        int delegateB = 0;
        int parsedItems = 0;

        /* Intentar parseo formato estándar: (u,v) o (u, v) */
        char *openParen = strchr(trimmed, '(');
        char *comma = strchr(trimmed, ',');
        char *closeParen = strchr(trimmed, ')');

        if (openParen && comma && closeParen && (openParen < comma) && (comma < closeParen)) {
            parsedItems = sscanf(openParen, "(%d,%d)", &delegateA, &delegateB);
            if (parsedItems != 2) {
                parsedItems = sscanf(openParen, "(%d , %d)", &delegateA, &delegateB);
            }
        } else {
            /* Fallback: formato numérico directo "u v" */
            parsedItems = sscanf(trimmed, "%d %d", &delegateA, &delegateB);
        }

        if (parsedItems != 2) {
            graph_destroy(graph);
            fclose(file);
            return PARSE_ERROR_INVALID_EDGE_FORMAT;
        }

        if (delegateA < 1 || delegateA > totalDelegates || delegateB < 1 || delegateB > totalDelegates) {
            graph_destroy(graph);
            fclose(file);
            return PARSE_ERROR_OUT_OF_BOUNDS;
        }

        graph_add_incompatibility(graph, delegateA, delegateB);
        edgesRead++;
    }

    fclose(file);

    graph_compute_metrics(graph);
    *outGraph = graph;
    return PARSE_SUCCESS;
}

const char* file_parser_get_error_message(FileParseStatus status) {
    switch (status) {
        case PARSE_SUCCESS:
            return "Archivo cargado y procesado exitosamente.";
        case PARSE_ERROR_FILE_NOT_FOUND:
            return "No se pudo abrir el archivo especificado. Verifique la ruta y el nombre.";
        case PARSE_ERROR_INVALID_HEADER:
            return "Encabezado del archivo inválido. Se esperaba 'n m'.";
        case PARSE_ERROR_INVALID_DIMENSIONS:
            return "Dimensiones inválidas (el número de delegados o incompatibilidades no es válido).";
        case PARSE_ERROR_INVALID_EDGE_FORMAT:
            return "Formato de incompatibilidad inválido. Se esperaba '(u,v)'.";
        case PARSE_ERROR_OUT_OF_BOUNDS:
            return "Identificador de delegado fuera del rango válido [1, n].";
        case PARSE_ERROR_MEMORY_FAILURE:
            return "Error de memoria al inicializar las estructuras del grafo.";
        default:
            return "Error desconocido durante el procesamiento del archivo.";
    }
}
