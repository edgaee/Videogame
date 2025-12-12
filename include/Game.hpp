#pragma once

#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Level.hpp"
#include "Platform.hpp"
#include "HidingSpot.hpp"
#include "Config.hpp"
#include "HUD.hpp"
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
    
    // Fondo
    sf::Texture mBgTexture;
    sf::Sprite mBgSprite;

    Player mPlayer;           // Instancia del jugador (Dexter)
    Level mLevel;             // Nivel actual
    HUD mHUD;                 // Interfaz de usuario
    
    // Debug
    sf::Font mDebugFont;
    sf::Text mFpsText;
    sf::Text mPositionText;
    sf::Text mStateText;
    sf::Clock mFrameClock;
    int mFrameCount;
    float mFpsUpdateTimer;

    // Audio
    sf::Music mBgMusic;

    // Pantalla de Inicio
    bool mIsGameStarted;
    sf::Texture mStartScreenTexture;
    sf::Sprite mStartScreenSprite;
    sf::Texture mStartScreenTexture2;
    sf::Sprite mStartScreenSprite2;
    sf::Text mTitleText;
    sf::Text mPressEnterText;
    
    // Pantalla de Game Over
    bool mIsGameOver;
    float mGameOverTimer; // Timer para retrasar la pantalla de Game Over
    sf::Text mGameOverText;
    sf::Text mRestartText;
    
    // Pantalla de Victoria
    bool mIsVictory;
    float mVictoryTimer; // Timer para retrasar la pantalla de Victoria
    bool mBossDefeatedPending; // Flag para saber si el boss fue derrotado pero aún no mostramos victoria
    sf::Text mVictoryText;
    sf::Text mVictoryStatsText;
};
