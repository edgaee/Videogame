#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Platform.hpp"
#include "HidingSpot.hpp"
#include "Player.hpp"
#include "Enemy.hpp"

// Estructura para representar una bala
struct Bullet {
    sf::Sprite sprite;
    sf::Vector2f velocity;
    sf::Vector2f targetPos;  // Posición objetivo (jugador)
    float travelTime;        // Tiempo total de viaje
    float elapsedTime;       // Tiempo transcurrido
    bool active;
    
    Bullet() : travelTime(0.f), elapsedTime(0.f), active(true) {}
};

class Level {
public:
    Level();
    
    // Carga el Nivel 1 (Estación de Policía)
    void loadLevel1();
    
    // Actualiza entidades del nivel (incluye enemigos)
    void update(float dt, Player& player);
    
    // Dibuja el nivel
    void draw(sf::RenderWindow& window);
    
    // Gestiona colisiones del jugador con el nivel
    void checkCollisions(Player& player, float dt);

    // Getters para Game (si fueran necesarios)
    const std::vector<HidingSpot>& getHidingSpots() const;
    bool isPlayerDetected() const;

private:
    sf::Texture mTexFloor;
    sf::Texture mTexVent;
    sf::Texture mTexProp1;
    sf::Texture mTexProp2;
    std::vector<Platform> mPlatforms;
    std::vector<HidingSpot> mHidingSpots;
    
    // Enemigos
    sf::Texture mTexEnemyWalk1;
    sf::Texture mTexEnemyWalk2;
    sf::Texture mTexEnemyIdle;
    sf::Texture mTexEnemyShoot;
    std::vector<Enemy> mEnemies;
    bool mPlayerDetected;
    
    // Sistema de balas
    sf::Texture mTexBullet;
    std::vector<Bullet> mBullets;
};
