/**
 * @file file_parser.h
 * @brief Módulo de lectura, análisis sintáctico y validación de archivos de convención.
 * @author Brayan Ceballos (bracebalDev)
 * @date 2020 (Original) - 2026 (Refactorización de Portafolio)
 * 
 * Procesa archivos planos con el formato especificado por la DAGGC / ONU:
 * Línea 1: n m (n: total delegados, m: total incompatibilidades)
 * Líneas siguientes: (u,v) donde u y v son delegados incompatibles.
 */

#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include "graph.h"
#include <stdbool.h>

/**
 * @enum FileParseStatus
 * @brief Códigos de estado del parser de archivos.
 */
typedef enum {
    PARSE_SUCCESS = 0,             /**< Archivo leído y procesado exitosamente */
    PARSE_ERROR_FILE_NOT_FOUND,    /**< No se pudo abrir el archivo especificado */
    PARSE_ERROR_INVALID_HEADER,    /**< Encabezado 'n m' inválido o corrupto */
    PARSE_ERROR_INVALID_DIMENSIONS,/**< Valores de n o m fuera de límites razonables */
    PARSE_ERROR_INVALID_EDGE_FORMAT,/**< Formato de arista '(u,v)' incorrecto */
    PARSE_ERROR_OUT_OF_BOUNDS,     /**< Identificador de delegado fuera del rango [1, n] */
    PARSE_ERROR_MEMORY_FAILURE     /**< Fallo de reserva de memoria durante la carga */
} FileParseStatus;

/**
 * @brief Carga y construye una instancia de Grafo a partir de un archivo plano.
 * 
 * Soporta formatos flexibles como "(u,v)", "(u, v)", "u v" y líneas con espacios en blanco.
 * 
 * @param filename Ruta o nombre del archivo de entrada (ej: "Convencion.in").
 * @param outGraph Puntero doble donde se retornará el grafo construido.
 * @return FileParseStatus Código de estado del procesamiento.
 */
FileParseStatus file_parser_load_convention(const char *filename, Graph **outGraph);

/**
 * @brief Convierte un código de estado de parseo a un mensaje descriptivo para el usuario.
 * 
 * @param status Código de estado FileParseStatus.
 * @return const char* Mensaje en español legible.
 */
const char* file_parser_get_error_message(FileParseStatus status);

#endif /* FILE_PARSER_H */
