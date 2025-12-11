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
    mLifeSprite.setScale(0.1f, 0.1f); // Reducir tamaño drásticamente
    
    // Cargar textura de Blood Slide para HUD
    if (!mBloodSlideTexture.loadFromFile(std::string(Config::IMAGE_PATH) + "bloodslides.png")) {
        std::cerr << "Error: No se pudo cargar bloodslides.png" << std::endl;
        // Fallback: Crear textura amarilla
        sf::Image img;
        img.create(32, 32, sf::Color::Yellow);
        mBloodSlideTexture.loadFromImage(img);
    }
    mBloodSlideSprite.setTexture(mBloodSlideTexture);
    mBloodSlideSprite.setScale(0.25f, 0.25f); // Escala mayor para mejor visibilidad
}

void HUD::draw(sf::RenderWindow& window, int lives, int bloodSlides) {
    // Guardar vista actual para restaurarla después (aunque Game.cpp ya maneja esto)
    sf::View currentView = window.getView();
    window.setView(window.getDefaultView()); // Dibujar en coordenadas de pantalla

    // === VIDAS (Esquina Superior Izquierda) ===
    float livesStartX = 20.f;
    float livesStartY = 20.f;
    float livesSpacing = 60.f;

    for (int i = 0; i < lives; ++i) {
        mLifeSprite.setPosition(livesStartX + i * livesSpacing, livesStartY);
        window.draw(mLifeSprite);
    }
    
    // === BLOOD SLIDES (Esquina Superior Derecha) ===
    float windowWidth = static_cast<float>(window.getSize().x);
    float slidesStartX = windowWidth - 40.f; // Desde la derecha
    float slidesStartY = 20.f;
    float slidesSpacing = 60.f;
    
    // Dibujar de derecha a izquierda
    for (int i = 0; i < bloodSlides; ++i) {
        mBloodSlideSprite.setPosition(slidesStartX - i * slidesSpacing, slidesStartY);
        window.draw(mBloodSlideSprite);
    }

    window.setView(currentView);
}
