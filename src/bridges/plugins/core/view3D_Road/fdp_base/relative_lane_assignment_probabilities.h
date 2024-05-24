// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_RELATIVE_LANE_ASSIGNMENT_PROBABILITIES_H_
#define FDP_BASE_RELATIVE_LANE_ASSIGNMENT_PROBABILITIES_H_

#include "fdp_base/cem_percentage_t.h"

namespace fdp_base {

/// Probabilities for lane assignment to different areas on the road, relative to the ego lane
struct RelativeLaneAssignmentProbabilities {
  /**
  Probability for the TP to be on the ego lane

  */
  CEM_percentage_t egoLaneProbability;
  /**
  Probability for the TP to be on the left lane (L1)

  */
  CEM_percentage_t leftLaneProbability;
  /**
  Probability for the TP to be on the right lane (R1)

  */
  CEM_percentage_t rightLaneProbability;
  /**
  Probability for the TP to be in the area left to the L1 lane

  */
  CEM_percentage_t leftAreaOutsideProbability;
  /**
  Probability for the TP to be in the area right to the R1 lane

  */
  CEM_percentage_t rightAreaOutsideProbability;
  /**
  Probability for the TP to be in an unknown area of the road

  */
  CEM_percentage_t unknownAreaProbability;
};

} // namespace fdp_base

#endif // FDP_BASE_RELATIVE_LANE_ASSIGNMENT_PROBABILITIES_H_
