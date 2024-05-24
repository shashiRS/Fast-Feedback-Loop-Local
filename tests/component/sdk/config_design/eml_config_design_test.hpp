/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     EML_config.hpp
 *  @brief    Testing the config interfaces
 */

#ifndef EML_CONFIG_HPP
#define EML_CONFIG_HPP

#include <exception>
#include <iostream>
#include <string>

#include <next/appsupport/config/config_base.hpp>

namespace next {
namespace appsupport {

template <typename T>
class MClient : public ConfigBase {
protected:
  MClient(std::string name) : ConfigBase(name, false){};
  ~MClient(){};

protected:
  bool doInit_() {
    this->setDefaultValues();
    return true;
  }

  bool doReset_(void) {
    std::cout << "doReset" << '\n';
    return true;
  }

  virtual void setDefaultValues() = 0;
  virtual void writeConfig(bool config_only) = 0;

public:
  static T &GetInstance() {
    if (ComponentName_ == "") {
      // throw new std::exception("not initialized");
    }
    static T theInstance(ComponentName_); // Better way than using a static member variable
    return theInstance;
  }

  static bool doInit(std::string name) {
    ComponentName_ = name;
    GetInstance().doInit_();
    return true;
  }

  static void doReset() { ComponentName_ = ""; }

private:
  static inline std::string ComponentName_ = "";
};

class EmlConfig : public MClient<EmlConfig> {

protected:
  EmlConfig(std::string name) : MClient(name){};
  friend class MClient<EmlConfig>;

public:
  void setDefaultValues() override {
    this->put(this->getRootName() + ".version", "0.0.0");
    this->put(this->getRootName() + ".start_time", 0);
    return;
  };

  void writeConfig(bool config_only) override {
    // TODO retreive all valid changable values
    std::cout << "writeConfig " << config_only << '\n';
    if (!config_only) {
      // return this->toJson();
    } else {
      // get _config subtree
      // ...put("_config." + this->componentName(), this->get_subTree(this->componentName() + ".version"));

      // get all values that can be changed but do not overwrite
      // ...put_notOverwrite(this->componentName() + ".version",  this->get_string(this->componentName() + ".version"));
      // ...put_notOverwrite(this->componentName() + ".version", this->get_string(this->componentName() + ".version"));

      // return ptree().toJson();
    }
    return;
  };

  void addPort(std::string name, std::string vaddr, std::string version, int dataSize, std::string dataName,
               std::string giaDll) {
    this->put(this->getRootName() + ".in_port." + name + ".vaddr", vaddr);
    this->put(this->getRootName() + ".in_port." + name + ".version", version);
    this->put(this->getRootName() + ".in_port." + name + ".data_type_size", dataSize);
    this->put(this->getRootName() + ".in_port." + name + ".data_type_name", dataName);
    this->put(this->getRootName() + ".in_port." + name + ".gia_dll", giaDll);
  }

  void addSyncToPort(std::string name, int timestamp, int id, std::string type) {
    this->put(this->getRootName() + ".in_port." + name + ".sync.sync_timestamp", timestamp);
    this->put(this->getRootName() + ".in_port." + name + ".sync.sync_id", id);
    this->put(this->getRootName() + ".in_port." + name + ".sync.sync_type", type);
  }
};

} // namespace appsupport
} // namespace next

#endif // EML_CONFIG_HPP
