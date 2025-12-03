#pragma once

#include <SFML/Graphics.hpp>

/**
 * Clase Game
 * ----------
 * Esta clase actúa como el motor central del juego.
 * Encapsula la ventana, el bucle principal y gestiona
 * los eventos, la lógica y el renderizado.
 */
class Game {
public:
    // Constructor: Inicializa la ventana y configuraciones
    Game();

    // Destructor: Limpieza de recursos si fuera necesario
    ~Game();

    // Método principal que inicia el bucle del juego
    void run();

private:
    // Procesar eventos de usuario (teclado, mouse, cierre de ventana)
    void processEvents();

    // Actualizar la lógica del juego (movimiento, colisiones, IA)
    // sf::Time deltaTime: Tiempo transcurrido desde el último frame
    void update(sf::Time deltaTime);

    // Dibujar todos los objetos en la ventana
    void render();

    // Miembros privados
    sf::RenderWindow mWindow; // La ventana de SFML
    
    // Aquí agregaremos más adelante al Player, Enemy, etc.
};
