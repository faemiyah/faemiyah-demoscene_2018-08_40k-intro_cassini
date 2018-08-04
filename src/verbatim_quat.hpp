#ifndef VERBATIM_QUAT_HPP
#define VERBATIM_QUAT_HPP

#include "verbatim_realloc.hpp"

/// Quaternion.
class quat
{
  private:
    /// Quaternion data.
    float m_data[4];

  public:
    /// Empty constructor.
    quat() { }

    /// Constructor.
    ///
    /// \param op1 First value.
    /// \param op2 Second value.
    /// \param op3 Third value.
    /// \param op4 Fourth value.
    quat(float op1, float op2, float op3, float op4)
    {
      m_data[0] = op1;
      m_data[1] = op2;
      m_data[2] = op3;
      m_data[3] = op4;
    }

  public:
    /// Calculate the magnitude of the quaternion.
    ///
    /// Analogous to 4-component vector length.
    ///
    /// \return Magnitude.
    float magnitude() const
    {
      return dnload_sqrtf(m_data[0] * m_data[0] +
          m_data[1] * m_data[1] +
          m_data[2] * m_data[2] +
          m_data[3] * m_data[3]);
    }

    /// Accessor.
    ///
    /// \param idx Index.
    float& operator[](unsigned idx)
    {
      return m_data[idx];
    }
    /// Const accessor.
    ///
    /// \param idx Index.
    const float& operator[](unsigned idx) const
    {
      return m_data[idx];
    }

    /// Unary minus operator.
    ///
    /// \return Result quaternion.
    quat operator-() const
    {
      return quat(-m_data[0],
          -m_data[1],
          -m_data[2],
          -m_data[3]);
    }
    /// Subtraction operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    quat operator-(const quat &rhs) const
    {
      return quat(m_data[0] - rhs[0],
          m_data[1] - rhs[1],
          m_data[2] - rhs[2],
          m_data[3] - rhs[3]);
    }
    /// Subtraction into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This quaternion.
    quat operator-=(const quat &rhs)
    {
      m_data[0] -= rhs[0];
      m_data[1] -= rhs[1];
      m_data[2] -= rhs[2];
      m_data[3] -= rhs[3];
      return *this;
    }

    /// Addition operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    quat operator+(const quat &rhs) const
    {
      return quat(m_data[0] + rhs[0],
          m_data[1] + rhs[1],
          m_data[2] + rhs[2],
          m_data[3] + rhs[3]);
    }
    /// Addition into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This quaternion.
    quat& operator+=(const quat &rhs)
    {
      m_data[0] += rhs[0];
      m_data[1] += rhs[1];
      m_data[2] += rhs[2];
      m_data[3] += rhs[3];
      return *this;
    }

    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    quat operator*(float rhs) const
    {
      return quat(m_data[0] * rhs,
          m_data[1] * rhs,
          m_data[2] * rhs,
          m_data[3] * rhs);
    }
    /// Multiplication into operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return This quaternion.
    quat& operator*=(float rhs)
    {
      m_data[0] *= rhs;
      m_data[1] *= rhs;
      m_data[2] *= rhs;
      m_data[3] *= rhs;
      return *this;
    }
    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result quaternion.
    quat operator*(const quat &rhs) const
    {
      // (Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
      // (Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
      // (Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
      // (Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2)
      return quat((m_data[0] * rhs[0]) - (m_data[1] * rhs[1]) - (m_data[2] * rhs[2]) - (m_data[3] * rhs[3]),
        (m_data[0] * rhs[1]) + (m_data[1] * rhs[0]) + (m_data[2] * rhs[3]) - (m_data[3] * rhs[2]),
        (m_data[0] * rhs[2]) - (m_data[1] * rhs[3]) + (m_data[2] * rhs[0]) + (m_data[3] * rhs[1]),
        (m_data[0] * rhs[3]) + (m_data[1] * rhs[2]) - (m_data[2] * rhs[1]) + (m_data[3] * rhs[0]));
    }

  public:
    /// Mix two quaternions.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \param ratio Mixing ratio.
    friend quat mix(const quat &lhs, const quat &rhs, float ratio)
    {
      return lhs + (rhs - lhs) * ratio;
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "[ " << m_data[0] << " ; " << m_data[1] << " ; " << m_data[2] << " ; " << m_data[3] <<
        " ]";
    }
#endif
};

#if defined(USE_LD)
/// Output to stream operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const quat &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
