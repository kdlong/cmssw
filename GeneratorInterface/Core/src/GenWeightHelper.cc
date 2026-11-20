#include "GeneratorInterface/Core/interface/GenWeightHelper.h"

#include <iostream>

using namespace tinyxml2;

namespace gen {
  namespace {
    const std::regex sherpaEWVariation{
        R"(MUR([0-9.]+)_MUF([0-9.]+)_PDF([0-9]+)_((?:MULTI|EXP)?ASSEW(?:LO1(?:LO2(?:LO3)?)?)?))"};
    const std::string sherpaEWGroupName = "Sherpa EW variations";
  }  // namespace

  GenWeightHelper::GenWeightHelper() {}

  int GenWeightHelper::sherpaCentralPdf(const std::vector<std::string>& weightNames) const {
    // The nominal PDF is the one used for the scale variations, falling back to the first PDF found
    int firstPdf = -1;
    std::smatch match;
    for (const auto& name : weightNames) {
      if (!std::regex_match(name, match, sherpaVariation_))
        continue;
      int pdf = std::stoi(match.str(3));
      if (firstPdf < 0)
        firstPdf = pdf;
      if (std::stof(match.str(1)) != 1.f || std::stof(match.str(2)) != 1.f)
        return pdf;
    }
    return firstPdf;
  }

  std::vector<std::unique_ptr<gen::WeightGroupInfo>> GenWeightHelper::parseWeightGroupsFromNames(
      std::vector<std::string> weightNames, bool addUnassociated) const {
    std::vector<ParsedWeight> parsedWeights;
    int index = 0;
    int groupIndex = -1;
    int showerGroupIndex = -1;
    std::unordered_map<std::string, size_t> dynamicalScaleIndices;
    std::string curGroup = "";
    // If size is 1, it's just the central weight
    // Still give the opportunity to add an unassociated group
    if (weightNames.size() <= 1)
      return buildGroups(parsedWeights, addUnassociated);

    // Sherpa variations are named MUR<x>_MUF<y>_PDF<lhaid>
    const int centralPdf = sherpaCentralPdf(weightNames);
    std::smatch match;

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
            attributes[boost::algorithm::trim_copy(subInfo[0])] = boost::algorithm::trim_copy(subInfo[1]);
          }
        }
        if (attributes["group"] != curGroup) {
          curGroup = attributes["group"];
          groupIndex++;
        }
        // Gen Weights can't have an ID, because they are just a
        // std::vector<float> in the event
        attributes["id"] = "";
        parsedWeights.push_back({attributes["id"], index, curGroup, text, attributes, groupIndex});
        auto& parsed = parsedWeights.back();
        if (isScaleWeightGroup(parsed)) {
          // MadGraph omits factors equal to one from its text labels. Pythia
          // also drops the XML scale attributes, so reconstruct them here.
          for (const auto& factor : {"mur", "muf"}) {
            const auto value = searchAttributes(factor, parsed);
            parsed.attributes[factor] = value.empty() ? "1" : value;
          }
          const auto dynName = searchAttributes("dyn_name", parsed);
          if (!dynName.empty()) {
            const auto entry = dynamicalScaleIndices.emplace(dynName, dynamicalScaleIndices.size()).first;
            parsed.attributes["DYN_SCALE"] = std::to_string(entry->second);
            parsed.attributes["dyn_scale_choice"] = dynName;
          }
        }
      } else if (std::regex_match(weightName, match, sherpaEWVariation)) {
        // These vary associated EW contributions and their combination with
        // the QCD matrix element, even though their labels contain scales/PDFs.
        std::unordered_map<std::string, std::string> attributes = {
            {"mur", match.str(1)}, {"muf", match.str(2)}, {"pdf", match.str(3)}};
        if (curGroup != sherpaEWGroupName) {
          curGroup = sherpaEWGroupName;
          groupIndex++;
        }
        parsedWeights.push_back({"", index, curGroup, weightName, attributes, groupIndex});
      } else if (std::regex_match(weightName, match, sherpaVariation_)) {
        std::unordered_map<std::string, std::string> attributes = {
            {"mur", match.str(1)}, {"muf", match.str(2)}, {"pdf", match.str(3)}};
        std::string group;
        if (std::stoi(match.str(3)) == centralPdf)
          group = "Central scale variation";
        else if (std::stof(match.str(1)) == 1.f && std::stof(match.str(2)) == 1.f)
          group = "PDF_variation";
        else
          group = "scale_variation_PDF" + match.str(3);
        if (group != curGroup) {
          curGroup = group;
          groupIndex++;
        }
        parsedWeights.push_back({"", index, curGroup, weightName, attributes, groupIndex});
      } else {
        parsedWeights.push_back(
            {"", index, weightName, weightName, std::unordered_map<std::string, std::string>(), groupIndex});
        if (isPartonShowerWeightGroup(parsedWeights.back())) {
          if (showerGroupIndex < 0) {
            showerGroupIndex = ++groupIndex;
          }
          parsedWeights.back().wgtGroup_idx = showerGroupIndex;  // all parton showers are grouped together
          curGroup = "shower";
        } else {
          // Weights with no recognized structure (e.g., Sherpa's MEWeight, NTrials)
          // are collected in a single group, consecutive ones sharing it
          if (curGroup != genericGroupName_) {
            curGroup = genericGroupName_;
            groupIndex++;
          }
          parsedWeights.back().groupname = genericGroupName_;
          parsedWeights.back().wgtGroup_idx = groupIndex;
        }
      }
      index++;
    }
    auto groups = buildGroups(parsedWeights, addUnassociated);
    for (auto& group : groups) {
      if (group->weightType() != gen::WeightType::kMEParamWeights || group->name() != sherpaEWGroupName)
        continue;
      std::string description =
          "Sherpa associated EW matrix-element variations; ASS: additive, MULTIASS: multiplicative, "
          "EXPASS: exponentiated; ";
      for (const auto& weight : group->idsContained())
        description += "[" + std::to_string(weight.localIndex) + "] " + weight.label + "; ";
      group->setDescription(description);
      group->setIsWellFormed(true);
    }
    return groups;
  }
}  // namespace gen
