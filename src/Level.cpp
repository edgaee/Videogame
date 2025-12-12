#include "Level.hpp"
#include "Config.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

Level::Level() : mPlayerDetected(false), mCollectedCount(0) {
    // Cargar texturas individuales
    std::string path = Config::IMAGE_PATH;
    if (!mTexFloor.loadFromFile(path + "tile_floor.png")) std::cerr << "Error floor" << std::endl;
    if (!mTexVent.loadFromFile(path + "tile_vent1.png")) std::cerr << "Error vent" << std::endl;
    
    // Props
    if (!mTexProp1.loadFromFile(path + "archivero1.png")) std::cerr << "Error archivero1" << std::endl;
    if (!mTexProp2.loadFromFile(path + "escritorio.png")) std::cerr << "Error escritorio" << std::endl;

    // === POLICÍA 1 ===
    if (!mTexPolice1_Idle.loadFromFile(path + "policia1_default.png")) std::cerr << "Error p1 idle" << std::endl;
    if (!mTexPolice1_Walk1.loadFromFile(path + "policia1_caminando1.png")) std::cerr << "Error p1 w1" << std::endl;
    if (!mTexPolice1_Walk2.loadFromFile(path + "policia1_caminando2.png")) std::cerr << "Error p1 w2" << std::endl;
    if (!mTexPolice1_Aim.loadFromFile(path + "policia1_arma1.png")) std::cerr << "Error p1 aim" << std::endl;
    if (!mTexPolice1_Shoot.loadFromFile(path + "policia1_arma2.png")) std::cerr << "Error p1 shoot" << std::endl;
    if (!mTexPolice1_Death1.loadFromFile(path + "policia1_muerte1.png")) std::cerr << "Error p1 d1" << std::endl;
    if (!mTexPolice1_Death2.loadFromFile(path + "policia1_muerte2.png")) std::cerr << "Error p1 d2" << std::endl;
    if (!mTexPolice1_Dead.loadFromFile(path + "policia1_muerto.png")) std::cerr << "Error p1 dead" << std::endl;
    
    // === POLICÍA 2 ===
    if (!mTexPolice2_Idle.loadFromFile(path + "policia2_default.png")) std::cerr << "Error p2 idle" << std::endl;
    if (!mTexPolice2_Walk1.loadFromFile(path + "policia2_caminando1.png")) std::cerr << "Error p2 w1" << std::endl;
    if (!mTexPolice2_Walk2.loadFromFile(path + "policia2_caminando2.png")) std::cerr << "Error p2 w2" << std::endl;
    if (!mTexPolice2_Aim.loadFromFile(path + "policia2_arma1.png")) std::cerr << "Error p2 aim" << std::endl;
    if (!mTexPolice2_Shoot.loadFromFile(path + "policia2_arma2.png")) std::cerr << "Error p2 shoot" << std::endl;
    if (!mTexPolice2_Death1.loadFromFile(path + "policia2_muerto1.png")) std::cerr << "Error p2 d1" << std::endl;
    if (!mTexPolice2_Death2.loadFromFile(path + "Policia2_muerto2.png")) std::cerr << "Error p2 d2" << std::endl;
    if (!mTexPolice2_Dead.loadFromFile(path + "policia2_muerto.png")) std::cerr << "Error p2 dead" << std::endl;
    
    // === DOAKES (BOSS) ===
    if (!mTexDoakes_Idle.loadFromFile(path + "Doakes_default.png")) std::cerr << "Error doakes idle" << std::endl;
    if (!mTexDoakes_Walk1.loadFromFile(path + "doakes_caminando1_arma.png")) std::cerr << "Error doakes w1" << std::endl;
    if (!mTexDoakes_Walk2.loadFromFile(path + "doakes_caminando2_arma.png")) std::cerr << "Error doakes w2" << std::endl;
    if (!mTexDoakes_Aim.loadFromFile(path + "doakes_arma_apuntando.png")) std::cerr << "Error doakes aim" << std::endl;
    if (!mTexDoakes_Shoot.loadFromFile(path + "doakes_arma_disparo.png")) std::cerr << "Error doakes shoot" << std::endl;
    if (!mTexDoakes_Dead.loadFromFile(path + "doakes_tirado.png")) std::cerr << "Error doakes dead" << std::endl;
    
    // Bala
    if (!mTexBullet.loadFromFile(path + "bala.png")) std::cerr << "Error bullet" << std::endl;
    
    // BloodSlide
    if (!mTexBloodSlide.loadFromFile(path + "bloodslides.png")) std::cerr << "Error bloodslide" << std::endl;
}

void Level::loadLevel1() {
    mPlatforms.clear();
    mHidingSpots.clear();
    mEnemies.clear();
    mBullets.clear();
    mCollectibles.clear();
    mCollectedCount = 0;

    // --- DISEÑO NIVEL 1: SIDE-SCROLLER LINEAL ---
    // Mundo: 6000px de ancho, suelo en Y=1000
    
    // === 1. SUELO CONTINUO ===
    float groundY = 1000.f;
    int numTiles = 95; // 95 tiles * 64px = 6080px (cubre todo el mundo expandido)

    for (int i = 0; i < numTiles; ++i) {
        mPlatforms.emplace_back(&mTexFloor, sf::Vector2f(i * 64.f, groundY));
    }
    
    // === 2. PLATAFORMAS FLOTANTES (Conductos) ===
    // Distribuidas a lo largo del nivel para permitir movimiento vertical
    
    // Zona temprana (x=300-800)
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(350.f, 800.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(414.f, 800.f));
    
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(600.f, 650.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(664.f, 650.f));
    
    // Zona media-baja (x=1000-1600)
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(1100.f, 750.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(1164.f, 750.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(1228.f, 750.f));
    
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(1400.f, 600.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(1464.f, 600.f));
    
    // Zona media (x=2000-2800)
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(2100.f, 700.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(2164.f, 700.f));
    
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(2500.f, 800.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(2564.f, 800.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(2628.f, 800.f));
    
    // Zona media-alta (x=3200-3800)
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(3200.f, 750.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(3264.f, 750.f));
    
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(3500.f, 600.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(3564.f, 600.f));
    
    // Zona avanzada (x=4000-4600)
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(4100.f, 800.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(4164.f, 800.f));
    
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(4400.f, 650.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(4464.f, 650.f));
    
    // Zona final - Boss (x=5000-5600)
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(5000.f, 750.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(5064.f, 750.f));
    
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(5300.f, 600.f));
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(5364.f, 600.f));
    
    // === 3. ESCONDITES (Archiveros) ===
    // Distribuidos cada 500-700px para permitir sigilo
    // Escala 0.3f para que sean del tamaño de Dexter
    mHidingSpots.emplace_back(&mTexProp1, sf::Vector2f(400.f, 1000.f), 0.3f);
    mHidingSpots.emplace_back(&mTexProp1, sf::Vector2f(1000.f, 1000.f), 0.3f);
    mHidingSpots.emplace_back(&mTexProp1, sf::Vector2f(1600.f, 1000.f), 0.3f);
    mHidingSpots.emplace_back(&mTexProp1, sf::Vector2f(2300.f, 1000.f), 0.3f);
    mHidingSpots.emplace_back(&mTexProp1, sf::Vector2f(3000.f, 1000.f), 0.3f);
    mHidingSpots.emplace_back(&mTexProp1, sf::Vector2f(3700.f, 1000.f), 0.3f);
    mHidingSpots.emplace_back(&mTexProp1, sf::Vector2f(4400.f, 1000.f), 0.3f);
    mHidingSpots.emplace_back(&mTexProp1, sf::Vector2f(5100.f, 1000.f), 0.3f);

    
    // === 4. COLECCIONABLES (Blood Slides) ===
    // 5 blood slides distribuidos por el nivel
    
    // BloodSlide 1: En el suelo temprano
    Collectible bs1;
    bs1.sprite.setTexture(mTexBloodSlide);
    bs1.sprite.setScale(0.5f, 0.5f);
    bs1.position = sf::Vector2f(700.f, 950.f);
    bs1.sprite.setPosition(bs1.position);
    sf::FloatRect bs1Bounds = bs1.sprite.getLocalBounds();
    bs1.sprite.setOrigin(bs1Bounds.width / 2.f, bs1Bounds.height / 2.f);
    mCollectibles.push_back(bs1);
    
    // BloodSlide 2: En plataforma alta
    Collectible bs2;
    bs2.sprite.setTexture(mTexBloodSlide);
    bs2.sprite.setScale(0.5f, 0.5f);
    bs2.position = sf::Vector2f(1400.f, 700.f);
    bs2.sprite.setPosition(bs2.position);
    sf::FloatRect bs2Bounds = bs2.sprite.getLocalBounds();
    bs2.sprite.setOrigin(bs2Bounds.width / 2.f, bs2Bounds.height / 2.f);
    mCollectibles.push_back(bs2);
    
    // BloodSlide 3: En el suelo zona media
    Collectible bs3;
    bs3.sprite.setTexture(mTexBloodSlide);
    bs3.sprite.setScale(0.5f, 0.5f);
    bs3.position = sf::Vector2f(2500.f, 950.f);
    bs3.sprite.setPosition(bs3.position);
    sf::FloatRect bs3Bounds = bs3.sprite.getLocalBounds();
    bs3.sprite.setOrigin(bs3Bounds.width / 2.f, bs3Bounds.height / 2.f);
    mCollectibles.push_back(bs3);
    
    // BloodSlide 4: En plataforma alta zona avanzada
    Collectible bs4;
    bs4.sprite.setTexture(mTexBloodSlide);
    bs4.sprite.setScale(0.5f, 0.5f);
    bs4.position = sf::Vector2f(3500.f, 750.f);
    bs4.sprite.setPosition(bs4.position);
    sf::FloatRect bs4Bounds = bs4.sprite.getLocalBounds();
    bs4.sprite.setOrigin(bs4Bounds.width / 2.f, bs4Bounds.height / 2.f);
    mCollectibles.push_back(bs4);
    
    // BloodSlide 5: Cerca del boss
    Collectible bs5;
    bs5.sprite.setTexture(mTexBloodSlide);
    bs5.sprite.setScale(0.5f, 0.5f);
    bs5.position = sf::Vector2f(4800.f, 950.f);
    bs5.sprite.setPosition(bs5.position);
    sf::FloatRect bs5Bounds = bs5.sprite.getLocalBounds();
    bs5.sprite.setOrigin(bs5Bounds.width / 2.f, bs5Bounds.height / 2.f);
    mCollectibles.push_back(bs5);
    
    // === 5. ENEMIGOS SECUENCIALES ===
    
    // Configurar texturas para Policía 1
    EnemyTextures police1Tex;
    police1Tex.idle = &mTexPolice1_Idle;
    police1Tex.walk1 = &mTexPolice1_Walk1;
    police1Tex.walk2 = &mTexPolice1_Walk2;
    police1Tex.aim = &mTexPolice1_Aim;
    police1Tex.shoot = &mTexPolice1_Shoot;
    police1Tex.death1 = &mTexPolice1_Death1;
    police1Tex.death2 = &mTexPolice1_Death2;
    police1Tex.dead = &mTexPolice1_Dead;
    
    // Configurar texturas para Policía 2
    EnemyTextures police2Tex;
    police2Tex.idle = &mTexPolice2_Idle;
    police2Tex.walk1 = &mTexPolice2_Walk1;
    police2Tex.walk2 = &mTexPolice2_Walk2;
    police2Tex.aim = &mTexPolice2_Aim;
    police2Tex.shoot = &mTexPolice2_Shoot;
    police2Tex.death1 = &mTexPolice2_Death1;
    police2Tex.death2 = &mTexPolice2_Death2;
    police2Tex.dead = &mTexPolice2_Dead;
    
    // Configurar texturas para Doakes (Boss)
    EnemyTextures doakesTex;
    doakesTex.idle = &mTexDoakes_Idle;
    doakesTex.walk1 = &mTexDoakes_Walk1;
    doakesTex.walk2 = &mTexDoakes_Walk2;
    doakesTex.aim = &mTexDoakes_Aim;
    doakesTex.shoot = &mTexDoakes_Shoot;
    doakesTex.death1 = nullptr; // Doakes no tiene animación de muerte
    doakesTex.death2 = nullptr;
    doakesTex.dead = &mTexDoakes_Dead;
    
    // Crear enemigos en posiciones específicas (distribuidos en mapa largo)
    // x=1200: Policía 1 (Patrullando 1000-1400)
    mEnemies.emplace_back(police1Tex, EnemyType::POLICE1, 
                          sf::Vector2f(1200.f, 1060.f), 1000.f, 1400.f);
    
    // x=2000: Policía 2 (Patrullando 1800-2200)
    mEnemies.emplace_back(police2Tex, EnemyType::POLICE2, 
                          sf::Vector2f(2000.f, 1060.f), 1800.f, 2200.f);
    
    // x=2900: Policía 1 segunda instancia (Patrullando 2700-3100)
    mEnemies.emplace_back(police1Tex, EnemyType::POLICE1, 
                          sf::Vector2f(2900.f, 1060.f), 2700.f, 3100.f);
    
    // x=3800: Policía 2 segunda instancia (Patrullando 3600-4000)
    mEnemies.emplace_back(police2Tex, EnemyType::POLICE2, 
                          sf::Vector2f(3800.f, 1060.f), 3600.f, 4000.f);
    
    // x=5200: DOAKES (BOSS) - Patrullando zona final (5000-5400)
    mEnemies.emplace_back(doakesTex, EnemyType::BOSS, 
                          sf::Vector2f(5200.f, 1040.f), 5000.f, 5400.f);
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
    
    // === ACTUALIZAR COLECCIONABLES ===
    sf::FloatRect playerBounds = player.getBounds();
    for (auto& collectible : mCollectibles) {
        if (!collectible.collected) {
            sf::FloatRect collectibleBounds = collectible.sprite.getGlobalBounds();
            if (playerBounds.intersects(collectibleBounds)) {
                collectible.collected = true;
                mCollectedCount++;
            }
        }
    }
    
    // === ACTUALIZAR ENEMIGOS ===
    for (auto it = mEnemies.begin(); it != mEnemies.end(); ) {
        // Verificar si el ataque del jugador golpea al enemigo
        if (playerAttacking && !it->isDying() && !it->isDead()) {
            sf::FloatRect enemyBounds = it->getBounds();
            if (attackBounds.intersects(enemyBounds)) {
                it->takeDamage(1); // Infligir 1 de daño (Boss requiere 5 golpes)
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
            
            // Verificar si el jugador está aproximadamente a la misma altura
            sf::FloatRect playerBounds = player.getBounds();
            sf::Vector2f gunPos = it->getGunPosition();
            float playerCenterY = playerBounds.top + playerBounds.height / 2.f;
            float heightDifference = std::abs(playerCenterY - gunPos.y);
            
            // Solo disparar si la diferencia de altura es menor a 150px
            if (heightDifference < 150.f) {
                // Crear bala
                Bullet bullet;
                bullet.sprite.setTexture(mTexBullet);
                
                // Posición inicial: pistola del enemigo
                bullet.sprite.setPosition(gunPos);
                
                // Escalar la bala (muy pequeña)
                bullet.sprite.setScale(0.08f, 0.08f);
                
                // Centrar origen
                sf::FloatRect bulletBounds = bullet.sprite.getLocalBounds();
                bullet.sprite.setOrigin(bulletBounds.width / 2.f, bulletBounds.height / 2.f);
                
                // Calcular destino (misma altura, hacia el jugador en línea recta)
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
    
    // Dibujar Coleccionables (no recolectados)
    for (auto& collectible : mCollectibles) {
        if (!collectible.collected) {
            window.draw(collectible.sprite);
        }
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

int Level::getCollectedCount() const {
    return mCollectedCount;
}

bool Level::isBossDefeated() const {
    // Buscar si el boss (Doakes) está vivo
    for (const auto& enemy : mEnemies) {
        if (enemy.getType() == EnemyType::BOSS) {
            // Encontramos al boss, verificar si está muerto o muriendo
            bool defeated = enemy.isDead() || enemy.isDying();
            std::cout << "Boss encontrado - isDead: " << enemy.isDead() << ", isDying: " << enemy.isDying() << ", Derrotado: " << defeated << std::endl;
            return defeated;
        }
    }
    // Si no encontramos al boss en la lista, significa que fue derrotado
    std::cout << "Boss no encontrado en lista - Asumiendo derrotado" << std::endl;
    return true;
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
