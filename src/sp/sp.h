#ifndef SP_SP_H
#define SP_SP_H

struct SpContext;
typedef struct SpContext SpContext;

struct SpContext *SpContextCreate();
void SpContextDestroy(struct SpContext *ctx);

void SpContextSetResolution(struct SpContext *ctx, int resolution);

int SpContextAddTriangle(
  struct SpContext *ctx,
  float ax, float ay, float az,
  float bx, float by, float bz,
  float cx, float cy, float cz);

void SpContextDump(struct SpContext *ctx);

#endif
