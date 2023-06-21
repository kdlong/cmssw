#include "PhysicsTools/NanoAOD/interface/HitPositionTableProducer.h"
#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCScintillatorDetId.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"

#include "DataFormats/CaloRecHit/interface/CaloRecHit.h"
#include "SimDataFormats/CaloHit/interface/PCaloHit.h"

#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"

template <typename T>
class HGCalHitPositionTableProducer : public HitPositionTableProducer<edm::View<T>> {
public:
  HGCalHitPositionTableProducer(edm::ParameterSet const& params)
      : HitPositionTableProducer<edm::View<T>>(params), 
        caloGeoToken_(edm::stream::EDProducer<>::esConsumes<edm::Transition::BeginRun>()) {}

  ~HGCalHitPositionTableProducer() override {}

  GlobalPoint positionFromHit(const CaloRecHit& hit) { 
    DetId detId = hit.detid();
    return positionFromDetId(detId); 
  }

  GlobalPoint positionFromHit(const PCaloHit& hit) { 
    DetId detId = hit.id();
    return positionFromDetId(detId); 
  }

  void beginRun(const edm::Run&, const edm::EventSetup& iSetup) override {
    auto& geom = iSetup.getData(caloGeoToken_);
    rhtools_.setGeometry(geom);
  }

  GlobalPoint positionFromDetId(DetId id) {
    DetId::Detector det = id.det();
    if (det == DetId::HGCalEE || det == DetId::HGCalHSi || det == DetId::HGCalHSc) {
      return rhtools_.getPosition(id);
    } else {
      throw cms::Exception("HGCalHitPositionTableProducer") << "Unsupported DetId type";
    }
  }

protected:
  edm::ESGetToken<CaloGeometry, CaloGeometryRecord> caloGeoToken_;
  hgcal::RecHitTools rhtools_;
};

#include "FWCore/Framework/interface/MakerMacros.h"
typedef HGCalHitPositionTableProducer<CaloRecHit> HGCalRecHitPositionTableProducer;
typedef HGCalHitPositionTableProducer<PCaloHit> HGCalSimHitPositionTableProducer;
DEFINE_FWK_MODULE(HGCalRecHitPositionTableProducer);
DEFINE_FWK_MODULE(HGCalSimHitPositionTableProducer);
