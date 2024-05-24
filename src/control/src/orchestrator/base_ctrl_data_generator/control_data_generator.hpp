/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     control_data_generator.h
 * @brief    generation of control data for components
 *
 **/

#include "base_ctrl_config_reader.hpp"
#include "base_ctrl_data_pool.hpp"
#include "tree_extractor_publisher.h"

#include <string>

namespace next {
namespace control {
namespace orchestrator {

class ControlDataGenerator {
public:
  ControlDataGenerator(std::string flow_id);
  bool initControlDataGenerator();
  bool generateAndSendControlData(uint64_t timestamp, const std::string &component, const std::string &method);
  bool isConnected();
  virtual ~ControlDataGenerator();

private:
  std::string flow_id_;
  BaseControlDataPool data_pool_;
  std::map<std::string, std::shared_ptr<TreeExtractorPublisher>> data_publishers_;

public:
  friend class ControlDataGeneratorTest;
};

} // namespace orchestrator
} // namespace control
} // namespace next
