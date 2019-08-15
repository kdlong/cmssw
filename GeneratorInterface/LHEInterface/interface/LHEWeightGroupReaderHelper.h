#ifndef GeneratorInterface_LHEInterface_LHEWeightGroupReaderHelper_h
#define GeneratorInterface_LHEInterface_LHEWeightGroupReaderHelper_h

#include <string>
#include <vector>
#include <map>
#include <regex>
#include <fstream>

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/ScaleWeightGroupInfo.h"


class LHEWeightGroupReaderHelper {
public:
    LHEWeightGroupReaderHelper() : curWeight_(gen::kUnknownWeights) {
        weightGroupStart_ = std::regex(".*<weightgroup.+>.*");
        weightGroupEnd_ = std::regex(".*</weightgroup>.*");
        weightContent_ = std::regex("<weight.*>\\s*(.+)</weight>");
    }

    //// possibly add more versions of this functions for different inputs
    void parseLHEFile(std::string filename);


    edm::OwnVector<gen::WeightGroupInfo> getWeightGroups() {return weightGroups_;}
    
private:
    // Functions
    std::regex createRegexSearch(std::vector<std::string>);
    std::map<std::string, std::string> getTagsMap(std::string, std::regex);

    // Variables
    gen::WeightType curWeight_;
    edm::OwnVector<gen::WeightGroupInfo> weightGroups_;
    std::regex weightGroupStart_;
    std::regex weightGroupEnd_;
    std::regex weightContent_;

};

void
LHEWeightGroupReaderHelper::parseLHEFile(std::string filename) {
    std::ifstream file;
    file.open(filename);

    //// may put in constructor, can have flag to specify these values
    ////     To make this class a little more flexible
    std::vector<std::string> weightGroup = {"name|type", "combine"};
    std::vector<std::string> weightInfo = {"MUF", "id", "MUR", "PDF"};
            
    std::regex groupTags = createRegexSearch(weightGroup);
    std::regex infoTags = createRegexSearch(weightInfo);
    /// end that comment


    std::string line;
    std::smatch matches;
    // TODO: Not sure the weight indexing is right here, this seems to more or less
    // count the lines which isn't quite the goal. TOCHECK!
    int index = 0;
    while(getline(file, line)) {
	if(std::regex_match(line, weightGroupStart_)) {
	    std::string groupLine = line;
	    std::string name = getTagsMap(line, groupTags)["name"];

        //TODO: Fine for now, but in general there should also be a check on the PDF weights,
        // e.g., it could be an unknown weight
	    if(name == "Central scale variation")
            weightGroups_.push_back(new gen::ScaleWeightGroupInfo(groupLine));
	    else
            weightGroups_.push_back(new gen::PdfWeightGroupInfo(groupLine));

	    /// file weights
	    while(getline(file, line) && !std::regex_match(line, weightGroupEnd_)) {
            auto tagsMap = getTagsMap(line, infoTags);
            std::regex_search(line, matches, weightContent_);
            // TODO: Add proper check that it worked
            std::string content = matches[1].str();

            auto& group = weightGroups_.back();
            if (group.weightType() == gen::kScaleWeights) {
                float muR = std::stof(tagsMap["MUR"]);
                float muF = std::stof(tagsMap["MUF"]);
                auto& scaleGroup = static_cast<gen::ScaleWeightGroupInfo&>(group);
                scaleGroup.addContainedId(index, tagsMap["id"], line, muR, muF);
            }
            else
                group.addContainedId(index, tagsMap["id"], line);

            index++;
            
            curWeight_ = gen::kUnknownWeights;
        }
    }
    }
}


std::map<std::string, std::string>
LHEWeightGroupReaderHelper::getTagsMap(std::string s, std::regex r) {
    std::smatch m;

    std::map<std::string, std::string> retMap;
    while(std::regex_search(s, m, r)) {
	for(int i = 1; i < m.length() - 1; i += 2) {
	    if(m[i] != "") {
		retMap[m[i]] = m[i + 1];                
	    }
	}
	s = m.suffix().str();
    }
                
    return retMap;
}



std::regex
LHEWeightGroupReaderHelper::createRegexSearch(std::vector<std::string> names) {
    std::string s = "(?:";
    size_t numNames = names.size();
    for(size_t i=0; i < numNames; ++i) {
	s += "\\s*(" + names[i] + ")=\"([^\"]+)\"";
	if(i != numNames - 1) {
	    s += "|";
	}
    }
    s += ")";

    return std::regex(s);
}


#endif
