#ifndef SimDataFormats_GeneratorProducts_GenWeightInfoProduct_h
#define SimDataFormats_GeneratorProducts_GenWeightInfoProduct_h

#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "FWCore/Utilities/interface/Exception.h"
#include "SimDataFormats/GeneratorProducts/interface/MEParamWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/PartonShowerWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/ScaleWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/UnknownWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"

namespace gen {
  struct WeightGroupData {
    size_t index;
    const gen::WeightGroupInfo* group;
  };

  // Position of a group in the per-type vectors of the GenWeightInfoProduct
  struct WeightGroupLocation {
    WeightType type;
    uint32_t index;
  };

  // Transient container used while the groups are built from the header (never persisted)
  typedef std::variant<UnknownWeightGroupInfo,
                       ScaleWeightGroupInfo,
                       PdfWeightGroupInfo,
                       MEParamWeightGroupInfo,
                       PartonShowerWeightGroupInfo>
      WeightGroup;
  typedef std::vector<WeightGroup> WeightGroupInfoContainer;

  inline WeightGroupInfo& weightGroupBase(WeightGroup& group) {
    return std::visit([](auto& g) -> WeightGroupInfo& { return g; }, group);
  }
  inline const WeightGroupInfo& weightGroupBase(const WeightGroup& group) {
    return std::visit([](const auto& g) -> const WeightGroupInfo& { return g; }, group);
  }
}  // namespace gen

class GenWeightInfoProduct {
public:
  GenWeightInfoProduct() {}
  GenWeightInfoProduct(const gen::WeightGroupInfoContainer& weightGroups);

  const std::vector<gen::WeightGroupData> allWeightGroupsInfoWithIndices() const;
  gen::WeightGroupData containingWeightGroupInfo(int index, size_t startSearch = 0) const;
  const gen::WeightGroupInfo* orderedWeightGroupInfo(int index) const;
  // The group at the given index as its concrete type; throws if the group has another type
  template <typename T>
  const T& weightGroup(size_t index) const;
  std::vector<gen::WeightGroupData> weightGroupsByType(gen::WeightType type) const;
  std::vector<int> weightGroupIndicesByType(gen::WeightType type) const;
  std::vector<gen::WeightGroupData> weightGroupsAndIndicesByType(gen::WeightType type, int maxStore = -1) const;
  std::optional<gen::WeightGroupData> pdfGroupWithIndexByLHAID(int lhaid) const;
  std::vector<gen::WeightGroupData> pdfGroupsWithIndicesByLHAIDs(const std::vector<int>& lhaids) const;
  void addWeightGroupInfo(const gen::WeightGroup& info);
  const int numberOfGroups() const { return order_.size(); }
  // If there are unassociated weights, the number of filled groups will be less
  // than the number of groups, because the unassociated group can't be filled.
  // Likewise the number of weights in the GenWeightInfoProduct product will be
  // less than the number of weights in the event
  const int numberOfFilledGroups() const;
  const int numberOfWeights() const;
  const int unassociatedIdx() const { return unassociatedIdx_; }

private:
  const gen::WeightGroupInfo& group(size_t index) const;
  template <typename T>
  static constexpr gen::WeightType weightTypeOf();
  // Shared by the const and non-const access to the per-type vectors
  template <typename T, typename Self>
  static auto& groupsOfType(Self& self);

  // The groups are stored by type; order_ gives their position in the event weight vector
  std::vector<gen::UnknownWeightGroupInfo> unknownGroups_;
  std::vector<gen::ScaleWeightGroupInfo> scaleGroups_;
  std::vector<gen::PdfWeightGroupInfo> pdfGroups_;
  std::vector<gen::MEParamWeightGroupInfo> meParamGroups_;
  std::vector<gen::PartonShowerWeightGroupInfo> partonShowerGroups_;
  std::vector<gen::WeightGroupLocation> order_;
  int unassociatedIdx_ = -1;
};

template <typename T>
constexpr gen::WeightType GenWeightInfoProduct::weightTypeOf() {
  if constexpr (std::is_same_v<T, gen::UnknownWeightGroupInfo>)
    return gen::WeightType::kUnknownWeights;
  else if constexpr (std::is_same_v<T, gen::ScaleWeightGroupInfo>)
    return gen::WeightType::kScaleWeights;
  else if constexpr (std::is_same_v<T, gen::PdfWeightGroupInfo>)
    return gen::WeightType::kPdfWeights;
  else if constexpr (std::is_same_v<T, gen::MEParamWeightGroupInfo>)
    return gen::WeightType::kMEParamWeights;
  else {
    static_assert(std::is_same_v<T, gen::PartonShowerWeightGroupInfo>, "Not a weight group type");
    return gen::WeightType::kPartonShowerWeights;
  }
}

template <typename T, typename Self>
auto& GenWeightInfoProduct::groupsOfType(Self& self) {
  if constexpr (std::is_same_v<T, gen::UnknownWeightGroupInfo>)
    return self.unknownGroups_;
  else if constexpr (std::is_same_v<T, gen::ScaleWeightGroupInfo>)
    return self.scaleGroups_;
  else if constexpr (std::is_same_v<T, gen::PdfWeightGroupInfo>)
    return self.pdfGroups_;
  else if constexpr (std::is_same_v<T, gen::MEParamWeightGroupInfo>)
    return self.meParamGroups_;
  else {
    static_assert(std::is_same_v<T, gen::PartonShowerWeightGroupInfo>, "Not a weight group type");
    return self.partonShowerGroups_;
  }
}

template <typename T>
const T& GenWeightInfoProduct::weightGroup(size_t index) const {
  if (index >= order_.size() || order_[index].type != weightTypeOf<T>())
    throw cms::Exception("GenWeightInfoProduct") << "Weight group " << index << " is not of the requested type";
  return groupsOfType<T>(*this).at(order_[index].index);
}

#endif  // SimDataFormats_GeneratorProducts_GenWeightInfoProduct_h
