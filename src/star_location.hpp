#ifndef STAR_LOCATION_HPP
#define STAR_LOCATION_HPP

#include "verbatim_vec3.hpp"

/// Star location.
class StarLocation
{
private:
  /// Star direction.
  vec3 m_dir;

  /// Star direction mapped into cube map.
  vec3 m_mapped;

  /// Star radius (angle).
  float m_radius;

  /// Star luminosity.
  float m_luminosity;

public:
  /// Constructor.
  /// \param dir Direction.
  /// \param radius Radius.
  /// \param luminosity Luminosity.
  explicit StarLocation(const vec3& dir, float radius, float luminosity) :
    m_dir(normalize(dir)),
    m_mapped(calculateMappedDirection(dir)),
    m_radius(radius),
    m_luminosity(luminosity)
  {
  }

public:
  /// Calculate luminosity in regard to this star.
  /// \param dir Direction aimed at.
  /// \return Luminosity.
  float calculateLuminosity(const vec3& dir) const
  {
    float angle = dot(dir, m_dir);
    float lower_bound = 1.0f - m_radius;

    if (angle < lower_bound)
    {
      return 0.0f;
    }
    float strength = (angle - lower_bound) / m_radius;
    return strength * m_luminosity * strength * strength;
  }

  /// Accessor.
  /// \return Mapped direction.
  const vec3& getMappedDirection() const
  {
    return m_mapped;
  }

private:
  /// Convert to mapped direction.
  /// \param dir Direction to map.
  /// \return Mapped direction.
  static vec3 calculateMappedDirection(const vec3& dir)
  {
    float mag_x = std::abs(dir[0]);
    float mag_y = std::abs(dir[1]);
    float mag_z = std::abs(dir[2]);

    if ((mag_x >= mag_y) && (mag_x >= mag_z))
    {
      return vec3((dir[0] >= 0.0f) ? 1.0f : -1.0f,
        dir[1] / mag_x,
        dir[2] / mag_x);
    }
    else if ((mag_y >= mag_x) && (mag_y >= mag_z))
    {
      return vec3(dir[0] / mag_y,
        (dir[1] >= 0.0f) ? 1.0f : -1.0f,
        dir[2] / mag_y);
    }
    return vec3(dir[0] / mag_z,
      dir[1] / mag_z,
      (dir[2] >= 0.0f) ? 1.0f : -1.0f);
  }
};

#endif
