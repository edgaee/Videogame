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

// Estructura para coleccionables (Blood Slides)
struct Collectible {
    sf::Sprite sprite;
    sf::Vector2f position;
    bool collected;
    
    Collectible() : collected(false) {}
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
    int getCollectedCount() const;
    bool isBossDefeated() const;

private:
    sf::Texture mTexFloor;
    sf::Texture mTexVent;
    sf::Texture mTexProp1;
    sf::Texture mTexProp2;
    std::vector<Platform> mPlatforms;
    std::vector<HidingSpot> mHidingSpots;
    
    // Enemigos - Policía 1
    sf::Texture mTexPolice1_Idle;
    sf::Texture mTexPolice1_Walk1;
    sf::Texture mTexPolice1_Walk2;
    sf::Texture mTexPolice1_Aim;
    sf::Texture mTexPolice1_Shoot;
    sf::Texture mTexPolice1_Death1;
    sf::Texture mTexPolice1_Death2;
    sf::Texture mTexPolice1_Dead;
    
    // Enemigos - Policía 2
    sf::Texture mTexPolice2_Idle;
    sf::Texture mTexPolice2_Walk1;
    sf::Texture mTexPolice2_Walk2;
    sf::Texture mTexPolice2_Aim;
    sf::Texture mTexPolice2_Shoot;
    sf::Texture mTexPolice2_Death1;
    sf::Texture mTexPolice2_Death2;
    sf::Texture mTexPolice2_Dead;
    
    // Boss - Doakes
    sf::Texture mTexDoakes_Idle;
    sf::Texture mTexDoakes_Walk1;
    sf::Texture mTexDoakes_Walk2;
    sf::Texture mTexDoakes_Aim;
    sf::Texture mTexDoakes_Shoot;
    sf::Texture mTexDoakes_Dead;
    
    std::vector<Enemy> mEnemies;
    bool mPlayerDetected;
    
    // Sistema de balas
    sf::Texture mTexBullet;
    std::vector<Bullet> mBullets;
    
    // Sistema de coleccionables
    sf::Texture mTexBloodSlide;
    std::vector<Collectible> mCollectibles;
    int mCollectedCount;
};
