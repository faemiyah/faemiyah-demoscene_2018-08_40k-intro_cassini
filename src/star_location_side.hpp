#ifndef STAR_LOCATION_SIDE_HPP
#define STAR_LOCATION_SIDE_HPP

#include "star_location.hpp"

#include "verbatim_seq.hpp"

/// Star location side for divide and conquer.
class StarLocationSide
{
  public:
    enum Bin
    {
      /// Negative X side.
      NEG_X,
      /// Positive X side.
      POS_X,
      /// Negative Y side.
      NEG_Y,
      /// Positive Y side.
      POS_Y,
      /// Negative Z side.
      NEG_Z,
      /// Positive Z side.
      POS_Z,
    };

  private:
    /// Number of subdivisions per side.
    static const int SUBDIVISIONS = 64;

    /// Type for star location sequence.
    typedef seq<StarLocation> StarLocationContainer;

  private:
    /// Array of stars per location.
    seq<StarLocationContainer> m_stars;

    /// Which bin does this side represent?
    Bin m_bin;

  public:
    /// Constructor.
    StarLocationSide(Bin bin) :
      m_stars(SUBDIVISIONS * SUBDIVISIONS),
      m_bin(bin)
    {
      // Must immediately have enough space.
      m_stars.resize(SUBDIVISIONS * SUBDIVISIONS);
    }

    /// Add star.
    /// \param star Star location to add.
    /// \return True if star location was correct for this side, false otherwise.
    void add(const StarLocation& star)
    {
      m_stars[serializeLocation(star)].emplace_back(star);
    }

    /// Get luminosity for given direction.
    /// \param dir Normalized direction.
    /// \param mapped Cube-mapped direction.
    /// \return Luminosiry extracted from this side.
    float calculateLuminosity(const vec3& dir, const vec3& mapped)
    {
      int ix = mapSubdivisionX(mapped);
      int iy = mapSubdivisionY(mapped);
      float luminosity = 0.0f;

      for (int jj = std::max(iy - 1, 0); (jj <= std::min(iy + 1, SUBDIVISIONS - 1)); ++jj)
      {
        int row = jj * SUBDIVISIONS;

        for (int ii = std::max(ix - 1, 0); (ii <= std::min(ix + 1, SUBDIVISIONS - 1)); ++ii)
        {
          int idx = row + ii;

          for (const StarLocation& vv : m_stars[idx])
          {
            luminosity += vv.calculateLuminosity(dir);
          }
        }
      }

      return luminosity;
    }

  private:
    /// Map subdivision.
    /// \param coord Coordinate to map.
    /// \return Mapped subdivision coordinate.
    int mapSubdivision(float coord)
    {
#if defined(USE_LD)
      if ((coord < -1.0f) || (coord > 1.0f))
      {
        std::ostringstream sstr;
        sstr << "coordiante " << coord << " not in mappable subdivision range";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      const int IDIV = static_cast<int>(SUBDIVISIONS);
      return std::min(static_cast<int>((coord + 1.0f) * 0.5f * static_cast<float>(IDIV)), IDIV - 1);
    }

    /// Map X location.
    /// \param dir Direction to map.
    /// \return X subdivision slot.
    int mapSubdivisionX(const vec3& dir)
    {
      if ((m_bin == NEG_X) || (m_bin == POS_X))
      {
        return mapSubdivision(dir[2]);
      }
      else if ((m_bin == NEG_Y) || (m_bin == POS_Y))
      {
        return mapSubdivision(dir[0]);
      }
      else
      {
        return mapSubdivision(dir[0]);
      }
    }

    /// Map Y location.
    /// \param dir Direction to map.
    /// \return Y subdivision slot.
    int mapSubdivisionY(const vec3& dir)
    {
      if ((m_bin == NEG_X) || (m_bin == POS_X))
      {
        return mapSubdivision(dir[1]);
      }
      else if ((m_bin == NEG_Y) || (m_bin == POS_Y))
      {
        return mapSubdivision(dir[2]);
      }
      else
      {
        return mapSubdivision(dir[1]);
      }
    }

    /// Serialize a location.
    /// \param star Star to serialize.
    int serializeLocation(const StarLocation& star)
    {
      const vec3& mapped = star.getMappedDirection();
      int ix = mapSubdivisionX(mapped);
      int iy = mapSubdivisionY(mapped);

      return (iy  * SUBDIVISIONS) + ix;
    }
};

#endif
