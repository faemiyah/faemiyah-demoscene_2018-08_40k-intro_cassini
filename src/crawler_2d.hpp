#ifndef CRAWLER_2D_HPP
#define CRAWLER_2D_HPP

/// Crawler.
class Crawler2D
{
  private:
    /// Current position.
    vec2 m_pos;

    /// Current direction.
    vec2 m_dir;

    /// Power of single impressions
    float m_power;

    /// Radius of impressions.
    float m_radius;

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
    Crawler2D(const vec2& pos, const vec2& dir, float power, float radius, unsigned lifetime,
        float divergence) :
      m_pos(normalize(pos)),
      m_dir(normalize(dir)),
      m_power(power),
      m_radius(radius),
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
      m_dir = normalize(m_dir + vec2(dx, dy));
    }

  public:
    /// Carve the crawler onto a cube map.
    ///
    /// \param img Cube map image.
    void carve(Image2D& img, float speed, unsigned channel)
    {
      float* value_address = img.getClosestPixelAddress(m_pos, channel);

      while(m_lifetime)
      {
        updateDirection();
        m_pos += m_dir * speed;

        {
          float* newAddress = img.getClosestPixelAddress(m_pos, channel);
          if(newAddress == value_address)
          {
            continue;
          }
          value_address = newAddress;
        }

        float iadv = 1.0f / static_cast<float>(img.getWidth());
        float jadv = 1.0f / static_cast<float>(img.getHeight());

        for(float ii = -m_radius; (ii <= m_radius); ii += iadv)
        {
          for(float jj = -m_radius; (jj <= m_radius); jj += jadv)
          {
            float mul = length(vec2(ii, jj)) / m_radius;
            
            if(mul < 1.0f)
            {
              mul = dnload_powf(dnload_cosf(mul * static_cast<float>(M_PI * 0.5)) * frand(1.0f), 2.0f);

              //std::cout << mul << std::endl;

              vec2 pos = m_pos + vec2(ii, jj);
              float* tmp_address = img.getClosestPixelAddress(pos, channel);
              *tmp_address = std::min(*tmp_address, -m_power * mul);
            }
          }
        }

        --m_lifetime;
      }
    }
};

#endif
