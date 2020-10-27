#ifndef SimDataFormats_GeneratorProducts_PartonShowerWeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_PartonShowerWeightGroupInfo_h

#include <map>

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"

namespace gen {
  enum class PSVarType { muR, cNS, con, def, red, alphaS, LAST };
  enum class PSSplittingType { combined, g2gg, x2xg, g2qq, q2qg };
  typedef std::pair<PSVarType, PSSplittingType> PSPair;

  struct PSPairHash {
    std::size_t operator()(const PSPair &pair) const {
      return static_cast<std::size_t>(pair.first) * static_cast<std::size_t>(PSVarType::LAST) +
             static_cast<std::size_t>(pair.second);
    }
  };

  class PartonShowerWeightGroupInfo : public WeightGroupInfo {
  public:
    PartonShowerWeightGroupInfo() : PartonShowerWeightGroupInfo("") {}
    PartonShowerWeightGroupInfo(std::string header, std::string name) : WeightGroupInfo(header, name) {
      weightType_ = WeightType::kPartonShowerWeights;
      nameIsPythiaSyntax_ = false;
    }
    PartonShowerWeightGroupInfo(std::string header) : PartonShowerWeightGroupInfo(header, header) {}
    PartonShowerWeightGroupInfo() : PartonShowerWeightGroupInfo("") {}
    PartonShowerWeightGroupInfo(const PartonShowerWeightGroupInfo &other) { copy(other); }
    ~PartonShowerWeightGroupInfo() override {}
    void copy(const PartonShowerWeightGroupInfo &other);
    PartonShowerWeightGroupInfo *clone() const override;
    void setNameIsPythiaSyntax(bool isPythiaSyntax) { nameIsPythiaSyntax_ = isPythiaSyntax; }
    bool nameIsPythiaSyntax() const { return nameIsPythiaSyntax_; }
    int variationIndex(bool isISR, bool isUp, PSVarType variationType, PSSplittingType splittingType) const;
    std::string variationName(bool isISR, bool isUp, PSVarType variationType, PSSplittingType splittingType) const;
    int variationIndex(bool isISR, bool isUp, PSVarType variationType) const;

  private:
    bool nameIsPythiaSyntax_ = false;
  };
}  // namespace gen

#endif
