#pragma once
#include <SFML/Graphics.hpp>

class Player {
public:
    Player();
    
    // Actualiza la posición del jugador basado en inputs y tiempo
    void update(sf::Time deltaTime);
    
    // Dibuja el sprite del jugador en la ventana
    void draw(sf::RenderWindow& window);

private:
    sf::Sprite mSprite;
    sf::Texture mTexture;
    float mSpeed; // Velocidad de movimiento en pixeles por segundo
};
