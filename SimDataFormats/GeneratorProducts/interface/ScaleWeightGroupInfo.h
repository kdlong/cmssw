#ifndef SimDataFormats_GeneratorProducts_ScaleWeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_ScaleWeightGroupInfo_h

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"

namespace gen {
    class ScaleWeightGroupInfo : public WeightGroupInfo {
        private:
            bool isFunctionalFormVar_;
            size_t centralIndex_;
            size_t muR1muF2Index_;
            size_t muR1muF05Index_;
            size_t muR2muF05Index_;
            size_t muR2muF1Index_;
            size_t muR2muF2Index_;
            size_t muR05muF05Index_;
            size_t muR05muF1Index_;
            size_t muR05muF2Index_;
        public:
            ScaleWeightGroupInfo() : ScaleWeightGroupInfo("") {}
	        ScaleWeightGroupInfo(std::string header, std::string name) : 
                WeightGroupInfo(header, name) { 
                weightType_ = WeightType::kScaleWeights;
                isFunctionalFormVar_ = false;
                centralIndex_ = 0;
                muR1muF2Index_ = 0;
                muR1muF05Index_ = 0;
                muR2muF05Index_ = 0;
                muR2muF1Index_ = 0;
                muR2muF2Index_ = 0;
                muR2muF05Index_ = 0;
                muR05muF05Index_ = 0;
                muR05muF1Index_ = 0;
                muR05muF2Index_ = 0;
            }
	        ScaleWeightGroupInfo(std::string header) : 
                ScaleWeightGroupInfo(header, header) { } 
            ScaleWeightGroupInfo(const ScaleWeightGroupInfo &other) {
                copy(other);
            }
            virtual ~ScaleWeightGroupInfo() override {}
            void copy(const ScaleWeightGroupInfo &other);
            virtual ScaleWeightGroupInfo* clone() const override;

            void setMuRMuFIndex(WeightMetaInfo& info, float muR, float muF);
            void setMuRMuFIndex(int globalIndex, std::string id, float muR, float muF);
            void addContainedId(int weightEntry, std::string id, std::string label, float muR, float muF);

            // Is a variation of the functional form of the dynamic scale
            bool isFunctionalFormVariation();
            void setIsFunctionalFormVariation(bool functionalVar) {isFunctionalFormVar_ = functionalVar; }
            size_t centralIndex() const {return centralIndex_; }
            size_t muR1muF2Index() const { return muR1muF2Index_; }
            size_t muR1muF05Index() const { return muR1muF05Index_; }
            size_t muR2muF05Index() const { return muR2muF05Index_; }
            size_t muR2muF1Index() const { return muR2muF1Index_; }
            size_t muR2muF2Index() const { return muR2muF2Index_; }
            size_t muR05muF05Index() const { return muR05muF05Index_; }
            size_t muR05muF1Index() const { return muR05muF1Index_; }
            size_t muR05muF2Index() const { return muR05muF2Index_; }
    };
}

#endif

