#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"

class HidingSpot {
public:
    HidingSpot(sf::Vector2f size, sf::Vector2f position);
    void draw(sf::RenderWindow& window);
    bool isPlayerInside(const Player& player) const;
    sf::FloatRect getBounds() const;

private:
    sf::RectangleShape mBody;
};
