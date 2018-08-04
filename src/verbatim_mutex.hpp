#ifndef VERBATIM_MUTEX_HPP
#define VERBATIM_MUTEX_HPP

/// Mutex class.
class Mutex
{
  private:
    /// Actual mutex.
    SDL_mutex *m_mutex;

  public:
    /// Accessor.
    ///
    /// \return Inner mutex.
    SDL_mutex* getInnerMutex()
    {
      return m_mutex;
    }

  private:
    /// Deleted copy constructor.
    Mutex(const Mutex&) = delete;
    /// Deleted assignment.
    Mutex& operator=(const Mutex&) = delete;

  public:
    /// Constructor.
    Mutex() :
      m_mutex(dnload_SDL_CreateMutex())
    {
    }

    /// Destructor.
    ~Mutex()
    {
      dnload_SDL_DestroyMutex(m_mutex);
    }

  public:
    /// Lock.
    void acquire()
    {
      dnload_SDL_LockMutex(m_mutex);
    }

    /// Unlock.
    void release()
    {
      dnload_SDL_UnlockMutex(m_mutex);
    }
};

#endif
