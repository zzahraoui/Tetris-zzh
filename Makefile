# Makefile pour Windows PowerShell avec SDL2 de MSYS2
CC = C:/msys64/mingw64/bin/gcc.exe
CFLAGS = -Wall -Wextra -g -std=c99 -Iinclude -IC:/msys64/mingw64/include -IC:/msys64/mingw64/include/SDL2
LDFLAGS = -LC:/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf

SRC_DIR = src
OBJ_DIR = obj

# Liste explicite de tous les fichiers
SOURCES = $(SRC_DIR)/list.c $(SRC_DIR)/pieces.c $(SRC_DIR)/game.c $(SRC_DIR)/render.c $(SRC_DIR)/main.c
OBJECTS = $(OBJ_DIR)/list.o $(OBJ_DIR)/pieces.o $(OBJ_DIR)/game.o $(OBJ_DIR)/render.o $(OBJ_DIR)/main.o

TARGET = tetris.exe

all: $(TARGET)
	@echo Compilation terminee!

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@if not exist SDL2.dll copy C:\msys64\mingw64\bin\SDL2.dll . 2>nul
	@if not exist SDL2_ttf.dll copy C:\msys64\mingw64\bin\SDL2_ttf.dll . 2>nul

$(OBJ_DIR)/list.o: $(SRC_DIR)/list.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/list.c -o $(OBJ_DIR)/list.o

$(OBJ_DIR)/pieces.o: $(SRC_DIR)/pieces.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/pieces.c -o $(OBJ_DIR)/pieces.o

$(OBJ_DIR)/game.o: $(SRC_DIR)/game.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/game.c -o $(OBJ_DIR)/game.o

$(OBJ_DIR)/render.o: $(SRC_DIR)/render.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/render.c -o $(OBJ_DIR)/render.o

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

$(OBJ_DIR):
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

clean:
	@if exist $(OBJ_DIR) rmdir /S /Q $(OBJ_DIR)
	@if exist $(TARGET) del /F $(TARGET)
	@if exist SDL2.dll del /F SDL2.dll
	@if exist SDL2_ttf.dll del /F SDL2_ttf.dll

run: $(TARGET)
	$(TARGET)

.PHONY: all clean run