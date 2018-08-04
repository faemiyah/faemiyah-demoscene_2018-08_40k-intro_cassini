#ifndef VERBATIM_TEXTURE_HPP
#define VERBATIM_TEXTURE_HPP

/// Filtering mode.
enum FilteringMode
{
  /// Nearest-neighbor filtering.
  NEAREST,

  /// Bilinear filtering.
  BILINEAR,

  /// Trilinear filtering.
  TRILINEAR,

  /// Trilinear filtering with anisotropy.
  ANISOTROPIC,
};

const float ANISOTROPY_LEVEL = 4.0f;

/// Wrap mode.
enum WrapMode
{
  /// Wrap at edge.
  WRAP,

  /// Clamp to edge.
  CLAMP 
};

/// Texture class.
class Texture
{
  private:
    /// Number of texture units managed by this class.
    static unsigned const MAX_TEXTURE_UNITS = 8;

  private:
    /// Currently bound textures. One for each texture unit.
    static Texture const *g_current_texture[MAX_TEXTURE_UNITS];

    /// Currently active texture unit.
    static unsigned g_active_texture_unit;

  private:
    /// OpenGL texture type.
    GLenum m_type;

    /// OpenGL texture name.
    GLuint m_id;

  private:
    /// Deleted copy constructor.
    Texture(const Texture&) = delete;
    /// Deleted assignment.
    Texture& operator=(const Texture&) = delete;

  public:
    /// Constructor.
    explicit Texture(GLenum type) :
      m_type(type)
    {
      dnload_glGenTextures(1, &m_id);
    }

    /// Destructor.
    ~Texture()
    {
#if defined(USE_LD)
      unbind();

      glDeleteTextures(1, &m_id);
#endif
    }

  protected:
    /// Set filtering mode for currently bound texture.
    ///
    /// \param data Data passed Filtering mode.
    /// \param filtering Filtering mode.
    /// \return True if mipmaps in use, false if not.
    bool setFiltering(void* data, FilteringMode filtering) const
    {
      // 'nearest' -filtering forced.
      if(NEAREST == filtering)
      {
        dnload_glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        dnload_glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        return false;
      }

      // Image textures may use mipmaps.
      if(data)
      {
        // Magnification always linear.
        dnload_glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Bilinear.
        if(BILINEAR == filtering)
        {
          dnload_glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          return false;
        }

        // Trilinear is on even if anisotropy is not.
        dnload_glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

#if !defined(VGL_GLES_ONLY)
        if (ANISOTROPIC == filtering)
        {
            dnload_glTexParameterf(m_type, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY_LEVEL);
        }
#endif

        // Mipmaps on.
        dnload_glGenerateMipmap(m_type);
        return true;
      }

      // FBOs use bilinar.
      dnload_glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      dnload_glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      return false;
    }

    /// Set wrap mode for currently bound texture.
    ///
    /// \param wrap Wrap mode.
    void setWrapMode(WrapMode wrap) const
    {
      GLint mode = (wrap == WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
      //GLint mode = (wrap == WRAP) ? GL_REPEAT : GL_CLAMP;

      dnload_glTexParameteri(m_type, GL_TEXTURE_WRAP_S, mode);
      dnload_glTexParameteri(m_type, GL_TEXTURE_WRAP_T, mode);

      /// Set 3rd axis as well if necessary.
      if((m_type == GL_TEXTURE_3D) || (m_type == GL_TEXTURE_CUBE_MAP))
      {
        dnload_glTexParameteri(m_type, GL_TEXTURE_WRAP_R, mode);
      }
    }

    /// Begin update operation.
    ///
    /// \return Texture to restore to active texture unit.
    const Texture* updateBegin()
    {
      const Texture* prev_texture = g_current_texture[g_active_texture_unit];

      bind(g_active_texture_unit);

      return prev_texture;
    }

    /// End update operation.
    ///
    /// \param texture Texture to restore to active texture unit.
    void updateEnd(const Texture* texture)
    {
      if(texture)
      {
        texture->bind(g_active_texture_unit);
      }
#if defined(USE_LD)
      else
      {
        unbind();
      }
#endif
    }

#if defined(USE_LD)
    /// Unbind texture from whichever texture unit it's bound to.
    void unbind() const
    {
      unsigned original_texture_unit = g_active_texture_unit;
  
      for(unsigned ii = 0; (ii < MAX_TEXTURE_UNITS); ++ii)
      {
        if(g_current_texture[ii] && (g_current_texture[ii]->m_id == m_id))
        {
          select_texture_unit(ii);
          glBindTexture(m_type, 0);
          g_current_texture[ii] = NULL;
        }
      }

      select_texture_unit(original_texture_unit);
    }
#endif

  public:
    /// Bind texture to given texture unit 
    ///
    /// \param op Unit to bind to.
    void bind(unsigned op) const
    {
#if defined(USE_LD)
      if(op >= MAX_TEXTURE_UNITS)
      {
        std::ostringstream sstr;
        sstr << "trying to bind to texture unit index " << op << " of maximum " << MAX_TEXTURE_UNITS;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif

      if(g_current_texture[op] != this)
      {
        select_texture_unit(op);
        dnload_glBindTexture(m_type, m_id);
        g_current_texture[op] = this;
      }
    }

    /// Accessor.
    ///
    /// \return Texture id.
    unsigned getId() const
    {
      return m_id;
    }

    /// Accessor.
    ///
    /// \return Texture type.
    GLenum getType() const
    {
      return m_type;
    }

  private:
    /// Activate a texture unit.
    ///
    /// \param op Texture unit to activate.
    static void select_texture_unit(unsigned op)
    {
      if(g_active_texture_unit != op)
      {
        dnload_glActiveTexture(GL_TEXTURE0 + op);
        g_active_texture_unit = op;
      }
    }    
};

const Texture *Texture::g_current_texture[Texture::MAX_TEXTURE_UNITS] =
{
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
};
unsigned Texture::g_active_texture_unit = 0;

#endif
