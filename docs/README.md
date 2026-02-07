# Manual de programación de demos y juegos para Amiga 500

Este directorio contiene la documentación del repositorio demoscene-repo: guías de uso de las librerías, descripción de efectos y referencias para desarrollar demos y juegos en Amiga 500 (OCS/ECS).

## Índice de documentos

| Documento | Contenido |
|-----------|-----------|
| [01-introduccion.md](01-introduccion.md) | Introducción al desarrollo en A500, arquitectura del proyecto y flujo de un efecto |
| [02-sistema-efectos.md](02-sistema-efectos.md) | Sistema de efectos: Load, Init, Kill, Render, VBlank y macro EFFECT |
| [03-libreria-2d.md](03-libreria-2d.md) | **Lib2D**: matrices 2D, transformaciones, recorte de líneas y polígonos (Liang-Barsky, Sutherland-Hodgman) |
| [04-libreria-3d.md](04-libreria-3d.md) | **Lib3D**: objetos 3D, transformaciones, visibilidad de caras, ordenación y shading |
| [05-graficos-amiga.md](05-graficos-amiga.md) | Gráficos en Amiga: bitmap, copper, blitter, sprites, modos de pantalla |
| [06-catalogo-efectos.md](06-catalogo-efectos.md) | Catálogo de todos los efectos del repositorio con descripción de cada uno |
| [07-referencia-apis.md](07-referencia-apis.md) | Referencia rápida de APIs (effect, gfx, blitter, copper, 2d, 3d) |

## Contexto

La información se ha deducido del código fuente del repositorio y del *Amiga Hardware Reference Manual* (1985, Commodore). Las librerías **lib2d** y **lib3d** están optimizadas para el 68000 y los chips custom (blitter, copper) y condensan técnicas útiles para futuros desarrollos.

## Uso recomendado

- Para **empezar**: leer 01-introduccion y 02-sistema-efectos.
- Para **gráficos 2D/3D**: 03-libreria-2d, 04-libreria-3d y 05-graficos-amiga.
- Para **elegir o entender un efecto**: 06-catalogo-efectos.
- Para **consultar funciones**: 07-referencia-apis.
