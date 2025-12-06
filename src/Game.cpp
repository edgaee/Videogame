#include "Game.hpp"
#include <sstream>
#include <iomanip>

// Constructor
Game::Game() 
    : mWindow(sf::VideoMode(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT), Config::WINDOW_TITLE),
      mFrameCount(0),
      mFpsUpdateTimer(0.f)
{
    // Limitar el framerate a 60 FPS para consistencia
    mWindow.setFramerateLimit(Config::FPS_LIMIT);

    // Cargar fuente para debug
    if (Config::DEBUG_MODE) {
        if (!mDebugFont.loadFromFile(Config::DEBUG_FONT)) {
            // Si falla la fuente, continuamos sin debug visual
        } else {
            mFpsText.setFont(mDebugFont);
            mFpsText.setCharacterSize(16);
            mFpsText.setFillColor(sf::Color::Green);
            mFpsText.setPosition(10.f, 10.f);

            mStateText.setFont(mDebugFont);
            mStateText.setCharacterSize(16);
            mStateText.setFillColor(sf::Color::Cyan);
            mStateText.setPosition(10.f, 35.f);
        }
    }
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
        updateDebugInfo(deltaTime);
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
    // Actualizar al jugador
    mPlayer.update(deltaTime);
}

// Actualizar información de debug
void Game::updateDebugInfo(sf::Time deltaTime) {
    if (!Config::DEBUG_MODE) return;

    mFrameCount++;
    mFpsUpdateTimer += deltaTime.asSeconds();

    // Actualizar FPS cada 0.5 segundos
    if (mFpsUpdateTimer >= 0.5f) {
        float fps = mFrameCount / mFpsUpdateTimer;
        std::ostringstream oss;
        oss << "FPS: " << std::fixed << std::setprecision(1) << fps;
        mFpsText.setString(oss.str());

        mFrameCount = 0;
        mFpsUpdateTimer = 0.f;
    }

    // Actualizar información de debug
    // (La posición XY ha sido removida por solicitud del usuario)

    // Actualizar estado del jugador
    std::string stateStr = "Estado: ";
    switch (mPlayer.getState()) {
        case PlayerState::IDLE:
            stateStr += "QUIETO";
            break;
        case PlayerState::RUNNING:
            stateStr += "CORRIENDO";
            break;
        case PlayerState::CROUCHING:
            stateStr += "AGACHADO";
            break;
    }
    mStateText.setString(stateStr);
}

// Renderizado (Dibujado)
void Game::render() {
    // 1. Limpiar la ventana con un color (negro por defecto)
    mWindow.clear(Config::BACKGROUND_COLOR);

    // 2. Dibujar objetos (Aquí dibujaremos al Player, mapa, etc.)
    mPlayer.draw(mWindow);

    // 3. Dibujar información de debug
    if (Config::DEBUG_MODE) {
        mWindow.draw(mFpsText);
        mWindow.draw(mStateText);
    }

    // 4. Mostrar lo dibujado en pantalla
    mWindow.display();
}
