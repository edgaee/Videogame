#include "Level.hpp"
#include "Config.hpp"
#include <iostream>

Level::Level() {
    // Cargar texturas individuales
    if (!mTexFloor.loadFromFile(std::string(Config::IMAGE_PATH) + "tile_floor.png")) {
        std::cerr << "Error: No se pudo cargar tile_floor.png" << std::endl;
    }
    if (!mTexVent.loadFromFile(std::string(Config::IMAGE_PATH) + "tile_vent1.png")) {
        std::cerr << "Error: No se pudo cargar tile_vent1.png" << std::endl;
    }
}

void Level::loadLevel1() {
    mPlatforms.clear();
    mHidingSpots.clear();

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

    // 2. Plataformas Elevadas (Aires Acondicionados)
    // Plataforma 1: Izquierda baja
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(200.f, 900.f));
    
    // Plataforma 2: Centro media
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(500.f, 750.f));
    
    // Plataforma 3: Derecha alta
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(800.f, 600.f));
    
    // Plataforma 4: Más a la derecha
    mPlatforms.emplace_back(&mTexVent, sf::Vector2f(1100.f, 750.f));


    // --- ESCONDITES ---
    // 1. Puerta oscura en el pasillo inicial
    mHidingSpots.emplace_back(sf::Vector2f(80.f, 150.f), sf::Vector2f(400.f, 850.f));

    // 2. Zona oscura bajo el conducto derecho
    mHidingSpots.emplace_back(sf::Vector2f(100.f, 150.f), sf::Vector2f(1300.f, 850.f));
}

void Level::update(float dt) {
    // Aquí podríamos animar tiles, mover plataformas móviles, etc.
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
}

const std::vector<HidingSpot>& Level::getHidingSpots() const {
    return mHidingSpots;
}

void Level::checkCollisions(Player& player, float dt) {
    // Lógica de colisión trasladada desde Player.cpp
    // Necesitamos que Player exponga métodos para mover y obtener velocidad
    
    sf::Vector2f velocity = player.getVelocity();
    
    // 1. Movimiento en X
    float moveX = velocity.x * dt;
    player.move(moveX, 0.f);
    
    // Colisión X
    sf::FloatRect playerBounds = player.getBounds();
    sf::FloatRect intersection;
    
    for (const auto& platform : mPlatforms) {
        sf::FloatRect platBounds = platform.getBounds();
        if (playerBounds.intersects(platBounds, intersection)) {
            if (intersection.height < 10.f) continue; // Ignorar roces suelo/techo

            if (moveX > 0) { // Derecha
                player.setPosition(platBounds.left - playerBounds.width / 2.f, player.getPosition().y);
            } else if (moveX < 0) { // Izquierda
                player.setPosition(platBounds.left + platBounds.width + playerBounds.width / 2.f, player.getPosition().y);
            }
            player.setVelocity(0.f, velocity.y); // Stop X
        }
    }

    // 2. Movimiento en Y
    // Gravedad ya aplicada en Player::update, aquí solo integramos posición
    float moveY = velocity.y * dt;
    player.move(0.f, moveY);

    // Colisión Y
    playerBounds = player.getBounds();
    bool onGround = false;

    for (const auto& platform : mPlatforms) {
        sf::FloatRect platBounds = platform.getBounds();
        if (playerBounds.intersects(platBounds, intersection)) {
            if (intersection.width < 10.f) continue; // Ignorar roces paredes

            if (moveY > 0) { // Cayendo
                // Solo aterrizar si la colisión es superficial (no estamos dentro del bloque)
                // Si la intersección es muy alta, probablemente es una colisión lateral que pasó el filtro de width
                if (intersection.height <= moveY + 15.f) { 
                    player.setPosition(player.getPosition().x, platBounds.top);
                    player.setVelocity(player.getVelocity().x, 0.f); // Stop Y
                    player.setGrounded(true);
                    onGround = true;
                }
            } else if (moveY < 0) { // Saltando (Techo)
                player.setPosition(player.getPosition().x, platBounds.top + platBounds.height + playerBounds.height);
                player.setVelocity(player.getVelocity().x, 0.f); // Stop Y
            }
        }
    }
    
    // Fallback suelo base (si se cae del mapa)
    // El suelo está en Y=1000, así que si baja de 1200 es que cayó al vacío
    if (player.getPosition().y > 1200.f) { 
         // Reset al spawn
         player.setPosition(Config::PLAYER_INITIAL_X, Config::PLAYER_INITIAL_Y);
         player.setVelocity(0.f, 0.f);
         player.setGrounded(true);
    }
}
