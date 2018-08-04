#ifndef VERBATIM_IMAGE_2D_RGBA_HPP
#define VERBATIM_IMAGE_2D_RGBA_HPP

#include "verbatim_image_2d.hpp"
#include "verbatim_uptr.hpp"

/// 2-dimensional RGBA image.
class Image2DRGBA : public Image2D
{
  public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    explicit Image2DRGBA(unsigned width, unsigned height) :
      Image2D(width, height, 4) { }

  public:
    /// Set pixel value wrapper.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param pr Red component.
    /// \param pg Green component.
    /// \param pb Blue component.
    /// \param pa Alpha component.
    void setPixel(unsigned px, unsigned py, float pr, float pg, float pb, float pa)
    {
      setValue(px, py, 0, pr);
      setValue(px, py, 1, pg);
      setValue(px, py, 2, pb);
      setValue(px, py, 3, pa);
    }

    /// Set pixel value wrapper.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param col Color.
    /// \param pa Alpha component.
    void setPixel(unsigned px, unsigned py, const vec3 &col, float pa)
    {
      setPixel(px, py, col[0], col[1], col[2], pa);
    }
};

/// Convenience typedef.
typedef uptr<Image2DRGBA> Image2DRGBAUptr;

#endif
