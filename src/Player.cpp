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
      mLandingTimer(0.f)
{
    std::string path = Config::IMAGE_PATH;

    // 1. Movimiento Normal
    mTextureIdle.loadFromFile(path + "dexter_default.png");
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

void Player::update(sf::Time deltaTime, const std::vector<Platform>& platforms) {
    float dt = deltaTime.asSeconds();

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
    updatePhysics(dt, platforms);
    updateAnimation(dt);
}

void Player::handleInput(float dt) {
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

    // Agacharse (S)
    bool isCrouchingInput = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    
    if (isCrouchingInput) {
        currentSpeed *= 0.5f;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        mVelocity.x = -currentSpeed;
        mFacingLeft = true;
        isMoving = true;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        mVelocity.x = currentSpeed;
        mFacingLeft = false;
        isMoving = true;
    }

    bool canJump = (mCurrentState == PlayerState::IDLE || mCurrentState == PlayerState::RUNNING || 
                    mCurrentState == PlayerState::CROUCHING || mCurrentState == PlayerState::CROUCH_WALKING);
                    
    // Saltar (W)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && canJump) {
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

void Player::updatePhysics(float dt, const std::vector<Platform>& platforms) {
    // 1. Movimiento en X
    float moveX = mVelocity.x * dt;
    mSprite.move(moveX, 0.f);
    
    // Colisión X
    sf::FloatRect playerBounds = mSprite.getGlobalBounds();
    for (const auto& platform : platforms) {
        sf::FloatRect platBounds = platform.getBounds();
        if (playerBounds.intersects(platBounds)) {
            // Resolver colisión X
            if (moveX > 0) { // Moviendo derecha
                mSprite.setPosition(platBounds.left - playerBounds.width / 2.f, mSprite.getPosition().y); // Ajuste por origen centrado en X
            } else if (moveX < 0) { // Moviendo izquierda
                mSprite.setPosition(platBounds.left + platBounds.width + playerBounds.width / 2.f, mSprite.getPosition().y);
            }
            mVelocity.x = 0.f;
        }
    }

    // 2. Movimiento en Y
    mVelocity.y += mGravity * dt;
    
    if (mCurrentState == PlayerState::PRE_JUMP) {
        mPreJumpTimer -= dt;
        if (mPreJumpTimer <= 0.f) {
            mVelocity.y = mJumpForce;
            setState(PlayerState::JUMPING);
        }
    }

    float moveY = mVelocity.y * dt;
    mSprite.move(0.f, moveY);

    // Colisión Y (Plataformas)
    playerBounds = mSprite.getGlobalBounds(); // Actualizar bounds tras mover Y
    bool onGround = false;

    for (const auto& platform : platforms) {
        sf::FloatRect platBounds = platform.getBounds();
        if (playerBounds.intersects(platBounds)) {
            // Resolver colisión Y
            if (moveY > 0) { // Cayendo
                // Colocamos al jugador justo encima de la plataforma
                // El origen del sprite está en (width/2, height), es decir, en los pies.
                // Por lo tanto, la posición Y del sprite debe ser igual al top de la plataforma.
                mSprite.setPosition(mSprite.getPosition().x, platBounds.top);
                mVelocity.y = 0.f;
                onGround = true;
                
                if (mCurrentState == PlayerState::JUMPING) {
                    setState(PlayerState::LANDING);
                    mLandingTimer = 0.1f;
                }
            } else if (moveY < 0) { // Saltando hacia arriba (golpeando techo)
                // Colocamos al jugador justo debajo de la plataforma
                // Posición Y = platBounds.top + platBounds.height + spriteHeight
                // Como el origen es bottom, es platBounds.bottom + height? No.
                // Bounds.top es la parte superior. Bounds.top + height es la parte inferior.
                mSprite.setPosition(mSprite.getPosition().x, platBounds.top + platBounds.height + playerBounds.height);
                mVelocity.y = 0.f;
            }
        }
    }

    if (mCurrentState == PlayerState::LANDING) {
        mLandingTimer -= dt;
        if (mLandingTimer <= 0.f) {
            setState(PlayerState::IDLE);
        }
    }

    // Colisión con suelo base (fallback)
    if (mSprite.getPosition().y >= mGroundY) {
        mSprite.setPosition(mSprite.getPosition().x, mGroundY);
        mVelocity.y = 0.f;
        onGround = true;

        if (mCurrentState == PlayerState::JUMPING) {
            setState(PlayerState::LANDING);
            mLandingTimer = 0.1f;
        }
    }

    clampToBounds();
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
                if (mHasSyringe) tex = mIsRunFrame1 ? &mTextureSyringeCrouchWalk1 : &mTextureSyringeCrouchWalk2;
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
    }

    mSprite.setTexture(*newTex, true); // Reset rect
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
    
    // Altura objetivo baseada en RUN1 (referencia de tamaño "normal")
    float targetHeight = (float)mTextureRun1.getSize().y;
    float currentHeight = (float)texture->getSize().y;
    
    if (currentHeight <= 0) return 1.f;

    // Si es agachado (quieto o caminando), reducir la altura objetivo
    // para que visualmente sea más pequeño que estar de pie
    if (texture == &mTextureCrouch || texture == &mTextureSyringeCrouch || 
        texture == &mTextureCrouchWalk1 || texture == &mTextureCrouchWalk2 ||
        texture == &mTextureSyringeCrouchWalk1 || texture == &mTextureSyringeCrouchWalk2) {
        targetHeight *= 0.7f; // 70% de la altura normal
    }
    
    // Ajuste específico para el ataque con jeringa (se veía muy grande)
    if (texture == &mTextureSyringeAttack) {
        targetHeight *= 0.85f; // Reducir un 15% para igualar visualmente
    }
    
    // Calcular escala para que la textura actual tenga la altura objetivo
    return targetHeight / currentHeight;
}

void Player::updateTextureForCurrentState() {
    setState(mCurrentState, true);
}

void Player::clampToBounds() {
    sf::Vector2f pos = mSprite.getPosition();
    float halfWidth = mSprite.getGlobalBounds().width / 2.f;
    if (pos.x - halfWidth < 0) pos.x = halfWidth;
    if (pos.x + halfWidth > Config::WINDOW_WIDTH) pos.x = Config::WINDOW_WIDTH - halfWidth;
    mSprite.setPosition(pos);
}

sf::Vector2f Player::getPosition() const { return mSprite.getPosition(); }
PlayerState Player::getState() const { return mCurrentState; }
void Player::draw(sf::RenderWindow& window) { window.draw(mSprite); }

