#ifndef VERBATIM_IMAGE_3D_HPP
#define VERBATIM_IMAGE_3D_HPP

#include "verbatim_gl.hpp"
#include "verbatim_image.hpp"
#include "verbatim_vec3.hpp"

/// Base 3-dimensional image class.
class Image3D : public Image
{
  private:
    /// Width.
    unsigned m_width;

    /// Height.
    unsigned m_height;

    /// Depth.
    unsigned m_depth;

  public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    /// \param depth Image depth.
    /// \param channels Number of channels.
    explicit Image3D(unsigned width, unsigned height, unsigned depth, unsigned channels) :
      Image(width * height * depth, channels),
      m_width(width),
      m_height(height),
      m_depth(depth)
    {
    }

  private:
#if defined(USE_LD) && defined(DEBUG)
    /// Check that accessed index is valid.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param pz Z coordinate.
    /// \param ch Channel index.
    void checkAccess(unsigned px, unsigned py, unsigned pz, unsigned ch) const
    {
      if((px >= m_width) || (py >= m_height) || (pz >= m_depth) || (ch >= getChannelCount()))
      {
        std::ostringstream sstr;
        sstr << "Image " << m_width << ";" << m_height << ";" << m_depth << ";" << getChannelCount() <<
          ": accessing " << px << ";" << py << ";" << pz << ";" << ch;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
    }
#endif

    /// Get index for coordinates.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param pz Z coordinate.
    /// \return Index.
    unsigned getIndex(unsigned px, unsigned py, unsigned pz) const
    {
      return ((pz * m_width * m_height) + (py * m_width) + px) * getChannelCount();
    }

  protected:
    /// Sample from the image.
    ///
    /// Note that this happens at the origin of texels as opposed to center.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param pz Z coordinate [0, 1[.
    /// \param pc Channel.
    /// \param nearest True to sample nearest as opposed to linear.
    /// \return Sampled color.
    float sample(float px, float py, float pz, unsigned pc, bool nearest) const
    {
      float fwidth = static_cast<float>(m_width);
      float fheight = static_cast<float>(m_height);
      float fdepth = static_cast<float>(m_depth);
      float cx = congr(px, 1.0f) * fwidth;
      float cy = congr(py, 1.0f) * fheight;
      float cz = congr(pz, 1.0f) * fdepth;

      unsigned ux = static_cast<unsigned>(cx);
      unsigned uy = static_cast<unsigned>(cy);
      unsigned uz = static_cast<unsigned>(cz);
      float fract_x = cx - static_cast<float>(ux);
      float fract_y = cy - static_cast<float>(uy);
      float fract_z = cz - static_cast<float>(uz);

      // Wrap ux/uy/uz to 0 if they have, due to floating point inaccuracy ended up as width/height.
      if(ux >= m_width)
      {
        ux = 0;
        fract_x = 0.0f;
      }
      if(uy >= m_height)
      {
        uy = 0;
        fract_y = 0.0f;
      }
      if(uz >= m_depth)
      {
        uz = 0;
        fract_z = 0.0f;
      }

      unsigned x1 = ux;
      unsigned x2 = (ux + 1) % m_width;
      unsigned y1 = uy;
      unsigned y2 = (uy + 1) % m_height;
      unsigned z1 = uz;
      unsigned z2 = (uz + 1) % m_depth;

      if(nearest)
      {
        if(fract_x < 0.5f)
        {
          if(fract_y < 0.5f)
          {
            if(fract_z < 0.5f)
            {
              return getValue(x1, y1, z1, pc);
            }
            return getValue(x1, y1, z2, pc);
          }
          if(fract_z < 0.5f)
          {
            return getValue(x1, y2, z1, pc);
          }
          return getValue(x1, y2, z2, pc);
        }
        if(fract_y < 0.5f)
        {
          if(fract_z < 0.5f)
          {
            return getValue(x2, y1, z1, pc);
          }
          return getValue(x2, y1, z2, pc);
        }
        if(fract_z < 0.5f)
        {
          return getValue(x2, y2, z1, pc);
        }
        return getValue(x2, y2, z2, pc);
      }

      float zz1 =
        smooth_mix(
            smooth_mix(getValue(x1, y1, z1, pc), getValue(x2, y1, z1, pc), fract_x),
            smooth_mix(getValue(x1, y2, z1, pc), getValue(x2, y2, z1, pc), fract_x),
            fract_y);
      float zz2 =
        smooth_mix(
            smooth_mix(getValue(x1, y1, z2, pc), getValue(x2, y1, z2, pc), fract_x),
            smooth_mix(getValue(x1, y2, z2, pc), getValue(x2, y2, z2, pc), fract_x),
            fract_y);
      return smooth_mix(zz1, zz2, fract_z);
    }

  public:
    /// Accessor.
    ///
    /// \return Width.
    unsigned getWidth() const
    {
      return m_width;
    }
    /// Accessor.
    ///
    /// \return Height.
    unsigned getHeight() const
    {
      return m_height;
    }
    /// Accessor.
    ///
    /// \return Depth.
    unsigned getDepth() const
    {
      return m_depth;
    }

    /// Get value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param pz Z coordinate.
    /// \param ch Channel index.
    /// \return Value.
    float getValue(unsigned px, unsigned py, unsigned pz, unsigned ch) const
    {
#if defined(USE_LD) && defined(DEBUG)
      checkAccess(px, py, pz, ch);
#endif
      unsigned idx = getIndex(px, py, pz);
      return Image::getValue(idx + ch);
    }
    /// Set value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param ch Channel index.
    /// \param val Value.
    void setValue(unsigned px, unsigned py, unsigned pz, unsigned ch, float val)
    {
#if defined(USE_LD) && defined(DEBUG)
      checkAccess(px, py, pz, ch);
#endif
      unsigned idx = getIndex(px, py, pz);
      Image::setValue(idx + ch, val);
    }

    /// Sample (in a bilinear fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param py z coordinate [0, 1[.
    /// \param pc Channel.
    float sampleLinear(float px, float py, float pz, unsigned pc) const
    {
      return sample(px, py, pz, pc, false);
    }
    /// Sample linear wrapper.
    ///
    /// \param pos Sampling coordinates (components: [0, 1[).
    /// \param pc Channel.
    float sampleLinear(const vec3& pos, unsigned pc) const
    {
      return sampleLinear(pos.x(), pos.y(), pos.z(), pc);
    }
    /// Sample (in a nearest fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param pc Channel.
    float sampleNearest(float px, float py, float pz, unsigned pc) const
    {
      return sample(px, py, pz, pc, true);
    }
    /// Sample nearest wrapper.
    ///
    /// \param pos Sampling coordinates (components: [0, 1[).
    /// \param pc Channel.
    float sampleNearest(const vec3& pos, unsigned pc) const
    {
      return sampleNearest(pos.x(), pos.y(), pos.z(), pc);
    }
};

#endif
