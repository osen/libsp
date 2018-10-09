#include "sp.h"

#include <stdlib.h>
#include <stdio.h>
#include <float.h>

struct SpVector3
{
  float x;
  float y;
  float z;
};

struct SpExtent
{
  struct SpVector3 min;
  struct SpVector3 max;
};

void _SpExtentReset(struct SpExtent* ctx)
{
  ctx->min.x = FLT_MAX;
  ctx->min.y = FLT_MAX;
  ctx->min.z = FLT_MAX;
  ctx->max.x = -FLT_MAX;
  ctx->max.y = -FLT_MAX;
  ctx->max.z = -FLT_MAX;
}

struct SpTriangle
{
  struct SpVector3 a;
  struct SpVector3 b;
  struct SpVector3 c;
};

struct SpPartition
{
  struct SpTriangle *triangles;
  size_t triangleCount;
  struct SpVector3 position;
  struct SpVector3 size;
};

struct SpContext
{
  int dirty;
  int resolution;
  struct SpExtent extent;

  struct SpPartition **partitions;
  size_t partitionCount;

  struct SpTriangle *triangles;
  size_t triangleCount;
};

struct SpPartition *_SpPartitionCreate(
  struct SpVector3 position, struct SpVector3 size)
{
  struct SpPartition *rtn = NULL;

  rtn = calloc(1, sizeof(*rtn));
  rtn->position = position;
  rtn->size = size;

  return rtn;
}

void _SpPartitionDestroy(struct SpPartition *ctx)
{
  if(ctx->triangles)
  {
    free(ctx->triangles);
  }
}

struct SpContext *SpContextCreate()
{
  struct SpContext *rtn = NULL;

  rtn = calloc(1, sizeof(*rtn));
  rtn->resolution = 10;

  return rtn;
}

void SpContextDestroy(struct SpContext *ctx)
{
  if(ctx->triangles)
  {
    free(ctx->triangles);
  }

  if(ctx->partitions)
  {
    struct SpPartition **pp = NULL;

    for(pp = ctx->partitions;
      pp < ctx->partitions + ctx->partitionCount; pp++)
    {
      _SpPartitionDestroy(*pp);
    }

    free(ctx->partitions);
  }
}

void SpContextSetResolution(struct SpContext *ctx, int resolution)
{
  ctx->resolution = resolution;
  ctx->dirty = 1;
}

int SpContextAddTriangle(
  struct SpContext *ctx,
  float ax, float ay, float az,
  float bx, float by, float bz,
  float cx, float cy, float cz)
{
  struct SpTriangle *triangles = NULL;
  struct SpTriangle triangle = {0};

  triangles = realloc(ctx->triangles,
    sizeof(*triangles) * (ctx->triangleCount + 1));

  if(!triangles)
  {
    return 1;
  }

  ctx->triangles = triangles;
  triangle.a.x = ax; triangle.a.y = ay; triangle.a.z = az;
  triangle.b.x = bx; triangle.b.y = by; triangle.b.z = bz;
  triangle.c.x = cx; triangle.c.y = cy; triangle.c.z = cz;
  triangles[ctx->triangleCount] = triangle;
  ctx->triangleCount++;
  ctx->dirty = 1;

  return 0;
}

void _SpContextUpdatePartitions(struct SpContext *ctx)
{
  struct SpVector3 partitionSize = {0};
  struct SpPartition **pp = NULL;

  if(ctx->partitions)
  {
    for(pp = ctx->partitions;
      pp < ctx->partitions + ctx->partitionCount; pp++)
    {
      _SpPartitionDestroy(*pp);
    }

    free(ctx->partitions);
    ctx->partitions = NULL;
    ctx->partitionCount = 0;
  }

  partitionSize.x = (ctx->extent.max.x - ctx->extent.min.x) / ctx->resolution;
  partitionSize.y = (ctx->extent.max.y - ctx->extent.min.y) / ctx->resolution;
  partitionSize.z = (ctx->extent.max.z - ctx->extent.min.z) / ctx->resolution;

  ctx->partitionCount = ctx->resolution * ctx->resolution;
  ctx->partitions = calloc(ctx->partitionCount, sizeof(*ctx->partitions));

  for(pp = ctx->partitions;
    pp < ctx->partitions + ctx->partitionCount; pp++)
  {
    *pp = _SpPartitionCreate(partitionSize, partitionSize);
  }
}

void _SpContextUpdateExtent(struct SpContext *ctx)
{
  struct SpTriangle *triangle = NULL;

  _SpExtentReset(&ctx->extent);

  for(triangle = ctx->triangles;
    triangle < ctx->triangles + ctx->triangleCount; triangle++)
  {
    if(triangle->a.x < ctx->extent.min.x) ctx->extent.min.x = triangle->a.x;
    if(triangle->a.y < ctx->extent.min.y) ctx->extent.min.y = triangle->a.y;
    if(triangle->a.z < ctx->extent.min.z) ctx->extent.min.z = triangle->a.z;
    if(triangle->a.x > ctx->extent.max.x) ctx->extent.max.x = triangle->a.x;
    if(triangle->a.y > ctx->extent.max.y) ctx->extent.max.y = triangle->a.y;
    if(triangle->a.z > ctx->extent.max.z) ctx->extent.max.z = triangle->a.z;

    if(triangle->b.x < ctx->extent.min.x) ctx->extent.min.x = triangle->b.x;
    if(triangle->b.y < ctx->extent.min.y) ctx->extent.min.y = triangle->b.y;
    if(triangle->b.z < ctx->extent.min.z) ctx->extent.min.z = triangle->b.z;
    if(triangle->b.x > ctx->extent.max.x) ctx->extent.max.x = triangle->b.x;
    if(triangle->b.y > ctx->extent.max.y) ctx->extent.max.y = triangle->b.y;
    if(triangle->b.z > ctx->extent.max.z) ctx->extent.max.z = triangle->b.z;

    if(triangle->c.x < ctx->extent.min.x) ctx->extent.min.x = triangle->c.x;
    if(triangle->c.y < ctx->extent.min.y) ctx->extent.min.y = triangle->c.y;
    if(triangle->c.z < ctx->extent.min.z) ctx->extent.min.z = triangle->c.z;
    if(triangle->c.x > ctx->extent.max.x) ctx->extent.max.x = triangle->c.x;
    if(triangle->c.y > ctx->extent.max.y) ctx->extent.max.y = triangle->c.y;
    if(triangle->c.z > ctx->extent.max.z) ctx->extent.max.z = triangle->c.z;
  }
}

void _SpContextUpdate(struct SpContext *ctx)
{
  if(!ctx->dirty)
  {
    return;
  }

  ctx->dirty = 0;
  _SpContextUpdateExtent(ctx);
  _SpContextUpdatePartitions(ctx);
}

void SpContextDump(struct SpContext *ctx)
{
  struct SpTriangle *triangle = NULL;
  struct SpPartition **pp = NULL;

  _SpContextUpdate(ctx);
  printf("Extent\n");

  printf("  Min: [%.2f, %.2f, %.2f]\n  Max: [%.2f, %.2f, %.2f]\n",
    ctx->extent.min.x, ctx->extent.min.y, ctx->extent.min.z,
    ctx->extent.max.x, ctx->extent.max.y, ctx->extent.max.z);

  printf("Triangles [%i]\n", (int)ctx->triangleCount);

  for(triangle = ctx->triangles;
    triangle < ctx->triangles + ctx->triangleCount; triangle++)
  {
    printf("  [%.2f, %.2f, %.2f][%.2f, %.2f, %.2f][%.2f, %.2f, %.2f]\n",
      triangle->a.x, triangle->a.y, triangle->a.z,
      triangle->b.x, triangle->b.y, triangle->b.z,
      triangle->c.x, triangle->c.y, triangle->c.z);
  }

  printf("Partitions [%i]\n", (int)ctx->partitionCount);

  for(pp = ctx->partitions;
    pp < ctx->partitions + ctx->partitionCount; pp++)
  {
    printf("  Size: [%.2f, %.2f, %.2f]\n",
      (*pp)->size.x, (*pp)->size.y, (*pp)->size.z);
  }
}

