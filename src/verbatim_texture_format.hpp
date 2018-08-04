#ifndef VERBATIM_TEXTURE_FORMAT_HPP
#define VERBATIM_TEXTURE_FORMAT_HPP

/// \cond
/// Defines the number of bytes used for RGB24 texture format.
/// If not defined, default assumes normal R8G8B8 format.
//#define VGL_RGB24_BYTES 3
/// \endcond

/// Texture format information.
class TextureFormat
{
  private:
    /// OpenGL texture format.
    GLenum m_format;

    /// OpenGL texture internal format.
    GLint m_internal_format;

    /// OpenGL type.
    GLenum m_type;

#if defined(USE_LD)
    /// Size of one texel in bytes.
    unsigned m_type_size;
#endif

  public:
    /// Constructor.
    ///
    /// \param channels Number of channels, 0 for depth texture.
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    explicit TextureFormat(unsigned channels, unsigned bpc, void* data) :
      m_format(determine_format(channels)),
      m_internal_format(determine_internal_format(channels, bpc, data)),
      m_type(determine_type(channels, bpc, data))
#if defined(USE_LD)
      , m_type_size(bpc * channels)
#endif
    {
#if defined(USE_LD)
      if(m_type == GL_UNSIGNED_SHORT_5_6_5)
      {
        m_type_size = 2;
      }
#endif
    }

  private:
    /// Get GL texture format for channel number.
    ///
    /// \param channels Number of channels, 0 for depth texture.
    /// \return Texture format.
    GLenum determine_format(unsigned channels)
    {
      if(channels == 0)
      {
        return GL_DEPTH_COMPONENT;
      }
      if(channels == 1)
      {
#if defined(DNLOAD_GLESV2)
        return GL_LUMINANCE;
#else
        return GL_RED;
#endif
      }
      if(channels == 2)
      {
#if defined(DNLOAD_GLESV2)
        return GL_LUMINANCE_ALPHA;
#else
        return GL_RG;
#endif
      }
      if(channels == 3)
      {
        return GL_RGB;
      }
#if defined(USE_LD)
      if(channels != 4)
      {
        std::ostringstream sstr;
        sstr << "invalid channel count: " << channels;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return GL_RGBA;
    }

    /// Get GL texture format for channel number.
    ///
    /// \param channels Number of channels, 0 for depth texture.
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    GLint determine_internal_format(unsigned channels, unsigned bpc, void* data)
    {
      if(channels == 0)
      {
        return GL_DEPTH_COMPONENT;
      }
      if(channels == 1)
      {
        return determine_internal_format_r(bpc, data);
      }
      if(channels == 2)
      {
        return determine_internal_format_rg(bpc, data);
      }
      if(channels == 3)
      {
        return determine_internal_format_rgb(bpc, data);
      }
#if defined(USE_LD)
      if(channels != 4)
      {
        std::ostringstream sstr;
        sstr << "invalid channel count: " << channels;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return determine_internal_format_rgba(bpc, data);
    }

    /// Determine R internal format.
    ///
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    GLint determine_internal_format_r(unsigned bpc, void* data)
    {
      if(bpc == 4)
      {
        return GL_R32F;
      }
      else if(bpc == 2)
      {
        return data ? GL_R16 : GL_R16F;
      }
#if defined(USE_LD)
      if(bpc != 1)
      {
        std::ostringstream sstr;
        sstr << "invalid bytes per channel count for R format: " << bpc;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
#if defined(DNLOAD_GLESV2)
      return GL_LUMINANCE;
#else
      return GL_RED;
#endif
    }

    /// Determine RG internal format.
    ///
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    GLint determine_internal_format_rg(unsigned bpc, void* data)
    {
      if(bpc == 4)
      {
        return GL_RG32F;
      }
      else if(bpc == 2)
      {
        return data ? GL_RG16 : GL_RG16F;
      }
#if defined(USE_LD)
      if(bpc != 1)
      {
        std::ostringstream sstr;
        sstr << "invalid bytes per channel count for RG format: " << bpc;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
#if defined(DNLOAD_GLESV2)
      return GL_LUMINANCE_ALPHA;
#else
      return GL_RG;
#endif
    }

    /// Determine RGB internal format.
    ///
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    GLint determine_internal_format_rgb(unsigned bpc, void* data)
    {
      if(bpc == 4)
      {
        return GL_RGB32F;
      }
      else if(bpc == 2)
      {
        return data ? GL_RGB16 : GL_RGB16F;
      }
#if defined(USE_LD)
      if(bpc >= 3)
      {
        std::ostringstream sstr;
        sstr << "invalid bytes per channel count for RGB format: " << bpc;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return GL_RGB;
    }

    /// Determine RGBA internal format.
    ///
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture internal format.
    GLint determine_internal_format_rgba(unsigned bpc, void* data)
    {
      if(bpc == 4)
      {
        return GL_RGBA32F;
      }
      else if(bpc == 2)
      {
        return data ? GL_RGBA16 : GL_RGBA16F;
      }
#if defined(USE_LD)
      if(bpc != 1)
      {
        std::ostringstream sstr;
        sstr << "invalid bytes per channel count for RGBA format: " << bpc;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return GL_RGBA;
    }

    /// Get GL data type for bits per pixel.
    ///
    /// \param channels Number of channels, 0 for depth texture.
    /// \param bpc Bytes per component.
    /// \param data Pointer to texture data.
    /// \return Texture data type.
    GLenum determine_type(unsigned channels, unsigned bpc, void* data)
    {
      if(bpc == 4)
      {
        return GL_FLOAT;
      }
      if(bpc == 2)
      {
        return data ? GL_UNSIGNED_SHORT : GL_FLOAT;
      }
      if(bpc == 1)
      {
        return GL_UNSIGNED_BYTE;
      }
#if defined(USE_LD)
      if((bpc != 0) || (channels != 3))
      {
        std::ostringstream sstr;
        sstr << "invalid channel and bpc combination to determine type: " << channels << " / " << bpc;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }      
#endif
      return GL_UNSIGNED_SHORT_5_6_5;
    }

  public:
    /// Accessor.
    ///
    /// \return OpenGL format.
    GLenum getFormat() const
    {
      return m_format;
    }

    /// Accessor.
    ///
    /// \return OpenGL internal format.
    GLint getInternalFormat() const
    {
      return m_internal_format;
    }

    /// Accessor.
    ///
    /// \return OpenGL type.
    GLenum getType() const
    {
      return m_type;
    }

#if defined(USE_LD)
    /// Accessor.
    ///
    /// \return Data size multiplier.
    unsigned getTypeSize() const
    {
      return m_type_size;
    }
#endif
};

#endif
