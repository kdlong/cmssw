#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Common/interface/View.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "DataFormats/Common/interface/Association.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"

#include <vector>
#include <iostream>

class ObjectIndexFromAssociationTableProducer : public edm::global::EDProducer<> {
public:
  ObjectIndexFromAssociationTableProducer(edm::ParameterSet const& params)
      : objName_(params.getParameter<std::string>("objName")),
        branchName_(params.getParameter<std::string>("branchName")),
        doc_(params.getParameter<std::string>("docString")),
        src_(consumes<edm::SimTrackContainer>(params.getParameter<edm::InputTag>("src"))),
        objMap_(consumes<edm::Association<SimClusterCollection>>(params.getParameter<edm::InputTag>("objMap"))),
        cut_(params.getParameter<std::string>("cut"), true) {
    produces<nanoaod::FlatTable>();
  }

  ~ObjectIndexFromAssociationTableProducer() override {}

  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {
    edm::Handle<edm::SimTrackContainer> objs;
    iEvent.getByToken(src_, objs);

    edm::Handle<edm::Association<SimClusterCollection>> assoc;
    iEvent.getByToken(objMap_, assoc);

    std::vector<int> keys;
    for (unsigned int i = 0; i < objs->size(); ++i) {
      SimTrackRef tk(objs, i);
      if (cut_(*tk)) {
        SimClusterRef match = (*assoc)[tk];
        int key = match.isNonnull() ? match.key() : -1;
        keys.emplace_back(key);
      }
    }

    std::cout << "Length of keys is " << keys.size() << std::endl;
    auto tab = std::make_unique<nanoaod::FlatTable>(keys.size(), objName_, false, true);
    tab->addColumn<int>(branchName_ + "Idx", keys, doc_);

    iEvent.put(std::move(tab));
  }

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<std::string>("objName")->setComment("name of the nanoaod::FlatTable to extend with this table");
    desc.add<std::string>("branchName")
        ->setComment(
            "name of the column to write (the final branch in the nanoaod will be <objName>_<branchName>Idx and "
            "<objName>_<branchName>Flav");
    desc.add<std::string>("docString")->setComment("documentation to forward to the output");
    desc.add<edm::InputTag>("src")->setComment(
        "physics object collection for the reconstructed objects (e.g. leptons)");
    desc.add<edm::InputTag>("objMap")->setComment(
        "tag to an edm::Association<SimClusterCollection> mapping src to gen, such as the one produced by MCMatcher");
    desc.add<std::string>("cut")->setComment("cut string to apply to input collection");
    descriptions.add("objIndexFromAssociation", desc);
  }

protected:
  const std::string objName_, branchName_, doc_;
  const edm::EDGetTokenT<edm::SimTrackContainer> src_;
  const edm::EDGetTokenT<edm::Association<SimClusterCollection>> objMap_;
  const StringCutObjectSelector<SimTrack> cut_;
};

#include "FWCore/Framework/interface/MakerMacros.h"
typedef ObjectIndexFromAssociationTableProducer SimClusterIndexFromAssociationTableProducer;
DEFINE_FWK_MODULE(SimClusterIndexFromAssociationTableProducer);
