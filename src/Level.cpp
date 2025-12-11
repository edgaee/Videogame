#include "Level.hpp"
#include "Config.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

Level::Level() {
    // Cargar texturas individuales
    std::string path = Config::IMAGE_PATH;
    if (!mTexFloor.loadFromFile(path + "tile_floor.png")) std::cerr << "Error floor" << std::endl;
    if (!mTexVent.loadFromFile(path + "tile_vent1.png")) std::cerr << "Error vent" << std::endl;
    
    // Props
    if (!mTexProp1.loadFromFile(path + "archivero.png")) std::cerr << "Error archivero" << std::endl;
    if (!mTexProp2.loadFromFile(path + "escritorio.png")) std::cerr << "Error escritorio" << std::endl;

    // Enemigos
    if (!mTexEnemyWalk1.loadFromFile(path + "policia1_caminando1.png")) std::cerr << "Error enemy w1" << std::endl;
    if (!mTexEnemyWalk2.loadFromFile(path + "policia1_caminando2.png")) std::cerr << "Error enemy w2" << std::endl;
    if (!mTexEnemyIdle.loadFromFile(path + "policia1_default.png")) std::cerr << "Error enemy idle" << std::endl;
    if (!mTexEnemyShoot.loadFromFile(path + "policia1_arma2.png")) std::cerr << "Error enemy shoot" << std::endl;
    
    // Bala
    if (!mTexBullet.loadFromFile(path + "bala.png")) std::cerr << "Error bullet" << std::endl;
}

void Level::loadLevel1() {
    mPlatforms.clear();
    mHidingSpots.clear();
    mEnemies.clear();
    mBullets.clear();

    // --- DISEÑO NIVEL 1: MIAMI METRO POLICE ---
    
    // 1. Generación del Suelo (Fila de tiles)
    // Empezamos en X=0, Y=1000 (Suelo base del jugador)
    // Cada tile mide 64x64 (asumido por tile_floor.png)
    float startX = 0.f;
    float groundY = 1000.f;
    int numTiles = 40; // 40 tiles * 64px = 2560px (ancho pantalla)

    for (int i = 0; i < numTiles; ++i) {
        mPlatforms.emplace_back(&mTexFloor, sf::Vector2f(startX + i * 64.f, groundY));
    }

    // --- ESCONDITES ---
    // 1. Puerta oscura en el pasillo inicial
    mHidingSpots.emplace_back(sf::Vector2f(80.f, 150.f), sf::Vector2f(400.f, 850.f));

    // 2. Zona oscura bajo el conducto derecho
    mHidingSpots.emplace_back(sf::Vector2f(100.f, 150.f), sf::Vector2f(1300.f, 850.f));
    
    // --- ENEMIGOS ---
    // Doakes patrullando el pasillo principal
    // Ajuste de altura: El suelo está en Y=1000.f. 
    // 1075.f se hunde un poco. Probamos subiéndolo a 1050.f.
    mEnemies.emplace_back(&mTexEnemyWalk1, &mTexEnemyWalk2, &mTexEnemyIdle, &mTexEnemyShoot,
                          sf::Vector2f(1800.f, 1050.f), 1500.f, 2200.f);
}

void Level::update(float dt, Player& player) {
    // Obtener hitbox del ataque del jugador
    sf::FloatRect attackBounds;
    bool playerAttacking = player.isAttacking();
    if (playerAttacking) {
        attackBounds = player.getAttackBounds();
    }
    
    // Resetear estado de detección
    mPlayerDetected = false;
    
    // Actualizar enemigos
    for (auto it = mEnemies.begin(); it != mEnemies.end(); ) {
        // Verificar si el ataque del jugador golpea al enemigo
        if (playerAttacking && !it->isDying() && !it->isDead()) {
            sf::FloatRect enemyBounds = it->getBounds();
            if (attackBounds.intersects(enemyBounds)) {
                it->kill();
            }
        }
        
        it->update(dt, player);
        
        // Verificar si este enemigo está persiguiendo al jugador
        if (it->isChasing()) {
            mPlayerDetected = true;
        }
        
        // Verificar si el enemigo quiere disparar
        if (it->wantsToShoot() && !player.isDead()) {
            it->confirmShot();
            
            // Crear bala
            Bullet bullet;
            bullet.sprite.setTexture(mTexBullet);
            
            // Posición inicial: pistola del enemigo
            sf::Vector2f gunPos = it->getGunPosition();
            bullet.sprite.setPosition(gunPos);
            
            // Escalar la bala (muy pequeña)
            bullet.sprite.setScale(0.08f, 0.08f);
            
            // Centrar origen
            sf::FloatRect bulletBounds = bullet.sprite.getLocalBounds();
            bullet.sprite.setOrigin(bulletBounds.width / 2.f, bulletBounds.height / 2.f);
            
            // Calcular destino (misma altura, hacia el jugador en línea recta)
            sf::FloatRect playerBounds = player.getBounds();
            float playerCenterX = playerBounds.left + playerBounds.width / 2.f;
            bullet.targetPos = sf::Vector2f(playerCenterX, gunPos.y);  // Misma altura Y
            
            // Calcular distancia horizontal y tiempo de viaje
            float distance = std::abs(bullet.targetPos.x - gunPos.x);
            
            // Velocidad de la bala (800 px/s)
            float bulletSpeed = 800.f;
            bullet.travelTime = distance / bulletSpeed;
            bullet.elapsedTime = 0.f;
            
            // Calcular velocidad
            bullet.velocity = sf::Vector2f(
                (bullet.targetPos.x - gunPos.x) / bullet.travelTime,
                (bullet.targetPos.y - gunPos.y) / bullet.travelTime
            );
            
            // Rotar la bala para que apunte hacia el jugador
            float angle = std::atan2(
                bullet.targetPos.y - gunPos.y,
                bullet.targetPos.x - gunPos.x
            ) * 180.f / 3.14159f;
            bullet.sprite.setRotation(angle);
            
            bullet.active = true;
            mBullets.push_back(bullet);
        }
        
        // Eliminar enemigos cuya animación de muerte terminó
        if (it->shouldRemove()) {
            it = mEnemies.erase(it);
        } else {
            ++it;
        }
    }
    
    // Actualizar balas
    for (auto it = mBullets.begin(); it != mBullets.end(); ) {
        it->elapsedTime += dt;
        it->sprite.move(it->velocity.x * dt, it->velocity.y * dt);
        
        // Verificar si la bala llegó al destino
        if (it->elapsedTime >= it->travelTime) {
            // Hacer daño al jugador si no está muerto ni escondido
            if (!player.isDead() && !player.isHidden()) {
                player.takeDamage(1);
            }
            it = mBullets.erase(it);
        } else {
            ++it;
        }
    }
}

void Level::draw(sf::RenderWindow& window) {
    // Dibujar Escondites (Fondo)
    for (auto& spot : mHidingSpots) {
        spot.draw(window);
    }

    // Dibujar Plataformas
    for (auto& platform : mPlatforms) {
        platform.draw(window);
    }
    
    // Dibujar Enemigos
    for (auto& enemy : mEnemies) {
        enemy.draw(window);
    }
    
    // Dibujar Balas
    for (auto& bullet : mBullets) {
        window.draw(bullet.sprite);
    }
}

const std::vector<HidingSpot>& Level::getHidingSpots() const {
    return mHidingSpots;
}

bool Level::isPlayerDetected() const {
    return mPlayerDetected;
}

void Level::checkCollisions(Player& player, float dt) {
    sf::Vector2f velocity = player.getVelocity();
    
    // Limitar velocidad máxima de caída
    const float maxFallSpeed = 500.f;
    if (velocity.y > maxFallSpeed) {
        velocity.y = maxFallSpeed;
        player.setVelocity(velocity.x, velocity.y);
    }
    
    // Guardar posición inicial
    sf::Vector2f startPos = player.getPosition();
    
    // === MOVIMIENTO Y COLISIÓN EN X ===
    float moveX = velocity.x * dt;
    if (moveX != 0.f) {
        player.move(moveX, 0.f);
        sf::FloatRect playerBounds = player.getBounds();
        
        for (const auto& platform : mPlatforms) {
            sf::FloatRect platBounds = platform.getBounds();
            sf::FloatRect intersection;
            
            if (playerBounds.intersects(platBounds, intersection)) {
                // Solo corregir colisión lateral si:
                // 1. La intersección es más alta que ancha (colisión lateral, no vertical)
                // 2. La intersección tiene una altura significativa
                if (intersection.height > intersection.width && intersection.height > 10.f) {
                    // Revertir el movimiento X en lugar de reposicionar
                    player.move(-moveX, 0.f);
                    velocity.x = 0.f;
                    player.setVelocity(0.f, velocity.y);
                    break;
                }
            }
        }
    }

    // === MOVIMIENTO Y COLISIÓN EN Y ===
    float moveY = velocity.y * dt;
    player.move(0.f, moveY);
    
    sf::FloatRect playerBounds = player.getBounds();
    float playerCenterX = player.getPosition().x;
    float playerHalfWidth = playerBounds.width / 2.f;
    float playerPosY = player.getPosition().y;

    for (const auto& platform : mPlatforms) {
        sf::FloatRect platBounds = platform.getBounds();
        sf::FloatRect intersection;
        
        if (playerBounds.intersects(platBounds, intersection)) {
            // Calcular superposición horizontal real
            float overlapLeft = std::max(playerCenterX - playerHalfWidth, platBounds.left);
            float overlapRight = std::min(playerCenterX + playerHalfWidth, platBounds.left + platBounds.width);
            float overlapWidth = overlapRight - overlapLeft;
            float requiredOverlap = playerBounds.width * 0.15f; // 15% mínimo
            
            if (overlapWidth >= requiredOverlap) {
                if (velocity.y > 0) { // Cayendo
                    // Aterrizar si:
                    // 1. Estamos cayendo (velocity.y > 0)
                    // 2. Los pies del jugador están cerca del tope de la plataforma
                    // 3. La intersección es superficial (no estamos muy adentro)
                    if (intersection.height <= std::abs(moveY) + 20.f) {
                        player.setPosition(player.getPosition().x, platBounds.top);
                        player.setVelocity(player.getVelocity().x, 0.f);
                        player.setGrounded(true);
                        break; // Solo aterrizar en una plataforma
                    }
                } else if (velocity.y < 0) { // Saltando
                    // Golpear techo si la intersección es superficial
                    if (intersection.height <= std::abs(moveY) + 10.f) {
                        player.setVelocity(player.getVelocity().x, 0.f);
                    }
                }
            }
        }
    }
    
    // Fallback: respawnear si cae del mapa
    if (player.getPosition().y > 1200.f) { 
         player.setPosition(Config::PLAYER_INITIAL_X, Config::PLAYER_INITIAL_Y);
         player.setVelocity(0.f, 0.f);
         player.setGrounded(true);
    }
}
