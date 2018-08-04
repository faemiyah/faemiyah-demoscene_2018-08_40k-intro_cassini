#ifndef VERBATIM_IMAGE_CUBE_HPP
#define VERBATIM_IMAGE_CUBE_HPP

#include "verbatim_thread.hpp"
#include "verbatim_vec3.hpp"

/// Cube map image.
template<typename T> class ImageCube
{
  public:
    /// Cube map side function.
    ///
    /// \param norm_dir Normalized direction.
    /// \param dir Direction mapped to cube map boundary.
    /// \param cx X coordinate in image.
    /// \param cy Y coordinate in image.
    /// \param img Target image.
    /// \param data Extra data to function.
    typedef void (*CubeMapSideFunc)(const vec3& norm_dir, const vec3& dir, unsigned cx, unsigned cy,
        T& img, void* data);

  private:
    /// Cube map direction function.
    ///
    /// \param fx Relative X coordinate at side.
    /// \param fy Relative Y coordinate at side.
    /// \return Direction vector mapped to cube map side.
    typedef vec3 (*CubeMapDirFunc)(float fx, float fy);

    /// Sub-class for distributed side calculation.
    class SideCalculationContainer
    {
      private:
        /// Cube map.
        ImageCube<T>& m_img;
        
        /// Side function.
        CubeMapSideFunc m_side_func;

        /// Extra data to side functions.
        void* m_data;

      public:
        /// Constructor.
        SideCalculationContainer(ImageCube<T>& img, CubeMapSideFunc side_func, void* data) :
          m_img(img),
          m_side_func(side_func),
          m_data(data)
        {
        }

      public:
        /// Calculate negative X side of cube map.
        ///
        /// \param data Pointer to side calculation container.
        /// \return Always 0.
        static int calculate_side_neg_x(void* data)
        {
          ImageCube<T>::SideCalculationContainer* container =
            static_cast<ImageCube<T>::SideCalculationContainer*>(data);
          container->m_img.calculateSideNegX(container->m_side_func, container->m_data);
          return 0;
        }

        /// Calculate positive X side of cube map.
        ///
        /// \param data Pointer to side calculation container.
        /// \return Always 0.
        static int calculate_side_pos_x(void* data)
        {
          ImageCube<T>::SideCalculationContainer* container =
            static_cast<ImageCube<T>::SideCalculationContainer*>(data);
          container->m_img.calculateSidePosX(container->m_side_func, container->m_data);
          return 0;
        }

        /// Calculate negative Y side of cube map.
        ///
        /// \param data Pointer to side calculation container.
        /// \return Always 0.
        static int calculate_side_neg_y(void* data)
        {
          ImageCube<T>::SideCalculationContainer* container =
            static_cast<ImageCube<T>::SideCalculationContainer*>(data);
          container->m_img.calculateSideNegY(container->m_side_func, container->m_data);
          return 0;
        }

        /// Calculate positive Y side of cube map.
        ///
        /// \param data Pointer to side calculation container.
        /// \return Always 0.
        static int calculate_side_pos_y(void* data)
        {
          ImageCube<T>::SideCalculationContainer* container =
            static_cast<ImageCube<T>::SideCalculationContainer*>(data);
          container->m_img.calculateSidePosY(container->m_side_func, container->m_data);
          return 0;
        }

        /// Calculate negative Z side of cube map.
        ///
        /// \param data Pointer to side calculation container.
        /// \return Always 0.
        static int calculate_side_neg_z(void* data)
        {
          ImageCube<T>::SideCalculationContainer* container =
            static_cast<ImageCube<T>::SideCalculationContainer*>(data);
          container->m_img.calculateSideNegZ(container->m_side_func, container->m_data);
          return 0;
        }

        /// Calculate positive Z side of cube map.
        ///
        /// \param data Pointer to side calculation container.
        /// \return Always 0.
        static int calculate_side_pos_z(void* data)
        {
          ImageCube<T>::SideCalculationContainer* container =
            static_cast<ImageCube<T>::SideCalculationContainer*>(data);
          container->m_img.calculateSidePosZ(container->m_side_func, container->m_data);
          return 0;
        }
    };

  private:
    /// Side image.
    T m_neg_x;
    /// Side image.
    T m_pos_x;
    /// Side image.
    T m_neg_y;
    /// Side image.
    T m_pos_y;
    /// Side image.
    T m_neg_z;
    /// Side image.
    T m_pos_z;

  public:
    /// Constructor.
    ///
    /// \param side Length of one side of a 2D cube map image.
    ImageCube(unsigned int side) :
      m_neg_x(side, side),
      m_pos_x(side, side),
      m_neg_y(side, side),
      m_pos_y(side, side),
      m_neg_z(side, side),
      m_pos_z(side, side)
    {
    }

  public:
    /// Calculate negative X side of cube map.
    ///
    /// \param side_func Function for calculating cube map pixel.
    /// \param data Data parameter for side function.
    void calculateSideNegX(CubeMapSideFunc side_func, void* data)
    {
      calculate_side_generic(dir_neg_x, side_func, m_neg_x, data);
    }

    /// Calculate positive X side of cube map.
    ///
    /// \param side_func Function for calculating cube map pixel.
    /// \param data Data parameter for side function.
    void calculateSidePosX(CubeMapSideFunc side_func, void* data)
    {
      calculate_side_generic(dir_pos_x, side_func, m_pos_x, data);
    }

    /// Calculate negative Y side of cube map.
    ///
    /// \param side_func Function for calculating cube map pixel.
    /// \param data Data parameter for side function.
    void calculateSideNegY(CubeMapSideFunc side_func, void* data)
    {
      calculate_side_generic(dir_neg_y, side_func, m_neg_y, data);
    }

    /// Calculate positive Y side of cube map.
    ///
    /// \param side_func Function for calculating cube map pixel.
    /// \param data Data parameter for side function.
    void calculateSidePosY(CubeMapSideFunc side_func, void* data)
    {
      calculate_side_generic(dir_pos_y, side_func, m_pos_y, data);
    }

    /// Calculate negative Z side of cube map.
    ///
    /// \param side_func Function for calculating cube map pixel.
    /// \param data Data parameter for side function.
    void calculateSideNegZ(CubeMapSideFunc side_func, void* data)
    {
      calculate_side_generic(dir_neg_z, side_func, m_neg_z, data);
    }

    /// Calculate positive Z side of cube map.
    ///
    /// \param side_func Function for calculating cube map pixel.
    /// \param data Data parameter for side function.
    void calculateSidePosZ(CubeMapSideFunc side_func, void* data)
    {
      calculate_side_generic(dir_pos_z, side_func, m_pos_z, data);
    }

    /// Distributed mode, calculate all sides.
    ///
    /// \param side_func Side calculation function.
    /// \param data Extra data to pass to side calculation functions.
    void calculateDistributed(CubeMapSideFunc side_func, void* data)
    {
      ImageCube<T>::SideCalculationContainer container(*this, side_func, data);

      Thread thr_neg_x(ImageCube<T>::SideCalculationContainer::calculate_side_neg_x, &container);
      Thread thr_pos_x(ImageCube<T>::SideCalculationContainer::calculate_side_pos_x, &container);
      Thread thr_neg_y(ImageCube<T>::SideCalculationContainer::calculate_side_neg_y, &container);
      Thread thr_pos_y(ImageCube<T>::SideCalculationContainer::calculate_side_pos_y, &container);
      Thread thr_neg_z(ImageCube<T>::SideCalculationContainer::calculate_side_neg_z, &container);
      Thread thr_pos_z(ImageCube<T>::SideCalculationContainer::calculate_side_pos_z, &container);
    }

    /// Clears a channel to a value.
    ///
    /// Clears all sides.
    ///
    /// \param channel Channel to clear.
    /// \param value Value to clear to (default: 0.0f).
    void clear(unsigned channel, float value = 0.0f)
    {
      m_neg_x.clear(channel, value);
      m_pos_x.clear(channel, value);
      m_neg_y.clear(channel, value);
      m_pos_y.clear(channel, value);
      m_neg_z.clear(channel, value);
      m_pos_z.clear(channel, value);
    }

    /// Gets the address for a pixel.
    ///
    /// \param dir Direction.
    /// \param channel Channel to access.
    /// \return Address of the closest pixel to given location.
    float* getClosestPixelAddress(const vec3& dir, unsigned channel)
    {
      float ax = abs(dir.x());
      float ay = abs(dir.y());
      float az = abs(dir.z());
      float fwidth = static_cast<float>(m_neg_x.getWidth() - 1);
      float fheight = static_cast<float>(m_neg_x.getHeight() - 1);

      // X sides.
      if((ax >= ay) && (ax >= az))
      {
#if defined(USE_LD) && defined(DEBUG)
        if(ax == 0.0f)
        {
          std::ostringstream sstr;
          sstr << "getClosestPixelAddress: invalid direction vector (x): " << dir;
          BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
        }
#endif
        float fy = (dir.y() / ax * 0.5f) + 0.5f;
        float fz = (dir.z() / ax * 0.5f) + 0.5f;
        if(dir.x() < 0.0f)
        {
          unsigned px = static_cast<unsigned>((0.0f + fz) * fwidth + 0.5f);
          unsigned py = static_cast<unsigned>((1.0f - fy) * fheight + 0.5f);
          return m_neg_x.getValueAddress(px, py, channel);
        }
        unsigned px = static_cast<unsigned>((1.0f - fz) * fwidth + 0.5f);
        unsigned py = static_cast<unsigned>((1.0f - fy) * fheight + 0.5f);
        return m_pos_x.getValueAddress(px, py, channel);
      }
      // Y sides.
      else if((ay >= ax) && (ay >= az))
      {
        float fx = (dir.x() / ay * 0.5f) + 0.5f;
        float fz = (dir.z() / ay * 0.5f) + 0.5f;
        if(dir.y() < 0.0f)
        {
          unsigned px = static_cast<unsigned>((0.0f + fx) * fwidth + 0.5f);
          unsigned py = static_cast<unsigned>((1.0f - fz) * fheight + 0.5f);
          return m_neg_y.getValueAddress(px, py, channel);
        }
        unsigned px = static_cast<unsigned>((0.0f + fx) * fwidth + 0.5f);
        unsigned py = static_cast<unsigned>((0.0f + fz) * fheight + 0.5f);
        return m_pos_y.getValueAddress(px, py, channel);
      }
      // Z sides.
      float fx = (dir.x() / az * 0.5f) + 0.5f;
      float fy = (dir.y() / az * 0.5f) + 0.5f;
      if(dir.z() < 0.0f)
      {
        unsigned px = static_cast<unsigned>((1.0f - fx) * fwidth + 0.5f);
        unsigned py = static_cast<unsigned>((1.0f - fy) * fheight + 0.5f);
        return m_neg_z.getValueAddress(px, py, channel);
      }
      unsigned px = static_cast<unsigned>((0.0f + fx) * fwidth + 0.5f);
      unsigned py = static_cast<unsigned>((1.0f - fy) * fheight + 0.5f);
      return m_pos_z.getValueAddress(px, py, channel);
    }

    /// Accessor.
    ///
    /// \return Cube map side image.
    T& getSideNegX()
    {
      return m_neg_x;
    }
    /// Accessor.
    ///
    /// \return Cube map side image.
    const T& getSideNegX() const
    {
      return m_neg_x;
    }

    /// Accessor.
    ///
    /// \return Cube map side image.
    T& getSidePosX()
    {
      return m_pos_x;
    }
    /// Accessor.
    ///
    /// \return Cube map side image.
    const T& getSidePosX() const
    {
      return m_pos_x;
    }

    /// Accessor.
    ///
    /// \return Cube map side image.
    T& getSideNegY()
    {
      return m_neg_y;
    }
    /// Accessor.
    ///
    /// \return Cube map side image.
    const T& getSideNegY() const
    {
      return m_neg_y;
    }

    /// Accessor.
    ///
    /// \return Cube map side image.
    T& getSidePosY()
    {
      return m_pos_y;
    }
    /// Accessor.
    ///
    /// \return Cube map side image.
    const T& getSidePosY() const
    {
      return m_pos_y;
    }

    /// Accessor.
    ///
    /// \return Cube map side image.
    T& getSideNegZ()
    {
      return m_neg_z;
    }
    /// Accessor.
    ///
    /// \return Cube map side image.
    const T& getSideNegZ() const
    {
      return m_neg_z;
    }

    /// Accessor.
    ///
    /// \return Cube map side image.
    T& getSidePosZ()
    {
      return m_pos_z;
    }
    /// Accessor.
    ///
    /// \return Cube map side image.
    const T& getSidePosZ() const
    {
      return m_pos_z;
    }

    /// Normalize color level.
    ///
    /// \param channel Channel
    /// \param ambient Ambient level (default: 0.0f).
    void normalizeSides(unsigned channel, float ambient = 0.0f)
    {
      unsigned width = m_neg_x.getWidth();
      unsigned height = m_neg_x.getHeight();
      float min_value = FLT_MAX;
      float max_value = -FLT_MAX;

      for(unsigned ii = 0; (ii < width); ++ii)
      {
        for(unsigned jj = 0; (jj < height); ++jj)
        {
          float val1 = m_neg_x.getValue(ii, jj, channel);
          float val2 = m_pos_x.getValue(ii, jj, channel);
          float val3 = m_neg_y.getValue(ii, jj, channel);
          float val4 = m_pos_y.getValue(ii, jj, channel);
          float val5 = m_neg_z.getValue(ii, jj, channel);
          float val6 = m_pos_z.getValue(ii, jj, channel);

          float lmin = std::min(std::min(std::min(std::min(std::min(val1, val2), val3), val4), val5), val6);
          float lmax = std::max(std::max(std::max(std::max(std::max(val1, val2), val3), val4), val5), val6);
          min_value = std::min(min_value, lmin);
          max_value = std::max(max_value, lmax);
        }
      }

      // If all values are identical, skip normalization.
      if(max_value != min_value)
      {
        float mul = (1.0f - ambient) / (max_value - min_value);

        //std::cout << "max: " << max_value << " ; min: " << min_value << " ; mul: " << mul << std::endl;

        for(unsigned ii = 0; (ii < width); ++ii)
        {
          for(unsigned jj = 0; (jj < height); ++jj)
          {
            float val1 = m_neg_x.getValue(ii, jj, channel);
            float val2 = m_pos_x.getValue(ii, jj, channel);
            float val3 = m_neg_y.getValue(ii, jj, channel);
            float val4 = m_pos_y.getValue(ii, jj, channel);
            float val5 = m_neg_z.getValue(ii, jj, channel);
            float val6 = m_pos_z.getValue(ii, jj, channel);

            m_neg_x.setValue(ii, jj, channel, (mul * (val1 - min_value)) + ambient);
            m_pos_x.setValue(ii, jj, channel, (mul * (val2 - min_value)) + ambient);
            m_neg_y.setValue(ii, jj, channel, (mul * (val3 - min_value)) + ambient);
            m_pos_y.setValue(ii, jj, channel, (mul * (val4 - min_value)) + ambient);
            m_neg_z.setValue(ii, jj, channel, (mul * (val5 - min_value)) + ambient);
            m_pos_z.setValue(ii, jj, channel, (mul * (val6 - min_value)) + ambient);
          }
        }
      }
   }

  private:
    /// Calculate generic side of cube map.
    ///
    /// \param dir_func Direction function.
    /// \param side_func Side calculation function.
    /// \param img Destination image.
    /// \param data Extra data for side calculation function.
    static void calculate_side_generic(CubeMapDirFunc dir_func, CubeMapSideFunc side_func, T& img, void* data)
    {
      const float CUBE_MAP_SIDE_MUL = 1.0f / (static_cast<float>(img.getWidth()) * 0.5f);

      for(unsigned ii = 0; (ii < img.getWidth()); ++ii)
      {
        float fi = static_cast<float>(ii) * CUBE_MAP_SIDE_MUL;

        for(unsigned jj = 0; (jj < img.getHeight()); ++jj)
        {
          float fj = static_cast<float>(jj) * CUBE_MAP_SIDE_MUL;
          vec3 dir = dir_func(fi, fj);
          vec3 norm_dir = normalize(dir);
          side_func(norm_dir, dir, ii, jj, img, data);
        }
      }
    }

    /// Direction function for negative X.
    ///
    /// \param fi Relative image coordinate X.
    /// \param fj Relative image coordinate Y.
    static vec3 dir_neg_x(float fi, float fj)
    {
      return vec3(-1.0f, 1.0f - fj, -1.0f + fi);
    }

    /// Direction function for positive X.
    ///
    /// \param fi Relative image coordinate X.
    /// \param fj Relative image coordinate Y.
    static vec3 dir_pos_x(float fi, float fj)
    {
      return vec3(1.0f, 1.0f - fj, 1.0f - fi);
    }

    /// Direction function for negative Y.
    ///
    /// \param fi Relative image coordinate X.
    /// \param fj Relative image coordinate Y.
    static vec3 dir_neg_y(float fi, float fj)
    {
      return vec3(-1.0f + fi, -1.0f, 1.0f - fj);
    }

    /// Direction function for positive Y.
    ///
    /// \param fi Relative image coordinate X.
    /// \param fj Relative image coordinate Y.
    static vec3 dir_pos_y(float fi, float fj)
    {
      return vec3(-1.0f + fi, 1.0f, -1.0f + fj);
    }

    /// Direction function for negative Z.
    ///
    /// \param fi Relative image coordinate X.
    /// \param fj Relative image coordinate Y.
    static vec3 dir_neg_z(float fi, float fj)
    {
      return vec3(1.0f - fi, 1.0f - fj, -1.0f);
    }

    /// Direction function for positive Z.
    ///
    /// \param fi Relative image coordinate X.
    /// \param fj Relative image coordinate Y.
    static vec3 dir_pos_z(float fi, float fj)
    {
      return vec3(-1.0f + fi, 1.0f - fj, 1.0f);
    }

  public:
    /// Creates a new cube map image.
    ///
    /// \param side Length of one cube map side.
    static uptr<ImageCube<T> > create(unsigned side)
    {
      return uptr<ImageCube<T> >(new ImageCube<T>(side));
    }
};

#endif
