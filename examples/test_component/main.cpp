/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * ========================== NEXT Project ==================================
 */

#include "main.h"
#include "test_component.hpp"

DLL_FUNCTION_EXPORT int nextDllMain(int argc, const char *argv[]) {
  TestComponent node("TestNode");
  node.parseCmd(argc, argv);
  node.run();

  return (0);
}

DLL_FUNCTION_EXPORT const char *nextDllName(void) { return "test_component"; }

DLL_FUNCTION_EXPORT int nextDllVersion(void) { return 0; }

int main(int argc, const char **argv) { return nextDllMain(argc, argv); }
