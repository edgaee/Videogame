#pragma once
#include <SFML/Graphics.hpp>
#include "Player.hpp"

class HidingSpot {
public:
    HidingSpot(sf::Texture* texture, sf::Vector2f position, float scale = 1.0f);
    void draw(sf::RenderWindow& window);
    bool isPlayerInside(const Player& player) const;
    sf::FloatRect getBounds() const;

private:
    sf::Sprite mSprite;
    sf::FloatRect mInteractionBounds;
};
