#ifndef VERBATIM_IMAGE_2D_HPP
#define VERBATIM_IMAGE_2D_HPP

#include "verbatim_gl.hpp"
#include "verbatim_image.hpp"
#include "verbatim_vec2.hpp"

/// Base 2-dimensional image class.
class Image2D : public Image
{
  private:
    /// Width.
    unsigned m_width;

    /// Height.
    unsigned m_height;

  public:
    /// Constructor.
    ///
    /// \param width Image width.
    /// \param height Image height.
    /// \param channels Number of channels.
    explicit Image2D(unsigned width, unsigned height, unsigned channels) :
      Image(width * height, channels),
      m_width(width),
      m_height(height) { }

  private:
#if defined(USE_LD) && defined(DEBUG)
    /// Check that accessed index is valid.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param ch Channel index.
    void checkAccess(unsigned px, unsigned py, unsigned ch) const
    {
      if((px >= m_width) || (py >= m_height) || (ch >= getChannelCount()))
      {
        std::ostringstream sstr;
        sstr << "Image " << m_width << ";" << m_height << ";" << getChannelCount() << ": accessing " << px <<
          ";" << py << ";" << ch;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
    }
#endif
    
    /// Get index for coordinates.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \return Index.
    unsigned getIndex(unsigned px, unsigned py) const
    {
      return ((py * m_width) + px) * getChannelCount();
    }

  protected:
    /// Sample from the image.
    ///
    /// Note that this happens at the origin of texels as opposed to center.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param pc Channel.
    /// \param nearest True to sample nearest as opposed to linear.
    /// \return Sampled color.
    float sample(float px, float py, unsigned pc, bool nearest) const
    {
      float fwidth = static_cast<float>(m_width);
      float fheight = static_cast<float>(m_height);
      float cx = congr(px, 1.0f) * fwidth;
      float cy = congr(py, 1.0f) * fheight;

      unsigned ux = static_cast<unsigned>(cx);
      unsigned uy = static_cast<unsigned>(cy);
      float fract_x = cx - static_cast<float>(ux);
      float fract_y = cy - static_cast<float>(uy);

      // Wrap ux/uy to 0 if they have, due to floating point inaccuracy ended up as width/height.
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

      unsigned x1 = ux;
      unsigned x2 = (ux + 1) % m_width;
      unsigned y1 = uy;
      unsigned y2 = (uy + 1) % m_height;

      if(nearest)
      {
        if(fract_x < 0.5f)
        {
          if(fract_y < 0.5f)
          {
            return getValue(x1, y1, pc);
          }
          return getValue(x1, y2, pc);
        }
        if(fract_y < 0.5f)
        {
          return getValue(x2, y1, pc);
        }
        return getValue(x2, y2, pc);
      }

      return smooth_mix(
          smooth_mix(getValue(x1, y1, pc), getValue(x2, y1, pc), fract_x),
          smooth_mix(getValue(x1, y2, pc), getValue(x2, y2, pc), fract_x),
          fract_y);
    }

  public:
    /// Apply a low-pass filter over the texture.
    ///
    /// This low-pass filter will wrap around the texture edges.
    ///
    /// \param op Kernel size.
    void filterLowpass(int op)
    {
#if defined(USE_LD)
      if(4 < getChannelCount())
      {
        std::ostringstream sstr;
        sstr << "cannot filter texture with " << getChannelCount() << " channels";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      unsigned element_count = getWidth() * getHeight() * getChannelCount();
      float *replacement_data = array_new(static_cast<float*>(NULL), element_count);
      int iwidth = static_cast<int>(getWidth());
      int iheight = static_cast<int>(getHeight());
      int ichannels = static_cast<int>(getChannelCount());

      for(int ii = 0; (ii < iwidth); ++ii)
      {
        for(int jj = 0; (jj < iheight); ++jj)
        {
          float values[] = { 0.0f, 0.0f, 0.0f, 0.0f };
          int idx = (jj * iwidth + ii) * ichannels;
          int divisor = 0;

          for(int kk = -op; (kk <= op); ++kk)
          {
            int rx = ii + kk;

            while(rx < 0)
            {
              rx += iwidth;
            }
            while(rx >= iwidth)
            {
              rx -= iwidth;
            }

            for(int ll = -op; (ll <= op); ++ll)
            {
              int ry = jj + ll;

              while(ry < 0)
              {
                ry += iheight;
              }
              while(ry >= iheight)
              {
                ry -= iheight;
              }

              unsigned ux = static_cast<unsigned>(rx);
              unsigned uy = static_cast<unsigned>(ry);

              for(unsigned mm = 0; (getChannelCount() > mm); ++mm)
              {
                values[mm] += getValue(ux, uy, mm);
              }

              ++divisor;
            }
          }

          for(int mm = 0; (ichannels > mm); ++mm)
          {
            replacement_data[idx + mm] = values[mm] / static_cast<float>(divisor);
          }
        }
      }

      replaceData(replacement_data);
    }

    /// Gets the address for a pixel.
    ///
    /// \param px X position.
    /// \param py Y position.
    /// \param channel Channel to access.
    /// \return Address of the closest pixel to given location.
    float* getClosestPixelAddress(float px, float py, unsigned channel)
    {
      float fwidth = static_cast<float>(m_width);
      float fheight = static_cast<float>(m_height);
      float cx = congr(px, 1.0f) * fwidth;
      float cy = congr(py, 1.0f) * fheight;

      return getValueAddress(static_cast<unsigned>(cx), static_cast<unsigned>(cy), channel);
    }
    /// Gets the address for a pixel wrapper.
    ///
    /// \param pos Position.
    /// \param channel Channel to access.
    /// \return Address of the closest pixel to given location.
    float* getClosestPixelAddress(const vec2& pos, unsigned channel)
    {
      return getClosestPixelAddress(pos.x(), pos.y(), channel);
    }

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

    /// Get value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param ch Channel index.
    /// \return Value.
    float getValue(unsigned px, unsigned py, unsigned ch) const
    {
#if defined(USE_LD) && defined(DEBUG)
      checkAccess(px, py, ch);
#endif
      unsigned idx = getIndex(px, py);
      return Image::getValue(idx + ch);
    }
    /// Get value address.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param ch Channel index.
    /// \return Value address.
    float* getValueAddress(unsigned px, unsigned py, unsigned ch)
    {
#if defined(USE_LD) && defined(DEBUG)
      checkAccess(px, py, ch);
#endif
      unsigned idx = getIndex(px, py);
      return Image::getValueAddress(idx + ch);
    }
    /// Set value.
    ///
    /// \param px X coordinate.
    /// \param py Y coordinate.
    /// \param ch Channel index.
    /// \param val Value.
    void setValue(unsigned px, unsigned py, unsigned ch, float val)
    {
#if defined(USE_LD) && defined(DEBUG)
      checkAccess(px, py, ch);
#endif
      unsigned idx = getIndex(px, py);
      Image::setValue(idx + ch, val);
    }

    /// Sample (in a bilinear fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param pc Channel.
    float sampleLinear(float px, float py, unsigned pc) const
    {
      return sample(px, py, pc, false);
    }
    /// Sample linear wrapper.
    ///
    /// \param pos Sampling coordinates (components: [0, 1[).
    /// \param pc Channel.
    float sampleLinear(const vec2& pos, unsigned pc) const
    {
      return sampleLinear(pos.x(), pos.y(), pc);
    }
    /// Sample (in a nearest fashion) from the image.
    ///
    /// \param px X coordinate [0, 1[.
    /// \param py Y coordinate [0, 1[.
    /// \param pc Channel.
    float sampleNearest(float px, float py, unsigned pc) const
    {
      return sample(px, py, pc, true);
    }
    /// Sample nearest wrapper.
    ///
    /// \param pos Sampling coordinates (components: [0, 1[).
    /// \param pc Channel.
    float sampleNearest(const vec2& pos, unsigned pc) const
    {
      return sampleNearest(pos.x(), pos.y(), pc);
    }
};

#endif
