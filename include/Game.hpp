#pragma once

#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Platform.hpp"
#include "HidingSpot.hpp"
#include "Config.hpp"
#include <vector>

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

    // Actualizar información de debug (FPS, posición del jugador)
    void updateDebugInfo(sf::Time deltaTime);

    // Miembros privados
    sf::RenderWindow mWindow; // La ventana de SFML
    sf::View mWorldView;      // Cámara del juego
    Player mPlayer;           // Instancia del jugador (Dexter)
    std::vector<Platform> mPlatforms; // Plataformas del nivel
    std::vector<HidingSpot> mHidingSpots; // Escondites
    
    // Debug
    sf::Font mDebugFont;
    sf::Text mFpsText;
    sf::Text mPositionText;
    sf::Text mStateText;
    sf::Clock mFrameClock;
    int mFrameCount;
    float mFpsUpdateTimer;

    // Pantalla de Inicio
    bool mIsGameStarted;
    sf::Texture mStartScreenTexture;
    sf::Sprite mStartScreenSprite;
    sf::Text mTitleText;
    sf::Text mPressEnterText;
};
