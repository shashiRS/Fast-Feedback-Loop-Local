// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_SPLIT_MERGE_H_
#define FDP_BASE_CEM_TP_T_OBJ_SPLIT_MERGE_H_

#include "Platform_Types.h"
#include "fdp_base/cem_tp_t_obj_split_merge_states.h"

namespace fdp_base {

/// Split/Merge state of the object
struct CEM_TP_t_ObjSplitMerge {
  /**
  Additional Information indicating a merge or split operation

  */
  CEM_TP_t_ObjSplitMergeStates eSplitMergeState;
  /**
  Used as additional information to indicate a merge. This object
slot contained in the last cycle an object which was merged
to the referred ID.
TP_OBJECT_SM_ID_NONE means no merge
TP_OBJECT_SM_ID_UNKNOWN
means that the merge partner is not available in the generic
object list.

  */
  uint8 uiMergeID;
  /**
  Used as additional information to indicate a split This object
slot contained in the last cycle an object which was splitted
to the referred ID.
TP_OBJECT_SM_ID_NONE means no split
TP_OBJECT_SM_ID_UNKNOWN
means that the split partner is not available in the generic
object list.

  */
  uint8 uiSplitID;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_SPLIT_MERGE_H_
