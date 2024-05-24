
#include <fstream>
#include <memory>

#include <ecal/ecal.h>

class trigger_mapper {
private:
  std::vector<std::tuple<std::string, std::string, std::string>> trigger_urls_;
  std::unordered_map<size_t, std::pair<std::string, std::unique_ptr<eCAL::CServiceClient>>> trigger_map_;

  bool create_trigger_connections();

public:
  bool initialize();
  std::vector<std::tuple<std::string, std::string, std::string>> *get_trigger_vector();
  std::unordered_map<size_t, std::pair<std::string, std::unique_ptr<eCAL::CServiceClient>>> *get_trigger_map();

  bool Reset();
};
