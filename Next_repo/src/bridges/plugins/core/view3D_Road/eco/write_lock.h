// COMPANY:   Continental Automotive
// COMPONENT: CTB

#ifndef ECO_WRITE_LOCK_H_
#define ECO_WRITE_LOCK_H_

#include "i_mutex.h"

namespace eco {
/// @brief Class to lock a mutex for writing.
///        When the lock class is destructed / goes out of scope, the mutex is released.
class WriteLock {
public:
  /// @brief Constructor. Locks the mutex for writing.
  /// @param mutex - The mutex to lock
  explicit WriteLock(::eco::IMutex &mutex) : m_mutex(mutex) { m_mutex.lock(); }

  /// @brief Destructor. Release the mutex read lock.
  ~WriteLock() { m_mutex.unlock(); }

private:
  /// @brief Private copy-constructor, no copy
  WriteLock(const WriteLock &);

  /// @brief Private assignment operator, no copy
  WriteLock &operator=(const WriteLock &);

  /// The mutex to lock
  ::eco::IMutex &m_mutex;
};
} // namespace eco
#endif // ECO_WRITE_LOCK_H_
