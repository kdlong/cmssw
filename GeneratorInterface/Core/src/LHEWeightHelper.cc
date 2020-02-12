#include "GeneratorInterface/Core/interface/LHEWeightHelper.h"
#include <boost/algorithm/string/join.hpp>
#include <iostream>

using namespace tinyxml2;

namespace gen {
    void LHEWeightHelper::setHeaderLines(std::vector<std::string> headerLines) {
	headerLines_ = headerLines;
    }

    void LHEWeightHelper::parseWeights() {
	parsedWeights_.clear();
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.Parse(("<root>" + boost::algorithm::join(headerLines_, "") + "</root>").c_str());
	std::vector<std::string> nameAlts_ = {"name", "type"};
	tinyxml2::XMLElement* root = xmlDoc.FirstChildElement("root");
	size_t weightIndex = 0;
	for (auto* e = root->FirstChildElement(); e != nullptr; e = e->NextSiblingElement()) {
	    std::string groupName = "";
	    if (strcmp(e->Name(), "weight") == 0) {
		// we are here if there is a weight that does not belong to any group
		std::string text = "";
		if (e->GetText()) {
		    text = e->GetText();
		}
		parsedWeights_.push_back({e->Attribute("id"), weightIndex++, groupName, text});
	    }
	    if (strcmp(e->Name(), "weightgroup") == 0) {
		// to deal wiht files with "id" instead of "name"
		for(auto nameAtt : nameAlts_) {
		    if(e->Attribute(nameAtt.c_str())) {
			groupName = e->Attribute(nameAtt.c_str());
			break;
		    }
		}
		if(groupName.empty()) {
		    throw std::runtime_error("couldn't find groupname");
		}
		for (auto* inner = e->FirstChildElement("weight"); inner != nullptr;
		     inner = inner->NextSiblingElement("weight")) {
		    // we are here if there is a weight in a weightgroup
		    std::string text = "";
		    if (inner->GetText())
			text = inner->GetText();
		    std::unordered_map<std::string, std::string> attributes;
		    for (auto* att = inner->FirstAttribute(); att != nullptr; att = att->Next())
			attributes[att->Name()] = att->Value();
		    parsedWeights_.push_back({inner->Attribute("id"), weightIndex++, groupName, text, attributes});
		}
	    }
	}
	buildGroups();
    }

    void LHEWeightHelper::buildGroups() {
	weightGroups_.clear();
	std::string currentGroupName;
	for (const auto& weight : parsedWeights_) {
	    if (weight.groupname != currentGroupName) {
		weightGroups_.push_back(*buildGroup(weight));
	    }
	    currentGroupName = weight.groupname;
	    WeightGroupInfo& group = weightGroups_.back();
	    
	    group.addContainedId(weight.index, weight.id, weight.content);
	    if (group.weightType() == gen::WeightType::kScaleWeights)
		updateScaleInfo(weight);
	    else if (group.weightType() == gen::WeightType::kPdfWeights)
		updatePdfInfo(weight);
	}
	
	// checks
	for(auto& wgt : weightGroups_) {
	    if(! wgt.isWellFormed()) std::cout << "\033[1;31m";
	    std::cout << std::boolalpha << wgt.name() << " (" << wgt.firstId() << "-" << wgt.lastId() << "): " << wgt.isWellFormed() << std::endl;
	    if (wgt.weightType() == gen::WeightType::kScaleWeights) {
		auto& wgtScale = dynamic_cast<gen::ScaleWeightGroupInfo&>( wgt);
		std::cout << wgtScale.centralIndex() << " ";
		std::cout << wgtScale.muR1muF2Index() << " ";
		std::cout << wgtScale.muR1muF05Index() << " ";
		std::cout << wgtScale.muR2muF1Index() << " ";
		std::cout << wgtScale.muR2muF2Index() << " ";
		std::cout << wgtScale.muR2muF05Index() << " ";
		std::cout << wgtScale.muR05muF1Index() << " ";
		std::cout << wgtScale.muR05muF2Index() << " ";
		std::cout << wgtScale.muR05muF05Index() << " \n";
	    } else if (wgt.weightType() == gen::WeightType::kPdfWeights) {
		std::cout << wgt.description() << "\n";
	    }
	    if(! wgt.isWellFormed()) std::cout << "\033[0m";		     
	    //std::cout <<  << "\n";
	}
	//splitPdfGroups();
    }

    std::unique_ptr<WeightGroupInfo> LHEWeightHelper::buildGroup(const ParsedWeight& weight) {
	if (isScaleWeightGroup(weight))
	    return std::make_unique<ScaleWeightGroupInfo>(weight.groupname);
	else if (isPdfWeightGroup(weight))
	    return std::make_unique<PdfWeightGroupInfo>(weight.groupname);
	else if (isMEParamWeightGroup(weight))
	    return std::make_unique<MEParamWeightGroupInfo>(weight.groupname);

	return std::make_unique<UnknownWeightGroupInfo>(weight.groupname);
    }
}
