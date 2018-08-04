#ifndef CRATER_MAP_HPP
#define CRATER_MAP_HPP

#include "crater.hpp"

/// Map of craters in one context.
class CraterMap
{
  private:
    /// Craters.
    seq<Crater> m_craters;

  private:
    /// Crater form function.
    ///
    /// Goes from 0 (center) to 1 (edge).
    ///
    /// \param op Height value.
    /// \return Height value. 0 is neutral.
    static float crater_func(float op)
    {
      const float RIMPOINT = 0.25f;
      const float WINDOW_SHARPNESS = 20.0f;
      const float DEPTH = 1.0f;

#if defined(USE_LD) && defined(DEBUG)
      if((op < 0.0f) || (op >= 1.0f))
      {
        std::ostringstream sstr;
        sstr << "illegal crater value: " << op;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      // Better crater profile function
      float stepfunction = 0.5f + dnload_tanhf(WINDOW_SHARPNESS * (op - RIMPOINT)) * 0.5f;
      //float stepfunction = 1.0f / (1.0f + dnload_powf(static_cast<float>(M_E), -2.0f * WINDOW_SHARPNESS * (op - RIMPOINT)));

      float y1 = dnload_powf(3.0f * op, 2.0f) - DEPTH;
      float y2 = (DEPTH * 0.25f) * (dnload_sinf((op - RIMPOINT) * (static_cast<float>(M_PI) / (1.0f - RIMPOINT)) + static_cast<float>(M_PI * 0.5)) + 1.0f);
      return stepfunction*y2 + (1.0f - stepfunction)*y1;
      //float y = 0.0f;

      /*if(op <= 0.75f)
      {
        return dnload_sinf(op * static_cast<float>(M_PI * 4.0 / 3.0) - static_cast<float>(M_PI * 0.5)) * 2.0f - 1.0f;
      }
      return dnload_cosf((op - 0.75f) * static_cast<float>(M_PI * 4.0)) * 0.5f + 0.5f;*/
    }

    /// Crater height multiplier from radius.
    ///
    /// \param rad Radius (angle).
    /// \return Relative height multiplier.
    static float crater_height_mul(float rad)
    {
      return dnload_sqrtf(dnload_sqrtf(dnload_sqrtf(rad)));
    }

  public:
    /// Add a crater.
    ///
    /// \param dir Crater direction.
    /// \param radius Crater radius.
    void addCrater(const vec3& dir, float radius)
    {
      m_craters.emplace_back(dir, radius);
    }

    /// Get height as calculated from craters.
    ///
    /// \param dir Direction.
    float getHeight(const vec3& dir)
    {
      // Crater height function increases on the edge and decreases towards the center.
      // Craters are deeper than their edges are tall.
      // Large craters have both deeper centers and higher edges.
      seq<float> distances;
      seq<float> radiuses;

      for(const Crater& vv : m_craters)
      {
        opt<float> dist = vv.getDistance(dir);

        if(dist)
        {
          distances.push_back(*dist);
          radiuses.push_back(vv.getRadius());
        }
      }

      if(distances.empty())
      {
        return 0.0f;
      }

      float height = crater_func(distances[0]) * crater_height_mul(radiuses[0]);

      for(unsigned ii = 1; (ii < distances.size()); ++ii)
      {
        float curr_dist = distances[ii];
        float crater_value = crater_func(curr_dist) * crater_height_mul(radiuses[ii]);

        // Newer craters overwrite old ones, because that's how craters work.
        float curr_weight = curr_dist;

#if defined(USE_LD) && defined(DEBUG)
        if((curr_dist < 0.0f) || (curr_dist > 1.0f))
        {
          std::ostringstream sstr;
          sstr << "invalid dist: " << curr_dist;
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif

        height = (curr_weight * height) + crater_value;
      }

      return height;
    }
};

#endif
