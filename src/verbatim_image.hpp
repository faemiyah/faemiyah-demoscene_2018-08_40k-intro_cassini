#ifndef VERBATIM_IMAGE_HPP
#define VERBATIM_IMAGE_HPP

#include "verbatim_gl.hpp"
#include "verbatim_uarr.hpp"

/// Base image class.
class Image
{
  private:
    /// Image data.
    uarr<float> m_data;

    /// Texel count.
    unsigned m_texel_count;

    /// Number of channels.
    unsigned m_channel_count;

  private:
    /// Deleted copy constructor.
    Image(const Image&) = delete;
    /// Deleted assignment.
    Image& operator=(const Image&) = delete;

  public:
    /// Constructor.
    ///
    /// \param texel_count Number of texels.
    /// \param channel_count Number of channels.
    explicit Image(unsigned texel_count, unsigned channel_count) :
      m_data(texel_count * channel_count),
      m_texel_count(texel_count),
      m_channel_count(channel_count)
    {
    }

  protected:
    /// Accessor.
    ///
    /// \param idx Index.
    /// \return value.
    float getValue(unsigned idx) const
    {
      return m_data[idx];
    }
    /// Gets address for value at index.
    ///
    /// \return Address for value.
    float* getValueAddress(unsigned idx)
    {
      return &(m_data[idx]);
    }
    /// Setter.
    ///
    /// \param idx Index.
    /// \param val Value.
    void setValue(unsigned idx, float value)
    {
      m_data[idx] = value;
    }

    /// Replace data.
    ///
    /// \param op New data.
    void replaceData(float *op)
    {
      m_data.reset(op);
    }

  public:
    /// Clears a channel to a value.
    ///
    /// \param channel Channel to clear.
    /// \param value Value to clear to (default: 0.0f).
    void clear(unsigned channel, float value = 0.0f)
    {
#if defined(USE_LD)
      if(channel > m_channel_count)
      {
        std::ostringstream sstr;
        sstr << "trying to clear channel " << channel << " in " << (m_channel_count + 1) << "-channel image";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      for(unsigned ii = channel, ee = getElementCount(); (ii < ee); ii += m_channel_count)
      {
        m_data[ii] = value;
      }
    }

    /// Recreates the export data array as UNORM data.
    ///
    /// \param bpc Bytes per component to convert to (default: 1).
    /// \return Pointer to raw image data.
    uarr<uint8_t> getExportData(unsigned bpc = 1)
    {
      unsigned element_count = getElementCount();

      // Floats do not need to be converted.
      if(bpc == 4)
      {
        uarr<uint8_t> ret(element_count * 4);
        float* export_data = reinterpret_cast<float*>(ret.get());

        for(unsigned ii = 0; (ii < element_count); ++ii)
        {
          export_data[ii] = m_data[ii];
        }

        return ret;
      }

      if(bpc == 2)
      {
        uarr<uint8_t> ret(element_count * 2);
        uint16_t* export_data = reinterpret_cast<uint16_t*>(ret.get());

        for(unsigned ii = 0; (ii < element_count); ++ii)
        {
          export_data[ii] = static_cast<uint16_t>(0.5f + clamp(m_data[ii], 0.0f, 1.0f) * 65535.0f);
        }

        return ret;
      }

#if defined(USE_LD)
      if(bpc != 1)
      {
        std::ostringstream sstr;
        sstr << "invalid bpc value for UNORM conversion: " << bpc;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif

      uarr<uint8_t> ret(element_count);

      for(unsigned ii = 0; (ii < element_count); ++ii)
      {
        ret[ii] = static_cast<uint8_t>(0.5f + clamp(m_data[ii], 0.0f, 1.0f) * 255.0f);
      }

      return ret;
    }

    /// Accessor.
    ///
    /// \return Texel count.
    unsigned getTexelCount() const
    {
      return m_texel_count;
    }

    /// Accessor.
    ///
    /// \return Number of channels.
    unsigned getChannelCount() const
    {
      return m_channel_count;
    }

    /// Gets the number of elements in the image.
    ///
    /// Element count is the number of texels times the number of channels.
    ///
    /// \return Number of elements.
    unsigned getElementCount() const
    {
      return m_texel_count * m_channel_count;
    }

    /// Fill image with noise.
    ///
    /// \param nfloor Noise floor.
    /// \param nceil Noise ceiling.
    void noise(float nfloor = 0.0f, float nceil = 1.0f)
    {
      for(unsigned ii = 0, ee = getElementCount(); (ii < ee); ++ii)
      {
        m_data[ii] = frand(nfloor, nceil);
      }
    }

    /// Normalize color level.
    ///
    /// \param channel Channel
    /// \param ambient Ambient level (default: 0.0f).
    void normalize(unsigned channel, float ambient = 0.0f)
    {
      unsigned element_count = m_texel_count * m_channel_count;
      float min_value = FLT_MAX;
      float max_value = -FLT_MAX;

      for(unsigned ii = channel; (element_count > ii); ii += m_channel_count)
      {
        float val = m_data[ii];

        min_value = std::min(val, min_value);
        max_value = std::max(val, max_value);
      }

      // If all values are identical, skip normalization.
      if(max_value != min_value)
      {
        float mul = (1.0f - ambient) / (max_value - min_value);

        //std::cout << "max: " << max_value << " ; min: " << min_value << " ; mul: " << mul << std::endl;

        for(unsigned ii = channel; (element_count > ii); ii += m_channel_count)
        {
          float val = m_data[ii];

          m_data[ii] = (mul * (val - min_value)) + ambient;
        }
      }
    }
};

#endif
