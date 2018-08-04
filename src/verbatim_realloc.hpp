#ifndef VERBATIM_REALLOC_HPP
#define VERBATIM_REALLOC_HPP

// Must be included prior to redefining memory management.
#include <algorithm>
#include <cfloat>
#include <cstdio>
#if defined(USE_LD)
#include <iostream>
#include <boost/throw_exception.hpp>
#endif

#if __cplusplus >= 201103L
/// Use C++11 exception specification if possible.
#define NOEXCEPT noexcept
#else
/// Otherwise use old style.
#define NOEXCEPT throw()
#endif

#if defined(USE_LD)
static size_t g_total = 0;
#endif

/// A global delete operator using free().
///
/// \param ptr Pointer to free.
void operator delete(void *ptr) NOEXCEPT
{
  if(ptr)
  {
    dnload_free(ptr);
  }
}

/// A global new operator using realloc().
///
/// \param sz Size to allocate.
/// \return Allocated pointer.
void* operator new(size_t sz)
{
#if defined(USE_LD)
  if(!sz)
  {
    BOOST_THROW_EXCEPTION(std::runtime_error("call to 'new' with size 0"));
  }
  g_total += sz;
#endif
  return dnload_realloc(NULL, sz);
}

/// Array delete.
///
/// Repolacement for delete[] using free().
/// Use with types that have destructors is not supported.
///
/// \param ptr Pointer to free.
inline void array_delete(void *ptr)
{
  if(ptr)
  {
    dnload_free(ptr);
  }
}

/// Array new.
///
/// Replacement for new[] using realloc().
/// Use with types that have destructors is not supported.
///
/// \param ptr Existing ptr (may be NULL).
/// \param count Number of elements to allocate.
/// \return Reallocated ptr.
template <typename T> inline T* array_new(T* ptr, size_t count)
{
#if defined(USE_LD)
  if(!count)
  {
    BOOST_THROW_EXCEPTION(std::runtime_error("array_new: resize to zero not supported"));
  }
#endif
  T* allocated = static_cast<T*>(dnload_realloc(ptr, sizeof(T) * count));
#if defined(USE_LD)
  if(!allocated)
  {
    std::ostringstream sstr;
    sstr << "array_new: allocating " << (sizeof(T) * count) << " bytes failed";
    BOOST_THROW_EXCEPTION(std::runtime_error(sstr.str()));
  }
  g_total += sizeof(T) * count;
#endif
  return allocated;
}

#endif
