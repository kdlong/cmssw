#include "catch2/catch_all.hpp"
#include "SimDataFormats/GeneratorProducts/interface/UnknownWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/ScaleWeightGroupInfo.h"

TEST_CASE("Sparse weight insertion keeps local indices and label caches consistent", "[WeightIndices]") {
  gen::UnknownWeightGroupInfo group("test");
  for (int index : {20, 40})
    group.addContainedId(index, "id" + std::to_string(index), "label" + std::to_string(index));
  group.cacheWeightIndicesByLabel();
  for (int index : {5, 30, 50})
    group.addContainedId(index, "id" + std::to_string(index), "label" + std::to_string(index));
  const std::vector<int> expected{5, 20, 30, 40, 50};
  REQUIRE(group.firstId() == 5);
  REQUIRE(group.lastId() == 50);
  REQUIRE(group.nIdsContained() == expected.size());
  for (size_t i = 0; i < expected.size(); ++i) {
    REQUIRE(group.weightMetaInfo(i).globalIndex == static_cast<size_t>(expected[i]));
    REQUIRE(group.weightMetaInfo(i).localIndex == i);
    REQUIRE(group.weightMetaInfoByGlobalIndex(expected[i]).localIndex == i);
    REQUIRE(group.weightIndexFromLabel("label" + std::to_string(expected[i])) == static_cast<int>(i));
    REQUIRE(group.containsWeight(expected[i]));
  }
  REQUIRE_FALSE(group.containsWeight(10));
  REQUIRE_THROWS(group.addContainedId(20, "duplicate", "duplicate"));
  REQUIRE_THROWS(group.addContainedId(-1, "negative", "negative"));
  REQUIRE(group.nIdsContained() == expected.size());
}

TEST_CASE("Scale caches shift by one inserted entry, independently of global gaps", "[WeightIndices][Scale]") {
  const int centralGlobalIndex = GENERATE(5, 145, 500);
  INFO("central global index " << centralGlobalIndex);
  gen::ScaleWeightGroupInfo group("scale");
  const std::vector<std::pair<float, float>> factors = {
      {0.5, 0.5}, {0.5, 1}, {0.5, 2}, {1, 0.5}, {1, 1}, {1, 2}, {2, 0.5}, {2, 1}, {2, 2}};
  for (size_t i = 0; i < factors.size(); ++i) {
    if (i == 4)
      continue;
    const int index = 100 + 10 * i;
    group.addContainedId(
        index, std::to_string(index), "default" + std::to_string(i), factors[i].first, factors[i].second);
  }
  REQUIRE_FALSE(group.isWellFormed());
  REQUIRE(group.centralIndex() == -1);
  for (size_t i = 0; i < factors.size(); ++i) {
    const int index = 300 + 10 * i;
    group.WeightGroupInfo::addContainedId(index, std::to_string(index), "HT" + std::to_string(i));
    group.setDyn(index, std::to_string(index), factors[i].first, factors[i].second, 0, "HT");
  }
  // Missing cells of a partially populated dynamical grid must remain invalid.
  group.setDyn(300, "300", 0.5, 0.5, 2, "partial");
  group.cacheWeightIndicesByLabel();
  group.addContainedId(centralGlobalIndex, "central", "central", 1., 1.);
  REQUIRE(group.isWellFormed());
  REQUIRE(group.containsCentralWeight());
  REQUIRE(group.nIdsContained() == 18);
  for (size_t i = 0; i < factors.size(); ++i) {
    const int index = i == 4 ? centralGlobalIndex : 100 + 10 * i;
    const auto local = group.scaleIndex(factors[i].first, factors[i].second);
    REQUIRE(group.weightMetaInfo(local).globalIndex == static_cast<size_t>(index));
    REQUIRE(group.weightMetaInfo(local).localIndex == local);
    const auto dyn = group.scaleIndex(factors[i].first, factors[i].second, "HT");
    REQUIRE(group.weightMetaInfo(dyn).globalIndex == 300 + 10 * i);
    if (i != 0)
      REQUIRE(group.scaleIndex(factors[i].first, factors[i].second, "partial") == size_t(-1));
  }
  REQUIRE(group.weightIndexFromLabel("central") == group.centralIndex());
}
