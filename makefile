CC = gcc
CFLAGS = -Wall -I./src
LDFLAGS = -lm
TARGET = raytracer_app.exe
SOURCE = main.c

LUA_SOURCES = $(wildcard src/*.c)

all: $(TARGET)

$(TARGET): $(SOURCE) $(LUA_SOURCES)
	@echo Compilando o motor C e o Lua a partir do codigo fonte...
	$(CC) $(SOURCE) $(LUA_SOURCES) $(CFLAGS) -o $(TARGET) $(LDFLAGS)
	@echo Compilacao concluida com sucesso!

run: $(TARGET)
	./$(TARGET)

clean:
	@echo Limpando arquivos gerados...
	del $(TARGET) resultado_esfera.ppm