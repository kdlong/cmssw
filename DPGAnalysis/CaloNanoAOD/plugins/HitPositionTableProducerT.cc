//Framework
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

//Util
#include "DataFormats/NanoAOD/interface/FlatTable.h"
#include "DataFormats/Common/interface/View.h"
#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

//Calo geometry includes
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

//HCAL geometry
#include "Geometry/CaloTopology/interface/HcalTopology.h"
#include "Geometry/HcalCommonData/interface/HcalDDDRecConstants.h"
#include "Geometry/HcalCommonData/interface/HcalHitRelabeller.h"
#include "Geometry/Records/interface/HcalRecNumberingRecord.h"

//DetIds
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"

//Hits
#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
#include "DataFormats/CaloRecHit/interface/CaloRecHit.h"
#include "DataFormats/ParticleFlowReco/interface/PFRecHit.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitDefs.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"

//stdlib
#include <vector>
#include <iostream>


template <typename T, bool HcalRelabel=false>
class HitPositionTableProducerT : public edm::stream::EDProducer<> {
public:
  HitPositionTableProducerT(edm::ParameterSet const& params)
      : name_(params.getParameter<std::string>("name")),
        doc_(params.getParameter<std::string>("doc")),
        src_(consumes<edm::View<T>>(params.getParameter<edm::InputTag>("src"))),
        cut_(params.getParameter<std::string>("cut"), true),
        caloGeomToken_(esConsumes<edm::Transition::BeginRun>()),
        hdrcToken_(esConsumes<edm::Transition::BeginRun>()){
    produces<nanoaod::FlatTable>();
  }

  ~HitPositionTableProducerT() override {}

  void beginRun(const edm::Run&, const edm::EventSetup& iSetup) override {
    caloGeom_ = iSetup.getHandle(caloGeomToken_);
    pHRNDC_ = iSetup.getHandle(hdrcToken_);
  }

  void produce(edm::Event& iEvent, const edm::EventSetup& iSetup) override {
    edm::Handle<edm::View<T>> objs;
    iEvent.getByToken(src_, objs);

    std::vector<float> xvals;
    std::vector<float> yvals;
    std::vector<float> zvals;
    std::vector<int> subdetvals;
    for (const auto& obj : *objs) {
      if (cut_(obj)) {
        int subdet;
        auto position = positionFromHit(obj, subdet);
        xvals.emplace_back(position.x());
        yvals.emplace_back(position.y());
        zvals.emplace_back(position.z());
        subdetvals.emplace_back(subdet);
      }
    }

    auto tab = std::make_unique<nanoaod::FlatTable>(xvals.size(), name_, false, true);
    tab->addColumn<float>("x", xvals, "x position");
    tab->addColumn<float>("y", yvals, "y position");
    tab->addColumn<float>("z", zvals, "z position");
    tab->addColumn<int>("subdet", subdetvals, "subdetector");

    iEvent.put(std::move(tab));
  }

  template <typename hit_t>
  GlobalPoint positionFromHit(const hit_t& hit, int& subdet) const { 
    return positionFromDetId(hit.detid(), subdet);
  }
  
  //why do PCaloHits and CaloRecHits have different interfaces????
  GlobalPoint positionFromHit(const PCaloHit& hit, int& subdet) const { 
    DetId id(hit.id());
    if constexpr(HcalRelabel){//why are Hcal SimHits mislabeled????
        HcalDetId hcalid = HcalHitRelabeller::relabel(id, pHRNDC_.product());
        id = DetId(hcalid.rawId());
    }
    return positionFromDetId(id, subdet);
  }

  GlobalPoint positionFromHit(const reco::PFRecHit& hit, int& subdet) const { 
    return positionFromDetId(hit.detId(), subdet);
  }

  GlobalPoint positionFromDetId(const DetId& id, int& subdet) const{
    switch(id.det()){
      case DetId::Hcal:
      case DetId::Ecal:
        subdet = id.subdetId();
        return getHitPosition(id);
      default:
        throw cms::Exception("HitPositionTableProducerT") 
            << "Cannot get position for DetId " << (long)id 
            << "With detector" << (long)id.det();
    }
  }

protected:
  const std::string name_, doc_;
  const edm::EDGetTokenT<edm::View<T>> src_;
  const StringCutObjectSelector<T> cut_;

  edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeomToken_;
  edm::ESGetToken<HcalDDDRecConstants, HcalRecNumberingRecord> hdrcToken_;

  edm::ESHandle<CaloGeometry> caloGeom_;
  edm::ESHandle<HcalDDDRecConstants> pHRNDC_;

  GlobalPoint getHitPosition(const DetId& id) const{
    auto thisCell = caloGeom_->getGeometry(id);
    if(!thisCell) {
      throw cms::Exception("HitPositionTableProducerT") 
          << "Could not find cell geometry for " 
          << (long)id << "(detector = " << (long)id.det() << ")";
    }
    return thisCell->getPosition();
  }
};

#include "FWCore/Framework/interface/MakerMacros.h"

typedef HitPositionTableProducerT<reco::PFRecHit> PFRecHitPositionTableProducer;
typedef HitPositionTableProducerT<CaloRecHit> CaloRecHitPositionTableProducer;
typedef HitPositionTableProducerT<EcalRecHit> EcalRecHitPositionTableProducer;
typedef HitPositionTableProducerT<HBHERecHit> HBHERecHitPositionTableProducer;
typedef HitPositionTableProducerT<HFRecHit> HFRecHitPositionTableProducer;
typedef HitPositionTableProducerT<HORecHit> HORecHitPositionTableProducer;
typedef HitPositionTableProducerT<PCaloHit, true> HcalSimHitPositionTableProducer;
typedef HitPositionTableProducerT<PCaloHit, false> EcalSimHitPositionTableProducer;

DEFINE_FWK_MODULE(PFRecHitPositionTableProducer);
DEFINE_FWK_MODULE(CaloRecHitPositionTableProducer);
DEFINE_FWK_MODULE(EcalRecHitPositionTableProducer);
DEFINE_FWK_MODULE(HBHERecHitPositionTableProducer);
DEFINE_FWK_MODULE(HORecHitPositionTableProducer);
DEFINE_FWK_MODULE(HFRecHitPositionTableProducer);
DEFINE_FWK_MODULE(HcalSimHitPositionTableProducer);
DEFINE_FWK_MODULE(EcalSimHitPositionTableProducer);


