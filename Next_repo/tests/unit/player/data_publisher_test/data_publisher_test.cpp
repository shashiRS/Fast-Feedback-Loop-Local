
#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>
#include <future>

#include "../../common/resource_manager_moc.hpp"
#include "resource_manager/buffer_queue.hpp"
#include "state_machine/player_state_machine.hpp"

#include <next/appsupport/config/config_client.hpp>
#include "plugin/core_lib_reader/core_lib_package.h"

constexpr int kSecondsToMicroSeconds = 1000000;
next::player::BufferQueue test_queue;

void PushInBuffer(std::shared_ptr<ResourceManagerMoc> resource_manager, std::size_t size, uint64_t reference_timestamp,
                  uint16_t cycleCounter, uint8_t cycleState = mts_package_no_cycle) {

  mts_memory_allocator_t *allocator = mts_get_default_allocator();
  mts::extensibility::package test_package(allocator, size);
  mts_package_t *mts_pkt = test_package.get();
  mts_pkt->cycle_counter = cycleCounter;
  mts_pkt->cycle_state = cycleState;
  mts_pkt->reference_timestamp = reference_timestamp;
  std::string testFormat = "test";
  std::copy(testFormat.begin(), testFormat.end(), mts_pkt->format_type);
  std::unique_ptr<next::sdk::types::IPackage> package = std::make_unique<CoreLibPackage>(std::move(test_package));

  resource_manager->GetDataBuffer()->pushBuffer(std::move(package));
}
void Initialize(boost::msm::back::state_machine<PlayerStateMachine> *sm,
                std::shared_ptr<PublisherManager> publisher_manager, std::shared_ptr<FileReader> file_reader,
                std::shared_ptr<ResourceManagerMoc> resource_manager) {
  resource_manager->Initialize();
  publisher_manager->Initialize(resource_manager);
  file_reader->SetResourceManager(resource_manager);

  // init states
  StateOpening &stateOpening = sm->get_state<StateOpening &>();
  stateOpening.Initialize(resource_manager, publisher_manager, file_reader);

  StatePlay &statePLAY = sm->get_state<StatePlay &>();
  statePLAY.Initialize(publisher_manager, resource_manager);

  StateUnloadRecording &stateUnloadRecording = sm->get_state<StateUnloadRecording &>();
  stateUnloadRecording.Initialize(resource_manager, file_reader);

  StateSkipTo &stateSkipTo = sm->get_state<StateSkipTo &>();
  stateSkipTo.Initialize(file_reader);
}

ESTATES WaitForReady(boost::msm::back::state_machine<PlayerStateMachine> *sm, uint32_t numSec) {
  ESTATES currentState = (ESTATES)sm->current_state_;
  while (currentState != ESTATES::E_READY && numSec) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    currentState = (ESTATES)sm->current_state_;
    numSec--;
  }
  return currentState;
}
class Player_DataPublisher_Fixture : public next::sdk::testing::TestUsingEcal {
public:
  Player_DataPublisher_Fixture() {

    next::appsupport::ConfigClient::do_init("next_player");
    next::appsupport::ConfigClient::finish_init();
    initmap_[next::sdk::logger::getConsoleLoglevelCfgKey()] = next::sdk::logger::LOGLEVEL::DEBUG;
    this->instance_name_ = "PlayerTestFixture";
  }
};
TEST_F(Player_DataPublisher_Fixture, step) {
  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_(new PublisherManager());
  std::shared_ptr<FileReader> file_reader_ptr_(new FileReader());
  std::shared_ptr<ResourceManagerMoc> resource_manager_ptr_(new ResourceManagerMoc());

  player_state_machine.start();
  Initialize(&player_state_machine, publisher_manager_ptr_, file_reader_ptr_, resource_manager_ptr_);
  resource_manager_ptr_->GetDataBuffer()->clearBuffer();
  resource_manager_ptr_->GetDataBuffer()->startPush();

  // add something to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 1, 1);
  PushInBuffer(resource_manager_ptr_, 100, 10, 2);
  PushInBuffer(resource_manager_ptr_, 100, 20, 3);
  PushInBuffer(resource_manager_ptr_, 100, 30, 4);

  player_state_machine.process_event(test_command_READY());
  player_state_machine.process_event(command_STEP_FORWARD(3));

  EXPECT_TRUE(WaitForReady(&player_state_machine, 10) == ESTATES::E_READY);
  EXPECT_TRUE(resource_manager_ptr_->GetDataBuffer()->size() == 1);

  resource_manager_ptr_->Shutdown();
  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
}
TEST_F(Player_DataPublisher_Fixture, step_by_timestamp) {

  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_(new PublisherManager());
  std::shared_ptr<FileReader> file_reader_ptr_(new FileReader());
  std::shared_ptr<ResourceManagerMoc> resource_manager_ptr_(new ResourceManagerMoc());

  player_state_machine.start();
  Initialize(&player_state_machine, publisher_manager_ptr_, file_reader_ptr_, resource_manager_ptr_);
  resource_manager_ptr_->GetDataBuffer()->clearBuffer();
  resource_manager_ptr_->GetDataBuffer()->startPush();

  // add something to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 1, 1);
  PushInBuffer(resource_manager_ptr_, 100, 102, 2);
  PushInBuffer(resource_manager_ptr_, 100, 120, 3);
  PushInBuffer(resource_manager_ptr_, 100, 203, 4);
  PushInBuffer(resource_manager_ptr_, 100, 304, 5);
  PushInBuffer(resource_manager_ptr_, 100, 405, 6);
  publisher_manager_ptr_->SetSteppingMode(STEPPING_BY_TIMESTAMP);

  player_state_machine.process_event(test_command_READY());
  player_state_machine.process_event(command_STEP_FORWARD(3));

  EXPECT_TRUE(WaitForReady(&player_state_machine, 10) == ESTATES::E_READY);
  EXPECT_TRUE(resource_manager_ptr_->GetDataBuffer()->size() == 1);
  EXPECT_TRUE(resource_manager_ptr_->GetReplayStatistics()->published_package_count == 5);

  // add some more data to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 506, 6);
  PushInBuffer(resource_manager_ptr_, 100, 607, 7);
  PushInBuffer(resource_manager_ptr_, 100, 708, 8);
  PushInBuffer(resource_manager_ptr_, 100, 809, 9);
  PushInBuffer(resource_manager_ptr_, 100, 910, 10);
  player_state_machine.process_event(command_STEP_FORWARD(3));

  EXPECT_TRUE(WaitForReady(&player_state_machine, 10) == ESTATES::E_READY);
  EXPECT_TRUE(resource_manager_ptr_->GetDataBuffer()->size() == 2);
  EXPECT_TRUE(resource_manager_ptr_->GetReplayStatistics()->published_package_count == 9);
  resource_manager_ptr_->Shutdown();
  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
}
TEST_F(Player_DataPublisher_Fixture, step_by_cycle_ID) {

  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_(new PublisherManager());
  std::shared_ptr<FileReader> file_reader_ptr_(new FileReader());
  std::shared_ptr<ResourceManagerMoc> resource_manager_ptr_(new ResourceManagerMoc());

  player_state_machine.start();
  Initialize(&player_state_machine, publisher_manager_ptr_, file_reader_ptr_, resource_manager_ptr_);
  resource_manager_ptr_->GetDataBuffer()->clearBuffer();
  resource_manager_ptr_->GetDataBuffer()->startPush();

  // add something to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 1, 1);
  PushInBuffer(resource_manager_ptr_, 100, 102, 2, mts_package_cycle_end);
  PushInBuffer(resource_manager_ptr_, 100, 120, 3);
  PushInBuffer(resource_manager_ptr_, 100, 203, 4, mts_package_cycle_end);
  PushInBuffer(resource_manager_ptr_, 100, 304, 5, mts_package_cycle_end);
  PushInBuffer(resource_manager_ptr_, 100, 405, 6);
  publisher_manager_ptr_->SetSteppingMode(STEPPING_BY_CYCLE_ID);

  player_state_machine.process_event(test_command_READY());
  player_state_machine.process_event(command_STEP_FORWARD(3));

  EXPECT_TRUE(WaitForReady(&player_state_machine, 10) == ESTATES::E_READY);
  EXPECT_TRUE(resource_manager_ptr_->GetDataBuffer()->size() == 1);
  // mts_package_cycle_end packages are not published
  EXPECT_TRUE(resource_manager_ptr_->GetReplayStatistics()->published_package_count == 2);

  // add some more data to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 506, 6, mts_package_cycle_end);
  PushInBuffer(resource_manager_ptr_, 100, 607, 7);
  PushInBuffer(resource_manager_ptr_, 100, 708, 8, mts_package_cycle_end);
  PushInBuffer(resource_manager_ptr_, 100, 809, 9);
  PushInBuffer(resource_manager_ptr_, 100, 910, 10, mts_package_cycle_end);
  player_state_machine.process_event(command_STEP_FORWARD(3));

  EXPECT_TRUE(WaitForReady(&player_state_machine, 10) == ESTATES::E_READY);
  EXPECT_TRUE(resource_manager_ptr_->GetDataBuffer()->size() == 0);
  EXPECT_TRUE(resource_manager_ptr_->GetReplayStatistics()->published_package_count == 5);
  resource_manager_ptr_->Shutdown();
  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
}
TEST_F(Player_DataPublisher_Fixture, step_no_publish) {

  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_(new PublisherManager());
  std::shared_ptr<FileReader> file_reader_ptr_(new FileReader());
  std::shared_ptr<ResourceManagerMoc> resource_manager_ptr_(new ResourceManagerMoc());

  player_state_machine.start();
  Initialize(&player_state_machine, publisher_manager_ptr_, file_reader_ptr_, resource_manager_ptr_);
  resource_manager_ptr_->GetDataBuffer()->clearBuffer();
  resource_manager_ptr_->GetDataBuffer()->startPush();

  // add something to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 1, 1);
  PushInBuffer(resource_manager_ptr_, 100, 10, 2);
  PushInBuffer(resource_manager_ptr_, 100, 20, 3);
  PushInBuffer(resource_manager_ptr_, 100, 30, 4);
  // signal end of file so play will stop when finishing the data
  resource_manager_ptr_->GetDataBuffer()->setEndOfFileFlag();
  resource_manager_ptr_->GetUdexService()->disable_publish();

  player_state_machine.process_event(test_command_READY());
  player_state_machine.process_event(command_STEP_FORWARD(3));

  EXPECT_TRUE(WaitForReady(&player_state_machine, 10) == ESTATES::E_READY);
  EXPECT_TRUE(resource_manager_ptr_->GetDataBuffer()->size() == 1);
  EXPECT_TRUE(resource_manager_ptr_->GetReplayStatistics()->published_package_count == 0);

  resource_manager_ptr_->Shutdown();
  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
}
TEST_F(Player_DataPublisher_Fixture, step_partial_publish) {

  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_(new PublisherManager());
  std::shared_ptr<FileReader> file_reader_ptr_(new FileReader());
  std::shared_ptr<ResourceManagerMoc> resource_manager_ptr_(new ResourceManagerMoc());

  player_state_machine.start();
  Initialize(&player_state_machine, publisher_manager_ptr_, file_reader_ptr_, resource_manager_ptr_);
  resource_manager_ptr_->GetDataBuffer()->clearBuffer();
  resource_manager_ptr_->GetDataBuffer()->startPush();

  // add something to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 1, 1);
  PushInBuffer(resource_manager_ptr_, 100, 10, 2);
  PushInBuffer(resource_manager_ptr_, 100, 20, 3);
  PushInBuffer(resource_manager_ptr_, 100, 30, 4);
  // signal end of file so play will stop when finishing the data
  resource_manager_ptr_->GetDataBuffer()->setEndOfFileFlag();
  resource_manager_ptr_->GetUdexService()->set_publish_count(1);

  player_state_machine.process_event(test_command_READY());
  player_state_machine.process_event(command_STEP_FORWARD(3));

  EXPECT_TRUE(WaitForReady(&player_state_machine, 10) == ESTATES::E_READY);
  EXPECT_TRUE(resource_manager_ptr_->GetDataBuffer()->size() == 1);
  EXPECT_TRUE(resource_manager_ptr_->GetReplayStatistics()->published_package_count == 1u);
  EXPECT_TRUE(resource_manager_ptr_->GetUdexService()->GetPublishedPackageCount() == 1u);
  EXPECT_TRUE(resource_manager_ptr_->GetUdexService()->GetPublishedPackageTimeByIndex(0u) == 1);
  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
}

TEST_F(Player_DataPublisher_Fixture, step_no_trigger) {

  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_(new PublisherManager());
  std::shared_ptr<FileReader> file_reader_ptr_(new FileReader());
  std::shared_ptr<ResourceManagerMoc> resource_manager_ptr_(new ResourceManagerMoc());

  player_state_machine.start();
  Initialize(&player_state_machine, publisher_manager_ptr_, file_reader_ptr_, resource_manager_ptr_);
  resource_manager_ptr_->GetDataBuffer()->clearBuffer();
  resource_manager_ptr_->GetDataBuffer()->startPush();

  // add something to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 1, 1);
  PushInBuffer(resource_manager_ptr_, 100, 10, 2);
  PushInBuffer(resource_manager_ptr_, 100, 20, 3);
  PushInBuffer(resource_manager_ptr_, 100, 30, 4);
  // signal end of file so play will stop when finishing the data
  resource_manager_ptr_->GetDataBuffer()->setEndOfFileFlag();
  resource_manager_ptr_->GetUdexService()->disable_trigger();

  player_state_machine.process_event(test_command_READY());
  player_state_machine.process_event(command_STEP_FORWARD(3));

  EXPECT_TRUE(WaitForReady(&player_state_machine, 10) == ESTATES::E_READY);
  EXPECT_TRUE(resource_manager_ptr_->GetDataBuffer()->size() == 1);
  EXPECT_TRUE(resource_manager_ptr_->GetReplayStatistics()->published_package_count == 3);

  resource_manager_ptr_->Shutdown();
  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
}
TEST_F(Player_DataPublisher_Fixture, step_partial_trigger) {

  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_(new PublisherManager());
  std::shared_ptr<FileReader> file_reader_ptr_(new FileReader());
  std::shared_ptr<ResourceManagerMoc> resource_manager_ptr_(new ResourceManagerMoc());

  player_state_machine.start();
  Initialize(&player_state_machine, publisher_manager_ptr_, file_reader_ptr_, resource_manager_ptr_);
  resource_manager_ptr_->GetDataBuffer()->clearBuffer();
  resource_manager_ptr_->GetDataBuffer()->startPush();

  // add something to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 1, 1);
  PushInBuffer(resource_manager_ptr_, 100, 10, 2);
  PushInBuffer(resource_manager_ptr_, 100, 20, 3);
  PushInBuffer(resource_manager_ptr_, 100, 30, 4);
  // signal end of file so play will stop when finishing the data
  resource_manager_ptr_->GetDataBuffer()->setEndOfFileFlag();
  resource_manager_ptr_->GetUdexService()->set_trigger_count(1);

  player_state_machine.process_event(test_command_READY());
  player_state_machine.process_event(command_STEP_FORWARD(3));

  EXPECT_TRUE(WaitForReady(&player_state_machine, 10) == ESTATES::E_READY);
  EXPECT_TRUE(resource_manager_ptr_->GetDataBuffer()->size() == 1);
  EXPECT_TRUE(resource_manager_ptr_->GetReplayStatistics()->published_package_count == 3);

  resource_manager_ptr_->Shutdown();
  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
}
TEST_F(Player_DataPublisher_Fixture, step_insufficient_data_and_block) {

  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_(new PublisherManager());
  std::shared_ptr<FileReader> file_reader_ptr_(new FileReader());
  std::shared_ptr<ResourceManagerMoc> resource_manager_ptr_(new ResourceManagerMoc());

  player_state_machine.start();

  Initialize(&player_state_machine, publisher_manager_ptr_, file_reader_ptr_, resource_manager_ptr_);
  // add something to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 1, 1);
  PushInBuffer(resource_manager_ptr_, 100, 10, 2);

  player_state_machine.process_event(test_command_READY());
  player_state_machine.process_event(command_STEP_FORWARD(3));
  // this should block as it will wait in the data buffer pop

  EXPECT_TRUE(WaitForReady(&player_state_machine, 3) == ESTATES::E_PLAY);

  // add one more element to finish steps
  PushInBuffer(resource_manager_ptr_, 100, 20, 3);

  // last step should be possible and player -> ready
  EXPECT_TRUE(WaitForReady(&player_state_machine, 3) == ESTATES::E_READY);

  resource_manager_ptr_->Shutdown();
  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
}

TEST_F(Player_DataPublisher_Fixture, step_and_pause) {

  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_(new PublisherManager());
  std::shared_ptr<FileReader> file_reader_ptr_(new FileReader());
  std::shared_ptr<ResourceManagerMoc> resource_manager_ptr_(new ResourceManagerMoc());

  player_state_machine.start();

  Initialize(&player_state_machine, publisher_manager_ptr_, file_reader_ptr_, resource_manager_ptr_);
  // add something to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 1, 1);
  PushInBuffer(resource_manager_ptr_, 100, 10, 2);

  player_state_machine.process_event(test_command_READY());
  player_state_machine.process_event(command_STEP_FORWARD(3));
  // this should block as it will wait in the data buffer pop

  EXPECT_TRUE(WaitForReady(&player_state_machine, 3) == ESTATES::E_PLAY);

  // process pase to exit the play state
  player_state_machine.process_event(command_PAUSE());

  EXPECT_TRUE(WaitForReady(&player_state_machine, 3) == ESTATES::E_READY);

  resource_manager_ptr_->Shutdown();
  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
}

TEST_F(Player_DataPublisher_Fixture, step_insufficient_data_eof) {

  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_(new PublisherManager());
  std::shared_ptr<FileReader> file_reader_ptr_(new FileReader());
  std::shared_ptr<ResourceManagerMoc> resource_manager_ptr_(new ResourceManagerMoc());

  player_state_machine.start();

  Initialize(&player_state_machine, publisher_manager_ptr_, file_reader_ptr_, resource_manager_ptr_);
  // add something to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 1, 1);
  PushInBuffer(resource_manager_ptr_, 100, 10, 2);
  // signal end of file reached
  resource_manager_ptr_->GetDataBuffer()->setEndOfFileFlag();

  player_state_machine.process_event(test_command_READY());
  player_state_machine.process_event(command_STEP_FORWARD(3));
  // this should not block as it will reach EoF

  EXPECT_EQ(WaitForReady(&player_state_machine, 1), ESTATES::E_READY);

  resource_manager_ptr_->Shutdown();
  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
}
TEST_F(Player_DataPublisher_Fixture, play) {

  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_(new PublisherManager());
  std::shared_ptr<FileReader> file_reader_ptr_(new FileReader());
  std::shared_ptr<ResourceManagerMoc> resource_manager_ptr_(new ResourceManagerMoc());

  player_state_machine.start();
  Initialize(&player_state_machine, publisher_manager_ptr_, file_reader_ptr_, resource_manager_ptr_);
  resource_manager_ptr_->GetDataBuffer()->clearBuffer();
  resource_manager_ptr_->GetDataBuffer()->startPush();

  // add something to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 1, 1);
  PushInBuffer(resource_manager_ptr_, 100, 10, 2);
  PushInBuffer(resource_manager_ptr_, 100, 20, 3);
  PushInBuffer(resource_manager_ptr_, 100, 30, 4);
  // signal end of file so play will stop when finishing the data
  resource_manager_ptr_->GetDataBuffer()->setEndOfFileFlag();

  player_state_machine.process_event(test_command_READY());
  player_state_machine.process_event(command_STEP_FORWARD(INT32_MAX));

  // should finish play
  EXPECT_TRUE(WaitForReady(&player_state_machine, 3) == ESTATES::E_READY);

  // and all data should be consumed
  EXPECT_TRUE(resource_manager_ptr_->GetDataBuffer()->size() == 0);

  resource_manager_ptr_->Shutdown();
  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
}
TEST_F(Player_DataPublisher_Fixture, play_speed_factor) {

  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_(new PublisherManager());
  std::shared_ptr<FileReader> file_reader_ptr_(new FileReader());
  std::shared_ptr<ResourceManagerMoc> resource_manager_ptr_(new ResourceManagerMoc());

  player_state_machine.start();
  Initialize(&player_state_machine, publisher_manager_ptr_, file_reader_ptr_, resource_manager_ptr_);
  resource_manager_ptr_->GetDataBuffer()->clearBuffer();
  resource_manager_ptr_->GetDataBuffer()->startPush();

  // add something to the buffer
  PushInBuffer(resource_manager_ptr_, 100, 1 * kSecondsToMicroSeconds, 1);
  PushInBuffer(resource_manager_ptr_, 100, 2 * kSecondsToMicroSeconds, 2);
  PushInBuffer(resource_manager_ptr_, 100, 3 * kSecondsToMicroSeconds, 3);
  PushInBuffer(resource_manager_ptr_, 100, 4 * kSecondsToMicroSeconds, 4);
  // signal end of file so play will stop when finishing the data
  resource_manager_ptr_->GetDataBuffer()->setEndOfFileFlag();
  resource_manager_ptr_->GetUdexService()->setPublishDuration(1 * kSecondsToMicroSeconds);

  player_state_machine.process_event(test_command_READY());
  player_state_machine.process_event(command_STEP_FORWARD(INT32_MAX));

  // should finish play
  EXPECT_TRUE(WaitForReady(&player_state_machine, 10) == ESTATES::E_READY);

  // and all data should be consumed
  EXPECT_TRUE(resource_manager_ptr_->GetDataBuffer()->size() == 0);

  float speed_factor = publisher_manager_ptr_->GetLastSpeedFactor();
  EXPECT_NEAR(speed_factor, 1, 0.003);

  resource_manager_ptr_->Shutdown();
  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
}

TEST_F(Player_DataPublisher_Fixture, play_speed_factor_slow_sim_time) {

  boost::msm::back::state_machine<PlayerStateMachine> player_state_machine;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_(new PublisherManager());
  std::shared_ptr<FileReader> file_reader_ptr_(new FileReader());
  std::shared_ptr<ResourceManagerMoc> resource_manager_ptr_(new ResourceManagerMoc());

  player_state_machine.start();
  Initialize(&player_state_machine, publisher_manager_ptr_, file_reader_ptr_, resource_manager_ptr_);
  resource_manager_ptr_->GetDataBuffer()->clearBuffer();
  resource_manager_ptr_->GetDataBuffer()->startPush();

  // add something to the buffer
  uint64_t ts = 0;
  for (uint16_t i = 0; i < 39; i++) {
    ts += (500u) * 1000u;
    PushInBuffer(resource_manager_ptr_, 100u, ts, i);
  }
  // signal end of file so play will stop when finishing the data
  resource_manager_ptr_->GetDataBuffer()->setEndOfFileFlag();
  resource_manager_ptr_->GetUdexService()->setPublishDuration(10000);
  publisher_manager_ptr_->UpdateSpeedFactor(1);

  player_state_machine.process_event(test_command_READY());
  player_state_machine.process_event(command_STEP_FORWARD(INT32_MAX));

  // should not finish play as ew decreased the play speed
  EXPECT_FALSE(WaitForReady(&player_state_machine, 10) == ESTATES::E_READY);

  float speed_factor = publisher_manager_ptr_->GetLastSpeedFactor();
  EXPECT_NEAR(speed_factor, 1, 0.15);
  resource_manager_ptr_->Shutdown();
  player_state_machine.process_event(event_shutdown());
  player_state_machine.Shutdown();
}
