#include <vector>
#include <string>

#include "SimDataFormats/GeneratorProducts/interface/LHEWeightInfoProduct.h"

        //LHEWeightInfoProduct(std::vector<gen::WeightGroupInfo>& weightGroups) {
        //    weightGroupsInfo_ = weightGroups;
        //}
LHEWeightInfoProduct::LHEWeightInfoProduct(const char* var) { 
    weightGroupsInfo_ = var; 
}

LHEWeightInfoProduct& LHEWeightInfoProduct::operator=(const LHEWeightInfoProduct &other) {
    weightGroupsInfo_ = other.weightGroupsInfo_; 
    return * this; 
}

LHEWeightInfoProduct& LHEWeightInfoProduct::operator=(LHEWeightInfoProduct &&other) {
    weightGroupsInfo_ = std::move(other.weightGroupsInfo_); 
    return *this;
}

const char* LHEWeightInfoProduct::getWeightGroupsInfo() { 
    return weightGroupsInfo_; 
}
//std::vector<gen::WeightGroupInfo> getWeightGroupsInfo() { return weightGroupsInfo_; }
//void addWeightGroupInfo(gen::WeightGroupInfo info) {  
//    weightGroupsInfo_.push_back(info); 
//}
void LHEWeightInfoProduct::addWeightGroupInfo(const char* var) {  
    weightGroupsInfo_ = var; 
}
