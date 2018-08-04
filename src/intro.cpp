//######################################
// Include #############################
//######################################

#include "dnload.h"

#if defined(USE_LD)
#include "glsl_program.hpp"
#include "image_png.hpp"
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>
#include <boost/scoped_array.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
namespace po = boost::program_options;
#endif

//######################################
// Define ##############################
//######################################

#if !defined(DISPLAY_MODE)
/// Screen mode.
///
/// Negative values windowed.
/// Positive values fullscreen.
#define DISPLAY_MODE -720
#endif

/// \cond
#if (0 > (DISPLAY_MODE))
#define SCREEN_F 0
#define SCREEN_H (-(DISPLAY_MODE))
#elif (0 < (DISPLAY_MODE))
#define SCREEN_F 1
#define SCREEN_H (DISPLAY_MODE)
#else
#error "invalid display mode (pre)"
#endif
#if ((800 == SCREEN_H) || (1200 == SCREEN_H))
#define SCREEN_W ((SCREEN_H / 10) * 16)
#else
#define SCREEN_W (((SCREEN_H * 16) / 9) - (((SCREEN_H * 16) / 9) % 4))
#endif
/// \endcond

/// Size of one sample in bytes.
#define AUDIO_SAMPLE_SIZE 4

/// \cond
#if (4 == AUDIO_SAMPLE_SIZE)
#define AUDIO_SAMPLE_TYPE_SDL AUDIO_F32SYS
typedef float sample_t;
#elif (2 == AUDIO_SAMPLE_SIZE)
#define AUDIO_SAMPLE_TYPE_SDL AUDIO_S16SYS
typedef int16_t sample_t;
#elif (1 == AUDIO_SAMPLE_SIZE)
#define AUDIO_SAMPLE_TYPE_SDL AUDIO_U8
typedef uint8_t sample_t;
#else
#error "invalid audio sample size"
#endif
/// \endcond

/// Audio channels.
#define AUDIO_CHANNELS 2

/// Audio samplerate.
#define AUDIO_SAMPLERATE 44100

/// Audio byterate.
#define AUDIO_BYTERATE (AUDIO_CHANNELS * AUDIO_SAMPLERATE * AUDIO_SAMPLE_SIZE)

/// Ticks per frame, for simplicity not going for 60fps exact.
#define FRAME_MILLISECONDS 17

/// Intro length in seconds.
#define INTRO_LENGTH_TICKS (167 * 1000)

/// Intro length (in bytes of audio).
#define INTRO_LENGTH_BYTES (INTRO_LENGTH_TICKS / 1000 * AUDIO_BYTERATE)

/// Synth work size.
#define SYNTH_WORK_SIZE (16 * 1024 * 1024)

/// Intro start position (in seconds).
#define INTRO_START 0

/// Precalculated file to load if synth cannot be ran.
#define SYNTH_TEST_INPUT_FILE "cassini.wav"
#if 0 // Change to 1 to enable writing synth output.
/// Output file to write for synth test.
#define SYNTH_TEST_OUTPUT_FILE SYNTH_TEST_INPUT_FILE
#endif

//######################################
// Global data #########################
//######################################

/// Global SDL window storage.
static SDL_Window *g_sdl_window;

/// Font paths to try.
static const char* g_font_paths[] =
{
  "/usr/local/share/fonts/dejavu/DejaVuSerif-Italic.ttf",
  "/usr/share/fonts/truetype/dejavu/DejaVuSerif-Italic.ttf",
#if defined(USE_LD)
  "./rel/DejaVuSerif-Italic.ttf",
  "DejaVuSerif-Italic.ttf",
#endif
  NULL,
};

/// Sketch texts.
static const char* g_sketch_text[] =
{
  "Fig. I",
  "Fig. II",
  "Fig. III",
  "Fig. IV",
  "Fig. V",
  "Fig. VI",
  "Fig. VII",
  "Fig. VIII",
  "Fig. IX",
  "Fig. X",
  "Fig. XI",
  "Fig. XII",
  "Fig. XIII",
  "Ann. 1676",
};

/// Struct for Text locations and times.
struct TextLocation
{
  /// Text to print.
  const char* txt;

  /// X position.
  int16_t px;

  /// Y position.
  int16_t py;

  /// Appear time.
  int stamp;
};

/// Greets texts.
static TextLocation g_blurb_greets[] =
{
  { "Blobtrox", -100, -29, 1000, },
  { "Ctrl-Alt-Test", -85, -49, 1200, },
  { "Desire", -70, -69, 1400, },
  { "Ekspert", -55, -89, 1600, },
  { "Fit", -90, 76, 5600, },
  { "Epoch", -108, 56, 5700, },
  { "G-INcorporated", -124, 36, 5800, },
  { "Hedelmae", -138, 16, 5900, },
  { "Ivory Labs", -150, -4, 6000, },
  { "Lonely Coders", -160, -24, 6100, },
  { "Luminaire", -156, -44, 6200, },
  { "Paraguay", -150, -64, 6300, },
  { "Royal Elite Ninjas", -142, -84, 6400, },
  { "TDA", -160, -22, 11200, },
  { "Tekotuotanto", -154, -42, 11400, },
  { "Traction", -142, -62, 11600, },
  { "...and you", -132, -82, 11800, },
  { NULL, 0, 0, 0 },
};

/// Credits texts (röllers).
static TextLocation g_blurb_credits_roell[] =
{
  { "note: cassini died on the way to his home planet", 0, -19, 17, },
  { NULL, 0, 0, 0 },
};

/// Credits texts.
static TextLocation g_blurb_credits[] =
{
  { "Cassini-Huygens", 0, 40, 800, },
  { "1997-10", 0, 0, 1700, },
  { "2017-09", 0, -20, 2100, },
  { "Juippi", -80, 60, 4100, },
  { "Trilkk", 0, 30, 4500, },
  { "Warma", 80, 0, 4900, },
  { "FreeBSD-ia32 33k intro", -40, -30, 5300, },
  { "Faemiyah 2018-08", 70, -60, 5700, },
  { NULL, 0, 0, 0 },
};

/// Cassini blurb.
///
/// Text taken verbatim from NASA's own rationale for ending the mission:
/// https://saturn.jpl.nasa.gov/mission/grand-finale/overview/
static TextLocation g_blurb_cassini[] =
{
  { "By 2017, Cassini had spent 13 years in", -100, 80, 0, },
  { "orbit around Saturn, following a", -100, 72, 20, },
  { "seven-year journey from Earth. The", -100, 64, 40, },
  { "spacecraft was low on the rocket fuel", -100, 56, 60, },
  { "used for adjusting its course. If left", -100, 48, 80, },
  { "unchecked, this situation would have", -100, 40, 100, },
  { "eventually prevent mission operators from", -100, 32, 120, },
  { "controlling the course of the spacecraft.", -100, 24, 140, },
  { "Two moons of Saturn, Enceladus and Titan,", -100, 16, 160, },
  { "have captured news headlines over the", -100, 8, 180, },
  { "past decade as Cassini data revealed", -100, 0, 200, },
  { "their potential to contain habitable - or", -100, -8, 220, },
  { "at least \"prebiotic\" - environments.", -100, -16, 240, },
  { "In order to avoid the unlikely", -100, -24, 260, },
  { "possibility of Cassini someday colliding", -100, -32, 280, },
  { "with one of these moons, NASA chose to", -100, -40, 300, },
  { "safely dispose of the spacecraft in the", -100, -48, 320, },
  { "atmosphere of Saturn. This ensured that", -100, -56, 340, },
  { "Cassini could not contaminate any future", -100, -64, 360, },
  { "studies of habitability and potential", -100, -72, 380, },
  { "life on those moons.", -100, -80, 400, },
  { NULL, 0, 0, 0 },
};

/// Cassini title text blurb.
static TextLocation g_blurb_cassini_why[] =
{
  { "Why End the Mission?", 80, -60, 200, },
  { NULL, 0, 0, 0 },
};

#if defined(USE_LD)

/// Developer mode global toggle.
static bool g_flag_developer = false;

/// Dircetion lock.
static bool g_direction_lock = true;

/// Usage blurb.
static const char *usage = ""
"Usage: cassini <options>\n"
"For Assembly 2018 64k intro compo.\n"
"Release version does not pertain to any size limitations.\n";

#else

/// Developer mode disabled.
#define g_flag_developer 0

#endif

//######################################
// Potentially disabled functions ######
//######################################

#if !defined(USE_LD) && (defined(EXTRA_GLGETERROR) && (EXTRA_GLGETERROR != 0))

namespace vgl
{

  /// Get an error string corresponding to a GL error.
  ///
  /// \param op GL error.
  /// \return Error string.
  const char* error_str(GLenum err)
  {
    switch(err)
    {
      case GL_NO_ERROR:
        return "GL_NO_ERROR";

      case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";

      case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";

      case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";

      case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";

      case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";

      default:
        break;
    }

    return "unknown error";
  }

  /// Perform error check.
  ///
  /// Throws an error on failure.
  static void error_check(const char* str = NULL)
  {
    GLenum err = dnload_glGetError();
    if(GL_NO_ERROR != err)
    {
      if(str)
      {
        while(*str)
        {
          dnload_putc(*str, stdout);
          ++str;
        }
        dnload_putc(':', stdout);
        dnload_putc(' ', stdout);
      }
      dnload_puts(error_str(err));
    }
  }

}

#endif

//######################################
// Global functions ####################
//######################################

#if 0
/// Random boolean value.
///
/// \return True or false.
static bool brand()
{
  return static_cast<bool>(dnload_rand() & 1);
}
#endif

/// Random unsigned value.
///
/// \param op Given maximum value.
/// \return Random value between 0 and up to but not including the maximum value.
static unsigned urand(unsigned op)
{
  return static_cast<unsigned>(dnload_rand()) % op;
}

/// Random float value.
///
/// \param op Given maximum value.
/// \return Random value between 0 and given value.
static float frand(float op)
{
  return static_cast<float>(dnload_rand() & 0xFFFF) * ((1.0f / 65535.0f) * op);
}

/// Random float value.
///
/// \param minimum Given minimum value.
/// \param maximum Given maximum value.
/// \return Random value between minimum and maximum value.
static float frand(float minimum, float maximum)
{
  return frand(maximum - minimum) + minimum;
}

/// Get current ticks.
///
/// \return Current ticks as int.
static int get_current_ticks()
{
  return static_cast<int>(dnload_SDL_GetTicks());
}

/// Tell if given event signifies we should quit.
///
/// \param event Event to check.
/// \ŗeturn True if should quit.
static bool is_quit_event(const SDL_Event& event)
{
  return ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE));
}

/// Swap buffers.
///
/// Uses global data.
static void swap_buffers()
{
  dnload_SDL_GL_SwapWindow(g_sdl_window);
}

//######################################
// Music ###############################
//######################################

#if !defined(USE_LD)
#include "synth.h"
#endif

/// Generic buffer for audio generation temp.
static uint8_t g_buffer[SYNTH_WORK_SIZE + (INTRO_LENGTH_BYTES * 9 / 8)];

/// Audio buffer for output, located after synth work size.
static uint8_t* g_audio_buffer = &(g_buffer[SYNTH_WORK_SIZE]);

/// Current audio position.
static int g_audio_position = INTRO_START * AUDIO_BYTERATE;

#if defined(USE_LD)
/// Next audio position to swap in for music position during audio callback.
static int g_next_audio_position = -1;
#endif

/// \brief Update audio stream.
///
/// \param userdata Not used.
/// \param stream Target stream.
/// \param len Number of bytes to write.
static void audio_callback(void *userdata, Uint8 *stream, int len)
{
  (void)userdata;

#if defined(USE_LD)
  if(g_next_audio_position >= 0)
  {
    g_audio_position = g_next_audio_position;
    g_next_audio_position = -1;
  }
#endif

  for(int ii = 0; (ii < len); ++ii)
  {
    stream[ii] = g_audio_buffer[g_audio_position + ii];
  }
  g_audio_position += len;
}

#if !defined(USE_LD)
// Dynamic compressor to make stuff louder
static void audio_master_fx(float *buf, size_t n_samples) {
    const float limiter_max = 0.995;

    const float preamp = 2.3;

    const float comp_th = 0.9;
    const float comp_att = 0.025;
    const float comp_rel = 0.015;

    const size_t lofi_end = 8200 * 48 * 6;
    const size_t lofi_fade_start = 8200 * 48 * 4;

    float comp_gain = 1.0;

    for (size_t i = 0; i < n_samples; ++i) {
	float in = buf[i] * preamp;

	if (i < lofi_end) {
	    int x;
	    if (i < lofi_fade_start) {
		x = 20;
		in *= 0.5;
	    } else {
		x = 20 + ((i - lofi_fade_start) * 64 / (lofi_end - lofi_fade_start));
		in *= (0.5 + ((i - lofi_fade_start) * 0.5 / (lofi_end - lofi_fade_start)));
	    }
	    int val = in * x;
	    in = val / static_cast<float>(x);
	}

	float out = in * comp_gain;

	if (fabs(out) > comp_th && comp_gain > comp_att) {
	    comp_gain -= comp_att;
	} else if (comp_gain < 1.0f) {
	    comp_gain = comp_gain + (1.0f - comp_gain) * comp_rel;
	}

	if (out > limiter_max) {
	    out = limiter_max;
	} else if (out < -limiter_max) {
	    out = -limiter_max;
	}

	buf[i] = out;
    }
}
#endif

/// Threadable synth function.
/// \param data Audio buffer location.
/// \return Always 0.
static int synth_func(void* data)
{
#if defined(USE_LD)

  // Fill audio data with 0 before doing anything to prevent noise if exceeding wave file length.
  {
    uint8_t* outbuf = static_cast<uint8_t*>(data);

    for(size_t ii = 0; (ii < INTRO_LENGTH_BYTES); ++ii)
    {
      outbuf[ii] = 0;
    }
  }

#if defined(SYNTH_TEST_INPUT_FILE)
  const fs::path REL_PATH("rel");
  const fs::path SYNTH_TEST_INPUT_FILE_PATH(SYNTH_TEST_INPUT_FILE);
  SDL_AudioSpec wav_spec;
  SDL_AudioSpec* wav_found;
  Uint32 wav_length;
  Uint8 *wav_buffer;

  wav_found = SDL_LoadWAV(SYNTH_TEST_INPUT_FILE_PATH.string().c_str(), &wav_spec, &wav_buffer, &wav_length);
  if(!wav_found)
  {
    fs::path fname = REL_PATH / SYNTH_TEST_INPUT_FILE_PATH;
    wav_found = SDL_LoadWAV(fname.string().c_str(), &wav_spec, &wav_buffer, &wav_length);
  }
  if(!wav_found)
  {
    fs::path fname = fs::path("..") / REL_PATH / SYNTH_TEST_INPUT_FILE_PATH;
    wav_found = SDL_LoadWAV(fname.string().c_str(), &wav_spec, &wav_buffer, &wav_length);
  }

  // If successful, read floating-point data from file.
  if(wav_found)
  {
    memcpy(data, wav_buffer, std::min(wav_length, static_cast<Uint32>(INTRO_LENGTH_BYTES)));
    SDL_FreeWAV(wav_buffer);
  }
  // No no audio data available, fill data with 0.
  else
#endif
  {
    std::cerr << "WARNING: synth not available, filling audio with 0" << std::endl;
  }

#else

  const int SAMPLE_COUNT = INTRO_LENGTH_BYTES / sizeof(sample_t);
#if 1
  synth(reinterpret_cast<float*>(data), SAMPLE_COUNT);
  audio_master_fx(reinterpret_cast<float*>(data), SAMPLE_COUNT);
#else
  uint8_t* outbuf = static_cast<uint8_t*>(data);
  for(size_t ii = 0; (ii < INTRO_LENGTH_BYTES); ++ii)
  {
    outbuf[ii] = 0;
  }
#endif

  // Write raw .wav file if necessary.
#if defined(SYNTH_TEST_OUTPUT_FILE)
  {
    SF_INFO info;
    info.samplerate = AUDIO_SAMPLERATE;
    info.channels = AUDIO_CHANNELS;
    info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

    SNDFILE *outfile = dnload_sf_open(SYNTH_TEST_OUTPUT_FILE, SFM_WRITE, &info);
    sf_count_t write_count = SAMPLE_COUNT / AUDIO_CHANNELS;
    dnload_sf_writef_float(outfile, reinterpret_cast<float*>(data), write_count);
    dnload_sf_close(outfile);
  }
#endif
#endif

  return 0;
}

/// SDL audio specification struct.
static SDL_AudioSpec audio_spec =
{
  AUDIO_SAMPLERATE,
  AUDIO_SAMPLE_TYPE_SDL,
  AUDIO_CHANNELS,
  0,
#if defined(USE_LD)
  4096,
#else
  256, // ~172.3Hz, lower values seem to cause underruns
#endif
  0,
  0,
  audio_callback,
  NULL
};

//######################################
// Generic code include ################
//######################################

#include "verbatim_cond.hpp"
#include "verbatim_font.hpp"
#include "verbatim_frame_buffer.hpp"
#include "verbatim_image_2d_gray.hpp"
#include "verbatim_image_2d_rgb.hpp"
#include "verbatim_image_3d_gray.hpp"
#include "verbatim_image_3d_rgb.hpp"
#include "verbatim_image_cube_gray.hpp"
#include "verbatim_image_cube_rgb.hpp"
#include "verbatim_image_cube_rgba.hpp"
#include "verbatim_mat2.hpp"
#include "verbatim_mat3.hpp"
#include "verbatim_opt.hpp"
#include "verbatim_png.hpp"
#include "verbatim_spline.hpp"
#include "verbatim_texture_3d.hpp"
#include "verbatim_texture_cube.hpp"

//######################################
// Code dependant on generic code ######
//######################################

/// Generates a random direction.
///
/// This equals random point in ball surface.
/// Use algorithm from Marsaglia (1972).
/// See: http://mathworld.wolfram.com/SpherePointPicking.html 
///
/// \return dir Normalized direction vector.
static vec3 random_direction()
{
  float x1;
  float x2;
  float sqr1;
  float sqr2;

  for(;;)
  {
    x1 = frand(-1.0f, 1.0f);
    x2 = frand(-1.0f, 1.0f);
    sqr1 = x1 * x1;
    sqr2 = x2 * x2;

    // Acceptable point, can stop randomizing.
    if(dnload_sqrtf(sqr1 + sqr2) < 1.0f)
    {
      break;
    }
  }

  float root1 = dnload_sqrtf(1.0f - sqr1 - sqr2);
  float px = 2.0f * x1 * root1;
  float py = 2.0f * x2 * root1;
  float pz = 1.0f - 2.0f * (sqr1 + sqr2);

  return vec3(px, py, pz);
}

//######################################
// Shaders #############################
//######################################

#include "header.glsl.hpp" // g_shader_header

#include "fluid.vert.glsl.hpp" // g_shader_vertex_fluid
#include "fluid.frag.glsl.hpp" // g_shader_fragment_fluid

#include "precalc.vert.glsl.hpp" // g_shader_vertex_precalc
#include "precalc.frag.glsl.hpp" // g_shader_fragment_precalc

#include "font.vert.glsl.hpp" // g_shader_vertex_font
#include "font.frag.glsl.hpp" // g_shader_fragment_font

#include "clouds.frag.glsl.hpp" // g_shader_fragment_clouds

#include "clouds_post.frag.glsl.hpp" // g_shader_fragment_clouds_post

#include "huygens.vert.glsl.hpp" // g_shader_vertex_huygens
#include "huygens.frag.glsl.hpp" // g_shader_fragment_huygens

#include "huygens_post.vert.glsl.hpp" // g_shader_vertex_huygens_post
#include "huygens_post.frag.glsl.hpp" // g_shader_fragment_huygens_post

#include "enceladus.frag.glsl.hpp" // g_shader_fragment_enceladus

#include "simple.frag.glsl.hpp" // g_shader_fragment_simple

#include "simple_post.frag.glsl.hpp" // g_shader_fragment_simple_post

#include "space.vert.glsl.hpp" // g_shader_vertex_space
#include "space.frag.glsl.hpp" // g_shader_fragment_space

#include "space_post.vert.glsl.hpp" // g_shader_vertex_space_post
#include "space_post.frag.glsl.hpp" // g_shader_fragment_space_post

/// Fixed uniform location.
static const GLint g_uniform_array = 0;

/// Fixed uniform location.
static const GLint g_uniform_saturn_bands = 5;

/// Fixed uniform location.
static const GLint g_uniform_saturn_rings = 6;

/// Fixed uniform location.
static const GLint g_uniform_noise_soft = 7;

/// Fixed uniform location.
static const GLint g_uniform_noise_volume_hq = 8;

/// Fixed uniform location.
static const GLint g_uniform_noise_volume_lq = 9;

/// Fixed uniform location.
static const GLint g_uniform_space = 9;

/// Fixed uniform location.
static const GLint g_uniform_rotation = 10;

/// Fixed uniform location.
static const GLint g_uniform_enceladus = 11;
/// Fixed uniform location.
static const GLint g_uniform_tethys = 13;
/// Fixed uniform location.
static const GLint g_uniform_trail = 14;

/// Fixed uniform location.
static const GLint g_uniform_fbo = 5;

/// Fixed uniform location.
static const GLint g_uniform_fluid_boundary = 6;

/// Fixed uniform location.
static const GLint g_uniform_fluid_input = 7;

/// Fixed uniform location.
static const GLint g_uniform_glyph = 5;

/// Fixed uniform location.
static const GLint g_uniform_glyph_rectangle = 6;

/// Fixed uniform location.
static const GLint g_uniform_glyph_color = 7;

/// Fixed uniform location.
static const GLint g_uniform_glyph_inverse_color = 8;

/// Fixed uniform location.
static const GLint g_uniform_glyph_phase = 9;

/// Fixed uniform location.
static const GLint g_uniform_distort = 10;

/// Pipeline information.
class Pipeline
{
  private:
#if defined(USE_LD)
    GlslProgram m_program;
#endif
    /// Vertex program.
    GLuint m_program_vert;
    /// Fragment program.
    GLuint m_program_frag;
    /// Pipeline.
    GLuint m_pipeline;

  public:
    /// Constructor.
    Pipeline(const char* header, const char* vert, const char* frag)
    {
#if defined(USE_LD)
      m_program.addShader(GL_VERTEX_SHADER, header, vert);
      m_program.addShader(GL_FRAGMENT_SHADER, header, frag);
      if(!link())
      {
        std::ostringstream sstr;
        sstr << "pipeline creation failure: " << getName();
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
      m_program_vert = m_program.getPipelineId(GL_VERTEX_SHADER);
      m_program_frag = m_program.getPipelineId(GL_FRAGMENT_SHADER);
      m_pipeline = m_program.getPipelineId();
#else
      const GLchar* shader_source_array[] = { header, vert };
      m_program_vert = dnload_glCreateShaderProgramv(GL_VERTEX_SHADER, 2, shader_source_array);

      shader_source_array[1] = frag;
      m_program_frag = dnload_glCreateShaderProgramv(GL_FRAGMENT_SHADER, 2, shader_source_array);

      dnload_glGenProgramPipelines(1, &m_pipeline);
      dnload_glUseProgramStages(m_pipeline, 1, m_program_vert);
      dnload_glUseProgramStages(m_pipeline, 2, m_program_frag);
#endif
    }

  public:
#if defined(USE_LD)
    /// Accessor.
    ///
    /// \return Program name string.
    std::string getName() const
    {
      return m_program.getName();
    }

    /// Link the program.
    bool link()
    {
      return m_program.link(true);
    }
#endif

    /// Bind this pipeline for use.
    void bind() const
    {
      dnload_glBindProgramPipeline(m_pipeline);
    }

    /// Send uniform.
    /// \param location Uniform location.
    /// \param value Uniform value.
    void uniformFrag(GLuint location, GLfloat value) const
    {
      dnload_glProgramUniform1f(m_program_frag, location, value);
    }
    /// Send uniform.
    /// \param location Uniform location.
    /// \param value Uniform value.
    void uniformVert(GLuint location, GLfloat value) const
    {
      dnload_glProgramUniform1f(m_program_vert, location, value);
    }

    /// Send uniform.
    /// \param location Uniform location.
    /// \param value Uniform value.
    void uniformFrag(GLuint location, GLint value) const
    {
      dnload_glProgramUniform1i(m_program_frag, location, value);
    }
    /// Send uniform and bind texture.
    /// \param location Uniform location.
    /// \param value Uniform value.
    void uniformFrag(GLuint location, unsigned int value, const Texture& tex) const
    {
      tex.bind(value);
      uniformFrag(location, static_cast<GLint>(value));
    }
    /// Send uniform.
    /// \param location Uniform location.
    /// \param value Uniform value.
    void uniformVert(GLuint location, GLint value) const
    {
      dnload_glProgramUniform1i(m_program_vert, location, value);
    }

    /// Send uniform.
    /// \param location Uniform location.
    /// \param value Uniform value array.
    void uniformFrag2fv(GLuint location, GLsizei count, const GLfloat* value) const
    {
      dnload_glProgramUniform2fv(m_program_frag, location, count, value);
    }
    /// Send uniform.
    /// \param location Uniform location.
    /// \param value Uniform value array.
    void uniformVert2fv(GLuint location, GLsizei count, const GLfloat* value) const
    {
      dnload_glProgramUniform2fv(m_program_vert, location, count, value);
    }

    /// Send uniform.
    /// \param location Uniform location.
    /// \param count Number of vec3's in the array.
    /// \param value Uniform value array.
    void uniformFrag3fv(GLuint location, GLsizei count, const GLfloat* value) const
    {
      dnload_glProgramUniform3fv(m_program_frag, location, count, value);
    }
    /// Send uniform wrapper.
    /// \param location Uniform location.
    /// \param value Uniform value array.
    void uniformFrag3fv(GLuint location, const vec3& value) const
    {
      uniformFrag3fv(location, 1, value.getData());
    }

    /// Send uniform.
    /// \param location Uniform location.
    /// \param value Uniform value array.
    void uniformVert3fv(GLuint location, GLsizei count, const GLfloat* value) const
    {
      dnload_glProgramUniform3fv(m_program_vert, location, count, value);
    }

    /// Send uniform.
    /// \param location Uniform location.
    /// \param value Uniform value array.
    void uniformVert4fv(GLuint location, GLsizei count, const GLfloat* value) const
    {
      dnload_glProgramUniform4fv(m_program_vert, location, count, value);
    }

    /// Send uniform.
    /// \param location Uniform location.
    /// \param value Uniform value array.
    void uniformVert(GLuint location, const mat3& value, bool transpose = false) const
    {
      dnload_glProgramUniformMatrix3fv(m_program_vert, location, 1, static_cast<GLboolean>(transpose),
          value.getData());
    }

    /// Send uniform.
    /// \param location Uniform location.
    /// \param value Uniform value array.
    void uniformFrag(GLuint location, const mat3& value, bool transpose = false) const
    {
      dnload_glProgramUniformMatrix3fv(m_program_frag, location, 1, static_cast<GLboolean>(transpose),
          value.getData());
    }
};

//######################################
// Non-generic code include ############
//######################################

#include "global_data.hpp"

//######################################
// Drawing #############################
//######################################

#if defined(USE_LD)

/// Debug mode position.
static vec3 g_pos(0.0f, 0.0f, 0.0f);

/// Debug mode forward.
static vec3 g_fw(0.0f, 0.0f, -1.0f);

/// Debug mode up.
static vec3 g_up(0.0f, 1.0f, 0.0f);

/// Rotation matrix.
static mat3 g_rotation_matrix(-0.99f, -0.16f, 0.02f, 0.14f, -0.77f, 0.63f, -0.08f, 0.62f, 0.78f);

#endif

/// Set frame settings to uniform array.
///
/// \param uniform_array Uniform array to set to.
/// \param frame Direction frame.
static void setFrameUniforms(float* uniform_array, const DirectionFrame& frame)
{
  uniform_array[0] = frame.getPosition().x();
  uniform_array[1] = frame.getPosition().y();
  uniform_array[2] = frame.getPosition().z();
  uniform_array[3] = frame.getForward().x();
  uniform_array[4] = frame.getForward().y();
  uniform_array[5] = frame.getForward().z();
  uniform_array[6] = frame.getUp().x();
  uniform_array[7] = frame.getUp().y();
  uniform_array[8] = frame.getUp().z();
  uniform_array[12] = static_cast<float>(frame.getSceneTime());
}

/// Feed distorts to a shader.
///
/// \param pipeline Pipeline to feed to.
/// \param data Global data instance.
/// \param ticks Total ticks at this time.
void draw_send_distort(const Pipeline& pipeline, const GlobalData& data, int ticks)
{
  // Prepare distorts.
  static const int DISTORT_COUNT = 96;
  float distorts[DISTORT_COUNT * 2];

  for(int ii = 0; (ii < DISTORT_COUNT); ++ii)
  {
    int idx = ticks + ii;
    distorts[ii * 2 + 0] = data.getDistort(idx);
    distorts[ii * 2 + 1] = static_cast<float>(idx % 2);
  }

  pipeline.uniformFrag2fv(g_uniform_distort, DISTORT_COUNT, distorts);
}

/// Draw text blurb array.
///
/// \param blurb Blurb array.
/// \param data Global data instance.
/// \param uniform_array Default uniform array.
/// \param col Text color.
/// \param inv_col Text inverse color.
/// \param fs Font size.
/// \param centered True to render centered.
/// \param offset Offset to add for text.
/// \param rand_offset Random per-text offset.
/// \param scene_time Current scene time.
/// \param start_time Blurb start time.
/// \param duration Blurb duration.
void draw_text_blurb_array(const TextLocation* blurb, const GlobalData& data, const float* uniform_array,
    const vec3& col, const vec3& inv_col, float fs, bool centered, const vec2& offset, const vec2 rand_offset,
    int scene_time, int start_time, int duration, int ticks)
{
  vgl::blend_mode(vgl::PREMULTIPLIED);


  const Pipeline& pipeline_font = data.getPipelineFont();
  pipeline_font.bind();

  pipeline_font.uniformVert3fv(g_uniform_array, 5, uniform_array);
  pipeline_font.uniformFrag3fv(g_uniform_glyph_color, col);
  pipeline_font.uniformFrag3fv(g_uniform_glyph_inverse_color, inv_col);
  draw_send_distort(pipeline_font, data, ticks);

  for(const TextLocation* tl = blurb; tl->txt; ++tl)
  {
    int text_time = scene_time - tl->stamp - start_time;

    if((text_time > 0) && (text_time < duration))
    {
      dnload_srand(static_cast<unsigned>(text_time / 200));

      float phase = static_cast<float>(text_time) / static_cast<float>(duration);
      float px = static_cast<float>(tl->px) * 0.01f + offset.x() + frand(1.0f) * rand_offset.x();
      float py = static_cast<float>(tl->py) * 0.01f + offset.y() + frand(1.0f) * rand_offset.y();

      //std::cout << "phase: " << phase << std::endl;

      if(centered)
      {
        data.drawTextCentered(g_uniform_glyph_rectangle, g_uniform_glyph_phase, g_uniform_glyph, px, py,
            fs, phase, tl->txt);
      }
      else
      {
        data.drawText(g_uniform_glyph_rectangle, g_uniform_glyph_phase, g_uniform_glyph, px, py,
            fs, phase, tl->txt);
      }
    }
  }

  vgl::blend_mode(vgl::DISABLED);
}

/// Draw the world.
///
/// \param ticks Tick count (ms).
/// \param aspec Screen aspect.
static void draw(int ticks, GlobalData& data)
{
  // Advance with ticks if split scene is over.
  if(ticks > (DIRECTION_SPLIT_START + DIRECTION_SPLIT_DURATION))
  {
    ticks += DIRECTION_SPLIT_DURATION;
  }

  // Uniform data.
  // 0: X position.
  // 1: Y position.
  // 2: Z position.
  // 3: X forward.
  // 4: Y forward.
  // 5: Z forward.
  // 6: X up.
  // 7: Y up.
  // 8: Z up.
  // 9: Screen width.
  // 10: Screen height.
  // 11: FoV.
  // 12: Time (ms) in this scene.
  // 13: Time (ms) in intro overall.
  // 14: Scene-specific.
  float uniform_array[15];

  DirectionFrame frame = data.resolveDirectionFrame(ticks);
  setFrameUniforms(uniform_array, frame);
  uniform_array[9] = static_cast<float>(data.getScreenWidth());
  uniform_array[10] = static_cast<float>(data.getScreenHeight());
  uniform_array[11] = data.getFov();
  uniform_array[13] = static_cast<float>(ticks);
  uniform_array[14] = -1.0f;

#if defined(USE_LD)
  if(!g_flag_developer || g_direction_lock)
  {
    g_pos = vec3(0.0f, 0.0f, 0.0f);
    g_fw = normalize(vec3(uniform_array[3], uniform_array[4], uniform_array[5]));
    g_up = normalize(vec3(uniform_array[6], uniform_array[7], uniform_array[8]));
  }
  else if(g_flag_developer)
  {
    uniform_array[0] += g_pos.x();
    uniform_array[1] += g_pos.y();
    uniform_array[2] += g_pos.z();
    uniform_array[3] = g_fw.x();
    uniform_array[4] = g_fw.y();
    uniform_array[5] = g_fw.z();
    uniform_array[6] = g_up.x();
    uniform_array[7] = g_up.y();
    uniform_array[8] = g_up.z();
  }
#endif

#if 0
    std::cout << "pos: " << vec3(uniform_array[0], uniform_array[1], uniform_array[2]) << std::endl <<
      "fw: " << vec3(uniform_array[3], uniform_array[4], uniform_array[5]) << std::endl <<
      "up: " << vec3(uniform_array[6], uniform_array[7], uniform_array[8]) << std::endl;
#endif

  // Prepare distorts.
  static const int DISTORT_COUNT = 96;
  float distorts[DISTORT_COUNT * 2];
  for(int ii = 0; (ii < DISTORT_COUNT); ++ii)
  {
    int idx = ticks + ii;
    distorts[ii * 2 + 0] = data.getDistort(idx);
    distorts[ii * 2 + 1] = static_cast<float>(idx % 2);
  }

  // Huygens sketches.
  if((HUYGENS_SKETCH_01 <= frame.getScene()) && (HUYGENS_SKETCH_15 >= frame.getScene()))
  {
    vec3 offset = data.getOffset(ticks) * 0.000004f;
    uniform_array[0] += offset.x();
    uniform_array[1] += offset.y();
    uniform_array[14] = static_cast<float>(frame.getScene());

    // Render.
    data.getFbo().bind();
    {
      const Pipeline& pipeline_huygens = data.getPipelineHuygens();
      pipeline_huygens.bind();

      pipeline_huygens.uniformVert3fv(g_uniform_array, 5, uniform_array);
      pipeline_huygens.uniformFrag3fv(g_uniform_array, 5, uniform_array);

      dnload_glRects(-1, -1, 1, 1);
    }

    vgl::blend_mode(vgl::PREMULTIPLIED);

    // Text.
    if(frame.getScene() == HUYGENS_SKETCH_15)
    {
      const int CREDITS_BLURB_START = 0;
      const int CREDITS_BLURB_DURATION = 2000;
      const float CREDITS_FONT_SIZE = 0.13f;
      int scene_time = frame.getSceneTime();

      draw_text_blurb_array(g_blurb_credits, data, uniform_array, vec3(1.0f),
          vec3(2.0f, 1.0f, 0.1f), CREDITS_FONT_SIZE, true, vec2(-offset.x(), -offset.y()), vec2(0.0f),
          scene_time, CREDITS_BLURB_START, CREDITS_BLURB_DURATION, ticks);

      // Röllers.
      draw_text_blurb_array(g_blurb_credits_roell, data, uniform_array, vec3(1.0f),
          vec3(2.0f, 1.0f, 0.1f), 0.07f, true, vec2(-offset.x(), -offset.y()), vec2(0.0f), scene_time,
          CREDITS_BLURB_START, 85, ticks);
    }
    else
    {
      const Pipeline& pipeline_font = data.getPipelineFont();
      pipeline_font.bind();

      pipeline_font.uniformVert3fv(g_uniform_array, 5, uniform_array);
      pipeline_font.uniformFrag3fv(g_uniform_glyph_color, vec3(0.0f));
      pipeline_font.uniformFrag3fv(g_uniform_glyph_inverse_color, vec3(1.0f));
      draw_send_distort(pipeline_font, data, ticks);

      int text_index = static_cast<int>(frame.getScene()) - static_cast<int>(HUYGENS_SKETCH_01);
      data.drawTextCentered(g_uniform_glyph_rectangle, g_uniform_glyph_phase, g_uniform_glyph, -offset.x(),
          -offset.y(), 0.2f, 0.5f, g_sketch_text[text_index]);
    }

    vgl::blend_mode(vgl::DISABLED);

    // Blit.
    data.bindDefaultFrameBuffer();
    {
      const Pipeline& pipeline_huygens_post = data.getPipelineHuygensPost();
      pipeline_huygens_post.bind();

      pipeline_huygens_post.uniformVert3fv(g_uniform_array, 5, uniform_array);
      pipeline_huygens_post.uniformFrag3fv(g_uniform_array, 5, uniform_array);

      pipeline_huygens_post.uniformFrag(g_uniform_fbo, 0, data.getFbo().getTextureColor());
      pipeline_huygens_post.uniformFrag(g_uniform_noise_soft, 1, data.getTextureNoiseSoft());
      draw_send_distort(pipeline_huygens_post, data, ticks);

      dnload_glRects(-1, -1, 1, 1);
    }
  }
  // Space.
  else if(SPACE == frame.getScene())
  {
    GLshort quad[] = { -1, -1, 1, 1 };
    unsigned draw_iteration = 0;

    while(draw_iteration < 2)
    {
      bool in_split = ((ticks >= DIRECTION_SPLIT_START) && (ticks < DIRECTION_SPLIT_END));

      if(in_split)
      {
        if(draw_iteration < 1)
        {
          quad[2] = 0;
          uniform_array[14] = 0.5f;
        }
        else
        {
          frame = data.resolveDirectionFrame(ticks + DIRECTION_SPLIT_SCENE_SPLIT_START);
          setFrameUniforms(uniform_array, frame);
          quad[0] = 0;
          quad[2] = 1;
        }
      }

      // Render.
      data.getFbo().bind();
      {
        const Pipeline& pipeline_space = data.getPipelineSpace();
        pipeline_space.bind();

        pipeline_space.uniformVert3fv(g_uniform_array, 5, uniform_array);
        pipeline_space.uniformFrag3fv(g_uniform_array, 5, uniform_array);

        pipeline_space.uniformFrag(g_uniform_saturn_bands, 0, data.getTextureSaturnBands());
        pipeline_space.uniformFrag(g_uniform_saturn_rings, 1, data.getTextureSaturnRings());
        pipeline_space.uniformFrag(g_uniform_noise_soft, 2, data.getTextureNoiseSoft());
        pipeline_space.uniformFrag(g_uniform_noise_volume_hq, 3, data.getTextureNoiseVolumeHq());
        pipeline_space.uniformFrag(g_uniform_noise_volume_lq, 4, data.getTextureNoiseVolumeLq());
        pipeline_space.uniformFrag(g_uniform_enceladus, 5, data.getTextureEnceladus());
        pipeline_space.uniformFrag(g_uniform_tethys, 6, data.getTextureTethys());

        dnload_glRects(quad[0], quad[1], quad[2], quad[3]);
      }

      if(in_split)
      {
        ++draw_iteration;
      }
      else
      {
        break;
      }
    }

    // Text blurb.
    {
      int BLURB_DURATION = 1300;
      int CASSINI_BLURB_START = 53200;
      int scene_time = frame.getSceneTime();
      float BLURB_FONT_SIZE = 0.05f;

      if((scene_time >= CASSINI_BLURB_START) && (scene_time < (CASSINI_BLURB_START + BLURB_DURATION + 1000)))
      {
        dnload_srand(static_cast<unsigned>(scene_time / 300));
        float omul = frand(0.1f);

        draw_text_blurb_array(g_blurb_cassini, data, uniform_array, vec3(0.95f, 0.77f, 0.63f),
            vec3(1.6f, 1.0f, 0.2f), BLURB_FONT_SIZE, true, vec2(0.0f), vec2(omul, 0.0f), scene_time,
            CASSINI_BLURB_START, BLURB_DURATION, ticks);

        draw_text_blurb_array(g_blurb_cassini_why, data, uniform_array, vec3(0.95f, 0.77f, 0.63f),
            vec3(1.6f, 1.0f, 0.2f), BLURB_FONT_SIZE * 1.5f, true, vec2(0.0f), vec2(0.0f), scene_time,
            CASSINI_BLURB_START, BLURB_DURATION, ticks);
      }
    }

    // Blit.
    data.bindDefaultFrameBuffer();
    {
      const Pipeline& pipeline_space_post = data.getPipelineSpacePost();
      pipeline_space_post.bind();

      pipeline_space_post.uniformVert3fv(g_uniform_array, 5, uniform_array);
      pipeline_space_post.uniformFrag3fv(g_uniform_array, 5, uniform_array);

      pipeline_space_post.uniformFrag(g_uniform_fbo, 0, data.getFbo().getTextureColor());
      pipeline_space_post.uniformFrag(g_uniform_noise_soft, 1, data.getTextureNoiseSoft());
      pipeline_space_post.uniformFrag(g_uniform_space, 2, data.getTextureSpace());

      dnload_glRects(-1, -1, 1, 1);
    }
  }
  // Simple space.
  else if(SIMPLE == frame.getScene())
  {
    // Render.
    data.getFbo().bind();
    {
      const Pipeline& pipeline_simple = data.getPipelineSimple();
      pipeline_simple.bind();

      pipeline_simple.uniformVert3fv(g_uniform_array, 5, uniform_array);
      pipeline_simple.uniformFrag3fv(g_uniform_array, 5, uniform_array);

      pipeline_simple.uniformFrag(g_uniform_saturn_bands, 0, data.getTextureSaturnBands());
      pipeline_simple.uniformFrag(g_uniform_saturn_rings, 1, data.getTextureSaturnRings());
      pipeline_simple.uniformFrag(g_uniform_noise_soft, 2, data.getTextureNoiseSoft());
      pipeline_simple.uniformFrag(g_uniform_noise_volume_hq, 3, data.getTextureNoiseVolumeHq());
      pipeline_simple.uniformFrag(g_uniform_trail, 4, data.getTextureTrail());

      dnload_glRects(-1, -1, 1, 1);
    }

    // Texts.
    {
      int GREETS_DURATION = 3000;
      float GREETS_FONT_SIZE = 0.12f;
  
      draw_text_blurb_array(g_blurb_greets, data, uniform_array, vec3(0.95f, 1.0f, 0.92f),
          vec3(2.0f, 1.0f, 0.1f), GREETS_FONT_SIZE, false, vec2(0.0f), vec2(0.0f), frame.getSceneTime(), 0,
          GREETS_DURATION, ticks);
    }

    // Blit.
    data.bindDefaultFrameBuffer();
    {
      const Pipeline& pipeline_simple_post = data.getPipelineSimplePost();
      pipeline_simple_post.bind();

      pipeline_simple_post.uniformVert3fv(g_uniform_array, 5, uniform_array);

      pipeline_simple_post.uniformFrag(g_uniform_fbo, 0, data.getFbo().getTextureColor());
      pipeline_simple_post.uniformFrag(g_uniform_noise_soft, 1, data.getTextureNoiseSoft());
      pipeline_simple_post.uniformFrag(g_uniform_noise_volume_hq, 2, data.getTextureNoiseVolumeHq());
      pipeline_simple_post.uniformFrag(g_uniform_space, 3, data.getTextureSpace());
      pipeline_simple_post.uniformFrag2fv(g_uniform_distort, DISTORT_COUNT, distorts);

      dnload_glRects(-1, -1, 1, 1);
    }
  }
  // Enceladus.
  else if(ENCELADUS == frame.getScene())
  {
    // Render.
    data.getFbo().bind();
    {
      const Pipeline& pipeline_enceladus = data.getPipelineEnceladus();
      pipeline_enceladus.bind();

      pipeline_enceladus.uniformVert3fv(g_uniform_array, 5, uniform_array);
      pipeline_enceladus.uniformFrag3fv(g_uniform_array, 5, uniform_array);

      pipeline_enceladus.uniformFrag(g_uniform_saturn_bands, 0, data.getTextureSaturnBands());
      pipeline_enceladus.uniformFrag(g_uniform_saturn_rings, 1, data.getTextureSaturnRings());
      pipeline_enceladus.uniformFrag(g_uniform_noise_soft, 2, data.getTextureNoiseSoft());
      pipeline_enceladus.uniformFrag(g_uniform_noise_volume_hq, 3, data.getTextureNoiseVolumeHq());
      pipeline_enceladus.uniformFrag(g_uniform_enceladus, 4, data.getTextureEnceladusSurface());

      dnload_glRects(-1, -1, 1, 1);
    }

    // Blit.
    data.bindDefaultFrameBuffer();
    {
      const Pipeline& pipeline_space_post = data.getPipelineSpacePost();
      pipeline_space_post.bind();

      pipeline_space_post.uniformVert3fv(g_uniform_array, 5, uniform_array);

      pipeline_space_post.uniformFrag(g_uniform_fbo, 0, data.getFbo().getTextureColor());
      pipeline_space_post.uniformFrag(g_uniform_noise_soft, 1, data.getTextureNoiseSoft());
      pipeline_space_post.uniformFrag(g_uniform_space, 2, data.getTextureSpace());

      dnload_glRects(-1, -1, 1, 1);
    }
  }
  // Enceladus.
  else if(CLOUDS == frame.getScene())
  {
    if(frame.getSceneTime() >= 14000)
    {
      vec3 offset = data.getOffset(ticks) * 0.006f;

      if(frame.getSceneTime() > 27000)
      {
        offset *= 0.3f;
      }

      uniform_array[3] += offset.x();
      uniform_array[4] += offset.y();
      uniform_array[5] += offset.z();
    }

    mat3 rotation_matrix;
#if defined(USE_LD)
    if(g_flag_developer && !g_direction_lock)
    {
      rotation_matrix = g_rotation_matrix;
    }
    else
#endif
    {
      rotation_matrix = mat3::rotation_euler(0.6f + static_cast<float>(frame.getSceneTime()) * 0.000008f,
          -0.8f + static_cast<float>(frame.getSceneTime()) * 0.000006f,
          3.1f - static_cast<float>(frame.getSceneTime()) * 0.0000005f);
    }

    // Render.
    data.getFboLq().bind();
    {
      const Pipeline& pipeline_clouds = data.getPipelineClouds();
      pipeline_clouds.bind();

      pipeline_clouds.uniformVert3fv(g_uniform_array, 5, uniform_array);
      pipeline_clouds.uniformFrag3fv(g_uniform_array, 5, uniform_array);

      pipeline_clouds.uniformFrag(g_uniform_rotation, rotation_matrix);

      pipeline_clouds.uniformFrag(g_uniform_saturn_rings, 0, data.getTextureSaturnRings());
      pipeline_clouds.uniformFrag(g_uniform_noise_soft, 1, data.getTextureNoiseSoft());
      pipeline_clouds.uniformFrag(g_uniform_noise_volume_hq, 2, data.getTextureNoiseVolumeHq());
      pipeline_clouds.uniformFrag(g_uniform_noise_volume_lq, 3, data.getTextureNoiseVolumeLq());

      dnload_glRects(-1, -1, 1, 1);
    }

    // Blit.
    data.bindDefaultFrameBuffer();
    {
      const Pipeline& pipeline_clouds_post = data.getPipelineCloudsPost();
      pipeline_clouds_post.bind();

      pipeline_clouds_post.uniformVert3fv(g_uniform_array, 5, uniform_array);
      pipeline_clouds_post.uniformFrag3fv(g_uniform_array, 5, uniform_array);

      pipeline_clouds_post.uniformFrag(g_uniform_fbo, 0, data.getFboLq().getTextureColor());
      pipeline_clouds_post.uniformFrag(g_uniform_noise_soft, 1, data.getTextureNoiseSoft());
      pipeline_clouds_post.uniformFrag(g_uniform_space, 2, data.getTextureSpace());

      dnload_glRects(-1, -1, 1, 1);
    }
  }

#if defined(USE_LD) || (defined(EXTRA_GLGETERROR) && (EXTRA_GLGETERROR != 0))
  vgl::error_check();
#endif
}

/// Inner fluidsim draw.
/// \param control Control value.
/// \param data Global data.
/// \param src FBO being read from.
/// \param dst FBO being written to.
/// \param input_tex Texture for input.
void draw_fluid_inner(int control, GlobalData& data, const FrameBuffer& src, const FrameBuffer& dst,
                      const Texture& input_tex)
{
  dst.bind();

  const Pipeline& pipeline_fluid = data.getPipelineFluid();
  pipeline_fluid.bind();

  pipeline_fluid.uniformFrag(g_uniform_array, control);
  pipeline_fluid.uniformFrag(g_uniform_fbo, 0, src.getTextureColor());
  pipeline_fluid.uniformFrag(g_uniform_fluid_boundary, 1, data.getTextureFluidBoundary());
  pipeline_fluid.uniformFrag(g_uniform_fluid_input, 2, input_tex);

  dnload_glRects(-1, -1, 1, 1);
}

#if defined(USE_LD)
static int g_fluid_debug = 0;
#endif

/// Shows fluid status with debug shader.
///
/// \param data Global data instance.
void draw_fluid_show(const Texture& tex, GlobalData& data)
{
#if defined(USE_LD)
  static int last_fluid_show = g_fluid_debug;
  if(g_fluid_debug != last_fluid_show)
  {
    std::cout << "fluid debug: " << g_fluid_debug << std::endl;
    last_fluid_show = g_fluid_debug;
  }
#else
  const int g_fluid_debug = 0;
#endif

  data.bindDefaultFrameBuffer();
  {
    const Pipeline& pipeline_precalc = data.getPipelinePrecalc();
    pipeline_precalc.bind();

    pipeline_precalc.uniformFrag(g_uniform_array, g_fluid_debug);
    pipeline_precalc.uniformFrag(g_uniform_fbo, 0, tex);

    dnload_glRects(-1, -1, 1, 1);
  }
  swap_buffers();
}

#if defined(USE_LD)
/// Set to 1 for extra fluid debugging.
#define FLUID_EXTRA_DEBUG 0
#endif

/// Draw fluidsim.
///
/// \param control Current fluid control.
/// \param show_dye Show dye instead of control fluid.
/// \param phase Current fluid phase.
/// \param data Global data instance.
/// \return Next phase for main fluid framebuffers.
int draw_fluid(bool update_fluid, bool show_dye, int control, int phase, int dye_phase, GlobalData& data)
{
  // Initial phase. Copy starting input to fluid FBO, copy dye input to dye FBO.
  if(control == 0)
  {
    {
      const FrameBuffer& src = data.getFluidFbo(phase);
      const FrameBuffer& dst = data.getFluidFbo(1 - phase);

      draw_fluid_inner(65535, data, src, dst, data.getTextureFluidInput());
    }

    {
      const FrameBuffer& src = data.getFluidDyeFbo(dye_phase);
      const FrameBuffer& dst = data.getFluidDyeFbo(1 - dye_phase);

      draw_fluid_inner(65535, data, src, dst, data.getTextureSaturnBands());
    }

    phase = 1 - phase;
    dye_phase = 1 - dye_phase;
  }

  if(update_fluid && (control != 0))
  {
    const int PROJECT_COUNT = 20;

    // Normal operation, run phases.
    for(int cc = 0; (cc <= PROJECT_COUNT + 4); ++cc)
    {
#if defined(FLUID_EXTRA_DEBUG) && FLUID_EXTRA_DEBUG
      const Texture* tex = 0;
#endif

      // Add inflow
      if(cc == 0)
      {
        const FrameBuffer& src = data.getFluidFbo(phase);
        const FrameBuffer& dst = data.getFluidFbo(1 - phase);

        draw_fluid_inner(cc, data, src, dst, data.getTextureFluidInput());

        phase = 1 - phase;

#if defined(FLUID_EXTRA_DEBUG) && FLUID_EXTRA_DEBUG
        tex = &(data.getFluidFbo(phase).getTextureColor());
#endif
      }
      // Build pressure.
      else if(cc == 1)
      {
        const FrameBuffer& src = data.getFluidFbo(phase);
        const FrameBuffer& dst = data.getFluidPressureFbo();

        draw_fluid_inner(cc, data, src, dst, data.getTextureFluidInput());

#if defined(FLUID_EXTRA_DEBUG) && FLUID_EXTRA_DEBUG
        tex = &(data.getFluidPressureFbo().getTextureColor());
#endif
      }
      // Project.
      else if((cc >= 2) && (cc < PROJECT_COUNT + 2))
      {
        while(cc < PROJECT_COUNT + 2)
        {
          const FrameBuffer& src = data.getFluidFbo(phase);
          const FrameBuffer& dst = data.getFluidFbo(1 - phase);

          draw_fluid_inner(cc, data, src, dst, data.getFluidPressureFbo().getTextureColor());

          phase = 1 - phase;

#if defined(FLUID_EXTRA_DEBUG) && FLUID_EXTRA_DEBUG
          tex = &(data.getFluidFbo(phase).getTextureColor());
#endif
          ++cc;
        }
        --cc;
      }
      // Dye advect.
      else if(cc == PROJECT_COUNT + 3)
      {
        const FrameBuffer& src = data.getFluidDyeFbo(dye_phase);
        const FrameBuffer& dst = data.getFluidDyeFbo(1 - dye_phase);

        draw_fluid_inner(cc, data, src, dst, data.getFluidFbo(phase).getTextureColor());

        dye_phase = 1 - dye_phase;

#if defined(FLUID_EXTRA_DEBUG) && FLUID_EXTRA_DEBUG
        tex = &(data.getFluidDyeFbo(dye_phase).getTextureColor());
#endif
      }
      // Default (apply pressure, advect).
      else
      {
        const FrameBuffer& src = data.getFluidFbo(phase);
        const FrameBuffer& dst = data.getFluidFbo(1 - phase);

        draw_fluid_inner(cc, data, src, dst, data.getFluidPressureFbo().getTextureColor());

        phase = 1 - phase;

#if defined(FLUID_EXTRA_DEBUG) && FLUID_EXTRA_DEBUG
        tex = &(data.getFluidFbo(phase).getTextureColor());
#endif
      }

#if defined(FLUID_EXTRA_DEBUG) && FLUID_EXTRA_DEBUG
      std::cout << "fluidsim_phase = " << cc << std::endl;
      {
        bool quit = false;
        while(!quit)
        {
          SDL_Event event;
          while(SDL_PollEvent(&event))
          {
            if(SDL_KEYDOWN == event.type)
            {
              switch(event.key.keysym.sym)
              {
                case SDLK_1:
                  g_fluid_debug = 1;
                  break;
                case SDLK_2:
                  g_fluid_debug = 2;
                  break;
                case SDLK_3:
                  g_fluid_debug = 3;
                  break;
                case SDLK_4:
                  g_fluid_debug = 4;
                  break;
                case SDLK_5:
                  g_fluid_debug = 5;
                  break;
                case SDLK_6:
                  g_fluid_debug = 6;
                  break;
                case SDLK_7:
                  g_fluid_debug = 7;
                  break;
                case SDLK_8:
                  g_fluid_debug = 8;
                  break;
                case SDLK_9:
                  g_fluid_debug = 9;
                  break;
                case SDLK_0:
                  g_fluid_debug = 0;
                  break;
                case SDLK_RETURN:
                  quit = true;
                  break;
                case SDLK_F5:
                  {
                    opt<Pipeline*> failed_pipeline = data.linkPipelines();
                    if(failed_pipeline)
                    {
                      std::ostringstream sstr;
                      sstr << "pipeline recreation failure: " << (*failed_pipeline)->getName();
                      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
                    }
                  }
                  break;
                default:
                  break;
              }
            }
          }
          draw_fluid_show(*tex, data);
          SDL_Delay(FRAME_MILLISECONDS);
        }
      }
#endif
    }
  }

#if defined(USE_LD)
  if(show_dye)
#endif
  {
    const Texture& tex = data.getFluidDyeFbo(dye_phase).getTextureColor();
    draw_fluid_show(tex, data);
  }
#if defined(USE_LD)
  else
  {
    const Texture& tex = data.getFluidFbo(phase).getTextureColor();
    draw_fluid_show(tex, data);
  }
#endif

#if defined(USE_LD) || (defined(EXTRA_GLGETERROR) && (EXTRA_GLGETERROR != 0))
  vgl::error_check();
#endif

  return phase;
}

//######################################
// Fluid simulation ####################
//######################################

// TODO: kaikki fluidsim paska tähän.
void AddInflow(Image2DRGBA &buf, unsigned x, unsigned y, unsigned w, unsigned h, const vec2& vel, float color)
{
  for (unsigned ii = x; ii < x + w; ii++)
  {
    for (unsigned jj = y; jj < y + h; jj++)
    {
      buf.setPixel(ii, jj, vel.x(), vel.y(), 0.0f, color);
    }
  }
}

//######################################
// Utility #############################
//######################################

#if defined(USE_LD)

/// Parse resolution from string input.
///
/// \param op Resolution string.
/// \return Tuple of width and height.
boost::tuple<unsigned, unsigned> parse_resolution(const std::string &op)
{
  size_t cx = op.find("x");
  
  if(std::string::npos == cx)
  {
    cx = op.rfind("p");

    if((std::string::npos == cx) || (0 >= cx))
    {
      std::ostringstream sstr;
      sstr << "invalid resolution string '" << op << '\'';
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }

    std::string sh = op.substr(0, cx);

    unsigned rh = boost::lexical_cast<unsigned>(sh);
    unsigned rw = (rh * 16) / 9;
    unsigned rem4 = rw % 4;

    return boost::make_tuple(rw - rem4, rh);
  }

  std::string sw = op.substr(0, cx);
  std::string sh = op.substr(cx + 1);

  return boost::make_tuple(boost::lexical_cast<int>(sw), boost::lexical_cast<int>(sh));
}

/// \brief Audio writing callback.
///
/// \param data Raw audio data.
/// \param size Audio data size (in samples).
void write_audio(void *data, size_t size)
{
  FILE *fd = fopen("intro.raw", "wb");

  if(fd != NULL)
  {
    fwrite(data, size, 1, fd);
  }

  fclose(fd);
  return;
}

/// \brief Image writing callback.
///
/// \param screen_w Screen width.
/// \param screen_h Screen height.
/// \param idx Frame index to write.
void write_frame(unsigned screen_w, unsigned screen_h, int idx)
{
  boost::scoped_array<uint8_t> image(new uint8_t[screen_w * screen_h * 3]);
  std::ostringstream sstr;

  glReadPixels(0, 0, static_cast<GLsizei>(screen_w), static_cast<GLsizei>(screen_h), GL_RGB, GL_UNSIGNED_BYTE,
      image.get());

  sstr << "intro_" << std::setfill('0') << std::setw(4) << idx << ".png";

  gfx::image_png_save(sstr.str(), screen_w, screen_h, 24, image.get());
  return;
}

#endif

//######################################
// intro / _start ######################
//######################################

#if defined(USE_LD)
/// \brief Intro body function.
///
/// \param screen_w Screen width.
/// \param screen_h Screen height.
/// \param flag_fullscreen Fullscreen toggle.
/// \param flag_record Record toggle.
/// \return Return value for program.
void intro(unsigned screen_w, unsigned screen_h, bool flag_fullscreen, bool flag_record)
#else
#define screen_w static_cast<unsigned>(SCREEN_W)
#define screen_h static_cast<unsigned>(SCREEN_H)
#define flag_fullscreen static_cast<bool>(SCREEN_F)
void _start()
#endif
{
  dnload();
  dnload_SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  g_sdl_window = dnload_SDL_CreateWindow(NULL, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      static_cast<int>(screen_w), static_cast<int>(screen_h),
      SDL_WINDOW_OPENGL | (flag_fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
  dnload_SDL_GL_CreateContext(g_sdl_window);
  dnload_SDL_ShowCursor(g_flag_developer);

#if defined(USE_LD)
  {
    GLenum err = glewInit();
    if(GLEW_OK != err)
    {
      std::ostringstream sstr;
      sstr << "glewInit(): " << glewGetErrorString(err);
      BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
    }
  }
#endif

  GlobalData global_data(screen_w, screen_h, 0.55f);

  // Initial states.
  vgl::blend_mode(vgl::DISABLED);

  // Begin precalc loop.
  {
#if 0
    SimulationSpace ss(FLUID_W, FLUID_H, 1.0f, 1.0f, 1.0f);

    Image2DRGBA img(FLUID_W, FLUID_H);
    {
      // TODO: init here
      for(unsigned ii = 0; (ii < img.getWidth()); ++ii)
      {
        for(unsigned jj = 0; (jj < img.getHeight()); ++jj)
        {
          img.setPixel(ii, jj, frand(1.0f), frand(1.0f), frand(1.0f), frand(1.0f));
        }
      }
    }
    //ss.TestLerp();
    ss.TestAdvectInitialCondition();
    Texture2D tex;
    tex.update(img);
#endif

    Image2DRGBA boundary(FLUID_WIDTH, FLUID_HEIGHT);
    Image2DRGBA initial(FLUID_WIDTH, FLUID_HEIGHT);
    // Create fluid textures.
    {
      for(unsigned ii = 0; ii < FLUID_HEIGHT; ii++)
      {
        boundary.setPixel(0, ii, 1, 0.5f, 0.0f, 1.0f);
        boundary.setPixel(FLUID_WIDTH - 1, ii, 0.0f, 0.5f, 0.0f, 1.0f);
      }
      // Inside, have everything return the same pixel, with 1.0 mul
      for(unsigned ii = 1; ii < FLUID_WIDTH - 1; ii++)
      {
        for(unsigned jj = 0; jj < FLUID_HEIGHT; jj++)
        {
          boundary.setPixel(ii, jj, 0.5f, 0.5f, 1.0f, 1.0f);
        }
      }

      // Initial condition and iterative too
      AddInflow(initial, 0, 0, FLUID_WIDTH, FLUID_HEIGHT, vec2(0.0f, 0.0f), 1.0f); // clear
      //AddInflow(initial, FLUID_WIDTH*0.45, 0.2*FLUID_HEIGHT, FLUID_WIDTH*0.1, 1, vec2(0.0f, 3.0f), 0.0f);

      dnload_srand(0);
      // Large amount of 1-pixel disturbances
      for(unsigned ii = 0; ii < FLUID_WIDTH; ii++)
      {
        AddInflow(initial, ii, urand(FLUID_HEIGHT), 1, 1, vec2(0.0f, 3.0f), 0.0f);
      }
      //AddInflow(initial, 64, 64, 1, 1, vec2(0.0f, 3.0f), 0.0f);
            
      /*
      //(0, 0) corner
      boundary.setPixel(0, 0, 1.0, 1.0, 0.0, 0.0);
      //(0, MAX) corner
      boundary.setPixel(0, FLUID_HEIGHT-1, 1.0, 0.0, 0.0, 0.0);
      //(MAX, 0) corner
      boundary.setPixel(FLUID_WIDTH-1, 0, 0.0, 1.0, 0.0, 0.0);
      //(MAX, MAX) corner
      boundary.setPixel(FLUID_WIDTH-1, FLUID_HEIGHT-1, 0.0, 0.0, 0.0, 0.0);
      // Left and Right boundary
      for(unsigned ii = 1; ii < FLUID_HEIGHT-1; ii++)
      {
        boundary.setPixel(0, ii, 1, 0.5, 0.0, 1.0);
        boundary.setPixel(FLUID_WIDTH-1, ii, 0.0, 0.5, 0.0, 1.0);
      }
      // Top and Bottom boundary
      for(unsigned ii = 1; ii < FLUID_WIDTH - 1; ii++)
      {
        boundary.setPixel(ii, 0, 0.5, 1, 1.0, 0.0);
        boundary.setPixel(ii, FLUID_HEIGHT - 1, 0.5, 0.0, 1.0, 0.0);
      }

      // Inside, have everything return the same pixel, with 1.0 mul
      for(unsigned ii = 1; ii < FLUID_WIDTH - 1; ii++)
      {
        for(unsigned jj = 1; jj < FLUID_HEIGHT - 1; jj++)
        {
          boundary.setPixel(ii, jj, 0.5, 0.5, 1.0, 1.0);
        }
      }

      // Initial state.
      for(unsigned ii = 0; (ii < FLUID_WIDTH); ++ii)
      {
        for(unsigned jj = 0; (jj < FLUID_WIDTH); ++jj)
        {
          initial.setPixel(ii, jj, 0.0, 0.0, 0.0, 0.0);
        }
      }

      // Initial state.
      for (unsigned ii = 900; (ii < 1000); ++ii)
      {
        for (unsigned jj = 900; (jj < 1000); ++jj)
        {
          initial.setPixel(ii, jj, 1.0, 1.0, 1.0, 1.0);
        }
      }*/
      /*for (unsigned ii = 0; (ii < FLUID_WIDTH); ++ii)
      {
        for (unsigned jj = 0; (jj < FLUID_HEIGHT); ++jj)
        {
          initial.setPixel(ii, jj, 0.0f, (static_cast<float>(ii) / (FLUID_WIDTH - 1) - 0.5f), 0.0f, 1.0f);
        }
      }*/
      
      /*for (unsigned ii = 0; ii < FLUID_WIDTH; ++ii)
      {
        for (unsigned jj = 0; jj < FLUID_HEIGHT; ++jj)
        {
          float vx = frand(-1.0f, 1.0f);
          float vy = frand(-1.0f, 1.0f);
          initial.setPixel(ii, jj, vx, vy, 0.0f, 0.0);
        }
      }*/
      global_data.updateFluid(boundary, initial);
    }

    // Offload precalc to other threads.
    Thread thr_init(GlobalData::initialize_func, &global_data);
    Thread thr_synth(synth_func, g_audio_buffer);

    int prev_ticks = get_current_ticks();
    int fluid_phase = 0;
    int dye_phase = 0;
    int fluid_control = 0;
    int fluid_frame = 0;
    bool quit = false;
#if defined(USE_LD)
    bool show_dye = true;
    bool update_fluid = true;
#else
    const bool show_dye = true;    
    const bool update_fluid = true;
#endif
    while(!global_data.isDone() || !quit)
    {
#if defined(USE_LD)
      // Loop until something should be done.
      {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
          if(SDL_KEYDOWN == event.type)
          {
            switch(event.key.keysym.sym)
            {
              case SDLK_1:
                g_fluid_debug = 1;
                break;
              case SDLK_2:
                g_fluid_debug = 2;
                break;
              case SDLK_3:
                g_fluid_debug = 3;
                break;
              case SDLK_4:
                g_fluid_debug = 4;
                break;
              case SDLK_5:
                g_fluid_debug = 5;
                break;
              case SDLK_6:
                g_fluid_debug = 6;
                break;
              case SDLK_7:
                g_fluid_debug = 7;
                break;
              case SDLK_8:
                g_fluid_debug = 8;
                break;
              case SDLK_9:
                g_fluid_debug = 9;
                break;
              case SDLK_0:
                g_fluid_debug = 0;
                break;
              case SDLK_TAB:
                show_dye = !show_dye;
                break;
              case SDLK_SPACE:
                update_fluid = !update_fluid;
                break;

              case SDLK_F5:
                {
                  opt<Pipeline*> failed_pipeline = global_data.linkPipelines();
                  if(failed_pipeline)
                  {
                    std::ostringstream sstr;
                    sstr << "pipeline recreation failure: " << (*failed_pipeline)->getName();
                    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
                  }
                }
                break;

              case SDLK_ESCAPE:
                quit = true;
                break;

              default:
                break;
            }
          }
          else if(SDL_KEYUP == event.type)
          {
            switch(event.key.keysym.sym)
            {
              case SDLK_SPACE:
                update_fluid = false;
                break;

              default:
                break;
            }
          }
        }
      }
#endif

      // Wait until next display.
      for(;;)
      {
        int curr_ticks = get_current_ticks();

        if((curr_ticks - prev_ticks) >= FRAME_MILLISECONDS)
        {
          prev_ticks = curr_ticks;
          break;
        }
        dnload_SDL_Delay(0);
      }

      // Update fluid simulation.
      {
        fluid_phase = draw_fluid(update_fluid, show_dye, fluid_control, fluid_phase, dye_phase, global_data);
        if(update_fluid || (fluid_control == 0))
        {
          dye_phase = 1 - dye_phase;

          // Capture fluid frame.
          if(fluid_frame == FLUID_CAPTURE_FRAME)
          {
            // Capture data.
            uarr<uint8_t> capture_array(FLUID_WIDTH * FLUID_HEIGHT * 4);
            global_data.getFluidDyeFbo(dye_phase).bind();
            dnload_glReadPixels(0, 0, FLUID_WIDTH, FLUID_HEIGHT, GL_RGBA, GL_BYTE, capture_array.get());

            // Write data to image.
            Image2DRGBA capture_image(FLUID_WIDTH, FLUID_HEIGHT);
            for(unsigned ii = 0; (ii < FLUID_WIDTH); ++ii)
            {
              for(unsigned jj = 0; (jj < FLUID_HEIGHT); ++jj)
              {
                // Texture looks ok in fluidsim, but shit on actual saturn without multiply [jaguar.jpg].
                unsigned idx = (jj * FLUID_WIDTH + ii) * 4;
                float rr = static_cast<float>(capture_array[idx + 0]) * (1.0f / 255.0f);
                float gg = static_cast<float>(capture_array[idx + 1]) * (1.0f / 255.0f);
                float bb = static_cast<float>(capture_array[idx + 2]) * (1.0f / 255.0f);
                float aa = 1.0f;
                capture_image.setPixel(ii, jj, rr, gg, bb, aa);
              }
            }
            global_data.updateSaturnBands(capture_image);
            quit = true; // Can exit precalc now.
          }

          ++fluid_frame;
        }
        fluid_control = 1;
      }

      // May need to do partial update if necessary.
      if(global_data.hasPendingUpdate())
      {
        global_data.updatePartial();
      }
    }
  }

  // Perform remaining updates to GPU.
  global_data.update();

#if defined(USE_LD)
  if(flag_record)
  {
    SDL_Event event;
    int frame_idx = 0;

    // audio
    SDL_PauseAudio(1);

    // video
    for(;;)
    {
      int ticks = frame_idx * FRAME_MILLISECONDS;

      if(ticks > INTRO_LENGTH_TICKS)
      {
        break;
      }

      while(SDL_PollEvent(&event))
      {
        if(is_quit_event(event))
        {
          break;
        }
      }

      draw(ticks, global_data);
      write_frame(screen_w, screen_h, frame_idx);
      swap_buffers();
      ++frame_idx;
    }

    dnload_SDL_Quit();
    return;
  }
#endif

#if defined(USE_LD)
  if(!g_flag_developer)
  {
    SDL_OpenAudio(&audio_spec, NULL);
    SDL_PauseAudio(0);
  }
#else
  dnload_SDL_OpenAudio(&audio_spec, NULL);
  dnload_SDL_PauseAudio(0);
#endif

  int prev_ticks = get_current_ticks();
  int playback_ticks = INTRO_START * 1000;

  for(;;)
  {
#if defined(USE_LD)
    static const float MOVE_SPEED_BASE = 10.0f;
    static float move_speed = MOVE_SPEED_BASE;
    static unsigned successful_frames = 0;
    static uint8_t mouse_look = 0;
    static int8_t move_backward = 0;
    static int8_t move_down = 0;
    static int8_t move_forward = 0;
    static int8_t move_left = 0;
    static int8_t move_right = 0;
    static int8_t move_up = 0;
    static int8_t time_delta = g_flag_developer ? 0 : 1;
    static int8_t time_speed = 1;
    int tick_add = 0;
    int mouse_look_x = 0;
    int mouse_look_y = 0;
    bool ignore_frameskip = false;
    bool quit = false;
#else
    static const int time_delta = 1;
    static const int time_speed = 1;
#endif
    SDL_Event event;

#if defined(USE_LD)
    while(SDL_PollEvent(&event))
    {
      if(SDL_QUIT == event.type)
      {
        quit = true;
      }
      else if(SDL_KEYDOWN == event.type)
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_a:
            move_left = 1;
            break;

          case SDLK_d:
            move_right = 1;
            break;

          case SDLK_e:
            move_up = 1;
            break;

          case SDLK_q:
            move_down = 1;
            break;

          case SDLK_s:
            move_backward = 1;
            break;

          case SDLK_w:
            move_forward = 1;
            break;

          case SDLK_LSHIFT:
          case SDLK_RSHIFT:
            move_speed = MOVE_SPEED_BASE * 10.0f;
            time_speed = 8;
            break;

          case SDLK_LALT:
            time_delta = -1;
            break;

          case SDLK_MODE:
          case SDLK_RALT:
            time_delta = 1;
            break;

          case SDLK_RETURN:
            g_direction_lock = true;
            break;

          case SDLK_LEFT:
            tick_add = -5000;
            ignore_frameskip = true;
            break;
          case SDLK_RIGHT:
            tick_add = 5000;
            ignore_frameskip = true;
            break;
          case SDLK_UP:
            tick_add = 30000;
            ignore_frameskip = true;
            break;
          case SDLK_DOWN:
            tick_add = -30000;
            ignore_frameskip = true;
            break;

          case SDLK_SPACE:
            {
              // Advance with ticks if split scene is over.
              int resolve_ticks = playback_ticks;
              if(resolve_ticks > (DIRECTION_SPLIT_START + DIRECTION_SPLIT_DURATION))
              {
                resolve_ticks += DIRECTION_SPLIT_DURATION;
              }
              DirectionFrame frame = global_data.resolveDirectionFrame(resolve_ticks);
              vec3 pos = frame.getPosition() + g_pos;
              vec3 eye(pos + g_fw * 1000.0f);
              vec3 up = g_up * 1000.0f;

              std::cout << "pos: " << iround(pos.x()) << " ; " << iround(pos.y()) << " ; " <<
                iround(pos.z()) << "\neye: " << iround(eye.x()) << " ; " << iround(eye.y()) << " ; " <<
                iround(eye.z()) << "\nup: " << iround(up.x()) << " ; " << iround(up.y()) << " ; " <<
                iround(up.z()) << "\nticks: " << playback_ticks << std::endl;
            }
            break;

          case SDLK_INSERT:
            {
              dnload_srand(SDL_GetTicks() + static_cast<unsigned>(time(0)));
              float PI2 = static_cast<float>(M_PI * 2.0);
              float rx = frand(PI2);
              float ry = frand(PI2);
              float rz = frand(PI2);

              g_rotation_matrix = mat3::rotation_euler(rx, ry, rz);

              std::cout << "rot: " << rx << " ; " << ry << " ; " << rz << "\n" << g_rotation_matrix <<
                std::endl;
            }
            break;

          case SDLK_F5:
            {
              opt<Pipeline*> failed_pipeline = global_data.linkPipelines();
              if(failed_pipeline)
              {
                std::ostringstream sstr;
                sstr << "pipeline recreation failure: " << (*failed_pipeline)->getName();
                BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
              }
            }
            break;

          case SDLK_ESCAPE:
            quit = true;
            break;

          default:
            break;
        }
      }
      else if(SDL_KEYUP == event.type)
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_a:
            move_left = 0;
            break;

          case SDLK_d:
            move_right = 0;
            break;

          case SDLK_e:
            move_up = 0;
            break;

          case SDLK_q:
            move_down = 0;
            break;

          case SDLK_s:
            move_backward = 0;
            break;

          case SDLK_w:
            move_forward = 0;
            break;

          case SDLK_LSHIFT:
          case SDLK_RSHIFT:
            move_speed = MOVE_SPEED_BASE;
            time_speed = 1;
            break;            

          case SDLK_MODE:
          case SDLK_LALT:
          case SDLK_RALT:
            time_delta = 0;
            break;

          default:
            break;
        }
      }
      else if(SDL_MOUSEBUTTONDOWN == event.type)
      {
        if(1 == event.button.button)
        {
          mouse_look = 1;
          g_direction_lock = false;
        }
      }
      else if(SDL_MOUSEBUTTONUP == event.type)
      {
        if(1 == event.button.button)
        {
          mouse_look = 0;
        }
      }
      else if(SDL_MOUSEMOTION == event.type)
      {
        if(0 != mouse_look)
        {
          mouse_look_x += event.motion.xrel;
          mouse_look_y += event.motion.yrel;
        }
      }
    }

    if(g_flag_developer)
    {
      float movement_rt = static_cast<float>(move_right - move_left) * move_speed;
      float movement_up = static_cast<float>(move_up - move_down) * move_speed;
      float movement_fw = static_cast<float>(move_forward - move_backward) * move_speed;

      g_up = normalize(g_up);
      g_fw = normalize(g_fw);
      vec3 rt = cross(g_fw, g_up);

      if(0 != mouse_look_x)
      {
        float angle = static_cast<float>(mouse_look_x) / static_cast<float>(screen_h);
        float ca = cosf(angle);
        float sa = sinf(angle);
        vec3 new_rt = (ca * rt) + (sa * g_fw);
        vec3 new_fw = (ca * g_fw) - (sa * rt);

        rt = new_rt;
        g_fw = new_fw;
      }
      if(0 != mouse_look_y)
      {
        float angle = static_cast<float>(mouse_look_y) / static_cast<float>(screen_h);
        float ca = cosf(angle);
        float sa = sinf(angle);
        vec3 new_fw = (ca * g_fw) + (sa * g_up);
        vec3 new_up = (ca * g_up) - (sa * g_fw);

        g_fw = new_fw;
        g_up = new_up;
      }

      g_pos += (movement_rt * rt) + (movement_up * g_up) + (movement_fw * g_fw);
    }

    // Clamp time if on developer mode.
    if(g_flag_developer)
    {
      playback_ticks = std::max(std::min(INTRO_LENGTH_TICKS, playback_ticks), 0);
    }
    
    if((playback_ticks > static_cast<int>(INTRO_LENGTH_TICKS)) || quit)
    {
      break;
    }
#else
    dnload_SDL_PollEvent(&event);

    if((playback_ticks > INTRO_LENGTH_TICKS) || is_quit_event(event))
    {
      break;
    }
#endif

#if defined(USE_LD)
    // If ignoring frameskip, we have jumped in time. Reset EVERYTHING.
    if(ignore_frameskip)
    {
      playback_ticks = std::min(std::max(playback_ticks + tick_add, 0), INTRO_LENGTH_TICKS);
      prev_ticks = get_current_ticks();

      // Set next audio position. Effective next audio frame. Error is negligible and does not drift.
      int next_apos = static_cast<int>((static_cast<double>(playback_ticks) / 1000.0) *
          static_cast<double>(AUDIO_BYTERATE));
      g_next_audio_position = next_apos - (next_apos % static_cast<int>(AUDIO_CHANNELS * sizeof(sample_t)));

      draw(static_cast<int>(playback_ticks), global_data);
      swap_buffers();
    }
    else
#endif
    {
      // First check, discard frame if we're too far (2 frames) behind.
      {
        int tick_diff = get_current_ticks() - prev_ticks;
        if((FRAME_MILLISECONDS * 3) < tick_diff)
        {
          prev_ticks += FRAME_MILLISECONDS;
          playback_ticks += time_delta * FRAME_MILLISECONDS;
#if defined(USE_LD)
          std::cout << "frameskip(" << successful_frames << "): " << (tick_diff - (FRAME_MILLISECONDS * 3)) <<
            std::endl;
          successful_frames = 0;
#endif
          continue;
        }
#if defined(USE_LD)
        else
        {
          ++successful_frames;
        }
#endif
      }

      draw(static_cast<int>(playback_ticks), global_data);
      swap_buffers();

      // Second check, wait until we can proceed (less than one frame left).
      for(;;)
      {
        int tick_diff = get_current_ticks() - prev_ticks;
        if(FRAME_MILLISECONDS <= tick_diff)
        {
          break;
        }
        dnload_SDL_Delay(0);
      }

      // Advance a frame.
      prev_ticks += FRAME_MILLISECONDS;
      playback_ticks += time_delta * time_speed * FRAME_MILLISECONDS;
    }
  }

  dnload_SDL_Quit();
#if !defined(USE_LD)
  asm_exit();
#endif
}

//######################################
// Main ################################
//######################################

#if defined(USE_LD)
/// Main function.
///
/// \param argc Argument count.
/// \param argv Arguments.
/// \return Program return code.
int DNLOAD_MAIN(int argc, char **argv)
{
  unsigned screen_w = SCREEN_W;
  unsigned screen_h = SCREEN_H;
  bool fullscreen = true;
  bool record = false;

#if !defined(DEBUG)
  try
#endif
  {
    if(argc > 0)
    {
      po::options_description desc("Options");
      desc.add_options()
        ("developer,d", "Developer mode.")
        ("help,h", "Print help text.")
        ("record,R", "Do not play intro normally, instead save frames as .png -files.")
        ("resolution,r", po::value<std::string>(), "Resolution to use, specify as 'WIDTHxHEIGHT' or 'HEIGHTp'.")
        ("window,w", "Start in window instead of full-screen.");

      po::variables_map vmap;
      po::store(po::command_line_parser(argc, argv).options(desc).run(), vmap);
      po::notify(vmap);

      if(vmap.count("developer"))
      {
        g_flag_developer = true;
      }
      if(vmap.count("help"))
      {
        std::cout << usage << desc << std::endl;
        return 0;
      }
      if(vmap.count("record"))
      {
        record = true;
      }
      if(vmap.count("resolution"))
      {
        boost::tie(screen_w, screen_h) = parse_resolution(vmap["resolution"].as<std::string>());
      }
      if(vmap.count("window"))
      {
        fullscreen = false;
      }
    }

    intro(screen_w, screen_h, fullscreen, record);
  }
#if !defined(DEBUG)
  catch(const boost::exception &err)
  {
    std::cerr << boost::diagnostic_information(err);
    return 1;
  }
  catch(const std::exception &err)
  {
    std::cerr << err.what() << std::endl;
    return 1;
  }
  catch(...)
  {
    std::cerr << __FILE__ << ": unknown exception caught\n";
    return -1;
  }
#endif
  return 0;
}
#endif

//######################################
// End #################################
//######################################

