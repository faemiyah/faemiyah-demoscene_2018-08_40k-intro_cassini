#ifndef VERBATIM_VEC3_HPP
#define VERBATIM_VEC3_HPP

#include "verbatim_gl.hpp"

/// 3-component vector class.
class vec3
{
  private:
    /// Vector data.
    float m_data[3];

  public:
    /// Empty constructor.
    explicit vec3()
    {
    }

    /// Constructor.
    ///
    /// \param px X component.
    /// \param py Y component.
    /// \param pz Z component.
    explicit vec3(float px, float py, float pz)
    {
      m_data[0] = px;
      m_data[1] = py;
      m_data[2] = pz;
    }

    /// Constructor.
    ///
    /// \param op Value for all components.
    explicit vec3(float op)
    {
      m_data[0] = op;
      m_data[1] = op;
      m_data[2] = op;
    }

  public:
    /// Accessor.
    ///
    /// \return Data.
    const float* getData() const
    {
      return m_data;
    }

    /// Accessor.
    ///
    /// \return X component.
    float x() const
    {
      return m_data[0];
    }
		
    /// Accessor.
    ///
    /// \return Y component.
    float y() const
    {
      return m_data[1];
    }

    /// Accessor.
    ///
    /// \return Z component.
    float z() const
    {
      return m_data[2];
    }

  public:
    /// Access operator.
    ///
    /// \param idx Index.
    /// \return Value.
    float& operator[](unsigned idx)
    {
      return m_data[idx];
    }
    /// Const access operator.
    ///
    /// \param idx Index.
    /// \return Value.
    const float& operator[](unsigned idx) const
    {
      return m_data[idx];
    }

    /// Assignment operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    vec3& operator=(const vec3 &rhs)
    {
      m_data[0] = rhs[0];
      m_data[1] = rhs[1];
      m_data[2] = rhs[2];
      return *this;
    }

    /// Unary minus operator.
    ///
    /// \return Result vector.
    vec3 operator-() const
    {
      return vec3(-m_data[0], -m_data[1], -m_data[2]);
    }
    /// Subtraction operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    vec3 operator-(const vec3 &rhs) const
    {
      return vec3(m_data[0] - rhs[0],
          m_data[1] - rhs[1],
          m_data[2] - rhs[2]);
    }
    /// Subtraction into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    vec3 operator-=(const vec3 &rhs)
    {
      m_data[0] -= rhs[0];
      m_data[1] -= rhs[1];
      m_data[2] -= rhs[2];
      return *this;
    }

    /// Addition operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    vec3 operator+(const vec3 &rhs) const
    {
      return vec3(m_data[0] + rhs[0],
          m_data[1] + rhs[1],
          m_data[2] + rhs[2]);
    }
    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    vec3& operator+=(const vec3 &rhs)
    {
      m_data[0] += rhs[0];
      m_data[1] += rhs[1];
      m_data[2] += rhs[2];
      return *this;
    }

    /// Addition operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    vec3 operator+(float rhs) const
    {
      return vec3(m_data[0] + rhs, m_data[1] + rhs, m_data[2] + rhs);
    }
    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    vec3& operator+=(float rhs)
    {
      m_data[0] += rhs;
      m_data[1] += rhs;
      m_data[2] += rhs;
      return *this;
    }

    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    vec3 operator*(float rhs) const
    {
      return vec3(m_data[0] * rhs,
          m_data[1] * rhs,
          m_data[2] * rhs);
    }
    /// Multiplication into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    vec3& operator*=(float rhs)
    {
      m_data[0] *= rhs;
      m_data[1] *= rhs;
      m_data[2] *= rhs;
      return *this;
    }

    /// Division operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    vec3 operator/(float rhs) const
    {
      return vec3(m_data[0] / rhs,
          m_data[1] / rhs,
          m_data[2] / rhs);
    }
    /// Division into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    vec3& operator/=(float rhs)
    {
      m_data[0] /= rhs;
      m_data[1] /= rhs;
      m_data[2] /= rhs;
      return *this;
    }

    /// Equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    bool operator==(const vec3 &rhs) const
    {
      return ((m_data[0] == rhs.m_data[0]) && (m_data[1] == rhs.m_data[1]) && (m_data[2] == rhs.m_data[2]));
    }

    /// Not equals operator.
    ///
    /// \param rhs Right-hand-side operand.
    bool operator!=(const vec3 &rhs) const
    {
      return ((m_data[0] != rhs.m_data[0]) || (m_data[1] != rhs.m_data[1]) || (m_data[2] != rhs.m_data[2]));
    }

  public:
    /// Rotation around Y-axis.
    ///
    /// \param angle Amount to rotate around y-axis, radians.
    /// \return Result vector.
    vec3 rotateY(float angle) const
    {
      return vec3(dnload_cosf(angle)*m_data[0] + dnload_sinf(angle) * m_data[2],
          m_data[1],
          -dnload_sinf(angle)*m_data[0] + dnload_cosf(angle) * m_data[2]);
    }

  public:
    /// Cross product.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    friend vec3 cross(const vec3 &lhs, const vec3 &rhs)
    {
      return vec3(lhs[1] * rhs[2] - lhs[2] * rhs[1],
          lhs[2] * rhs[0] - lhs[0] * rhs[2],
          lhs[0] * rhs[1] - lhs[1] * rhs[0]);
    }

    /// Dot product.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result value.
    friend float dot(const vec3 &lhs, const vec3 &rhs)
    {
      return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
    }

    /// Calculate vector length.
    ///
    /// \param op Vector to calculate length on.
    /// \return Length of given vector.
    friend float length(const vec3 &op)
    {
      return dnload_sqrtf(dot(op, op));
    }

    /// Component-wise maximum.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result value.
    friend vec3 max(const vec3& lhs, const vec3& rhs)
    {
      return vec3(std::max(lhs[0], rhs[0]), std::max(lhs[1], rhs[1]), std::max(lhs[2], rhs[2]));
    }
    /// Component-wise minimum.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result value.
    friend vec3 min(const vec3& lhs, const vec3& rhs)
    {
      return vec3(std::min(lhs[0], rhs[0]), std::min(lhs[1], rhs[1]), std::min(lhs[2], rhs[2]));
    }

    /// Mix two vectors.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \param ratio Mixing ratio.
    friend vec3 mix(const vec3 &lhs, const vec3 &rhs, float ratio)
    {
      return lhs + (rhs - lhs) * ratio;
    }

    /// Normalize a vector.
    ///
    /// \param op Vector to normalize.
    /// \return Result vector.
    friend vec3 normalize(const vec3 &op)
    {
      float len = length(op);
      if(len > 0.0f)
      {
        return op / length(op);
      }
      return vec3(0.0f);
    }

    /// Multiplication operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    friend vec3 operator*(float lhs, const vec3 &rhs)
    {
      return rhs * lhs;
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Stream to output to.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "[ " << m_data[0] << " ; " << m_data[1] << " ; " << m_data[2] << " ]";
    }
#endif
};

#if defined(USE_LD)
/// Stream output operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const vec3 &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
