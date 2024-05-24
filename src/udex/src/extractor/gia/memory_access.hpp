/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     memory_access.hpp
 *  @brief    handling of memory alocation, locking and dealocation
 */

#ifndef NEXT_UDEX_MEMORY_ACCESS
#define NEXT_UDEX_MEMORY_ACCESS

#include <map>

struct IMemoryAccess {
  virtual void *MemAlloc(int nDataSize) = 0;
  virtual void MemFree(void *pData) = 0;
  virtual void MemLock(void *pData, int bLock) = 0;
  virtual bool IsPointerUsable(void *pData) = 0;
};

class CMemoryAccess : public IMemoryAccess {
private:
  CMemoryAccess() {}

public:
  virtual ~CMemoryAccess() {}

  inline static IMemoryAccess &GetInstance() { return instance_; }

  inline static void *AllocMemory(int nDataSize) {
    void *pData{nullptr};
    // void* pData = ::MemAlloc(nDataSize, MEMMGR_FLAG_DEFAULT);
    if (0 < nDataSize) {
      size_t size = (size_t)nDataSize;
      pData = malloc(size);
    }
    if (pData) {
      //::MemLock(pData); // avoid auto free
      pointers_[pData] = 1; // insert new pointer in the pointer map
    }

    return pData;
  }

  inline static void FreeMemory(void *pData) {
    if (pData) {
      pointers_[pData]--;
      if (pointers_[pData] == 0)
        pointers_.erase(pData);
      //::MemUnlock(pData); // unlock because it was locked after allocation
      //::MemFree(pData);
      free(pData);
    }
  }

  inline static void LockMemory(void *pData, int bLock) {
    // try/catch block is used to prevent crash when "non-MTS" pointers are used in LockMemory starting with MTS2.6
    try {
      if (bLock) {
        if (pData) {
          //::MemLock(pData);
          pointers_[pData]++;
        }
      } else {
        if (pData && pointers_.find(pData) != pointers_.end()) {
          //::MemUnlock(pData);
          pointers_[pData]--;
          if (pointers_[pData] == 0)
            pointers_.erase(pData);
        }
      }
    } catch (...) {
      //::ReportError(err_severity_exception, err_category_exception, 0, 0U, 0, L"Memory operation on Non-MTS pointer;
      // Operation not executed.");
    }
  }

  inline static bool IsPointerManaged(void *pData) {
    if (pData && pointers_.find(pData) != pointers_.end())
      return true;
    // There are cases when pData is a usable pointer but not allocated from this wrapper
    // For this we try to lock/unlock it and this will let us know if the pointer is ok or not
    try {
      //::MemLock(pData);
      //::MemUnlock(pData);
    } catch (...) {
      return false;
    }

    return true;
  }

  // IMemoryAccess
  void *MemAlloc(int nDataSize) {
    std::lock_guard lock(memory_mutex_);
    return CMemoryAccess::AllocMemory(nDataSize);
  }

  void MemFree(void *pData) {
    std::lock_guard lock(memory_mutex_);
    CMemoryAccess::FreeMemory(pData);
  }

  void MemLock(void *pData, int bLock) {
    std::lock_guard lock(memory_mutex_);
    CMemoryAccess::LockMemory(pData, bLock);
  }

  void *GetInterface(wchar_t *szIfcName) {
    if (wcscmp(szIfcName, L"IMemoryAccess") == 0) {
      return static_cast<IMemoryAccess *>(this);
    }
  }

  bool IsPointerUsable(void *pData) {
    std::lock_guard lock(memory_mutex_);
    return IsPointerManaged(pData);
  }

private:
  static CMemoryAccess instance_;
  // map used to store all the pointers to the memory allocated with this wrapper;
  // void* -> pointer to the memory
  // unsigned int - lock/unlock counter
  static std::map<void *, unsigned int> pointers_;
  std::mutex memory_mutex_;
};

#endif
