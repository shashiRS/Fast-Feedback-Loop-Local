/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin.cpp
 * @brief    interface for bridge plugins
 *
 * See plugin.h.
 *
 **/

#include <next/bridgesdk/plugin.h>

#include <cassert>

#include <next/plugins_utils/plugins_types.h>
#include "plugin_private.h"

namespace next {
namespace bridgesdk {
namespace plugin {

Plugin::Plugin(const char *path) : impl_(std::make_unique<PluginPrivate>(path)) { assert(impl_); }

Plugin::~Plugin() { impl_.reset(); }

std::string Plugin::getName() const { return impl_->name_; }
void Plugin::setName(const std::string &name) { impl_->name_ = name; }

std::string Plugin::getVersion() const { return impl_->version_; }
void Plugin::setVersion(const std::string &version) { impl_->version_ = version; }

bool Plugin::setSynchronizationMode(bool enabled) { return impl_->setSynchronizationMode(enabled); }

bool Plugin::setPackageDropMode(bool enabled) { return impl_->setPackageDropMode(enabled); }

/*!
 * Empty hull to be implemented by user plugin
 */
bool Plugin::init() { return true; }

/*!
 * Empty hull to be implemented by user plugin
 */
bool Plugin::update([[maybe_unused]] std::vector<std::string> user_input) { return true; }

/*!
 * Empty hull to be implemented by user plugin
 */
bool Plugin::enterReset() { return true; }

/*!
 * Empty hull to be implemented by user plugin
 */
bool Plugin::exitReset() { return true; }

void Plugin::connectPlugin(std::shared_ptr<next::udex::subscriber::DataSubscriber> &data_subscriber,
                           std::shared_ptr<next::databridge::data_manager::IDataManager> &data_manager,
                           std::shared_ptr<next::udex::explorer::SignalExplorer> &signal_explorer) {

  this->impl_->setDataSubscriber(data_subscriber);
  this->impl_->setDataReceiver(data_manager);
  this->impl_->setSignalExpolrer(signal_explorer);
}

bool Plugin::getDescription([[maybe_unused]] next::bridgesdk::plugin_config_t &config_fields) { return false; }

bool Plugin::addConfig([[maybe_unused]] const next::bridgesdk::plugin_config_t &config) { return false; }

bool Plugin::removeConfig([[maybe_unused]] const next::bridgesdk::plugin_config_t &config) { return false; }

} // namespace plugin
} // namespace bridgesdk
} // namespace next
