#pragma once
#include <SFML/Graphics.hpp>

/**
 * Configuración Centralizada del Juego
 * ===================================
 * Este archivo contiene todas las constantes del proyecto
 * para facilitar mantenimiento y cambios globales.
 */

namespace Config {
    // === VENTANA ===
    constexpr int WINDOW_WIDTH  = 2560;
    constexpr int WINDOW_HEIGHT = 1440;
    constexpr int WORLD_WIDTH   = 4000; // Ancho total del nivel
    constexpr int FPS_LIMIT = 165;
    constexpr const char* WINDOW_TITLE = "Dexter: Dark Passenger";

    // === JUGADOR ===
    constexpr float PLAYER_SPEED = 400.f;        // Píxeles por segundo
    constexpr float ANIMATION_SPEED = 0.15f;     // Segundos por frame
    constexpr float PLAYER_INITIAL_X = 100.f;    // Centro horizontal (Lado izquierdo)
    constexpr float PLAYER_INITIAL_Y = 500.f;    // Posición vertical (Aire para caer)
    constexpr float PLAYER_TARGET_HEIGHT = 150.f; // Altura visual objetivo
    constexpr float CROUCH_SPEED = 200.f;        // Velocidad al agacharse (píxeles por segundo)

    // === ASSETS === 
    constexpr const char* ASSET_PATH = "./assets/";
    constexpr const char* IMAGE_PATH = "./assets/images/";
    constexpr const char* FONT_PATH = "./assets/fonts/";

    // === COLORES ===
    inline const sf::Color BACKGROUND_COLOR = sf::Color::Black;

    // === DEBUG ===
    constexpr bool DEBUG_MODE = true;             // Mostrar información de debug
    constexpr const char* DEBUG_FONT = "./assets/fonts/Minecraft.ttf";
}

