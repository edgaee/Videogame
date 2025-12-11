#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Config.hpp"

class Player; // Forward declaration

// Estados del enemigo
enum class EnemyState {
    PATROLLING, // Caminando (patrullando)
    CHASING,    // Persiguiendo al jugador
    SHOOTING,   // Disparando
    TURNING     // Girando (momento de pausa antes de cambiar dirección)
};

class Enemy {
public:
    /**
     * Constructor del enemigo
     * @param textureWalk1 Textura del primer frame de caminata
     * @param textureWalk2 Textura del segundo frame de caminata
     * @param textureIdle Textura cuando está quieto/girando
     * @param textureShoot Textura disparando
     * @param position Posición inicial (X, Y)
     * @param patrolMinX Límite izquierdo del patrullaje
     * @param patrolMaxX Límite derecho del patrullaje
     */
    Enemy(sf::Texture* textureWalk1, sf::Texture* textureWalk2, sf::Texture* textureIdle, sf::Texture* textureShoot,
          sf::Vector2f position, float patrolMinX, float patrolMaxX);
    
    // Actualiza el movimiento y animación del enemigo
    void update(float dt, Player& player);
    
    // Dibuja el sprite y el cono de visión
    void draw(sf::RenderWindow& window);
    
    // Verifica si el jugador está en el cono de visión
    bool checkPlayerDetection(const Player& player) const;
    
    // Obtiene la posición del enemigo
    sf::Vector2f getPosition() const;
    
    // Obtiene los límites del sprite
    sf::FloatRect getBounds() const;

private:
    // Métodos internos
    void updateAnimation(float dt);
    void updateVisionCone();
    void updateOrigin();
    float getTextureScale() const;

    sf::Sprite mSprite;
    sf::ConvexShape mVisionCone;
    
    // Texturas
    sf::Texture* mTextureWalk1;
    sf::Texture* mTextureWalk2;
    sf::Texture* mTextureIdle;
    sf::Texture* mTextureShoot;

    // Audio
    sf::SoundBuffer mBufferChasing;
    sf::Sound mSoundChasing;
    sf::SoundBuffer mBufferGunshot;
    sf::Sound mSoundGunshot;
    bool mChasingSoundPlayed;
    
    // Patrullaje
    float mPatrolStart;
    float mPatrolEnd;
    float mSpeed;
    bool mMovingRight;
    
    // Estado y Animación
    EnemyState mCurrentState;
    float mAnimationTimer;
    bool mIsWalkFrame1;
    float mTurnTimer;       // Tiempo de pausa al girar
    
    // Configuración del cono de visión
    float mVisionDistance;  // Distancia del cono
    float mVisionAngle;     // Ángulo del cono (medio ángulo)

    // Combate
    float mShootTimer;
    float mShootCooldown;
    float mLostSightTimer; // Tiempo sin ver al jugador antes de volver a patrullar
};
