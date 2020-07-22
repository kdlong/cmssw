#include "GeneratorInterface/Core/interface/GenWeightHelper.h"
#include <iostream>

using namespace tinyxml2;

namespace gen {
  GenWeightHelper::GenWeightHelper() {}

  void GenWeightHelper::parseWeightGroupsFromNames(std::vector<std::string> weightNames) {
    parsedWeights_.clear();
    size_t index = 0;
    size_t groupIndex = 0;
    std::string curGroup = "";
    // If size is 1, it's just the central weight
    if (weightNames.size() <= 1)
      return;

    for (std::string weightName : weightNames) {
      std::cout << weightName << std::endl;
    }
    for (std::string weightName : weightNames) {
      if (weightName.find("LHE") != std::string::npos) {
        // Parse as usual, this is the SUSY workflow
        std::vector<std::string> info;
        boost::split(info, weightName, boost::is_any_of(","));
        std::unordered_map<std::string, std::string> attributes;
        std::string text = info.back();
        info.pop_back();
        for (auto i : info) {
          std::vector<std::string> subInfo;
          boost::split(subInfo, i, boost::is_any_of("="));
          if (subInfo.size() == 2) {
            attributes[trim(subInfo[0])] = trim(subInfo[1]);
          }
        }
        if (attributes["group"] != curGroup) {
          curGroup = attributes["group"];
          groupIndex++;
        }
        parsedWeights_.push_back({attributes["id"], index++, curGroup, text, attributes, groupIndex});
      } else {
        parsedWeights_.push_back(
            {weightName, index++, weightName, weightName, std::unordered_map<std::string, std::string>(), groupIndex++});
        if (isPartonShowerWeightGroup(parsedWeights_.back()))
          parsedWeights_.back().wgtGroup_idx = -1;  // all parton showers are grouped together
      }
      // Working on the not-so-nice assumption that all non-LHE gen weights are PS weights
      // else if (weightGroups_.size() == 0) {
      //   weightGroups_.push_back(new gen::PartonShowerWeightGroupInfo("shower"));
      // }
    }
    buildGroups();
    printWeights();
  }

  inline std::string GenWeightHelper::trim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
  }
}  // namespace gen
