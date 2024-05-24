#include "version.h"

namespace next {
namespace databridge {
namespace plugin_manager {

bool Version::operator<(Version v) { return version_compare(version_string, v.version_string) == -1; }
bool Version::operator>(Version v) { return version_compare(version_string, v.version_string) == +1; }
bool Version::operator<=(Version v) { return version_compare(version_string, v.version_string) != +1; }
bool Version::operator>=(Version v) { return version_compare(version_string, v.version_string) != -1; }
bool Version::operator==(Version v) { return version_compare(version_string, v.version_string) == 0; }

Version::Version(std::string v) : version_string(v) {}

int Version::version_compare(std::string v1, std::string v2) {
  size_t i = 0, j = 0;
  while (i < v1.length() || j < v2.length()) {
    int acc1 = 0, acc2 = 0;

    while (i < v1.length() && v1[i] != '.') {
      acc1 = acc1 * 10 + (v1[i] - '0');
      i++;
    }
    while (j < v2.length() && v2[j] != '.') {
      acc2 = acc2 * 10 + (v2[j] - '0');
      j++;
    }

    if (acc1 < acc2)
      return -1;
    if (acc1 > acc2)
      return +1;

    ++i;
    ++j;
  }
  return 0;
}
} // namespace plugin_manager
} // namespace databridge
} // namespace next
