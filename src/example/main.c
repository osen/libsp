#include "sp.h"

#include <stdlib.h>

int main()
{
  SpContext *context = NULL;

  context = SpContextCreate();

  SpContextAddTriangle(
    context,
    0.0f, 0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f);

  SpContextAddTriangle(
    context,
    0.0f, 10.0f, 0.0f,
    -10.0f, -10.0f, 0.0f,
    10.0f, -10.0f, 0.0f);

  SpContextDump(context);

  SpContextDestroy(context);

  return 0;
}
