#ifndef CRATER_HPP
#define CRATER_HPP

/// Crater representation for surface.
class Crater
{
  private:
    /// Center of crater (normalized).
    vec3 m_center;

    /// Radius of crater (angle).
    float m_radius;

  public:
    /// Constructor.
    ///
    /// \param dir Crater direction.
    /// \param radius Crater radius.
    Crater(const vec3& dir, float radius) :
      m_center(normalize(dir)),
      m_radius(radius)
    {
    }

  public:
    /// Get relative distance from center of the crater.
    ///
    /// Returned as relative distance [0, 1]. Empty optional will be returned if too far.
    ///
    /// \param dir Direction. Must be normalized.
    /// \return Optional distance.
    opt<float> getDistance(const vec3& dir) const
    {
      float angle = dot(dir, m_center);
      float lower_bound = 1.0f - m_radius;

      if (angle <= lower_bound)
      {
        return opt<float>();
      }
      // Need to clamp due to floating point precision problems.
      float ret = std::min((angle - lower_bound) / m_radius, 1.0f);
      return opt<float>(1.0f - ret);
    }

    /// Accessor.
    ///
    /// \return Radius of this crater.
    float getRadius() const
    {
      return m_radius;
    }
};

#endif
