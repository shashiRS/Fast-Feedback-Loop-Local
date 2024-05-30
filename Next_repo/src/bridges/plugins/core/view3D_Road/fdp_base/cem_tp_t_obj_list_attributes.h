// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_LIST_ATTRIBUTES_H_
#define FDP_BASE_CEM_TP_T_OBJ_LIST_ATTRIBUTES_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Available attributes in CEM_TP_t_ObjectList and CEM_TP_t_Object values: enum {
/// TP_OBJ_ATTRIBUTE_LIST_QUALIFIERS=1,TP_OBJ_ATTRIBUTE_HYPOTHESES_TABLE=2
/// ,TP_OBJ_ATTRIBUTE_SENSOR_SOURCE_IDS=4,TP_OBJ_ATTRIBUTE_SENSOR_SOURCE=8
/// ,TP_OBJ_ATTRIBUTE_GENERAL=16,TP_OBJ_ATTRIBUTE_SPLIT_MERGE=32,
/// TP_OBJ_ATTRIBUTE_ABS_CORRELATION=64,TP_OBJ_ATTRIBUTE_REF_POINT=128
/// ,TP_OBJ_ATTRIBUTE_ADD_KINEMATICS=256,TP_OBJ_ATTRIBUTE_KINEMATICS=512
/// ,TP_OBJ_ATTRIBUTE_SHAPE_POINTS=1024,TP_OBJ_ATTRIBUTE_GEOMETRY=2048
/// ,TP_OBJ_ATTRIBUTE_CLASSIFICATION=4096,TP_OBJ_ATTRIBUTE_ADD_CLASSIFICATION=8192
/// ,TP_OBJ_ATTRIBUTE_DYN_PROP=16384,TP_OBJ_ATTRIBUTE_INTENTION=32768
/// ,TP_OBJ_ATTRIBUTE_LANE_STATUS=65536,TP_OBJ_ATTRIBUTE_QUALIFIERS=131072
/// ,TP_OBJ_ATTRIBUTE_ACC_QUALIFIERS=262144,TP_OBJ_ATTRIBUTE_EBA_QUALIFIERS=524288
/// ,TP_OBJ_ATTRIBUTE_OCCLUSION=1048576,TP_OBJ_ATTRIBUTE_EXTENSION=2097152
/// ,TP_OBJ_ATTRIBUTE_NONE=0,TP_OBJ_ATTRIBUTE_ALL=4294967295,}
/// @range{0,4294967295}
class CEM_TP_t_ObjListAttributes {
public:
  typedef uint32 underlying_type;
  enum do_not_use {
    TP_OBJ_ATTRIBUTE_NONE = 0U,
    TP_OBJ_ATTRIBUTE_LIST_QUALIFIERS = 1U,
    TP_OBJ_ATTRIBUTE_HYPOTHESES_TABLE = 2U,
    TP_OBJ_ATTRIBUTE_SENSOR_SOURCE_IDS = 4U,
    TP_OBJ_ATTRIBUTE_SENSOR_SOURCE = 8U,
    TP_OBJ_ATTRIBUTE_GENERAL = 16U,
    TP_OBJ_ATTRIBUTE_SPLIT_MERGE = 32U,
    TP_OBJ_ATTRIBUTE_ABS_CORRELATION = 64U,
    TP_OBJ_ATTRIBUTE_REF_POINT = 128U,
    TP_OBJ_ATTRIBUTE_ADD_KINEMATICS = 256U,
    TP_OBJ_ATTRIBUTE_KINEMATICS = 512U,
    TP_OBJ_ATTRIBUTE_SHAPE_POINTS = 1024U,
    TP_OBJ_ATTRIBUTE_GEOMETRY = 2048U,
    TP_OBJ_ATTRIBUTE_CLASSIFICATION = 4096U,
    TP_OBJ_ATTRIBUTE_ADD_CLASSIFICATION = 8192U,
    TP_OBJ_ATTRIBUTE_DYN_PROP = 16384U,
    TP_OBJ_ATTRIBUTE_INTENTION = 32768U,
    TP_OBJ_ATTRIBUTE_LANE_STATUS = 65536U,
    TP_OBJ_ATTRIBUTE_QUALIFIERS = 131072U,
    TP_OBJ_ATTRIBUTE_ACC_QUALIFIERS = 262144U,
    TP_OBJ_ATTRIBUTE_EBA_QUALIFIERS = 524288U,
    TP_OBJ_ATTRIBUTE_OCCLUSION = 1048576U,
    TP_OBJ_ATTRIBUTE_EXTENSION = 2097152U,
    TP_OBJ_ATTRIBUTE_ALL = 4294967295U
  };

  CEM_TP_t_ObjListAttributes() : value(static_cast<underlying_type>(TP_OBJ_ATTRIBUTE_NONE)) {}

  CEM_TP_t_ObjListAttributes(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  CEM_TP_t_ObjListAttributes(const CEM_TP_t_ObjListAttributes &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END CEM_TP_t_ObjListAttributes(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  CEM_TP_t_ObjListAttributes &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_TP_t_ObjListAttributes &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  CEM_TP_t_ObjListAttributes &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  CEM_TP_t_ObjListAttributes &operator=(const CEM_TP_t_ObjListAttributes &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_TP_t_ObjListAttributes &) operator=(const underlying_type v) {
    value = v;
    return *this;
  }

  template <typename T>
  bool operator==(const T v) const {
    return value == static_cast<const underlying_type>(v);
  }
  /// \endcond

private:
  underlying_type value;
};
} // namespace fdp_base

// PRQA S 2180 --

#endif // FDP_BASE_CEM_TP_T_OBJ_LIST_ATTRIBUTES_H_
