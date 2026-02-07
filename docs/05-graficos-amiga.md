# Gráficos en Amiga: bitmap, copper, blitter, sprites

Resumen práctico deducido del código y del Amiga Hardware Reference Manual para programar demos y juegos en A500.

## Bitmap (bitmap.h, libgfx)

- **BitmapT**: width, height, depth, bytesPerRow, bplSize, flags, planes[].  
- **Flags**: BM_CLEAR (poner a 0 al crear), BM_CPUONLY (no usar blitter en algunas utilidades), BM_INTERLEAVED (planos entrelazados), BM_MINIMAL, BM_HAM, BM_EHB, BM_STATIC.
- **NewBitmap(w, h, depth, flags)** — Asigna bitplanes en chip RAM.
- **DeleteBitmap(bm)** — Libera.
- **BitmapSetPointers**, **BitmapMakeDisplayable** — Ajustan punteros o formato para el hardware.
- **BitmapSize(bm)** — Tamaño total en bytes.
- **InitSharedBitmap** — Inicializa un bitmap que comparte memoria con otro (doble buffer).

El ancho en palabras (bytesPerRow/2) y bplSize deben ser correctos para el copper y el blitter (múltiplos según modo).

## Playfield y modos (playfield.h, libgfx)

- **SetupPlayfield(mode, depth, x, y, w, h)** — Configura resolución (MODE_LORES/MODE_HIRES), número de planos, posición y tamaño de la ventana de visualización.
- **SetupMode**, **SetupDisplayWindow** — Detalle de BPLCON0, DIWSTRT/DIWSTOP, DDFSTRT/DDFSTOP.
- En lores: 320 pixels lógicos por línea; en hires: 640. Altura típica 256 (PAL) o 200 (NTSC).

## Copper (copper.h, libgfx)

- **CopListT** — Lista de instrucciones: WAIT (vp, hp, máscaras), MOVE (registro, valor). Cada instrucción son 2 palabras (CopInsT); los MOVE de 32 bits usan CopInsPairT.
- **NewCopList(length)** — Reserva lista.
- **CopWait(cp, vp, hp)** — Inserta WAIT.
- **CopMove16(cp, reg, value)** / **CopMove32(cp, regpair, value)** — Inserta MOVE.
- **CopSetupBitplanes(cp, bitmap, depth)** — Añade MOVE para bplpt y modulo; devuelve puntero a los MOVE de bplpt para actualizarlos cada frame (doble buffer).
- **CopSetupDisplayWindow**, **CopSetupMode**, **CopLoadColor**, **CopLoadColorArray** — Ventana, modo y paleta.
- **CopListFinish(cp)** — Termina con WAIT que detiene el copper.
- **CopListActivate(cp)** — Pone cop1lc y espera VBlank.
- **CopperStop()** — Detiene el copper (y con ello refresco de bitplanes/sprites).

Restricciones importantes (según HRM y comentarios en plasma): la posición vertical de inicio debe ser divisible por 4 si se usa copper por línea (por el comportamiento del contador VP); cuidado con el paso de línea 255 a 256.

## Blitter (blitter.h, libblit)

- **WaitBlitter()** — Espera a que el blitter termine (dmaconr & DMAF_BLTDONE).
- **BlitterStop()** — Parada ordenada del blitter.
- **BlitterCopy** / **BlitterCopyArea** / **BlitterCopyFast** — Copia de bitmap (o área) a otra posición. Setup + Start; el blitter trabaja por planos.
- **BlitterCopyMasked** — Copia con máscara (para sprites o gráficos con transparencia).
- **BlitterSetArea** / **BlitterClear** — Rellenar área con patrón o cero.
- **BlitterFillArea** — Relleno con patrón (modo fill del blitter).
- **BlitterOr** — OR de un plano sobre otro.
- **BlitterLine** — Dibuja línea (modo línea del blitter); BlitterLineSetup/BlitterLineSetupFull para configurar modo (OR/EOR, patrón).
- **BitmapCopy**, **BitmapCopyFast**, **BitmapCopyMasked**, **BitmapCopyArea** — Wrappers que iteran sobre planos y llaman al blitter.
- **BitmapSetArea**, **BitmapClear** — Poner color o borrar (por planos).
- **BitmapMakeMask** — Genera máscara a partir de un bitmap (para CopyMasked).
- Minterms (bltcon0): A, B, C, D; combinaciones para operaciones lógicas (AND, OR, XOR, etc.). Ver blitter.h (A_AND_B, A_OR_B, etc.).

El blitter usa chip RAM; hay que esperar a que termine antes de cambiar datos que use o antes de desactivar DMA.

## Sprites (sprite.h, libgfx)

- Estructuras para datos de sprite (altura, posición VSTART/HSTART/VSTOP, datos de imagen).
- **MakeSprite**, **EndSprite**, **NullSprData** — Construcción de datos de sprite.
- **CopSetupSprites** — Añade a la copper los MOVE para SPRxPT y control.
- **SpriteUpdatePos**, **SpriteHeight** — Actualizar posición/altura en la lista.
- **ResetSprites** — Poner sprites a null o posición segura.

Los sprites tienen prioridad fija (0 delante de 1, etc.) y comparten color registers 17–31 por parejas.

## Chunky to planar (c2p) y Pixmap

- **PixmapT** — Formato chunky (píxeles consecutivos); tipo PM_CMAP4, PM_CMAP8, PM_RGB12.
- **PixmapScramble_4_1**, **PixmapScramble_4_2** — Reordenación de palabras para convertir chunky a planar (blitter o CPU).
- En efectos como fire-rgb y plasma se usa conversión chunky→planar (a veces por blitter en varias pasadas) para dibujar en un buffer chunky y luego mostrar en bitplanes.

## Colores y paleta (color.h, palette.h, libgfx)

- **LoadColors**, **LoadColorArray** — Cargar paleta en registros COLOR00…
- **CopLoadColor**, **CopLoadColorArray** — Inserción en copper (cambios por línea o por frame).
- Formato típico: 12 bits (4R, 4G, 4B) por color en OCS/ECS.

## Líneas por CPU (line.h, libgfx)

- **CpuLineSetup(bitmap, plane)** — Prepara dibujo de líneas en un plano.
- **CpuLine(x1, y1, x2, y2)** — Dibuja segmento (Bresenham u otro); usado en loader, wireframe por CPU, etc.

## Referencia rápida de flujo

1. Crear bitmap(s) con NewBitmap.
2. Crear copper list con NewCopList; CopSetupBitplanes (y opcionalmente sprites, colores, ventana).
3. SetupPlayfield para modo y ventana.
4. EnableDMA(DMAF_RASTER | DMAF_BLITTER | …).
5. CopListActivate(cp).
6. Cada frame: actualizar punteros bplpt en copper si hay doble buffer; dibujar en bitmap (Blitter*, CpuLine, etc.); WaitBlitter si hace falta.
7. Al salir: BlitterStop, CopperStop, DeleteBitmap, DeleteCopList, DisableDMA.

Para más detalles de cada API ver [07-referencia-apis.md](07-referencia-apis.md) y los includes del repositorio.
