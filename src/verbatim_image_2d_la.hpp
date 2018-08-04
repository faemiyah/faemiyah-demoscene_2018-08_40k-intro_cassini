#ifndef VERBATIM_IMAGE_2D_LA_HPP
#define VERBATIM_IMAGE_2D_LA_HPP

#include "verbatim_image_2d.hpp"
#include "verbatim_uptr.hpp"

/// 2-dimensional luminance-alpha image.
class Image2DLA : public Image2D
{
  public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    explicit Image2DLA(unsigned width, unsigned height) :
      Image2D(width, height, 2) { }

  public:
    /// Set pixel value wrapper.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param luminance Luminance component.
    /// \param alpha Alpha component.
    void setPixel(unsigned px, unsigned py, float luminance, float alpha)
    {
      setValue(px, py, 0, luminance);
      setValue(px, py, 1, alpha);
    }
};

/// Convenience typedef.
typedef uptr<Image2DLA> Image2DLAUptr;

#endif
