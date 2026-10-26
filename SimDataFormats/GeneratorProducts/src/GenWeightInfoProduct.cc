#include "SimDataFormats/GeneratorProducts/interface/GenWeightInfoProduct.h"

#include <algorithm>
#include <string>
#include <vector>

GenWeightInfoProduct::GenWeightInfoProduct(const gen::WeightGroupInfoContainer& weightGroups) {
  for (const auto& group : weightGroups)
    addWeightGroupInfo(group);
}

const gen::WeightGroupInfo& GenWeightInfoProduct::group(size_t index) const {
  const auto& location = order_.at(index);
  switch (location.type) {
    case gen::WeightType::kScaleWeights:
      return scaleGroups_.at(location.index);
    case gen::WeightType::kPdfWeights:
      return pdfGroups_.at(location.index);
    case gen::WeightType::kMEParamWeights:
      return meParamGroups_.at(location.index);
    case gen::WeightType::kPartonShowerWeights:
      return partonShowerGroups_.at(location.index);
    case gen::WeightType::kUnknownWeights:
      return unknownGroups_.at(location.index);
  }
  throw cms::Exception("GenWeightInfoProduct") << "Weight group " << index << " has an invalid type";
}

const int GenWeightInfoProduct::numberOfFilledGroups() const {
  int filled = 0;
  for (size_t i = 0; i < order_.size(); i++)
    filled += group(i).nIdsContained() > 0 ? 1 : 0;
  return filled;
}

const int GenWeightInfoProduct::numberOfWeights() const {
  int nweights = 0;
  for (size_t i = 0; i < order_.size(); i++)
    nweights += group(i).nIdsContained();
  return nweights;
}

const std::vector<gen::WeightGroupData> GenWeightInfoProduct::allWeightGroupsInfoWithIndices() const {
  std::vector<gen::WeightGroupData> groupInfo;
  groupInfo.reserve(order_.size());
  for (size_t i = 0; i < order_.size(); i++)
    groupInfo.push_back({i, &group(i)});
  return groupInfo;
}

gen::WeightGroupData GenWeightInfoProduct::containingWeightGroupInfo(int index, size_t startSearch) const {
  // When filling the weights, most likely to find the weight matches the
  // previous group or the one after
  if (startSearch < order_.size() && group(startSearch).containsWeight(index))
    return {startSearch, &group(startSearch)};
  else if (startSearch + 1 < order_.size() && group(startSearch + 1).containsWeight(index))
    return {startSearch + 1, &group(startSearch + 1)};

  for (size_t i = 0; i < order_.size(); i++) {
    if (group(i).containsWeight(index))
      return {i, &group(i)};
  }

  throw cms::Exception("GenWeightInfoProduct")
      << "No weight group found containing weight index " << index << " in the " << order_.size() << " groups.";
}

const gen::WeightGroupInfo* GenWeightInfoProduct::orderedWeightGroupInfo(int weightGroupIndex) const {
  if (weightGroupIndex < 0 || weightGroupIndex >= static_cast<int>(order_.size()))
    throw cms::Exception("GenWeightInfoProduct") << "Weight index requested is outside the range of weights in the "
                                                    "product";
  return &group(weightGroupIndex);
}

std::vector<gen::WeightGroupData> GenWeightInfoProduct::weightGroupsAndIndicesByType(gen::WeightType type,
                                                                                     int maxStore) const {
  std::vector<gen::WeightGroupData> matchingGroups;
  size_t toStore = maxStore <= 0 ? order_.size() : std::min<size_t>(maxStore, order_.size());
  for (size_t i = 0; i < order_.size(); i++) {
    if (order_[i].type == type) {
      matchingGroups.push_back({i, &group(i)});
      if (matchingGroups.size() == toStore)
        break;
    }
  }
  return matchingGroups;
}

std::vector<gen::WeightGroupData> GenWeightInfoProduct::weightGroupsByType(gen::WeightType type) const {
  return weightGroupsAndIndicesByType(type);
}

std::optional<gen::WeightGroupData> GenWeightInfoProduct::pdfGroupWithIndexByLHAID(int lhaid) const {
  for (size_t i = 0; i < order_.size(); i++) {
    if (order_[i].type == gen::WeightType::kPdfWeights &&
        weightGroup<gen::PdfWeightGroupInfo>(i).containsLhapdfId(lhaid))
      return gen::WeightGroupData{i, &group(i)};
  }
  return std::nullopt;
}

std::vector<gen::WeightGroupData> GenWeightInfoProduct::pdfGroupsWithIndicesByLHAIDs(
    const std::vector<int>& lhaids) const {
  std::vector<gen::WeightGroupData> groups;
  for (auto lhaid : lhaids) {
    auto matchingPdfSet = pdfGroupWithIndexByLHAID(lhaid);
    if (matchingPdfSet)
      groups.push_back(*matchingPdfSet);
  }
  return groups;
}

std::vector<int> GenWeightInfoProduct::weightGroupIndicesByType(gen::WeightType type) const {
  std::vector<int> matchingGroupIndices;
  for (size_t i = 0; i < order_.size(); i++) {
    if (order_[i].type == type)
      matchingGroupIndices.push_back(i);
  }
  return matchingGroupIndices;
}

void GenWeightInfoProduct::addWeightGroupInfo(const gen::WeightGroup& info) {
  if (gen::weightGroupBase(info).name() == "unassociated")
    unassociatedIdx_ = order_.size();
  std::visit(
      [this](const auto& group) {
        using T = std::decay_t<decltype(group)>;
        auto& groups = groupsOfType<T>(*this);
        order_.push_back({weightTypeOf<T>(), static_cast<uint32_t>(groups.size())});
        groups.push_back(group);
      },
      info);
}
