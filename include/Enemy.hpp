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
    TURNING,    // Girando (momento de pausa antes de cambiar dirección)
    DYING,      // Muriendo (parpadeo)
    DEAD        // Muerto (desvaneciendose)
};

// Tipos de enemigo
enum class EnemyType {
    POLICE1,    // Policía 1 (original)
    POLICE2,    // Policía 2 (nuevo)
    BOSS        // Doakes (jefe final)
};

// Estructura que contiene todas las texturas para un tipo de enemigo
struct EnemyTextures {
    sf::Texture* idle;
    sf::Texture* walk1;
    sf::Texture* walk2;
    sf::Texture* aim;
    sf::Texture* shoot;
    sf::Texture* death1;   // Primer frame de muerte (opcional)
    sf::Texture* death2;   // Segundo frame de muerte (opcional)
    sf::Texture* dead;     // Frame final de muerte
    
    EnemyTextures() : idle(nullptr), walk1(nullptr), walk2(nullptr), 
                      aim(nullptr), shoot(nullptr), 
                      death1(nullptr), death2(nullptr), dead(nullptr) {}
};

class Enemy {
public:
    /**
     * Constructor del enemigo (NUEVO: usa EnemyTextures)
     * @param textures Estructura con todas las texturas del enemigo
     * @param type Tipo de enemigo (POLICE1, POLICE2, BOSS)
     * @param position Posición inicial (X, Y)
     * @param patrolMinX Límite izquierdo del patrullaje
     * @param patrolMaxX Límite derecho del patrullaje
     */
    Enemy(const EnemyTextures& textures, EnemyType type,
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
    
    // Mata al enemigo (inicia animación de muerte)
    void kill();
    
    // Verifica si el enemigo está muerto o muriendo
    bool isDead() const;
    bool isDying() const;
    
    // Verifica si el enemigo está persiguiendo al jugador
    bool isChasing() const;
    
    // Verifica si el enemigo debe ser eliminado del juego
    bool shouldRemove() const;
    
    // Sistema de disparo
    bool wantsToShoot() const;       // Devuelve true si quiere disparar
    void confirmShot();              // Confirma que se disparó la bala
    sf::Vector2f getGunPosition() const;  // Posición de la pistola
    bool isFacingRight() const;      // Dirección que mira
    
    // Sistema de salud (para boss)
    void takeDamage(int damage = 1);
    int getHealth() const;

private:
    // Métodos internos
    void updateAnimation(float dt);
    void updateVisionCone();
    void updateOrigin();
    float getTextureScale() const;

    sf::Sprite mSprite;
    sf::ConvexShape mVisionCone;
    
    // Tipo y salud
    EnemyType mType;
    int mHealth;
    int mMaxHealth;
    
    // Texturas (punteros a texturas externas)
    EnemyTextures mTextures;

    // Audio
    sf::SoundBuffer mBufferChasing;
    sf::Sound mSoundChasing;
    sf::SoundBuffer mBufferGunshot;
    sf::Sound mSoundGunshot;
    bool mChasingSoundPlayed;
    
    // Animación de muerte
    float mDeathTimer;
    float mDeathBlinkTimer;
    float mDeathAlpha;
    int mDeathFrame;  // Frame actual de la animación de muerte (0, 1, 2)
    
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
    bool mWantsToShoot;    // Flag para indicar que quiere disparar
};
