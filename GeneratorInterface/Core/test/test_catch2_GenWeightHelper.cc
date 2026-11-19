#include "catch2/catch_all.hpp"

#include "GeneratorInterface/Core/interface/GenWeightHelper.h"

#include <string>
#include <vector>

namespace {
  // Weight names written by Pythia to the GenLumiInfoHeader, with the given separator
  // (':' up to Pythia 8.30x, '.' from 8.310)
  std::vector<std::string> pythiaNames(const std::string& sep) {
    std::vector<std::string> names = {"nominal", "Baseline"};
    for (const std::string sr : {"fsr", "isr"}) {
      for (const std::string var :
           {"murfac=0.707", "murfac=1.414", "murfac=0.5", "murfac=2.0", "murfac=0.25", "murfac=4.0"})
        names.push_back(sr + sep + var);
      for (const std::string var : {"murfac=0.5", "murfac=2.0"})
        for (const std::string split : {"g2gg", "g2qq", "q2qg", "x2xg"})
          names.push_back(sr + sep + split + sep + var);
      for (const std::string var : {"cns=-2.0", "cns=2.0"})
        for (const std::string split : {"g2gg", "g2qq", "q2qg", "x2xg"})
          names.push_back(sr + sep + split + sep + var);
    }
    return names;
  }

  void checkPythiaPS(const std::vector<std::string>& names) {
    gen::GenWeightHelper helper;
    auto groups = helper.parseWeightGroupsFromNames(names, false);
    REQUIRE(groups.size() == 1);
    REQUIRE(groups[0]->weightType() == gen::WeightType::kPartonShowerWeights);
    REQUIRE(groups[0]->nIdsContained() == names.size());
    REQUIRE(groups[0]->isWellFormed());
    const auto& ps = *static_cast<gen::PartonShowerWeightGroupInfo*>(groups[0].get());
    REQUIRE(ps.nameIsPythiaSyntax());
    auto nameAt = [&names](int idx) { return idx < 0 ? std::string("missing") : names.at(idx); };
    REQUIRE(nameAt(ps.weightIndexFromLabel("Baseline")) == "Baseline");
    const char sep = names[2][3];
    REQUIRE(nameAt(ps.isrCombinedUpIndex(gen::PSVarType::def)) == std::string("isr") + sep + "murfac=2.0");
    REQUIRE(nameAt(ps.isrCombinedDownIndex(gen::PSVarType::def)) == std::string("isr") + sep + "murfac=0.5");
    REQUIRE(nameAt(ps.fsrCombinedUpIndex(gen::PSVarType::def)) == std::string("fsr") + sep + "murfac=2.0");
    REQUIRE(nameAt(ps.fsrCombinedDownIndex(gen::PSVarType::def)) == std::string("fsr") + sep + "murfac=0.5");
  }
}  // namespace

TEST_CASE("GenWeightHelper parsing of generator weight names", "[GenWeightHelper]") {
  SECTION("Pythia PS weights, ':' separator") { checkPythiaPS(pythiaNames(":")); }

  SECTION("Pythia PS weights, '.' separator (Pythia >= 8.310)") { checkPythiaPS(pythiaNames(".")); }

  SECTION("Sherpa weights") {
    std::vector<std::string> names = {"Weight", "MEWeight", "WeightNormalisation", "NTrials"};
    for (const std::string mur : {"0.5", "1", "2"})
      for (const std::string muf : {"0.5", "1", "2"})
        if (!(mur == "1" && muf == "1"))
          names.push_back("MUR" + mur + "_MUF" + muf + "_PDF303200");
    names.push_back("MUR1_MUF1_PDF303200");
    for (int i = 1; i <= 100; i++)
      names.push_back("MUR1_MUF1_PDF" + std::to_string(303200 + i));
    names.push_back("MUR1_MUF1_PDF13000");
    names.push_back("MUR2_MUF2_PDF13000");

    gen::GenWeightHelper helper;
    auto groups = helper.parseWeightGroupsFromNames(names, false);
    REQUIRE(groups.size() == 4);

    REQUIRE(groups[0]->weightType() == gen::WeightType::kUnknownWeights);
    REQUIRE(groups[0]->nIdsContained() == 4);

    REQUIRE(groups[1]->weightType() == gen::WeightType::kScaleWeights);
    const auto& scale = *static_cast<gen::ScaleWeightGroupInfo*>(groups[1].get());
    REQUIRE(scale.nIdsContained() == 9);
    REQUIRE(scale.isWellFormed());
    REQUIRE(scale.lhaid() == 303200);
    REQUIRE(scale.centralIndex() == 8);
    REQUIRE(scale.muR2muF1Index() == 6);

    REQUIRE(groups[2]->weightType() == gen::WeightType::kPdfWeights);
    const auto& pdf = *static_cast<gen::PdfWeightGroupInfo*>(groups[2].get());
    REQUIRE(pdf.nIdsContained() == 100);
    REQUIRE(pdf.parentLhapdfId() == 303200);
    // The first weight of the group is the 13th in the event (after 4 Sherpa and 9 scale weights)
    REQUIRE(pdf.weightMetaInfo(0).globalIndex == 13);

    // Scale variations with an alternative PDF
    REQUIRE(groups[3]->weightType() == gen::WeightType::kScaleWeights);
    REQUIRE(groups[3]->nIdsContained() == 2);
  }

  SECTION("Unrecognized weight names") {
    gen::GenWeightHelper helper;
    auto groups = helper.parseWeightGroupsFromNames({"central", "foo", "bar"}, true);
    REQUIRE(groups.size() == 2);
    REQUIRE(groups[0]->weightType() == gen::WeightType::kUnknownWeights);
    REQUIRE(groups[0]->nIdsContained() == 3);
    REQUIRE(groups[1]->name() == "unassociated");
  }
}

TEST_CASE("PDF uncertainty types and descriptions come from LHAPDF", "[GenWeightHelper][PDF]") {
  struct PdfCase {
    int lhaid;
    const char* errorType;
    gen::PdfUncertaintyType uncertaintyType;
    const char* descriptionPrefix;
  };
  for (const auto& test : {PdfCase{325300, "symmhessian+as", gen::kHessianUnc, "Hessian "},
                           PdfCase{316200, "replicas", gen::kMonteCarloUnc, "Monte Carlo "},
                           PdfCase{14000, "hessian", gen::kHessianUnc, "Hessian "}}) {
    INFO("LHAPDF ID " << test.lhaid);
    const auto name = LHAPDF::lookupPDF(test.lhaid).first;
    REQUIRE(LHAPDF::PDFSet(name).errorType() == test.errorType);

    gen::PdfWeightGroupInfo directGroup(name);
    directGroup.setParentLhapdfInfo(test.lhaid);
    REQUIRE(directGroup.uncertaintyType() == test.uncertaintyType);

    gen::GenWeightHelper helper;
    const auto groups = helper.parseWeightGroupsFromNames(
        {"nominal", "LHE, id = 1001, group = " + name + ", PDF=" + std::to_string(test.lhaid) + " MemberID=0"}, false);
    REQUIRE(groups.size() == 2);
    REQUIRE(groups[1]->weightType() == gen::WeightType::kPdfWeights);
    const auto& pdf = *static_cast<const gen::PdfWeightGroupInfo*>(groups[1].get());
    REQUIRE(pdf.parentLhapdfId() == test.lhaid);
    REQUIRE(pdf.uncertaintyType() == test.uncertaintyType);
    REQUIRE(pdf.description() == std::string(test.descriptionPrefix) + "Uncertainty sets for LHAPDF set " + name +
                                     " with LHAID = " + std::to_string(test.lhaid) + "; ");
  }

  SECTION("Missing error type remains unknown, including after changing parent set") {
    gen::PdfWeightGroupInfo group;
    group.setParentLhapdfInfo(325300);
    REQUIRE(group.uncertaintyType() == gen::kHessianUnc);
    // cteq6l1 does not define ErrorType in its LHAPDF metadata.
    REQUIRE(LHAPDF::PDFSet("cteq6l1").errorType() == "unknown");
    group.setParentLhapdfInfo(10042);
    REQUIRE(group.uncertaintyType() == gen::kUnknownUnc);
  }
}
