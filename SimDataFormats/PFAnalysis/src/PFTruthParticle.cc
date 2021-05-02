#include "SimDataFormats/PFAnalysis/interface/PFTruthParticle.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include <FWCore/MessageLogger/interface/MessageLogger.h>

PFTruthParticle::PFTruthParticle() {
  // No operation
}

PFTruthParticle::~PFTruthParticle() {}

PFTruthParticle::PFTruthParticle(const TrackingParticleRefVector& trackingParticles, const SimClusterRefVector& simClusters) {
    setTrackingParticles(trackingParticles);
    setSimClusters(simClusters);
}

void PFTruthParticle::setTrackingParticles(const TrackingParticleRefVector& refs) { trackingParticles_ = refs; }

void PFTruthParticle::setSimClusters(const SimClusterRefVector& refs) { simClusters_ = refs; }

void PFTruthParticle::addSimCluster(const SimClusterRef& sc) { simClusters_.push_back(sc); }

void PFTruthParticle::addTrackingParticle(const TrackingParticleRef& tp) { trackingParticles_.push_back(tp); }

void PFTruthParticle::addGenParticle(const reco::GenParticleRef& gp) { genParticles_.push_back(gp); }

void PFTruthParticle::addG4Track(const SimTrack& t) { g4Tracks_.push_back(t); }

PFTruthParticle::genp_iterator PFTruthParticle::genParticle_begin() const { return genParticles_.begin(); }

PFTruthParticle::genp_iterator PFTruthParticle::genParticle_end() const { return genParticles_.end(); }

PFTruthParticle::g4t_iterator PFTruthParticle::g4Track_begin() const { return g4Tracks_.begin(); }

PFTruthParticle::g4t_iterator PFTruthParticle::g4Track_end() const { return g4Tracks_.end(); }
