#ifndef SimDataFormats_GeneratorProducts_PartonShowerWeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_PartonShowerWeightGroupInfo_h

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"

namespace gen {
    class PartonShowerWeightGroupInfo : public WeightGroupInfo {
        public:
            PartonShowerWeightGroupInfo() : PartonShowerWeightGroupInfo("") {}
	        PartonShowerWeightGroupInfo(std::string header, std::string name) : 
                WeightGroupInfo(header, name) { 
                weightType_ = WeightType::kPartonShowerWeights;
            }
	        PartonShowerWeightGroupInfo(std::string header) : 
                PartonShowerWeightGroupInfo(header, header) { } 
            PartonShowerWeightGroupInfo(const PartonShowerWeightGroupInfo &other) {
                copy(other);
            }
            virtual ~PartonShowerWeightGroupInfo() override {}
            void copy(const PartonShowerWeightGroupInfo &other);
            virtual PartonShowerWeightGroupInfo* clone() const override;

            // Is a variation of the functional form of the dynamic scale
    };
}

#endif


