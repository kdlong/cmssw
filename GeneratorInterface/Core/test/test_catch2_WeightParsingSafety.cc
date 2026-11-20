#include "catch2/catch_all.hpp"
#include "GeneratorInterface/Core/interface/GenWeightHelper.h"
#include "GeneratorInterface/Core/interface/LHEWeightHelper.h"

TEST_CASE("Flattened LHE labels without group attributes have valid destinations", "[GenWeightHelper][MissingGroup]") {
  const auto attribute = GENERATE(std::string(""), std::string("group = , "));
  gen::GenWeightHelper helper;
  SECTION("The first weights lack group attributes") {
    const auto groups = helper.parseWeightGroupsFromNames(
        {"LHE, id = 10, " + attribute + "unrecognizedA", "LHE, id = 20, " + attribute + "unrecognizedB"}, false);
    REQUIRE(groups.size() == 1);
    REQUIRE(groups[0]->weightType() == gen::WeightType::kUnknownWeights);
    REQUIRE(groups[0]->nIdsContained() == 2);
    REQUIRE_FALSE(groups[0]->name().empty());
    REQUIRE(groups[0]->weightMetaInfo(0).globalIndex == 0);
    REQUIRE(groups[0]->weightMetaInfo(1).globalIndex == 1);
  }
  SECTION("A nameless group follows shower and named scale weights") {
    const auto groups = helper.parseWeightGroupsFromNames({"nominal",
                                                           "LHE, id = 10, group = Central scale variation, MUR=1 MUF=1",
                                                           "LHE, id = 20, " + attribute + "unrecognizedA",
                                                           "LHE, id = 30, " + attribute + "unrecognizedB"},
                                                          false);
    REQUIRE(groups.size() == 3);
    REQUIRE(groups[0]->weightType() == gen::WeightType::kPartonShowerWeights);
    REQUIRE(groups[1]->weightType() == gen::WeightType::kScaleWeights);
    REQUIRE(groups[2]->weightType() == gen::WeightType::kUnknownWeights);
    REQUIRE(groups[2]->nIdsContained() == 2);
    REQUIRE(groups[2]->weightMetaInfo(0).globalIndex == 2);
  }
  SECTION("Existing orphan PDF recognition is retained") {
    const auto groups = helper.parseWeightGroupsFromNames({"LHE, id = 10, " + attribute + "PDF=325300 MemberID=0",
                                                           "LHE, id = 20, " + attribute + "PDF=325300 MemberID=1"},
                                                          false);
    REQUIRE(groups.size() == 1);
    REQUIRE(groups[0]->weightType() == gen::WeightType::kPdfWeights);
    auto& pdf = *static_cast<gen::PdfWeightGroupInfo*>(groups[0].get());
    REQUIRE(pdf.parentLhapdfId() == 325300);
    REQUIRE(pdf.lhaIds() == std::vector<int>{325300, 325301});
  }
}

TEST_CASE("XML weight IDs are checked before constructing strings", "[LHEWeightHelper][MissingID]") {
  gen::LHEWeightHelper helper;
  helper.setFailIfInvalidXML(GENERATE(false, true));
  SECTION("Missing, empty, and whitespace-only IDs fail with a useful error") {
    for (const std::string attribute : {"", " id=''", " id='   '"}) {
      const std::vector<std::string> header{"<weightgroup name='PDF_variation'>\n",
                                            "<weight" + attribute + ">PDF=325300 MemberID=0</weight>\n",
                                            "</weightgroup>\n"};
      REQUIRE_THROWS_WITH(helper.parseWeights(header, false),
                          Catch::Matchers::ContainsSubstring("Missing or empty id attribute"));
    }
  }
  SECTION("Valid numerical and named IDs remain unchanged") {
    const auto groups = helper.parseWeights({"<weightgroup name='PDF_variation'>\n",
                                             "<weight id='central'>PDF=325300 MemberID=0</weight>\n",
                                             "<weight id='2001'>PDF=325300 MemberID=1</weight>\n",
                                             "</weightgroup>\n"},
                                            false);
    REQUIRE(groups.size() == 1);
    REQUIRE(groups[0]->weightMetaInfo(0).id == "central");
    REQUIRE(groups[0]->weightMetaInfo(1).id == "2001");
  }
}
