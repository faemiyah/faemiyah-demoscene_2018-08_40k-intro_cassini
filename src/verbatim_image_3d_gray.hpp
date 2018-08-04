#ifndef VERBATIM_IMAGE_3D_GRAY_HPP
#define VERBATIM_IMAGE_3D_GRAY_HPP

#include "verbatim_image_3d.hpp"
#include "verbatim_uptr.hpp"
#include "verbatim_vec3.hpp"

/// 3-dimensional grayscale image.
class Image3DGray : public Image3D
{
  public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    /// \param depth Image depth.
    explicit Image3DGray(unsigned width, unsigned height, unsigned depth) :
      Image3D(width, height, depth, 1)
    {
    }

  public:
    /// Sample (in a bilinear fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param pz Z coordinate [0, 1[.
    /// \return Sampled value.
    float sampleLinear(float px, float py, float pz) const
    {
      return Image3D::sampleLinear(px, py, pz, 0);
    }
    /// sampleLinear wrapper.
    ///
    /// \param pos Position in image.
    /// \return Sampled value.
    float sampleLinear(const vec3& pos) const
    {
      return sampleLinear(pos.x(), pos.y(), pos.z());
    }
    /// Sample (in a nearest fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param pz Z coordinate [0, 1[.
    /// \return Sampled value.
    float sampleNearest(float px, float py, float pz) const
    {
      return Image3D::sampleNearest(px, py, pz, 0);
    }
    /// sampleNearest wrapper.
    ///
    /// \param pos Position in image.
    /// \return Sampled value.
    float sampleNearest(const vec3& pos) const
    {
      return sampleNearest(pos.x(), pos.y(), pos.z());
    }

    /// Get pixel value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param pz Z coordinate.
    /// \return Pixel value at given coordinates.
    float getPixel(unsigned px, unsigned py, unsigned pz) const
    {
      return getValue(px, py, pz, 0);
    }
    /// Set pixel value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param pz Z coordinate.
    /// \param op Gray level.
    void setPixel(unsigned px, unsigned py, unsigned pz, float op)
    {
      setValue(px, py, pz, 0, op);
    }
};

/// Convenience typedef.
typedef uptr<Image3DGray> Image3DGrayUptr;

#endif
