#ifndef VERBATIM_COND_HPP
#define VERBATIM_COND_HPP

#include "verbatim_scoped_lock.hpp"

/// Condition variable.
class Cond
{
  private:
    /// Actual cond.
    SDL_cond *m_cond;

  private:
    /// Deleted copy constructor.
    Cond(const Cond&) = delete;
    /// Deleted assignment.
    Cond& operator=(const Cond&) = delete;

  public:
    /// Constructor.
    Cond() :
      m_cond(dnload_SDL_CreateCond())
    {
    }

    /// Destructor.
    ~Cond()
    {
      dnload_SDL_DestroyCond(m_cond);
    }

  public:
    /// Signal the cond.
    void signal()
    {
      dnload_SDL_CondSignal(m_cond);
    }

    /// Wait on cond.
    ///
    /// \param mutex Mutex (already locked).
    void wait(Mutex& mutex)
    {
      dnload_SDL_CondWait(m_cond, mutex.getInnerMutex());
    }

    /// Wait on scoped lock.
    ///
    /// \param scoped_lock Scoped lock (already held).
    void wait(ScopedLock& scoped_lock)
    {
      wait(scoped_lock.getMutex());
    }
};

#endif
