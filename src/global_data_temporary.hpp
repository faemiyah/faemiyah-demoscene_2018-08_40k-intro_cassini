#ifndef GLOBAL_DATA_TEMPORARY_HPP
#define GLOBAL_DATA_TEMPORARY_HPP

#include "saturn_rings_png.h"

#include "crater_map.hpp"
#include "crawler_2d.hpp"
#include "crawler_map.hpp"
#include "star_location_tree.hpp"

//#define DEBUG_FAST_SPACE
//#define DEBUG_FAST_ENCELADUS
//#define DEBUG_FAST_TETHYS

/// Fluid simulation width.
const unsigned FLUID_WIDTH = 2048;
/// Fluid simulation height.
const unsigned FLUID_HEIGHT = FLUID_WIDTH;
/// Frame count at which the fluid is captured.
const int FLUID_CAPTURE_FRAME = 500;

/// Temporary global data container.
class GlobalDataTemporary
{
  private:
    /// Cube map side.
    static const unsigned CUBE_MAP_SIDE = 1440;

    /// Cube map side for moons.
    static const unsigned CUBE_MAP_SIDE_MOON = 2048;

  public:
    /// Noise image.
    Image2DGray noise_2d;
    /// Noise volume (high quality).
    Image3DGray noise_3d_hq;
    /// Noise volume (low quality).
    Image3DGray noise_3d_lq;

    /// Saturn image.
    Image2DRGB saturn_bands;
    /// Saturn image.
    Image2DRGBAUptr saturn_rings;

    /// Enceladus surface image.
    Image2DGray enceladus_surface;

    /// Space image.
    ImageCubeRGBUptr space;
    /// Dione image.
    ImageCubeRGBAUptr dione;
    /// Enceladus image.
    ImageCubeRGBAUptr enceladus;
    /// Rhea image.
    ImageCubeRGBAUptr rhea;
    /// Tethys image.
    ImageCubeRGBAUptr tethys;
    /// Trail image.
    ImageCubeGrayUptr trail;

    /// Stars.
    StarLocationTree star_tree;

    /// Craters for Enceladus.
    CraterMap craters_enceladus;
    /// Crawlers for Enceladus.
    CrawlerMap crawlers_enceladus;
    /// Craters for Tethys.
    CraterMap craters_tethys;

  private:
    /// Condition variable to wait on.
    Cond m_cond;

    /// Mutex guard.
    Mutex m_mutex;

    /// Is the calculation done?
    bool m_done;

    /// Is there an update pending?
    bool m_pending;

  public:
    /// Constructor.
    GlobalDataTemporary() :
      noise_2d(512, 512),
      noise_3d_hq(128, 128, 128),
      noise_3d_lq(64, 64, 64),
      saturn_bands(FLUID_WIDTH, 1),
      enceladus_surface(2048, 2048),
      m_done(false),
      m_pending(false)
    {
      // Saturn's bands need to be complete before anything else.
      func_saturn_bands(this);
    }

  private:
    /// Milky way calculation.
    ///
    /// \param dir Direction being faced.
    /// \return 3-point vector, milky way color in that direction.
    vec3 calculateMilkyWay(const vec3& dir) const
    {
      vec3 milky_fw = normalize(vec3(1.0f, 0.1f, -0.2f));
      if(dot(milky_fw, dir) <= 0.0f)
      {
        return vec3(0.0f);
      }

      vec3 milky_rt = normalize(vec3(0.8f, 0.6f, 0.1f));
      vec3 milky_up = cross(milky_rt, milky_fw);
      milky_rt = cross(milky_fw, milky_up);

      vec2 pos = vec2(dnload_asinf(dot(dir, milky_rt)), dnload_asinf(dot(dir, milky_up)));

      float ratio = smooth_step(0.8f, 0.0f, abs(pos.x()));
      vec3 center = mix(vec3(0.0f), vec3(1.0f, 1.0f, 0.8f), sampleNoise2D(pos * 0.7f) + 0.1f) * smooth_step(0.3f * ratio, 0.1f * ratio, abs(pos.y())) * ratio * 0.5f;
      float ratio2 = sqrtf(1.0f - abs(pos.x() * 1.57f));
      vec3 overlay = mix(vec3(0.0f), vec3(0.8f, 0.8f, 1.0f), sampleNoise2D(pos + 0.2f) + 0.1f) * smooth_step(0.2f * ratio2, 0.1f * ratio2, abs(pos.y())) * ratio2 * 0.5f;

      return mix(max(center, vec3(0.0f)), max(overlay, vec3(0.0f)), 0.5f);
    }

    /// Sample noise in 2D
    ///
    /// Generates a kind-of perlin output.
    ///
    /// \param pos Position to sample from.
    /// \param rot Rotation component (default: identity).
    /// \return Noise value.
    float sampleNoise2D(const vec2& pos, const mat2& rot = mat2::identity()) const
    {
      vec2 pos0 = pos;
      vec2 pos1 = (rot * pos0) * 0.5f;
      vec2 pos2 = (rot * pos1) * 0.5f;
      vec2 pos3 = (rot * pos2) * 0.5f;
      vec2 pos4 = (rot * pos3) * 0.5f;
      vec2 pos5 = (rot * pos4) * 0.5f;
      vec2 pos6 = (rot * pos5) * 0.5f;
      vec2 pos7 = (rot * pos6) * 0.5f;
      vec2 pos8 = (rot * pos7) * 0.5f;

      return noise_2d.sampleLinear(pos0) * 0.1f -
        noise_2d.sampleLinear(pos1) * 0.15f +
        noise_2d.sampleLinear(pos2) * 0.2f -
        noise_2d.sampleLinear(pos3) * 0.25f +
        noise_2d.sampleLinear(pos4) * 0.3f -
        noise_2d.sampleLinear(pos5) * 0.35f +
        noise_2d.sampleLinear(pos6) * 0.4f -
        noise_2d.sampleLinear(pos7) * 0.45f +
        noise_2d.sampleLinear(pos8) * 0.5f;
    }

    /// Sample noise in 3D.
    ///
    /// Generates a kind-of perlin output.
    ///
    /// \param pos Position to sample from.
    /// \param rot Rotation component (default: identity).
    /// \return Noise value.
    float sampleNoise3D(const vec3& pos, const mat3& rot = mat3::identity()) const
    {
      vec3 pos0 = pos;
      vec3 pos1 = rot * (pos0 * 0.5f);
      vec3 pos2 = rot * (pos1 * 0.5f);
      vec3 pos3 = rot * (pos2 * 0.5f);
      vec3 pos4 = rot * (pos3 * 0.5f);
      vec3 pos5 = rot * (pos4 * 0.5f);
      vec3 pos6 = rot * (pos5 * 0.5f);
      vec3 pos7 = rot * (pos6 * 0.5f);
      vec3 pos8 = rot * (pos7 * 0.5f);

      return noise_3d_hq.sampleLinear(pos0) * 0.1f -
        noise_3d_hq.sampleLinear(pos1) * 0.15f +
        noise_3d_hq.sampleLinear(pos2) * 0.2f -
        noise_3d_hq.sampleLinear(pos3) * 0.25f +
        noise_3d_hq.sampleLinear(pos4) * 0.3f -
        noise_3d_hq.sampleLinear(pos5) * 0.35f +
        noise_3d_hq.sampleLinear(pos6) * 0.4f -
        noise_3d_hq.sampleLinear(pos7) * 0.45f +
        noise_3d_hq.sampleLinear(pos8) * 0.5f;
    }

    /// Pause for an update.
    void updatePause()
    {
      m_pending = true;

      m_cond.wait(m_mutex);

      m_pending = false;
    }

  public:
    /// Initialization.
    ///
    /// i.e. perform precalc.
    void initialize()
    {
      ScopedLock guard(m_mutex);

      // Asynchronous calculation for functionality not using random elements.
      Thread thr_saturn_rings(&func_saturn_rings, this);

      // Synchronous calculation for functionality using random elements.
      dnload_srand(1563233668); // Intro visuals rely on this seed for reals.
      func_noise_2d(this);
      func_noise_3d(this);
      func_stars(this);
      func_craters(this);

      // Asynchronous cube map elements, one at a time.
      {
        space = ImageCubeRGB::create(CUBE_MAP_SIDE);
        Thread thr_space(&func_space, this);
      }
      updatePause();

      {
        enceladus = ImageCubeRGBA::create(CUBE_MAP_SIDE_MOON);
        Thread thr_enceladus(&func_enceladus, this);
      }
      updatePause();

      {
        tethys = ImageCubeRGBA::create(CUBE_MAP_SIDE_MOON);
        Thread thr_tethys(&func_tethys, this);
      }
      updatePause();

      {
        trail = ImageCubeGray::create(CUBE_MAP_SIDE_MOON);
        Thread thr_trail(&func_trail, this);
      }
      updatePause();

      m_done = true;
    }

    /// Accessor.
    ///
    /// \return Mutex.
    Mutex& getMutex()
    {
      return m_mutex;
    }

    /// Is there an update pending?
    ///
    /// \return True if yes, false if no.
    bool hasPendingUpdate() const
    {
      return m_pending;
    }

    /// Is calculation done?
    ///
    /// \return True if yes, false if no.
    bool isDone() const
    {
      return m_done;
    }

    /// Signal the intenal condition variable.
    void signal()
    {
      m_cond.signal();
    }

  private:
    /// 2D noise generation function.
    ///
    /// \param global_data Temporary global data.
    /// \return Always 0.
    static int func_noise_2d(void* pdata)
    {
      GlobalDataTemporary* data = static_cast<GlobalDataTemporary*>(pdata);

      data->noise_2d.noise();

      //noise.filterLowpass(3);
      data->noise_2d.normalize(0);

      return 0;
    }

    /// 3D noise generation function.
    ///
    /// \param global_data Temporary global data.
    /// \return Always 0.
    static int func_noise_3d(void* pdata)
    {
      GlobalDataTemporary* data = static_cast<GlobalDataTemporary*>(pdata);

      data->noise_3d_hq.noise();
      data->noise_3d_lq.noise();

      data->noise_3d_hq.normalize(0);
      data->noise_3d_lq.normalize(0);

      return 0;
    }

    /// Saturn bands function.
    ///
    /// \param global_data Temporary global data.
    /// \return Always 0.
    static int func_saturn_bands(void* pdata)
    {
      GlobalDataTemporary* data = static_cast<GlobalDataTemporary*>(pdata);
      const vec3 BRIGHT = vec3(0.9f, 0.8f, 0.5f);
      //const vec3 BRIGHT(1.0f, 0.95f, 0.6f);
      const vec3 DARK = vec3(0.7f, 0.6f, 0.4f);
      /*const vec3 BRIGHT(1.0f, 0.9f, 0.55f);
      const vec3 DARK(0.6f, 0.5f, 0.3f);*/
      const vec3 POLE(0.3f, 0.4f, 0.4f);

      dnload_srand(4);
      float mixratio = frand(1.0f);
      for(unsigned ii = 0, ee = data->saturn_bands.getWidth() - 1; (ii <= ee); ++ii)
      {
        if(frand(1.0f) < 0.05f)
        {
          mixratio = frand(1.0f);
        }
        float fi = static_cast<float>(ii) / static_cast<float>(ee);
        vec3 col = mix(mix(BRIGHT, DARK, mixratio), POLE, abs((fi - 0.5f) * 1.3f));

        data->saturn_bands.setPixel(ii, 0, col);
      }

      data->saturn_bands.filterLowpass(3);

      return 0;
    }

    /// Saturn rings function.
    ///
    /// \param global_data Temporary global data.
    /// \return Always 0.
    static int func_saturn_rings(void* pdata)
    {
      GlobalDataTemporary* data = static_cast<GlobalDataTemporary*>(pdata);

      data->saturn_rings = png_read(g_saturn_rings_png);

      for(unsigned ii = 0; (ii < data->saturn_rings->getWidth()); ++ii)
      {
        float rr = data->saturn_rings->getValue(ii, 0, 0);
        float gg = data->saturn_rings->getValue(ii, 0, 1);
        float bb = data->saturn_rings->getValue(ii, 0, 2);
        float aa = dnload_sqrtf(dnload_sqrtf(smooth_step(0.0f, 2.4f, rgb_to_luma(rr, gg, bb))));

        // Premultiplied.
        data->saturn_rings->setValue(ii, 0, 0, rr * aa);
        data->saturn_rings->setValue(ii, 0, 1, gg * aa);
        data->saturn_rings->setValue(ii, 0, 2, bb * aa);
        data->saturn_rings->setValue(ii, 0, 3, aa);
      }

      return 0;
    }

    /// Stars function.
    ///
    /// \param global_data Temporary global data.
    /// \return Always 0.
    static int func_stars(void* pdata)
    {
      GlobalDataTemporary* data = static_cast<GlobalDataTemporary*>(pdata);
      const unsigned STAR_COUNT = 32768;

      for(unsigned ii = 0; (ii < STAR_COUNT); ++ii)
      {
        vec3 dir = random_direction();
        StarLocation star(dir, 0.0000022f, frand(0.1f, 1.0f));
        data->star_tree.add(star);
      }

      return 0;
    }

    /// Space side calculation.
    ///
    /// \param norm_dir Normalized direction.
    /// \param dir Direction mapped to cube map boundary.
    /// \param cx X coordinate in image.
    /// \param cy Y coordinate in image.
    /// \param img Target image.
    /// \param data Extra data to function.
    static void func_space_side(const vec3& norm_dir, const vec3& dir, unsigned ii, unsigned jj,
        Image2DRGB& img, void* pdata)
    {
      GlobalDataTemporary* data = static_cast<GlobalDataTemporary*>(pdata);

#if defined(DEBUG_FAST_SPACE)
      img.setPixel(ii, jj, 0.0f, 0.0f, 0.0f);
      (void)norm_dir;
      (void)dir;
      (void)data;
#else
      float luminosity = data->star_tree.calculateLuminosity(norm_dir, dir);
      vec3 milky = data->calculateMilkyWay(norm_dir) + vec3(luminosity);

      img.setPixel(ii, jj, milky.x(), milky.y(), milky.z());
#endif
    }

    /// Space calculation.
    ///
    /// \param data Temporary global data.
    /// \return Always 0.
    static int func_space(void* pdata)
    {
      GlobalDataTemporary* data = static_cast<GlobalDataTemporary*>(pdata);
      data->space->calculateDistributed(func_space_side, data);
      return 0;
    }

    /// Craters randomization.
    ///
    /// \param data Temporary global data.
    /// \return Always 0.
    static int func_craters(void* pdata)
    {
      GlobalDataTemporary* data = static_cast<GlobalDataTemporary*>(pdata);

      dnload_srand(3);

      for(unsigned ii = 0; (ii < 160); ++ii)
      {
        vec3 dir = random_direction();
        float csize = frand(0.2f, 1.0f);
        data->craters_enceladus.addCrater(dir, 0.0005f + 0.003f * csize * csize * csize * csize * csize);
      }

      dnload_srand(11);

      for(unsigned ii = 0; (ii < 444); ++ii)
      {
        vec3 pos = random_direction();
        vec3 dir = random_direction();
        float power = 0.25f + frand(0.45f);
        float radius = 0.0025f + frand(0.0015f);
        unsigned lifetime = 64 + urand(900);
        float divergence = frand(0.05f);
        data->crawlers_enceladus.addCrawler(pos, dir, power, radius, 128, lifetime, divergence);
      }

      dnload_srand(15);

      for(unsigned ii = 0; (ii < 155); ++ii)
      {
        vec3 dir = random_direction();
        float csize = frand(0.3f, 1.0f);
        data->craters_tethys.addCrater(dir, 0.001f + 0.02f * csize * csize * csize * csize);
      }
      // Tethys has a massive crater.
      data->craters_tethys.addCrater(vec3(0.0f, 0.5f, 1.0f), 0.06f);

      return 0;
    }

    /// Clear a side.
    static void func_side_clear(const vec3& /*norm_dir*/, const vec3& /*dir*/, unsigned ii, unsigned jj,
        Image2DRGBA& img, void* /*data*/)
    {
      img.setPixel(ii, jj, 0.0f, 0.0f, 0.0f, 0.0f);
    }

    /// Enceladus side calculation.
    ///
    /// \param norm_dir Normalized direction.
    /// \param dir Direction mapped to cube map boundary.
    /// \param cx X coordinate in image.
    /// \param cy Y coordinate in image.
    /// \param img Target image.
    /// \param data Extra data to function.
    static void func_enceladus_side(const vec3& norm_dir, const vec3& dir, unsigned ii, unsigned jj,
        Image2DRGBA& img, void* pdata)
    {
      const mat3 rot(-0.99f, -0.16f, 0.02f, 0.14f, -0.77f, 0.63f, -0.08f, 0.62f, 0.78f);
      const float HEIGHT_MUL_CRAWLER = 0.31f;
      const float HEIGHT_MUL_CRATER = 1.0f;
      const float HEIGHT_MUL_NOISE = 3.13f;
      const float FREQUENCY_MUL_NOISE = 0.27f;
      GlobalDataTemporary* data = static_cast<GlobalDataTemporary*>(pdata);

#if defined(DEBUG_FAST_ENCELADUS)
      float noise_color = 1.0f;
      float blue_diff = 0.0f;
      float height = 1.0f;
      (void)HEIGHT_MUL_CRAWLER;
      (void)HEIGHT_MUL_CRATER;
      (void)HEIGHT_MUL_NOISE;
      (void)FREQUENCY_MUL_NOISE;
      (void)data;
      (void)norm_dir;
      (void)dir;
#else
      float noise_height = data->sampleNoise3D(norm_dir * FREQUENCY_MUL_NOISE, rot) * HEIGHT_MUL_NOISE;
      float crater_height = data->craters_enceladus.getHeight(norm_dir) * HEIGHT_MUL_CRATER;
      float crater_step_abs = smooth_step(-0.61f, 0.0f, -std::abs(crater_height));
      float crater_step_pos = smooth_step(0.0f, 0.005f, crater_height);
      float old_height = img.getValue(ii, jj, 3) * HEIGHT_MUL_CRAWLER;
      float new_height = noise_height + crater_height * (1.0f + crater_step_pos * dnload_tanhf(noise_height * 17.0f) * 0.4f);
      float height = old_height * crater_step_abs + new_height;

      float noise_color = data->sampleNoise3D(dir * FREQUENCY_MUL_NOISE * 1.89f, rot) * 0.3f + 0.65f;
      float noise_color_blue = data->sampleNoise3D(dir * FREQUENCY_MUL_NOISE * 2.73f, rot) * 0.39f + 0.65f;
      float blue_diff = std::abs(noise_color_blue - noise_color);
      blue_diff *= blue_diff;
#endif

      img.setPixel(ii, jj, noise_color, noise_color, noise_color + blue_diff, height);
    }

    /// Tethys side calculation.
    ///
    /// \param norm_dir Normalized direction.
    /// \param dir Direction mapped to cube map boundary.
    /// \param cx X coordinate in image.
    /// \param cy Y coordinate in image.
    /// \param img Target image.
    /// \param data Extra data to function.
    static void func_tethys_side(const vec3& norm_dir, const vec3& dir, unsigned ii, unsigned jj,
        Image2DRGBA& img, void* pdata)
    {
      const mat3 rot(-0.99f, -0.16f, 0.02f, 0.14f, -0.77f, 0.63f, -0.08f, 0.62f, 0.78f);
      const float HEIGHT_MUL_CRATER = 1.0f;
      const float HEIGHT_MUL_NOISE = 0.63f;
      const float FREQUENCY_MUL_NOISE = 0.73f;
      GlobalDataTemporary* data = static_cast<GlobalDataTemporary*>(pdata);

#if defined(DEBUG_FAST_TETHYS)
      float luminance = 1.0f;
      float height = 1.0f;
      (void)HEIGHT_MUL_CRATER;
      (void)HEIGHT_MUL_NOISE;
      (void)FREQUENCY_MUL_NOISE;
      (void)norm_dir;
      (void)dir;
      (void)data;
#else
      float height_craters = data->craters_tethys.getHeight(norm_dir) * HEIGHT_MUL_CRATER;
      float crater_step_pos = smooth_step(0.0f, 0.005f, height_craters);
      float height_noise = data->sampleNoise3D(norm_dir * FREQUENCY_MUL_NOISE, rot) * HEIGHT_MUL_NOISE;
      float height = height_noise + height_craters * (1.0f + crater_step_pos * dnload_tanhf(height_noise * 9.0f));

      float luminance = data->sampleNoise3D(dir * 3.1f, rot) * 0.5f + 0.5f;
#endif

      img.setPixel(ii, jj, luminance, luminance, luminance, height);
    }

    /// Enceladus calculation.
    ///
    /// \param data Temporary global data.
    /// \return Always 0.
    static int func_enceladus(void* pdata)
    {
      GlobalDataTemporary* data = static_cast<GlobalDataTemporary*>(pdata);

      // Carve gorges into 2D data.
      data->enceladus_surface.clear(0, 0.0f);
      dnload_srand(16);
      // Carve crawlers.
      for(unsigned ii = 0; (ii < 2); ++ii)
      {
        float px = frand(1.0f);
        float py = frand(1.0f);
        vec2 pos(px, py);
        float rot = frand(static_cast<float>(M_PI * 2.0));
        vec2 dir(dnload_cosf(rot), dnload_sinf(rot));
        float power = 0.67f + frand(0.33f);
        float radius = 0.04f + frand(0.02f);
        unsigned lifetime = 64 + urand(920);
        float divergence = frand(0.08f);

        Crawler2D crawler(pos, dir, power, radius, lifetime, divergence);
        crawler.carve(data->enceladus_surface, 0.001f, 0);
      }
      data->enceladus_surface.filterLowpass(3);
      data->enceladus_surface.normalize(0);

      // Carve gorges into 3D data.
      data->enceladus->clear(3, 0.0f);
      dnload_srand(4);
      data->crawlers_enceladus.carve(*(data->enceladus));

      // Add other height and color data.
      data->enceladus->calculateDistributed(func_enceladus_side, data);
      data->enceladus->normalizeSides(3);

      return 0;
    }

    /// Tethys calculation.
    ///
    /// \param data Temporary global data.
    /// \return Always 0.
    static int func_tethys(void* pdata)
    {
      GlobalDataTemporary* data = static_cast<GlobalDataTemporary*>(pdata);
      data->tethys->calculateDistributed(func_tethys_side, data);
      data->tethys->normalizeSides(3);
      return 0;
    }

    /// Trail calculation.
    ///
    /// \param data Temporary global data.
    /// \return Always 0.
    static int func_trail(void* pdata)
    {
      GlobalDataTemporary* data = static_cast<GlobalDataTemporary*>(pdata);

      // Carve trail into 3D data.
      dnload_srand(8);
      data->trail->clear(0, 0.0f);
      vec3 pos = random_direction();
      vec3 dir = random_direction();
      Crawler crw(pos, dir, 1.0f, 0.011f, 128, 19000, 0.011f);
      crw.carve(*(data->trail), 0.001f);

      // Add other height and color data.
      data->trail->normalizeSides(0);

      return 0;
    }
};

#endif
