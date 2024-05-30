/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef UDEX_CORE_LIB_WRAP_DATA_DESCRIPTOR_PARSER
#define UDEX_CORE_LIB_WRAP_DATA_DESCRIPTOR_PARSER

#include <map>

#include <next/sdk/sdk_macros.h>

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/extensibility/sources.h>
#include <mts/extensibility/package.hpp>
#include <mts/introspection/conversion.hpp>
#include <mts/introspection/xtypes/dynamic_type.hpp>
#include <mts/introspection/xtypes/parser/json.hpp>
NEXT_RESTORE_WARNINGS

#include <boost/property_tree/ptree.hpp>
#include <next/udex/data_types.hpp>

namespace CoreLibWrap {

namespace pt = boost::property_tree;

using signal_list = std::vector<std::pair<std::string, next::udex::PayloadType>>;

class DataDescriptionParser {
public:
  bool GetSignalInfo(const std::string &topic, const std::string &url, const std::string &schema,
                     next::sdk::types::PackageFormatType format_type, next::udex::Signal &signal);

  std::pair<std::string, pt::ptree> parseDescription(const std::string_view &description);

  bool ExtractNameAndIndexFromArraySignal(size_t &index, std::string &signal_name);

private:
  void CalculateOffsets(const mts::introspection::xtypes::dynamic_type &dynamic_type_node, size_t depth,
                        signal_list &signals, size_t &size);

  pt::ptree fillSignalTree(const mts::introspection::xtypes::dynamic_type &dynamic_type_node, const size_t &depth);

private:
  std::map<std::string, next::udex::Signal> url_signal_info_cache_;
  std::map<std::string, mts::introspection::xtypes::dynamic_type::ptr> topic_dynamic_type_;
  std::map<std::string, std::pair<signal_list, size_t>> topic_signal_list_cache_;
};

} // namespace CoreLibWrap
#endif
