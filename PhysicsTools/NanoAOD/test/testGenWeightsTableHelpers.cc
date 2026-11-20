#include "catch2/catch_all.hpp"
#include "PhysicsTools/NanoAOD/interface/GenWeightGroupSelection.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"
#include <limits>

namespace {
  void addPdf(GenWeightInfoProduct& info, int id) {
    auto group = std::make_unique<gen::PdfWeightGroupInfo>("PDF_variation");
    group->setParentLhapdfInfo(id);
    info.addWeightGroupInfo(std::move(group));
  }
}  // namespace

TEST_CASE("PDF selection supports priority, all matches, and an explicit fallback", "[PDFSelection]") {
  GenWeightInfoProduct info;
  for (int id : {325300, 306000, 91400, 261000})
    addPdf(info, id);
  const std::vector<int> ids{91400, 306000, 260000};
  const size_t unlimited = std::numeric_limits<size_t>::max();
  SECTION("One group uses preference order rather than header order") {
    const auto selected = nanoaod::selectPdfWeightGroups(info, ids, 1, true);
    REQUIRE(selected.size() == 1);
    REQUIRE(selected[0].index == 2);
  }
  SECTION("Unlimited groups keeps all matches in preference order") {
    const auto selected = nanoaod::selectPdfWeightGroups(info, ids, unlimited, true);
    REQUIRE(selected.size() == 2);
    REQUIRE(selected[0].index == 2);
    REQUIRE(selected[1].index == 1);
  }
  SECTION("Fallback is only used when no IDs match, and selects just the first group") {
    for (size_t limit : {size_t(1), unlimited}) {
      const auto selected = nanoaod::selectPdfWeightGroups(info, {260000}, limit, true);
      REQUIRE(selected.size() == 1);
      REQUIRE(selected[0].index == 0);
      REQUIRE(nanoaod::selectPdfWeightGroups(info, {260000}, limit, false).empty());
    }
  }
  SECTION("Sherpa replicas remain available when their ID is not listed") {
    GenWeightInfoProduct sherpa;
    addPdf(sherpa, 261000);
    const auto selected = nanoaod::selectPdfWeightGroups(sherpa, ids, 1, true);
    REQUIRE(selected.size() == 1);
    REQUIRE(selected[0].index == 0);
  }
  SECTION("An empty preference list preserves header order and group limits") {
    REQUIRE(nanoaod::selectPdfWeightGroups(info, {}, 1, true)[0].index == 0);
    const auto selected = nanoaod::selectPdfWeightGroups(info, {}, unlimited, true);
    REQUIRE(selected.size() == 4);
    for (size_t i = 0; i < selected.size(); ++i)
      REQUIRE(selected[i].index == i);
  }
  SECTION("No PDF groups is valid") {
    GenWeightInfoProduct empty;
    REQUIRE(nanoaod::selectPdfWeightGroups(empty, ids, 1, true).empty());
  }
}
