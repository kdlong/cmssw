#ifndef SimDataFormats_GeneratorProducts_LHEWeightInfoProduct_h
#define SimDataFormats_GeneratorProducts_LHEWeightInfoProduct_h

#include <iterator>
#include <memory>
#include <vector>
#include <string>

//#include <hepml.hpp>

#include "SimDataFormats/GeneratorProducts/interface/LesHouches.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"

class LHEWeightInfoProduct {
    public:
        LHEWeightInfoProduct() {}
	    LHEWeightInfoProduct(const LHEWeightInfoProduct& other); 
	    LHEWeightInfoProduct(LHEWeightInfoProduct&& other);
        ~LHEWeightInfoProduct() {}
        //LHEWeightInfoProduct(std::vector<gen::WeightGroupInfo>& weightGroups) {
        //    weightGroupsInfo_ = weightGroups;
        //}
        LHEWeightInfoProduct(const char* var);
        LHEWeightInfoProduct& operator=(const LHEWeightInfoProduct &other); 
        LHEWeightInfoProduct& operator=(LHEWeightInfoProduct &&other); 

        const char* getWeightGroupsInfo();
        //std::vector<gen::WeightGroupInfo> getWeightGroupsInfo() { return weightGroupsInfo_; }
        //void addWeightGroupInfo(gen::WeightGroupInfo info) {  
        //    weightGroupsInfo_.push_back(info); 
        //}
        void addWeightGroupInfo(const char* var);

    private:
        const char* weightGroupsInfo_;
        //std::vector<gen::WeightGroupInfo> weightGroupsInfo_;


};

#endif // GeneratorWeightInfo_LHEInterface_LHEWeightInfoProduct_h

