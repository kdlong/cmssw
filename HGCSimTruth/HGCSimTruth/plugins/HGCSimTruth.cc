/*
 * CMSSW module to create the trees used for the HH2bbtautau analysis.
 */

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ProducerBase.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/ForwardDetId/interface/HGCalDetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"
#include "DataFormats/HcalDetId/interface/HcalTestNumbering.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/SiStripDetId/interface/StripSubdetector.h"

#include "SimDataFormats/CaloAnalysis/interface/CaloParticle.h"
#include "SimDataFormats/CaloAnalysis/interface/CaloParticleFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
#include "SimDataFormats/CaloTest/interface/HGCalTestNumbering.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"

#include "SimGeneral/MixingModule/interface/DigiAccumulatorMixMod.h"
#include "SimGeneral/MixingModule/interface/DigiAccumulatorMixModFactory.h"
#include "SimGeneral/MixingModule/interface/PileUpEventPrincipal.h"
#include "SimGeneral/TrackingAnalysis/interface/EncodedTruthId.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "Geometry/HcalCommonData/interface/HcalHitRelabeller.h"
#include "Geometry/HcalTowerAlgo/interface/HcalGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"

#include "TMath.h"
#include "TCanvas.h"
#include "TH1F.h"

#include <sys/time.h>

class HGCTruthProducer : public edm::stream::EDProducer<>
{
public:
    static void fillDescriptions(edm::ConfigurationDescriptions&);

    explicit HGCTruthProducer(const edm::ParameterSet&);
    ~HGCTruthProducer();

    virtual void beginJob() override;
    virtual void beginRun(const edm::Run&, const edm::EventSetup&) override;
    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;

private:
    bool verbose_;

    edm::EDGetTokenT<std::vector<CaloParticle>> caloParticleToken_;
    edm::EDGetTokenT<std::vector<SimCluster>> simClusterToken_;
    edm::EDGetTokenT<std::vector<std::vector<std::pair<int, int>>>> simClusterHistoryToken_;
    std::vector<edm::EDGetTokenT<HGCRecHitCollection>> recHitTokens_;
};

void HGCTruthProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;

    desc.addUntracked<bool>("verbose", false);
    desc.add<edm::InputTag>("caloParticleCollection", edm::InputTag("mix", "MergedCaloTruth"));
    desc.add<edm::InputTag>("simClusterCollection", edm::InputTag("mix", "MergedCaloTruth"));
    desc.add<edm::InputTag>("simClusterHistoryCollection", edm::InputTag("mix", "MergedCaloTruth"));
    desc.add<std::vector<edm::InputTag>>("recHitCollections", {
        edm::InputTag("HGCalRecHit", "HGCEERecHits"),
        edm::InputTag("HGCalRecHit", "HGCHEFRecHits"),
        edm::InputTag("HGCalRecHit", "HGCHEBRecHits"),
    });

    descriptions.add("hgcTruthProducer", desc);
}

HGCTruthProducer::HGCTruthProducer(const edm::ParameterSet& params)
    : verbose_(params.getUntrackedParameter<bool>("verbose"))
    , caloParticleToken_(consumes<std::vector<CaloParticle>>(params.getParameter<edm::InputTag>("caloParticleCollection")))
    , simClusterToken_(consumes<std::vector<SimCluster>>(params.getParameter<edm::InputTag>("simClusterCollection")))
    , simClusterHistoryToken_(consumes<std::vector<std::vector<std::pair<int, int>>>>(params.getParameter<edm::InputTag("simClusterHistoryCollection")>)) {
    if (verbose_) {
        std::cout << "running TreeWriter in verbose mode" << std::endl;
    }

    // setup recHitTokens
    for (auto& edm::InputTag recHitCollection : params.getParameter<std::vector<edm::InputTag>>("recHitCollections")) {
        recHitTokens_.push_back(consumes<HGCRecHitCollection>(recHitCollection));
    }

    // define products
    produces<std::vector<SimCluster>>();  // SimClusters
    produces<std::vector<float>>();  // radii
    produces<std::vector<math::XYZTLorentzVectorD>>();  // rechit-base four-momenta of merged clusters
}

void HGCTruthProducer::produce(edm::Event& event, const edm::EventSetup& setup) {
    // create unique pointers for output products
    std::unique_ptr<std::vector<SimCluster>> clusters = std::make_unique<std::vector<SimCluster>>();
    std::unique_ptr<std::vector<float>> radii = std::make_unique<std::vector<float>>();
    std::unique_ptr<std::vector<math::XYZTLorentzVectorD>> vectors = std::make_unique<std::vector<math::XYZTLorentzVectorD>>();

    // TODO

    // save outputs
    event.put(std::move(clusters));
    event.put(std::move(radii));
    event.put(std::move(vectors));
}

DEFINE_FWK_MODULE(HGCTruthProducer);
