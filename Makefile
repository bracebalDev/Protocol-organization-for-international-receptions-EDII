# ==============================================================================
# Makefile - Sistema de Organización de Protocolo para Recepciones Internacionales
# Autor: Brayan Ceballos (bracebalDev)
# Asignatura: Elementos Discretos II - Universidad de Carabobo
# ==============================================================================

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -O2 -Iinclude
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
TARGET = $(BIN_DIR)/protocol_organizer
STANDALONE_TARGET = $(BIN_DIR)/protocol_organizer_standalone

.PHONY: all clean run standalone dirs test

all: dirs $(TARGET) standalone

dirs:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
	@echo "Build complete: $(TARGET)"

standalone: dirs
	$(CC) $(CFLAGS) protocol_organizer.c -o $(STANDALONE_TARGET)
	@echo "Standalone build complete: $(STANDALONE_TARGET)"

run: all
	./$(TARGET) Convencion.in

test: all
	@echo "Ejecutando pruebas con dataset oficial..."
	./$(TARGET) Convencion.in

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete."
