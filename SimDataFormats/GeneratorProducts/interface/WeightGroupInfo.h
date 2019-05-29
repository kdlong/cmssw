#ifndef SimDataFormats_GeneratorProducts_WeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_WeightGroupInfo_h

/** \class PdfInfo
 *
 */
#include <string>

namespace gen {
    typedef std::pair<size_t, std::string> WeightId;

    enum WeightType {
        pdfWeights,
        scaleWeights,
        matrixElementWeights,
        unknownWeights,
    };

	class WeightGroupInfo {
        public:
	        WeightGroupInfo() {}
	        WeightGroupInfo(std::string header): 
                headerEntry_(header), name_(header), firstId_(0), lastId_(0) {}
            int getWeightVectorEntry(const std::string& wgtId, size_t weightEntry) {
                int orderedEntry = weightEntry - firstId_;
                int entry = -1;
                if (orderedEntry >= 0 && static_cast<size_t>(orderedEntry) < idsContained_.size())
                    if (idsContained_.at(orderedEntry).second == wgtId)
                        return orderedEntry;
                //auto it = std::find(
                return entry;
            }
            void addContainedID(std::string id, size_t weightEntry) {
                if (!indexInRange(weightEntry))
                    throw std::domain_error("This entry is out of the expected range");
                size_t orderedEntry = weightEntry - firstId_;
                idsContained_.insert(idsContained_.begin()+weightEntry, std::make_pair(orderedEntry, id));
            }
            void setWeightType(WeightType type) { weightType_ = type; }

            void setFirstEntry(size_t entryNum) { firstId_ = entryNum;}
            void setLastEntry(size_t entryNum) { lastId_ = entryNum;}

            bool indexInRange(size_t index) {
                return (index <= lastId_ && index >= firstId_);
            }

        private:
            std::string headerEntry_;
            std::string name_;
            WeightType weightType_;
            std::vector<WeightId> idsContained_;
            size_t firstId_;
            size_t lastId_;
	};
}

#endif // SimDataFormats_GeneratorProducts_WeightGroupInfo_h

