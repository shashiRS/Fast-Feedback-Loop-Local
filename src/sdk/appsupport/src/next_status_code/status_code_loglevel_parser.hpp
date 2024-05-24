#ifndef NEXT_STATUS_CODE_LOGLEVEL_PARSER_HPP
#define NEXT_STATUS_CODE_LOGLEVEL_PARSER_HPP

#include <next/appsupport/next_status_code/next_status_code.hpp>
#include <next/sdk/logger/logger_data_types.hpp>

namespace next {
namespace appsupport {
namespace loglevel_parser {

next::sdk::logger::LOGLEVEL LogLevelFromStatusCode(next::appsupport::StatusCode statusCode) {
  int status_code_as_int = static_cast<int>(statusCode);

  int status_code_level = status_code_as_int / 100;
  next::sdk::logger::LOGLEVEL log_level;
  switch (status_code_level) {
  case 2:
    log_level = sdk::logger::LOGLEVEL::INFO;
    break;
  case 3:
    log_level = sdk::logger::LOGLEVEL::WARN;
    break;
  case 4:
    log_level = sdk::logger::LOGLEVEL::ERR;
    break;
  default:
    log_level = sdk::logger::LOGLEVEL::DEBUG;
  }
  return log_level;
}

} // namespace loglevel_parser
} // namespace appsupport
} // namespace next

#endif // NEXT_STATUS_CODE_LOGLEVEL_PARSER_HPP
