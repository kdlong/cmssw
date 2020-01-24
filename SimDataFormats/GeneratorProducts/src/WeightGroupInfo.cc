#include <string>
#include <algorithm>
#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include "FWCore/Utilities/interface/Exception.h"

#include <iostream>

namespace gen {
    void WeightGroupInfo::copy(const WeightGroupInfo &other) {
        isWellFormed_ = other.isWellFormed_;
        headerEntry_ = other.headerEntry_;
        name_ = other.name_;
        description_ = other.description_;
        weightType_ = other.weightType_;
        idsContained_ = other.idsContained_;
        firstId_ = other.firstId_;
        lastId_ = other.lastId_;
    }

    WeightGroupInfo* WeightGroupInfo::clone() const {
        throw cms::Exception("LogicError", "WeightGroupInfo is abstract, so it's clone() method can't be implemented.\n");
    }

    WeightMetaInfo WeightGroupInfo::weightMetaInfo(int weightEntry) const {
        return idsContained_.at(weightEntry);
    }

    WeightMetaInfo WeightGroupInfo::weightMetaInfoByGlobalIndex(std::string wgtId, int weightEntry) const {
        int entry = weightVectorEntry(wgtId, weightEntry);
        if (entry < 0 || entry >= static_cast<int>(idsContained_.size()))
            throw std::range_error("Weight entry " + std::to_string(weightEntry) + 
                    " is not a member of group " + name_ + 
                    ". \n    firstID = " + std::to_string(firstId_) + " lastId = " + std::to_string(lastId_));
        return idsContained_.at(entry);
    }

    int WeightGroupInfo::weightVectorEntry(const std::string& wgtId) const {
        return weightVectorEntry(wgtId, 0);
    }

    bool WeightGroupInfo::containsWeight(const std::string& wgtId, int weightEntry) const {
        return weightVectorEntry(wgtId, weightEntry) != -1;
    }

    int WeightGroupInfo::weightVectorEntry(const std::string& wgtId, int weightEntry) const {
        // First try ordered search
        if (indexInRange(weightEntry)) {
            size_t orderedEntry = weightEntry - firstId_;
            if (orderedEntry < idsContained_.size())
                if (wgtId.empty() || idsContained_.at(orderedEntry).id == wgtId) {
                    return orderedEntry;
                }
        }
        // Fall back to search on ID
        else if (!wgtId.empty()) {
            auto it = std::find_if(idsContained_.begin(), idsContained_.end(), 
                            [wgtId] (const WeightMetaInfo& w) { return w.id == wgtId; });
            if (it != idsContained_.end())
                return std::distance(idsContained_.begin(), it);
        }
        return -1;
    }

    void WeightGroupInfo::addContainedId(int weightEntry, std::string id, std::string label="") {
        if (firstId_ == -1 || weightEntry < firstId_) {
            firstId_ = weightEntry;
            // Reset to reflect that indices will be shifted
            for (auto& entry : idsContained_)
                entry.localIndex = entry.globalIndex - firstId_;
        }
        if (weightEntry > lastId_)
            lastId_ = weightEntry;
        
        WeightMetaInfo info;
        info.globalIndex = weightEntry;
        info.localIndex = weightEntry - firstId_;
        info.id = id;
        info.label = label;

        if (idsContained_.size() < info.localIndex) {
            idsContained_.resize(info.localIndex);
            idsContained_.insert(idsContained_.begin()+info.localIndex, info);
        }
        else if (idsContained_.size() == info.localIndex) {
            idsContained_.push_back(info);
        }
        else {
            idsContained_.resize(info.localIndex+1);
            idsContained_[info.localIndex] = info;
        }
    }

    std::vector<WeightMetaInfo> WeightGroupInfo::containedIds() const { return idsContained_; }


    bool WeightGroupInfo::indexInRange(int index) const {
        return (index <= lastId_ && index >= firstId_);
    }
}
