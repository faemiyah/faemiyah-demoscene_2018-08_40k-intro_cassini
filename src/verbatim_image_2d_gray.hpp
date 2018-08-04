#ifndef VERBATIM_IMAGE_2D_GRAY_HPP
#define VERBATIM_IMAGE_2D_GRAY_HPP

#include "verbatim_image_2d.hpp"
#include "verbatim_uptr.hpp"

/// 2-dimensional grayscale image.
class Image2DGray : public Image2D
{
  public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    explicit Image2DGray(unsigned width, unsigned height) :
      Image2D(width, height, 1) { }

  public:
    /// Sample (in a bilinear fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \return Sampled value.
    float sampleLinear(float px, float py) const
    {
      return Image2D::sampleLinear(px, py, 0);
    }
    /// sampleLinear wrapper.
    ///
    /// \param pos Position in image.
    /// \return Sampled value.
    float sampleLinear(const vec2& pos) const
    {
      return sampleLinear(pos.x(), pos.y());
    }
    /// Sample (in a nearest fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \return Sampled value.
    float sampleNearest(float px, float py) const
    {
      return Image2D::sampleNearest(px, py, 0);
    }
    /// sampleNearest wrapper.
    ///
    /// \param pos Position in image.
    /// \return Sampled value.
    float sampleNearest(const vec2& pos) const
    {
      return sampleNearest(pos.x(), pos.y());
    }

    /// Get pixel value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \return Pixel value at given coordinates.
    float getPixel(unsigned px, unsigned py) const
    {
      return getValue(px, py, 0);
    }
    /// Set pixel value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param op Gray level.
    void setPixel(unsigned px, unsigned py, float op)
    {
      setValue(px, py, 0, op);
    }
};

/// Convenience typedef.
typedef uptr<Image2DGray> Image2DGrayUptr;

#endif
