## Sistema de Movimiento Mejorado - Dexter

### Estados del Jugador

Dexter tiene 3 estados principales de movimiento, más un **Modo Ataque** con Jeringa:

#### 1. **IDLE (Quieto)**
- Estado por defecto
- Dexter permanece en su posición normal
- Animación: `dexter_default.png` (o `dexter_default_jeringa.png` en modo ataque)
- Velocidad: 0 píxeles/segundo

#### 2. **RUNNING (Corriendo)**
- Se activa al presionar A o D
- Dexter se mueve en la dirección indicada
- Animación: Alterna entre `dexter_paso1.png` y `dexter_paso2.png` cada 0.15 segundos
  - En modo ataque: Alterna entre `dexter_paso1_jeringa.png` y `dexter_paso2_jeringa.png`
- Velocidad: 400 píxeles/segundo (configurable en `Config.hpp`)
- El sprite se invierte automáticamente según la dirección (izquierda/derecha)
- Los cambios de dirección rápidos (A → D o D → A) se reflejan instantáneamente

#### 3. **CROUCHING (Agachado)**
- Se activa al presionar S
- Dexter adopta posición agachada
- Animación: `dexter_agachado.png` (o `dexter_agachado_jeringa.png` en modo ataque)
- Velocidad de movimiento: 200 píxeles/segundo (más lenta que correr)
- Puede moverse mientras está agachado pero a menor velocidad
- Los pies se mantienen alineados con los estados IDLE y RUNNING

#### 4. **MODO ATAQUE (Con Jeringa)** ⚔️
- Se activa al presionar SPACE (primera vez)
- Dexter saca una jeringa, simulando un ataque/modo defensivo
- Mantiene los mismos estados (IDLE, RUNNING, CROUCHING) pero con animaciones de jeringa
- Todas las acciones funcionan igual, solo cambian las texturas:
  - **IDLE con jeringa:** `dexter_default_jeringa.png`
  - **RUNNING con jeringa:** Alterna `dexter_paso1_jeringa.png` y `dexter_paso2_jeringa.png`
  - **CROUCHING con jeringa:** `dexter_agachado_jeringa.png`
- Presionar SPACE nuevamente desactiva el modo ataque y vuelve a las animaciones normales

---

### Controles

| Tecla | Acción |
|-------|--------|
| **A** | Moverse a la izquierda |
| **D** | Moverse a la derecha |
| **S** | Agacharse |
| **SPACE** | Activar/Desactivar Modo Ataque (Jeringa) |
| **ESC** | Salir del juego |

---

### Información de Debug (Pantalla)

En la esquina superior izquierda aparecen las siguientes métricas:

- **FPS** (Verde): Frames por segundo actual
- **Estado** (Cyan): Estado actual del jugador (QUIETO / CORRIENDO / AGACHADO)
- *Cuando el Modo Ataque está activo, verás las imágenes de jeringa en lugar de las normales*

---

### Características Principales

✅ **Alineación de sprites:** Todos los estados mantienen los pies alineados en la misma línea Y  
✅ **Cambios de dirección rápidos:** La animación se actualiza instantáneamente al cambiar entre A y D  
✅ **Modo Ataque (Jeringa):** Toggle con SPACE sin cambiar la mecánica de movimiento  
✅ **Animación fluida:** Transiciones suaves entre estados con animación de 0.15 segundos por frame  
✅ **Límites de pantalla:** El jugador no puede salir del área visible (2560x1440)  
✅ **Rendimiento optimizado:** 60 FPS sin lag, solo recálculos necesarios

### Integrando Imágenes de Jeringa

Para activar el Modo Ataque completamente, asegúrate de que estos archivos existan en `assets/images/`:
- `dexter_default_jeringa.png` - Estado quieto/idle en modo ataque
- `dexter_paso1_jeringa.png` - Frame 1 de movimiento en modo ataque
- `dexter_paso2_jeringa.png` - Frame 2 de movimiento en modo ataque
- `dexter_agachado_jeringa.png` - Agacharse en modo ataque (opcional)

Si estos archivos no existen, el juego automáticamente usa las imágenes normales como fallback.

---
### Archivos Modificados

#### `Player.hpp`
- Nuevo enum: `PlayerState` (IDLE, RUNNING, CROUCHING)
- Nuevas texturas: `mTextureCrouch`, `mTextureCrouch1`, `mTextureCrouch2`
- Nuevo miembro: `mCurrentState` para rastrear el estado actual
- Nuevo miembro: `mFacingLeft` para rastrear la dirección
- Nuevos métodos: `getState()`, `setState()`

#### `Player.cpp`
- Lógica de estados en `update()`
- Sistema de cambio de estado automático
- Detecta SPACE y CTRL para agacharse
- Velocidad reducida al agacharse
- Animaciones según el estado

#### `Game.hpp`
- Nuevo miembro: `mStateText` para mostrar el estado

#### `Game.cpp`
- Actualización de `mStateText` en `updateDebugInfo()`
- Renderización del estado en pantalla

#### `Config.hpp`
- Nueva constante: `CROUCH_SPEED = 200.f`

---

### Técnicas Implementadas

✅ **Máquina de Estados**: Controla transiciones entre IDLE → RUNNING → CROUCHING
✅ **Animación Fluida**: Transiciones suaves entre estados
✅ **Refactorización**: Código limpio y mantenible
✅ **Escalado Dinámico**: Ajusta el tamaño de sprites automáticamente
✅ **Limitador de Pantalla**: Evita que Dexter salga del área visible
✅ **Debug Visual**: Información en pantalla en tiempo real

---

### Posibles Mejoras Futuras

- [ ] Animación de agacharse/levantarse (transición suave)
- [ ] Múltiples frames de agacharse si existen imágenes
- [ ] Salto cuando está corriendo
- [ ] Sistema de armas/ataques
- [ ] Velocidades configurables por teclado (Up/Down)
- [ ] Efectos de sonido por estado
- [ ] Partículas/polvo al agacharse
