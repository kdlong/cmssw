#include "GeneratorInterface/Core/interface/WeightHelper.h"

namespace gen {
    WeightHelper::WeightHelper() :
        pdfSetsInfo({
            // In principle this can be parsed from $LHAPDF_DATA_PATH/pdfsets.index,
            // but do we really want to do that? Can also just hardcode a subset...
            // TODO: Actually we can just take this from LHAPDF
            {"NNPDF31_nnlo_hessian_pdfas", 306000, kHessianUnc},
            {"NNPDF31_nnlo_as_0118", 303600, kMonteCarloUnc},
            {"NNPDF31_nlo_as_0118", 303400, kMonteCarloUnc},
            {"NNPDF31_nlo_hessian_pdfas", 305800, kHessianUnc},
            {"NNPDF31_nnlo_as_0108", 322500, kVariationSet}, 
            {"NNPDF31_nnlo_as_0110", 322700, kVariationSet},
            {"NNPDF31_nnlo_as_0112", 322900, kVariationSet},
            {"NNPDF31_nnlo_as_0114", 323100, kVariationSet},
            {"NNPDF31_nnlo_as_0117", 323300, kVariationSet},
            {"NNPDF31_nnlo_as_0119", 323500, kVariationSet},
            {"NNPDF31_nnlo_as_0122", 323700, kVariationSet},
            {"NNPDF31_nnlo_as_0124", 323900, kVariationSet},
            {"NNPDF31_nnlo_as_0118_nf_4_mc_hessian", 325500, kHessianUnc},
            {"NNPDF31_nlo_as_0118_nf_4", 320500, kMonteCarloUnc},
            {"NNPDF31_nnlo_as_0118_nf_4", 320900, kMonteCarloUnc},
            {"NNPDF30_nlo_nf_5_pdfas", 292200, kMonteCarloUnc},
            {"NNPDF30_nnlo_nf_5_pdfas", 292600, kMonteCarloUnc},
            {"NNPDF30_nnlo_nf_4_pdfas", 292400, kMonteCarloUnc},
            {"NNPDF30_nlo_nf_4_pdfas", 292000, kMonteCarloUnc},
            {"NNPDF30_lo_as_0130", 263000, kMonteCarloUnc}, 
            {"NNPDF30_lo_as_0118", 262000, kMonteCarloUnc}, 
            {"CT14nnlo", 13000, kHessianUnc},
            {"CT14nlo", 13100, kHessianUnc},
            {"CT14nnlo_as_0116", 13065, kVariationSet},
            {"CT14nnlo_as_0120", 13069, kVariationSet},
            {"CT14nlo_as_0116", 13163, kVariationSet},
            {"CT14nlo_as_0120", 13167, kVariationSet},
            {"CT14lo", 13200, kVariationSet},
            {"MMHT2014nlo68clas118", 25200, kHessianUnc},
            {"MMHT2014nnlo68cl", 25300, kHessianUnc},
            {"MMHT2014lo68cl", 25000, kHessianUnc},
            {"PDF4LHC15_nlo_100_pdfas", 90200, kMonteCarloUnc},
            {"PDF4LHC15_nnlo_100_pdfas", 91200, kMonteCarloUnc},
            {"PDF4LHC15_nlo_30_pdfas", 90400, kMonteCarloUnc},
            {"PDF4LHC15_nnlo_30_pdfas", 91400, kMonteCarloUnc},
            {"ABMP16als118_5_nnlo", 42780, kHessianUnc},
            {"HERAPDF20_NLO_EIG", 61130, kHessianUnc},
            {"HERAPDF20_NNLO_EIG", 61200, kHessianUnc}, 
            {"HERAPDF20_NLO_VAR", 61130, kHessianUnc},
            {"HERAPDF20_NNLO_VAR", 61230, kHessianUnc},
            {"CT14qed_inc_proton", 13400, kHessianUnc},
            {"LUXqed17_plus_PDF4LHC15_nnlo_100", 82200, kMonteCarloUnc},
        })
    { model_ = ""; }
    
    bool WeightHelper::isScaleWeightGroup(const ParsedWeight& weight) {
        return (weight.groupname.find("scale_variation") != std::string::npos
                    || weight.groupname.find("Central scale variation") != std::string::npos);
    }

    bool WeightHelper::isPdfWeightGroup(const ParsedWeight& weight) {
        const std::string& name = weight.groupname;
        if (name.find("PDF_variation") != std::string::npos)
            return true;

        return std::find_if(pdfSetsInfo.begin(), pdfSetsInfo.end(), 
                [name] (const PdfSetInfo& setInfo) { return setInfo.name == name; }) != pdfSetsInfo.end();
    }

    bool WeightHelper::isMEParamWeightGroup(const ParsedWeight& weight) {
        return (weight.groupname.find("mg_reweighting") != std::string::npos);
    }

    std::string WeightHelper::searchAttributes(const std::string& label, const ParsedWeight& weight) {
        for (const auto& lab : attributeNames_.at(label)) {
            auto& attributes = weight.attributes;
            if (attributes.find(lab) != attributes.end()) {
                return boost::algorithm::trim_copy_if(attributes.at(lab), boost::is_any_of("\""));
            }
        }
        // Next regexes
        return "";
    }

    void WeightHelper::updateScaleInfo(const ParsedWeight& weight) {
        auto& group = weightGroups_.back();
        auto& scaleGroup = dynamic_cast<gen::ScaleWeightGroupInfo&>(group);
        std::string muRText = searchAttributes("mur", weight);
        std::string muFText = searchAttributes("mur", weight);
        if (muRText.empty() || muFText.empty()) {
            scaleGroup.setIsWellFormed(false);
            return;
        }

        try {
            float muR = std::stof(muRText);
            float muF = std::stof(muFText);
            scaleGroup.setMuRMuFIndex(weight.index, weight.id, muR, muF);
        }
        catch(std::invalid_argument& e) {
            scaleGroup.setIsWellFormed(false);
        }
    }

    void WeightHelper::updatePdfInfo(const ParsedWeight& weight) {
        auto& pdfGroup = dynamic_cast<gen::PdfWeightGroupInfo&>(weightGroups_.back());
        std::string lhaidText = searchAttributes("pdf", weight);
        int lhaid = 0;
        if (!lhaidText.empty()) {
            try {
                lhaid = std::stoi(lhaidText);
            }
            catch(std::invalid_argument& e) {
                pdfGroup.setIsWellFormed(false);
            }

            if (!pdfGroup.containsParentLhapdfId(lhaid, weight.index)) {
                std::string description;
                auto pdfInfo = std::find_if(pdfSetsInfo.begin(), pdfSetsInfo.end(),
                    [lhaid] (const PdfSetInfo& setInfo) { return setInfo.lhapdfId == lhaid; });
                if (pdfInfo != pdfSetsInfo.end()) {
                    pdfGroup.setUncertaintyType(pdfInfo->uncertaintyType);
                    if (pdfInfo->uncertaintyType == gen::kHessianUnc)
                        description = "Hessian ";
                    else if (pdfInfo->uncertaintyType == gen::kMonteCarloUnc)
                        description = "Monte Carlo ";
                    description += "Uncertainty sets for LHAPDF set " + pdfInfo->name;
                    description += " with LHAID = " + std::to_string(lhaid);
                }
                description = "Uncertainty sets for LHAPDF set with LHAID = " + std::to_string(lhaid);
                pdfGroup.addLhapdfId(lhaid, weight.index);
                pdfGroup.setDescription(description);
            }
        }
        else
            pdfGroup.setIsWellFormed(false);
    }

    // TODO: Could probably recycle this code better
    std::unique_ptr<GenWeightProduct> WeightHelper::weightProduct(std::vector<double> weights, float w0) {
        auto weightProduct = std::make_unique<GenWeightProduct>(w0);
        weightProduct->setNumWeightSets(weightGroups_.size());
        int weightGroupIndex = 0;
        for (unsigned int i = 0; i < weights.size(); i++) {
            addWeightToProduct(weightProduct, weights.at(i), "", i, weightGroupIndex);
        }
        return std::move(weightProduct);
    }

    std::unique_ptr<GenWeightProduct> WeightHelper::weightProduct(std::vector<gen::WeightsInfo> weights, float w0) {
        auto weightProduct = std::make_unique<GenWeightProduct>(w0);
        weightProduct->setNumWeightSets(weightGroups_.size());
        int weightGroupIndex = 0;
        int i = 0;
        for (const auto& weight : weights) {
            weightGroupIndex = addWeightToProduct(weightProduct, weight.wgt, weight.id, i++, weightGroupIndex);
        }
        return std::move(weightProduct);
    }

    int WeightHelper::addWeightToProduct(std::unique_ptr<GenWeightProduct>& product, 
            double weight, std::string name, int weightNum, int groupIndex) {
        groupIndex = findContainingWeightGroup(name, weightNum, groupIndex);
        auto group = weightGroups_[groupIndex];
        int entry = group.weightVectorEntry(name, weightNum);
        product->addWeight(weight, groupIndex, entry);
        return groupIndex;
    }

    int WeightHelper::findContainingWeightGroup(std::string wgtId, int weightIndex, int previousGroupIndex) {
        // Start search at previous index, under expectation of ordered weights
        previousGroupIndex = previousGroupIndex >=0 ? previousGroupIndex : 0;
        for (int index = previousGroupIndex; 
                index < std::min(index+1, static_cast<int>(weightGroups_.size())); index++) {
            const gen::WeightGroupInfo& weightGroup = weightGroups_[index];
            if (weightGroup.indexInRange(weightIndex) && weightGroup.containsWeight(wgtId, weightIndex)) {
                return static_cast<int>(index);
            }
        }

        // Fall back to unordered search
        int counter = 0;
        for (auto weightGroup : weightGroups_) {
            if (weightGroup.containsWeight(wgtId, weightIndex))
                return counter;
            counter++;
        }
        // Needs to be properly handled
        throw std::range_error("Unmatched Generator weight! ID was " + wgtId + " index was " + std::to_string(weightIndex)
                + "\nNot found in any of " + std::to_string(weightGroups_.size()) + " weightGroups.");
    }
}

