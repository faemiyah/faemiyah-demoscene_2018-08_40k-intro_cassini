#ifndef VERBATIM_IMAGE_CUBE_RGBA_HPP
#define VERBATIM_IMAGE_CUBE_RGBA_HPP

#include "verbatim_image_2d_rgba.hpp"
#include "verbatim_image_cube.hpp"

/// Cube map image specialization.
typedef ImageCube<Image2DRGBA> ImageCubeRGBA;

/// Cube map image unique pointer type.
typedef uptr<ImageCubeRGBA> ImageCubeRGBAUptr;

#endif
