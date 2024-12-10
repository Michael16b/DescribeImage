# Nom de l'exécutable
TARGET = editor

# Fichier source
SRC = editor.c

# Compilateur et options
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lSDL2 -lSDL2_image

# Règle par défaut : compile l'exécutable
all: $(TARGET)

# Règle pour générer l'exécutable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Nettoyage des fichiers temporaires
clean:
	rm -f $(TARGET)

# Récompilation complète
rebuild: clean all

