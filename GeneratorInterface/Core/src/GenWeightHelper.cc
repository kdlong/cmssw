#include "GeneratorInterface/Core/interface/GenWeightHelper.h"
#include <iostream>

using namespace tinyxml2;

namespace gen {
    GenWeightHelper::GenWeightHelper() {
    }

    void
    GenWeightHelper::parseWeightGroupsFromNames(std::vector<std::string> weightNames) {
        int index = 0;
        
        if (weightNames.size() <= 1)
            return;

        for (std::string weightName : weightNames) {
            if(weightName.find("LHE") != std::string::npos) {
                // Parse as usual, this is the SUSY workflow
                // std::vector<std::string> info;
                // boost::split(info, weightName, boost::is_any_of(","));
                weightGroups_.push_back(new gen::UnknownWeightGroupInfo("inGen"));
            }
            // Working on the not-so-nice assumption that all non-LHE gen weights are PS weights
            else if (weightGroups_.size() == 0) {
                weightGroups_.push_back(new gen::PartonShowerWeightGroupInfo("shower"));
            }
            auto& group = weightGroups_.back();
            // No IDs for Gen weights
            group.addContainedId(index++, "", weightName);
        }
    }
}


