#ifndef SimDataFormats_GeneratorProducts_PartonShowerWeightGroupInfo_h
#define SimDataFormats_GeneratorProducts_PartonShowerWeightGroupInfo_h

#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"

namespace gen {
  enum class PSVarType { muR, cNS, con, def, red, alphaS, LAST };
  enum class PSSplittingType { combined, g2gg, x2xg, g2qq, q2qg };

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
    static void setGuessPSWeightIdx(bool guessPSWeightIdx) { guessPSWeightIdx_ = guessPSWeightIdx_; }
    int psWeightIdxGuess(const std::string &varName) const;

  private:
    bool nameIsPythiaSyntax_ = false;
    static inline bool guessPSWeightIdx_ = false;

    const std::vector<std::string> expectedPythiaSyntax = {
        "fsr:murfac=0.707",    "fsr:murfac=1.414",    "fsr:murfac=0.5",      "fsr:murfac=2.0",
        "fsr:murfac=0.25",     "fsr:murfac=4.0",      "fsr:g2gg:murfac=0.5", "fsr:g2gg:murfac=2.0",
        "fsr:g2qq:murfac=0.5", "fsr:g2qq:murfac=2.0", "fsr:q2qg:murfac=0.5", "fsr:q2qg:murfac=2.0",
        "fsr:x2xg:murfac=0.5", "fsr:x2xg:murfac=2.0", "fsr:g2gg:cns=-2.0",   "fsr:g2gg:cns=2.0",
        "fsr:g2qq:cns=-2.0",   "fsr:g2qq:cns=2.0",    "fsr:q2qg:cns=-2.0",   "fsr:q2qg:cns=2.0",
        "fsr:x2xg:cns=-2.0",   "fsr:x2xg:cns=2.0",    "isr:murfac=0.707",    "isr:murfac=1.414",
        "isr:murfac=0.5",      "isr:murfac=2.0",      "isr:murfac=0.25",     "isr:murfac=4.0",
        "isr:g2gg:murfac=0.5", "isr:g2gg:murfac=2.0", "isr:g2qq:murfac=0.5", "isr:g2qq:murfac=2.0",
        "isr:q2qg:murfac=0.5", "isr:q2qg:murfac=2.0", "isr:x2xg:murfac=0.5", "isr:x2xg:murfac=2.0",
        "isr:g2gg:cns=-2.0",   "isr:g2gg:cns=2.0",    "isr:g2qq:cns=-2.0",   "isr:g2qq:cns=2.0",
        "isr:q2qg:cns=-2.0",   "isr:q2qg:cns=2.0",    "isr:x2xg:cns=-2.0",   "isr:x2xg:cns=2.0",
    };
    const std::vector<std::string> expectedOrder = {
        "isrRedHi",        "fsrRedHi",        "isrRedLo",        "fsrRedLo",        "isrDefHi",
        "fsrDefHi",        "isrDefLo",        "fsrDefLo",        "isrConHi",        "fsrConHi",
        "isrConLo",        "fsrConLo",        "fsr_G2GG_muR_dn", "fsr_G2GG_muR_up", "fsr_G2QQ_muR_dn",
        "fsr_G2QQ_muR_up", "fsr_Q2QG_muR_dn", "fsr_Q2QG_muR_up", "fsr_X2XG_muR_dn", "fsr_X2XG_muR_up",
        "fsr_G2GG_cNS_dn", "fsr_G2GG_cNS_up", "fsr_G2QQ_cNS_dn", "fsr_G2QQ_cNS_up", "fsr_Q2QG_cNS_dn",
        "fsr_Q2QG_cNS_up", "fsr_X2XG_cNS_dn", "fsr_X2XG_cNS_up", "isr_G2GG_muR_dn", "isr_G2GG_muR_up",
        "isr_G2QQ_muR_dn", "isr_G2QQ_muR_up", "isr_Q2QG_muR_dn", "isr_Q2QG_muR_up", "isr_X2XG_muR_dn",
        "isr_X2XG_muR_up", "isr_G2GG_cNS_dn", "isr_G2GG_cNS_up", "isr_G2QQ_cNS_dn", "isr_G2QQ_cNS_up",
        "isr_Q2QG_cNS_dn", "isr_Q2QG_cNS_up", "isr_X2XG_cNS_dn", "isr_X2XG_cNS_up",
    };
  };
}  // namespace gen

#endif
