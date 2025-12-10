#include "Game.hpp"
#include <sstream>
#include <iomanip>

// Constructor
Game::Game() 
    : mWindow(sf::VideoMode(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT), Config::WINDOW_TITLE),
      mFrameCount(0),
      mFpsUpdateTimer(0.f),
      mIsGameStarted(false)
{
    // Limitar el framerate a 60 FPS para consistencia
    mWindow.setFramerateLimit(Config::FPS_LIMIT);

    // Configurar Pantalla de Inicio
    // Cargar imagen dexter_jeringa.png
    if (mStartScreenTexture.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_jeringa.png")) {
        mStartScreenSprite.setTexture(mStartScreenTexture);
        
        // Centrar el sprite en la pantalla
        sf::FloatRect bounds = mStartScreenSprite.getLocalBounds();
        mStartScreenSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        mStartScreenSprite.setPosition(Config::WINDOW_WIDTH / 2.f, Config::WINDOW_HEIGHT / 2.f);
    }

    // Cargar fuente para debug y título
    if (!mDebugFont.loadFromFile(Config::DEBUG_FONT)) {
        // Si falla la fuente, continuamos sin debug visual
    } else {
        // Configurar textos de debug
        if (Config::DEBUG_MODE) {
            mFpsText.setFont(mDebugFont);
            mFpsText.setCharacterSize(16);
            mFpsText.setFillColor(sf::Color::Green);
            mFpsText.setPosition(10.f, 10.f);

            mStateText.setFont(mDebugFont);
            mStateText.setCharacterSize(16);
            mStateText.setFillColor(sf::Color::Cyan);
            mStateText.setPosition(10.f, 35.f);
        }
        
        // Configurar texto de título "Dexter Game" para pantalla de inicio
        mTitleText.setFont(mDebugFont);
        mTitleText.setString("Dexter Game");
        mTitleText.setCharacterSize(80);
        mTitleText.setFillColor(sf::Color::White);
        
        // Centrar texto en la parte superior
        sf::FloatRect textBounds = mTitleText.getLocalBounds();
        mTitleText.setOrigin(textBounds.width / 2.f, 0.f);
        mTitleText.setPosition(Config::WINDOW_WIDTH / 2.f, 50.f);
        
        // Configurar texto de instrucción "Presiona ENTER para comenzar"
        mPressEnterText.setFont(mDebugFont);
        mPressEnterText.setString("Presiona ENTER para comenzar");
        mPressEnterText.setCharacterSize(30);
        mPressEnterText.setFillColor(sf::Color::Yellow);
        
        // Centrar texto de instrucción en la parte inferior
        sf::FloatRect enterBounds = mPressEnterText.getLocalBounds();
        mPressEnterText.setOrigin(enterBounds.width / 2.f, 0.f);
        mPressEnterText.setPosition(Config::WINDOW_WIDTH / 2.f, Config::WINDOW_HEIGHT - 150.f);
    }

    // Inicializar Plataformas
    
    // 1. Suelo largo abajo (Base) - Coincide con el suelo del jugador (Y=1000)
    mPlatforms.emplace_back(nullptr, sf::Vector2f(2560.f, 50.f), sf::Vector2f(0.f, 1000.f));

    // 2. Plataforma izquierda (Muy baja, casi a nivel de suelo)
    mPlatforms.emplace_back(nullptr, sf::Vector2f(200.f, 30.f), sf::Vector2f(150.f, 920.f));
    
    // 3. Plataforma derecha (Un poco más alta)
    mPlatforms.emplace_back(nullptr, sf::Vector2f(200.f, 30.f), sf::Vector2f(450.f, 780.f));

    // 4. Pared de prueba (Bloque alto en el suelo a la derecha)
    mPlatforms.emplace_back(nullptr, sf::Vector2f(50.f, 200.f), sf::Vector2f(800.f, 800.f));
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
        
        // Detectar ENTER para iniciar el juego desde la pantalla de inicio
        if (!mIsGameStarted && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
            mIsGameStarted = true;
        }
    }
}

// Actualización de lógica
void Game::update(sf::Time deltaTime) {
    // Solo actualizar el jugador si el juego ha comenzado
    if (mIsGameStarted) {
        mPlayer.update(deltaTime, mPlatforms);
    }
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

    if (!mIsGameStarted) {
        // Pantalla de Inicio
        // Dibujar imagen de dexter_jeringa centrada
        mWindow.draw(mStartScreenSprite);
        
        // Dibujar título "Dexter Game"
        mWindow.draw(mTitleText);
        
        // Dibujar texto "Presiona ENTER para comenzar"
        mWindow.draw(mPressEnterText);
    } else {
        // 2. Dibujar objetos (Aquí dibujaremos al Player, mapa, etc.)
        for (auto& platform : mPlatforms) {
            platform.draw(mWindow);
        }
        mPlayer.draw(mWindow);

        // 3. Dibujar información de debug
        if (Config::DEBUG_MODE) {
            mWindow.draw(mFpsText);
            mWindow.draw(mStateText);
        }
    }

    // 4. Mostrar lo dibujado en pantalla
    mWindow.display();
}
