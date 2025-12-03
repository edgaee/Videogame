#include "Player.hpp"
#include <iostream>

Player::Player() : mSpeed(400.f) {
    // Intentar cargar la textura
    if (!mTexture.loadFromFile("assets/images/dexter_default.png")) {
        std::cerr << "Error: No se pudo cargar la textura del jugador." << std::endl;
        // Si falla, no veremos nada, o podríamos poner un color al sprite si no tiene textura
    }

    mSprite.setTexture(mTexture);
    
    // Posición inicial arbitraria
    mSprite.setPosition(100.f, 10.f);
}

void Player::update(sf::Time deltaTime) {
    sf::Vector2f movement(0.f, 0.f);

    // Detectar teclas A (Izquierda) y D (Derecha)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        movement.x -= mSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        movement.x += mSpeed;
    }

    // Mover el sprite
    // Multiplicamos por deltaTime.asSeconds() para que la velocidad sea independiente de los FPS
    mSprite.move(movement * deltaTime.asSeconds());
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(mSprite);
}
