#ifndef GeneratorInterface_LHEInterface_WeightHelper_h
#define GeneratorInterface_LHEInterface_WeightHelper_h

#include "DataFormats/Common/interface/OwnVector.h"
#include "SimDataFormats/GeneratorProducts/interface/GenWeightProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightsInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/UnknownWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/ScaleWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/MEParamWeightGroupInfo.h"
#include <boost/algorithm/string.hpp>

namespace gen {
    struct PdfSetInfo {
        std::string name;
        int lhapdfId;
        PdfUncertaintyType uncertaintyType; 
    };

    struct ParsedWeight {
        std::string id;
        size_t index;
        std::string groupname;
        std::string content;
        std::unordered_map<std::string, std::string> attributes;
    };
    
    class WeightHelper {
    public:
        WeightHelper();
        edm::OwnVector<gen::WeightGroupInfo> weightGroups() {
            return weightGroups_;
        }
        std::unique_ptr<GenWeightProduct> weightProduct(std::vector<gen::WeightsInfo>, float w0);
        std::unique_ptr<GenWeightProduct> weightProduct(std::vector<double>, float w0);
        void setModel(std::string model) { model_ = model; }
        int addWeightToProduct(std::unique_ptr<GenWeightProduct>& product, double weight, std::string name, int weightNum, int groupIndex);
        int findContainingWeightGroup(std::string wgtId, int weightIndex, int previousGroupIndex);
    protected:
        std::string model_;
        std::vector<ParsedWeight> parsedWeights_;
        const std::vector<PdfSetInfo> pdfSetsInfo;
        std::map<std::string, std::string> currWeightAttributeMap_;
        std::map<std::string, std::string> currGroupAttributeMap_;
        edm::OwnVector<gen::WeightGroupInfo> weightGroups_;
        bool isScaleWeightGroup(const ParsedWeight& weight);
        bool isMEParamWeightGroup(const ParsedWeight& weight);
        bool isPdfWeightGroup(const ParsedWeight& weight);
        void updateScaleInfo(const ParsedWeight& weight);
        void updatePdfInfo(const ParsedWeight& weight);
        std::string searchAttributes(const std::string& label, const ParsedWeight& weight);

        // Possible names for the same thing
        const std::unordered_map<std::string, std::vector<std::string>> attributeNames_ = {
            {"muf",  {"muR", "MUR", "muf","facscfact"}},
            {"mur",  {"muF", "MUF", "mur","renscfact"}},
            {"pdf", {"PDF", "PDF set", "lhapdf", "pdf", "pdf set", "pdfset"}}
        };
    };
}

#endif

