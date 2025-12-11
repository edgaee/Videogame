#include "HUD.hpp"
#include <iostream>

HUD::HUD() {
    if (!mLifeTexture.loadFromFile(std::string(Config::IMAGE_PATH) + "blood_drop_life.png")) {
        std::cerr << "Error: No se pudo cargar blood_drop_life.png" << std::endl;
        // Fallback: Crear textura roja
        sf::Image img;
        img.create(32, 32, sf::Color::Red);
        mLifeTexture.loadFromImage(img);
    }
    mLifeSprite.setTexture(mLifeTexture);
    // Escalar si es necesario (asumiendo icono de 32-64px)
    // mLifeSprite.setScale(0.5f, 0.5f); 
}

void HUD::draw(sf::RenderWindow& window, int lives) {
    // Guardar vista actual para restaurarla después (aunque Game.cpp ya maneja esto)
    sf::View currentView = window.getView();
    window.setView(window.getDefaultView()); // Dibujar en coordenadas de pantalla

    float startX = 20.f;
    float startY = 20.f;
    float spacing = 40.f;

    for (int i = 0; i < lives; ++i) {
        mLifeSprite.setPosition(startX + i * spacing, startY);
        window.draw(mLifeSprite);
    }

    window.setView(currentView);
}
