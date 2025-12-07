#include "Player.hpp"
#include <iostream>

Player::Player() 
    : mSpeed(Config::PLAYER_SPEED), 
      mAnimationTimer(0.f), 
      mIsRunFrame1(true),
      mCurrentState(PlayerState::IDLE),
      mFacingLeft(false),
      mPreviousFacingLeft(false),
      mHasSyringe(false)
{
    // Cargar texturas para movimiento normal
    bool success = true;
    if (!mTextureIdle.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_default.png")) success = false;
    if (!mTextureRun1.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_paso1.png")) success = false;
    if (!mTextureRun2.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_paso2.png")) success = false;
    
    // Cargar texturas para agacharse
    if (!mTextureCrouch.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_agachado.png")) success = false;
    
    // Intentar cargar animación de agacharse si existen
    // Si no existen, usaremos solo dexter_agachado.png
    mTextureCrouch1.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_agachado.png");
    mTextureCrouch2.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_agachado.png");
    
    // Cargar texturas para jeringa
    // Si no existen, usar las texturas normales como fallback
    // Para IDLE, usar la misma imagen dexter_default en ambos casos
    mTextureSyringeIdle.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_default.png");
    
    // Para IDLE quieto CON jeringa, cargar dexter_jeringa.png
    if (!mTextureSyringeIdleStatic.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_jeringa.png")) {
        mTextureSyringeIdleStatic.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_default.png");
    }
    
    if (!mTextureSyringeRun1.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_paso1_jeringa.png")) {
        mTextureSyringeRun1.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_paso1.png");
    }
    if (!mTextureSyringeRun2.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_paso2_jeringa.png")) {
        mTextureSyringeRun2.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_paso2.png");
    }
    if (!mTextureSyringeCrouch.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_agachado_jeringa.png")) {
        mTextureSyringeCrouch.loadFromFile(std::string(Config::IMAGE_PATH) + "dexter_agachado.png");
    }
    
    // Debug: mostrar tamaños de texturas
    std::cout << "Tamaños de texturas:" << std::endl;
    std::cout << "  IDLE: " << mTextureIdle.getSize().x << "x" << mTextureIdle.getSize().y << std::endl;
    std::cout << "  SYRINGE_IDLE: " << mTextureSyringeIdle.getSize().x << "x" << mTextureSyringeIdle.getSize().y << std::endl;
    std::cout << "  RUN1: " << mTextureRun1.getSize().x << "x" << mTextureRun1.getSize().y << std::endl;
    std::cout << "  SYRINGE_RUN1: " << mTextureSyringeRun1.getSize().x << "x" << mTextureSyringeRun1.getSize().y << std::endl;

    if (!success) {
        std::cerr << "Error: No se pudieron cargar algunas texturas del jugador." << std::endl;
    }

    // Configuración inicial del sprite
    mSprite.setTexture(mTextureIdle);
    
    // Posición inicial (centro horizontal, en la parte baja para ver completo el sprite)
    mSprite.setPosition(Config::PLAYER_INITIAL_X, Config::PLAYER_INITIAL_Y);
    
    // Actualizar origen DESPUÉS de establecer la posición
    updateOrigin();
}

void Player::update(sf::Time deltaTime) {
    sf::Vector2f movement(0.f, 0.f);
    bool isMoving = false;
    
    // Detectar tecla de jeringa (Space) - toggle modo ataque
    static bool spacePressed = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        if (!spacePressed) {
            spacePressed = true;
            mHasSyringe = !mHasSyringe;  // Toggle jeringa (modo ataque)
            // Actualizar la textura del estado actual sin cambiar de estado
            updateTextureForCurrentState();
        }
    } else {
        spacePressed = false;
    }
    
    // Detectar tecla de agacharse (S)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        setState(PlayerState::CROUCHING);
    } else {
        // Si no estamos agachados
        if (mCurrentState == PlayerState::CROUCHING) {
            setState(PlayerState::IDLE);
        }
        
        // Detectar teclas A (Izquierda) y D (Derecha)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            movement.x -= mSpeed;
            isMoving = true;
            mFacingLeft = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            movement.x += mSpeed;
            isMoving = true;
            mFacingLeft = false;
        }

        // Si está en estado de agacharse, pasar a corriendo
        if (isMoving && mCurrentState == PlayerState::IDLE) {
            setState(PlayerState::RUNNING);
        } else if (!isMoving && mCurrentState == PlayerState::RUNNING) {
            setState(PlayerState::IDLE);
        }
    }

    // Detectar cambio de dirección y actualizar escala inmediatamente
    if (mFacingLeft != mPreviousFacingLeft && mCurrentState == PlayerState::RUNNING) {
        // La dirección cambió mientras se está corriendo
        mPreviousFacingLeft = mFacingLeft;
        mSprite.setScale(mFacingLeft ? -1.f : 1.f, 1.f);
    } else if (mCurrentState != PlayerState::RUNNING) {
        // Actualizar la dirección anterior cuando no estamos corriendo
        mPreviousFacingLeft = mFacingLeft;
    }

    // Aplicar movimiento
    if (mCurrentState == PlayerState::CROUCHING) {
        // No se mueve mucho si está agachado
        mSprite.move(movement * (Config::CROUCH_SPEED / Config::PLAYER_SPEED) * deltaTime.asSeconds());
    } else {
        mSprite.move(movement * deltaTime.asSeconds());
    }

    // Mantener al jugador dentro de los límites de la pantalla
    clampToBounds();

    // Manejar animaciones y escalas según el estado
    switch (mCurrentState) {
        case PlayerState::RUNNING:
            // Animar entre las dos texturas de corrida
            animate(deltaTime.asSeconds());
            break;
        
        case PlayerState::CROUCHING:
        case PlayerState::IDLE:
            // No hacer nada especial aquí, todo se configura en setState
            break;
    }
    
    // Aplicar offset Y para alinear todas las imágenes en la base
    sf::Vector2f currentPos = mSprite.getPosition();
    float yOffset = getTextureYOffset(mSprite.getTexture());
    mSprite.setPosition(currentPos.x, Config::PLAYER_INITIAL_Y + yOffset);
}

void Player::animate(float dt) {
    mAnimationTimer += dt;

    // Cambiar de frame cada ANIMATION_SPEED segundos
    if (mAnimationTimer >= Config::ANIMATION_SPEED) {
        mAnimationTimer = 0.f;
        
        // Alternar frame
        if (mIsRunFrame1) {
            // Usar textura de jeringa si la tiene
            if (mHasSyringe) {
                mSprite.setTexture(mTextureSyringeRun1);
            } else {
                mSprite.setTexture(mTextureRun1);
            }
        } else {
            // Usar textura de jeringa si la tiene
            if (mHasSyringe) {
                mSprite.setTexture(mTextureSyringeRun2);
            } else {
                mSprite.setTexture(mTextureRun2);
            }
        }
        
        // Actualizar origen
        updateOrigin();
        
        mIsRunFrame1 = !mIsRunFrame1;
    }
}

void Player::updateOrigin() {
    sf::FloatRect bounds = mSprite.getLocalBounds();
    // El origen solo centra horizontalmente
    // Verticalmente comienza desde el TOP (0) para permitir offset individual
    mSprite.setOrigin(bounds.width / 2.f, 0.f);
}

float Player::getTextureYOffset(const sf::Texture* texture) const {
    // Alinea todos los sprites por sus pies (parte inferior)
    // Usa dexter_paso1.png como referencia de altura base
    
    if (!texture) return 0.f;
    
    float referenceHeight = (float)mTextureRun1.getSize().y;  // Altura de referencia (corriendo)
    float currentHeight = (float)texture->getSize().y;
    
    // Obtener la escala actual del sprite
    sf::Vector2f spriteScale = mSprite.getScale();
    float scaleY = (spriteScale.y < 0) ? -spriteScale.y : spriteScale.y;
    
    float scaledHeight = currentHeight * scaleY;
    
    // Desplazamiento hacia abajo para alinear los pies
    // Mantiene la base en la misma línea Y
    return referenceHeight - scaledHeight;
}

float Player::getTextureScale(const sf::Texture* texture) const {
    // Retorna la escala necesaria para proporcionalidad visual
    // RUN1 (229x378) es la referencia para estados de movimiento
    // IDLE (340x733) se escala respecto a RUN1
    // CROUCH (192x242) se escala para ser ~50% de la altura de RUN1
    // ATTACK se escala como IDLE para mantener proporcionalidad
    
    float runHeight = (float)mTextureRun1.getSize().y;  // 378 (referencia)
    float currentHeight = (float)texture->getSize().y;
    
    if (currentHeight <= 0) return 1.f;
    
    // Si es IDLE, calcular escala respecto a RUN1
    if (texture == &mTextureIdle || texture == &mTextureSyringeIdle || texture == &mTextureSyringeIdleStatic) {
        // Todas las IDLE (normal y jeringa) deben escalar al mismo tamaño final
        // Usamos la altura de RUN1 como referencia
        float referenceHeight = runHeight;  // 378 (altura de RUN1)
        return referenceHeight / currentHeight;
    }
    
    // Si es CROUCH, escalar para ser aproximadamente 65% de RUN1
    if (texture == &mTextureCrouch || texture == &mTextureSyringeCrouch) {
        // Ambas versiones de CROUCH (normal y jeringa) deben tener el mismo tamaño final
        // Usamos la altura de mTextureCrouch (versión normal) como referencia
        float crouchReferenceHeight = (float)mTextureCrouch.getSize().y;  // Altura de CROUCH normal
        float targetHeight = runHeight * 0.65f;
        return targetHeight / crouchReferenceHeight;  // Escala basada en CROUCH normal
    }
    
    return 1.f;
}

void Player::clampToBounds() {
    sf::FloatRect playerBounds = mSprite.getGlobalBounds();
    
    // Limitar izquierda
    if (playerBounds.left < 0) {
        mSprite.setPosition(mSprite.getOrigin().x, mSprite.getPosition().y);
    }
    
    // Limitar derecha
    if (playerBounds.left + playerBounds.width > Config::WINDOW_WIDTH) {
        mSprite.setPosition(Config::WINDOW_WIDTH - playerBounds.width + mSprite.getOrigin().x, 
                          mSprite.getPosition().y);
    }
}

void Player::setState(PlayerState newState) {
    if (mCurrentState != newState) {
        mCurrentState = newState;
        mAnimationTimer = 0.f;
        mIsRunFrame1 = true;
        
        // Configurar textura y escala según el nuevo estado
        if (newState == PlayerState::RUNNING) {
            // Cambiar inmediatamente a la primera textura de corrida
            if (mHasSyringe) {
                mSprite.setTexture(mTextureSyringeRun1);
            } else {
                mSprite.setTexture(mTextureRun1);
            }
            mSprite.setScale(mFacingLeft ? -1.f : 1.f, 1.f);
            updateOrigin();
        } else if (newState == PlayerState::IDLE) {
            // Actualizar origen cuando se entra en estado IDLE
            // Usar textura de jeringa si la tiene
            if (mHasSyringe) {
                // Cuando está quieto con jeringa, usar la imagen dexter_jeringa.png
                mSprite.setTexture(mTextureSyringeIdleStatic);
            } else {
                mSprite.setTexture(mTextureIdle);
            }
            float idleScale = getTextureScale(mSprite.getTexture());
            mSprite.setScale(mFacingLeft ? -idleScale : idleScale, idleScale);
            updateOrigin();
        } else if (newState == PlayerState::CROUCHING) {
            // Cambiar a textura agachada (verificar si tiene jeringa)
            if (mHasSyringe) {
                mSprite.setTexture(mTextureSyringeCrouch);
            } else {
                mSprite.setTexture(mTextureCrouch);
            }
            float crouchScale = getTextureScale(mSprite.getTexture());
            mSprite.setScale(mFacingLeft ? -crouchScale : crouchScale, crouchScale);
            updateOrigin();
        }
    }
}

void Player::updateTextureForCurrentState() {
    // Actualizar la textura basado en el estado actual sin cambiar de estado
    // Esto se usa cuando se activa/desactiva la jeringa mientras está en IDLE
    
    if (mCurrentState == PlayerState::IDLE) {
        if (mHasSyringe) {
            // Cuando está quieto con jeringa, usar la imagen dexter_jeringa.png
            mSprite.setTexture(mTextureSyringeIdleStatic);
        } else {
            mSprite.setTexture(mTextureIdle);
        }
        float idleScale = getTextureScale(mSprite.getTexture());
        mSprite.setScale(mFacingLeft ? -idleScale : idleScale, idleScale);
        updateOrigin();
    } else if (mCurrentState == PlayerState::CROUCHING) {
        if (mHasSyringe) {
            mSprite.setTexture(mTextureSyringeCrouch);
        } else {
            mSprite.setTexture(mTextureCrouch);
        }
        float crouchScale = getTextureScale(mSprite.getTexture());
        mSprite.setScale(mFacingLeft ? -crouchScale : crouchScale, crouchScale);
        updateOrigin();
    }
    // Si está en RUNNING, no hacer nada aquí (la animación lo maneja)
}

sf::Vector2f Player::getPosition() const {
    return mSprite.getPosition();
}

PlayerState Player::getState() const {
    return mCurrentState;
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(mSprite);
}

