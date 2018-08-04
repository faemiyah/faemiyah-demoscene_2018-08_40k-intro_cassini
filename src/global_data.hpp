#ifndef GLOBAL_DATA_HPP
#define GLOBAL_DATA_HPP

#include "direction.hpp"
#include "global_data_temporary.hpp"

/// Global data container.
class GlobalData
{
  private:
    /// Screen width.
    vec2 m_screen_size;
    /// Field of view.
    float m_fov;

    /// Pipeline program.
    Pipeline m_pipeline_fluid;
    /// Pipeline program.
    Pipeline m_pipeline_precalc;
    /// Pipeline program.
    Pipeline m_pipeline_font;
    /// Pipeline program.
    Pipeline m_pipeline_clouds;
    /// Pipeline program.
    Pipeline m_pipeline_clouds_post;
    /// Pipeline program.
    Pipeline m_pipeline_huygens;
    /// Pipeline program.
    Pipeline m_pipeline_huygens_post;
    /// Pipeline program.
    Pipeline m_pipeline_enceladus;
    /// Pipeline program.
    Pipeline m_pipeline_simple;
    /// Pipeline program.
    Pipeline m_pipeline_simple_post;
    /// Pipeline program.
    Pipeline m_pipeline_space;
    /// Pipeline program.
    Pipeline m_pipeline_space_post;
    
    /// Framebuffer.
    FrameBuffer m_fbo;
    /// Loq-quality framebuffer.
    FrameBuffer m_fbo_lq;
    /// Fluid simulation framebuffer 1 (velocity buffer 1).
    FrameBuffer m_fluid_fbo_1;
    /// Fluid simulation framebuffer 2 (velocity buffer 2).
    FrameBuffer m_fluid_fbo_2;
    /// Fluid simulation framebuffer 3 (dye buffer 1).
    FrameBuffer m_fluid_dye_fbo_1;
    /// Fluid simulation framebuffer 4 (dye buffer 2).
    FrameBuffer m_fluid_dye_fbo_2;
    /// Fluid simulation framebuffer 5 (pressure).
    FrameBuffer m_fluid_pressure_fbo;

    /// Font.
    Font m_font;

    /// Fluid simulation boundary texture.
    Texture2D m_tex_fluid_boundary;
    /// Fluid simulation boundary texture.
    Texture2D m_tex_fluid_input;
    /// Soft noise texture.
    Texture2D m_tex_noise_soft;
    /// Noise volume.
    Texture3D m_tex_noise_volume_hq;
    /// Noise volume.
    Texture3D m_tex_noise_volume_lq;
    /// Saturn band texture.
    Texture2D m_tex_saturn_bands;
    /// Saturn ring texture.
    Texture2D m_tex_saturn_rings;
    /// Space texture.
    TextureCube m_tex_space;
    /// Moon texture.
    TextureCube m_tex_enceladus;
    /// Moon surface texture.
    Texture2D m_tex_enceladus_surface;
    /// Moon texture.
    TextureCube m_tex_tethys;
    /// Trail texture.
    TextureCube m_tex_trail;

    /// Direction data.
    Direction m_direction;

    /// Temporary global data.
    uptr<GlobalDataTemporary> m_temporary;

    /// Distort array.
    seq<float> m_distorts;
    /// Offset array (for e.g. 'shakycam').
    seq<vec3> m_offsets;

  public:
    /// Constructor.
    explicit GlobalData(unsigned width, unsigned height, float fov) :
      m_screen_size(static_cast<float>(width), static_cast<float>(height)),
      m_fov(fov),
      m_pipeline_fluid(g_shader_header, g_shader_vertex_fluid, g_shader_fragment_fluid),
      m_pipeline_precalc(g_shader_header, g_shader_vertex_precalc, g_shader_fragment_precalc),
      m_pipeline_font(g_shader_header, g_shader_vertex_font, g_shader_fragment_font),
      m_pipeline_clouds(g_shader_header, g_shader_vertex_space, g_shader_fragment_clouds),
      m_pipeline_clouds_post(g_shader_header, g_shader_vertex_space_post, g_shader_fragment_clouds_post),
      m_pipeline_huygens(g_shader_header, g_shader_vertex_huygens, g_shader_fragment_huygens),
      m_pipeline_huygens_post(g_shader_header, g_shader_vertex_huygens_post, g_shader_fragment_huygens_post),
      m_pipeline_enceladus(g_shader_header, g_shader_vertex_space, g_shader_fragment_enceladus),
      m_pipeline_simple(g_shader_header, g_shader_vertex_space, g_shader_fragment_simple),
      m_pipeline_simple_post(g_shader_header, g_shader_vertex_space_post, g_shader_fragment_simple_post),
      m_pipeline_space(g_shader_header, g_shader_vertex_space, g_shader_fragment_space),
      m_pipeline_space_post(g_shader_header, g_shader_vertex_space_post, g_shader_fragment_space_post),
      m_fbo(width, height),
      m_fbo_lq(width * 2 / 3, height * 2 / 3),
      m_fluid_fbo_1(FLUID_WIDTH, FLUID_HEIGHT, true, false, 4, BILINEAR, WRAP),
      m_fluid_fbo_2(FLUID_WIDTH, FLUID_HEIGHT, true, false, 4, BILINEAR, WRAP),
      m_fluid_dye_fbo_1(FLUID_WIDTH, FLUID_HEIGHT, true, false, 4, BILINEAR, WRAP),
      m_fluid_dye_fbo_2(FLUID_WIDTH, FLUID_HEIGHT, true, false, 4, BILINEAR, WRAP),
      m_fluid_pressure_fbo(FLUID_WIDTH, FLUID_HEIGHT, true, false, 4, BILINEAR, WRAP),
      m_font(128, g_font_paths),
      m_direction(g_direction),
      m_temporary(new GlobalDataTemporary())
    {
      // Create all usual ASCII7 characters.
      for(unsigned ii = static_cast<unsigned>('!'); (static_cast<unsigned>('z') >= ii); ++ii)
      {
        m_font.createCharacter(ii);
      }

      // Randomize distorts. Each distort may exist for a certain amount of time depending on effect.
      // They are mapped to an area of [-1, 1] - it is up to the effect to scale this to correct range to
      // limit visibility.
      {
        const float DEGRADE = 0.9965f;
        vec3 offset(0.0f);
        vec3 delta(0.0f);

        // Generate a bit more random data just to be sure.
        for(unsigned ii = 0; (ii < (INTRO_LENGTH_TICKS + DIRECTION_SPLIT_DURATION + 1000)); ++ii)
        {
          m_distorts.push_back(frand(-1.0f, 1.0f));

          // Delta is normalized later to whichever value is desired.
          float dx = frand(-1.0f, 1.0f);
          float dy = frand(-1.0f, 1.0f);
          float dz = frand(-1.0f, 1.0f);
          delta += vec3(dx, dy, dz);
          offset += delta;
          m_offsets.push_back(offset);
          offset *= DEGRADE;
          delta *= DEGRADE;
        }
      }

      // Saturn bands have already been calculated.
      updateSaturnBands(m_temporary->saturn_bands);

#if defined(USE_LD) || (defined(EXTRA_GLGETERROR) && (EXTRA_GLGETERROR != 0))
      vgl::error_check();
#endif
    }

  public:
    /// Restore default framebuffer.
    void bindDefaultFrameBuffer() const
    {
      FrameBuffer::bind_default_frame_buffer(m_fbo.getWidth(), m_fbo.getHeight());
    }

    /// Calculates the width of a string.
    ///
    /// \param fs Font size.
    /// \param content String content.
    /// \return Width of the string.
    float calculateStringWidth(float fs, const char* content) const
    {
      static const float FONT_SPACE_WIDTH = 0.4f;
      float cx = 0.0f;

      for(;; ++content)
      {
        char cc = *content;
        if(!cc)
        {
          break;
        }

        // Do not try to render whitespace.
        if(' ' == cc)
        {
#if defined(FONT_ENABLE_KERNING)
          previous_char = 0;
#endif
          cx += fs * FONT_SPACE_WIDTH;
          continue;
        }

        const Character &chr = m_font.getCharacter(static_cast<unsigned>(cc));

#if defined(FONT_ENABLE_KERNING)
        unsigned current_char = chr.getFontIndex();
        if(previous_char)
        {
          float kerning = m_globals.fnt.getKerning(previous_char, current_char);
          cx += kerning * fs;
        }
#endif

        cx += chr.getAdvanceX() * fs;
#if defined(FONT_ENABLE_KERNING)
        previous_char = current_char;
#endif
      }

      return cx;
    }

    /// Draw this character.
    ///
    /// \param rect_uniform Uniform for glyph rectangle.
    /// \param phase_uniform Uniform for glyph phase.
    /// \param tex_uniform Uniform for glyph texture.
    /// \param chr Character.
    /// \param px X offset.
    /// \param py Y offset.
    /// \param fs Font size.
    void drawCharacter(GLuint rect_uniform, GLuint phase_uniform, GLuint tex_uniform, const Character& chr,
        float px, float py, float fs, float phase) const
    {
      vec2 chrOffset = chr.getQuadOffset() * fs;
      vec2 chrSize = chr.getQuadSize() * fs;
      float rect[4] =
      {
        chrOffset.x() + px,
        chrOffset.y() + py,
        chrSize.x(),
        chrSize.y(),
      };

#if 0
      std::cout << "quad: " << chr.getQuadOffset() << " ; " << chr.getQuadSize() << std::endl;
      std::cout << "rect: " << rect[0] << " ; " << rect[1] << " ; " << rect[2] << " ; " << rect[3] << std::endl;
      std::cout << "fs: " << fs << " ; phase: " << phase << std::endl;
#endif

      m_pipeline_font.uniformVert4fv(rect_uniform, 1, rect);
      m_pipeline_font.uniformFrag(phase_uniform, phase);
      m_pipeline_font.uniformFrag(tex_uniform, 0, chr.getTexture());
      dnload_glRects(0, 0, 1, 1);
    }

    /// Draw a string.
    ///
    /// \param rect_uniform Uniform for glyph rectangl.
    /// \param phase_uniform Uniform for glyph phase.
    /// \param tex_uniform Uniform for glyph texture.
    /// \param px X position.
    /// \param py Y position.
    /// \param fs Font size.
    /// \param content Content string.
    void drawText(GLuint rect_uniform, GLuint phase_uniform, GLuint tex_uniform, float px, float py, float fs,
        float phase, const char *content) const
    {
      static const float FONT_SPACE_WIDTH = 0.5f;
      float cx = px;
#if defined(FONT_ENABLE_KERNING)
      unsigned previous_char = 0;
#endif

      for(;; ++content)
      {
        char cc = *content;
        if(!cc)
        {
          break;
        }

        // Do not try to render whitespace.
        if(' ' == cc)
        {
#if defined(FONT_ENABLE_KERNING)
          previous_char = 0;
#endif
          cx += fs * FONT_SPACE_WIDTH;
          continue;
        }

        const Character &chr = m_font.getCharacter(static_cast<unsigned>(cc));

#if defined(FONT_ENABLE_KERNING)
        unsigned current_char = chr.getFontIndex();
        if(previous_char)
        {
          float kerning = m_globals.fnt.getKerning(previous_char, current_char);
          cx += kerning * fs;
        }
#endif

        drawCharacter(rect_uniform, phase_uniform, tex_uniform, chr, cx, py, fs, phase);

        cx += chr.getAdvanceX() * fs;
#if defined(FONT_ENABLE_KERNING)
        previous_char = current_char;
#endif
      }
    }

    /// Draw a string, centered on coordinates.
    ///
    /// \param rect_uniform Uniform for glyph rectangle.
    /// \param phase_uniform Uniform for glyph phase.
    /// \param tex_uniform Uniform for glyph texture.
    /// \param px X position.
    /// \param py Y position.
    /// \param fs Font size.
    /// \param content Content string.
    void drawTextCentered(GLuint rect_uniform, GLuint phase_uniform, GLuint tex_uniform, float px, float py,
        float fs, float phase, const char *content) const
    {
      float width = calculateStringWidth(fs, content);

      drawText(rect_uniform, phase_uniform, tex_uniform, px - width * 0.5f, py - fs * 0.5f, fs, phase,
          content);
    }

    /// Accessor.
    const Pipeline& getPipelineFluid() const
    {
      return m_pipeline_fluid;
    }
    /// Accessor.
    const Pipeline& getPipelinePrecalc() const
    {
      return m_pipeline_precalc;
    }
    /// Accessor.
    const Pipeline& getPipelineFont() const
    {
      return m_pipeline_font;
    }
    /// Accessor.
    const Pipeline& getPipelineClouds() const
    {
      return m_pipeline_clouds;
    }
    /// Accessor.
    const Pipeline& getPipelineCloudsPost() const
    {
      return m_pipeline_clouds_post;
    }
    /// Accessor.
    const Pipeline& getPipelineHuygens() const
    {
      return m_pipeline_huygens;
    }
    /// Accessor.
    const Pipeline& getPipelineHuygensPost() const
    {
      return m_pipeline_huygens_post;
    }
    /// Accessor.
    const Pipeline& getPipelineEnceladus() const
    {
      return m_pipeline_enceladus;
    }
    /// Accessor.
    const Pipeline& getPipelineSimple() const
    {
      return m_pipeline_simple;
    }
    /// Accessor.
    const Pipeline& getPipelineSimplePost() const
    {
      return m_pipeline_simple_post;
    }

    /// Accessor.
    const Pipeline& getPipelineSpace() const
    {
      return m_pipeline_space;
    }
    /// Accessor.
    const Pipeline& getPipelineSpacePost() const
    {
      return m_pipeline_space_post;
    }

    /// Accessor.
    float getScreenWidth() const
    {
      return m_screen_size[0];
    }
    /// Accessor.
    float getScreenHeight() const
    {
      return m_screen_size[1];
    }
    /// Accessor.
    float getAspectRatio() const
    {
      return getScreenWidth() / getScreenHeight();
    }

    /// Accessor.
    float getFov() const
    {
      return m_fov;
    }

    /// Accessor.
    const FrameBuffer& getFbo() const
    {
      return m_fbo;
    }
    /// Accessor.
    const FrameBuffer& getFboLq() const
    {
      return m_fbo_lq;
    }
    /// Accessor.
    /// \param idx Fluid dye framebuffer index (0 or 1).
    const FrameBuffer& getFluidDyeFbo(int idx) const
    {
      if(idx >= 1)
      {
        return m_fluid_dye_fbo_2;
      }
      return m_fluid_dye_fbo_1;
    }
    /// Accessor.
    /// \param idx Fluid framebuffer index (0 or 1).
    const FrameBuffer& getFluidFbo(int idx) const
    {
      if(idx >= 1)
      {
        return m_fluid_fbo_2;
      }
      return m_fluid_fbo_1;
    }
    /// Accessor.
    const FrameBuffer& getFluidPressureFbo() const
    {
      return m_fluid_pressure_fbo;
    }

    /// Accessor.
    const Font& getFont() const
    {
      return m_font;
    }

    /// Accessor.
    const Texture2D& getTextureFluidBoundary() const
    {
      return m_tex_fluid_boundary;
    }
    /// Accessor.
    const Texture2D& getTextureFluidInput() const
    {
      return m_tex_fluid_input;
    }
    /// Accessor.
    const Texture2D& getTextureNoiseSoft() const
    {
      return m_tex_noise_soft;
    }
    /// Accessor.
    const Texture3D& getTextureNoiseVolumeHq() const
    {
      return m_tex_noise_volume_hq;
    }
    /// Accessor.
    const Texture3D& getTextureNoiseVolumeLq() const
    {
      return m_tex_noise_volume_lq;
    }
    /// Accessor.
    const Texture2D& getTextureSaturnBands() const
    {
      return m_tex_saturn_bands;
    }
    /// Accessor.
    const Texture2D& getTextureSaturnRings() const
    {
      return m_tex_saturn_rings;
    }
    /// Accessor.
    const TextureCube& getTextureSpace() const
    {
      return m_tex_space;
    }
    /// Accessor.
    const TextureCube& getTextureEnceladus() const
    {
      return m_tex_enceladus;
    }
    /// Accessor.
    const Texture2D& getTextureEnceladusSurface() const
    {
      return m_tex_enceladus_surface;
    }
    /// Accessor.
    const TextureCube& getTextureTethys() const
    {
      return m_tex_tethys;
    }
    /// Accessor.
    const TextureCube& getTextureTrail() const
    {
      return m_tex_trail;
    }

    /// Accessor.
    /// \param op Millisecond to get distort for.
    float getDistort(int op) const
    {
#if defined(USE_LD)
      if(op >= static_cast<int>(m_distorts.size()))
      {
        std::ostringstream sstr;
        sstr << "getDistort(): accessing distort index " << op << ", array size is " << m_distorts.size();
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      return m_distorts[op];
    }
    /// Accessor.
    /// \param op Millisecond to get offset for.
    const vec3& getOffset(int op) const
    {
      return m_offsets[op];
    }

    /// Is there a pending update?
    ///
    /// \return True if yes, false if no.
    bool hasPendingUpdate() const
    {
      return m_temporary && m_temporary->hasPendingUpdate();
    }

    /// Is temporary calculation done?
    ///
    /// \return True if yes, false if no.
    bool isDone() const
    {
      return m_temporary && m_temporary->isDone();
    }

    /// Resolve direction frame based on timestamp.
    ///
    /// \param stamp Timestamp in frames.
    /// \return Direction frame data.
    DirectionFrame resolveDirectionFrame(int stamp) const
    {
      return m_direction.resolveDirectionFrame(stamp);
    }

#if defined(USE_LD)
    /// Re-link pipelines.
    /// \return Empty optional on success, pipeline that failed to link on failure.
    opt<Pipeline*> linkPipelines()
    {
      if(!m_pipeline_fluid.link())
      {
        return opt<Pipeline*>(&m_pipeline_fluid);
      }
      if(!m_pipeline_precalc.link())
      {
        return opt<Pipeline*>(&m_pipeline_precalc);
      }
      if(!m_pipeline_font.link())
      {
        return opt<Pipeline*>(&m_pipeline_font);
      }
      if(!m_pipeline_clouds.link())
      {
        return opt<Pipeline*>(&m_pipeline_clouds);
      }
      if(!m_pipeline_clouds_post.link())
      {
        return opt<Pipeline*>(&m_pipeline_clouds_post);
      }
      if(!m_pipeline_huygens.link())
      {
        return opt<Pipeline*>(&m_pipeline_huygens);
      }
      if(!m_pipeline_huygens_post.link())
      {
        return opt<Pipeline*>(&m_pipeline_huygens_post);
      }
      if(!m_pipeline_enceladus.link())
      {
        return opt<Pipeline*>(&m_pipeline_enceladus);
      }
      if(!m_pipeline_simple.link())
      {
        return opt<Pipeline*>(&m_pipeline_simple);
      }
      if(!m_pipeline_simple_post.link())
      {
        return opt<Pipeline*>(&m_pipeline_simple_post);
      }
      if(!m_pipeline_space.link())
      {
        return opt<Pipeline*>(&m_pipeline_space);
      }
      if(!m_pipeline_space_post.link())
      {
        return opt<Pipeline*>(&m_pipeline_space_post);
      }
      return opt<Pipeline*>();
    }
#endif

    /// Update fluid textures.
    ///
    /// \param fluid_boundary Fluid boundary image.
    /// \param fluid_input Fluid input image.
    void updateFluid(Image2DRGBA& fluid_boundary, Image2DRGBA& fluid_input)
    {
      m_tex_fluid_boundary.update(fluid_boundary, 1, NEAREST, CLAMP);
      m_tex_fluid_input.update(fluid_input, 4);
    }

    /// Partial update.
    void updatePartial()
    {
      ScopedLock guard(m_temporary->getMutex());

      if(m_temporary->space)
      {
        m_tex_space.update(*(m_temporary->space));
        m_temporary->space.reset();
      }
      if(m_temporary->enceladus)
      {
        m_tex_enceladus.update(*(m_temporary->enceladus), 2);
        m_temporary->enceladus.reset();
      }
      if(m_temporary->tethys)
      {
        m_tex_tethys.update(*(m_temporary->tethys), 2);
        m_temporary->tethys.reset();
      }
      if(m_temporary->trail)
      {
        m_tex_trail.update(*(m_temporary->trail), 2, NEAREST);
        m_temporary->trail.reset();
      }

      m_temporary->signal();
    }

    /// Updates saturn bands texture.
    ///
    /// \param img Image to update with.
    void updateSaturnBands(Image2D& img)
    {
      m_tex_saturn_bands.update(img, 1, TRILINEAR, CLAMP);
    }

    /// Update data to GPU.
    void update()
    {
      m_tex_noise_soft.update(m_temporary->noise_2d, 2);
      m_tex_noise_volume_hq.update(m_temporary->noise_3d_hq, 2);
      m_tex_noise_volume_lq.update(m_temporary->noise_3d_lq, 1);
      m_tex_saturn_rings.update(*(m_temporary->saturn_rings));
      m_tex_enceladus_surface.update(m_temporary->enceladus_surface);

#if defined(USE_LD)
      // None of the cube maps should exist after partial updates.
      if(m_temporary->space || m_temporary->dione || m_temporary->enceladus || m_temporary->rhea ||
          m_temporary->tethys)
      {
        BOOST_THROW_EXCEPTION(std::runtime_error("GlobalData::update: cube maps still present"));
      }
#endif
      // Get rid of temporary data.
      m_temporary.reset();

#if defined(USE_LD)
      std::cout << vgl::get_data_size_texture() << " bytes used for texture data" << std::endl;
#endif
    }

  public:
    /// Run initialization.
    /// \param pointer to global data.
    /// \return Always 0.
    static int initialize_func(void* global_data)
    {
      GlobalData* data = static_cast<GlobalData*>(global_data);

      data->m_temporary->initialize();

      return 0;
    }
};

#endif
