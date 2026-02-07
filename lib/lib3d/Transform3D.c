/*
 * Transform3D — Aplica una matriz 3×3 + traslación a n puntos 3D.
 * Cada punto (x,y,z) se multiplica por M y se suma la traslación; resultado en fixed-point
 * con normfx. Usado para pasar vértices de espacio objeto a mundo o cámara.
 */
#include <3d.h>
#include <fx.h>

#define MULVERTEX() {                 \
  short v0 = (*v++);                  \
  short v1 = (*v++);                  \
  short v2 = (*v++);                  \
  short t3 = (*v++);                  \
  int t0 = v0 * x;                    \
  int t1 = v1 * y;                    \
  int t2 = v2 * z;                    \
  *dst++ = normfx(t0 + t1 + t2) + t3; \
}

void Transform3D(Matrix3D *M, Point3D *out, Point3D *in, short n) {
  short *src = (short *)in;
  short *dst = (short *)out;

  while (--n >= 0) {
    short *v = (short *)M;
    short x = *src++;
    short y = *src++;
    short z = *src++;

    MULVERTEX();
    MULVERTEX();
    MULVERTEX();

    src++; dst++;
  }
}
