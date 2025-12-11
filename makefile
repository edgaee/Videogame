# Directorios de origen y destino
SRC_DIR := src
BIN_DIR := bin

SFML := -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lbox2d

# Archivos fuente del juego principal
GAME_SRC := $(SRC_DIR)/main.cpp $(SRC_DIR)/Game.cpp $(SRC_DIR)/Player.cpp $(SRC_DIR)/Level.cpp $(SRC_DIR)/Platform.cpp $(SRC_DIR)/HidingSpot.cpp $(SRC_DIR)/Enemy.cpp $(SRC_DIR)/HUD.cpp

# Obtener todos los archivos .cpp para ejemplos (excluyendo los del juego)
ALL_CPP := $(wildcard $(SRC_DIR)/*.cpp)
EXAMPLE_FILES := $(filter-out $(GAME_SRC), $(ALL_CPP))

# Generar los nombres de los archivos .exe para ejemplos
EXAMPLE_EXES := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.exe,$(EXAMPLE_FILES))

# Regla por defecto: compilar ejemplos y el juego
all: examples game

# Regla para ejemplos
examples: $(EXAMPLE_EXES)

$(BIN_DIR)/%.exe: $(SRC_DIR)/%.cpp
	g++ $< -o $@ $(SFML) -Iinclude

# Regla para el juego
game:
	g++ $(GAME_SRC) -o $(BIN_DIR)/DexterGame.exe $(SFML) -Iinclude

# Ejecutar juego
run-game: game
	./$(BIN_DIR)/DexterGame.exe

# Regla para ejecutar ejemplos (run01_Imagen, etc)
run%: $(BIN_DIR)/%.exe
	./$<

clean:
	rm -f $(BIN_DIR)/*.exe

.PHONY: all clean examples game run-game
