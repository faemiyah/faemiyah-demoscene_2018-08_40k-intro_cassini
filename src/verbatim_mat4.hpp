#ifndef VERBATIM_MAT4_HPP
#define VERBATIM_MAT4_HPP

#include "verbatim_mat3.hpp"

/// 4x4 matrix class.
class mat4
{
  private:
    /// Matrix data.
    float m_data[16];

  public:
    /// Empty constructor.
    mat4() { }

    /// Constructor.
    ///
    /// \param op1 First value.
    /// \param op2 Second value.
    /// \param op3 Third value.
    /// \param op4 Fourth value.
    /// \param op5 Fifth value.
    /// \param op6 Sixth value.
    /// \param op7 Seventh value.
    /// \param op8 Eighth value.
    /// \param op9 Ninth value.
    /// \param op10 Tenth value.
    /// \param op11 Eleventh value.
    /// \param op12 Twelwth value.
    /// \param op13 Thirteenth value.
    /// \param op14 Foiurteenth value.
    /// \param op15 Fifteenth value.
    /// \param op16 Sixteenth value.
    mat4(float op1, float op2, float op3, float op4, float op5, float op6, float op7, float op8, float op9,
        float op10, float op11, float op12, float op13, float op14, float op15, float op16)
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
      m_data[9] = op10;
      m_data[10] = op11;
      m_data[11] = op12;
      m_data[12] = op13;
      m_data[13] = op14;
      m_data[14] = op15;
      m_data[15] = op16;
    }

    /// Expansion constructor.
    ///
    /// \param mat 3x3 Matrix.
    /// \param pos Position.
    mat4(const mat3 &mat, const vec3 &pos = vec3(0.0f, 0.0f, 0.0f))
    {
      m_data[0] = mat[0];
      m_data[1] = mat[1];
      m_data[2] = mat[2];
      m_data[3] = 0.0f;
      m_data[4] = mat[3];
      m_data[5] = mat[4];
      m_data[6] = mat[5];
      m_data[7] = 0.0f;
      m_data[8] = mat[6];
      m_data[9] = mat[7];
      m_data[10] = mat[8];
      m_data[11] = 0.0f;
      m_data[12] = pos[0];
      m_data[13] = pos[1];
      m_data[14] = pos[2];
      m_data[15] = 1.0f;
    }
    
    /// Axis construction.
    ///
    /// \param rt Right axis.
    /// \param up Up axis.
    /// \param fw Forward axis.
    /// \param pos Position.
    mat4(const vec3 &unit_rt, const vec3 &unit_up, const vec3 &unit_fw,
        const vec3 &pos = vec3(0.0f, 0.0f, 0.0f))
    {
      m_data[0] = unit_rt[0];
      m_data[1] = unit_rt[1];
      m_data[2] = unit_rt[2];
      m_data[3] = 0.0f;
      m_data[4] = unit_up[0];
      m_data[5] = unit_up[1];
      m_data[6] = unit_up[2];
      m_data[7] = 0.0f;
      m_data[8] = unit_fw[0];
      m_data[9] = unit_fw[1];
      m_data[10] = unit_fw[2];
      m_data[11] = 0.0f;
      m_data[12] = pos[0];
      m_data[13] = pos[1];
      m_data[14] = pos[2];
      m_data[15] = 1.0f;
    }

  public:
    /// Add translation component.
    ///
    /// \param op Translation component to add.
    void addTranslation(const vec3 &op)
    {
      m_data[12] += op[0];
      m_data[13] += op[1];
      m_data[14] += op[2];
    }

    /// Accessor.
    ///
    /// \return Data.
    const float* getData() const
    {
      return m_data;
    }

    /// Get rotation matrix part as 3x3 matrix.
    ///
    /// \return Rotation matrix part.
    mat3 getRotation() const
    {
      return mat3(m_data[0], m_data[1], m_data[2],
          m_data[4], m_data[5], m_data[6],
          m_data[8], m_data[9], m_data[10]);
    }

    /// Get forward component.
    ///
    /// \return Forward vector.
    vec3 getForward() const
    {
      return vec3(-m_data[8], -m_data[9], -m_data[10]);
    }  

    /// Get translation vector.
    ///
    /// \return Translation vector part.
    vec3 getTranslation() const
    {
      return vec3(m_data[12], m_data[13], m_data[14]);
    }
    /// Set translation vector.
    ///
    /// \param tx X translation.
    /// \param ty Y translation.
    /// \param tz Z translation.
    void setTranslation(float tx, float ty, float tz)
    {
      m_data[12] = tx;
      m_data[13] = ty;
      m_data[14] = tz;
    }
    /// Set translation vector.
    ///
    /// \param op New translation vector.
    void setTranslation(const vec3 &op)
    {
      setTranslation(op[0], op[1], op[2]);
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

    /// Addition operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result matrix.
    mat4 operator+(const mat4 &rhs) const
    {
      return mat4(m_data[0] + rhs[0],
          m_data[1] + rhs[1],
          m_data[2] + rhs[2],
          m_data[3] + rhs[3],
          m_data[4] + rhs[4],
          m_data[5] + rhs[5],
          m_data[6] + rhs[6],
          m_data[7] + rhs[7],
          m_data[8] + rhs[8],
          m_data[9] + rhs[9],
          m_data[10] + rhs[10],
          m_data[11] + rhs[11],
          m_data[12] + rhs[12],
          m_data[13] + rhs[13],
          m_data[14] + rhs[14],
          m_data[15] + rhs[15]);
    }

    /// Substraction operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result matrix.
    mat4 operator-(const mat4 &rhs) const
    {
      return mat4(m_data[0] - rhs[0],
          m_data[1] - rhs[1],
          m_data[2] - rhs[2],
          m_data[3] - rhs[3],
          m_data[4] - rhs[4],
          m_data[5] - rhs[5],
          m_data[6] - rhs[6],
          m_data[7] - rhs[7],
          m_data[8] - rhs[8],
          m_data[9] - rhs[9],
          m_data[10] - rhs[10],
          m_data[11] - rhs[11],
          m_data[12] - rhs[12],
          m_data[13] - rhs[13],
          m_data[14] - rhs[14],
          m_data[15] - rhs[15]);
    }

    /// Multiply by scalar.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result matrix.
    mat4 operator*(float rhs) const
    {
      return mat4(m_data[0] * rhs,
          m_data[1] * rhs,
          m_data[2] * rhs,
          m_data[3] * rhs,
          m_data[4] * rhs,
          m_data[5] * rhs,
          m_data[6] * rhs,
          m_data[7] * rhs,
          m_data[8] * rhs,
          m_data[9] * rhs,
          m_data[10] * rhs,
          m_data[11] * rhs,
          m_data[12] * rhs,
          m_data[13] * rhs,
          m_data[14] * rhs,
          m_data[15] * rhs);
    }

    /// Multiply vector.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result vector.
    vec3 operator*(const vec3 &rhs) const
    {
      return vec3(m_data[0] * rhs[0] + m_data[4] * rhs[1] + m_data[8] * rhs[2] + m_data[12],
          m_data[1] * rhs[0] + m_data[5] * rhs[1] + m_data[9] * rhs[2] + m_data[13],
          m_data[2] * rhs[0] + m_data[6] * rhs[1] + m_data[10] * rhs[2] + m_data[14]);
    }

    /// Multiplication operator.
    ///
    /// \param rhs Right-hand-side operand.
    /// \return Result matrix.
    const mat4 operator*(const mat4 &rhs) const
    {
      mat4 ret;

      for(unsigned ii = 0; (ii < 16); ii += 4)
      { 
        for(unsigned jj = 0; (jj < 4); ++jj)
        {
          ret[ii + jj] = m_data[0 + jj] * rhs[ii + 0] +
            m_data[4 + jj] * rhs[ii + 1] +
            m_data[8 + jj] * rhs[ii + 2] +
            m_data[12 + jj] * rhs[ii + 3];
        }
      }

      return ret;
    }

  public:
    /// Scale the 3x3 part of matrix directly.
    ///
    /// \param sx X scale.
    /// \param sy Y scale.
    /// \param sz Z scale.
    void applyScale(float sx, float sy, float sz)
    {
      m_data[0] *= sx;
      m_data[1] *= sy;
      m_data[2] *= sz;
      m_data[4] *= sx;
      m_data[5] *= sy;
      m_data[6] *= sz;
      m_data[8] *= sx;
      m_data[9] *= sy;
      m_data[10] *= sz;
    }
    /// Scale wrapper.
    ///
    /// \param op Scaling vector.
    void applyScale(const vec3 &op)
    {
      applyScale(op[0], op[1], op[2]);
    }

    /// Create an identity matrix.
    ///
    /// \return Result matrix.
    static mat4 identity()
    {
      return mat4(1.0f, 0.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f, 0.0f,
          0.0f, 0.0f, 1.0f, 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f);
    }

    /// Pack an animated rotation-translation into a storage matrix.
    ///
    /// \param yaw Yaw rotation.
    /// \param pitch Pitch rotation.
    /// \param roll Roll rotation.
    /// \param dyaw Rotation diff yaw (per frame).
    /// \param dpitch Rotation diff pitch (per frame).
    /// \param droll Rotation diff roll (per frame).
    /// \param pos Position.
    /// \return Result matrix.
    static mat4 pack_rta(float yaw, float pitch, float roll, float dyaw, float dpitch, float droll,
        const vec3 &pos)
    {
      return mat4(yaw, pitch, roll, 0.0f,
          dyaw, dpitch, droll, 0.0f,
          0.0f, 0.0f, 0.0f, 0.0f,
          pos[0], pos[1], pos[2], 0.0f);
    }

    /// Create a 'look at' -matrix.
    ///
    /// \param pos Position.
    /// \param eye Eye focal point.
    /// \param up Up direction.
    /// \return Result matrix.
    static mat4 lookat(const vec3 &pos, const vec3 &eye, const vec3 &up = vec3(0.0f, 1.0f, 0.0f))
    {
      vec3 unit_fw = normalize(pos - eye);
      vec3 unit_up = normalize(up);

      if(std::abs(dot(unit_fw, unit_up)) > 0.999f)
      {
        unit_up = vec3(unit_fw[1], unit_fw[0], unit_fw[2]);
      }

      vec3 unit_rt = normalize(cross(unit_up, unit_fw));
      unit_up = normalize(cross(unit_fw, unit_rt));

      return mat4(unit_rt, unit_up, unit_fw, pos);
    }

    /// Create a projection matrix.
    ///
    /// \param xfov Horizontal field of view in radians.
    /// \param width Viewport width.
    /// \param height Viewport height.
    /// \param znear Near clip plane.
    /// \param zfar Near clip plane.
    /// \return Result matrix.
    static mat4 projection(float xfov, unsigned width, unsigned height, float znear, float zfar)
    {
      float ff = dnload_tanf(static_cast<float>(M_PI * 0.5) - xfov * 0.5f);
      float n_f = 1.0f / (znear - zfar);

      return mat4(ff, 0.0f, 0.0f, 0.0f,
          0.0f, ff * (static_cast<float>(width) / static_cast<float>(height)), 0.0f, 0.0f,
          0.0f, 0.0f, (znear + zfar) * n_f, -1.0f,
          0.0f, 0.0f, 2.0f * znear * zfar * n_f, 0.0f);
    }

    /// Create a rotation matrix.
    ///
    /// \param rot Input quaternion.
    /// \param pos Translation vector.
    static mat4 rotation(const quat &rot, const vec3 &pos = vec3(0.0f, 0.0f, 0.0f))
    {
      return mat4(mat3::rotation(rot), pos);
    }

    /// Create rotation matrix.
    ///
    /// \param yaw Yaw rotation.
    /// \param pitch Pitch rotation.
    /// \param roll Roll rotation.
    /// \param pos Translation vector.
    /// \return Result matrix.
    static mat4 rotation_euler(float yaw, float pitch, float roll,
        const vec3 &pos = vec3(0.0f, 0.0f, 0.0f))
    {
      return mat4(mat3::rotation_euler(yaw, pitch, roll), pos);
    }

    /// Create scaling matrix.
    ///
    /// \param sx X scale.
    /// \param sy Y scale.
    /// \param sz Z scale.
    /// \return Result matrix.
    static mat4 scale(float sx, float sy, float sz)
    {
      return mat4(sx, 0.0f, 0.0f, 0.0f,
          0.0f, sy, 0.0f, 0.0f,
          0.0f, 0.0f, sz, 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f);
    }
    /// Create scaling matrix wrapper.
    ///
    /// \param op Scale.
    /// \return Result matrix.
    static mat4 scale(float op)
    {
      return scale(op, op, op);
    }

    /// Create full transformation matrix.
    ///
    /// \param yaw Yaw rotation.
    /// \param pitch Pitch rotation.
    /// \param roll Roll rotation.
    /// \param translation Translation vector.
    /// \return Result matrix.
    static mat4 transformation_euler(const vec3 &sca, const vec3 &rot, const vec3 &tra)
    {
      mat3 rpart = mat3::rotation_euler(rot[0], rot[1], rot[2]);
      return mat4(
          rpart[0] * sca[0], rpart[1] * sca[0], rpart[2] * sca[0], 0.0f,
          rpart[3] * sca[1], rpart[4] * sca[1], rpart[5] * sca[1], 0.0f,
          rpart[6] * sca[2], rpart[7] * sca[2], rpart[8] * sca[2], 0.0f,
          tra[0], tra[1], tra[2], 1.0f);
    }

    /// Create a translation matrix.
    ///
    /// \param tx X translation.
    /// \param ty Y translation.
    /// \param tz Z translation.
    /// \return Translation matrix
    static mat4 translation(float tx, float ty, float tz)
    {
      return mat4(1.0f, 0.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f, 0.0f,
          0.0f, 0.0f, 1.0f, 0.0f,
          tx, ty, tz, 1.0f);
    }
    /// Create a translation matrix.
    ///
    /// \param pos Position vector.
    /// \return Translation matrix
    static mat4 translation(const vec3 &pos)
    {
      return translation(pos[0], pos[1], pos[2]);
    }
    
    /// Unpack an animated rotation-translation storage matrix.
    ///
    /// Will modify the matrix being unpacked to advance animation.
    ///
    /// \param transform Matrix storage.
    /// \param frame Frame id.
    /// \return Normal transformation matrix.
    static mat4 unpack_rta(const mat4 &transform, int frame)
    {
      if(transform[15] != 0.0f)
      {
        return transform;
      }
      float fidx = static_cast<float>(frame);
      float yaw = transform[0] + transform[4] * fidx;
      float pitch = transform[1] + transform[5] * fidx;
      float roll = transform[2] + transform[6] * fidx;
      return rotation_euler(yaw, pitch, roll, vec3(transform[12], transform[13], transform[14]));
    }

  public:
    /// Mix between two matrices.
    ///
    /// \param lhs Left-hand-side operand.
    /// \param rhs Right-hand-side operand.
    /// \param ratio Mixing ratio.
    friend mat4 mix(const mat4 &lhs, const mat4 &rhs, float ratio)
    {
      return lhs + (rhs - lhs) * ratio;
    }

    /// Convert to camera matrix.
    ///
    /// \param op Input matrix.
    /// \return View matrix from the given input.
    friend mat4 viewify(const mat4 &op)
    {
      mat3 rotation = transpose(op.getRotation());
      vec3 translation = rotation * (-op.getTranslation());
      return mat4(rotation, translation);
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
        m_data[0] << " ;  " << m_data[4] << " ; " << m_data[8] << " ; " << m_data[12] << "\n  " <<
        m_data[1] << " ; " << m_data[5] << " ; " << m_data[9] << " ; " << m_data[13] << "\n  " <<
        m_data[2] << " ; " << m_data[6] << " ; " << m_data[10] << " ; " << m_data[14] << "\n  " <<
        m_data[3] << " ; " << m_data[7] << " ; " << m_data[11] << " ; " << m_data[15] <<
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
inline std::ostream& operator<<(std::ostream &lhs, const mat4 &rhs)
{
  return rhs.put(lhs);
}
#endif

#endif
