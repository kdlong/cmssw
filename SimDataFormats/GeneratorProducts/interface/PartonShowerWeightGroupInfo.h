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
    PartonShowerWeightGroupInfo(std::string header, std::string name);
    PartonShowerWeightGroupInfo(std::string header) : PartonShowerWeightGroupInfo(header, header) {}
    PartonShowerWeightGroupInfo() : PartonShowerWeightGroupInfo("") {}
    PartonShowerWeightGroupInfo(const PartonShowerWeightGroupInfo &other) { copy(other); }
    virtual ~PartonShowerWeightGroupInfo() override {}
    void copy(const PartonShowerWeightGroupInfo &other);
    virtual PartonShowerWeightGroupInfo *clone() const override;
    void setNameIsPythiaSyntax(bool isPythiaSyntax) { nameIsPythiaSyntax_ = isPythiaSyntax; }
    bool nameIsPythiaSyntax(bool isPythiaSyntax) const { return nameIsPythiaSyntax_; }
    int variationIndex(bool isISR, bool isUp, PSVarType variationType, PSSplittingType splittingType) const;
    std::string variationName(bool isISR, bool isUp, PSVarType variationType, PSSplittingType splittingType) const;
    int variationIndex(bool isISR, bool isUp, PSVarType variationType) const;

  private:
    bool nameIsPythiaSyntax_ = false;
    typedef PSVarType varType;
    typedef PSSplittingType sptType;
    // Order determined by !isUp*2 + !isISR -> isrHi, fsrHi, isrLo, fsrLo
    const std::unordered_map<PSPair, std::vector<int>, PSPairHash> oldPythia_order = {
        {{varType::def, sptType::combined}, {6, 7, 8, 9}},
        {{varType::red, sptType::combined}, {2, 3, 4, 5}},
        {{varType::con, sptType::combined}, {10, 11, 12, 13}},
        {{varType::muR, sptType::g2gg}, {31, 15, 30, 14}},
        {{varType::muR, sptType::g2qq}, {33, 17, 32, 16}},
        {{varType::muR, sptType::q2qg}, {35, 19, 34, 18}},
        {{varType::muR, sptType::x2xg}, {37, 21, 36, 20}},
        {{varType::cNS, sptType::g2gg}, {39, 23, 38, 22}},
        {{varType::cNS, sptType::g2qq}, {41, 25, 40, 24}},
        {{varType::cNS, sptType::q2qg}, {43, 27, 42, 26}},
        {{varType::cNS, sptType::x2xg}, {45, 29, 44, 28}},
    };

    const std::unordered_map<PSPair, std::vector<int>, PSPairHash> newPythia_order = {
        {{varType::def, sptType::combined}, {27, 5, 26, 4}},
        {{varType::red, sptType::combined}, {25, 3, 2, 24}},
        {{varType::con, sptType::combined}, {29, 7, 28, 6}},
        {{varType::muR, sptType::g2gg}, {31, 9, 30, 8}},
        {{varType::muR, sptType::g2qq}, {33, 11, 32, 10}},
        {{varType::muR, sptType::q2qg}, {35, 13, 34, 12}},
        {{varType::muR, sptType::x2xg}, {37, 15, 36, 14}},
        {{varType::cNS, sptType::g2gg}, {39, 17, 38, 16}},
        {{varType::cNS, sptType::g2qq}, {41, 19, 40, 18}},
        {{varType::cNS, sptType::q2qg}, {43, 21, 42, 20}},
        {{varType::cNS, sptType::x2xg}, {45, 23, 44, 22}},
    };
  };
}  // namespace gen

#endif
