#ifndef NEXT_CONFIG_TYPES_H_
#define NEXT_CONFIG_TYPES_H_

#include <functional>

namespace next {
namespace appsupport {
namespace configtypes {

using change_hook = std::function<bool(std::string)>;

} // namespace configtypes
} // namespace appsupport
} // namespace next

#endif // NEXT_CONFIG_TYPES_H_
