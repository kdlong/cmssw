#ifndef PhysicsTools_NanoAOD_GenWeightGroupSelection_h
#define PhysicsTools_NanoAOD_GenWeightGroupSelection_h

#include <cstddef>
#include <algorithm>
#include <vector>
#include "SimDataFormats/GeneratorProducts/interface/GenWeightInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"

namespace nanoaod {
  inline bool hasPdfUncertaintyVariations(const gen::WeightGroupData& group) {
    const auto& pdf = *static_cast<const gen::PdfWeightGroupInfo*>(group.group);
    return std::any_of(pdf.lhaIds().begin(), pdf.lhaIds().end(), [&pdf](int id) {
      return id > pdf.parentLhapdfId() && pdf.isIdInParentSet(id);
    });
  }

  inline std::vector<gen::WeightGroupData> selectPdfWeightGroups(const GenWeightInfoProduct& info,
                                                                 const std::vector<int>& pdfIds,
                                                                 size_t maxGroups,
                                                                 bool fallbackToFirstPDF) {
    auto groups = pdfIds.empty() ? info.weightGroupsAndIndicesByType(gen::WeightType::kPdfWeights, -1)
                                 : info.pdfGroupsWithIndicesByLHAIDs(pdfIds);
    // In priority mode, prefer a group with actual noncentral members. A
    // complete single-member LHAPDF set still carries no uncertainty weights.
    if (maxGroups == 1) {
      const auto varied = std::find_if(groups.begin(), groups.end(), hasPdfUncertaintyVariations);
      if (varied != groups.end())
        return {*varied};
      if (fallbackToFirstPDF && !pdfIds.empty()) {
        const auto available = info.weightGroupsAndIndicesByType(gen::WeightType::kPdfWeights, -1);
        const auto fallback = std::find_if(available.begin(), available.end(), hasPdfUncertaintyVariations);
        if (fallback != available.end())
          return {*fallback};
      }
    }
    if (groups.empty() && fallbackToFirstPDF && !pdfIds.empty()) {
      const auto available = info.weightGroupsAndIndicesByType(gen::WeightType::kPdfWeights, -1);
      const auto varied = std::find_if(available.begin(), available.end(), hasPdfUncertaintyVariations);
      if (varied != available.end())
        groups = {*varied};
      else if (!available.empty())
        groups = {available.front()};
    }
    if (groups.size() > maxGroups)
      groups.resize(maxGroups);
    return groups;
  }
}  // namespace nanoaod
#endif
