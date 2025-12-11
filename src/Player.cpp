#include "Player.hpp"
#include "Platform.hpp"
#include <iostream>

Player::Player() 
    : mSpeed(Config::PLAYER_SPEED), 
      mAnimationTimer(0.f), 
      mIsRunFrame1(true),
      mCurrentState(PlayerState::IDLE),
      mFacingLeft(false),
      mPreviousFacingLeft(false),
      mHasSyringe(false),
      mIsInSyringeAttack(false),
      mSyringeAttackFrame(0),
      mPreviousStateBeforeAttack(PlayerState::IDLE),
      mShouldDisableSyringeAfterAttack(false),
      mVelocity(0.f, 0.f),
      mGravity(2000.f),
      mJumpForce(-900.f),
      mGroundY(Config::PLAYER_INITIAL_Y),
      mPreJumpTimer(0.f),
      mLandingTimer(0.f),
      mFallingTimer(0.f),
      mLives(3),
      mInvulnerableTimer(0.f),
      mIsInvulnerable(false)
{
    std::string path = Config::IMAGE_PATH;

    // 1. Movimiento Normal
    if (!mTextureIdle.loadFromFile(path + "dexter_default.png")) {
        std::cerr << "Error: No se pudo cargar dexter_default.png" << std::endl;
        // Fallback visual: Cuadrado Rojo
        mTextureIdle.create(50, 150);
        sf::Image img;
        img.create(50, 150, sf::Color::Red);
        mTextureIdle.update(img);
    }

    mTextureRun1.loadFromFile(path + "dexter_paso1.png");
    mTextureRun2.loadFromFile(path + "dexter_paso2.png");
    
    // 2. Agachado
    if (!mTextureCrouch.loadFromFile(path + "dexter_agachado.png")) mTextureCrouch = mTextureIdle;
    if (!mTextureCrouchWalk1.loadFromFile(path + "dexter_agachado_paso1.png")) mTextureCrouchWalk1 = mTextureCrouch;
    if (!mTextureCrouchWalk2.loadFromFile(path + "dexter_agachado_paso2.png")) mTextureCrouchWalk2 = mTextureCrouch;

    // 3. Salto
    if (!mTexturePreJump.loadFromFile(path + "dexter_pre_salto.png")) mTexturePreJump = mTextureCrouch;
    if (!mTextureJump.loadFromFile(path + "dexter_salto.png")) mTextureJump = mTextureRun1;
    if (!mTextureLanding.loadFromFile(path + "dexter_landing.png")) mTextureLanding = mTextureCrouch;

    // 4. Jeringa
    mTextureSyringeIdle.loadFromFile(path + "dexter_default.png");
    if (!mTextureSyringeIdleStatic.loadFromFile(path + "dexter_jeringa.png")) mTextureSyringeIdleStatic = mTextureIdle;
    
    if (!mTextureSyringeRun1.loadFromFile(path + "dexter_paso1_jeringa.png")) mTextureSyringeRun1 = mTextureRun1;
    if (!mTextureSyringeRun2.loadFromFile(path + "dexter_paso2_jeringa.png")) mTextureSyringeRun2 = mTextureRun2;
    
    if (!mTextureSyringeCrouch.loadFromFile(path + "dexter_agachado_jeringa.png")) mTextureSyringeCrouch = mTextureCrouch;
    if (!mTextureSyringeCrouchWalk1.loadFromFile(path + "dexter_agachado_paso1_jeringa.png")) mTextureSyringeCrouchWalk1 = mTextureCrouchWalk1;
    if (!mTextureSyringeCrouchWalk2.loadFromFile(path + "dexter_agachado_paso2_jeringa.png")) mTextureSyringeCrouchWalk2 = mTextureCrouchWalk2;

    // 5. Ataque
    if (!mTextureSyringeAttack.loadFromFile(path + "dexter_jeringa_ataque.png")) mTextureSyringeAttack = mTextureIdle;

    mSprite.setPosition(Config::PLAYER_INITIAL_X, mGroundY);
    
    // FIX: Forzar estado inicial para evitar bug de tamaño
    setState(PlayerState::IDLE);
}

void Player::update(sf::Time deltaTime) {
    float dt = deltaTime.asSeconds();

    // Actualizar invulnerabilidad
    if (mIsInvulnerable) {
        mInvulnerableTimer -= dt;
        if (mInvulnerableTimer <= 0.f) {
            mIsInvulnerable = false;
            mSprite.setColor(sf::Color::White); // Restaurar color
        } else {
            // Parpadeo
            int blink = (int)(mInvulnerableTimer * 20) % 2;
            mSprite.setColor(blink ? sf::Color(255, 255, 255, 100) : sf::Color::White);
        }
    }

    // Lógica de Ataque (Bloqueante)
    if (mIsInSyringeAttack) {
        mAnimationTimer += dt;
        if (mAnimationTimer >= 0.3f) { // Duración del ataque (0.3 segundos)
            mAnimationTimer = 0.f;
            mIsInSyringeAttack = false;
            setState(PlayerState::IDLE, true); // Forzar actualización de textura
        } else {
            // Asegurar que se muestre el frame de ataque
            const sf::Texture* tex = &mTextureSyringeAttack;
            mSprite.setTexture(*tex, true); // Reset rect
            float scale = getTextureScale(tex);
            mSprite.setScale(mFacingLeft ? -scale : scale, scale);
            updateOrigin();
        }
        return;
    }

    handleInput(dt);
    updatePhysics(dt);
    updateAnimation(dt);
}

void Player::handleInput(float dt) {
    if (mCurrentState == PlayerState::HIDING) {
        mVelocity.x = 0.f;
        return;
    }

    if (mCurrentState == PlayerState::PRE_JUMP || mCurrentState == PlayerState::LANDING) {
        mVelocity.x = 0.f;
        return;
    }

    // Toggle Jeringa (Q)
    static bool qPressed = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        if (!qPressed) {
            qPressed = true;
            mHasSyringe = !mHasSyringe;
            updateTextureForCurrentState();
        }
    } else {
        qPressed = false;
    }

    // Ataque (C)
    static bool cPressed = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C) && mHasSyringe) {
        if (!cPressed) {
            cPressed = true;
            mIsInSyringeAttack = true;
            mAnimationTimer = 0.f;
            return;
        }
    } else {
        cPressed = false;
    }

    mVelocity.x = 0.f;
    bool isMoving = false;
    float currentSpeed = mSpeed;

    // Agacharse (Flecha Abajo)
    bool isCrouchingInput = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    
    if (isCrouchingInput) {
        currentSpeed *= 0.5f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        mVelocity.x = -currentSpeed;
        mFacingLeft = true;
        isMoving = true;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        mVelocity.x = currentSpeed;
        mFacingLeft = false;
        isMoving = true;
    }

    bool canJump = (mCurrentState == PlayerState::IDLE || mCurrentState == PlayerState::RUNNING || 
                    mCurrentState == PlayerState::CROUCHING || mCurrentState == PlayerState::CROUCH_WALKING);
                    
    // Saltar (Flecha Arriba)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && canJump) {
        setState(PlayerState::PRE_JUMP);
        mPreJumpTimer = 0.1f;
        mVelocity.x = 0.f;
    }
    else if (canJump) {
        if (isMoving) {
            setState(isCrouchingInput ? PlayerState::CROUCH_WALKING : PlayerState::RUNNING);
        } else {
            setState(isCrouchingInput ? PlayerState::CROUCHING : PlayerState::IDLE);
        }
    }
}

void Player::updatePhysics(float dt) {
    // 1. Gravedad
    mVelocity.y += mGravity * dt;
    
    // Detectar caída libre (Walking off ledge) con umbral de tiempo
    // Si la velocidad es positiva (cayendo) y no estamos ya en estado de salto/aterrizaje
    if (mVelocity.y > 100.f) { // Umbral de velocidad mínima para considerar caída
        mFallingTimer += dt;
        
        // Si lleva cayendo más de 0.2 segundos (ajustable), cambiar a animación de salto
        if (mFallingTimer > 0.2f) {
            if (mCurrentState != PlayerState::JUMPING && 
                mCurrentState != PlayerState::PRE_JUMP && 
                mCurrentState != PlayerState::LANDING && 
                mCurrentState != PlayerState::HIDING) {
                
                setState(PlayerState::JUMPING);
            }
        }
    } else {
        // Resetear timer si no está cayendo
        mFallingTimer = 0.f;
    }

    // 2. Salto (Pre-jump delay)
    if (mCurrentState == PlayerState::PRE_JUMP) {
        mPreJumpTimer -= dt;
        if (mPreJumpTimer <= 0.f) {
            mVelocity.y = mJumpForce;
            setState(PlayerState::JUMPING);
        }
    }
    
    // Nota: El movimiento real (x, y) y las colisiones ahora se manejan en Level::checkCollisions
    // Aquí solo actualizamos estados dependientes del tiempo como el aterrizaje
    
    if (mCurrentState == PlayerState::LANDING) {
        mLandingTimer -= dt;
        if (mLandingTimer <= 0.f) {
            setState(PlayerState::IDLE);
        }
    }
}

sf::Vector2f Player::getVelocity() const {
    return mVelocity;
}

void Player::setVelocity(float x, float y) {
    mVelocity.x = x;
    mVelocity.y = y;
}

void Player::move(float x, float y) {
    mSprite.move(x, y);
}

void Player::setPosition(float x, float y) {
    mSprite.setPosition(x, y);
}

void Player::setGrounded(bool grounded) {
    if (grounded) {
        if (mCurrentState == PlayerState::JUMPING) {
            setState(PlayerState::LANDING);
            mLandingTimer = 0.1f;
        }
    }
}

void Player::updateAnimation(float dt) {
    if (mCurrentState == PlayerState::RUNNING || mCurrentState == PlayerState::CROUCH_WALKING) {
        mAnimationTimer += dt;
        if (mAnimationTimer >= 0.15f) {
            mAnimationTimer = 0.f;
            mIsRunFrame1 = !mIsRunFrame1;
            
            const sf::Texture* tex = nullptr;
            if (mCurrentState == PlayerState::RUNNING) {
                if (mHasSyringe) tex = mIsRunFrame1 ? &mTextureSyringeRun1 : &mTextureSyringeRun2;
                else             tex = mIsRunFrame1 ? &mTextureRun1 : &mTextureRun2;
            } else {
                // FIX: Usar paso 2 normal para la animación con jeringa (solicitud usuario)
                if (mHasSyringe) tex = mIsRunFrame1 ? &mTextureSyringeCrouchWalk1 : &mTextureCrouchWalk2;
                else             tex = mIsRunFrame1 ? &mTextureCrouchWalk1 : &mTextureCrouchWalk2;
            }
            
            if (tex) {
                mSprite.setTexture(*tex, true); // Reset rect
                float scale = getTextureScale(tex);
                mSprite.setScale(mFacingLeft ? -scale : scale, scale);
                updateOrigin();
            }
        }
    }
    
    // Actualizar dirección (flip) si cambió, incluso en el aire
    if (mFacingLeft != mPreviousFacingLeft) {
        float scale = std::abs(mSprite.getScale().y);
        mSprite.setScale(mFacingLeft ? -scale : scale, scale);
        mPreviousFacingLeft = mFacingLeft;
    }
}

void Player::setState(PlayerState newState, bool force) {
    if (mCurrentState == newState && !force) return;
    
    mCurrentState = newState;
    mAnimationTimer = 0.f;
    mIsRunFrame1 = true;

    const sf::Texture* newTex = &mTextureIdle;

    switch (newState) {
        case PlayerState::IDLE:
            newTex = mHasSyringe ? &mTextureSyringeIdleStatic : &mTextureIdle;
            break;
        case PlayerState::RUNNING:
            newTex = mHasSyringe ? &mTextureSyringeRun1 : &mTextureRun1;
            break;
        case PlayerState::CROUCHING:
            newTex = mHasSyringe ? &mTextureSyringeCrouch : &mTextureCrouch;
            break;
        case PlayerState::CROUCH_WALKING:
            newTex = mHasSyringe ? &mTextureSyringeCrouchWalk1 : &mTextureCrouchWalk1;
            break;
        case PlayerState::PRE_JUMP:
            newTex = &mTexturePreJump;
            break;
        case PlayerState::JUMPING:
            newTex = &mTextureJump;
            break;
        case PlayerState::LANDING:
            newTex = &mTextureLanding;
            break;
        case PlayerState::HIDING:
            newTex = &mTextureIdle; // Usar Idle pero modificar color
            break;
    }

    mSprite.setTexture(*newTex, true); // Reset rect
    
    // Efecto visual de escondite
    if (newState == PlayerState::HIDING) {
        mSprite.setColor(sf::Color(255, 255, 255, 100)); // Semitransparente
    } else {
        mSprite.setColor(sf::Color::White); // Normal
    }

    float scale = getTextureScale(newTex);
    mSprite.setScale(mFacingLeft ? -scale : scale, scale);
    updateOrigin();
}

void Player::updateOrigin() {
    sf::FloatRect bounds = mSprite.getLocalBounds();
    mSprite.setOrigin(bounds.width / 2.f, bounds.height);
}

float Player::getTextureScale(const sf::Texture* texture) const {
    if (!texture) return 1.f;
    
    float targetHeight = Config::PLAYER_TARGET_HEIGHT;
    const sf::Texture* referenceTexture = texture;

    // Lógica de Agrupación para consistencia en animaciones
    // Usamos una textura base para calcular la escala de todo el grupo de animación
    // Esto evita que el personaje "respire" (cambie de tamaño) si los frames tienen alturas diferentes

    // 1. Grupo Correr Normal
    if (texture == &mTextureRun1 || texture == &mTextureRun2) {
        referenceTexture = &mTextureRun1;
    }
    // 2. Grupo Correr Jeringa
    else if (texture == &mTextureSyringeRun1 || texture == &mTextureSyringeRun2) {
        referenceTexture = &mTextureSyringeRun1;
    }
    // 3. Grupo Agachado Normal (Quieto)
    else if (texture == &mTextureCrouch) {
        referenceTexture = &mTextureCrouch;
        targetHeight *= 0.7f;
    }
    // 3b. Grupo Agachado Caminando (Normal)
    else if (texture == &mTextureCrouchWalk1 || texture == &mTextureCrouchWalk2) {
        referenceTexture = texture; // Escalar individualmente para forzar altura exacta
        targetHeight *= 0.6f; // Reducir tamaño (60% vs 70% de parado)
    }
    // 4. Grupo Agachado Jeringa (Quieto)
    else if (texture == &mTextureSyringeCrouch) {
        referenceTexture = &mTextureSyringeCrouch;
        targetHeight *= 0.7f;
    }
    // 4b. Grupo Agachado Jeringa (Caminando)
    else if (texture == &mTextureSyringeCrouchWalk1 || texture == &mTextureSyringeCrouchWalk2) {
        referenceTexture = texture; // Escalar individualmente
        targetHeight *= 0.6f; // Reducir tamaño
    }
    // 5. Ataque
    else if (texture == &mTextureSyringeAttack) {
        targetHeight *= 0.85f;
    }

    float currentHeight = (float)referenceTexture->getSize().y;
    if (currentHeight <= 0) return 1.f;
    
    return targetHeight / currentHeight;
}

void Player::updateTextureForCurrentState() {
    setState(mCurrentState, true);
}

void Player::clampToBounds() {
    sf::Vector2f pos = mSprite.getPosition();
    float halfWidth = mSprite.getGlobalBounds().width / 2.f;
    
    // Bloquear movimiento en los bordes
    if (pos.x - halfWidth < 0) {
        pos.x = halfWidth;
        mVelocity.x = 0.f;
    }
    if (pos.x + halfWidth > Config::WORLD_WIDTH) {
        pos.x = Config::WORLD_WIDTH - halfWidth;
        mVelocity.x = 0.f;
    }
    mSprite.setPosition(pos);
}

sf::Vector2f Player::getPosition() const { return mSprite.getPosition(); }
sf::FloatRect Player::getBounds() const { return mSprite.getGlobalBounds(); }
PlayerState Player::getState() const { return mCurrentState; }
bool Player::isHidden() const { return mCurrentState == PlayerState::HIDING; }

void Player::setHidden(bool hidden) {
    if (hidden) {
        setState(PlayerState::HIDING);
    } else {
        setState(PlayerState::IDLE);
    }
}

void Player::takeDamage(int damage) {
    if (mIsInvulnerable || mCurrentState == PlayerState::HIDING) return;

    mLives -= damage;
    if (mLives < 0) mLives = 0;

    // Activar invulnerabilidad
    mIsInvulnerable = true;
    mInvulnerableTimer = 1.0f; // 1 segundo

    // Reproducir sonido de herido (pendiente de implementar Audio Manager o similar)
    // Por ahora solo visual
}

int Player::getLives() const { return mLives; }
bool Player::isDead() const { return mLives <= 0; }

void Player::draw(sf::RenderWindow& window) { window.draw(mSprite); }

