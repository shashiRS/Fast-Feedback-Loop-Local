#ifndef NEXT_BRIDGES_CANTPOBJECTSTEST_HPP
#define NEXT_BRIDGES_CANTPOBJECTSTEST_HPP

#include <next/sdk/sdk.hpp>

namespace next {

class CanTpObjectsPluginTest : public sdk::testing::TestUsingEcal {
public:
  using SearchRequest = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchRequest;
  using SearchResult = next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchResult;

  CanTpObjectsPluginTest() { this->instance_name_ = "CanTpObjectsPluginTest"; }
};

} // namespace next

#endif // NEXT_BRIDGES_CANTPOBJECTSTEST_HPP
