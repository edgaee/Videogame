#include "Game.hpp"
#include <sstream>
#include <iomanip>
#include <cmath> 

// Constructor
Game::Game() 
    : mWindow(sf::VideoMode(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT), Config::WINDOW_TITLE),
      mFrameCount(0),
      mFpsUpdateTimer(0.f),
      mIsGameStarted(false),
      mIsGameOver(false),
      mGameOverTimer(0.f)
{
    // Limitar el framerate a 60 FPS para consistencia
    mWindow.setFramerateLimit(Config::FPS_LIMIT);

    // Cargar Fondo
    if (!mBgTexture.loadFromFile(std::string(Config::IMAGE_PATH) + "city_sunset.png")) {
        // Fallback
    }
    mBgSprite.setTexture(mBgTexture);

    // Inicializar Vista (Cámara)
    mWorldView = mWindow.getDefaultView();

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

            mPositionText.setFont(mDebugFont);
            mPositionText.setCharacterSize(16);
            mPositionText.setFillColor(sf::Color::Yellow);
            mPositionText.setPosition(10.f, 40.f);

            mStateText.setFont(mDebugFont);
            mStateText.setCharacterSize(16);
            mStateText.setFillColor(sf::Color::Cyan);
            mStateText.setPosition(10.f, 70.f);
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
        
        // Configurar texto de Game Over
        mGameOverText.setFont(mDebugFont);
        mGameOverText.setString("GAME OVER");
        mGameOverText.setCharacterSize(100);
        mGameOverText.setFillColor(sf::Color::Red);
        mGameOverText.setOutlineColor(sf::Color::Black);
        mGameOverText.setOutlineThickness(3.f);
        
        sf::FloatRect gameOverBounds = mGameOverText.getLocalBounds();
        mGameOverText.setOrigin(gameOverBounds.width / 2.f, gameOverBounds.height / 2.f);
        mGameOverText.setPosition(Config::WINDOW_WIDTH / 2.f, Config::WINDOW_HEIGHT / 2.f - 50.f);
        
        // Configurar texto de reinicio
        mRestartText.setFont(mDebugFont);
        mRestartText.setString("Dexter ha sido detectado!\nPresiona R para reiniciar");
        mRestartText.setCharacterSize(30);
        mRestartText.setFillColor(sf::Color::White);
        
        sf::FloatRect restartBounds = mRestartText.getLocalBounds();
        mRestartText.setOrigin(restartBounds.width / 2.f, 0.f);
        mRestartText.setPosition(Config::WINDOW_WIDTH / 2.f, Config::WINDOW_HEIGHT / 2.f + 50.f);
    }

    // Cargar Nivel
    mLevel.loadLevel1();

    // Cargar Música de Fondo
    if (!mBgMusic.openFromFile(std::string(Config::ASSET_PATH) + "audio/stealth.wav")) {
        // Error handling
    } else {
        mBgMusic.setLoop(true);
        mBgMusic.setVolume(50.f);
        // No reproducir todavía, esperar a start game
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
        
        // Detectar ENTER para iniciar el juego desde la pantalla de inicio
        if (!mIsGameStarted && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return) {
            mIsGameStarted = true;
            if (mBgMusic.getStatus() != sf::Music::Playing) {
                mBgMusic.play();
            }
        }
        
        // Detectar R para reiniciar después de Game Over
        if (mIsGameOver && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
            // Reiniciar el juego
            mIsGameOver = false;
            mPlayer = Player(); // Resetear jugador completo
            mLevel.loadLevel1(); // Resetear nivel
            if (mBgMusic.getStatus() != sf::Music::Playing) {
                mBgMusic.play();
            }
            mPlayer.setPosition(Config::PLAYER_INITIAL_X, Config::PLAYER_INITIAL_Y);
            mPlayer.setVelocity(0.f, 0.f);
            mPlayer.setHidden(false);
            mLevel.loadLevel1(); // Recargar el nivel (resetea enemigos)
        }

        // Detectar 'E' para esconderse
        if (mIsGameStarted && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
            if (mPlayer.isHidden()) {
                mPlayer.setHidden(false);
            } else {
                // Verificar si está en un escondite (Consultar al Nivel)
                for (const auto& spot : mLevel.getHidingSpots()) {
                    if (spot.isPlayerInside(mPlayer)) {
                        mPlayer.setHidden(true);
                        break;
                    }
                }
            }
        }
    }
}

// Actualización de lógica
void Game::update(sf::Time deltaTime) {
    // No actualizar si estamos en Game Over
    if (mIsGameOver) return;
    
    // Solo actualizar el jugador si el juego ha comenzado
    if (mIsGameStarted) {
        float dt = deltaTime.asSeconds();
        
        // 1. Actualizar Player (Input, Animación, Gravedad)
        mPlayer.update(deltaTime);
        
        // 2. Actualizar Nivel (Entidades y Enemigos)
        mLevel.update(dt, mPlayer);
        
        // 3. Verificar Game Over (Vidas)
        if (mPlayer.isDead()) {
            // Si acaba de morir, detener música
            if (mBgMusic.getStatus() == sf::Music::Playing) {
                mBgMusic.stop();
            }
            
            // Incrementar timer para mostrar animación de muerte antes del Game Over
            mGameOverTimer += dt;
            if (mGameOverTimer > 2.0f) { // Esperar 2 segundos
                mIsGameOver = true;
            }
            // No retornamos inmediatamente para permitir que se dibuje el sprite de muerte
            // Pero evitamos colisiones y movimiento extra en Player::update (ya manejado por estado DEAD)
        } else {
            mGameOverTimer = 0.f;
        }
        
        // 4. Resolver Colisiones (Nivel mueve al Player)
        // Solo si no está muerto (para evitar que se mueva el cadáver si hay lógica extra)
        if (!mPlayer.isDead()) {
            mLevel.checkCollisions(mPlayer, dt);
        }

        // Actualizar Cámara (Lerp)
        sf::Vector2f playerPos = mPlayer.getPosition();
        sf::Vector2f currentCenter = mWorldView.getCenter();
        
        // Factor de suavizado (0.1 es suave, 1.0 es instantáneo)
        float smoothFactor = 5.0f * deltaTime.asSeconds(); 
        
        // Lerp manual
        float newX = currentCenter.x + (playerPos.x - currentCenter.x) * smoothFactor;
        float newY = currentCenter.y + (playerPos.y - currentCenter.y) * smoothFactor;
        
        // Clamp para no ver fuera del mundo (opcional, pero recomendado)
        // Asumiendo mundo de 0 a WORLD_WIDTH en X y 0 a WINDOW_HEIGHT en Y
        float viewHalfWidth = mWorldView.getSize().x / 2.f;
        float viewHalfHeight = mWorldView.getSize().y / 2.f;
        
        if (newX - viewHalfWidth < 0) newX = viewHalfWidth;
        if (newX + viewHalfWidth > Config::WORLD_WIDTH) newX = Config::WORLD_WIDTH - viewHalfWidth;
        if (newY - viewHalfHeight < 0) newY = viewHalfHeight;
        if (newY + viewHalfHeight > Config::WINDOW_HEIGHT) newY = Config::WINDOW_HEIGHT - viewHalfHeight;

        mWorldView.setCenter(newX, newY);
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
    std::ostringstream posOss;
    posOss << "Pos: (" << (int)mPlayer.getPosition().x << ", " << (int)mPlayer.getPosition().y << ")";
    mPositionText.setString(posOss.str());
    mPositionText.setPosition(10.f, 40.f); // Debajo de FPS

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
        case PlayerState::HIDING:
            stateStr += "ESCONDIDO";
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
        // Resetear vista para UI
        mWindow.setView(mWindow.getDefaultView());

        // Dibujar imagen de dexter_jeringa centrada
        mWindow.draw(mStartScreenSprite);
        
        // Dibujar título "Dexter Game"
        mWindow.draw(mTitleText);
        
        // Dibujar texto "Presiona ENTER para comenzar"
        mWindow.draw(mPressEnterText);
    } else if (mIsGameOver) {
        // Pantalla de Game Over
        // Aplicar cámara del juego para ver el nivel de fondo
        mWindow.setView(mWorldView);
        
        // Dibujar el nivel de fondo (oscurecido)
        mLevel.draw(mWindow);
        mPlayer.draw(mWindow);
        
        // Overlay oscuro
        mWindow.setView(mWindow.getDefaultView());
        sf::RectangleShape overlay(sf::Vector2f(Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        mWindow.draw(overlay);
        
        // Dibujar textos de Game Over
        mWindow.draw(mGameOverText);
        mWindow.draw(mRestartText);
    } else {
        // Aplicar cámara del juego
        mWindow.setView(mWorldView);

        // 0. Dibujar Fondo (Parallax simple: fijo con la cámara o movimiento lento)
        // Para que sea fijo (skybox), dibujamos con la vista por defecto o ajustamos posición
        // Opción A: Fondo estático (siempre cubre la pantalla)
        sf::View currentView = mWindow.getView();
        mWindow.setView(mWindow.getDefaultView());
        mWindow.draw(mBgSprite);
        mWindow.setView(currentView);

        // 2. Dibujar objetos (Delegar al Nivel)
        mLevel.draw(mWindow);
        
        // Dibujar Player
        mPlayer.draw(mWindow);

        // 3. Dibujar HUD (Interfaz)
        mHUD.draw(mWindow, mPlayer.getLives());
        
        if (Config::DEBUG_MODE) {
            // Dibujar UI de debug (sobre todo lo demás)
            // Usar defaultView para que el texto quede fijo en pantalla
            mWindow.setView(mWindow.getDefaultView());
            mWindow.draw(mFpsText);
            mWindow.draw(mPositionText);
            mWindow.draw(mStateText);
        }
    }

    mWindow.display();
}
