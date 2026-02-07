# Catálogo de efectos

Descripción breve de cada efecto del repositorio, deducida del código y de los comentarios.

| Efecto | Carpeta | Descripción |
|--------|---------|-------------|
| **Abduction** | abduction | Efecto visual de “abducción” (animación/escena temática). |
| **Anim** | anim | Animación de sprites o gráficos pregenerados (datos en data/, gen-anim.py). |
| **AnimPolygons** | anim-polygons | Polígonos 2D animados a partir de datos SVG (cock.svg, dancing.svg) convertidos con encode.py. |
| **Ball** | ball | Bola o objeto que rebota; datos generados con gen-ball.py. |
| **Blurred** | blurred | Efecto de desenfoque sobre el bitmap (blur por CPU o blitter). |
| **Blurred3D** | blurred3d | Objeto 3D con efecto de blur (múltiples pasadas o acumulación). Datos: szescian.mtl. |
| **Bobs3D** | bobs3d | Bobs (objetos movibles) en 3D; datos pilka.mtl. |
| **BumpMapRGB** | bumpmap-rgb | Bump mapping en RGB; scripts bumpmap.py, light.py para datos. |
| **ButterflyGears** | butterfly-gears | Engranajes tipo “mariposa”; textureloop.asm y datos generados (gen-unrolled-loop.py). |
| **Carrion** | carrion | Muestra una imagen estática partida (split-img.py). |
| **Cathedral** | cathedral | Efecto tipo “catedral” (rayos, perspectiva o túneles). |
| **Circles** | circles | Dibujo de círculos (libgfx Circle/CircleEdge o equivalente). |
| **Logo (Color-cycling)** | color-cycling | Logo con color cycling en la paleta. |
| **Credits** | credits | Pantalla de créditos (texto o lista de nombres). |
| **Darkroom** | darkroom | Efecto de “cuarto oscuro” (revelado, fade o contraste). |
| **Dna3D** | dna3d | Doble hélice 3D (DNA); datos dna_gen.py, bobs, necrocoq. |
| **Empty** | empty | Efecto vacío: solo cambia color de fondo y llama a OptimizedFunction(); plantilla para pruebas. |
| **FireRGB** | fire-rgb | Fuego en modo RGB; buffer chunky, conversión a planar (dualtab); gen-dualtab.py. |
| **FlatShade** | flatshade | Objeto 3D con shading plano (iluminación por cara); VBlank; datos codi.mtl. |
| **FlatShadeConvex** | flatshade-convex | Shading plano para mallas convexas; datos pilka.mtl. |
| **Floor** | floor | Suelo con franjas y profundidad; manipulación de paleta y registro BPLCON1 por línea (scroll horizontal); datos floor.c, stripes.c, stripeWidth, stripeLight. |
| **FloorOld** | floor-old | Versión anterior del suelo (más compleja o distinta técnica). |
| **Forest** | forest | Bosque (árboles, partículas o sprites). |
| **GameOfLife** | game-of-life | Conway’s Game of Life: reglas clásicas; blitter con minterms para contar vecinos; estados previos en bitplanes con colores más oscuros; datos RLE. |
| **Glitch** | glitch | Glitch visual (corrupción de bitmap o paleta). |
| **Glitches** | glitches | Varios glitches; datos gen_tearing.py. |
| **GrowingTree** | growing-tree | Árbol que crece (L-system o ramificación procedural). |
| **GUI** | gui | Interfaz gráfica de prueba (botones, texto, libgui). |
| **HighWay** | highway | Efecto de carretera (perspectiva, líneas que se acercan). |
| **KbdTest** | kbtest | Prueba de teclado (muestra teclas pulsadas). |
| **Credits (Layers)** | layers | Capas (varios planos o prioridades); en el código se registra como “Credits”. |
| **Loader** | loader | Barra de carga con líneas dibujadas por CPU y módulo Protracker; datos loader.c. |
| **Lines** | lines | Líneas (posiblemente con optimizaciones en asm: CpuEdgeOpt, CpuLineOpt). |
| **MagnifyingGlass** | magnifying-glass | Lupa que distorsiona la imagen; datos ball-anim.csv, gen-lens.py. |
| **MetaBalls** | metaballs | Metaballs (blobs que se fusionan); datos gen-metaball.py. |
| **MultiPipe** | multipipe | Múltiples “pipes” o túneles; stripes.c. |
| **Neons** | neons | Neones/tubo de neón; rotación de paleta en VBlank (CustomRotatePalette); datos neons.py. |
| **Plasma** | plasma | Plasma clásico; copper chunky 8×4, tablas precalculadas; plasma-colors.c. |
| **PlayAHX** | playahx | Reproductor de música AHX. |
| **PlayCinter** | playctr | Reproductor Cinter (CinterConvert, delta); música en VBlank. |
| **PlayP61** | playp61 | Reproductor P61 (OctaMED); VBlank = P61_Music. |
| **PlayProtracker** | playpt | Reproductor Protracker; datos virgill-*.asm/.raw. |
| **Plotter** | plotter | Efecto tipo plotter (trazos, flares); datos plotter-flares.py. |
| **Prisms** | prisms | Prismas (objetos 3D o refracción visual). |
| **Roller** | roller | Efecto de rodillo (scroll vertical o cilindro). |
| **Rotator** | rotator | Rotador (imagen o bitmap girando); mainloop.asm. |
| **SeaAnemone** | sea-anemone | Anémona de mar (tentáculos, partículas). |
| **Shapes** | shapes | Formas 2D (lib2d: transformaciones, recorte). |
| **ShowPCHG** | showpchg | Muestra animación de paleta PCHG (color cycling por línea/frame). |
| **SpookyTree** | spooky-tree | Árbol “fantasma”; imagen partida (split-img.py); VBlank. |
| **Stencil3D** | stencil3d | 3D con stencil (máscara o recorte); datos cock-anim.csv, kurak-head.mtl. |
| **Stripes** | stripes | Franjas (scroll o animación de bandas). |
| **TexTri** | texobj | Triángulos texturizados (objeto 3D con textura); datos cube.mtl; mainloop.asm. |
| **TextScroll** | textscroll | Scroll de texto; datos text-scroll.txt. |
| **Thunders** | thunders | Rayos o relámpagos. |
| **Tiles8** | tiles8 | Tiles 8×8; twist-colors, twist.py. |
| **Tiles16** | tiles16 | Tiles 16×16. |
| **TileZoomer** | tilezoomer | Zoom sobre tiles. |
| **Transparency** | transparency | Demostración de transparencia (blitter o mezcla de planos). |
| **Turmite** | turmite | Turmite (autómata sobre rejilla); datos RLE/patrones. |
| **TwisterRGB** | twister-rgb | Efecto “twister” en RGB. |
| **UVLight** | uvlight | Malla 3D con UV y luz; gen-uvmap.py. |
| **UVMap** | uvmap | Mapeado UV 3D; mainloop.asm; gen-uvmap.py. |
| **UVMapRGB** | uvmap-rgb | UV mapping en RGB. |
| **Weave** | weave | Efecto de tejido (patrón entrelazado). |
| **Wireframe** | wireframe | Objeto 3D en wireframe (aristas); datos pilka; visibilidad de caras. |

## Leyenda

- **Load/UnLoad**: precalcula o libera datos en background.
- **VBlank**: actualiza paleta, música o punteros cada frame.
- **Datos .mtl / .c / .py**: mallas, paletas o tablas generadas con herramientas del repo (obj2c, png2c, scripts Python).
- Efectos **3D** usan lib3d (Object3D, UpdateFaceVisibility, SortFaces, etc.).
- Efectos **2D** usan lib2d, blitter y/o copper (plasma, floor, game-of-life).

Para ver la implementación concreta, abrir el `.c` correspondiente en `effects/<nombre>/` y buscar `EFFECT(...)` y las funciones Init, Render, Kill.
