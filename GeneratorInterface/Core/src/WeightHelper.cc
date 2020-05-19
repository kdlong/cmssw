#include "GeneratorInterface/Core/interface/WeightHelper.h"
#include <regex>

namespace gen {
    WeightHelper::WeightHelper() : pdfSetsInfo(setupPdfSetsInfo())
    {
        model_ = "";
	
    }

    std::vector<PdfSetInfo> WeightHelper::setupPdfSetsInfo() {
        std::vector<PdfSetInfo> tmpSetsInfo;
        std::string lhapdf_path = std::getenv("LHAPDF_DATA_PATH");
	std::ifstream pdf_file;
	pdf_file.open(lhapdf_path+"/pdfsets.index");
	int lha_set, dummy;
	std::string lha_name;
	while(pdf_file >> lha_set >> lha_name >> dummy) {
            tmpSetsInfo.push_back({lha_name, lha_set, kUnknownUnc});   
	}
        return tmpSetsInfo;
    }

    
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

    bool WeightHelper::isOrphanPdfWeightGroup(ParsedWeight& weight) {
        std::string lhaidText = searchAttributes("pdf", weight);
        auto pairLHA = LHAPDF::lookupPDF(stoi(lhaidText));
        // require pdf set to exist and it to be the first entry (ie 0)
        // possibly change this requirement
        if(pairLHA.first != "" && pairLHA.second == 0) {
            weight.groupname = std::string(pairLHA.first);
            return true;
        }
        return false;
    }
    
    bool WeightHelper::isMEParamWeightGroup(const ParsedWeight& weight) {
        return (weight.groupname.find("mg_reweighting") != std::string::npos);
    }

    std::string WeightHelper::searchAttributes(const std::string& label, const ParsedWeight& weight) const {
        std::string attribute = searchAttributesByTag(label, weight);
        return attribute.empty() ? searchAttributesByRegex(label, weight) : attribute;
        attribute = searchAttributesByRegex(label, weight);
    }

    std::string WeightHelper::searchAttributesByTag(const std::string& label, const ParsedWeight& weight) const {
        for (const auto& lab : attributeNames_.at(label)) {
            auto& attributes = weight.attributes;
            if (attributes.find(lab) != attributes.end()) {
                return boost::algorithm::trim_copy_if(attributes.at(lab), boost::is_any_of("\""));
            }
        }
        return "";
    }

    std::string WeightHelper::searchAttributesByRegex(const std::string& label, const ParsedWeight& weight) const {
        for (const auto& lab : attributeNames_.at(label)) {
            auto& content = weight.content;
            
            std::regex expr(lab+"=([0-9.]+(?:[eE][+-]?[0-9]+)?)");
            std::smatch match;
            if (std::regex_search(content, match, expr)) {
                return boost::algorithm::trim_copy(match.str(1));
            }
        }
        return "";
    }

    void WeightHelper::updateScaleInfo(const ParsedWeight& weight) {
        auto& group = weightGroups_.back();
        auto& scaleGroup = dynamic_cast<gen::ScaleWeightGroupInfo&>(group);
        std::string muRText = searchAttributes("mur", weight);
        std::string muFText = searchAttributes("muf", weight);
        if (muRText.empty() || muFText.empty()) {
            scaleGroup.setIsWellFormed(false);
            return;
        }
	// currently skips events with a dynscale. May add back
        //size_t dyn = -1;
	if (weight.attributes.find("DYN_SCALE") != weight.attributes.end()) {
	    //  dyn = std::stoi(boost::algorithm::trim_copy_if(weight.attributes.at("DYN_SCALE"), boost::is_any_of("\"")));
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
		std::string description = "";
		auto pdfInfo = std::find_if(pdfSetsInfo.begin(), pdfSetsInfo.end(),
					    [lhaid] (const PdfSetInfo& setInfo) { return setInfo.lhapdfId == lhaid; });
		if (pdfInfo != pdfSetsInfo.end()) {
		    pdfGroup.setUncertaintyType(pdfInfo->uncertaintyType);
		    if (pdfInfo->uncertaintyType == gen::kHessianUnc)
			description += "Hessian ";
		    else if (pdfInfo->uncertaintyType == gen::kMonteCarloUnc)
			description += "Monte Carlo ";
		    description += "Uncertainty sets for LHAPDF set " + pdfInfo->name;
		    description += " with LHAID = " + std::to_string(lhaid);
		    description += "; ";
		}
		//else
		//    description += "Uncertainty sets for LHAPDF set with LHAID = " + std::to_string(lhaid);
		pdfGroup.addLhapdfId(lhaid, weight.index);
		pdfGroup.appendDescription(description);
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

    void WeightHelper::splitPdfGroups() {
	// std::vector<gen::PdfWeightGroupInfo> groupsToSplit;
	// for (auto& group: weightGroups_) {
	//     if (group.weightType() == gen::WeightType::kPdfWeights) {
	// 	gen::PdfWeightGroupInfo& = dynamic_cast<gen::PdfWeightGroupInfo&>(group);
	// 	if (group.containsMultipleSets())
	// 	    groupsToSplit.push_back(group);
	//     }
	// }
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

