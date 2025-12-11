#include "Enemy.hpp"
#include "Player.hpp"
#include <cmath>
#include <iostream>

Enemy::Enemy(sf::Texture* textureWalk1, sf::Texture* textureWalk2, sf::Texture* textureIdle, sf::Texture* textureShoot,
             sf::Vector2f position, float patrolMinX, float patrolMaxX)
    : mTextureWalk1(textureWalk1),
      mTextureWalk2(textureWalk2),
      mTextureIdle(textureIdle),
      mTextureShoot(textureShoot),
      mPatrolStart(patrolMinX),
      mPatrolEnd(patrolMaxX),
      mSpeed(150.f),
      mMovingRight(true),
      mCurrentState(EnemyState::PATROLLING),
      mAnimationTimer(0.f),
      mIsWalkFrame1(true),
      mTurnTimer(0.f),
      mVisionDistance(350.f),
      mVisionAngle(30.f),
      mShootTimer(0.f),
      mShootCooldown(0.f),
      mLostSightTimer(0.f),
      mChasingSoundPlayed(false)
{
    // Cargar Audio
    if (mBufferChasing.loadFromFile(std::string(Config::ASSET_PATH) + "audio/chasing.wav")) {
        mSoundChasing.setBuffer(mBufferChasing);
        mSoundChasing.setLoop(false); // Solo una vez al detectar
    }
    if (mBufferGunshot.loadFromFile(std::string(Config::ASSET_PATH) + "audio/gunshot.wav")) {
        mSoundGunshot.setBuffer(mBufferGunshot);
    }

    // Configurar sprite inicial
    if (mTextureWalk1) {
        mSprite.setTexture(*mTextureWalk1);
    }
    
    // Calcular escala para altura objetivo (similar al Player)
    float scale = getTextureScale();
    mSprite.setScale(scale, scale);
    
    // Configurar origen en el centro-abajo del sprite
    updateOrigin();
    
    // Posicionar el enemigo
    mSprite.setPosition(position);
    
    // Configurar el cono de visión (triángulo)
    mVisionCone.setPointCount(3);
    mVisionCone.setFillColor(sf::Color(0, 0, 255, 60)); // Azul semitransparente (Patrulla)
    mVisionCone.setOutlineColor(sf::Color(0, 0, 200, 100));
    mVisionCone.setOutlineThickness(1.f);
    
    updateVisionCone();
}

float Enemy::getTextureScale() const {
    if (!mTextureWalk1) return 1.f;
    
    // Altura objetivo para el enemigo (290px)
    float targetHeight = 290.f;
    float currentHeight = static_cast<float>(mTextureWalk1->getSize().y);
    
    if (currentHeight <= 0) return 1.f;
    
    return targetHeight / currentHeight;
}

void Enemy::updateOrigin() {
    sf::FloatRect bounds = mSprite.getLocalBounds();
    // Origen en centro-abajo del sprite (pies del personaje)
    mSprite.setOrigin(bounds.width / 2.f, bounds.height);
}

void Enemy::update(float dt, Player& player) {
    // Actualizar timers
    if (mShootCooldown > 0.f) mShootCooldown -= dt;

    // Máquina de Estados
    switch (mCurrentState) {
        case EnemyState::PATROLLING:
            {
                // Detección
                if (checkPlayerDetection(player)) {
                    mCurrentState = EnemyState::CHASING;
                    mVisionCone.setFillColor(sf::Color(255, 0, 0, 60)); // Rojo
                    mSpeed = 300.f; // Correr
                    if (!mChasingSoundPlayed) {
                        mSoundChasing.play();
                        mChasingSoundPlayed = true;
                    }
                    break;
                }

                // Mover en la dirección actual
                float moveAmount = mSpeed * dt * (mMovingRight ? 1.f : -1.f);
                mSprite.move(moveAmount, 0.f);
                
                // Verificar límites de patrulla
                float posX = mSprite.getPosition().x;
                
                if (mMovingRight && posX >= mPatrolEnd) {
                    mSprite.setPosition(mPatrolEnd, mSprite.getPosition().y);
                    mCurrentState = EnemyState::TURNING;
                    mTurnTimer = 0.5f; // Pausa de medio segundo
                    
                    // Mostrar textura idle al girar
                    if (mTextureIdle) {
                        mSprite.setTexture(*mTextureIdle, true);
                        float scale = getTextureScale();
                        mSprite.setScale(mMovingRight ? -scale : scale, scale);
                        updateOrigin();
                    }
                }
                else if (!mMovingRight && posX <= mPatrolStart) {
                    mSprite.setPosition(mPatrolStart, mSprite.getPosition().y);
                    mCurrentState = EnemyState::TURNING;
                    mTurnTimer = 0.5f;
                    
                    if (mTextureIdle) {
                        mSprite.setTexture(*mTextureIdle, true);
                        float scale = getTextureScale();
                        mSprite.setScale(mMovingRight ? -scale : scale, scale);
                        updateOrigin();
                    }
                }
                
                updateAnimation(dt);
            }
            break;

        case EnemyState::TURNING:
            mTurnTimer -= dt;
            if (mTurnTimer <= 0.f) {
                mMovingRight = !mMovingRight;
                mCurrentState = EnemyState::PATROLLING;
                
                // Voltear sprite
                float scale = std::abs(mSprite.getScale().x);
                mSprite.setScale(mMovingRight ? scale : -scale, scale);
            }
            break;

        case EnemyState::CHASING:
            {
                // Verificar si sigue viendo al jugador
                bool canSee = checkPlayerDetection(player);
                float distToPlayer = std::abs(player.getPosition().x - mSprite.getPosition().x);

                if (!canSee) {
                    mLostSightTimer += dt;
                    if (mLostSightTimer > 2.0f) { // 2 segundos sin ver -> Volver a patrullar
                        mCurrentState = EnemyState::PATROLLING;
                        mVisionCone.setFillColor(sf::Color(0, 0, 255, 60)); // Azul
                        mSpeed = 150.f; // Velocidad normal
                        mChasingSoundPlayed = false;
                        mLostSightTimer = 0.f;
                    }
                } else {
                    mLostSightTimer = 0.f;
                    
                    // Si está muy cerca, disparar
                    if (distToPlayer < 300.f) { // Rango de disparo
                        mCurrentState = EnemyState::SHOOTING;
                        mShootTimer = 0.5f; // Tiempo de apuntado
                        // Cambiar textura a disparo
                        if (mTextureShoot) {
                            mSprite.setTexture(*mTextureShoot, true);
                            updateOrigin();
                        }
                        break;
                    }
                }

                // Moverse hacia el jugador
                float playerX = player.getPosition().x;
                float myX = mSprite.getPosition().x;
                
                // Orientación
                if (playerX > myX) mMovingRight = true;
                else mMovingRight = false;
                
                float scale = std::abs(mSprite.getScale().x);
                mSprite.setScale(mMovingRight ? scale : -scale, scale);

                // Movimiento
                float moveAmount = mSpeed * dt * (mMovingRight ? 1.f : -1.f);
                mSprite.move(moveAmount, 0.f);
                
                updateAnimation(dt);
            }
            break;

        case EnemyState::SHOOTING:
            {
                // Orientar hacia jugador
                float playerX = player.getPosition().x;
                float myX = mSprite.getPosition().x;
                if (playerX > myX) mMovingRight = true;
                else mMovingRight = false;
                float scale = std::abs(mSprite.getScale().x);
                mSprite.setScale(mMovingRight ? scale : -scale, scale);

                mShootTimer -= dt;
                if (mShootTimer <= 0.f) {
                    // Disparar
                    if (mShootCooldown <= 0.f) {
                        player.takeDamage(1);
                        mSoundGunshot.play();
                        mShootCooldown = 1.5f;
                    }
                    
                    // Volver a perseguir
                    mCurrentState = EnemyState::CHASING;
                    // Restaurar textura normal en el siguiente updateAnimation
                }
            }
            break;
    }
    
    updateVisionCone();
}

void Enemy::updateAnimation(float dt) {
    if (mCurrentState != EnemyState::PATROLLING) return;
    
    mAnimationTimer += dt;
    if (mAnimationTimer >= 0.2f) { // Cambiar frame cada 0.2 segundos
        mAnimationTimer = 0.f;
        mIsWalkFrame1 = !mIsWalkFrame1;
        
        sf::Texture* tex = mIsWalkFrame1 ? mTextureWalk1 : mTextureWalk2;
        if (tex) {
            mSprite.setTexture(*tex, true);
            float scale = getTextureScale();
            mSprite.setScale(mMovingRight ? scale : -scale, scale);
            updateOrigin();
        }
    }
}

void Enemy::updateVisionCone() {
    // El cono sale de la posición de los "ojos" del enemigo
    sf::Vector2f pos = mSprite.getPosition(); // Posición de los pies (centro-inferior)
    
    // Calcular dimensiones reales del sprite (independiente de la escala)
    sf::FloatRect localBounds = mSprite.getLocalBounds();
    sf::Vector2f scale = mSprite.getScale();
    float spriteWidth = localBounds.width * std::abs(scale.x);
    float spriteHeight = localBounds.height * std::abs(scale.y);
    
    // Punto de origen del cono (ojos del enemigo)
    // Los pies están en pos.y, la cabeza está en pos.y - spriteHeight
    // Los ojos están aproximadamente al 85% de altura desde los pies
    float eyeY = pos.y - spriteHeight * 0.85f;
    
    // El cono debe salir desde el frente de la cara
    // Cuando mira a la derecha (mMovingRight = true), los ojos están a la derecha del centro
    // Cuando mira a la izquierda (mMovingRight = false), los ojos están a la izquierda del centro
    float direction = mMovingRight ? 1.f : -1.f;
    
    // Offset horizontal desde el centro del sprite hacia la cara
    // La cara está aproximadamente al 40% del ancho desde el centro hacia la dirección que mira
    float faceOffset = spriteWidth * 0.4f * direction;
    float eyeX = pos.x + faceOffset;
    
    // Convertir ángulo a radianes
    float angleRad = mVisionAngle * 3.14159f / 180.f;
    
    // Calcular los tres puntos del triángulo
    // Punto 0: Origen (ojos)
    mVisionCone.setPoint(0, sf::Vector2f(eyeX, eyeY));
    
    // Punto 1: Esquina superior del cono
    float endX1 = eyeX + direction * mVisionDistance;
    float endY1 = eyeY - mVisionDistance * std::tan(angleRad);
    mVisionCone.setPoint(1, sf::Vector2f(endX1, endY1));
    
    // Punto 2: Esquina inferior del cono
    float endX2 = eyeX + direction * mVisionDistance;
    float endY2 = eyeY + mVisionDistance * std::tan(angleRad);
    mVisionCone.setPoint(2, sf::Vector2f(endX2, endY2));
}

bool Enemy::checkPlayerDetection(const Player& player) const {
    // Si el jugador está escondido, no puede ser detectado
    if (player.isHidden()) {
        return false;
    }
    
    // Verificar si el jugador intersecta con el cono de visión
    // Usamos los bounds del cono para una detección simplificada
    sf::FloatRect coneBounds = mVisionCone.getGlobalBounds();
    sf::FloatRect playerBounds = player.getBounds();
    
    if (!coneBounds.intersects(playerBounds)) {
        return false;
    }
    
    // Verificación adicional: el jugador debe estar en la dirección correcta
    sf::Vector2f playerPos = player.getPosition();
    sf::Vector2f enemyPos = mSprite.getPosition();
    
    if (mMovingRight && playerPos.x < enemyPos.x) {
        return false; // El enemigo mira a la derecha pero el jugador está a la izquierda
    }
    if (!mMovingRight && playerPos.x > enemyPos.x) {
        return false; // El enemigo mira a la izquierda pero el jugador está a la derecha
    }
    
    // Verificar distancia
    float distance = std::abs(playerPos.x - enemyPos.x);
    if (distance > mVisionDistance) {
        return false;
    }
    
    return true;
}

void Enemy::draw(sf::RenderWindow& window) {
    // Dibujar el cono de visión primero (detrás del sprite)
    window.draw(mVisionCone);
    
    // Dibujar el sprite del enemigo
    window.draw(mSprite);
}

sf::Vector2f Enemy::getPosition() const {
    return mSprite.getPosition();
}

sf::FloatRect Enemy::getBounds() const {
    return mSprite.getGlobalBounds();
}
