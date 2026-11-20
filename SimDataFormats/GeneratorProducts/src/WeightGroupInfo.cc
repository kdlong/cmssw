#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"

#include <algorithm>
#include <iostream>
#include <string>

#include "FWCore/Utilities/interface/Exception.h"

namespace gen {
  WeightGroupInfo* WeightGroupInfo::clone() const {
    throw cms::Exception("WeightGroupInfo") << "In group " << name_
                                            << ": WeightGroupInfo is abstract, so it's clone() method can't be "
                                               "implemented.";
  }

  const WeightMetaInfo& WeightGroupInfo::weightMetaInfo(int weightEntry) const {
    if (weightEntry < 0 || weightEntry >= static_cast<int>(idsContained_.size()))
      throw cms::Exception("WeightGroupInfo")
          << "Local index " << std::to_string(weightEntry) << " is not within the range of group " << name_
          << " which has " << idsContained_.size() << " entries\n";
    return idsContained_.at(weightEntry);
  }

  const WeightMetaInfo& WeightGroupInfo::weightMetaInfoByGlobalIndex(int weightEntry) const {
    std::string emptyLabel = "";
    return weightMetaInfoByGlobalIndex(emptyLabel, weightEntry);
  }

  const WeightMetaInfo& WeightGroupInfo::weightMetaInfoByGlobalIndex(std::string& wgtId, int weightEntry) const {
    int entry = weightVectorEntry(wgtId, weightEntry);
    if (entry < 0 || entry >= static_cast<int>(idsContained_.size()))
      throw cms::Exception("WeightGroupInfo")
          << "Weight entry " << std::to_string(weightEntry) << " is not a member of group " << name_
          << ". \n    firstID = " << std::to_string(firstId_) << " lastId = " << std::to_string(lastId_);
    return idsContained_.at(entry);
  }

  int WeightGroupInfo::weightVectorEntry(std::string& wgtId) const { return weightVectorEntry(wgtId, 0); }

  bool WeightGroupInfo::containsWeight(std::string& wgtId, int weightEntry) const {
    return weightVectorEntry(wgtId, weightEntry) != -1;
  }

  bool WeightGroupInfo::containsWeight(int weightEntry) const {
    // Match on the global index only. The weight IDs are arbitrary strings and can collide
    // with the index of a different weight (e.g. IDs "1", "2", ... for indices 0, 1, ...)
    std::string id;
    return weightVectorEntry(id, weightEntry) != -1;
  }

  int WeightGroupInfo::weightVectorEntry(std::string& wgtId, int weightEntry) const {
    // First try direct comparison assuming ordered indices
    int orderedEntry = weightEntry - firstId_;
    if (indexInRange(weightEntry) && orderedEntry < static_cast<int>(idsContained_.size())) {
      if (!wgtId.empty() && idsContained_.at(orderedEntry).id == wgtId) {
        return orderedEntry;
      } else if (static_cast<int>(idsContained_.at(orderedEntry).globalIndex) == weightEntry) {
        return orderedEntry;
      }
    }
    // Fall back to global search on ID or global index
    auto it = std::find_if(idsContained_.begin(), idsContained_.end(), [wgtId, weightEntry](const WeightMetaInfo& w) {
      return wgtId.empty() ? static_cast<int>(w.globalIndex) == weightEntry : w.id == wgtId;
    });
    if (it != idsContained_.end())
      return std::distance(idsContained_.begin(), it);
    return -1;
  }

  void WeightGroupInfo::addContainedId(int weightEntry, std::string id, std::string label = "") {
    if (weightEntry < 0)
      throw cms::Exception("WeightGroupInfo")
          << "Negative global weight index " << weightEntry << " in group " << name_;
    auto position = std::lower_bound(
        idsContained_.begin(), idsContained_.end(), weightEntry, [](const WeightMetaInfo& entry, int index) {
          return entry.globalIndex < static_cast<size_t>(index);
        });
    if (position != idsContained_.end() && position->globalIndex == static_cast<size_t>(weightEntry))
      throw cms::Exception("WeightGroupInfo")
          << "Duplicate global weight index " << weightEntry << " in group " << name_;
    const size_t localIndex = std::distance(idsContained_.begin(), position);
    if (id.empty())
      id = std::to_string(weightEntry);
    if (firstId_ < 0 || weightEntry < firstId_)
      firstId_ = weightEntry;
    if (weightEntry > lastId_)
      lastId_ = weightEntry;
    WeightMetaInfo info = {static_cast<size_t>(weightEntry), localIndex, id, label};
    idsContained_.insert(position, std::move(info));
    for (size_t i = localIndex + 1; i < idsContained_.size(); ++i)
      idsContained_[i].localIndex = i;
    if (!weightLabelsToIndices_.empty())
      cacheWeightIndicesByLabel();
  }

  bool WeightGroupInfo::indexInRange(int index) const { return (index <= lastId_ && index >= firstId_); }

  void WeightGroupInfo::cacheWeightIndicesByLabel() {
    for (const auto& weight : idsContained_)
      weightLabelsToIndices_[weight.label] = weight.localIndex;
  }

  std::vector<std::string> WeightGroupInfo::weightLabels() const {
    std::vector<std::string> labels;
    labels.reserve(idsContained_.size());
    for (const auto& weight : idsContained_)
      labels.push_back(weight.label);
    return labels;
  }

  int WeightGroupInfo::weightIndexFromLabel(std::string weightLabel) const {
    if (!weightLabelsToIndices_.empty()) {
      if (weightLabelsToIndices_.find(weightLabel) != weightLabelsToIndices_.end())
        return static_cast<int>(weightLabelsToIndices_.at(weightLabel));
      return -1;
    }

    auto it = std::find_if(
        idsContained_.begin(), idsContained_.end(), [weightLabel](const auto& w) { return weightLabel == w.label; });
    if (it == idsContained_.end())
      return -1;
    return std::distance(idsContained_.begin(), it);
  }

}  // namespace gen
