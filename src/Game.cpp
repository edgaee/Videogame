#include "Game.hpp"

// Constructor
Game::Game() 
    : mWindow(sf::VideoMode(1280, 720), "Dexter: Dark Passenger") 
{
    // Limitar el framerate a 60 FPS para consistencia
    mWindow.setFramerateLimit(60);
}

// Destructor
Game::~Game() {
    // Nada que limpiar manualmente por ahora, SFML maneja la ventana
}

// Bucle principal del juego
void Game::run() {
    sf::Clock clock; // Reloj para medir el tiempo entre frames (delta time)

    while (mWindow.isOpen()) {
        // Medir el tiempo transcurrido desde la última vuelta del bucle
        sf::Time deltaTime = clock.restart();

        processEvents();
        update(deltaTime);
        render();
    }
}

// Procesamiento de eventos (Inputs)
void Game::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        // Cerrar la ventana si se presiona el botón de cerrar o Escape
        if (event.type == sf::Event::Closed) {
            mWindow.close();
        }
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            mWindow.close();
        }
    }
}

// Actualización de lógica
void Game::update(sf::Time deltaTime) {
    // Aquí actualizaremos al jugador, enemigos, física, etc.
    // Por ahora está vacío.
}

// Renderizado (Dibujado)
void Game::render() {
    // 1. Limpiar la ventana con un color (negro por defecto)
    mWindow.clear(sf::Color::Black);

    // 2. Dibujar objetos (Aquí dibujaremos al Player, mapa, etc.)
    // mWindow.draw(playerSprite); 

    // 3. Mostrar lo dibujado en pantalla
    mWindow.display();
}
