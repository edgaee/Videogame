#include "Game.hpp"

/**
 * Punto de entrada principal (Main)
 * ---------------------------------
 * Su única responsabilidad es instanciar la clase Game
 * y ponerla en marcha. Mantiene el main limpio y ordenado.
 */
int main() {
    // Crear la instancia del juego
    Game game;

    // Iniciar el bucle principal
    game.run();

    return 0;
}
