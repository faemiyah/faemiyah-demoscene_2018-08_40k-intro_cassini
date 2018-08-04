#ifndef VERBATIM_IMAGE_CUBE_GRAY_HPP
#define VERBATIM_IMAGE_CUBE_GRAY_HPP

#include "verbatim_image_2d_gray.hpp"
#include "verbatim_image_cube.hpp"

/// Cube map image specialization.
typedef ImageCube<Image2DGray> ImageCubeGray;

/// Cube map image unique pointer type.
typedef uptr<ImageCubeGray> ImageCubeGrayUptr;

#endif
