#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/ForwardDetId/interface/ForwardSubdetector.h"
#include "DataFormats/ForwardDetId/interface/HFNoseDetId.h"
#include "DataFormats/HcalDetId/interface/HcalDetId.h"

#include <numeric>

const unsigned int SimCluster::longLivedTag = 65536;

SimCluster::SimCluster() {
  // No operation
}

SimCluster::SimCluster(const SimTrack &simtrk) {
  addG4Track(simtrk);
  event_ = simtrk.eventId();
  particleId_ = simtrk.trackId();

  theMomentum_.SetPxPyPzE(
      simtrk.momentum().px(), simtrk.momentum().py(), simtrk.momentum().pz(), simtrk.momentum().E());
}

SimCluster::SimCluster(EncodedEventId eventID, uint32_t particleID) {
  event_ = eventID;
  particleId_ = particleID;
}

SimCluster::SimCluster(const std::vector<SimTrack> &simtrks, int pdgId) {
  if (simtrks.size() > 0) {
    double sumPx = 0.;
    double sumPy = 0.;
    double sumPz = 0.;
    double sumE = 0.;

    for (const SimTrack &t : simtrks) {
      addG4Track(t);
      sumPx += t.momentum().px();
      sumPy += t.momentum().py();
      sumPz += t.momentum().pz();
      sumE += t.momentum().E();
    }

    theMomentum_.SetPxPyPzE(sumPx, sumPy, sumPz, sumE);

    // set event and particle ID (!= pdgID) from the first track for consistency
    event_ = simtrks[0].eventId();
    particleId_ = simtrks[0].trackId();
  }

  pdgId_ = pdgId;
}

math::XYZTLorentzVectorF SimCluster::impactMomentumMuOnly() const {
	math::XYZTLorentzVectorF mom;
	for (auto& t : g4Tracks_) {
		if (std::abs(t.type()) == 13)
			mom += t.getMomentumAtBoundary();
	}
	return mom;
}

math::XYZTLorentzVectorF SimCluster::impactMomentumNoMu() const {
	math::XYZTLorentzVectorF mom;
	for (auto& t : g4Tracks_) {
		if (std::abs(t.type()) != 13)
			mom += t.getMomentumAtBoundary();
	}
	return mom;
}

SimCluster SimCluster::operator+(const SimCluster& toAdd) {
	SimCluster orig = *this;
	return orig += toAdd;
}

SimCluster& SimCluster::operator+=(const SimCluster& toMerge) {
    for (auto& track : toMerge.g4Tracks())
        this->addG4Track(track);

    for (auto& hit_and_e : toMerge.hits_and_fractions())
        this->addDuplicateRecHitAndFraction(hit_and_e.first, hit_and_e.second);

    auto& mergeMom = toMerge.impactMomentum();
    float sumE = impactMomentum_.energy() + mergeMom.energy();
    impactPoint_ = (impactPoint_*impactMomentum_.energy() + mergeMom.energy()*toMerge.impactPoint())/sumE;

    this->impactMomentum_ += mergeMom;
    return *this;
}

// At least one simHit in the HGCAL
bool SimCluster::isHGCAL() const {
    for (const auto& hitsAndEnergies : hits_and_fractions()) {
        const DetId id = hitsAndEnergies.first;
        bool forward = id.det() == DetId::HGCalEE
                || id.det() == DetId::HGCalHSi
                || id.det() == DetId::HGCalHSc
                || (id.det() == DetId::Forward && id.subdetId() != static_cast<int>(HFNose))
                || (id.det() == DetId::Hcal && id.subdetId() == HcalSubdetector::HcalEndcap);

        if(forward)
            return true;
    }
    return false;
}

SimCluster::~SimCluster() {}

std::ostream &operator<<(std::ostream &s, SimCluster const &tp) {
  s << "CP momentum, q, ID, & Event #: " << tp.p4() << " " << tp.charge() << " " << tp.pdgId() << " "
    << tp.eventId().bunchCrossing() << "." << tp.eventId().event() << std::endl;

  for (SimCluster::genp_iterator hepT = tp.genParticle_begin(); hepT != tp.genParticle_end(); ++hepT) {
    s << " HepMC Track Momentum " << (*hepT)->momentum().rho() << std::endl;
  }

  for (SimCluster::g4t_iterator g4T = tp.g4Track_begin(); g4T != tp.g4Track_end(); ++g4T) {
    s << " Geant Track Momentum  " << g4T->momentum() << std::endl;
    s << " Geant Track ID & type " << g4T->trackId() << " " << g4T->type() << std::endl;
    if (g4T->type() != tp.pdgId()) {
      s << " Mismatch b/t SimCluster and Geant types" << std::endl;
    }
  }
  s << " # of cells = " << tp.hits_.size()
    << ", effective cells = " << std::accumulate(tp.fractions_.begin(), tp.fractions_.end(), 0.f) << std::endl;
  return s;
}
