/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     argument_parser.hpp
 * @brief    parses the arguments for the container
 */

#ifndef NEXT_CONTAINER_ARGUMENT_PARSER_HPP_
#define NEXT_CONTAINER_ARGUMENT_PARSER_HPP_
namespace next {
namespace container {

class ArgumentParserNextContainer {

public:
  ArgumentParserNextContainer() = default;
  virtual ~ArgumentParserNextContainer() = default;

  std::string parseArguments(int argc, char *argv[], std::vector<std::string> &args_extended,
                             std::vector<std::string> &node_names) {
    std::vector<std::string> args;
    for (int i_args = 0; i_args < argc; i_args++) {
      args.push_back(argv[i_args]);
    }
    return parseArguments(args, args_extended, node_names);
  }

  std::vector<char *> convertArguments(const std::vector<std::string> &args_extended) {
    std::vector<char *> args_char_array;
    for (const auto &arg : args_extended) {
      args_char_array.push_back((char *)arg.data());
    }
    args_char_array.push_back(nullptr);
    return args_char_array;
  }

  std::string parseArguments(const std::vector<std::string> &args, std::vector<std::string> &args_extended,
                             std::vector<std::string> &node_names) {

    const std::string k_name_parameter = "-name";
    std::string instance_name = "next_node_container";

    bool node_arg_key_found = false;

    size_t counter = 0;
    for (auto arg_it = args.begin(); arg_it < args.end(); arg_it++) {

      // check for container name
      auto checkContainerName = [&] { return k_name_parameter.compare(*arg_it) == 0 && (counter + 1 < args.size()); };
      if (checkContainerName()) {
        instance_name = args[counter + 1];
        arg_it++;
        continue;
      }

      // add node names to list and remove from arguments
      extractNodeNames(*arg_it, args_extended, node_arg_key_found, node_names);
      counter++;
    }
    addNextArgumentIfMissing(args_extended);

    return instance_name;
  }

private:
  void addNextArgumentIfMissing(std::vector<std::string> &args) {
    const std::string k_next_arg_dummy = "-n";
    for (const auto &arg : args) {
      if (arg.compare(k_next_arg_dummy) == 0) {
        return;
      }
    }
    args.push_back("-n");
  }

  void extractNodeNames(const std::string &argv_temp, std::vector<std::string> &args_new, bool &node_arg_key_found,
                        std::vector<std::string> &node_names) {
    std::string k_cut_argument_reference = "-cut";

    bool found_this_time = false;
    if (k_cut_argument_reference.compare(argv_temp) == 0) {
      node_arg_key_found = true;
      found_this_time = true;
    }

    // we are in the node name list
    if (node_arg_key_found && !found_this_time) {
      // list entry
      if (argv_temp.find("-") == std::string::npos) {
        node_names.push_back(std::string(argv_temp));
      } else { // list end
        node_arg_key_found = false;
      }
    }

    // not an argument for us -> push it forward to container
    if (!node_arg_key_found) {
      args_new.push_back(argv_temp);
    }
  }
};
} // namespace container
} // namespace next
#endif // NEXT_CONTAINER_ARGUMENT_PARSER_HPP_
