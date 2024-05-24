// COMPANY:   Continental Automotive
// COMPONENT: Eco
#ifndef ECO_I_MUTEX_H_
#define ECO_I_MUTEX_H_

namespace eco {
/// Provide read and write interfaces for a mutex (lock / unlock) functionality.
class IMutex {

public:
  /// Virtual desctructor.
  virtual ~IMutex() {}

  /// @brief Lock the mutex for write access.
  virtual void lock() = 0;

  /// @brief Lock the mutex for read access.
  virtual void lockRead() = 0;

  /// @brief Unlock the mutex for write access.
  virtual void unlock() = 0;

  /// @brief Unlock the mutex for read access.
  virtual void unlockRead() = 0;
};

} // namespace eco

#endif // ECO_I_MUTEX_H_
