#ifndef FLUIDSIMULATION
#define FLUIDSIMULATION

#include "dnload.h"
#include "verbatim_gl.hpp"
#include "verbatim_image_2d_rgba.hpp"
#include "verbatim_uarr.hpp"
#include "verbatim_vec2.hpp"
#include "verbatim_vec3.hpp"

void AddInflow(Image2DRGBA buf, int x, int y, int w, int h, vec2 vel)
{
  for (unsigned ii = x; ii < x + w; ii++)
  {
    for (unsigned jj = y; jj < y + h; jj++)
    {
      buf.setPixel(ii, jj, vel.x, vel.y, 0.0f, 1.0f);
    }
  }
}

#endif
