#include "HidingSpot.hpp"

HidingSpot::HidingSpot(sf::Vector2f size, sf::Vector2f position) {
    mBody.setSize(size);
    mBody.setPosition(position);
    mBody.setFillColor(sf::Color(50, 50, 50, 180)); // Gris oscuro semitransparente
    mBody.setOutlineColor(sf::Color(200, 200, 200));
    mBody.setOutlineThickness(2.f);
}

void HidingSpot::draw(sf::RenderWindow& window) {
    window.draw(mBody);
}

bool HidingSpot::isPlayerInside(const Player& player) const {
    return mBody.getGlobalBounds().intersects(player.getBounds());
}

sf::FloatRect HidingSpot::getBounds() const {
    return mBody.getGlobalBounds();
}
