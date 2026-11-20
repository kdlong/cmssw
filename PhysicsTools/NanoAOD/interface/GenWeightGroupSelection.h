#ifndef PhysicsTools_NanoAOD_GenWeightGroupSelection_h
#define PhysicsTools_NanoAOD_GenWeightGroupSelection_h

#include <cstddef>
#include <vector>
#include "SimDataFormats/GeneratorProducts/interface/GenWeightInfoProduct.h"

namespace nanoaod {
  inline std::vector<gen::WeightGroupData> selectPdfWeightGroups(const GenWeightInfoProduct& info,
                                                                 const std::vector<int>& pdfIds,
                                                                 size_t maxGroups,
                                                                 bool fallbackToFirstPDF) {
    if (pdfIds.empty())
      return info.weightGroupsAndIndicesByType(gen::WeightType::kPdfWeights, maxGroups);
    auto groups = info.pdfGroupsWithIndicesByLHAIDs(pdfIds);
    if (groups.empty() && fallbackToFirstPDF)
      groups = info.weightGroupsAndIndicesByType(gen::WeightType::kPdfWeights, 1);
    if (groups.size() > maxGroups)
      groups.resize(maxGroups);
    return groups;
  }
}  // namespace nanoaod
#endif
