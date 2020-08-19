#include "SimDataFormats/GeneratorProducts/interface/MEParamWeightGroupInfo.h"
#include <iostream>

namespace gen {
  void MEParamWeightGroupInfo::copy(const MEParamWeightGroupInfo& other) { WeightGroupInfo::copy(other); }

  MEParamWeightGroupInfo* MEParamWeightGroupInfo::clone() const { return new MEParamWeightGroupInfo(*this); }

  void MEParamWeightGroupInfo::updateWeight(int globalIndex, std::string id, double weight) {
    size_t localIndex = weightMetaInfoByGlobalIndex(id, globalIndex).localIndex;
    std::cout << "Inside here. Local index is " << localIndex << std::endl;
    //auto lower =
    //    std::lower_bound(massValue_.begin(), massValue_.end(), std::make_pair(weight, std::numeric_limits<int>::min()));
    //massValue_.insert(lower, std::make_pair(weight, localIndex));
    //std::cout << "Size of `mass is " << massValue_.size() << " centralIdx_ is " << centralIdx_ << std::endl;
    //isWellFormed_ = massValue_.size() % 2 == 1;
    //if (isWellFormed_) {
    //  numSigma_ = massValue_.size() / 2;
    //  central_ = massValue_.at(centralIdx_).first;
    //  centralIdx_ = massValue_.at(centralIdx_).second;
    //}
  }

  void MEParamWeightGroupInfo::updateWeight(int globalIndex, std::string id, std::vector<std::string>& content) {
    size_t localIndex = weightMetaInfoByGlobalIndex(id, globalIndex).localIndex;
    splitContent_[localIndex] = content;
  }

}  // namespace gen
