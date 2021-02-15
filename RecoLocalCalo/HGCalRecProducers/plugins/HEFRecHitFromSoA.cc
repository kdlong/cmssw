#include <string>

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHit.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "DataFormats/ForwardDetId/interface/HGCSiliconDetId.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/EDPutToken.h"

#include "CUDADataFormats/HGCal/interface/HGCRecHitSoA.h"

class HEFRecHitFromSoA : public edm::stream::EDProducer<> {
public:
  explicit HEFRecHitFromSoA(const edm::ParameterSet& ps);
  ~HEFRecHitFromSoA() override;

  void produce(edm::Event&, const edm::EventSetup&) override;
  void convert_soa_data_to_collection_(const uint32_t&, HGCRecHitCollection&, HGCRecHitSoA*);

private:
  std::unique_ptr<HGChefRecHitCollection> rechits_;
  edm::EDGetTokenT<HGCRecHitSoA> recHitSoAToken_;
  edm::EDPutTokenT<HGChefRecHitCollection> recHitCollectionToken_;
};

HEFRecHitFromSoA::HEFRecHitFromSoA(const edm::ParameterSet& ps) {
  recHitSoAToken_ = consumes<HGCRecHitSoA>(ps.getParameter<edm::InputTag>("HEFRecHitSoATok"));
  recHitCollectionToken_ = produces<HGChefRecHitCollection>();
}

HEFRecHitFromSoA::~HEFRecHitFromSoA() {}

void HEFRecHitFromSoA::produce(edm::Event& event, const edm::EventSetup& setup) {
  HGCRecHitSoA recHitsSoA = event.get(recHitSoAToken_);
  rechits_ = std::make_unique<HGCRecHitCollection>();
  convert_soa_data_to_collection_(recHitsSoA.nhits_, *rechits_, &recHitsSoA);
  event.put(std::move(rechits_));
}

void HEFRecHitFromSoA::convert_soa_data_to_collection_(const uint32_t& nhits,
                                                       HGCRecHitCollection& rechits,
                                                       HGCRecHitSoA* h_calibSoA) {
  rechits.reserve(nhits);
  for (uint i = 0; i < nhits; ++i) {
    DetId id_converted(h_calibSoA->id_[i]);
    rechits.emplace_back(id_converted,
			 h_calibSoA->energy_[i],
			 h_calibSoA->time_[i],
			 0,
			 h_calibSoA->flagBits_[i],
			 h_calibSoA->son_[i],
			 h_calibSoA->timeError_[i]);
  }
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(HEFRecHitFromSoA);
