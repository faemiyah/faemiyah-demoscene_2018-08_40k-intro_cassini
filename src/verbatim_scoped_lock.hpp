#ifndef VERBATIM_SCOPED_LOCK_HPP
#define VERBATIM_SCOPED_LOCK_HPP

#include "verbatim_mutex.hpp"

/// Scoped lock.
class ScopedLock
{
  private:
    /// Mutex that was locked.
    Mutex& m_mutex;

  public:
    /// Accessor.
    ///
    /// \return Referred mutex.
    Mutex& getMutex()
    {
      return m_mutex;
    }

  private:
    /// Deleted copy constructor.
    ScopedLock(const ScopedLock&) = delete;
    /// Deleted assignment.
    ScopedLock& operator=(const ScopedLock&) = delete;

  public:
    /// Constructor.
    ///
    /// Implicitly locks the mutex.
    ///
    /// \param op Mutex to lock.
    ScopedLock(Mutex& op)
      : m_mutex(op)
    {
      op.acquire();
    }

    /// Destructor.
    ///
    /// Implicitly unlocks the mutex.
    ~ScopedLock()
    {
      m_mutex.release();
    }
};

#endif
