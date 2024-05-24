#ifndef NEXT_UDEX_STRUCT_INFO_PROVIDER_TEST_H
#define NEXT_UDEX_STRUCT_INFO_PROVIDER_TEST_H

#include <next/udex/struct_extractor/struct_info_provider.hpp>

constexpr size_t size_float = sizeof(float);

class SignalInfoProviderTest : public ISignalInfoProvider {

public:
  SignalInfoProviderTest(){};

  next::udex::SignalInfo getSignalInfo(const std::string &url) override {
    next::udex::SignalInfo signal_info;
    signal_info.offset = offsetdummy[url];
    signal_info.signal_type = typedummy[url];
    return signal_info;
  }

  size_t getOffsetByUrl(const std::string &url) override { return offsetdummy[url]; }

  bool deserializationRequired() override { return false; }

  bool getDeserializedMemory(memoryBlock &) override { return true; }

  std::unordered_map<std::string, next::udex::SignalType> typedummy{
      {"sur.numobj", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"tp1", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"tp1.pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"tp1.vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"tp2", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"tp2.pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"tp2.vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"sur.tp1", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"sur.tp1.pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"sur.tp1.vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"sur.tp2", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"sur.tp2.pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"sur.tp2.vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"sur.tp1", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"sur.tp[0]", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"sur.tp[0].pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"sur.tp[0].vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"sur.tp[1]", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"sur.tp[1].pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"sur.tp[1].vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"sur.tp[2]", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"sur.tp[2].pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"sur.tp[2].vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"sur.tp[3]", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"sur.tp[3].pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"sur.tp[3].vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},

      {"device.sur[0].tp[0]", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"device.sur[0].tp[0].pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[0].tp[0].vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[0].tp[1]", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"device.sur[0].tp[1].pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[0].tp[1].vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[0].tp[2]", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"device.sur[0].tp[2].pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[0].tp[2].vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[0].tp[3]", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"device.sur[0].tp[3].pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[0].tp[3].vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[1].tp[0]", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"device.sur[1].tp[0].pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[1].tp[0].vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[1].tp[1]", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"device.sur[1].tp[1].pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[1].tp[1].vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[1].tp[2]", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"device.sur[1].tp[2].pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[1].tp[2].vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[1].tp[3]", next::udex::SignalType::SIGNAL_TYPE_STRUCT},
      {"device.sur[1].tp[3].pos", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
      {"device.sur[1].tp[3].vel", next::udex::SignalType::SIGNAL_TYPE_FLOAT},
  };

  std::unordered_map<std::string, size_t> offsetdummy{
      {"sur.numobj", 11 * size_float},
      {"tp1", 0 * size_float},
      {"tp1.pos", 0 * size_float},
      {"tp1.vel", 1 * size_float},
      {"tp2", 3 * size_float},
      {"tp2.pos", 3 * size_float},
      {"tp2.vel", 4 * size_float},
      {"sur.tp1", 6 * size_float},
      {"sur.tp1.pos", 6 * size_float},
      {"sur.tp1.vel", 7 * size_float},
      {"sur.tp2", 8 * size_float},
      {"sur.tp2.pos", 8 * size_float},
      {"sur.tp2.vel", 9 * size_float},
      {"sur.tp1", 6 * size_float},
      {"sur.tp[0]", 10 * size_float},
      {"sur.tp[0].pos", 11 * size_float},
      {"sur.tp[0].vel", 10 * size_float},
      {"sur.tp[1]", 12 * size_float},
      {"sur.tp[1].pos", 13 * size_float},
      {"sur.tp[1].vel", 12 * size_float},
      {"sur.tp[2]", 14 * size_float},
      {"sur.tp[2].pos", 15 * size_float},
      {"sur.tp[2].vel", 14 * size_float},
      {"sur.tp[3]", 16 * size_float},
      {"sur.tp[3].pos", 17 * size_float},
      {"sur.tp[3].vel", 16 * size_float},

      {"device.sur[0].tp[0]", 0 * size_float},
      {"device.sur[0].tp[0].pos", 1 * size_float},
      {"device.sur[0].tp[0].vel", 0 * size_float},
      {"device.sur[0].tp[1]", 2 * size_float},
      {"device.sur[0].tp[1].pos", 3 * size_float},
      {"device.sur[0].tp[1].vel", 2 * size_float},
      {"device.sur[0].tp[2]", 4 * size_float},
      {"device.sur[0].tp[2].pos", 5 * size_float},
      {"device.sur[0].tp[2].vel", 4 * size_float},
      {"device.sur[0].tp[3]", 6 * size_float},
      {"device.sur[0].tp[3].pos", 7 * size_float},
      {"device.sur[0].tp[3].vel", 6 * size_float},

      {"device.sur[1].tp[0]", 10 * size_float},
      {"device.sur[1].tp[0].pos", 11 * size_float},
      {"device.sur[1].tp[0].vel", 10 * size_float},
      {"device.sur[1].tp[1]", 12 * size_float},
      {"device.sur[1].tp[1].pos", 13 * size_float},
      {"device.sur[1].tp[1].vel", 12 * size_float},
      {"device.sur[1].tp[2]", 14 * size_float},
      {"device.sur[1].tp[2].pos", 15 * size_float},
      {"device.sur[1].tp[2].vel", 14 * size_float},
      {"device.sur[1].tp[3]", 16 * size_float},
      {"device.sur[1].tp[3].pos", 17 * size_float},
      {"device.sur[1].tp[3].vel", 16 * size_float},
  };
};

#endif // NEXT_UDEX_STRUCT_INFO_PROVIDER_TEST_H
