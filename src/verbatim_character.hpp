#ifndef VERBATIM_CHARACTER_HPP
#define VERBATIM_CHARACTER_HPP

#include "verbatim_image_2d_gray.hpp"
#include "verbatim_texture_2d.hpp"

/// One character.
class Character
{
  private:
    /// Texture for the glyph.
    Texture2D m_texture;

    /// Glyph rectangle offset for this character.
    vec2 m_quad_offset;

    /// Glyph rectangle size for this character.
    vec2 m_quad_size;

    /// Index in font.
    unsigned m_font_index;

    /// Typographic left.
    float m_left;

    /// Typographic top.
    float m_top;

    /// Advance value X.
    float m_advance_x;

  private:
    /// Deleted copy constructor.
    Character(const Character&) = delete;
    /// Deleted assignment.
    Character& operator=(const Character&) = delete;

  public:
    /// Constructor.
    ///
    /// \param idx Index in font.
    /// \param bitmap Glyph bitmap.
    /// \param left Typographic left value.
    /// \param top Typographic top value.
    /// \param advx Advance value (x).
    /// \param advy Advance value (y).
    /// \param base_size Base font size.
    /// \param buf Geometry buffer to insert to.
    Character(unsigned idx, const FT_Bitmap *bitmap, float left, float top, float advx, float base_size) :
      m_font_index(idx),
      m_left(left),
      m_top(top),
      m_advance_x(advx)
    {
      unsigned bw = bitmap->width + 2;
      unsigned bh = bitmap->rows + 2;
      unsigned extra_width = (bw % 4) ? (4 - (bw % 4)) : 0;
      unsigned extra_height = (bh % 4) ? (4 - (bh % 4)) : 0;

      Image2DGray img(bw + extra_width, bh + extra_height);

      for(unsigned ii = 0; (img.getWidth() > ii); ++ii)
      {
        for(unsigned jj = 0; (img.getHeight() > jj); ++jj)
        {
          img.setPixel(ii, jj, 0.0f);
        }
      }

      for(unsigned ii = 0; (bitmap->width > ii); ++ii)
      {
        for(unsigned jj = 0; (bitmap->rows > jj); ++jj)
        {
          const uint8_t *rptr = reinterpret_cast<const uint8_t*>(bitmap->buffer) + (jj * bitmap->width) + ii;
          img.setPixel(ii + 1, img.getHeight() - 2 - jj, static_cast<float>(*rptr) * (1.0f / 255.0f));
        }
      }

      m_texture.update(img);

      {
        float fs = static_cast<float>(base_size);
        float fw = static_cast<float>(img.getWidth()) / fs;
        float fh = static_cast<float>(img.getHeight()) / fs;
        float fl = m_left - (1.0f / fs)/* + static_cast<float>(extra_width) / fs*/;
        float ft = m_top + (1.0f / fs)/* + static_cast<float>(extra_height) / fs*/;

        m_quad_offset = vec2(fl, ft - fh);
        m_quad_size = vec2(fw, fh);
      }
    }

  public:
    /// Accessor.
    ///
    /// \return X advance.
    float getAdvanceX() const
    {
      return m_advance_x;
    }

    /// Accessor.
    ///
    /// \return Index in font.
    unsigned getFontIndex() const
    {
      return m_font_index;
    }

    /// Accessor.
    ///
    /// \return Character rectangle offset.
    const vec2& getQuadOffset() const
    {
      return m_quad_offset;
    }
    /// Accessor.
    ///
    /// \return Character rectangle size.
    const vec2& getQuadSize() const
    {
      return m_quad_size;
    }

    /// Accessor.
    ///
    /// \return Texture.
    const Texture& getTexture() const
    {
      return m_texture;
    }
};

/// Convenience typedef.
typedef uptr<Character> CharacterUptr;

#endif
