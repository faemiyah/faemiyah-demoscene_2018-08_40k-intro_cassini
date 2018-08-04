#ifndef CRAWLER_HPP
#define CRAWLER_HPP

/// Crawler.
class Crawler
{
  private:
    /// Current position.
    vec3 m_pos;

    /// Current direction.
    vec3 m_dir;

    /// Power of single impressions
    float m_power;

    /// Radius of impressions.
    float m_radius;

    /// Count of impressions.
    unsigned m_count;

    /// Lifetime.
    unsigned m_lifetime;

    /// Divergence.
    float m_divergence;

  public:
    /// Constructor.
    ///
    /// \param pos Initial position.
    /// \param dir Initial direction.
    /// \param power Value decrease per impression.
    /// \param radius Radius of impressions.
    /// \param count Count of impressions.
    /// \param lifetime Iteration count.
    /// \param divergence 
    Crawler(const vec3& pos, const vec3& dir, float power, float radius, unsigned count, unsigned lifetime,
        float divergence) :
      m_pos(normalize(pos)),
      m_dir(normalize(dir)),
      m_power(power),
      m_radius(radius),
      m_count(count),
      m_lifetime(lifetime),
      m_divergence(divergence)
    {
    }

  private:
    /// Update direction in accordance to position.
    void updateDirection()
    {
      float dx = frand(-m_divergence, m_divergence);
      float dy = frand(-m_divergence, m_divergence);
      float dz = frand(-m_divergence, m_divergence);
      vec3 crs = cross(m_dir, m_pos);
      m_dir = normalize(cross(m_pos, crs) + vec3(dx, dy, dz));
    }

  public:
    /// Carve the crawler onto a cube map.
    ///
    /// \param img Cube map image.
    void carve(ImageCubeGray& img, float speed)
    {
      float* value_address = img.getClosestPixelAddress(m_pos, 0);

      while(m_lifetime)
      {
        updateDirection();
        m_pos = normalize(m_pos + m_dir * speed);

        {
          float* newAddress = img.getClosestPixelAddress(m_pos, 0);
          if(newAddress == value_address)
          {
            continue;
          }
          value_address = newAddress;
        }

        const float ROT_MUL = static_cast<float>(M_PI * 2.0) / static_cast<float>(m_count);
        vec3 rt = cross(m_dir, m_pos);
        for(unsigned ii = 0; (ii < m_count); ++ii)
        {
          float rot = static_cast<float>(ii) * ROT_MUL;
          float ci = dnload_cosf(rot);
          float si = dnload_sinf(rot);
          vec3 pos = m_pos + ((si * rt) + (ci * m_dir)) * (m_radius * ((ii % 2) ? 0.5f : 1.0f));

          float* tmp_address = img.getClosestPixelAddress(pos, 0);
          *tmp_address = std::max(*tmp_address, static_cast<float>(m_lifetime));
        }
        *value_address = std::min(*value_address, static_cast<float>(m_lifetime));

        --m_lifetime;
      }
    }

    /// Carve the crawler onto a cube map.
    ///
    /// \param img Cube map image.
    void carve(ImageCubeRGBA& img, float speed)
    {
      float* value_address = img.getClosestPixelAddress(m_pos, 3);

      while(m_lifetime)
      {
        updateDirection();
        m_pos = normalize(m_pos + m_dir * speed);

        {
          float* newAddress = img.getClosestPixelAddress(m_pos, 3);
          if(newAddress == value_address)
          {
            continue;
          }
          value_address = newAddress;
        }

        const float ROT_MUL = static_cast<float>(M_PI * 2.0) / static_cast<float>(m_count);
        vec3 rt = cross(m_dir, m_pos);
        for(unsigned ii = 0; (ii < m_count); ++ii)
        {
          float rot = static_cast<float>(ii) * ROT_MUL;
          float ci = dnload_cosf(rot);
          float si = dnload_sinf(rot);
          vec3 pos = m_pos + ((si * rt) + (ci * m_dir)) * (m_radius * ((ii % 2) ? 0.5f : 1.0f));

          float mul = dnload_powf(dnload_cosf(abs(si) * static_cast<float>(M_PI * 0.5)), 2.0f);

          float* tmp_address = img.getClosestPixelAddress(pos, 3);
          *tmp_address = std::min(*tmp_address, -m_power * mul);
        }
        *value_address = std::min(*value_address, -m_power);

        --m_lifetime;
      }
    }
};

#endif
