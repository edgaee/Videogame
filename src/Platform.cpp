#include "Platform.hpp"

Platform::Platform(sf::Texture* texture, sf::Vector2f position) {
    mSprite.setPosition(position);
    
    if (texture) {
        mSprite.setTexture(*texture);
    } else {
        // Fallback si no hay textura (aunque ahora siempre debería haber)
        // Podríamos cargar una textura por defecto o dejarlo vacío
    }
}

void Platform::draw(sf::RenderWindow& window) {
    window.draw(mSprite);
}

sf::FloatRect Platform::getBounds() const {
    return mSprite.getGlobalBounds();
}
