#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Config.hpp"
#include <vector>

class Platform; // Forward declaration

// Estados del jugador
enum class PlayerState {
    IDLE,           // Quieto
    RUNNING,        // Corriendo
    CROUCHING,      // Agachado quieto
    CROUCH_WALKING, // Caminando agachado
    PRE_JUMP,       // Preparación salto
    JUMPING,        // En el aire
    LANDING,        // Aterrizaje
    HIDING,         // Escondido
    DEAD            // Muerto
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
    void setPosition(float x, float y);
    
    // Obtiene los límites del sprite
    sf::FloatRect getBounds() const;

    // Física
    sf::Vector2f getVelocity() const;
    void setVelocity(float x, float y);
    void move(float x, float y);
    void setGrounded(bool grounded);

    // Obtiene el estado actual del jugador
    PlayerState getState() const;
    
    // Establece si el jugador está escondido
    void setHidden(bool hidden);
    bool isHidden() const;
    
    // Sistema de ataque
    bool isAttacking() const;
    sf::FloatRect getAttackBounds() const;

    // Sistema de Salud
    void takeDamage(int damage);
    int getLives() const;
    bool isDead() const;

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
    
    // Texturas Muerte (secuencia)
    sf::Texture mTextureDead1;         // dexter_muerto1.png
    sf::Texture mTextureDead2;         // dexter_muerto2.png
    sf::Texture mTextureDead3;         // dexter_muerto3.png
    sf::Texture mTextureDead;          // dexter_tirado.png (final)

    // Audio
    sf::SoundBuffer mBufferPain;
    sf::Sound mSoundPain;

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
    float mFallingTimer;

    // Variables de Jeringa
    bool mHasSyringe;
    bool mIsInSyringeAttack;
    int mSyringeAttackFrame;
    PlayerState mPreviousStateBeforeAttack;
    bool mShouldDisableSyringeAfterAttack;

    // Salud
    int mLives;
    float mInvulnerableTimer;
    bool mIsInvulnerable;
    
    // Animación de muerte
    int mDeathFrame;
    float mDeathTimer;
    float mDeathBlinkTimer;
    float mDeathAlpha;
    bool mDeathAnimationComplete;
};



