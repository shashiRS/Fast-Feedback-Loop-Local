/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     gia_interfaces.h
 *  @brief    interfaces to access GIA adapter and converters
 */

#ifndef NEXT_UDEX_GIA_INTERFACES
#define NEXT_UDEX_GIA_INTERFACES

#include <string.h>
#include "version_info.hpp"

typedef enum eSimPortDataTypes {
  simUI8_t = 0,
  simI8_t,
  simUI16_t,
  simI16_t,
  simUI32_t,
  simI32_t,
  simF32_t,
  simI64_t,
  simF64_t,
  simOPAQUE_t,
  simREFERENCE_t,
  simUI64_t /*,
   simBUNDLE_t*/
} SimPortDataTypes_t;

typedef void *(*SimAlloc_t)(int cbData);
typedef void (*SimFree_t)(void *pData);
typedef void (*SimLock_t)(void *pData, int bLock);

struct ISetFcPointer {
  virtual void SetMemFcPointer(SimAlloc_t FuncAlloc, SimFree_t FuncFree, SimLock_t FuncLock) = 0;
};

struct IConverterDestroy {
  virtual void Destroy() = 0;
};

struct IRPortDefaultValue {
  virtual void *GetRPortDefaultValue(unsigned long &ulDefaultValueSize) = 0;
  virtual void SetRPortDefaultValue(void *pDefaultValue, unsigned long ulDefaultValueSize) = 0;
};

struct IConverter : public IConverterDestroy, public IRPortDefaultValue {
  virtual const wchar_t *GetTypeName() = 0;

  virtual long GetNumDependencies(unsigned long &ulNumDeps) = 0;
  virtual long GetDependencyName(unsigned long ulDepIndex, wchar_t *szDependencyName,
                                 unsigned long ulMaxDepNameLength) = 0;
  virtual long GetDependencyName(unsigned long ulDepIndex, char *szDependencyName,
                                 unsigned long ulMaxDepNameLength) = 0;
  virtual long SetDependency(const wchar_t *szDependencyName, SimPortDataTypes_t eDepType, unsigned long ulDepSize,
                             const IVersionInfo *pDepVersionInfo, const wchar_t *szDepTypeName, void *pDepData) = 0;

  virtual bool AreCompatible(SimPortDataTypes_t eRPortType, unsigned long ulRPortSize,
                             const IVersionInfo &rPortVersionInfo, SimPortDataTypes_t ePPortType,
                             unsigned long ulPPortSize, const IVersionInfo *pPPortVersionInfo,
                             const wchar_t *szPPortTypeName) = 0;

  /** returns:
   *   SIM_ERR_OK - when this value can be used
   *   SIM_ERR_USE_VALUE_FROM_PPORT - when the conversion is not implemented here
   *                                  and the value from Provide Port must be used
   */
  virtual long ConvertData(SimPortDataTypes_t eRPortType, unsigned long ulRPortSize,
                           const IVersionInfo &rPortVersionInfo, SimPortDataTypes_t ePPortType,
                           unsigned long ulPPortSize, const IVersionInfo *pPPortVersionInfo, void *pPPortData,
                           void **pOutData, unsigned long &ulOutSize) = 0;

  /** returns:
   *   SIM_ERR_OK - when this value can be used
   *   SIM_ERR_USE_DEFAULT_VALUE_FROM_RPORT - when the default vaule is not implemented here
   *                                            and the default value from Request Port must be used
   */
  virtual long GetDefaultValue(SimPortDataTypes_t eRPortType, unsigned long ulRPortSize,
                               const IVersionInfo &rPortVersionInfo, void **pDefaultData,
                               unsigned long &ulDefaultSize) = 0;

  virtual ISetFcPointer *GetFcSetInterface() = 0;
};

struct IAdapterAccess {
  virtual void *GetInterface(wchar_t *szIfcName) = 0;
};

struct IAdapter {
  virtual IConverter *CreateConverter(const wchar_t *szTypeName) = 0;
};

#endif
