#ifndef VERBATIM_IMAGE_CUBE_RGB_HPP
#define VERBATIM_IMAGE_CUBE_RGB_HPP

#include "verbatim_image_2d_rgb.hpp"
#include "verbatim_image_cube.hpp"

/// Cube map image specialization.
typedef ImageCube<Image2DRGB> ImageCubeRGB;

/// Cube map image unique pointer type.
typedef uptr<ImageCubeRGB> ImageCubeRGBUptr;

#endif
