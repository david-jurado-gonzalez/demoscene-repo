/*
 * SortFaces — Ordena las caras visibles del objeto por Z medio (suma de z de los 3 vértices).
 * Rellena visibleFace con pares (z, índice_cara) y llama a SortItemArray para pintar
 * de atrás adelante (painter's algorithm). Incluye un guard (0, -1) al final de la lista.
 */
#include <3d.h>

void SortFaces(Object3D *object) {
  short *item = (short *)object->visibleFace;
  short count = 0;

  void *_objdat = object->objdat;
  short *group = object->faceGroups;

  do {
    short f;

    while ((f = *group++)) {
      if (FACE(f)->flags >= 0) {
        short z;
        short i;

        i = FACE(f)->indices[0].vertex;
        z = VERTEX(i)->z;
        i = FACE(f)->indices[1].vertex;
        z += VERTEX(i)->z;
        i = FACE(f)->indices[2].vertex;
        z += VERTEX(i)->z;

        *item++ = z;
        *item++ = f;
        count++;
      }
    }
  } while (*group);

  /* guard element */
  *item++ = 0;
  *item++ = -1;

  SortItemArray(object->visibleFace, count);
}
