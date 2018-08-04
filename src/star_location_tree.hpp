#ifndef STAR_LOCATION_TREE_HPP
#define STAR_LOCATION_TREE_HPP

#include "star_location_side.hpp"

/// Star location tree.
class StarLocationTree
{
private:
  /// Side for containing stars.
  StarLocationSide m_neg_x;
  /// Side for containing stars.
  StarLocationSide m_pos_x;
  /// Side for containing stars.
  StarLocationSide m_neg_y;
  /// Side for containing stars.
  StarLocationSide m_pos_y;
  /// Side for containing stars.
  StarLocationSide m_neg_z;
  /// Side for containing stars.
  StarLocationSide m_pos_z;

public:
  /// Constructor.
  StarLocationTree() :
    m_neg_x(StarLocationSide::NEG_X),
    m_pos_x(StarLocationSide::POS_X),
    m_neg_y(StarLocationSide::NEG_Y),
    m_pos_y(StarLocationSide::POS_Y),
    m_neg_z(StarLocationSide::NEG_Z),
    m_pos_z(StarLocationSide::POS_Z)
  {
  }

public:
  /// Add a star.
  /// \param star Star location to add.
  void add(const StarLocation& star)
  {
    const vec3& mapped = star.getMappedDirection();

    if(mapped[0] == -1.0f)
    {
      m_neg_x.add(star);
      //m_neg_y.add(star);
      //m_pos_y.add(star);
      //m_neg_z.add(star);
      //m_pos_z.add(star);
    }
    else if(mapped[0] == 1.0f)
    {
      m_pos_x.add(star);
      //m_neg_y.add(star);
      //m_pos_y.add(star);
      //m_neg_z.add(star);
      //m_pos_z.add(star);
    }
    else if(mapped[1] == -1.0f)
    {
      //m_neg_x.add(star);
      //m_pos_x.add(star);
      m_neg_y.add(star);
      //m_neg_z.add(star);
      //m_pos_z.add(star);
    }
    else if(mapped[1] == 1.0f)
    {
      //m_neg_x.add(star);
      //m_pos_x.add(star);
      m_pos_y.add(star);
      //m_neg_z.add(star);
      //m_pos_z.add(star);
    }
    else if(mapped[2] == -1.0f)
    {
      //m_neg_x.add(star);
      //m_pos_x.add(star);
      //m_neg_y.add(star);
      //m_pos_y.add(star);
      m_neg_z.add(star);
    }
    else
    {
#if defined(USE_LD)
      if(mapped[2] != 1.0f)
      {
        std::ostringstream sstr;
        sstr << "mapped star location " << mapped << " not mappable to any side";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
#endif
      //m_neg_x.add(star);
      //m_pos_x.add(star);
      //m_neg_y.add(star);
      //m_pos_y.add(star);
      m_pos_z.add(star);
    }
  }

  /// Get luminosity for given direction.
  /// \param dir Normalized direction.
  /// \param mapped Cube-mapped direction.
  /// \return Luminosity extracted from all sides.
  float calculateLuminosity(const vec3& dir, const vec3& mapped)
  {
    float luminosity = 0.0f;

    luminosity += m_neg_x.calculateLuminosity(dir, mapped);
    luminosity += m_pos_x.calculateLuminosity(dir, mapped);
    luminosity += m_neg_y.calculateLuminosity(dir, mapped);
    luminosity += m_pos_y.calculateLuminosity(dir, mapped);
    luminosity += m_neg_z.calculateLuminosity(dir, mapped);
    luminosity += m_pos_z.calculateLuminosity(dir, mapped);

    return luminosity;
  }
};

#endif
