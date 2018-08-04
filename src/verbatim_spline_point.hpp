#ifndef VERBATIM_SPLINE_POINT_HPP
#define VERBATIM_SPLINE_POINT_HPP

#include "verbatim_vec3.hpp"

/// Spline point.
class SplinePoint
{
  private:
    /// Actual point position.
    vec3 m_point;

    /// Previous control point.
    vec3 m_prev;

    /// Next control point.
    vec3 m_next;

    /// Timestamp.
    float m_timestamp;

  public:
    /// Constructor.
    ///
    /// \param pos Position.
    /// \param stamp Timestamp.
    SplinePoint(const vec3 &pos, float stamp) :
      m_point(pos),
      m_timestamp(stamp) { }
    
  public:
    /// Accessor.
    ///
    /// \return Next control point.
    const vec3& getNext() const
    {
      return m_next;
    }
    /// Setter.
    ///
    /// \param op New next control point.
    void setNext(const vec3 &op)
    {
      m_next = op;
    }

    /// Accessor.
    ///
    /// \return Actual point.
    const vec3& getPoint() const
    {
      return m_point;
    }

    /// Accessor.
    ///
    /// \return Previous control point.
    const vec3& getPrev() const
    {
      return m_prev;
    }
    /// Setter.
    ///
    /// \param op New previous control point.
    void setPrev(const vec3 &op)
    {
      m_prev = op;
    }

    /// Accessor.
    ///
    /// \return Timestamp.
    float getTimestamp() const
    {
      return m_timestamp;
    }
};

#endif
