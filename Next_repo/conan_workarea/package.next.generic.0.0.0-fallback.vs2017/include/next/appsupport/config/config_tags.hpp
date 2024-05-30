#ifndef NEXT_CONFIG_TAGS_H_
#define NEXT_CONFIG_TAGS_H_

#include <string>
#include <unordered_map>

namespace next {
namespace appsupport {
namespace configtags {
/**
 * @brief enum for different available config tags
 *
 */
enum class TAG { ACTIVE, MANUAL_UPDATED, READONLY };
const std::unordered_map<TAG, std::string> kTagStr = {
    {TAG::ACTIVE, "ACTIVE"}, {TAG::MANUAL_UPDATED, "MANUAL_UPDATED"}, {TAG::READONLY, "READONLY"}};

const std::unordered_map<std::string, TAG> kTagStrToEnum = {
    {"ACTIVE", TAG::ACTIVE}, {"MANUAL_UPDATED", TAG::MANUAL_UPDATED}, {"READONLY", TAG::READONLY}};
} // namespace configtags
} // namespace appsupport
} // namespace next

#endif // NEXT_CONFIG_TAGS_H_
