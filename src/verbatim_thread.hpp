#ifndef VERBATIM_THREAD_HPP
#define VERBATIM_THREAD_HPP

/// Thread class.
class Thread
{
  private:
    /// Actual thread.
    SDL_Thread* m_thread;

  private:
    /// Deleted copy constructor.
    Thread(const Thread&) = delete;
    /// Deleted assignment.
    Thread& operator=(const Thread&) = delete;

  public:
    /// Constructor.
    ///
    /// \param func Function pointer to run.
    /// \param data Data for function.
    Thread(int (*func)(void*), void* data) :
      m_thread(dnload_SDL_CreateThread(func, NULL, data))
    {
    }

    /// Destructor.
    ///
    /// Will implicitly join the thread.
    ~Thread()
    {
      join();
    }

  private:
    /// Join the thread.
    void join()
    {
      dnload_SDL_WaitThread(m_thread, NULL);
    }
};

#endif
