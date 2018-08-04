#ifndef VERBATIM_FRAME_BUFFER_HPP
#define VERBATIM_FRAME_BUFFER_HPP

#include "verbatim_texture_2d.hpp"

/// Framebuffer.
class FrameBuffer
{
  private:
    /// Current render target.
    static FrameBuffer const *g_current_frame_buffer;

  private:
    /// Framebuffer id.
    GLuint m_id;

    /// Color buffer id.
    GLuint m_color_buffer;

    /// Depth buffer id.
    GLuint m_depth_buffer;

    /// Attached render target texture.
    Texture2D m_color_texture;

    /// Attached render target texture.
    Texture2D m_depth_texture;

    /// Width of this framebuffer.
    unsigned m_width;

    /// Height of this framebuffer.
    unsigned m_height;

  public:
    /// Constructor.
    ///
    /// \param width Framebuffer width.
    /// \param height Framebuffer height.
    /// \param color_texture Use color texture (default: yes).
    /// \param depth_texture Use depth texture (default: no).
    /// \param bpc Bytes per pixel (default: 2).
    /// \param filtering Filtering mode (default: BILINEAR).
    /// \param wrap Wrap mode (default: CLAMP).
    explicit FrameBuffer(unsigned width, unsigned height, bool color_texture = true,
        bool depth_texture = false, unsigned bpc = 2, FilteringMode filtering = BILINEAR,
        WrapMode wrap = CLAMP) :
      m_id(0),
      m_depth_buffer(0),
      m_width(width),
      m_height(height)
    {
#if defined(USE_LD)
      if((m_width <= 0) || (m_height <= 0))
      {
        std::ostringstream sstr;
        sstr << "invalid framebuffer dimensions: " << width << "x" << height;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif

      if(color_texture)
      {
        m_color_texture.update(width, height, 4, bpc, filtering, wrap);
      }
      if(depth_texture)
      {
        m_depth_texture.update(width, height, 0, (bpc < 2) ? 2 : bpc, filtering, wrap);
      }

      dnload_glGenFramebuffers(1, &m_id);
      dnload_glBindFramebuffer(GL_FRAMEBUFFER, m_id);

      if(color_texture)
      {
        dnload_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
            m_color_texture.getId(), 0);
      }

      if(depth_texture)
      {
        dnload_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
            m_depth_texture.getId(), 0);
      }
#if 0
      // Generate renderbuffer in lieu of texture.
      else
      {
        dnload_glGenRenderbuffers(1, &m_depth_buffer);
        dnload_glBindRenderbuffer(GL_RENDERBUFFER, m_depth_buffer);
        dnload_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
        dnload_glBindRenderbuffer(GL_RENDERBUFFER, 0);
        dnload_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_buffer);
      }
#endif

#if defined(USE_LD)
      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if(status != GL_FRAMEBUFFER_COMPLETE)
      {
        std::ostringstream sstr;
        sstr << "framebuffer " << m_width << "x" << m_height << (color_texture ? "C" : "") <<
          (depth_texture ? "D" : "") << ": error 0x" << std::hex << status << std::endl;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif

      // Restore earlier framebuffer.
      dnload_glBindFramebuffer(GL_FRAMEBUFFER, get_current_frame_buffer_id());
    }

    /// Destructor.
    ~FrameBuffer()
    {
#if defined(USE_LD)
      glDeleteFramebuffers(1, &m_id);
      if(m_color_buffer)
      {
        glDeleteRenderbuffers(1, &m_color_buffer);
      }
      if(m_depth_buffer)
      {
        glDeleteRenderbuffers(1, &m_depth_buffer);
      }
#endif
    }

  public:
    /// Bind this framebuffer to use.
    void bind() const
    {
      if(g_current_frame_buffer != this)
      {
        dnload_glBindFramebuffer(GL_FRAMEBUFFER, m_id);
        dnload_glViewport(0, 0, static_cast<GLsizei>(m_width), static_cast<GLsizei>(m_height));
        g_current_frame_buffer = this;
      }
    }

    /// Accessor.
    ///
    /// \return Framebuffer id.
    unsigned getId() const
    {
      return m_id;
    }

    /// Accessor.
    ///
    /// \return Get attached texture.
    Texture const& getTextureColor() const
    {
      return m_color_texture;
    }

    /// Accessor.
    ///
    /// \return Get attached texture.
    Texture const& getTextureDepth() const
    {
      return m_depth_texture;
    }

    /// Accessor.
    ///
    /// \return Render target width.
    unsigned getWidth() const
    {
      return m_width;
    }

    /// Accessor.
    ///
    /// \return Render target height.
    unsigned getHeight() const
    {
      return m_height;
    }

  private:
    /// Get current render target id.
    ///
    /// \return ID of currently bound render target or 0.
    static unsigned get_current_frame_buffer_id()
    {
      return g_current_frame_buffer ? g_current_frame_buffer->getId() : 0;
    }

  public:
    /// Bind default framebuffer.
    ///
    /// \param screen_width Width of default framebuffer.
    /// \param screen_height Height of default framebuffer.
    static void bind_default_frame_buffer(unsigned screen_width, unsigned screen_height)
    {
      if(g_current_frame_buffer)
      {
        dnload_glBindFramebuffer(GL_FRAMEBUFFER, 0);
        dnload_glViewport(0, 0, static_cast<GLsizei>(screen_width), static_cast<GLsizei>(screen_height));
        g_current_frame_buffer = NULL;
      }
    }
};

const FrameBuffer *FrameBuffer::g_current_frame_buffer = NULL;

#endif
