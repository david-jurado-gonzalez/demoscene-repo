# Referencia rápida de APIs

Resumen de cabeceras y funciones más usadas para demos y juegos en este repositorio.

## effect.h

- **EffectT** — Load, UnLoad, Init, Kill, Render, VBlank; magic, name.
- **EFFECT(NAME, L, U, I, K, R, V)** — Define `NAME##Effect`.
- **frameCount**, **lastFrameCount**, **exitLoop**.
- **ReadFrameCount()**, **TimeWarp(frame)** (si no DEMO).
- **EffectLoad**, **EffectInit**, **EffectKill**, **EffectUnLoad**, **EffectRun**, **EffectIsRunning**.
- **TaskWaitVBlank()** (con MULTITASK) / **WaitVBlank**.
- **PROFILE**, **ProfilerStart**, **ProfilerStop** (con PROFILER).

## common.h

- **min**, **max**, **abs**, **swap**, **roundup**, **rounddown**, **nitems**.
- **getword(tab, idx)**, **getlong(tab, idx)**, **getptr(tab, idx)** — Lectura indexada (tabla de 16/32 bits).
- **div16**, **mod16**, **udiv16**, **umod16**, **mul16**, **divmod16** — Aritmética 68000.
- **bclr**, **bset**, **bchg** — Bits en byte.
- **rorw**, **rorl**, **rolw**, **roll** — Rotaciones.
- **stbi**, **stwi**, **stli** (postinc), **stbd**, **stwd**, **stld** (predec) — Escritura en puntero con incremento.
- **swapr(a, b)** — Intercambiar registros (exg).
- **GetSP()** — Puntero de pila.

## fx.h (fixed-point y trigonometría)

- **SIN(a)**, **COS(a)** — Tabla sintab; argumento en unidades 0..0xFFF (SIN_MASK), SIN_HALF_PI, SIN_PI.
- **normfx(a)** — (a << 4) >> 16; normalizar producto fixed-point.
- **shift12(a)** — Pasar de fx12 a entero.
- **fx4i(i)**, **fx12i(i)** — Entero a fixed 4.12 o 12.4.
- **fx4f(f)**, **fx12f(f)** — Float a fixed.
- **sintab[]** — Tabla externa de seno.
- **isqrt(x)** — Raíz cuadrada entera.

## 2d.h / lib2d

- **Matrix2D**, **Point2D**, **Line2D**, **Box2D**; **ClipWin** (global).
- **LoadIdentity2D**, **Translate2D**, **Scale2D**, **Rotate2D**, **Transform2D**.
- **PointsInsideBox**, **ClipLine2D**, **ClipPolygon2D** (clipFlags: PF_LEFT, PF_RIGHT, PF_TOP, PF_BOTTOM).
- **ShapeT** — points, polygons, origPoint, viewPoint, polygon, etc.

## 3d.h / lib3d

- **Point3D**, **UVCoordT**, **Node3D**, **EdgeT**, **Matrix3D**, **FaceT**, **FaceIndexT**, **Object3D**, **Mesh3D**.
- **LoadIdentity3D**, **Translate3D**, **Scale3D**, **LoadRotate3D**, **LoadReverseRotate3D**, **Compose3D**, **Transform3D**.
- **NewObject3D**, **DeleteObject3D**.
- **UpdateObjectTransformation**, **UpdateFaceVisibility**, **UpdateVertexVisibility**.
- **SortFaces**, **SortFacesMinZ**, **AllFacesDoubleSided**.
- Macros: **NODE3D**, **POINT**, **VERTEX**, **UVCOORD**, **EDGE**, **FACE** (requieren _objdat).

## gfx.h / bitmap.h

- **Point2D**, **Line2D**, **Box2D**, **Size2D**, **Area2D**.
- **ClipArea**, **InsideArea**.
- **BitmapT** — width, height, depth, bytesPerRow, bplSize, flags, planes[].
- **BitmapSize**, **BitmapSetPointers**, **InitSharedBitmap**.
- **NewBitmap(w, h, depth, flags)**, **DeleteBitmap**, **BitmapMakeDisplayable**.

## blitter.h / libblit

- **WaitBlitter()**, **BlitterBusy()**, **BlitterStop()**.
- **BlitterCopy**, **BlitterCopyArea**, **BlitterCopyFast**, **BlitterCopyMasked**.
- **BlitterOr**, **BlitterSetArea**, **BlitterClear**, **BlitterFillArea**, **BlitterLine**, **BlitterLineSetup**, **BlitterLineSetupFull**.
- **BitmapCopy**, **BitmapCopyFast**, **BitmapCopyMasked**, **BitmapCopyArea**.
- **BitmapSetArea**, **BitmapClear**, **BitmapMakeMask**.
- **BitmapAddSaturated**, **BitmapDecSaturated**, **BitmapIncSaturated**.
- Minterms: **A_AND_B**, **A_OR_B**, **A_TO_D**, **HALF_ADDER**, **LINE_OR**, **LINE_EOR**, etc.
- **FirstWordMask**, **LastWordMask**, **LineMode**.

## copper.h / libgfx

- **CopInsT**, **CopInsPairT**, **CopListT**.
- **NewCopList(len)**, **DeleteCopList**, **CopListReset**, **CopListFinish**, **CopListActivate**, **CopListRun**.
- **CopperStop()**, **CopInsPtr(list)**.
- **CopWait(cp, vp, hp)**, **CopMove16**, **CopMove32**.
- **CopSetupBitplanes**, **CopSetupDisplayWindow**, **CopSetupMode**, **CopLoadColor**, **CopLoadColorArray**, **CopSetupSprites**.

## playfield.h / libgfx

- **SetupPlayfield(mode, depth, x, y, w, h)** — MODE_LORES, MODE_HIRES.
- **SetupMode**, **SetupDisplayWindow**, **SetupBitplaneFetch**, etc.

## color.h / palette.h

- **LoadColors(palette, start)** — Cargar en COLOR00…
- **LoadColorArray**, **CopLoadColor**, **CopLoadColorArray**.

## line.h / libgfx

- **CpuLineSetup(bitmap, plane)**.
- **CpuLine(x1, y1, x2, y2)**.

## sprite.h / libgfx

- **MakeSprite**, **EndSprite**, **NullSprData**, **SpriteUpdatePos**, **SpriteHeight**, **ResetSprites**.

## pixmap.h

- **PixmapT** — type (PM_CMAP4, PM_CMAP8, PM_RGB12), width, height, pixels.
- **NewPixmap**, **DeletePixmap**, **InitSharedPixmap**.
- **PixmapScramble_4_1**, **PixmapScramble_4_2**.

## system/ (interrupt, memory, task)

- **AddIntServer**, **RemIntServer** — INTB_VERTB, etc.
- **AllocMem**, **FreeMem** (o wrappers del proyecto).
- **TaskInit**, **TaskRun** — Tareas con stack propio.

## custom.h / custom_regdef.h

- **custom** — Puntero a struct Custom (registros del hardware).
- **EnableDMA**, **DisableDMA** — DMAF_RASTER, DMAF_BLITTER, DMAF_BLITHOG, DMAF_AUDIO, etc.

Para definiciones exactas de registros y constantes, consultar los `.h` en `include/` y el *Amiga Hardware Reference Manual*.
