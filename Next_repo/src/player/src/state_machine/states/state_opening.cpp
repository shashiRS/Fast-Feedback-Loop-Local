/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */

#include "state_opening.hpp"

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>

StateOpening::StateOpening() : status_message_(new next::control::events::PlayerMetaInformationMessage()) {}

void StateOpening::SafeErrorToStatusEvent(const std::vector<std::string> &file_path) {
  status_message_->status = next::control::events::PlayerState::ON_ERROR;
  for (const auto &path : file_path) {
    if (!status_message_->recording_name.empty()) {
      status_message_->recording_name += ", "; // Delimiter
    }
    status_message_->recording_name += path;
  }
}

void StateOpening::Initialize(std::shared_ptr<IResourceManager> resourceManager,
                              std::shared_ptr<PublisherManager> publisherManager,
                              std::shared_ptr<FileReader> fileReader) {
  resource_manager_ptr_ = resourceManager;
  file_reader_ptr_ = fileReader;
  publisher_manager_ptr_ = publisherManager;
}

bool StateOpening::OpenFile(const std::vector<std::string> &file_path) {
  if (file_reader_ptr_->OpenFile(file_path)) {

    // this is not entirely correct - file_path being a vector
    auto static_ptr = next::appsupport::ConfigClient::getConfig();
    static_ptr->put(next::appsupport::configkey::player::getRecordingPathKey(), file_path[0]);

    publisher_manager_ptr_->Setup(); // triggers from config // remove argument here

    file_reader_ptr_->GetFileStats(status_message_->min_timestamp, status_message_->max_timestamp,
                                   status_message_->timestamp);

    publisher_manager_ptr_->UpdateFilterExpression();
    if (file_reader_ptr_->SupportsFilter()) {
      file_reader_ptr_->SetFilterExpression(publisher_manager_ptr_->GetFilterExpression());
    }

    file_reader_ptr_->StartReadAheadThread();

    status_message_->status = next::control::events::PlayerState::ON_READY;
    status_message_->speed_factor = publisher_manager_ptr_->GetRealTimeFactorFromConfigFile();
    status_message_->recording_name = "";
    for (const auto &path : file_path) {
      if (!status_message_->recording_name.empty()) {
        status_message_->recording_name += ", "; // Delimiter
      }
      status_message_->recording_name += path;
    }
    status_message_->stepping_type = "";
    status_message_->stepping_value = 0;
    return true;
  } else {
    SafeErrorToStatusEvent(file_path);
    return false;
  }
}
