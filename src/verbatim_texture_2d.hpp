#ifndef VERBATIM_TEXTURE_2D_HPP
#define VERBATIM_TEXTURE_2D_HPP

#include "verbatim_image_2d.hpp"
#include "verbatim_texture.hpp"
#include "verbatim_texture_format.hpp"

/// 2-dimensional texture.
class Texture2D : public Texture
{
  private:
    /// Texture width.
    unsigned m_width;

    /// Texture height.
    unsigned m_height;

  public:
    /// Constructor.
    explicit Texture2D(GLenum target = GL_TEXTURE_2D) :
      Texture(target)
    {
    }

  private:
    /// Explicit update operation.
    ///
    /// \param width Width of the texture.
    /// \param height Height of the texture.
    /// \param channels Number of channels, 0 for depth texture.
    /// \param bpc Bytes per component in texture data.
    /// \param data Pointter to texture data, must be one byte per color channel per texel.
    /// \param wrap Wrap mode to use.
    /// \param filtering Filtering mode to use.
    void update(unsigned width, unsigned height, unsigned channels, unsigned bpc, void* data, WrapMode wrap,
        FilteringMode filtering)
    {
      const Texture* prev_texture = updateBegin();
      TextureFormat format(channels, bpc, data);

      m_width = width;
      m_height = height;

      dnload_glTexImage2D(getType(), 0, format.getInternalFormat(),
          static_cast<GLsizei>(width), static_cast<GLsizei>(height),
          0, format.getFormat(), format.getType(), data);

#if defined(USE_LD)
      unsigned data_size = width * height * format.getTypeSize();
      if(setFiltering(data, filtering))
      {
        data_size = (data_size * 4) / 3;
      }
#else
      setFiltering(data, filtering);
#endif
      setWrapMode(wrap);

#if defined(USE_LD)
      vgl::increment_data_size_texture(data_size);
#endif

      updateEnd(prev_texture);
    }

  public:
    /// Accessor.
    ///
    /// \return Texture width.
    unsigned getWidth() const
    {
      return m_width;
    }

    /// Accessor.
    ///
    /// \return Texture height.
    unsigned getHeight() const
    {
      return m_height;
    }

    /// Update texture contents with an image.
    ///
    /// Creates a 16-bit texture.
    ///
    /// \param image Image to update with.
    /// \param bpc Bytes per component to convert the image to (default: 1).
    /// \param filtering Filtering mode (default: trilinear).
    /// \param wrap Wrap mode (defaut: wrap).
    void update(Image2D &image, unsigned bpc = 1, FilteringMode filtering = TRILINEAR, WrapMode wrap = WRAP)
    {
      uarr<uint8_t> export_data = image.getExportData(bpc);
      update(image.getWidth(), image.getHeight(), image.getChannelCount(), bpc, export_data.get(), wrap,
          filtering);
    }

    /// Update contents with nothing.
    ///
    /// Usable for framebuffer textures. By default, 4 channels since RGB framebuffer is an extension.
    ///
    /// \param width Texture width.
    /// \param height Texture height.
    /// \param channels Number of channels, use 0 for depth texture.
    /// \param bpc Bytes per component for fragments (default: 2).
    /// \param filtering Filtering mode.
    void update(unsigned width, unsigned height, unsigned channels = 4, unsigned bpc = 2,
        FilteringMode filtering = BILINEAR, WrapMode wrap = CLAMP)
    {
      update(width, height, channels, bpc, NULL, wrap, filtering);
    }
};

#endif
