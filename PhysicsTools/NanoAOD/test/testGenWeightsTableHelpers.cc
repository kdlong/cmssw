#include "catch2/catch_all.hpp"
#include "PhysicsTools/NanoAOD/interface/GenWeightCounters.h"
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

TEST_CASE("Weight counters reject inconsistent dimensions before indexing", "[WeightCounters]") {
  genCounter::Counter counter;
  counter.incLHE(2., {1., 2.}, "PDF");
  SECTION("Consistent weighted sums are preserved") {
    counter.incLHE(-1., {3., 4.}, "PDF");
    REQUIRE(counter.weightSumMap_.at("PDF") == std::vector<long double>{-1., 0.});
  }
  SECTION("Growing and shrinking vectors both fail without changing the sum") {
    for (const auto& input : {std::vector<double>{1.}, std::vector<double>{1., 2., 3.}}) {
      REQUIRE_THROWS_WITH(counter.incLHE(1., input, "PDF"),
                          Catch::Matchers::ContainsSubstring("Weight counter PDF changed size"));
      REQUIRE(counter.weightSumMap_.at("PDF") == std::vector<long double>{2., 4.});
    }
  }
  SECTION("Empty vectors do not contribute and do not fix the initial dimension") {
    counter.incLHE(1., {}, "PDF");
    counter.incLHE(1., {}, "new");
    counter.incLHE(1., {3.}, "new");
    REQUIRE(counter.weightSumMap_.at("PDF") == std::vector<long double>{2., 4.});
    REQUIRE(counter.weightSumMap_.at("new") == std::vector<long double>{3.});
  }
  SECTION("Stream merges reject either size mismatch") {
    for (const auto& input : {std::vector<double>{1.}, std::vector<double>{1., 2., 3.}}) {
      genCounter::Counter incoming;
      incoming.incLHE(1., input, "PDF");
      REQUIRE_THROWS_WITH(counter.mergeSumMap(incoming),
                          Catch::Matchers::ContainsSubstring("Cannot merge weight counter PDF"));
      REQUIRE(counter.weightSumMap_.at("PDF") == std::vector<long double>{2., 4.});
    }
  }
  SECTION("Stream merges include new names and use independent dimensions per name") {
    genCounter::Counter incoming;
    incoming.incLHE(1., {3., 4.}, "PDF");
    incoming.incLHE(1., {5.}, "new");
    counter.mergeSumMap(incoming);
    REQUIRE(counter.weightSumMap_.at("PDF") == std::vector<long double>{5., 8.});
    REQUIRE(counter.weightSumMap_.at("new") == std::vector<long double>{5.});
  }
  SECTION("Empty merging operands are neutral") {
    std::vector<long double> sum{2., 4.};
    genCounter::mergeSumVectors(sum, {});
    REQUIRE(sum == std::vector<long double>{2., 4.});
    std::vector<long double> empty;
    genCounter::mergeSumVectors(empty, sum);
    REQUIRE(empty == sum);
  }
  SECTION("A new run clears actual counters, allowing a new vector dimension") {
    genCounter::CounterMap map;
    map.setLabel("model");
    map.get()->incGenOnly(2.);
    map.get()->incLHE(2., {1., 2.}, "PDF");
    map.clear();
    REQUIRE(map.get()->num_ == 0);
    REQUIRE(map.get()->sumw_ == 0);
    REQUIRE(map.get()->weightSumMap_.empty());
    REQUIRE_NOTHROW(map.get()->incLHE(1., {1., 2., 3.}, "PDF"));
    REQUIRE(map.get()->weightSumMap_.at("PDF").size() == 3);
  }
}
