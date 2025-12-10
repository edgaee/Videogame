#include "Platform.hpp"

Platform::Platform(sf::Texture* texture, sf::Vector2f size, sf::Vector2f position) {
    mBody.setSize(size);
    mBody.setOrigin(0.f, 0.f); // Origen en la esquina superior izquierda por defecto
    mBody.setPosition(position);
    
    if (texture) {
        mBody.setTexture(texture);
        mBody.setFillColor(sf::Color::White);
    } else {
        // Si no hay textura, usamos colores para debug
        // Verde para "suelo" (si está abajo), Rojo para otras
        // Por simplicidad, usaremos un color por defecto y el usuario puede cambiarlo si quiere lógica más compleja
        mBody.setFillColor(sf::Color::Green); 
        mBody.setOutlineThickness(1.f);
        mBody.setOutlineColor(sf::Color::Black);
    }
}

void Platform::draw(sf::RenderWindow& window) {
    window.draw(mBody);
}

sf::FloatRect Platform::getBounds() const {
    return mBody.getGlobalBounds();
}
