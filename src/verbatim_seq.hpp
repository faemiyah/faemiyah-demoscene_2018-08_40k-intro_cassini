#ifndef VERBATIM_ARRAY_HPP
#define VERBATIM_ARRAY_HPP

#include "verbatim_realloc.hpp"

/// Sequential memory array.
///
/// Replacement for std::vector, uses realloc for memory management.
template<typename T> class seq
{
  private:
    /// Starting capacity.
    static const unsigned DEFAULT_CAPACITY = 8;

  public:
    /// Convenience typedef.
    typedef T value_type;

    /// Iterator type.
    typedef T* iterator;

    /// Const iterator type.
    typedef const T* const_iterator;

  private:
    /// Array data.
    T *m_data;

    /// Array size.
    unsigned m_size;

    /// Array capacity.
    unsigned m_capacity;

  private:
    /// Deleted copy constructor.
    seq(const seq<T>&) = delete;
    /// Deleted assignment.
    seq<T>& operator=(const seq<T>&) = delete;

  public:
    /// Constructor.
    ///
    /// \param op Starting capacity.
    explicit seq(unsigned op = DEFAULT_CAPACITY) :
      m_data(array_new(static_cast<T*>(NULL), op)),
      m_size(0),
      m_capacity(op) { }

    /// Destructor.
    ~seq()
    {
      destructInternal();
      array_delete(m_data);
    }

  private:
#if defined(USE_LD) && defined(DEBUG)
    /// Check if access is correct, throw error if not.
    ///
    /// \param idx Index to check.
    void accessCheck(unsigned idx) const
    {
      if((static_cast<unsigned>(m_size) <= idx) || (0 > idx))
      {
        std::ostringstream sstr;
        sstr << "accessing index " << idx << " from seq of size " << m_size;
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
    }
    /// Check if access is correct, throw error if not.
    ///
    /// \param idx Index to check.
    void accessCheck(int idx) const
    {
      if(0 > idx)
      {
        std::ostringstream sstr;
        sstr << "accessing negative index " << idx << " from seq";
        BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
      }
      return accessCheck(static_cast<unsigned>(idx));
    }
#endif

    /// Boundary check for index.
    ///
    /// \param idx Index to check.
    /// \return Reference type accessed.
    T& accessIndex(unsigned idx)
    {
#if defined(USE_LD) && defined(DEBUG)
      accessCheck(idx);
#endif
      return m_data[idx];
    }
    /// Boundary check for index (const version).
    ///
    /// \param idx Index to check.
    /// \return Reference type accessed.
    const T& accessIndex(unsigned idx) const
    {
#if defined(USE_LD) && defined(DEBUG)
      accessCheck(idx);
#endif
      return m_data[idx];
    }
    /// Boundary check for index.
    ///
    /// \param idx Index to check.
    /// \return Reference type accessed.
    T& accessIndex(int idx)
    {
#if defined(USE_LD) && defined(DEBUG)
      accessCheck(idx);
#endif
      return m_data[idx];
    }
    /// Boundary check for index (const version).
    ///
    /// \param idx Index to check.
    /// \return Reference type accessed.
    const T& accessIndex(int idx) const
    {
#if defined(USE_LD) && defined(DEBUG)
      accessCheck(idx);
#endif
      return m_data[idx];
    }

    /// Clear internals.
    void destructInternal()
    {
      for(T &vv : *this)
      {
        vv.~T();
      }
    }

    /// Grow if necessary.
    ///
    /// \return Pointer to the end of sequence.
    T* growCheck()
    {
      if(m_size >= m_capacity)
      {
        resizeInternal(m_capacity * 2);
      }
      return end();
    }

    /// Internal resize.
    ///
    /// \param cnt New size.
    void resizeInternal(unsigned cnt)
    {
      m_data = array_new(m_data, cnt);
      m_capacity = cnt;
    }

  public:
    /// Accessor.
    ///
    /// \return Last element.
    T& back()
    {
      return accessIndex(m_size - 1);
    }
    /// Accessor.
    ///
    /// \return Last element.
    const T& back() const
    {
      return accessIndex(m_size - 1);
    }

    /// Get iterator.
    ///
    /// \return Iterator to begin.
    iterator begin() const
    {
      return m_data;
    }
    /// Get const iterator.
    ///
    /// \return Const iterator to begin.
    const_iterator cbegin() const
    {
      return m_data;
    }

    /// Accessor.
    ///
    /// \return Current capacity.
    unsigned capacity() const
    {
      return m_capacity;
    }

    /// Tell if the sequence is empty.
    ///
    /// \return True if yes, false if no.
    bool empty() const
    {
      return (0 >= m_size);
    }
    
    /// Get iterator.
    ///
    /// \return Iterator to end.
    iterator end() const
    {
      return &(m_data[m_size]);
    }
    /// Get iterator.
    ///
    /// \return Iterator to end.
    const_iterator cend() const
    {
      return &(m_data[m_size]);
    }

    /// Accessor.
    ///
    /// \return First element.
    T& front()
    {
      return accessIndex(0);
    }
    /// Accessor.
    ///
    /// \return First element.
    const T& front() const
    {
      return accessIndex(0);
    }

    /// Accessor.
    ///
    /// \return Pointer to data.
    T* getData()
    {
      return begin();
    }
    /// Const accessor.
    ///
    /// \return Pointer to data.
    const T* getData() const
    {
      return begin();
    }

    /// Get byte size.
    ///
    /// \return Current size in bytes.
    unsigned getSizeBytes() const
    {
      return m_size * sizeof(T);
    }

    /// Resize to given size.
    ///
    /// \param cnt Count.
    void resize(unsigned cnt)
    {
      if(m_size == cnt)
      {
        return;
      }

      // Ensure capacity.
      if(cnt > m_capacity)
      {
        resizeInternal(cnt);
      }

      for(unsigned ii = m_size; (ii < cnt); ++ii)
      {
        new(&m_data[ii]) T();
      }
      
      for(unsigned ii = m_size; (ii > cnt); --ii)
      {
        m_data[ii - 1].~T();
      }

      m_size = cnt;
    }

    /// Get array size.
    ///
    /// \return Current size.
    unsigned size() const
    {
      return m_size;
    }

#if defined(__LP64__)
    /// Access operator.
    ///
    /// \return Element reference.
    T& operator[](size_t idx)
    {
      return accessIndex(static_cast<unsigned>(idx));
    }
    /// Const access operator.
    ///
    /// \return Element reference.
    const T& operator[](size_t idx) const
    {
      return accessIndex(static_cast<unsigned>(idx));
    }
#endif
    /// Access operator.
    ///
    /// \return Element reference.
    T& operator[](unsigned idx)
    {
      return accessIndex(idx);
    }
    /// Const access operator.
    ///
    /// \return Element reference.
    const T& operator[](unsigned idx) const
    {
      return accessIndex(idx);
    }
    /// Access operator wrapper.
    ///
    /// \return Element reference.
    T& operator[](int idx)
    {
      return accessIndex(idx);
    }
    /// Const access operator wrapper.
    ///
    /// \return Element reference.
    const T& operator[](int idx) const
    {
      return accessIndex(idx);
    }

    /// Truth value operator.
    ///
    /// \return True if sequence is not empty.
    operator bool() const
    {
      return (0 < m_size);
    }

    /// Clear the array.
    ///
    /// Will NOT resize the array, the capacity will remain.
    void clear()
    {
      destructInternal();
      m_size = 0;
    }

    /// Remove last element from the array.
    ///
    /// Calling this function on an empty array has undefined results.
    void pop_back()
    {
      back().~T();
      --m_size;
    }

    /// Add an element into array.
    ///
    /// \param op Element to add.
    T& push_back(const T &op)
    {
      new(growCheck()) T(op);
      return m_data[m_size++];
    }

    /// Emplace an element into array.
    ///
    /// \param args Arguments.
    /// \return Reference to newly emplaced object.
    template<typename...Args> T& emplace_back(Args&&...args)
    {
      new(growCheck()) T(args...);
      return m_data[m_size++];
    }
};

#endif
