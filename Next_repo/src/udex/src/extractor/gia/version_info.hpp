/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     version_info.hpp
 *  @brief    handles version formats ( x.x.x.x, string and single value long)
 */

#ifndef NEXT_UDEX_VERSION_INFO
#define NEXT_UDEX_VERSION_INFO

#include <exception>

template <typename DEST_TYPE>
class CTryCast {
  CTryCast() {}

public:
  template <typename SRC_TYPE>
  static DEST_TYPE *PtrCast(SRC_TYPE *pSrcType) {
    try {
      DEST_TYPE *pDestType = dynamic_cast<DEST_TYPE *>(pSrcType);
      return pDestType;
    } catch (std::exception & /*ignoredExc*/) {
    }

    return NULL;
  }
};

struct IVersionInfoDestroy {
  virtual void Destroy() = 0;
};

struct IClonable {
  virtual IClonable *Clone() const = 0;
};

struct IVersionInfo : public IClonable, public IVersionInfoDestroy {
  virtual bool Equals(const IVersionInfo *pIVersionInfo) const = 0;

  virtual const wchar_t *ToString() const = 0;
};

class CVersionInfoImpl : public IVersionInfo {
public:
  CVersionInfoImpl(unsigned char ucMajor, unsigned char ucMinor, unsigned char ucPatch, unsigned char ucBugFix = 0)
      : major_(ucMajor), minor_(ucMinor), patch_(ucPatch), bugfix_(ucBugFix) {
    wchar_t szTmp[256] = L"";
    if (ucBugFix == 0) {
      swprintf(szTmp, sizeof(szTmp) / sizeof(wchar_t), L"%u.%u.%u", ucMajor, ucMinor, ucPatch);
    } else {
      swprintf(szTmp, sizeof(szTmp) / sizeof(wchar_t), L"%u.%u.%u.%u", ucMajor, ucMinor, ucPatch, ucBugFix);
    }

    toString = szTmp;
  }

  CVersionInfoImpl(const CVersionInfoImpl &versionInfo) {
    major_ = versionInfo.major_;
    minor_ = versionInfo.minor_;
    patch_ = versionInfo.patch_;
    bugfix_ = versionInfo.bugfix_;
    toString = versionInfo.toString;
  }

  CVersionInfoImpl(const long version) {
    major_ = static_cast<unsigned char>((version >> 16) & 0xFF);
    minor_ = static_cast<unsigned char>((version >> 8) & 0xFF);
    patch_ = static_cast<unsigned char>(version & 0xFF);
    bugfix_ = 0;

    wchar_t szTmp[256] = L"";
    swprintf(szTmp, sizeof(szTmp) / sizeof(wchar_t), L"%u.%u.%u", major_, minor_, patch_);

    toString = szTmp;
  }

  virtual ~CVersionInfoImpl() = default;

  void IsNew(){};

  bool Equals(const IVersionInfo *pIVersionInfo) const {
    const CVersionInfoImpl *pVersionInfo = CTryCast<const CVersionInfoImpl>::PtrCast(pIVersionInfo);
    if (pVersionInfo) {
      return major_ == pVersionInfo->major_ && minor_ == pVersionInfo->minor_ && patch_ == pVersionInfo->patch_ &&
             bugfix_ == pVersionInfo->bugfix_;
    }

    return false;
  }

  const wchar_t *ToString() const { return toString.c_str(); }

  IClonable *Clone() const { return new CVersionInfoImpl(major_, minor_, patch_, bugfix_); }

  void Destroy() { delete this; }

  inline unsigned char GetMajor() const { return major_; }

  inline unsigned char GetMinor() const { return minor_; }

  inline unsigned char GetPatch() const { return patch_; }

  inline unsigned char GetBugfix() const { return bugfix_; }

private:
  unsigned char major_;
  unsigned char minor_;
  unsigned char patch_;
  unsigned char bugfix_;
  std::wstring toString;
};

#endif
