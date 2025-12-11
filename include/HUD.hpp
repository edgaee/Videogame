#pragma once
#include <SFML/Graphics.hpp>
#include "Config.hpp"
#include <vector>

class HUD {
public:
    HUD();
    
    // Dibuja el HUD (vidas y blood slides)
    void draw(sf::RenderWindow& window, int lives, int bloodSlides);

private:
    sf::Texture mLifeTexture;
    sf::Sprite mLifeSprite;
    
    sf::Texture mBloodSlideTexture;
    sf::Sprite mBloodSlideSprite;
};
