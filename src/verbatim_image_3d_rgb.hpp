#ifndef VERBATIM_IMAGE_3D_RGB_HPP
#define VERBATIM_IMAGE_3D_RGB_HPP

#include "verbatim_image_3d.hpp"
#include "verbatim_uptr.hpp"
#include "verbatim_vec3.hpp"

/// 3-dimensional RGB image.
class Image3DRGB : public Image3D
{
  public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    /// \param depth Image depth.
    explicit Image3DRGB(unsigned width, unsigned height, unsigned depth) :
      Image3D(width, height, depth, 3)
    {
    }

  public:
    /// Set pixel value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param pz Y coordinate.
    /// \param pr Red component.
    /// \param pg Green component.
    /// \param pb Blue component.
    void setPixel(unsigned px, unsigned py, unsigned pz, float pr, float pg, float pb)
    {
      setValue(px, py, pz, 0, pr);
      setValue(px, py, pz, 1, pg);
      setValue(px, py, pz, 2, pb);
    }
    /// Set pixel value wrapper.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param col 3-component vector as color.
    void setPixel(unsigned px, unsigned py, unsigned pz, const vec3 &col)
    {
      setPixel(px, py, pz, col[0], col[1], col[2]);
    }
};

/// Convenience typedef.
typedef uptr<Image3DRGB> Image3DRGBUptr;

#endif
