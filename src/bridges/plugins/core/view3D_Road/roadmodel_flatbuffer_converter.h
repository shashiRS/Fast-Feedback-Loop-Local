#ifndef NEXT_PLUGINS_ROADMODEL_FLATBUFFER_CONVERTER_H_
#define NEXT_PLUGINS_ROADMODEL_FLATBUFFER_CONVERTER_H_

#include <next/bridgesdk/plugin.h>
#include <next/plugins_utils/plugins_types.h>

#include "reduced_road_model.h"
#include "roadmodel.h"

namespace next {
namespace plugins {

class RoadModelFlatBufferConverter {

  RoadModel road_model_;
  VisuRoadModel &plugin_;

public:
  RoadModelFlatBufferConverter(VisuRoadModel &visu_roadmodel_plugin) : plugin_(visu_roadmodel_plugin) {}
  virtual ~RoadModelFlatBufferConverter() {}
  RoadModelFlatBufferConverter(const RoadModelFlatBufferConverter &) = delete;
  RoadModelFlatBufferConverter(RoadModelFlatBufferConverter &&) = delete;
  RoadModelFlatBufferConverter &operator=(const RoadModelFlatBufferConverter &) = delete;
  RoadModelFlatBufferConverter &operator=(const RoadModelFlatBufferConverter &&) = delete;

  bool convertRawDataToRoadModelAndSend(next::plugins_utils::SensorConfig sensor_config,
                                        std::shared_ptr<BaseRoadModelList> base_roadmodel,
                                        RoadModelMaxSize roadModelMaxSize, uint64_t timestamp);

protected:
  // flatbuffer related functions

  void sendJSDataFromFlatBuffer(uint8_t *flatbuf, const next::plugins_utils::SensorConfig &sensor_config,
                                size_t buff_size, uint64_t timestamp);

  // functions interacting with udex
  bool extractLaneTopologyFromRawData(std::shared_ptr<BaseRoadModelList> base_roadmodel,
                                      RoadModelMaxSize roadModelMaxSize);
  bool extractRoadPointsFromRawData(std::shared_ptr<BaseRoadModelList> base_roadmodel,
                                    RoadModelMaxSize roadModelMaxSize);
  bool extractLinearObjectFromRawData(std::shared_ptr<BaseRoadModelList> base_roadmodel,
                                      RoadModelMaxSize roadModelMaxSize);

  void calculate(const next::plugins_utils::SensorConfig &sensor_config, RoadModelMaxSize roadModelMaxSize,
                 uint64_t timestamp);
};
} // namespace plugins
} // namespace next
#endif // NEXT_PLUGINS_ROADMODEL_FLATBUFFER_CONVERTER_H_
