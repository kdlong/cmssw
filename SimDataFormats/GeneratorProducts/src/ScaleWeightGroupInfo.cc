#include "SimDataFormats/GeneratorProducts/interface/ScaleWeightGroupInfo.h"
#include <stdexcept>
#include <iostream>

namespace gen {
    void ScaleWeightGroupInfo::copy(const ScaleWeightGroupInfo &other) {
        centralIndex_ = other.centralIndex_;
        muR1muF2Index_ = other.muR1muF2Index_;
        muR1muF05Index_ = other.muR1muF05Index_;
        muR2muF05Index_ = other.muR2muF05Index_;
        muR2muF1Index_ = other.muR2muF1Index_;
        muR2muF2Index_ = other.muR2muF2Index_;
        muR05muF05Index_ = other.muR2muF05Index_;
        muR05muF1Index_ = other.muR05muF1Index_;
        muR05muF2Index_ = other.muR05muF2Index_;
        WeightGroupInfo::copy(other);
    }

    ScaleWeightGroupInfo* ScaleWeightGroupInfo::clone() const {
        return new ScaleWeightGroupInfo(*this);
    }

    void ScaleWeightGroupInfo::addContainedId(int globalIndex, std::string id, std::string label, float muR, float muF) {
        WeightGroupInfo::addContainedId(globalIndex, id, label);
        setMuRMuFIndex(globalIndex, id, muR, muF);
    }

    void ScaleWeightGroupInfo::setMuRMuFIndex(int globalIndex, std::string id, float muR, float muF) {
        auto metaInfo = weightMetaInfoByGlobalIndex(id, globalIndex);
        setMuRMuFIndex(metaInfo, muR, muF);
    }

    void ScaleWeightGroupInfo::setMuRMuFIndex(WeightMetaInfo& info, float muR, float muF) {
        if (muR == 0.5 && muF == 0.5)
            muR05muF05Index_ = info.localIndex;
        else if (muR == 0.5 && muF == 1.0)
            muR05muF1Index_ = info.localIndex;
        else if (muR == 0.5 && muF == 2.0)
            muR05muF2Index_ = info.localIndex;
        else if (muR == 1.0 && muF == 0.5)
            muR1muF05Index_ = info.localIndex;
        else if (muR == 1.0 && muF == 1.0)
            centralIndex_ = info.localIndex;
        else if (muR == 1.0 && muF == 2.0)
            muR1muF2Index_ = info.localIndex;
        else if (muR == 2.0 && muF == 0.5)
            muR2muF05Index_ = info.localIndex;
        else if (muR == 2.0 && muF == 1.0)
            muR2muF1Index_ = info.localIndex;
        else if (muR == 2.0 && muF == 2.0)
            muR2muF2Index_ = info.localIndex;
        else
            isWellFormed_ = false;
    }
}


