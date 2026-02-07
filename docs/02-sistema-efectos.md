# Sistema de efectos

## Estructura `EffectT` (effect.h)

Cada efecto se describe con una estructura de tipo `EffectT`:

```c
typedef struct Effect {
  u_int magic;           /* EFFECT_MAGIC 'GTN!' para el loader */
  const char *name;      /* Nombre del efecto (símbolo C) */
  union { EffectFuncT Func; intptr_t Status; } Load;   /* Precalcular en background */
  EffectFuncT UnLoad;   /* Liberar recursos de Load */
  union { EffectFuncT Func; intptr_t Status; } Init;  /* Inicializar para ejecución */
  EffectFuncT Kill;     /* Liberar recursos de Init */
  EffectFuncT Render;   /* Dibujar un frame */
  EffectFuncT VBlank;  /* Llamado en cada VBlank */
} EffectT;
```

- **Load** — Función (o estado) que se ejecuta en tarea de fondo mientras otro efecto está en pantalla. Idóneo para generar tablas, paletas o datos pesados.
- **UnLoad** — Libera lo asignado en Load.
- **Init** — Reserva memoria (bitmaps, copper), configura playfield, DMA, interrupciones. Se llama al pasar a este efecto.
- **Kill** — Desactiva DMA/copper y libera memoria de Init.
- **Render** — Se llama cada frame; aquí va el dibujado (blitter, CPU, copper).
- **VBlank** — Opcional; se invoca desde la ISR de vertical blank (actualizar punteros, paletas, música).

Cualquier callback puede ser `NULL` si el efecto no lo usa.

## Macro `EFFECT(NAME, L, U, I, K, R, V)`

Define la variable `NAME##Effect` de tipo `EffectT` con:

- `NAME` — Nombre del efecto (el símbolo será `WireframeEffect`, `PlasmaEffect`, etc.).
- `L` — Load (función o NULL).
- `U` — UnLoad (función o NULL).
- `I` — Init.
- `K` — Kill.
- `R` — Render.
- `V` — VBlank (función o NULL).

Ejemplo:

```c
EFFECT(Plasma, Load, NULL, Init, Kill, Render, NULL);
```

## Variables globales del sistema de efectos

- **frameCount** — Número de frames (50 Hz) desde que se llamó a `Render` por primera vez en el efecto actual.
- **lastFrameCount** — Frame en que se llamó al último Render; sirve para medir tiempo entre frames.
- **exitLoop** — Si se pone a `true`, el bucle de Render termina (p. ej. botón izquierdo del ratón).

Funciones auxiliares: `EffectLoad`, `EffectInit`, `EffectKill`, `EffectUnLoad`, `EffectRun`, `EffectIsRunning`.

## Perfilado (PROFILER)

Con `PROFILER` definido se pueden usar:

- `PROFILE(Nombre)` — Declara un perfil para un bloque.
- `ProfilerStart(Nombre)` / `ProfilerStop(Nombre)` — Marcan inicio y fin del bloque a medir.

Los datos (líneas, total, min, max, count) se guardan en una estructura `ProfileT` asociada al nombre.

## Multitarea

Con `MULTITASK`, `TaskWaitVBlank()` pone la tarea actual a esperar el siguiente vertical blank y deja CPU a otras tareas. Sin multitarea, se define como `WaitVBlank`.

## Uso típico en un efecto

1. **Init**: `NewBitmap`, `NewCopList`, `CopSetupBitplanes`, `SetupPlayfield`, `LoadColors`, `EnableDMA`, `CopListActivate`.
2. **Render**: actualizar estado (ángulos, posiciones), limpiar o copiar buffers, dibujar con blitter/CPU/copper, opcionalmente `TaskWaitVBlank()` al final.
3. **Kill**: `BlitterStop`, `CopperStop`, `DeleteBitmap`, `DeleteCopList`, `DisableDMA`, etc.
4. **VBlank** (si hace falta): actualizar `bplpt` en copper, cambiar paleta, avanzar reproductor de música.

Ver [06-catalogo-efectos.md](06-catalogo-efectos.md) para ejemplos concretos por efecto.
