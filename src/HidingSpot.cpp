#include "HidingSpot.hpp"

HidingSpot::HidingSpot(sf::Texture* texture, sf::Vector2f position, float scale) {
    if (texture) {
        mSprite.setTexture(*texture);
        mSprite.setScale(scale, scale);
        
        // Centrar origen en la base del sprite
        sf::FloatRect bounds = mSprite.getLocalBounds();
        mSprite.setOrigin(bounds.width / 2.f, bounds.height);
        mSprite.setPosition(position);
        
        // Área de interacción ligeramente más grande que el sprite
        sf::FloatRect spriteBounds = mSprite.getGlobalBounds();
        mInteractionBounds = sf::FloatRect(
            spriteBounds.left - 50.f,
            spriteBounds.top,
            spriteBounds.width + 100.f,
            spriteBounds.height
        );
    }
}

void HidingSpot::draw(sf::RenderWindow& window) {
    window.draw(mSprite);
}

bool HidingSpot::isPlayerInside(const Player& player) const {
    return mInteractionBounds.intersects(player.getBounds());
}

sf::FloatRect HidingSpot::getBounds() const {
    return mInteractionBounds;
}
