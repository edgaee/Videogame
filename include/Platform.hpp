#pragma once
#include <SFML/Graphics.hpp>

class Platform {
public:
    // Constructor actualizado para usar texturas
    Platform(sf::Texture* texture, sf::Vector2f position);
    
    void draw(sf::RenderWindow& window);
    sf::FloatRect getBounds() const;

private:
    sf::Sprite mSprite;
};
