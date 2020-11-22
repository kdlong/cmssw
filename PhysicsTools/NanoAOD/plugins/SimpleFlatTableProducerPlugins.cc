#include "PhysicsTools/NanoAOD/interface/SimpleFlatTableProducer.h"

#include "DataFormats/Candidate/interface/Candidate.h"
typedef SimpleFlatTableProducer<reco::Candidate> SimpleCandidateFlatTableProducer;

#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
typedef SimpleFlatTableProducer<SimCluster> SimpleSimClusterFlatTableProducer;

#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
typedef SimpleFlatTableProducer<PCaloHit> SimplePCaloHitFlatTableProducer;

#include "SimDataFormats/CaloAnalysis/interface/PCaloHitWithPosition.h"
typedef SimpleFlatTableProducer<PCaloHitWithPosition> SimplePCaloHitWithPositionFlatTableProducer;

#include "SimDataFormats/Track/interface/SimTrack.h"
typedef SimpleFlatTableProducer<SimTrack> SimpleSimTrackFlatTableProducer;

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
typedef EventSingletonSimpleFlatTableProducer<GenEventInfoProduct> SimpleGenEventFlatTableProducer;

#include "SimDataFormats/HTXS/interface/HiggsTemplateCrossSections.h"
typedef EventSingletonSimpleFlatTableProducer<HTXS::HiggsClassification> SimpleHTXSFlatTableProducer;

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(SimplePCaloHitFlatTableProducer);
DEFINE_FWK_MODULE(SimplePCaloHitWithPositionFlatTableProducer);
DEFINE_FWK_MODULE(SimpleSimTrackFlatTableProducer);
DEFINE_FWK_MODULE(SimpleSimClusterFlatTableProducer);
DEFINE_FWK_MODULE(SimpleCandidateFlatTableProducer);
DEFINE_FWK_MODULE(SimpleGenEventFlatTableProducer);
DEFINE_FWK_MODULE(SimpleHTXSFlatTableProducer);
