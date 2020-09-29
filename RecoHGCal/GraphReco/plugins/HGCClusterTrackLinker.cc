#include <memory>
#include <iostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "DataFormats/Common/interface/View.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h" 

#include "DataFormats/HGCalReco/interface/TICLLayerTile.h"

class HGCClusterTrackLinker : public edm::stream::EDProducer<> {
public:
    explicit HGCClusterTrackLinker(const edm::ParameterSet&);
    ~HGCClusterTrackLinker() {};

 private:
    void produce(edm::Event&, const edm::EventSetup&) override;
    
    edm::EDGetTokenT<edm::View<reco::Track>> tracksToken_;
    edm::EDGetTokenT<reco::PFCandidateCollection> pfCandsToken_;

};

HGCClusterTrackLinker::HGCClusterTrackLinker(const edm::ParameterSet& config) :
    tracksToken_(consumes<edm::View<reco::Track>>(config.getParameter<edm::InputTag>("tracks"))),
    pfCandsToken_(consumes<reco::PFCandidateCollection>(config.getParameter<edm::InputTag>("pfCands")))
{}

void HGCClusterTrackLinker::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  edm::Handle<reco::PFCandidateCollection> pfCands;
  iEvent.getByToken(pfCandsToken_, pfCands);
    

  edm::Handle<edm::View<reco::Track>> tracks;
  iEvent.getByToken(tracksToken_, tracks);

  ticl::TICLLayerTile candTile;
  ticl::TICLLayerTile tracksTile;

  for (size_t i = 0; i < tracks->size(); i++) {
    edm::RefToBase<reco::Track> track(tracks, i);
    if (std::abs(track->eta()) > ticl::TileConstants::minEta && std::abs(track->eta()) < ticl::TileConstants::maxEta)
      tracksTile.fill(track->eta(), track->phi(), i);
  }

  for (size_t ip = 0; ip < pfCands->size(); ip++) {
    reco::PFCandidateRef pfCand(pfCands, ip);
    candTile.fill(pfCand->eta(), pfCand->phi(), ip);
  }

  for (size_t bin = 0; bin < static_cast<size_t>(candTile.nBins()); bin++) {
    const std::vector<unsigned int>& trackIndices = tracksTile[bin]; 
    const std::vector<unsigned int>& candIndices = candTile[bin]; 
    std::cout << "----------------------------------------------------------\n";
    std::cout << "For global bin " << bin;
    std::cout << " found " << trackIndices.size() << " tracks and " << candIndices.size() << " pfcands" << std::endl;
    for (auto& tidx : trackIndices) {
        edm::RefToBase<reco::Track> track(tracks, tidx);
      std::cout << "Associated track with index " << tidx;
      std::cout << "Track has eta,phi,pt" << track->eta() << ", " << track->phi() << ", " << track->pt() << std::endl;
    }
    for (auto& cidx : candIndices) {
      reco::PFCandidateRef pfCand(pfCands, cidx);
      std::cout << "Associated cand with index " << cidx;
      std::cout << "PFCand has eta,phi,pt" << pfCand->eta() << ", " << pfCand->phi() << ", " << pfCand->pt() << std::endl;
    }
  }
}

DEFINE_FWK_MODULE(HGCClusterTrackLinker);
