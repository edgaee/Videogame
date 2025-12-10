#pragma once
#include <SFML/Graphics.hpp>
#include "Config.hpp"

// Estados del jugador
enum class PlayerState {
    IDLE,           // Quieto
    RUNNING,        // Corriendo
    CROUCHING,      // Agachado quieto
    CROUCH_WALKING, // Caminando agachado
    PRE_JUMP,       // Preparación salto
    JUMPING,        // En el aire
    LANDING         // Aterrizaje
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
    // Métodos internos
    void handleInput(float dt);
    void updatePhysics(float dt);
    void updateAnimation(float dt);
    void updateOrigin();
    void clampToBounds();
    void setState(PlayerState newState, bool force = false);
    float getTextureScale(const sf::Texture* texture) const;
    void updateTextureForCurrentState();

    sf::Sprite mSprite;
    
    // Texturas Normales
    sf::Texture mTextureIdle;
    sf::Texture mTextureRun1, mTextureRun2;
    sf::Texture mTextureCrouch; // Agachado quieto
    sf::Texture mTextureCrouchWalk1, mTextureCrouchWalk2; // Agachado caminando
    sf::Texture mTexturePreJump, mTextureJump, mTextureLanding; // Salto

    // Texturas Jeringa
    sf::Texture mTextureSyringeIdle, mTextureSyringeIdleStatic;
    sf::Texture mTextureSyringeRun1, mTextureSyringeRun2;
    sf::Texture mTextureSyringeCrouch;
    sf::Texture mTextureSyringeCrouchWalk1, mTextureSyringeCrouchWalk2;
    
    // Texturas Ataque
    sf::Texture mTextureSyringeAttack; // Un solo frame de ataque

    // Variables de Física
    sf::Vector2f mVelocity;
    float mSpeed;
    float mGravity;
    float mJumpForce;
    float mGroundY;

    // Variables de Estado y Animación
    PlayerState mCurrentState;
    float mAnimationTimer;
    bool mIsRunFrame1;
    bool mFacingLeft;
    bool mPreviousFacingLeft;
    
    // Timers para salto
    float mPreJumpTimer;
    float mLandingTimer;

    // Variables de Jeringa
    bool mHasSyringe;
    bool mIsInSyringeAttack;
    int mSyringeAttackFrame;
    PlayerState mPreviousStateBeforeAttack;
    bool mShouldDisableSyringeAfterAttack;
};



