#ifndef SPACIAL_SPACIAL_H
#define SPACIAL_SPACIAL_H

struct SpContext *SpContextCreate();
void SpContextDestroy(struct SpContext *ctx);

void SpContextSetResolution(struct SpContext *ctx, int resolution);

int SpContextAddTriangle(
  struct SpContext *ctx,
  float ax, float ay, float az,
  float bx, float by, float bz,
  float cx, float cy, float cz);

void SpContextDump(struct SpContext *ctx);

typedef struct SpContext SpContext;

#endif
