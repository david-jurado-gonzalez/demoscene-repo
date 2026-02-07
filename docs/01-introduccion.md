# Introducción al desarrollo de demos y juegos en Amiga 500

## Objetivo del repositorio

Este proyecto es un **trackmo / demo** para Amiga 500 (y compatibles OCS/ECS): una secuencia de efectos visuales y de audio que se ejecutan uno tras otro. El código sirve tanto para producir la demo final como para **aprender y reutilizar** técnicas de programación a bajo nivel en Amiga.

## Arquitectura general

- **effects/** — Cada subcarpeta es un efecto (plasma, wireframe, fire-rgb, etc.). Cada efecto se registra con la macro `EFFECT()` y expone: Load, UnLoad, Init, Kill, Render, VBlank.
- **lib/** — Librerías compartidas:
  - **lib2d** — Matemáticas y recorte 2D (matrices, líneas, polígonos).
  - **lib3d** — Objetos 3D, transformaciones, visibilidad, ordenación de caras.
  - **libgfx** — Bitmap, copper lists, sprites, modos de pantalla, colores.
  - **libblit** — Operaciones con el blitter (copiar, rellenar, líneas, máscaras).
  - **libc**, **libmisc**, **libpt**, **libp61**, **libahx**, **libctr** — C runtime, utilidades, reproductores de música.
- **include/** — Cabeceras (effect.h, 2d.h, 3d.h, gfx.h, blitter.h, copper.h, etc.).
- **system/** — Inicialización, interrupciones, VBlank, tareas, drivers (teclado, ratón, disco).
- **demo/** — Orquestación de la demo (loader, lista de efectos, sincronización).

## Flujo de un efecto

1. **Load** (opcional) — Se ejecuta en background mientras corre otro efecto. Precalcula datos (tablas, paletas, geometría) para el efecto que se lanzará después.
2. **Init** — Asignación de memoria (bitmaps, copper lists), configuración del playfield, DMA, interrupciones. Se llama justo antes de mostrar el efecto.
3. **Render** — Se invoca cada frame; aquí va la lógica de dibujado (blitter, CPU, copper).
4. **VBlank** (opcional) — Se llama desde la interrupción de vertical blank; útil para actualizar punteros de bitplanes, paletas o música sin bloquear el Render.
5. **Kill** — Libera recursos creados en Init (bitmaps, copper, DMA).
6. **UnLoad** (opcional) — Libera recursos creados en Load.

El bucle principal hace básicamente: `EffectLoad` → `EffectInit` → bucle `Render` hasta condición de salida → `EffectKill` → `EffectUnLoad`.

## Hardware relevante (Amiga 500)

- **68000** — CPU; el código usa aritmética en fixed-point (fx4, fx12) y tablas (sin/cos, invsqrt) para evitar floats.
- **Agnus** — DMA: bitplanes, sprites, blitter, copper. La memoria de chips debe contener bitplanes, listas copper y datos que use el blitter.
- **Denise** — Video: modos lores/hires, número de bitplanes, EHB, HAM.
- **Paula** — Audio y disquetera.
- **Copper** — Coprocesador que ejecuta listas de instrucciones (WAIT, MOVE) sincronizadas con el haz; se usa para cambiar colores por línea, scroll, sprites, etc.
- **Blitter** — Copia, relleno y dibujo de líneas en memoria; muchas veces más rápido que la CPU para operaciones sobre bitplanes.

## Convenciones de código

- **Fixed-point**: `fx4i`, `fx12i` (entero → fixed), `normfx` (resultado de multiplicaciones). Ver `include/fx.h` y `common.h`.
- **Coordenadas**: típicamente origen arriba-izquierda; en 3D, Z positivo hacia la cámara según el contexto.
- **Chip RAM**: bitmaps, copper lists y buffers usados por blitter/copper deben estar en chip memory (el linker y el allocator del sistema reservan esta región).

## Siguientes pasos

- Leer [02-sistema-efectos.md](02-sistema-efectos.md) para el detalle del sistema de efectos.
- Revisar [03-libreria-2d.md](03-libreria-2d.md) y [04-libreria-3d.md](04-libreria-3d.md) para aprovechar las librerías 2D/3D optimizadas para Amiga.
