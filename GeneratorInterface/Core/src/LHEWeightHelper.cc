#include "GeneratorInterface/Core/interface/LHEWeightHelper.h"
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <iostream>

using namespace tinyxml2;

namespace gen {
  void LHEWeightHelper::setHeaderLines(std::vector<std::string> headerLines) { headerLines_ = headerLines; }

  void LHEWeightHelper::parseWeights() {
    parsedWeights_.clear();

    if (!isConsistent()) {
      swapHeaders();
    }

    tinyxml2::XMLDocument xmlDoc;
    std::string fullHeader = boost::algorithm::join(headerLines_, "");

    int xmlError = xmlDoc.Parse(fullHeader.c_str());
    // in case of &gt; instead of <
    if (xmlError != 0) {
      boost::replace_all(fullHeader, "&lt;", "<");
      boost::replace_all(fullHeader, "&gt;", ">");
      xmlError = xmlDoc.Parse(fullHeader.c_str());
    }
    // error persists (how to handle error?)
    if (xmlError != 0) {
      std::cerr << "Error in lhe xml file" << std::endl;
      xmlDoc.PrintError();
      return;
    }

    std::vector<std::string> nameAlts_ = {"name", "type"};

    size_t weightIndex = 0;
    size_t groupIndex = 0;
    //for (auto* e = root->FirstChildElement(); e != nullptr; e = e->NextSiblingElement()) {
    for (auto* e = xmlDoc.RootElement(); e != nullptr; e = e->NextSiblingElement()) {
      std::string groupName = "";
      if (strcmp(e->Name(), "weight") == 0) {
        // we are here if there is a weight that does not belong to any group
        std::string text = "";
        if (e->GetText()) {
          text = e->GetText();
        }
        std::unordered_map<std::string, std::string> attributes;
        for (auto* att = e->FirstAttribute(); att != nullptr; att = att->Next())
          attributes[att->Name()] = att->Value();
        parsedWeights_.push_back({e->Attribute("id"), weightIndex++, groupName, text, attributes, groupIndex});
      } else if (strcmp(e->Name(), "weightgroup") == 0) {
        // to deal wiht files with "id" instead of "name"
        for (auto nameAtt : nameAlts_) {
          if (e->Attribute(nameAtt.c_str())) {
            groupName = e->Attribute(nameAtt.c_str());
            break;
          }
        }
        if (groupName.empty()) {
          throw std::runtime_error("couldn't find groupname");
        }
        // May remove this, very specific error
        if (groupName.find(".") != std::string::npos)
          groupName.erase(groupName.find("."), groupName.size());

        for (auto* inner = e->FirstChildElement("weight"); inner != nullptr;
             inner = inner->NextSiblingElement("weight")) {
          // we are here if there is a weight in a weightgroup
          std::string text = "";
          if (inner->GetText())
            text = inner->GetText();
          std::unordered_map<std::string, std::string> attributes;
          for (auto* att = inner->FirstAttribute(); att != nullptr; att = att->Next())
            attributes[att->Name()] = att->Value();
          parsedWeights_.push_back({inner->Attribute("id"), weightIndex++, groupName, text, attributes, groupIndex});
        }
      }
      groupIndex++;
    }
    buildGroups();
  }

  bool LHEWeightHelper::isConsistent() {
    int curLevel = 0;

    for (auto line : headerLines_) {
      if (line.find("<weightgroup") != std::string::npos) {
        curLevel++;
        if (curLevel != 1) {
          return false;
        }
      } else if (line.find("</weightgroup>") != std::string::npos) {
        curLevel--;
        if (curLevel != 0) {
          return false;
        }
      }
    }
    return curLevel == 0;
  }

  void LHEWeightHelper::swapHeaders() {
    int curLevel = 0;
    int open = -1;
    int close = -1;
    for (size_t idx = 0; idx < headerLines_.size(); idx++) {
      std::string line = headerLines_[idx];
      if (line.find("<weightgroup") != std::string::npos) {
        curLevel++;
        if (curLevel != 1) {
          open = idx;
        }
      } else if (line.find("</weightgroup>") != std::string::npos) {
        curLevel--;
        if (curLevel != 0) {
          close = idx;
        }
      }
      if (open > -1 && close > -1) {
        std::swap(headerLines_[open], headerLines_[close]);
        open = -1;
        close = -1;
      }
    }
  }

  void LHEWeightHelper::buildGroups() {
    weightGroups_.clear();
    size_t currentGroupIdx = -1;
    for (auto& weight : parsedWeights_) {
      if (currentGroupIdx != weight.wgtGroup_idx) {
        weightGroups_.push_back(*buildGroup(weight));
        currentGroupIdx = weight.wgtGroup_idx;
      }

      // split PDF groups
      if (weightGroups_.back().weightType() == gen::WeightType::kPdfWeights) {
        auto& pdfGroup = dynamic_cast<gen::PdfWeightGroupInfo&>(weightGroups_.back());
        int lhaid = getLhapdfId(weight);
        if (lhaid > 0 && !pdfGroup.isIdInParentSet(lhaid) && pdfGroup.getParentLhapdfId() > 0) {
          weightGroups_.push_back(*buildGroup(weight));
        }
      }
      WeightGroupInfo& group = weightGroups_.back();
      group.addContainedId(weight.index, weight.id, weight.content);
      if (group.weightType() == gen::WeightType::kScaleWeights)
        updateScaleInfo(weight);
      else if (group.weightType() == gen::WeightType::kPdfWeights)
        updatePdfInfo(weight);
    }
    cleanupOrphanCentralWeight();
    // checks
    for (auto& wgt : weightGroups_) {
      if (!wgt.isWellFormed())
        std::cout << "\033[1;31m";
      std::cout << std::boolalpha << wgt.name() << " (" << wgt.firstId() << "-" << wgt.lastId()
                << "): " << wgt.isWellFormed() << std::endl;
      if (wgt.weightType() == gen::WeightType::kScaleWeights) {
        auto& wgtScale = dynamic_cast<gen::ScaleWeightGroupInfo&>(wgt);
        std::cout << wgtScale.centralIndex() << " ";
        std::cout << wgtScale.muR1muF2Index() << " ";
        std::cout << wgtScale.muR1muF05Index() << " ";
        std::cout << wgtScale.muR2muF1Index() << " ";
        std::cout << wgtScale.muR2muF2Index() << " ";
        std::cout << wgtScale.muR2muF05Index() << " ";
        std::cout << wgtScale.muR05muF1Index() << " ";
        std::cout << wgtScale.muR05muF2Index() << " ";
        std::cout << wgtScale.muR05muF05Index() << " \n";
        for (auto name : wgtScale.getDynNames()) {
          std::cout << name << ": ";
          std::cout << wgtScale.getScaleIndex(1.0, 1.0, name) << " ";
          std::cout << wgtScale.getScaleIndex(1.0, 2.0, name) << " ";
          std::cout << wgtScale.getScaleIndex(1.0, 0.5, name) << " ";
          std::cout << wgtScale.getScaleIndex(2.0, 1.0, name) << " ";
          std::cout << wgtScale.getScaleIndex(2.0, 2.0, name) << " ";
          std::cout << wgtScale.getScaleIndex(2.0, 0.5, name) << " ";
          std::cout << wgtScale.getScaleIndex(0.5, 1.0, name) << " ";
          std::cout << wgtScale.getScaleIndex(0.5, 2.0, name) << " ";
          std::cout << wgtScale.getScaleIndex(0.5, 0.5, name) << "\n";
        }

      } else if (wgt.weightType() == gen::WeightType::kPdfWeights) {
        std::cout << wgt.description() << "\n";
      }
      if (!wgt.isWellFormed())
        std::cout << "\033[0m";
    }
  }

  std::unique_ptr<WeightGroupInfo> LHEWeightHelper::buildGroup(ParsedWeight& weight) {
    if (isScaleWeightGroup(weight))
      return std::make_unique<ScaleWeightGroupInfo>(weight.groupname);
    else if (isPdfWeightGroup(weight))
      return std::make_unique<PdfWeightGroupInfo>(weight.groupname);
    else if (isMEParamWeightGroup(weight))
      return std::make_unique<MEParamWeightGroupInfo>(weight.groupname);
    else if (isOrphanPdfWeightGroup(weight))
      return std::make_unique<PdfWeightGroupInfo>(weight.groupname);

    return std::make_unique<UnknownWeightGroupInfo>(weight.groupname);
  }
}  // namespace gen
