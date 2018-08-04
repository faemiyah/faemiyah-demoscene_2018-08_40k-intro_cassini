#ifndef VERBATIM_MAT3_HPP
#define VERBATIM_MAT3_HPP

#include "verbatim_quat.hpp"
#include "verbatim_vec3.hpp"

/// 3x3 matrix class.
class mat3
{
  private:
    /// Matrix data.
    float m_data[9];

  public:
    /// Empty constructor.
    mat3() { }

    /// \brief Full constructor.
    ///
    /// \param op1 First element.
    /// \param op2 Second element.
    /// \param op3 Third element.
    /// \param op4 Fourth element.
    /// \param op5 Fifth element.
    /// \param op6 Sixth element.
    /// \param op7 Seventh element.
    /// \param op8 Eight element.
    /// \param op9 Ninth element.
    mat3(float op1, float op2, float op3, float op4, float op5, float op6, float op7, float op8, float op9)
    {
      m_data[0] = op1;
      m_data[1] = op2;
      m_data[2] = op3;
      m_data[3] = op4;
      m_data[4] = op5;
      m_data[5] = op6;
      m_data[6] = op7;
      m_data[7] = op8;
      m_data[8] = op9;
    }

    /// Matrix constructor from a base.
    ///
    /// \param x_vec X base vector.
    /// \param y_vec Y base vector.
    /// \param z_vec Z base vector.
    mat3(const vec3 &x_base, const vec3 &y_base, const vec3 &z_base)
    {
      m_data[0] = x_base[0];
      m_data[1] = y_base[0];
      m_data[2] = z_base[0];
      m_data[3] = x_base[1];
      m_data[4] = y_base[1];
      m_data[5] = z_base[1];
      m_data[6] = x_base[2];
      m_data[7] = y_base[2];
      m_data[8] = z_base[2];
    }

  private:
    /// Create zxy rotation matrix in this.
    ///
    /// \param rx Rotation X.
    /// \param ry Rotation Y.
    /// \param rz Rotation Z.
    static mat3 rotation_zxy(float rx, float ry, float rz)
    {
      float sx = dnload_sinf(rx);
      float sy = dnload_sinf(ry);
      float sz = dnload_sinf(rz);
      float cx = dnload_cosf(rx);
      float cy = dnload_cosf(ry);
      float cz = dnload_cosf(rz);

      return mat3(sx * sy * sz + cy * cz,
          sz * cx,
          sx * sz * cy - sy * cz,
          sx * sy * cz - sz * cy,
          cx * cz,
          sx * cy * cz + sy * sz,
          sy * cx,
          -sx,
          cx * cy);
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
    vec3 operator*(const vec3 &rhs) const
    {
      return vec3(m_data[0] * rhs[0] + m_data[3] * rhs[1] + m_data[6] * rhs[2],
          m_data[1] * rhs[0] + m_data[4] * rhs[1] + m_data[7] * rhs[2],
          m_data[2] * rhs[0] + m_data[5] * rhs[1] + m_data[8] * rhs[2]);
    }
    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result matrix.
    const mat3 operator*(const mat3 &rhs) const
    {
      mat3 ret;

      for(unsigned ii = 0; (ii < 9); ii += 3)
      { 
        for(unsigned jj = 0; (jj < 3); ++jj)
        {
          ret[ii + jj] = m_data[0 + jj] * rhs[ii + 0] +
            m_data[3 + jj] * rhs[ii + 1] +
            m_data[6 + jj] * rhs[ii + 2];
        }
      }

      return ret;
    }

    /// Create an identity matrix.
    ///
    /// \return Result matrix.
    static mat3 identity()
    {
      return mat3(1.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f,
          0.0f, 0.0f, 1.0f);
    }

    /// Create a rotation matrix.
    ///
    /// \param op Input quaternion.
    static mat3 rotation(const quat &op)
    {
      float mag = op.magnitude();
      float w = op[0] / mag;
      float x = op[1] / mag;
      float y = op[2] / mag;
      float z = op[3] / mag;
      float wx2 = 2.0f * w * x;
      float wy2 = 2.0f * w * y;
      float wz2 = 2.0f * w * z;
      float xy2 = 2.0f * x * y;
      float xz2 = 2.0f * x * z;
      float yz2 = 2.0f * y * z;
      float xx2 = 2.0f * x * x;
      float yy2 = 2.0f * y * y;
      float zz2 = 2.0f * z * z;

      // Using unit quaternion to matrix algo by J.M.P. van Wavaren at Id Software:
      // http://fabiensanglard.net/doom3_documentation/37726-293748.pdf
      return mat3(1.0f - yy2 - zz2,
          xy2 + wz2,
          xz2 - wy2,
          xy2 - wz2,
          1.0f - xx2 - zz2,
          yz2 + wx2,
          xz2 + wy2,
          yz2 - wx2,
          1.0f - xx2 - yy2);
    }

    /// Create an euler rotation matrix.
    ///
    /// \param yaw Yaw rotation.
    /// \param pitch Pitch rotation.
    /// \param roll Roll rotation.
    static mat3 rotation_euler(float yaw, float pitch, float roll)
    {
      return rotation_zxy(pitch, yaw, roll);
    }

  public:
    /// Transpose a matrix.
    ///
    /// \param op Input matrix.
    /// \return Transposed matrix.
    friend mat3 transpose(const mat3 &op)
    {
      return mat3(op[0], op[3], op[6],
          op[1], op[4], op[7],
          op[2], op[5], op[8]);
    }

#if defined(USE_LD)
  public:
    /// Output to stream.
    ///
    /// \param ostr Output stream.
    /// \return Output stream.
    std::ostream& put(std::ostream &ostr) const
    {
      return ostr << "[ " <<
        m_data[0] << " ;  " << m_data[3] << " ; " << m_data[6] << "\n  " <<
        m_data[1] << " ; " << m_data[4] << " ; " << m_data[7] << "\n  " <<
        m_data[2] << " ; " << m_data[5] << " ; " << m_data[8] <<
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
inline std::ostream& operator<<(std::ostream &lhs, const mat3 &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
