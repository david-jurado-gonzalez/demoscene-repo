/*
 * StarFox-style demo: nave, enemigos, planeta, túnel y disparos.
 * Basado en flatshade-convex. Duración ~2 minutos (6000 frames @ 50Hz).
 */
#include <stdlib.h>
#include <strings.h>
#include "effect.h"
#include "blitter.h"
#include "copper.h"
#include "3d.h"
#include "fx.h"

#define WIDTH   256
#define HEIGHT  256
#define DEPTH   4

#define FRAMES_END  6000   /* 2 min @ 50Hz */
#define MAX_ENEMIES 10
#define MAX_BULLETS 24
#define TUNNEL_HORIZON  70
#define SHIP_Z          fx4i(-90)
#define PLANET_Z        fx4i(-600)
#define ENEMY_Z_START   fx4i(-350)
#define ENEMY_Z_END     fx4i(-120)
#define BULLET_SPEED    6

static Object3D *ship;
static Object3D *planet;
static Object3D *enemies[MAX_ENEMIES];
static short enemyZ[MAX_ENEMIES];
static short enemyX[MAX_ENEMIES];
static short nEnemies;

static short bulletX[MAX_BULLETS];
static short bulletY[MAX_BULLETS];
static short nBullets;

static CopListT *cp;
static CopInsPairT *bplptr;
static BitmapT *screen[2];
static short active;

#include "data/starfox-pal.c"
#include "data/pilka.c"
#include "data/cube.c"

static void Init(void) {
  short i;

  ship = NewObject3D(&cube);
  ship->translate.x = 0;
  ship->translate.y = fx4i(40);
  ship->translate.z = SHIP_Z;
  /* scale en formato fx12 como en lib3d (NewObject3D); fx4i(0.5) daría 0 por truncado */
  ship->scale.x = ship->scale.y = ship->scale.z = fx12f(0.5);

  planet = NewObject3D(&pilka);
  planet->translate.x = 0;
  planet->translate.y = 0;
  planet->translate.z = PLANET_Z;
  planet->scale.x = planet->scale.y = planet->scale.z = fx12f(1.2);

  for (i = 0; i < MAX_ENEMIES; i++) {
    enemies[i] = NewObject3D(&cube);
    enemyZ[i] = ENEMY_Z_START - (i * (ENEMY_Z_START - ENEMY_Z_END) / (MAX_ENEMIES + 1));
    enemyX[i] = (short)((random() % 160) - 80);
  }
  nEnemies = MAX_ENEMIES;

  for (i = 0; i < MAX_BULLETS; i++)
    bulletY[i] = -1;
  nBullets = 0;

  screen[0] = NewBitmap(WIDTH, HEIGHT, DEPTH, BM_CLEAR);
  screen[1] = NewBitmap(WIDTH, HEIGHT, DEPTH, BM_CLEAR);

  SetupPlayfield(MODE_LORES, DEPTH, X(32), Y(0), WIDTH, HEIGHT);
  LoadColors(starfox_colors, 0);

  cp = NewCopList(80);
  bplptr = CopSetupBitplanes(cp, screen[0], DEPTH);
  CopListFinish(cp);
  CopListActivate(cp);
  EnableDMA(DMAF_BLITTER | DMAF_RASTER | DMAF_BLITHOG);
}

static void Kill(void) {
  short i;
  DeleteBitmap(screen[0]);
  DeleteBitmap(screen[1]);
  DeleteCopList(cp);
  DeleteObject3D(ship);
  DeleteObject3D(planet);
  for (i = 0; i < MAX_ENEMIES; i++)
    DeleteObject3D(enemies[i]);
}

static void UpdateEdgeVisibilityConvex(Object3D *object) {
  register char s asm("d2") = 1;
  void *_objdat = object->objdat;
  short *group = object->faceGroups;
  short f;

  do {
    while ((f = *group++)) {
      register char flags asm("d3") = FACE(f)->flags;
      if (flags >= 0) {
        register short *index asm("a3") = (short *)(&FACE(f)->count);
        short vertices = *index++ - 3;
        short i;
        i = *index++; NODE3D(i)->flags = s;
        i = *index++; EDGE(i)->flags ^= flags;
        i = *index++; NODE3D(i)->flags = s;
        i = *index++; EDGE(i)->flags ^= flags;
        do {
          i = *index++; NODE3D(i)->flags = s;
          i = *index++; EDGE(i)->flags ^= flags;
        } while (--vertices != -1);
      }
    }
  } while (*group);
}

#define MULVERTEX1(D, E) {                      \
  short t0 = (*v++) + y;                        \
  short t1 = (*v++) + x;                        \
  int t2 = (*v++) * z;                          \
  v++;                                          \
  D = ((t0 * t1 + t2 - xy) >> 4) + E;           \
}

#define MULVERTEX2(D) {                         \
  short t0 = (*v++) + y;                        \
  short t1 = (*v++) + x;                        \
  int t2 = (*v++) * z;                          \
  short t3 = (*v++);                            \
  D = normfx(t0 * t1 + t2 - xy) + t3;           \
}

static void TransformVertices(Object3D *object) {
  Matrix3D *M = &object->objectToWorld;
  void *_objdat = object->objdat;
  short *group = object->vertexGroups;

  int m0 = (M->x << 8) - ((M->m00 * M->m01) >> 4);
  int m1 = (M->y << 8) - ((M->m10 * M->m11) >> 4);
  M->z -= normfx(M->m20 * M->m21);

  do {
    short i;
    while ((i = *group++)) {
      if (NODE3D(i)->flags) {
        short *pt = (short *)NODE3D(i);
        short *v = (short *)M;
        short x, y, z, zp;
        int xy, xp, yp;
        *pt++ = 0;
        x = *pt++;
        y = *pt++;
        z = *pt++;
        xy = x * y;
        MULVERTEX1(xp, m0);
        MULVERTEX1(yp, m1);
        MULVERTEX2(zp);
        /* Evitar división por cero: vértices en o detrás del plano de la cámara (zp <= 0) */
        if (zp <= 0) {
          *pt++ = WIDTH / 2;
          *pt++ = HEIGHT / 2;
        } else {
          *pt++ = div16(xp, zp) + WIDTH / 2;
          *pt++ = div16(yp, zp) + HEIGHT / 2;
        }
        *pt++ = zp;
      }
    }
  } while (*group);
}

static void DrawObject(void *planes, Object3D *object,
                       CustomPtrT custom_ asm("a6")) {
  void *_objdat = object->objdat;
  short *group = object->edgeGroups;

  _WaitBlitter(custom_);
  custom_->bltafwm = -1;
  custom_->bltalwm = -1;
  custom_->bltadat = 0x8000;
  custom_->bltbdat = 0xffff;
  custom_->bltcmod = WIDTH / 8;
  custom_->bltdmod = WIDTH / 8;

  do {
    short e;
    while ((e = *group++)) {
      char edgeColor = EDGE(e)->flags;
      short *edge = (short *)EDGE(e);

      if (edgeColor > 0) {
        short bltcon0, bltcon1, bltsize, bltbmod, bltamod;
        int bltapt, bltcpt;
        *edge++ = 0;
        {
          short x0, y0, x1, y1;
          short dmin, dmax, derr;
          short *pt;
          short i;
          i = *edge++;
          pt = (short *)VERTEX(i);
          x0 = *pt++;
          y0 = *pt++;
          i = *edge++;
          pt = (short *)VERTEX(i);
          x1 = *pt++;
          y1 = *pt++;
          if (y0 == y1)
            continue;
          if (y0 > y1) {
            swapr(x0, x1);
            swapr(y0, y1);
          }
          dmax = x1 - x0;
          if (dmax < 0)
            dmax = -dmax;
          dmin = y1 - y0;
          if (dmax >= dmin) {
            if (x0 >= x1)
              bltcon1 = AUL | SUD | LINEMODE | ONEDOT;
            else
              bltcon1 = SUD | LINEMODE | ONEDOT;
          } else {
            if (x0 >= x1)
              bltcon1 = SUL | LINEMODE | ONEDOT;
            else
              bltcon1 = LINEMODE | ONEDOT;
            swapr(dmax, dmin);
          }
          bltcpt = (int)planes + (short)(((y0 << 5) + (x0 >> 3)) & ~1);
          bltcon0 = rorw(x0 & 15, 4) | BC0F_LINE_EOR;
          bltcon1 |= rorw(x0 & 15, 4);
          dmin <<= 1;
          derr = dmin - dmax;
          bltamod = derr - dmax;
          bltbmod = dmin;
          bltsize = (dmax << 6) + 66;
          bltapt = derr;
        }
#define DRAWLINE()                              \
        _WaitBlitter(custom_);                  \
        custom_->bltcon0 = bltcon0;             \
        custom_->bltcon1 = bltcon1;             \
        custom_->bltcpt = (void *)bltcpt;        \
        custom_->bltapt = (void *)bltapt;        \
        custom_->bltdpt = planes;                \
        custom_->bltbmod = bltbmod;              \
        custom_->bltamod = bltamod;             \
        custom_->bltsize = bltsize;
        {
          if (edgeColor & 1) { DRAWLINE(); }
          bltcpt += WIDTH * HEIGHT / 8;
          if (edgeColor & 2) { DRAWLINE(); }
          bltcpt += WIDTH * HEIGHT / 8;
          if (edgeColor & 4) { DRAWLINE(); }
          bltcpt += WIDTH * HEIGHT / 8;
          if (edgeColor & 8) { DRAWLINE(); }
        }
      }
    }
  } while (*group);
}

/* Dibuja un segmento de línea en color 0-15 (para túnel y disparos). */
static void DrawLineSeg(void *planes, short x0, short y0, short x1, short y1,
                        short color, CustomPtrT custom_ asm("a6")) {
  short bltcon0, bltcon1, bltsize, bltbmod, bltamod;
  int bltapt, bltcpt;
  short dmin, dmax, derr;
  int bplSize = WIDTH * HEIGHT / 8;

  if (color <= 0)
    return;
  if (y0 == y1)
    return;
  if (y0 > y1) {
    swapr(x0, x1);
    swapr(y0, y1);
  }
  dmax = x1 - x0;
  if (dmax < 0)
    dmax = -dmax;
  dmin = y1 - y0;
  if (dmax >= dmin) {
    if (x0 >= x1)
      bltcon1 = AUL | SUD | LINEMODE | ONEDOT;
    else
      bltcon1 = SUD | LINEMODE | ONEDOT;
  } else {
    if (x0 >= x1)
      bltcon1 = SUL | LINEMODE | ONEDOT;
    else
      bltcon1 = LINEMODE | ONEDOT;
    swapr(dmax, dmin);
  }
  bltcpt = (int)planes + (short)(((y0 << 5) + (x0 >> 3)) & ~1);
  bltcon0 = rorw(x0 & 15, 4) | BC0F_LINE_EOR;
  bltcon1 |= rorw(x0 & 15, 4);
  dmin <<= 1;
  derr = dmin - dmax;
  bltamod = derr - dmax;
  bltbmod = dmin;
  bltsize = (dmax << 6) + 66;
  bltapt = derr;

  if (color & 1) {
    _WaitBlitter(custom_);
    custom_->bltcon0 = bltcon0;
    custom_->bltcon1 = bltcon1;
    custom_->bltcpt = (void *)bltcpt;
    custom_->bltapt = (void *)bltapt;
    custom_->bltdpt = planes;
    custom_->bltbmod = bltbmod;
    custom_->bltamod = bltamod;
    custom_->bltsize = bltsize;
  }
  if (color & 2) {
    _WaitBlitter(custom_);
    custom_->bltcon0 = bltcon0;
    custom_->bltcon1 = bltcon1;
    custom_->bltcpt = (void *)bltcpt;
    custom_->bltapt = (void *)bltapt;
    custom_->bltdpt = (void *)((char *)planes + bplSize);
    custom_->bltbmod = bltbmod;
    custom_->bltamod = bltamod;
    custom_->bltsize = bltsize;
  }
  if (color & 4) {
    _WaitBlitter(custom_);
    custom_->bltcon0 = bltcon0;
    custom_->bltcon1 = bltcon1;
    custom_->bltcpt = (void *)bltcpt;
    custom_->bltapt = (void *)bltapt;
    custom_->bltdpt = (void *)((char *)planes + 2 * bplSize);
    custom_->bltbmod = bltbmod;
    custom_->bltamod = bltamod;
    custom_->bltsize = bltsize;
  }
  if (color & 8) {
    _WaitBlitter(custom_);
    custom_->bltcon0 = bltcon0;
    custom_->bltcon1 = bltcon1;
    custom_->bltcpt = (void *)bltcpt;
    custom_->bltapt = (void *)bltapt;
    custom_->bltdpt = (void *)((char *)planes + 3 * bplSize);
    custom_->bltbmod = bltbmod;
    custom_->bltamod = bltamod;
    custom_->bltsize = bltsize;
  }
}

static void DrawTunnel(void *planes, CustomPtrT custom_ asm("a6")) {
  short cx = WIDTH / 2;
  short horizon = TUNNEL_HORIZON;
  short color = 4;
  /* Líneas desde horizonte hacia abajo: túnel tipo Star Fox */
  DrawLineSeg(planes, cx, horizon, 32, HEIGHT - 1, color, custom_);
  DrawLineSeg(planes, cx, horizon, WIDTH - 32, HEIGHT - 1, color, custom_);
  DrawLineSeg(planes, cx, horizon, cx - 60, HEIGHT - 1, color, custom_);
  DrawLineSeg(planes, cx, horizon, cx + 60, HEIGHT - 1, color, custom_);
  DrawLineSeg(planes, 32, HEIGHT - 1, WIDTH - 32, HEIGHT - 1, color, custom_);
}

static void DrawBullets(void *planes, CustomPtrT custom_ asm("a6")) {
  short i;
  short color = 15;
  for (i = 0; i < MAX_BULLETS; i++) {
    short x = bulletX[i];
    short y = bulletY[i];
    if (y < 0)
      continue;
    /* Pequeño rectángulo 4x4 como disparo */
    DrawLineSeg(planes, x, y, x + 4, y, color, custom_);
    DrawLineSeg(planes, x + 4, y, x + 4, y + 4, color, custom_);
    DrawLineSeg(planes, x + 4, y + 4, x, y + 4, color, custom_);
    DrawLineSeg(planes, x, y + 4, x, y, color, custom_);
  }
}

static void BitmapClearFast(BitmapT *dst) {
  u_short height = (short)dst->height * (short)dst->depth;
  u_short bltsize = (height << 6) | (dst->bytesPerRow >> 1);
  void *bltpt = dst->planes[0];
  WaitBlitter();
  custom->bltcon0 = DEST | A_TO_D;
  custom->bltcon1 = 0;
  custom->bltafwm = -1;
  custom->bltalwm = -1;
  custom->bltadat = 0;
  custom->bltdmod = 0;
  custom->bltdpt = bltpt;
  custom->bltsize = bltsize;
}

static void BitmapFillFast(BitmapT *dst) {
  void *bltpt = dst->planes[0] + (dst->bplSize * DEPTH) - 2;
  u_short bltsize = (0 << 6) | (WIDTH >> 4);
  WaitBlitter();
  custom->bltapt = bltpt;
  custom->bltdpt = bltpt;
  custom->bltamod = 0;
  custom->bltdmod = 0;
  custom->bltcon0 = (SRCA | DEST) | A_TO_D;
  custom->bltcon1 = BLITREVERSE | FILL_XOR;
  custom->bltafwm = -1;
  custom->bltalwm = -1;
  custom->bltsize = bltsize;
  WaitBlitter();
}

static short enemyOrder[MAX_ENEMIES];

static int compareEnemyZ(const void *a, const void *b) {
  short ia = *(const short *)a;
  short ib = *(const short *)b;
  return (int)enemyZ[ia] - (int)enemyZ[ib];
}

static void Render(void) {
  short i;
  void *planes = screen[active]->planes[0];

  if (frameCount >= FRAMES_END) {
    exitLoop = true;
    return;
  }

  BitmapClearFast(screen[active]);

  /* Túnel (fondo) */
  _WaitBlitter(custom);
  custom->bltafwm = -1;
  custom->bltalwm = -1;
  custom->bltadat = 0x8000;
  custom->bltbdat = 0xffff;
  custom->bltcmod = WIDTH / 8;
  custom->bltdmod = WIDTH / 8;
  DrawTunnel(planes, custom);

  /* Planeta (muy atrás) */
  planet->rotate.x = frameCount * 2;
  planet->rotate.y = frameCount * 3;
  planet->rotate.z = frameCount;
  UpdateObjectTransformation(planet);
  UpdateFaceVisibility(planet);
  UpdateEdgeVisibilityConvex(planet);
  TransformVertices(planet);
  DrawObject(planes, planet, custom);

  /* Enemigos: avanzan hacia la cámara, orden por Z (lejos primero) */
  for (i = 0; i < MAX_ENEMIES; i++) {
    enemyOrder[i] = i;
    enemyZ[i] += 12;
    if (enemyZ[i] > ENEMY_Z_END) {
      enemyZ[i] = ENEMY_Z_START;
      enemyX[i] = (short)((random() % 160) - 80);
    }
    enemies[i]->translate.z = enemyZ[i];
    enemies[i]->translate.x = fx4i(enemyX[i]);
    enemies[i]->translate.y = normfx(COS(frameCount * 4 + i * 200) * 20);
    enemies[i]->rotate.x = frameCount * 5 + i * 100;
    enemies[i]->rotate.y = frameCount * 4;
    enemies[i]->rotate.z = frameCount * 3;
  }
  qsort(enemyOrder, MAX_ENEMIES, sizeof(short), compareEnemyZ);
  for (i = 0; i < MAX_ENEMIES; i++) {
    short idx = enemyOrder[i];
    UpdateObjectTransformation(enemies[idx]);
    UpdateFaceVisibility(enemies[idx]);
    UpdateEdgeVisibilityConvex(enemies[idx]);
    TransformVertices(enemies[idx]);
    DrawObject(planes, enemies[idx], custom);
  }

  /* Nave: delante, abajo, ligera rotación y balanceo */
  ship->rotate.x = frameCount * 4;
  ship->rotate.y = frameCount * 6;
  ship->rotate.z = 0;
  ship->translate.x = normfx(SIN(frameCount * 2) * 30);
  UpdateObjectTransformation(ship);
  UpdateFaceVisibility(ship);
  UpdateEdgeVisibilityConvex(ship);
  TransformVertices(ship);
  DrawObject(planes, ship, custom);

  /* Disparos: spawn periódico y movimiento */
  if ((frameCount & 3) == 0 && nBullets < MAX_BULLETS) {
    for (i = 0; i < MAX_BULLETS; i++) {
      if (bulletY[i] < 0) {
        bulletX[i] = WIDTH / 2 - 2 + (short)((random() % 8) - 4);
        bulletY[i] = HEIGHT - 40;
        nBullets++;
        break;
      }
    }
  }
  for (i = 0; i < MAX_BULLETS; i++) {
    if (bulletY[i] >= 0) {
      bulletY[i] -= BULLET_SPEED;
      if (bulletY[i] < 0)
        nBullets--;
    }
  }
  DrawBullets(planes, custom);

  BitmapFillFast(screen[active]);
  CopUpdateBitplanes(bplptr, screen[active], DEPTH);
  TaskWaitVBlank();
  active ^= 1;
}

EFFECT(Starfox, NULL, NULL, Init, Kill, Render, NULL);
