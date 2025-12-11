#pragma once
#include <SFML/Graphics.hpp>
#include "Config.hpp"
#include <vector>

class HUD {
public:
    HUD();
    
    // Dibuja el HUD (vidas)
    void draw(sf::RenderWindow& window, int lives);

private:
    sf::Texture mLifeTexture;
    sf::Sprite mLifeSprite;
};
