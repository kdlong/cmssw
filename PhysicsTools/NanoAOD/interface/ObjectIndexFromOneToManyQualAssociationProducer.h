#include "FWCore/Framework/interface/global/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Common/interface/View.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/Common/interface/OneToManyWithQualityGeneric.h"
#include <vector>
#include <iostream>

template <typename T, typename M>
class ObjectIndexFromOneToManyQualAssociationTableProducer : public edm::global::EDProducer<> {
public:
  ObjectIndexFromOneToManyQualAssociationTableProducer(edm::ParameterSet const& params)
      : objName_(params.getParameter<std::string>("objName")),
        branchName_(params.getParameter<std::string>("branchName")),
        doc_(params.getParameter<std::string>("docString")),
        src_(consumes<T>(params.getParameter<edm::InputTag>("src"))),
        objMap_(consumes<edm::AssociationMap<edm::OneToManyWithQualityGeneric<T, M, float>>>(
            params.getParameter<edm::InputTag>("objMap"))),
        cut_(params.getParameter<std::string>("cut"), true) {
    produces<nanoaod::FlatTable>();
  }

  ~ObjectIndexFromOneToManyQualAssociationTableProducer() override {}

  void produce(edm::StreamID id, edm::Event& iEvent, const edm::EventSetup& iSetup) const override {
    edm::Handle<T> objs;
    iEvent.getByToken(src_, objs);

    edm::Handle<edm::AssociationMap<edm::OneToManyWithQualityGeneric<T, M, float>>> assoc;
    iEvent.getByToken(objMap_, assoc);
    //std::cout << "Size of map is " << assoc->size() << std::endl;

    std::vector<int> keys;
    std::vector<float> qualities;
    std::vector<int> nMatches;
    for (unsigned int i = 0; i < objs->size(); ++i) {
      edm::Ref<T> tk(objs, i);
      int key = -1;
      float quality = 0.0;
      int nmatch = 0;
      if (cut_(*tk)) {
        // As far as I can tell try/catch is the only way to do this
        if (assoc->numberOfAssociations(tk)) {
            auto& matchWithQual = (*assoc)[tk];
            // TODO: For now, just take the first
            auto match = matchWithQual.front();
            bool isvalid = match.first.isNonnull();
            key = isvalid ? match.first.key() : -1;
            //std::cout << "qual is " << match.second << std::endl;
            quality = isvalid ? match.second : 0.0;
            nmatch = matchWithQual.size();
        }
        
        nMatches.emplace_back(nmatch);
        keys.emplace_back(key);
        qualities.emplace_back(quality);
      }
    }

    auto tab = std::make_unique<nanoaod::FlatTable>(keys.size(), objName_, false, true);
    tab->addColumn<int>(branchName_ + "BestIdx", keys, doc_);
    tab->addColumn<int>(branchName_ + "BestMatchQual", qualities, doc_);
    tab->addColumn<int>(branchName_ + "Matches", nMatches, doc_);

    iEvent.put(std::move(tab));
  }

protected:
  const std::string objName_, branchName_, doc_;
  const edm::EDGetTokenT<T> src_;
  const edm::EDGetTokenT<edm::AssociationMap<edm::OneToManyWithQualityGeneric<T, M, float>>> objMap_;
  const StringCutObjectSelector<typename T::value_type> cut_;
};
