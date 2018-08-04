#ifndef VERBATIM_ARR_HPP
#define VERBATIM_ARR_HPP

#include "verbatim_realloc.hpp"

/// Fixed-size memory array.
///
/// Think of this as unique_ptr for arrays.
///
/// Will not call constructors or destructors for the data. Does not store array size.
/// User is responsible for valid access and bookkeeping.
template<typename T> class uarr
{
  public:
    /// Convenience typedef.
    typedef T value_type;

  private:
    /// Array data.
    T *m_data;

  private:
    /// Deleted copy constructor.
    uarr(const uarr<T>&) = delete;
    /// Deleted assignment.
    uarr<T>& operator=(const uarr<T>&) = delete;

  public:
    /// Empty constructor.
    explicit uarr() :
      m_data(NULL)
    {
    }

    /// Constructor.
    ///
    /// \param count Array size.
    explicit uarr(unsigned count) :
      m_data(NULL)
    {
      if(count)
      {
        m_data = array_new(m_data, count);
      }
    }

    /// Move constructor.
    ///
    /// \param src Source array.
    uarr(uarr<T>&& src)
    {
      m_data = src.release();
    }

    /// Destructor.
    ~uarr()
    {
      erase();
    }

  private:
    /// Erase contents.
    void erase()
    {
      array_delete(m_data);
    }

  public:
    /// Accessor.
    ///
    /// \return Pointer to data.
    T* get() const
    {
      return m_data;
    }

    /// Replace contents.
    ///
    /// \param op New contents.
    void reset(T* op = NULL)
    {
      erase();
      m_data = op;
    }

    /// Release ownership of pointer.
    T* release()
    {
      T* ret = m_data;
      m_data = NULL;
      return ret;
    }

    /// Resize to given size.
    ///
    /// \param count Count.
    void resize(unsigned count)
    {
      if(count)
      {
        m_data = array_new(m_data, count);
        return;
      }
      erase();
      m_data = NULL;
    }

#if defined(__LP64__)
    /// Access operator.
    ///
    /// \return Element reference.
    T& operator[](size_t idx)
    {
      return m_data[idx];
    }
    /// Const access operator.
    ///
    /// \return Element reference.
    const T& operator[](size_t idx) const
    {
      return m_data[idx];
    }
#endif
    /// Access operator.
    ///
    /// \return Element reference.
    T& operator[](unsigned idx)
    {
      return m_data[idx];
    }
    /// Const access operator.
    ///
    /// \return Element reference.
    const T& operator[](unsigned idx) const
    {
      return m_data[idx];
    }
    /// Access operator wrapper.
    ///
    /// \return Element reference.
    T& operator[](int idx)
    {
      return m_data[idx];
    }
    /// Const access operator wrapper.
    ///
    /// \return Element reference.
    const T& operator[](int idx) const
    {
      return m_data[idx];
    }

    /// Truth value operator.
    ///
    /// \return True if data has been initialized.
    operator bool() const
    {
      return (NULL != m_data);
    }

    /// Move operator.
    ///
    /// \param src Source array.
    uarr<T>& operator=(uarr<T>&& src)
    {
      m_data = src.release();
      return *this;
    }
};

#endif
