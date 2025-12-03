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
    // Método privado para manejar la animación
    void animate(float dt);

    sf::Sprite mSprite;
    
    // Texturas para la animación
    sf::Texture mTextureIdle;
    sf::Texture mTextureRun1;
    sf::Texture mTextureRun2;

    float mSpeed; // Velocidad de movimiento en pixeles por segundo
    
    // Variables de animación
    float mAnimationTimer; // Acumulador de tiempo para cambiar frames
    bool mIsRunFrame1;     // Estado para alternar entre paso1 y paso2
};
