#ifndef VERBATIM_UPTR_HPP
#define VERBATIM_UPTR_HPP

#include "verbatim_realloc.hpp"

/// Replacement for std::unique_ptr.
template<typename T> class uptr
{
  private:
    /// Object reference.
    T* m_ref;

  private:
    /// Deleted copy constructor.
    uptr(const uptr<T>&) = delete;
    /// Deleted assignment.
    uptr<T>& operator=(const uptr<T>&) = delete;

  public:
    /// Empty constructor.
    explicit uptr() :
      m_ref(NULL)
    {
    }

    /// Constructor.
    ///
    /// \param op Pointer to take control of.
    explicit uptr(T *op) :
      m_ref(op)
    {
    }

    /// Move constructor.
    ///
    /// Transfers ownership.
    ///
    /// \param op Source.
    uptr(uptr<T> &&op) :
      m_ref(op.release())
    {
    }

    /// Destructor.
    ~uptr()
    {
      erase();
    }

  private:
    /// Delete contents if they exist.
    ///
    /// Will not actually modify the content pointer.
    void erase()
    {
      delete m_ref;
    }

    /// Release ownership, return released pointer.
    ///
    /// Should not be called by user.
    T* release()
    {
      T* ret = m_ref;
      m_ref = NULL;
      return ret;
    }

  public:
    /// Accessor.
    ///
    /// \return Referenced pointer.
    T* get() const
    {
      return m_ref;
    }
    /// Pointer operator.
    ///
    /// \return Pointer.
    T* operator->() const
    {
      return m_ref;
    }

    /// Replace contents.
    ///
    /// \param op New contents.
    void reset(T* op = NULL)
    {
      erase();
      m_ref = op;
    }

    /// Dereference operator.
    ///
    /// \return Dereferenced contents.
    T& operator*()
    {
      return *m_ref;
    }
    /// Const dereference operator.
    ///
    /// \return Dereferenced contents.
    const T& operator*() const
    {
      return *m_ref;
    }

    /// Assignment operator.
    ///
    /// Takes ownership.
    ///
    /// \param op Source.
    uptr<T>& operator=(T *op)
    {
      reset(op);
      return *this;
    }
    /// Move operator.
    ///
    /// Transfers ownership.
    ///
    /// \param op Source.
    uptr<T>& operator=(uptr<T> &&op)
    {
      reset(op.m_ref);
      op.m_ref = NULL;
      return *this;
    }

    /// Truth value operator.
    ///
    /// \return True if pointer is not empty.
    operator bool() const
    {
      return (m_ref != NULL);
    }
};

#endif
