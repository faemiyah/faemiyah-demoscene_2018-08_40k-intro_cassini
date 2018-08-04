#ifndef VERBATIM_TEXTURE_3D_HPP
#define VERBATIM_TEXTURE_3D_HPP

#include "verbatim_image_3d.hpp"
#include "verbatim_texture.hpp"

/// 3D texture.
class Texture3D : public Texture
{
  private:
    /// Texture width.
    unsigned m_width;

    /// Texture height.
    unsigned m_height;

    /// Texture depth.
    unsigned m_depth;

  public:
    /// Constructor.
    explicit Texture3D(GLenum target = GL_TEXTURE_3D) :
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
    void update(unsigned width, unsigned height, unsigned depth, unsigned channels, unsigned bpc, void* data,
        WrapMode wrap, FilteringMode filtering)
    {
      const Texture* prev_texture = updateBegin();
      TextureFormat format(channels, bpc, data);

      m_width = width;
      m_height = height;
      m_depth = depth;

      dnload_glTexImage3D(getType(), 0, format.getInternalFormat(),
          static_cast<GLsizei>(width), static_cast<GLsizei>(height), static_cast<GLsizei>(depth),
          0, format.getFormat(), format.getType(), data);

#if defined(USE_LD)
      unsigned data_size = width * height * depth * format.getTypeSize();
      if(setFiltering(data, filtering))
      {
        data_size = (data_size * 8) / 7;
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
    /// Accessor.
    ///
    /// \return Texture height.
    unsigned getDepth() const
    {
      return m_height;
    }

    /// Update texture contents with an image.
    ///
    /// \param image Image to update with.
    /// \param bpc Bytes per component to convert the image to (default: 1).
    /// \param filtering Filtering mode (default: trilinear).
    /// \param wrap Wrap mode (defaut: wrap).
    void update(Image3D &image, unsigned bpc = 1, FilteringMode filtering = TRILINEAR, WrapMode wrap = WRAP)
    {
      uarr<uint8_t> export_data = image.getExportData(bpc);
      update(image.getWidth(), image.getHeight(), image.getDepth(), image.getChannelCount(), bpc,
          export_data.get(), wrap, filtering);
    }
};

#endif
