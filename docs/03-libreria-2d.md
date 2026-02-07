# Librería 2D (lib2d)

La **lib2d** proporciona matemáticas 2D y recorte de primitivas optimizadas para 68000 y uso en demos/juegos en Amiga. Todo trabaja en **fixed-point** (ver `fx.h` y `common.h`).

## Tipos (2d.h, gfx.h)

- **Point2D** — `short x, y`
- **Line2D** — `short x1, y1, x2, y2`
- **Box2D** — `short minX, minY, maxX, maxY`
- **Matrix2D** — Matriz 2×2 + traslación: `m00, m01, x, m10, m11, y`
- **ClipWin** — Variable global `Box2D` que define la ventana de recorte para líneas y polígonos.
- **ShapeT** — Forma 2D: arrays de puntos (orig/view), listas de índices por polígono y flags.

## Matrices 2D

- **LoadIdentity2D(M)** — Pone M en identidad.
- **Translate2D(M, x, y)** — Añade traslación (x, y) a M.
- **Scale2D(M, sx, sy)** — Añade escala a M.
- **Rotate2D(M, a)** — Añade rotación de ángulo `a` (en unidades de tabla sin/cos, típicamente 0..0xFFF para 360°).
- **Transform2D(M, out, in, n)** — Aplica M a `n` puntos; `out[i] = M * in[i]`. Usa `normfx()` para las multiplicaciones (fixed-point).

Las transformaciones se componen en el orden en que se llaman (por ejemplo: LoadIdentity → Rotate → Scale → Translate).

## Recorte de líneas (Liang-Barsky)

- **ClipLine2D(line)** — Recorta el segmento `line` contra la ventana global `ClipWin`. Devuelve `true` si queda algún segmento visible y actualiza `line` con los extremos recortados. Implementación en `ClipLine2D.c` usando parámetros t de la recta y las caras del box.

**Uso**: establecer `ClipWin` (minX, minY, maxX, maxY) y luego llamar a `ClipLine2D` antes de dibujar la línea (p. ej. con blitter o CPU).

## Recorte de polígonos (Sutherland-Hodgman)

- **ClipPolygon2D(in, outp, n, clipFlags)** — Recorta el polígono de `n` vértices en `in` contra las aristas de la ventana. `clipFlags` es combinación de:
  - `PF_LEFT`, `PF_RIGHT`, `PF_TOP`, `PF_BOTTOM` (1, 2, 4, 8)
  Se recorta en ese orden (izquierda, arriba, derecha, abajo). `*outp` apunta al buffer de salida (puede alternar con `in`). Devuelve el número de vértices del polígono recortado.

Implementación en `ClipPolygon2D.c`: por cada arista del clip se recorre el polígono y se generan nuevos vértices en las intersecciones.

## Puntos dentro de un box

- **PointsInsideBox(in, flags, n)** — Para cada punto en `in` escribe en `flags` un byte indicando si está dentro de `ClipWin` (usado para visibilidad o recorte rápido).

## Ventana de recorte

Debe asignarse antes de usar recorte:

```c
ClipWin.minX = 0;
ClipWin.minY = 0;
ClipWin.maxX = 320;
ClipWin.maxY = 256;
```

## Archivos de lib2d

| Archivo | Función |
|---------|---------|
| LoadIdentity2D.c | Identidad 2D |
| Translate2D.c, Scale2D.c, Rotate2D.c | Composición de transformaciones |
| Transform2D.c | Aplicar matriz a N puntos (loop con normfx) |
| ClipLine2D.c | Liang-Barsky para Line2D |
| ClipPolygon2D.c | Sutherland-Hodgman para polígono 2D |
| ClipWin.c | Definición/uso de ClipWin |
| PointsInsideBox.c | Test de puntos dentro de ClipWin |

## Optimizaciones para Amiga

- Uso de **div16** (instrucción `divs` en 68000) para divisiones en recorte.
- Matrices y puntos en **short**; producto con **normfx** (desplazamiento 4 bits) para mantener precisión.
- Tabla de seno/coseno compartida con el resto del proyecto (`sintab` en fx.h) para rotaciones.

Estas rutinas son la base para efectos 2D (shapes, anim-polygons, plotter, etc.) y para HUD o menús en juegos.
