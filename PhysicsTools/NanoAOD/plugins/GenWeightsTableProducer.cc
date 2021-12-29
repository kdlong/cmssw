#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/NanoAOD/interface/MergeableCounterTable.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenWeightInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenLumiInfoHeader.h"
#include "SimDataFormats/GeneratorProducts/interface/GenWeightProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/WeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/ScaleWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/PdfWeightGroupInfo.h"
#include "SimDataFormats/GeneratorProducts/interface/PartonShowerWeightGroupInfo.h"
#include "FWCore/Utilities/interface/transform.h"
#include "PhysicsTools/NanoAOD/interface/GenWeightCounters.h"
#include <iostream>

namespace {
  typedef std::vector<gen::SharedWeightGroupData> WeightGroupDataContainer;
  typedef std::array<std::vector<gen::SharedWeightGroupData>, 2> WeightGroupsToStore;
}  // namespace
using CounterMap = genCounter::CounterMap;
using Counter = genCounter::Counter;

class GenWeightsTableProducer : public edm::global::EDProducer<edm::LuminosityBlockCache<WeightGroupsToStore>,
                                                               edm::StreamCache<CounterMap>,
                                                               edm::RunSummaryCache<CounterMap>,
                                                               edm::EndRunProducer> {
public:
  GenWeightsTableProducer(edm::ParameterSet const& params);

  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override;
  void addWeightGroupToTable(
			     std::unique_ptr<std::vector<nanoaod::FlatTable>>& weightTablevec,
                             const WeightGroupDataContainer& weightInfos,
                             WeightsContainer& allWeights) const;
  // Need to either pass the handle or a pointer to avoid a copy and conversion to the base class
  WeightGroupDataContainer weightDataPerType(edm::Handle<GenWeightInfoProduct>& weightsInfoHandle,
                                             gen::WeightType weightType,
                                             int& maxStore) const;

  WeightGroupsToStore groupsToStore(bool foundLheWeights, 
                                    edm::Handle<GenWeightInfoProduct>& genWeightInfoHandle,
                                    edm::Handle<GenWeightInfoProduct>& lheWeightInfoHandle) const;

  std::pair<std::string, std::vector<double>> orderedScaleWeights(const std::vector<double>& scaleWeights,
                                          const gen::ScaleWeightGroupInfo& scaleGroup) const;

  std::pair<std::string, std::vector<double>> preferredPSweights(const std::vector<double>& psWeights,
                                         const gen::PartonShowerWeightGroupInfo& pswV) const;

  //Lumiblock
  std::shared_ptr<WeightGroupsToStore> globalBeginLuminosityBlock(edm::LuminosityBlock const& iLumi,
                                                                  edm::EventSetup const&) const override {
    // Set equal to the max number of groups
    // subtrack 1 for each weight group you find
    bool foundLheWeights = false;
    edm::Handle<GenWeightInfoProduct> lheWeightInfoHandle;
    for (auto& token : lheWeightInfoTokens_) {
      iLumi.getByToken(token, lheWeightInfoHandle);
      if (lheWeightInfoHandle.isValid()) {
        foundLheWeights = true;
        break;
      }
    }
    edm::Handle<GenWeightInfoProduct> genWeightInfoHandle;
    iLumi.getByToken(genWeightInfoToken_, genWeightInfoHandle);

    auto tostore = groupsToStore(foundLheWeights, genWeightInfoHandle, lheWeightInfoHandle);
    return std::make_shared<WeightGroupsToStore>(tostore);
  }

  void globalEndLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) const override {}
  // create an empty counter
  std::unique_ptr<CounterMap> beginStream(edm::StreamID) const override { return std::make_unique<CounterMap>(); }
  // inizialize to zero at begin run
  void streamBeginRun(edm::StreamID id, edm::Run const&, edm::EventSetup const&) const override {
    streamCache(id)->clear();
  }

  void streamBeginLuminosityBlock(edm::StreamID id,
                                  edm::LuminosityBlock const& lumiBlock,
                                  edm::EventSetup const& eventSetup) const override {
    auto counterMap = streamCache(id);
    edm::Handle<GenLumiInfoHeader> genLumiInfoHead;
    lumiBlock.getByToken(genLumiInfoHeadTag_, genLumiInfoHead);
    if (!genLumiInfoHead.isValid())
      edm::LogWarning("LHETablesProducer")
          << "No GenLumiInfoHeader product found, will not fill generator model string.\n";
    counterMap->setLabel(genLumiInfoHead.isValid() ? genLumiInfoHead->configDescription() : "");
    std::string label = genLumiInfoHead.isValid() ? counterMap->getLabel() : "NULL";
  }
  // create an empty counter
  std::shared_ptr<CounterMap> globalBeginRunSummary(edm::Run const&, edm::EventSetup const&) const override {
    return std::make_shared<CounterMap>();
  }
  // add this stream to the summary
  void streamEndRunSummary(edm::StreamID id,
                           edm::Run const&,
                           edm::EventSetup const&,
                           CounterMap* runCounterMap) const override;
  // nothing to do per se
  void globalEndRunSummary(edm::Run const&, edm::EventSetup const&, CounterMap* runCounterMap) const override {}
  // write the total to the run
  void globalEndRunProduce(edm::Run& iRun, edm::EventSetup const& es, CounterMap const* runCounterMap) const override;
  // nothing to do here
  //void globalEndRun(edm::Run const& iRun, edm::EventSetup const& es, CounterMap* runCounterMap) const override {}

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

protected:
  const std::vector<edm::EDGetTokenT<GenWeightProduct>> lheWeightTokens_;
  const std::vector<edm::EDGetTokenT<GenWeightInfoProduct>> lheWeightInfoTokens_;
  const edm::EDGetTokenT<GenWeightProduct> genWeightToken_;
  const edm::EDGetTokenT<GenWeightInfoProduct> genWeightInfoToken_;
  const edm::EDGetTokenT<GenEventInfoProduct> genEventInfoToken_;
  const edm::EDGetTokenT<GenLumiInfoHeader> genLumiInfoHeadTag_;
  const std::vector<gen::WeightType> weightgroups_;
  const std::vector<int> maxGroupsPerType_;
  const std::vector<int> pdfIds_;
  const std::unordered_map<gen::WeightType, std::string> weightTypeNames_ = {
      {gen::WeightType::kScaleWeights, "LHEScaleWeight"},
      {gen::WeightType::kPdfWeights, "LHEPdfWeight"},
      {gen::WeightType::kMEParamWeights, "MEParamWeight"},
      {gen::WeightType::kPartonShowerWeights, "PSWeight"},
      {gen::WeightType::kUnknownWeights, "UnknownWeight"},
  };
  int lheWeightPrecision_;
  bool keepAllPSWeights_;
  bool ignoreLheGroups_ = true;
  bool ignoreGenGroups_ = false;
  int nStoreUngroupedLhe_ = 105;

  enum { inLHE, inGen };
};
//put back if needed; till now not used
GenWeightsTableProducer::GenWeightsTableProducer(edm::ParameterSet const& params)
    : lheWeightTokens_(
          edm::vector_transform(params.getParameter<std::vector<edm::InputTag>>("lheWeights"),
                                [this](const edm::InputTag& tag) { return mayConsume<GenWeightProduct>(tag); })),
      lheWeightInfoTokens_(edm::vector_transform(
          params.getParameter<std::vector<edm::InputTag>>("lheWeights"),
          [this](const edm::InputTag& tag) { return mayConsume<GenWeightInfoProduct, edm::InLumi>(tag); })),
      genWeightToken_(consumes<GenWeightProduct>(params.getParameter<edm::InputTag>("genWeights"))),
      genWeightInfoToken_(
          consumes<GenWeightInfoProduct, edm::InLumi>(params.getParameter<edm::InputTag>("genWeights"))),
      genEventInfoToken_(consumes<GenEventInfoProduct>(params.getParameter<edm::InputTag>("genEvent"))),
      genLumiInfoHeadTag_(
          mayConsume<GenLumiInfoHeader, edm::InLumi>(params.getParameter<edm::InputTag>("genLumiInfoHeader"))),
      weightgroups_(edm::vector_transform(params.getParameter<std::vector<std::string>>("weightgroups"),
                                          [](auto& c) { return gen::WeightType(c.at(0)); })),
      maxGroupsPerType_(params.getParameter<std::vector<int>>("maxGroupsPerType")),
      pdfIds_(params.getUntrackedParameter<std::vector<int>>("pdfIds", {})),
      lheWeightPrecision_(params.getParameter<int32_t>("lheWeightPrecision")),
      keepAllPSWeights_(params.getParameter<bool>("keepAllPSWeights")) {
  if (weightgroups_.size() != maxGroupsPerType_.size())
    throw std::invalid_argument("Inputs 'weightgroups' and 'maxGroupsPerType' must have equal size");
  produces<nanoaod::FlatTable>("GENWeight");
  produces<nanoaod::MergeableCounterTable, edm::Transition::EndRun>();
  produces<std::string>("genModel");
  produces<std::vector<nanoaod::FlatTable>>("LHEWeightTableVec");
}

void GenWeightsTableProducer::produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const {
  //access counter for weight sums
  Counter& counter = *streamCache(id)->get();
  edm::Handle<GenWeightProduct> lheWeightHandle;
  bool foundLheWeights = false;
  for (auto& token : lheWeightTokens_) {
    iEvent.getByToken(token, lheWeightHandle);
    if (lheWeightHandle.isValid()) {
      foundLheWeights = true;
      break;
    }
  }
  //Taken from genweight producer 
  // generator information (always available)
  auto const& genInfo = iEvent.get(genEventInfoToken_);
  const double genWeight = genInfo.weight();
  // table for gen info, always available
  auto outGeninfo = std::make_unique<nanoaod::FlatTable>(1, "genWeight", true);
  outGeninfo->setDoc("generator weight");
  outGeninfo->addColumnValue<float>("", genInfo.weight(), "generator weight");
  iEvent.put(std::move(outGeninfo), "GENWeight");
  //this will take care of sum of genWeights
  counter.incGenOnly(genWeight);

  std::string& model_label = streamCache(id)->getLabel();
  auto outM = std::make_unique<std::string>((!model_label.empty()) ? std::string("GenModel_") + model_label : "");
  iEvent.put(std::move(outM), "genModel");

  WeightsContainer lheWeights;
  if (foundLheWeights) {
    const GenWeightProduct* lheWeightProduct = lheWeightHandle.product();
    lheWeights = lheWeightProduct->weights();
  }

  edm::Handle<GenWeightProduct> genWeightHandle;
  iEvent.getByToken(genWeightToken_, genWeightHandle);
  const GenWeightProduct* genWeightProduct = genWeightHandle.product();
  WeightsContainer genWeights = genWeightProduct->weights();

  auto const& weightInfos = *luminosityBlockCache(iEvent.getLuminosityBlock().index());

  //create a container with dummy weight vector
  auto weightTablevec = std::make_unique<std::vector<nanoaod::FlatTable>>();
  if (foundLheWeights) {
    addWeightGroupToTable(weightTablevec, weightInfos.at(inLHE), lheWeights);
  }

  addWeightGroupToTable(weightTablevec, weightInfos.at(inGen), genWeights);

  iEvent.put(std::move(weightTablevec),"LHEWeightTableVec");
}

void GenWeightsTableProducer::addWeightGroupToTable(
						    std::unique_ptr<std::vector<nanoaod::FlatTable>>& weightTablevec,
                                                    const WeightGroupDataContainer& weightInfos,
                                                    WeightsContainer& allWeights) const {
  std::unordered_map<gen::WeightType, int> typeCount = {};
  for (auto& type : gen::allWeightTypes)
    typeCount[type] = 0;

  // TODO: Handle keepAllWeights 
  for (const auto& groupInfo : weightInfos) {
    gen::WeightType weightType = groupInfo.group->weightType();
    std::string entryName = weightTypeNames_.at(weightType);
    std::string label = groupInfo.group->description();
    auto& weights = allWeights.at(groupInfo.index);
    if (weightType == gen::WeightType::kScaleWeights) {
        const auto& scaleGroup = *std::static_pointer_cast<const gen::ScaleWeightGroupInfo>(groupInfo.group);
        auto weightsAndLabel = orderedScaleWeights(weights, scaleGroup);
        label.append(weightsAndLabel.first);
        weights = weightsAndLabel.second;
    } else if (weightType == gen::WeightType::kPartonShowerWeights) {
      const auto& psGroup = *std::static_pointer_cast<const gen::PartonShowerWeightGroupInfo>(groupInfo.group);
      auto weightsAndLabel = preferredPSweights(weights, psGroup);
      label.append(weightsAndLabel.first);
      weights = weightsAndLabel.second;
    }
    else
      label.append(groupInfo.group->description());

    if(typeCount[weightType] > 0) {
      entryName.append("AltSet");
      entryName.append(std::to_string(typeCount[weightType]));
    }
    weightTablevec->emplace_back(weights.size(), entryName, false);
    weightTablevec->back().addColumn<float>("", weights, label, lheWeightPrecision_);

    typeCount[weightType]++;
  }
}

WeightGroupsToStore GenWeightsTableProducer::groupsToStore(bool foundLheWeights, 
    edm::Handle<GenWeightInfoProduct>& genWeightInfoHandle, 
    edm::Handle<GenWeightInfoProduct>& lheWeightInfoHandle) const {
  std::unordered_map<gen::WeightType, int> storePerType;
  for (size_t i = 0; i < weightgroups_.size(); i++)
      storePerType[weightgroups_.at(i)] = maxGroupsPerType_.at(i);
  
  WeightGroupsToStore weightsToStore;
  if (!(ignoreLheGroups_ && ignoreGenGroups_)) {
      for (auto weightType : gen::allWeightTypes) {
          if (foundLheWeights && !ignoreLheGroups_) {
              auto lheWeights = weightDataPerType(lheWeightInfoHandle, weightType, storePerType[weightType]);
              for (auto& w : lheWeights)
                  weightsToStore.at(inLHE).push_back({w.index, std::move(w.group)});
          }
      if (!ignoreGenGroups_) {
          auto genWeights = weightDataPerType(genWeightInfoHandle, weightType, storePerType[weightType]);
          for (auto& w : genWeights)
              weightsToStore.at(inGen).push_back({w.index, std::move(w.group)});
          }
      }
  }
  // For now treating 'ignoreGroups' as only relevant to LHE...
  else if (ignoreLheGroups_) {
      auto& allGroups = lheWeightInfoHandle->allWeightGroupsInfo();
      for (size_t i = 0; i < allGroups.size(); i++) {
          weightsToStore.at(inLHE).push_back({i, std::make_shared<gen::WeightGroupInfo>(allGroups[i])});
      }
  }
  return weightsToStore;
}

WeightGroupDataContainer GenWeightsTableProducer::weightDataPerType(edm::Handle<GenWeightInfoProduct>& weightsInfoHandle,
                                                                    gen::WeightType weightType,
                                                                    int& maxStore) const {
  std::vector<gen::WeightGroupData> allgroups;
  if (weightType == gen::WeightType::kPdfWeights && !pdfIds_.empty()) {
    allgroups = weightsInfoHandle->pdfGroupsWithIndicesByLHAIDs(pdfIds_);
  } else
    allgroups = weightsInfoHandle->weightGroupsAndIndicesByType(weightType);

  int toStore = maxStore;
  if (maxStore < 0 || static_cast<int>(allgroups.size()) <= maxStore) {
    // Modify size in case one type of weight is present in multiple products
    maxStore -= allgroups.size();
    toStore = allgroups.size();
  }

  WeightGroupDataContainer out;
  for (int i = 0; i < toStore; i++) {
    auto& group = allgroups.at(i);
    gen::SharedWeightGroupData temp = {group.index, std::move(group.group)};
    out.push_back(temp);
  }
  return out;
}

std::pair<std::string, std::vector<double>> 
GenWeightsTableProducer::orderedScaleWeights(const std::vector<double>& scaleWeights,
                                                const gen::ScaleWeightGroupInfo& scaleGroup) const {
  std::vector<double> weights;
  std::string labels = "LHE scale variation weights (w_var / w_nominal); ";
  if (scaleGroup.isWellFormed()) {
    weights.emplace_back(scaleWeights.at(scaleGroup.muR05muF05Index()));
    labels += "[0] is muR=0.5 muF=0.5; ";
    weights.emplace_back(scaleWeights.at(scaleGroup.muR05muF1Index()));
    labels += "[1] is muR=0.5 muF=1; ";
    weights.emplace_back(scaleWeights.at(scaleGroup.muR05muF2Index()));
    labels += "[2] is muR=0.5 muF=2; ";
    weights.emplace_back(scaleWeights.at(scaleGroup.muR1muF05Index()));
    labels += "[3] is muR=1 muF=0.5; ";
    weights.emplace_back(scaleWeights.at(scaleGroup.centralIndex()));
    labels += "[4] is muR=1 muF=1; ";
    weights.emplace_back(scaleWeights.at(scaleGroup.muR1muF2Index()));
    labels += "[5] is muR=1 muF=2; ";
    weights.emplace_back(scaleWeights.at(scaleGroup.muR2muF05Index()));
    labels += "[6] is muR=2 muF=0.5; ";
    weights.emplace_back(scaleWeights.at(scaleGroup.muR2muF1Index()));
    labels += "[7] is muR=2 muF=1; ";
    weights.emplace_back(scaleWeights.at(scaleGroup.muR2muF2Index()));
    labels += "[8] is muR=2 muF=2";
  }
  else {
    size_t nstore = std::min<size_t>(gen::ScaleWeightGroupInfo::MIN_SCALE_VARIATIONS, weights.size());
    weights = std::vector<double>(begin(weights), std::begin(weights)+nstore);
    labels.append("WARNING: Unexpected format found. Contains first " + std::to_string(nstore) +
                 " elements of weights vector, unordered");
  }

  return std::make_pair(labels, weights);
}

std::pair<std::string, std::vector<double>> GenWeightsTableProducer::preferredPSweights(const std::vector<double>& psWeights,
                                                                const gen::PartonShowerWeightGroupInfo& pswV) const {
  std::vector<double> psTosave;

  std::string labels = "PS weights (w_var / w_nominal); ";
  // TODO: Probably just drop them if they aren't well formed...
  if (pswV.isWellFormed()) {
    double baseline = psWeights.at(pswV.weightIndexFromLabel("Baseline"));
    psTosave.emplace_back(psWeights.at(pswV.variationIndex(true, false, gen::PSVarType::def)) / baseline);
    labels += "[0] is ISR=2 FSR=1; ";
    psTosave.emplace_back(psWeights.at(pswV.variationIndex(false, false, gen::PSVarType::def)) / baseline);
    labels += "[1] is ISR=1 FSR=2; ";
    psTosave.emplace_back(psWeights.at(pswV.isrCombinedDownIndex(gen::PSVarType::def)) / baseline);
    labels += "[2] is ISR=0.5 FSR=1; ";
    psTosave.emplace_back(psWeights.at(pswV.fsrCombinedDownIndex(gen::PSVarType::def)) / baseline);
    labels += "[3] is ISR=1 FSR=0.5; ";
  }
  return std::make_pair(labels, psTosave);
}

void GenWeightsTableProducer::streamEndRunSummary(edm::StreamID id,
                                                  edm::Run const&,
                                                  edm::EventSetup const&,
                                                  CounterMap* runCounterMap) const {
  //this takes care for mergeing all the weight sums
  runCounterMap->mergeSumMap(*streamCache(id));
}

void GenWeightsTableProducer::globalEndRunProduce(edm::Run& iRun,
                                                  edm::EventSetup const&,
                                                  CounterMap const* runCounterMap) const {
  auto out = std::make_unique<nanoaod::MergeableCounterTable>();

  for (auto x : runCounterMap->countermap) {
    auto& runCounter = x.second;
    std::string label = std::string("_") + x.first;
    std::string doclabel = (!x.first.empty()) ? (std::string(", for model label ") + x.first) : "";

    out->addInt("genEventCount" + label, "event count" + doclabel, runCounter.num_);
    out->addFloat("genEventSumw" + label, "sum of gen weights" + doclabel, runCounter.sumw_);
    out->addFloat("genEventSumw2" + label, "sum of gen (weight^2)" + doclabel, runCounter.sumw2_);

    double norm = runCounter.sumw_ ? 1.0 / runCounter.sumw_ : 1;
    //Sum from map
    for (auto& sumw : runCounter.weightSumMap_) {
      //Normalize with genEventSumw
      for (auto& val : sumw.second)
        val *= norm;
      out->addVFloat(sumw.first + "Sumw" + label,
                     "Sum of genEventWeight *" + sumw.first + "[i]/genEventSumw" + doclabel,
                     sumw.second);
    }
  }
  iRun.put(std::move(out));
}
void GenWeightsTableProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::vector<edm::InputTag>>("lheWeights");
  desc.add<std::vector<edm::InputTag>>("lheInfo", std::vector<edm::InputTag>{{"externalLHEProducer"}, {"source"}})
      ->setComment("tag(s) for the LHE information (LHEEventProduct and LHERunInfoProduct)");
  //desc.add<std::vector<edm::InputTag>>("genWeights");
  desc.add<edm::InputTag>("genWeights");
  desc.add<edm::InputTag>("genEvent", edm::InputTag("generator"))
      ->setComment("tag for the GenEventInfoProduct, to get the main weight");
  desc.add<edm::InputTag>("genLumiInfoHeader", edm::InputTag("generator"))
      ->setComment("tag for the GenLumiInfoProduct, to get the model string");
  desc.add<std::vector<std::string>>("weightgroups");
  desc.add<std::vector<int>>("maxGroupsPerType");
  desc.addOptionalUntracked<std::vector<int>>("pdfIds");
  desc.add<int32_t>("lheWeightPrecision", -1)->setComment("Number of bits in the mantissa for LHE weights");
  desc.add<bool>("keepAllPSWeights", false)->setComment("True:stores all 45 PS weights; False:saves preferred 4");
  descriptions.addDefault(desc);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(GenWeightsTableProducer);
