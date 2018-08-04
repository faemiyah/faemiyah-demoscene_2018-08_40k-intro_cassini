#ifndef VERBATIM_OPT_HPP
#define VERBATIM_OPT_HPP

/// Optional value.
///
/// Replacement for std::optional.
template<typename T> class opt
{
  private:
    /// Container for optional data.
    uint8_t m_data[sizeof(T)];

    /// Flag for initialization.
    bool m_initialized;

  public:
    /// Constructor.
    explicit opt() :
      m_initialized(false)
    {
    }

    /// Constructor with value.
    ///
    /// \param op Value.
    explicit opt(const T& op) :
      m_initialized(true)
    {
      new(getPtr()) T(op);
    }

    /// Move constructor.
    ///
    /// \param op Value.
    opt(T&& op) :
      m_initialized(true)
    {
      new(getPtr()) T(op);
    }

    /// Constructor with optional value.
    ///
    /// \param op Value.
    explicit opt(const opt<T>& op) :
      m_initialized(false)
    {
      if(op)
      {
        new(getPtr()) T(*op);
        m_initialized = true;
      }
    }

    /// Constructor with optional value.
    ///
    /// \param op Value.
    opt(opt<T>&& op) :
      m_initialized(false)
    {
      if(op)
      {
        new(getPtr()) T(*op);
        op.destruct();
        m_initialized = true;
      }
    }

    /// Destructor.
    ~opt()
    {
      destruct();
    }

  private:
#if defined(USE_LD) && defined(DEBUG)
    /// Perform access check.
    ///
    /// Throws an error if the optional value is not initialized.
    void accessCheck() const
    {
      if(!hasValue())
      {
        BOOST_THROW_EXCEPTION(std::runtime_error("optional value is uninitialized"));
      }
    }
#endif

    /// Destruct the contained object if it exists.
    void destruct()
    {
      if(hasValue())
      {
        get().~T();
        m_initialized = false;
      }
    }

    /// Get the pointer value top internal data.
    ///
    /// \return Pointer to internal data.
    T* getPtr()
    {
      return reinterpret_cast<T*>(m_data);
    }
    /// Get the pointer value top internal data (const version).
    ///
    /// \return Pointer to internal data.
    const T* getPtr() const
    {
      return reinterpret_cast<const T*>(m_data);
    }

    /// Tell if this optional value is initialized.
    ///
    /// \return True if yes, false if no.
    bool hasValue() const
    {
      return m_initialized;
    }

  public:
    /// Get the contained object.
    ///
    /// \return Contained object.
    T& get()
    {
#if defined(USE_LD) && defined(DEBUG)
      accessCheck();
#endif
      return *getPtr();
    }
    /// Get the contained object (const version).
    ///
    /// \return Contained object.
    const T& get() const
    {
#if defined(USE_LD) && defined(DEBUG)
      accessCheck();
#endif
      return *getPtr();
    }

  public:
    /// Dereference operator.
    ///
    /// \return Contained object.
    T& operator*()
    {
      return get();
    }
    /// Dereference operator (const version).
    ///
    /// \return Contained object.
    const T& operator*() const
    {
      return get();
    }

    /// Member access operator.
    ///
    /// \return Contained object.
    T* operator->()
    {
      return getPtr();
    }
    /// Member access operator.
    ///
    /// \return Contained object.
    const T* operator->() const
    {
      return getPtr();
    }

    /// Boolean operator.
    ///
    /// \return True if this optional value is populated.
    operator bool() const
    {
      return hasValue();
    }

    /// Assignment operator.
    ///
    /// \param op Assigned object.
    opt<T>& operator=(const T& op)
    {
      destruct();
      new(getPtr()) T(op);
      m_initialized = true;
      return *this;
    }
    /// Move operator.
    ///
    /// \param op Assigned object.
    opt<T>& operator=(T&& op)
    {
      destruct();
      new(getPtr()) T(op);
      m_initialized = true;
      return *this;
    }
    /// Assignment operator.
    ///
    /// Moves complete optional.
    ///
    /// \param op Assigned object.
    opt<T>& operator=(const opt<T>& op)
    {
      destruct();
      if(op)
      {
        new(getPtr()) T(op);
        m_initialized = true;
      }
      return *this;
    }
    /// Move operator.
    ///
    /// Moves complete optional.
    ///
    /// \param op Assigned object.
    opt<T>& operator=(opt<T>&& op)
    {
      destruct();
      if(op)
      {
        new(getPtr()) T(op);
        op.destruct();
        m_initialized = true;
      }
      return *this;
    }
};

#endif
