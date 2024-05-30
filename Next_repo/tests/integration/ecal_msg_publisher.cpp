#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include <ecal/ecal.h>
#include <ecal/msg/string/publisher.h>
#include <json/json.h>

Json::Value MakeObjectList(const Json::Value value) {
  Json::Value value_to_send;
  value_to_send["dataStream"]["objectData"]["trafficParticipants"]["CemFdp21P"] = Json::arrayValue;
  value_to_send["dataStream"]["objectData"]["trafficParticipants"]["CemFdp21P"][0] = value;
  value_to_send["dataStream"]["objectData"]["trafficParticipants"]["CemFdp21P"][0]["sensorUid"] = 1;
  value_to_send["dataStream"]["objectData"]["trafficParticipants"]["CemFdp21P"][0]["sensor"] = "CemFdp21P";
  value_to_send["dataStream"]["objectData"]["trafficParticipants"]["CemFdp21P"][0]["name"] = "FDP21p";
  value_to_send["dataStream"]["objectData"]["trafficParticipants"]["CemFdp21P"][0]["uid"] = 1;
  value_to_send["dataStream"]["objectData"]["trafficParticipants"]["CemFdp21P"][0]["type"] = "TP";
  value_to_send["dataStream"]["objectData"]["trafficParticipants"]["CemFdp21P"][0]["enabled"] = true;
  return value_to_send;
}

bool ReadJsonFromFile(std::string file, Json::Value &root) {
  Json::CharReaderBuilder builder;
  std::ifstream test(file, std::ifstream::binary);
  std::string errs;
  bool ok = Json::parseFromStream(builder, test, &root, &errs);
  if (!ok) {
    // report to the user the failure and their locations in the document.
    std::cout << errs << "\n";
  }
  return ok;
}

std::vector<Json::Value> RetrieveSignalFromJson(const std::string &sensor_name_value,
                                                const std::string &sensor_name_key, const Json::Value &root) {

  std::vector<Json::Value> values{};
  for (Json::Value::ArrayIndex i = 0; i != root.size(); i++) {
    if ((root[i].isMember(sensor_name_key)) && (root[i][sensor_name_key] == sensor_name_value)) {
      values.push_back(root[i]);
    }
  }

  return values;
}

int main(int argc, char **argv) {
  std::cout << "-------------------------------" << std::endl;
  std::cout << " HELLO WORLD SENDER" << std::endl;
  std::cout << "-------------------------------" << std::endl;

  std::string json_file_path{argv[1]};
  Json::Value root{};

  // initialize eCAL API
  eCAL::Initialize(argc, argv, "minimal_json_send");

  // publisher for topic "Hello"
  eCAL::string::CPublisher<std::string> pub("fdp21p_objects");

  auto res = ReadJsonFromFile(json_file_path, root);

  // send updates
  int cnt = 0;
  while (eCAL::Ok()) {

    if (res) {
      for (Json::Value::ArrayIndex i = 0; i != root.size(); i++) {
        auto values_fdp21 = RetrieveSignalFromJson("CemFdp21P Vehicle", "sensor", root[i]["structures"]);
        for (const auto &elm : values_fdp21) {

          Json::Value value_to_send{MakeObjectList(elm)};
          Json::FastWriter fastWriter;
          std::string output = fastWriter.write(value_to_send);

          // send content
          pub.Send(output, cnt++);

          // sleep 10 ms
          eCAL::Process::SleepMS(200);
        }
      }
    }
  }

  // finalize eCAL API
  eCAL::Finalize();

  return (0);
}
