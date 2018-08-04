#ifndef VERBATIM_IMAGE_2D_RGB_HPP
#define VERBATIM_IMAGE_2D_RGB_HPP

#include "verbatim_image_2d.hpp"
#include "verbatim_uptr.hpp"
#include "verbatim_vec3.hpp"

/// 2-dimensional RGB image.
class Image2DRGB : public Image2D
{
  public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    explicit Image2DRGB(unsigned width, unsigned height) :
      Image2D(width, height, 3)
    {
    }

  public:
    /// Set pixel value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param pr Red component.
    /// \param pg Green component.
    /// \param pb Blue component.
    void setPixel(unsigned px, unsigned py, float pr, float pg, float pb)
    {
      setValue(px, py, 0, pr);
      setValue(px, py, 1, pg);
      setValue(px, py, 2, pb);
    }
    /// Set pixel value wrapper.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param col 3-component vector as color.
    void setPixel(unsigned px, unsigned py, const vec3 &col)
    {
      setPixel(px, py, col[0], col[1], col[2]);
    }
};

/// Convenience typedef.
typedef uptr<Image2DRGB> Image2DRGBUptr;

#endif
