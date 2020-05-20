#ifndef SimDataFormats_GeneratorProducts_PdfWeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_PdfWeightGroupInfo_h

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include "LHAPDF/LHAPDF.h"
#include <iostream>
#include <set>
#include <unordered_map>

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
            // Map local index to lhaid
            std::unordered_map<int, int> lhapdfIdsContained_;
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

        std::unordered_map<int, int> getLHAPDFids() {return lhapdfIdsContained_;}
        int getLHAPDFidFromIdx(int idx) const { return lhapdfIdsContained_.at(idx); }
        void setUncertaintyType(PdfUncertaintyType uncertaintyType) { uncertaintyType_ = uncertaintyType; }
        void setHasAlphasVariations(bool hasAlphasVars) { hasAlphasVars_ = hasAlphasVars; }
        void setAlphasUpIndex(int alphasUpIndex) { alphasUpIndex_ = alphasUpIndex; }
        void setAlphasDownIndex(int alphasDownIndex) { alphasDownIndex_ = alphasDownIndex; }
        PdfUncertaintyType uncertaintyType() const { return uncertaintyType_; }
        bool hasAlphasVariations() const { return hasAlphasVars_; }
        std::vector<WeightMetaInfo> idsContainedInPdfSet(int refLhaid) {
            std::vector<WeightMetaInfo> setIds;
            int lhaid = refLhaid;
            int index = indexOfLhapdfId(lhaid);
            while (index <= lastId_ && parentLhapdfId(lhaid) == refLhaid) {
                setIds.push_back(idsContained_.at(index));
                    index++;
                    lhaid = lhapdfIdsContained_[index];
                }
                return setIds;
            }
            bool containsMultipleSets() const { return lhapdfIdsContained_.size() > 1; }
        int parentLhapdfId(int lhaid) const {
            return lhaid - LHAPDF::lookupPDF(lhaid).second;
        }
        bool containsParentLhapdfId(int lhaid) const { 
            if (indexOfLhapdfId(lhaid) != -1)
                return true;
                            
            return indexOfLhapdfId(parentLhapdfId(lhaid)) != -1;
        }
            bool containsLhapdfId(int lhaid) const { return indexOfLhapdfId(lhaid) != -1; }
            int indexOfLhapdfId(int lhaid) const {
                for (const auto& id : lhapdfIdsContained_) {
                    if (id.second == lhaid)
                        return id.first;
                }
                return -1;
            }
            int alphasUpIndex() const { return alphasUpIndex_; }
            int alphasDownIndex() const { return alphasDownIndex_; }
            void addLhapdfId(int lhaid, int globalIndex) { 
                lhapdfIdsContained_[globalIndex-firstId_] = lhaid; 
            }
            std::vector<int> lhapdfIdsContained() const { 
                std::vector<int> lhaids;
                for (const auto& id : lhapdfIdsContained_) {
                    lhaids.push_back(id.first);
                }
                return lhaids; 
            }
            void removeIdsContained(std::vector<WeightMetaInfo> metaInfos) { 
                for (auto& weight : metaInfos) {
                    auto info = weightMetaInfoByGlobalIndex(weight.id, weight.globalIndex);
                    if (info == weight) {
                        idsContained_.erase(idsContained_.begin()+weight.localIndex, idsContained_.begin()+weight.localIndex+1);
                        lhapdfIdsContained_.erase(weight.localIndex);
                    }
                }
            }
            void removeIdsContainedExcept(std::vector<WeightMetaInfo> metaInfos) { 
                std::vector<WeightMetaInfo> newContainedIds;
                std::unordered_map<int, int> newLhapdfIds;
                for (auto& weight : metaInfos) {
                    auto info = weightMetaInfoByGlobalIndex(weight.id, weight.globalIndex);
                    if (info == weight) {
                        newContainedIds.push_back(weight);
                        if (lhapdfIdsContained_.find(weight.localIndex) != lhapdfIdsContained_.end())
                            newLhapdfIds[weight.localIndex] = lhapdfIdsContained_[weight.localIndex];
                    }
                }
                idsContained_ = newContainedIds;
                lhapdfIdsContained_ = newLhapdfIds;
            }
    };
}


#endif // SimDataFormats_GeneratorProducts_PdfWeightGroupInfo_h

