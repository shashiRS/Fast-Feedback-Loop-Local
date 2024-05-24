
#include <gtest/gtest.h>
#include <thread>

#include <next/sdk/sdk.hpp>

#include "config_change_hook_handler.hpp"

namespace next {
namespace appsupport {

TEST(ConfigChangeHookHandlertest, addCallbackAndCallCallback_CallbacksShallBeCalled) {

  ConfigChangeHookHandler config_change_hook_handler;

  std::string input_function_call = "ip";

  bool is_first_call_back_parameter_handed_over = false;
  config_change_hook_handler.addChangeHook([&]([[maybe_unused]] std::string input) {
    if (input.compare(input_function_call) == 0)
      is_first_call_back_parameter_handed_over = true;
    return true;
  });

  bool is_second_call_back_parameter_handed_over = false;
  config_change_hook_handler.addChangeHook([&]([[maybe_unused]] std::string input) {
    if (input.compare(input_function_call) == 0)
      is_second_call_back_parameter_handed_over = true;
    return true;
  });

  config_change_hook_handler.callChangeHooks(input_function_call);

  EXPECT_TRUE(is_first_call_back_parameter_handed_over);
  EXPECT_TRUE(is_second_call_back_parameter_handed_over);
}

TEST(ConfigChangeHookHandlertest, callCallbacksTwice_CallbacksShallBeCalledTwice) {

  ConfigChangeHookHandler config_change_hook_handler;

  std::string input_function_call = "ip";
  bool is_first_callback_parameter_handed_over = false;

  config_change_hook_handler.addChangeHook([&]([[maybe_unused]] std::string input) {
    if (input.compare(input_function_call) == 0)
      is_first_callback_parameter_handed_over = true;
    return true;
  });

  bool is_second_callback_parameter_handed_over = false;

  config_change_hook_handler.addChangeHook([&]([[maybe_unused]] std::string input) {
    if (input.compare(input_function_call) == 0)
      is_second_callback_parameter_handed_over = true;
    return true;
  });

  config_change_hook_handler.callChangeHooks(input_function_call);

  EXPECT_TRUE(is_first_callback_parameter_handed_over);
  EXPECT_TRUE(is_second_callback_parameter_handed_over);

  // reset callback status
  is_first_callback_parameter_handed_over = false;
  is_second_callback_parameter_handed_over = false;

  // call again
  input_function_call = "ip2";
  config_change_hook_handler.callChangeHooks(input_function_call);

  EXPECT_TRUE(is_first_callback_parameter_handed_over);
  EXPECT_TRUE(is_second_callback_parameter_handed_over);
}

TEST(ConfigChangeHookHandlertest, removeAddedChangeHookAndCallCallbacks_noCallbackShouldBeCalled) {

  ConfigChangeHookHandler config_change_hook_handler;

  std::string input_function_call = "ip";
  bool is_first_callback_called = false;

  auto id = config_change_hook_handler.addChangeHook([&]([[maybe_unused]] std::string input) {
    is_first_callback_called = true;
    return true;
  });

  config_change_hook_handler.removeChangeHook(id);

  config_change_hook_handler.callChangeHooks(input_function_call);

  EXPECT_FALSE(is_first_callback_called);
}

TEST(ConfigChangeHookHandlertest, CallAddCallbackAfterRemoval) {

  ConfigChangeHookHandler config_change_hook_handler;

  std::string input_function_call = "ip";

  bool is_first_callback_called = false;
  auto id = config_change_hook_handler.addChangeHook([&]([[maybe_unused]] std::string input) {
    is_first_callback_called = true;
    return true;
  });

  bool is_second_callback_called = false;
  config_change_hook_handler.addChangeHook([&]([[maybe_unused]] std::string input) {
    is_second_callback_called = true;
    return true;
  });

  config_change_hook_handler.removeChangeHook(id);

  config_change_hook_handler.callChangeHooks(input_function_call);

  EXPECT_FALSE(is_first_callback_called);
  EXPECT_TRUE(is_second_callback_called);

  is_first_callback_called = false;
  is_second_callback_called = false;
  bool is_third_callback_called = false;
  config_change_hook_handler.addChangeHook([&]([[maybe_unused]] std::string input) {
    is_third_callback_called = true;
    return true;
  });

  config_change_hook_handler.callChangeHooks(input_function_call);

  EXPECT_FALSE(is_first_callback_called);
  EXPECT_TRUE(is_second_callback_called);
  EXPECT_TRUE(is_third_callback_called);
}

TEST(ConfigChangeHookHandlertest, removeFromEmptyHookList_noCrash) {

  ConfigChangeHookHandler config_change_hook_handler;

  config_change_hook_handler.removeChangeHook(0);
  config_change_hook_handler.callChangeHooks("test");
}

TEST(ConfigChangeHookHandlertest, getDefaultHookActivationStatus) {

  ConfigChangeHookHandler config_change_hook_handler;

  EXPECT_TRUE(config_change_hook_handler.getChangeHooksActivationStatus());
}

TEST(ConfigChangeHookHandlertest, deactivateHooks_noHooksAreCalled) {

  ConfigChangeHookHandler config_change_hook_handler;

  config_change_hook_handler.setChangeHooksActivationStatus(false);
  EXPECT_FALSE(config_change_hook_handler.getChangeHooksActivationStatus());

  bool callback_called = false;
  config_change_hook_handler.addChangeHook([&]([[maybe_unused]] std::string input) {
    callback_called = true;
    return true;
  });

  config_change_hook_handler.callChangeHooks("test");

  EXPECT_FALSE(callback_called);
}

TEST(ConfigChangeHookHandlertest, reactivateHooks_HooksAreCalled) {

  ConfigChangeHookHandler config_change_hook_handler;

  config_change_hook_handler.setChangeHooksActivationStatus(false);
  EXPECT_FALSE(config_change_hook_handler.getChangeHooksActivationStatus());

  bool callback_called = false;
  config_change_hook_handler.addChangeHook([&]([[maybe_unused]] std::string input) {
    callback_called = true;
    return true;
  });

  config_change_hook_handler.callChangeHooks("test");

  EXPECT_FALSE(callback_called);

  config_change_hook_handler.setChangeHooksActivationStatus(true);
  EXPECT_TRUE(config_change_hook_handler.getChangeHooksActivationStatus());

  config_change_hook_handler.callChangeHooks("test2");

  EXPECT_TRUE(callback_called);
}

} // namespace appsupport
} // namespace next
