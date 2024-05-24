/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * ========================== NEXT Project ==================================
 */

#include "simulation_node.hpp"

int main(int argc, const char **argv) {
  SimulationNode node("TestNode");
  node.parseCmd(argc, argv);
  node.run();
}
