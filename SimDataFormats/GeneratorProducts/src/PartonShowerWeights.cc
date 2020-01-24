#include "SimDataFormats/GeneratorProducts/interface/PartonShowerWeightGroupInfo.h"

namespace gen {
    void PartonShowerWeightGroupInfo::copy(const PartonShowerWeightGroupInfo &other) {
        WeightGroupInfo::copy(other);
    }

    PartonShowerWeightGroupInfo* PartonShowerWeightGroupInfo::clone() const {
        return new PartonShowerWeightGroupInfo(*this);
    }
}

