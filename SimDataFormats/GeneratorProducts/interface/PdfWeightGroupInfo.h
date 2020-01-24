#ifndef SimDataFormats_GeneratorProducts_PdfWeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_PdfWeightGroupInfo_h

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include <iostream>
#include <set>

namespace gen {
    enum PdfUncertaintyType {
        kHessianUnc,
        kMonteCarloUnc,
        kVariationSet,
        kUnknownUnc,
    };

    class PdfWeightGroupInfo : public WeightGroupInfo {
        private:
            PdfUncertaintyType uncertaintyType_;
            bool hasAlphasVars_;
            int alphasUpIndex_;
            int alphasDownIndex_;
            std::vector<std::pair<int, int>> lhapdfIdsContained_;
        public:
            PdfWeightGroupInfo() : WeightGroupInfo() { weightType_ = WeightType::kPdfWeights; }
	        PdfWeightGroupInfo(std::string header, std::string name) : 
                WeightGroupInfo(header, name) { weightType_ = WeightType::kPdfWeights; } 
	        PdfWeightGroupInfo(std::string header) : 
                WeightGroupInfo(header) { weightType_ = WeightType::kPdfWeights; } 
            PdfWeightGroupInfo(const PdfWeightGroupInfo &other) {
                copy(other);
            }
            virtual ~PdfWeightGroupInfo() override {}
            void copy(const PdfWeightGroupInfo &other);
            virtual PdfWeightGroupInfo* clone() const override;

            void setUncertaintyType(PdfUncertaintyType uncertaintyType) { uncertaintyType_ = uncertaintyType; }
            void setHasAlphasVariations(bool hasAlphasVars) { hasAlphasVars_ = hasAlphasVars; }
            void setAlphasUpIndex(int alphasUpIndex) { alphasUpIndex_ = alphasUpIndex; }
            void setAlphasDownIndex(int alphasDownIndex) { alphasDownIndex_ = alphasDownIndex; }
            PdfUncertaintyType uncertaintyType() const { return uncertaintyType_; }
            bool hasAlphasVariations() const { return hasAlphasVars_; }
            bool containsMultipleSets() const { return lhapdfIdsContained_.size() > 1; }
            bool containsParentLhapdfId(int lhaid, int globalIndex) const { 
                if (indexOfLhapdfId(lhaid) != -1)
                    return true;
                int parentid = lhaid - (globalIndex - firstId_);
                return indexOfLhapdfId(parentid) != -1; 
            }
            bool containsLhapdfId(int lhaid) const { return indexOfLhapdfId(lhaid) != -1; }
            int indexOfLhapdfId(int lhaid) const {
                for (const auto& id : lhapdfIdsContained_) {
                    if (id.first == lhaid)
                        return id.second;
                }
                return -1;
            }
            int alphasUpIndex() const { return alphasUpIndex_; }
            int alphasDownIndex() const { return alphasDownIndex_; }
            void addLhapdfId(int lhaid, int globalIndex) { 
                lhapdfIdsContained_.push_back(std::make_pair(lhaid, globalIndex-firstId_)); 
            }
            std::vector<int> lhapdfIdsContained() const { 
                std::vector<int> lhaids;
                for (const auto& id : lhapdfIdsContained_) {
                    lhaids.push_back(id.first);
                }
                return lhaids; 
            }
    };
}


#endif // SimDataFormats_GeneratorProducts_PdfWeightGroupInfo_h

