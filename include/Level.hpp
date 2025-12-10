#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Platform.hpp"
#include "HidingSpot.hpp"
#include "Player.hpp"

class Level {
public:
    Level();
    
    // Carga el Nivel 1 (Estación de Policía)
    void loadLevel1();
    
    // Actualiza entidades del nivel
    void update(float dt);
    
    // Dibuja el nivel
    void draw(sf::RenderWindow& window);
    
    // Gestiona colisiones del jugador con el nivel
    void checkCollisions(Player& player, float dt);

    // Getters para Game (si fueran necesarios)
    const std::vector<HidingSpot>& getHidingSpots() const;

private:
    sf::Texture mTexFloor;
    sf::Texture mTexVent;
    std::vector<Platform> mPlatforms;
    std::vector<HidingSpot> mHidingSpots;
};
