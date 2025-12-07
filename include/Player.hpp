#pragma once
#include <SFML/Graphics.hpp>
#include "Config.hpp"

// Estados del jugador
enum class PlayerState {
    IDLE,       // Quieto
    RUNNING,    // Corriendo
    CROUCHING   // Agachado
};

class Player {
public:
    Player();
    
    // Actualiza la posición del jugador basado en inputs y tiempo
    void update(sf::Time deltaTime);
    
    // Dibuja el sprite del jugador en la ventana
    void draw(sf::RenderWindow& window);

    // Obtiene la posición actual del jugador
    sf::Vector2f getPosition() const;

    // Obtiene el estado actual del jugador
    PlayerState getState() const;

private:
    // Método privado para manejar la animación
    void animate(float dt);

    // Método privado para actualizar el origen del sprite
    void updateOrigin();

    // Método privado para mantener al jugador dentro de los límites de la pantalla
    void clampToBounds();

    // Método para cambiar de estado
    void setState(PlayerState newState);

    // Método para obtener el offset Y de una textura (para alineación vertical)
    float getTextureYOffset(const sf::Texture* texture) const;

    // Método para obtener la escala necesaria para igualar alturas
    float getTextureScale(const sf::Texture* texture) const;

    // Método para actualizar la textura sin cambiar de estado
    // (usado cuando se activa/desactiva la jeringa en IDLE)
    void updateTextureForCurrentState();

    sf::Sprite mSprite;
    
    // Texturas para movimiento normal
    sf::Texture mTextureIdle;
    sf::Texture mTextureRun1;
    sf::Texture mTextureRun2;
    
    // Texturas para agacharse
    sf::Texture mTextureCrouch;
    sf::Texture mTextureCrouch1;
    sf::Texture mTextureCrouch2;
    
    // Texturas para jeringa
    sf::Texture mTextureSyringeIdle;    // dexter_default.png (mismo que IDLE normal)
    sf::Texture mTextureSyringeIdleStatic; // dexter_jeringa.png (cuando está quieto CON jeringa)
    sf::Texture mTextureSyringeRun1;
    sf::Texture mTextureSyringeRun2;
    sf::Texture mTextureSyringeCrouch;

    float mSpeed; // Velocidad de movimiento en píxeles por segundo
    
    // Variables de animación
    float mAnimationTimer; // Acumulador de tiempo para cambiar frames
    bool mIsRunFrame1;     // Estado para alternar entre paso1 y paso2
    
    // Estado del jugador
    PlayerState mCurrentState;
    bool mFacingLeft;      // Dirección a la que mira
    bool mPreviousFacingLeft; // Dirección anterior (para detectar cambios)
    bool mHasSyringe;      // Si Dexter tiene la jeringa activa
};

