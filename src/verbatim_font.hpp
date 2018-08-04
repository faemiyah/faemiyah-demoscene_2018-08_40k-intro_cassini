#ifndef VERBATIM_FONT_HPP
#define VERBATIM_FONT_HPP

#include "verbatim_character.hpp"

//#define FONT_ENABLE_KERNING

/// Font.
class Font
{
  private:
    /// FreeType library reference.
    static FT_Library g_freetype_library;

  private:
    /// Let's stay ASCII7 for now.
    static const unsigned MAX_CHARACTERS = 128;

  private:
    /// Font structure.
    FT_Face m_face;

    /// Characters loaded.
    CharacterUptr m_characters[MAX_CHARACTERS];

    /// 'M' size for the face (pixels).
    unsigned m_font_size;

  private:
    /// Deleted copy constructor.
    Font(const Font&) = delete;
    /// Deleted assignment.
    Font& operator=(const Font&) = delete;

  public:
    /// Constructor.
    ///
    /// \param fs Font size.
    /// \param fnames Filename array.
    Font(unsigned fs, const char **fnames) :
      m_face(NULL),
      m_font_size(fs)
    {
      freetype_init();

      for(const char **iter = fnames; true; ++iter)
      {
#if defined(USE_LD)
        if(!(*iter))
        {
          std::ostringstream sstr;
          sstr << "no suitable font found, tried:";
          for(const char **ii = fnames; *ii; ++ii)
          {
            sstr << ((ii != fnames) ? " '" : ", '") << (*ii) << "'";
          }
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif
        if(load(*iter))
        {
          break;
        }
      }

      FT_Error err = dnload_FT_Set_Pixel_Sizes(m_face, 0, m_font_size);
#if defined(USE_LD)
      if(err)
      {
        std::ostringstream sstr;
        sstr << "could not set font size to " << m_font_size << " on " << m_face;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#else
      (void)err;
#endif
    }

    /// Destructor.
    ~Font()
    {
#if defined(USE_LD)
      if(m_face)
      {
        FT_Done_Face(m_face);
      }
#endif
    }

  private:
    /// Attempt to load a font.
    ///
    /// \param fname Filename.
    /// \return True on success, false on failure.
    bool load(const char *fname)
    {
      FT_Error err = dnload_FT_New_Face(g_freetype_library, fname, 0, &m_face);
      return (0 == err);
    }

  public:
    /// Create one character (for later use).
    ///
    /// \param unicode Unicode character id.
    void createCharacter(unsigned unicode)
    {
#if defined(USE_LD)
      if(MAX_CHARACTERS <= unicode)
      {
        std::ostringstream sstr;
        sstr << "attempting to create character " << unicode << " with too high unicode index";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif

      unsigned idx = dnload_FT_Get_Char_Index(m_face, unicode);
#if defined(USE_LD)
      if(0 >= idx)
      {
        std::ostringstream sstr;
        sstr << "no character " << unicode << " found in font";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif

      FT_Error err = dnload_FT_Load_Glyph(m_face, idx, FT_LOAD_DEFAULT);
#if defined(USE_LD)
      if(err)
      {
        std::ostringstream sstr;
        sstr << "error loading character " << unicode << " at index " << idx << ": " << err;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#else
      (void)err;
#endif

      FT_GlyphSlot glyph = m_face->glyph;
      if(glyph->format != FT_GLYPH_FORMAT_BITMAP)
      {
        err = dnload_FT_Render_Glyph(glyph, FT_RENDER_MODE_NORMAL);
#if defined(USE_LD)
        if(err)
        {
          std::ostringstream sstr;
          sstr << "error rendering glyph " << unicode << " at index " << idx << ": " << err;
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif
      }

      float fsize = static_cast<float>(m_font_size);

      // No Y advance - only left-right text supported.
      m_characters[unicode] = new Character(idx,
          &(glyph->bitmap),
          static_cast<float>(glyph->bitmap_left) / fsize,
          static_cast<float>(glyph->bitmap_top) / fsize,
          (static_cast<float>(glyph->advance.x) / fsize) * (1.0f / 64.0f),
          fsize);
    }

    /// Accessor.
    ///
    /// \param unicode Character by unicode.
    /// \return Reference to the character.
    const Character& getCharacter(unsigned unicode) const
    {
#if defined(USE_LD)
      if(MAX_CHARACTERS <= unicode)
      {
        std::ostringstream sstr;
        sstr << "unicode index outside of range: " << unicode;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
      if(!m_characters[unicode])
      {
        std::ostringstream sstr;
        sstr << "character " << unicode << " has not been created";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return *(m_characters[unicode]);
    }

#if defined(FONT_ENABLE_KERNING)
    /// Get kerning between 2 character indices.
    ///
    /// \param prev Previous index.
    /// \param next Next index.
    /// \return Kerning value.
    float getKerning(unsigned prev, unsigned next) const
    {
      FT_Vector delta;

      FT_Error err = dnload_FT_Get_Kerning(m_face, prev, next, FT_KERNING_DEFAULT, &delta);
#if defined(USE_LD)
      if(err)
      {
        std::ostringstream sstr;
        sstr << "could not get kerning information for " << prev << " and " << next;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#else
      (void)err;
#endif

      return static_cast<float>(delta.x) * (1.0f / 4096.0f); // Where does the scale come from?
    }
#endif

  private:
    /// Initialize FreeType.
    static void freetype_init()
    {
      if(g_freetype_library)
      {
        return;
      }

      FT_Error err = dnload_FT_Init_FreeType(&g_freetype_library);
#if defined(USE_LD)
      if(0 != err)
      {
        std::ostringstream sstr;
        sstr << "could not init FreeType: " << err;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#else
      (void)err;
#endif
    }

  public:
#if defined(USE_LD)
    /// Deinitialize FreeType.
    static void freetype_quit()
    {
      if(!g_freetype_library)
      {
        return;
      }

      FT_Error err = FT_Done_FreeType(g_freetype_library);
      if(0 != err)
      {
        std::ostringstream sstr;
        sstr << "could not close FreeType: " << err;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }

      g_freetype_library = NULL;
    }
#endif
};

FT_Library Font::g_freetype_library = NULL;

#endif
