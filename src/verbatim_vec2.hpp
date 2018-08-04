#ifndef VERBATIM_VEC2_HPP
#define VERBATIM_VEC2_HPP

#include "verbatim_realloc.hpp"

/// 2-component vector class.
class vec2
{
  private:
    /// Data.
    float m_data[2];

  public:
    /// Empty constructor.
    explicit vec2()
    {
    }

    /// Constructor.
    ///
    /// \param px X component.
    /// \param py Y component.
    explicit vec2(float px, float py)
    {
      m_data[0] = px;
      m_data[1] = py;
    }

    /// Constructor.
    ///
    /// \param op Value for all components.
    explicit vec2(float op)
    {
      m_data[0] = op;
      m_data[1] = op;
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
    vec2& operator=(const vec2 &rhs)
    {
      m_data[0] = rhs[0];
      m_data[1] = rhs[1];
      return *this;
    }

    /// Unary minus operator.
    ///
    /// \return Result vector.
    vec2 operator-() const
    {
      return vec2(-m_data[0], -m_data[1]);
    }
    /// Substraction operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    vec2 operator-(const vec2 &rhs) const
    {
      return vec2(m_data[0] - rhs[0], m_data[1] - rhs[1]);
    }
    /// Substraction into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    vec2 operator-=(const vec2 &rhs)
    {
      m_data[0] -= rhs[0];
      m_data[1] -= rhs[1];
      return *this;
    }

    /// Addition operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    vec2 operator+(const vec2 &rhs) const
    {
      return vec2(m_data[0] + rhs[0], m_data[1] + rhs[1]);
    }
    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    vec2& operator+=(const vec2 &rhs)
    {
      m_data[0] += rhs[0];
      m_data[1] += rhs[1];
      return *this;
    }

    /// Addition operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    vec2 operator+(float rhs) const
    {
      return vec2(m_data[0] + rhs, m_data[1] + rhs);
    }
    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    vec2& operator+=(float rhs)
    {
      m_data[0] += rhs;
      m_data[1] += rhs;
      return *this;
    }

    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    vec2 operator*(float rhs) const
    {
      return vec2(m_data[0] * rhs, m_data[1] * rhs);
    }
    /// Multiplication into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    vec2& operator*=(float rhs)
    {
      m_data[0] *= rhs;
      m_data[1] *= rhs;
      return *this;
    }

    /// Division operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    vec2 operator/(float rhs) const
    {
      return vec2(m_data[0] / rhs, m_data[1] / rhs);
    }
    /// Division into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This vector.
    vec2& operator/=(float rhs)
    {
      m_data[0] /= rhs;
      m_data[1] /= rhs;
      return *this;
    }

  public:
    /// Dot product.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result value.
    friend float dot_product(const vec2 &lhs, const vec2 &rhs)
    {
      return lhs[0] * rhs[0] + lhs[1] * rhs[1];
    }

    /// Calculate vector length.
    ///
    /// \param op Vector to calculate length on.
    /// \return Length of given vector.
    friend float length(const vec2 &op)
    {
      return dnload_sqrtf(dot_product(op, op));
    }

    /// Component-wise maximum.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result value.
    friend vec2 max(const vec2& lhs, const vec2& rhs)
    {
      return vec2(std::max(lhs[0], rhs[0]), std::max(lhs[1], rhs[1]));
    }
    /// Component-wise minimum.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result value.
    friend vec2 min(const vec2& lhs, const vec2& rhs)
    {
      return vec2(std::min(lhs[0], rhs[0]), std::min(lhs[1], rhs[1]));
    }

    /// Mix two vectors.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \param ratio Mixing ratio.
    friend vec2 mix(const vec2 &lhs, const vec2 &rhs, float ratio)
    {
      return lhs + (rhs - lhs) * ratio;
    }

    /// Normalize a vector.
    ///
    /// \param op Vector to normalize.
    /// \return Result vector.
    friend vec2 normalize(const vec2 &op)
    {
      return op / length(op);
    }

    /// Multiplication operator.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    friend vec2 operator*(float lhs, const vec2 &rhs)
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
      return ostr << "[ " << m_data[0] << " ; " << m_data[1] << " ]";
    }
#endif
};

#if defined(USE_LD)
/// Stream output operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const vec2 &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
