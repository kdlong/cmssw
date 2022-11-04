#include "PhysicsTools/NanoAOD/interface/SimpleFlatTableProducer.h"

#include "DataFormats/CaloRecHit/interface/CaloRecHit.h"
typedef SimpleFlatTableProducer<CaloRecHit> SimpleCaloRecHitFlatTableProducer;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(SimpleCaloRecHitFlatTableProducer);
