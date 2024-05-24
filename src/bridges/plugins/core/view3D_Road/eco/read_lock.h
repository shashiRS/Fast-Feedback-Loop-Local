// COMPANY:   Continental Automotive
// COMPONENT: Eco
#ifndef ECO_READ_LOCK_H_
#define ECO_READ_LOCK_H_

#include "i_mutex.h"

namespace eco {
/// @brief Class to lock a mutex for reading.
///        When the lock class is destructed / goes out of scope, the mutex is released.
class ReadLock {
public:
  /// @brief Constructor. Locks the mutex for reading.
  /// @param mutex - The mutex to lock
  explicit ReadLock(::eco::IMutex &mutex) : m_mutex(mutex) { m_mutex.lockRead(); }

  /// @brief Destructor. Release the mutex read lock.
  ~ReadLock() { m_mutex.unlockRead(); }

private:
  /// @brief Private copy-constructor, no copy
  ReadLock(const ReadLock &);

  /// @brief Private assignment operator, no copy
  ReadLock &operator=(const ReadLock &);

  /// The mutex to lock
  ::eco::IMutex &m_mutex;
};
} // namespace eco
#endif // ECO_READ_LOCK_H_
