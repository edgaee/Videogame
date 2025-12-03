#include "Player.hpp"
#include <iostream>

Player::Player() 
    : mSpeed(400.f), mAnimationTimer(0.f), mIsRunFrame1(true) 
{
    // Cargar las 3 texturas
    bool success = true;
    if (!mTextureIdle.loadFromFile("assets/images/dexter_default.png")) success = false;
    if (!mTextureRun1.loadFromFile("assets/images/dexter_paso1.png")) success = false;
    if (!mTextureRun2.loadFromFile("assets/images/dexter_paso2.png")) success = false;

    if (!success) {
        std::cerr << "Error: No se pudieron cargar algunas texturas del jugador." << std::endl;
    }

    // Configuración inicial del sprite
    mSprite.setTexture(mTextureIdle);
    
    // Ajustar el origen al centro horizontal de la imagen para que el flip (espejo) funcione bien
    sf::FloatRect bounds = mSprite.getLocalBounds();
    mSprite.setOrigin(bounds.width / 2.f, 0.f);

    // Posición inicial
    mSprite.setPosition(0.f, 0.f);
}

void Player::update(sf::Time deltaTime) {
    sf::Vector2f movement(0.f, 0.f);
    bool isMoving = false;
    
    // Detectar dirección actual basada en la escala (si es negativa en X, mira a la izquierda)
    bool facingLeft = mSprite.getScale().x < 0;

    // Detectar teclas A (Izquierda) y D (Derecha)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        movement.x -= mSpeed;
        isMoving = true;
        facingLeft = true;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        movement.x += mSpeed;
        isMoving = true;
        facingLeft = false;
    }

    // Mover el sprite
    mSprite.move(movement * deltaTime.asSeconds());

    // Manejo de animación y escala
    if (isMoving) {
        // Si veníamos de estar quietos (textura idle), cambiar inmediatamente a la textura de correr
        // Esto evita que se vea la textura idle gigante durante el primer frame de movimiento
        if (mSprite.getTexture() == &mTextureIdle) {
            mSprite.setTexture(mIsRunFrame1 ? mTextureRun1 : mTextureRun2);
            mAnimationTimer = 0.f; 
        }

        animate(deltaTime.asSeconds());
        
        // Asegurar escala 1.0 (normal) al correr
        mSprite.setScale(facingLeft ? -1.f : 1.f, 1.f);
        
        // Actualizar origen para centrar la textura actual
        sf::FloatRect bounds = mSprite.getLocalBounds();
        mSprite.setOrigin(bounds.width / 2.f, 0.f);
        
    } else {
        mSprite.setTexture(mTextureIdle);
        mAnimationTimer = 0.f;
        
        // Calcular factor de escala para igualar la altura de la animación de correr
        // Esto evita que el personaje se vea gigante si la imagen default es más grande
        float runHeight = (float)mTextureRun1.getSize().y;
        float idleHeight = (float)mTextureIdle.getSize().y;
        float scaleFactor = (idleHeight > 0) ? (runHeight / idleHeight) : 1.f;
        
        // Aplicar escala manteniendo la dirección
        mSprite.setScale(facingLeft ? -scaleFactor : scaleFactor, scaleFactor);
        
        // Actualizar origen para centrar la textura idle
        sf::FloatRect bounds = mSprite.getLocalBounds();
        mSprite.setOrigin(bounds.width / 2.f, 0.f);
    }
}

void Player::animate(float dt) {
    mAnimationTimer += dt;

    // Cambiar de frame cada 0.15 segundos
    if (mAnimationTimer >= 0.15f) {
        mAnimationTimer = 0.f; // Reiniciar contador
        
        // Alternar frame
        if (mIsRunFrame1) {
            mSprite.setTexture(mTextureRun1);
        } else {
            mSprite.setTexture(mTextureRun2);
        }
        
        // Actualizar origen cada vez que cambia la textura para evitar saltos
        sf::FloatRect bounds = mSprite.getLocalBounds();
        mSprite.setOrigin(bounds.width / 2.f, 0.f);
        
        mIsRunFrame1 = !mIsRunFrame1;
    }
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(mSprite);
}
