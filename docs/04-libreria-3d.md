# Librería 3D (lib3d)

La **lib3d** implementa objetos 3D, transformaciones, visibilidad de caras y ordenación para renderizado en Amiga. Está pensada para mallas estáticas (vértices, aristas, caras) y usa **fixed-point** y tablas (sin/cos, invsqrt) para no usar floats.

## Tipos principales (3d.h)

- **Point3D** — `short x, y, z`
- **UVCoordT** — `short u, v` (coordenadas de textura)
- **Node3D** — `flags`, `point` (posición mundial), `vertex` (proyectado/pantalla)
- **EdgeT** — `flags`, `point[2]` (índices de vértices)
- **Matrix3D** — Matriz 3×3 + traslación (m00..m22, x, y, z)
- **FaceT** — `normal[3]`, `flags` (visible/color), `material`, `count`, `indices[]` (FaceIndexT: vertex, edge)
- **Object3D** — Objeto instanciado: punteros a datos del mesh, `rotate`, `scale`, `translate`, matrices `objectToWorld` y `worldToObject`, `camera` (en espacio objeto), lista `visibleFace` para ordenación.
- **Mesh3D** — Definición de malla: contadores (vertices, texcoords, edges, faces, materials), puntero `data`, y grupos (vertexGroups, edgeGroups, faceGroups, objects). Los datos suelen generarse con **obj2c** a partir de un .obj.

## Transformaciones 3D

- **LoadIdentity3D(M)** — M = I.
- **Translate3D(M, x, y, z)** — Añade traslación.
- **Scale3D(M, sx, sy, sz)** — Añade escala.
- **LoadRotate3D(M, ax, ay, az)** — Carga M con rotación Rx(ax)*Ry(ay)*Rz(az) (ángulos en unidades de tabla sin/cos).
- **LoadReverseRotate3D(M, ax, ay, az)** — Rotación inversa (transpuesta), usada para worldToObject.
- **Compose3D(md, ma, mb)** — md = ma * mb.
- **Transform3D(M, out, in, n)** — Aplica M a n puntos 3D; usa macro MULVERTEX con normfx.

Orden típico para objectToWorld: **Rx*Ry*Rz*Scale*Translate**. Para worldToObject: **Translate^(-1)*Scale^(-1)*Rz*Ry*Rx** (para cámara en origen).

## Objetos y mallas

- **NewObject3D(mesh)** — Crea un Object3D a partir de un Mesh3D (copia referencias a datos y reserva visibleFace).
- **DeleteObject3D(object)** — Libera el objeto.
- **UpdateObjectTransformation(object)** — Calcula objectToWorld y worldToObject a partir de rotate, scale, translate; y la posición de la cámara en espacio objeto (para visibilidad).

## Visibilidad e iluminación

- **UpdateFaceVisibility(object)** — Para cada cara, calcula el producto escalar entre la normal y el vector hacia la cámara (en espacio objeto). Si ≥ 0, la cara es visible; entonces se calcula un término de luz (invsqrt de la distancia) y se guarda en `face->flags` un color 0..15. Si no es visible y la cara es de doble cara (material < 0), se usa la normal invertida. Usa tabla **InvSqrt[]** para normalizar el factor de luz sin raíz cuadrada en tiempo real.
- **UpdateVertexVisibility(object)** — Marca qué vértices pertenecen a caras visibles (para wireframe/edges).
- **AllFacesDoubleSided(object)** — Marca todas las caras como de doble cara (siempre visibles o con luz por ambas caras).

## Ordenación de caras

- **SortFaces(object)** — Rellena `visibleFace` con las caras visibles (flags ≥ 0), ordenadas por Z medio de la cara (suma de z de los 3 vértices). Incluye un guard (0, -1) al final. Usa `SortItemArray` (ordenación por Z para pintar de atrás adelante).
- **SortFacesMinZ(object)** — Variante que ordena por la Z mínima de la cara (útil para ciertos efectos).

## Macros de acceso a datos

Con `_objdat` definido como el puntero a datos del objeto:

- **NODE3D(i)**, **POINT(i)**, **VERTEX(i)** — Acceso al nodo i, posición mundial y vértice proyectado.
- **UVCOORD(i)**, **EDGE(i)**, **FACE(i)** — Coordenadas UV, arista y cara por índice.

Los índices vienen de los grupos (faceGroups, etc.): se recorren con `while ((f = *group++))` hasta un 0.

## Flujo típico de render 3D

1. Actualizar ángulos/posición (rotate, translate, scale).
2. **UpdateObjectTransformation(object)**.
3. **Transform3D(objectToWorld, points, points, n)** (o equivalente) para pasar vértices a mundo; luego proyección a 2D (perspectiva o paralela) escribiendo en `vertex`.
4. **UpdateFaceVisibility(object)** (y opcionalmente UpdateVertexVisibility).
5. **SortFaces(object)** (o SortFacesMinZ).
6. Dibujar caras en el orden de visibleFace (flatshade, textura, etc.) o dibujar aristas para wireframe.

## Archivos de lib3d

| Archivo | Función |
|---------|---------|
| LoadIdentity3D.c, Translate3D.c, Scale3D.c | Identidad, traslación, escala |
| LoadRotate3D.c, LoadReverseRotate3D.c | Rotación 3D (Euler) e inversa |
| Compose3D.c | Multiplicación de matrices 3D |
| Transform3D.c | Aplicar matriz a N puntos |
| NewObject3D.c, DeleteObject3D.c | Crear/destruir objeto |
| UpdateObjectTransformation.c | objectToWorld, worldToObject, cámara en objeto |
| UpdateFaceVisibility.c | Back-face culling + luz por cara (InvSqrt) |
| UpdateVertexVisibility.c | Marcado de vértices visibles |
| SortFaces.c, SortFacesMinZ.c | Ordenación por Z para painter's algorithm |
| AllFacesDoubleSided.c | Caras de doble cara |

## Optimizaciones para Amiga

- **Fixed-point** en todo; seno/coseno vía tabla (`SIN`, `COS` en fx.h).
- **InvSqrt[]** precalculada para iluminación por cara sin sqrt().
- Estructuras compactas (short, char) para caber en caché y menos ancho de banda.
- Ordenación por Z en CPU; el dibujado puede hacerse con blitter o CPU según el efecto (wireframe, flatshade, textura).

Esta librería es la base de efectos como wireframe, flatshade, flatshade-convex, texobj, uvmap, stencil3d, bobs3d, blurred3d, dna3d, etc., y puede reutilizarse en juegos 3D simples (objetos estáticos, cámara rotando).
