#include "SimDataFormats/GeneratorProducts/interface/PartonShowerWeightGroupInfo.h"
#include <exception>
#include <iostream>

namespace gen {
  void PartonShowerWeightGroupInfo::copy(const PartonShowerWeightGroupInfo& other) { 
    WeightGroupInfo::copy(other); 
    nameIsPythiaSyntax_ = other.nameIsPythiaSyntax_;
  }

  PartonShowerWeightGroupInfo* PartonShowerWeightGroupInfo::clone() const {
    return new PartonShowerWeightGroupInfo(*this);
  }

  int PartonShowerWeightGroupInfo::variationIndex(bool isISR, bool isUp, PSVarType variationType) const {
    return variationIndex(isISR, isUp, variationType, PSSplittingType::combined);
  }

  int PartonShowerWeightGroupInfo::variationIndex(bool isISR, bool isUp, PSVarType variationType, 
          PSSplittingType splittingType) const {
        std::string label = isISR ? "isr" : "fsr";

        if ((variationType == PSVarType::con || variationType == PSVarType::def || variationType == PSVarType::red) 
                && splittingType != PSSplittingType::combined)
            throw std::invalid_argument("VariationType must be muR or CNS if subprocess is specified");

        std::string variation;
        switch(variationType) {
            case PSVarType::con:
                variation = !nameIsPythiaSyntax_ ? "Con" : (isUp ? "murfac=4.0" : "murfac=0.25");
                break;
            case PSVarType::def:
                variation = !nameIsPythiaSyntax_ ? "Def" : (isUp ? "murfac=2.0" : "murfac=0.5");
                break;
            case PSVarType::red:
                variation = !nameIsPythiaSyntax_ ? "Red" : (isUp ? "murfac=1.414" : "murfac=0.707");
            case PSVarType::muR:
                variation = !nameIsPythiaSyntax_ ? "muR" : (isUp ? "murfac=2.0" : "murfac=0.5");
                break;
            case PSVarType::cNS:
                variation = !nameIsPythiaSyntax_ ? "cNS" : (isUp ? "cns=2.0" : "murfac=-2.0");
                break;
        }

        std::string splitting;
        switch(splittingType) {
            case PSSplittingType::g2gg:
                splitting = !nameIsPythiaSyntax_ ? "G2GG" : "g2gg";
                break;
            case PSSplittingType::g2qq:
                splitting = !nameIsPythiaSyntax_ ? "G2QQ" : "g2qq";
                break;
            case PSSplittingType::x2xg:
                splitting = !nameIsPythiaSyntax_ ? "X2XG" : "x2xg";
                break;
            default:
                break;
        }

        if (nameIsPythiaSyntax_) {
            std::string app = splittingType != PSSplittingType::combined ? splitting + ":" + variation : variation;
            label += ":" + app; 
        }
        else {
            if (splittingType != PSSplittingType::combined) {
                label += variation + "_" + splitting + "_" + variation + (isUp ? "_up" : "_dn");
            }
            else 
                label += variation + (isUp ? "Hi" : "Lo");
        }

        return weightIndexFromLabel(label);

    }

}  // namespace gen
