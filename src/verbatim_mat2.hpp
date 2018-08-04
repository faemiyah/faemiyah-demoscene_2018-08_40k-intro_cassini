#ifndef VERBATIM_MAT2_HPP
#define VERBATIM_MAT2_HPP

#include "verbatim_vec2.hpp"

/// 2x2 matrix class.
class mat2
{
  private:
    /// Matrix data.
    float m_data[4];

  public:
    /// Empty constructor.
    mat2() { }

    /// \brief Full constructor.
    ///
    /// \param op1 First element.
    /// \param op2 Second element.
    /// \param op3 Third element.
    /// \param op4 Fourth element.
    mat2(float op1, float op2, float op3, float op4)
    {
      m_data[0] = op1;
      m_data[1] = op2;
      m_data[2] = op3;
      m_data[3] = op4;
    }

    /// Matrix constructor from a base.
    ///
    /// \param x_vec X base vector.
    /// \param y_vec Y base vector.
    mat2(const vec2 &x_base, const vec2 &y_base)
    {
      m_data[0] = x_base[0];
      m_data[1] = y_base[0];
      m_data[2] = x_base[1];
      m_data[3] = y_base[1];
    }

  public:
    /// Accessor.
    ///
    /// \return Data.
    const float* getData() const
    {
      return m_data;
    }

    /// Access operator.
    ///
    /// \return Value.
    float& operator[](unsigned idx)
    {
      return m_data[idx];
    }

    /// Const access operator.
    ///
    /// \return Value.
    const float& operator[](unsigned idx) const
    {
      return m_data[idx];
    }

    /// Multiply vector.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    vec2 operator*(const vec2 &rhs) const
    {
      return vec2(m_data[0] * rhs[0] + m_data[2] * rhs[1],
          m_data[1] * rhs[0] + m_data[3] * rhs[1]);
    }
    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result matrix.
    const mat2 operator*(const mat2 &rhs) const
    {
      mat2 ret;

      for(unsigned ii = 0; (ii < 4); ii += 2)
      { 
        for(unsigned jj = 0; (jj < 2); ++jj)
        {
          ret[ii + jj] = m_data[0 + jj] * rhs[ii + 0] +
            m_data[2 + jj] * rhs[ii + 1];
        }
      }

      return ret;
    }

    /// Create an identity matrix.
    ///
    /// \return Result matrix.
    static mat2 identity()
    {
      return mat2(1.0f, 0.0f, 0.0f, 1.0f);
    }

    /// Create a rotation matrix.
    ///
    /// \param op Rotation.
    static mat2 rotation(float op)
    {
      float sr = dnload_sinf(op);
      float cr = dnload_cosf(op);

      return mat2(cr, sr, -sr, cr);
    }

  public:
    /// Transpose a matrix.
    ///
    /// \param op Input matrix.
    /// \return Transposed matrix.
    friend mat2 transpose(const mat2 &op)
    {
      return mat2(op[0], op[2], op[1], op[3]);
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "[ " << m_data[0] << " ;  " << m_data[2] << "\n  " <<
        m_data[1] << " ; " << m_data[3] << " ]";
    }
#endif
};

#if defined(USE_LD)
/// Output to stream operator.
///
/// \param lhs Left-hand-side operand.
/// \param rhs Right-hand-side operand.
/// \return Output stream.
inline std::ostream& operator<<(std::ostream &lhs, const mat2 &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
