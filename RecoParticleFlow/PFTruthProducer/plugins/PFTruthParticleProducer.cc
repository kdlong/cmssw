// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/global/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/Associations/interface/TrackToTrackingParticleAssociator.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimDataFormats/CaloAnalysis/interface/CaloParticle.h"
#include "SimDataFormats/CaloAnalysis/interface/CaloParticleFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "SimDataFormats/PFAnalysis/interface/PFTruthParticle.h"
#include "SimDataFormats/PFAnalysis/interface/PFTruthParticleFwd.h"


#include "FWCore/Utilities/interface/EDGetToken.h"
#include <set>

//
// class declaration
//
/*
 *
 *  produces in two steps PFTruthParticles.
 *  This producer considers truth particles one-by-one and does not
 *  account for possible overlaps. These will need to be resolved in a second producer
 *  (PFTruthParticleMerger).
 *  The advantage is that the plugin here can run on every single event, independent of
 *  pileup mixing and event density, while the second producer can be run after mixing in pileup
 *
 *  Step A)
 *     Produces an ideal PFTruthParticle, without considering detector limitations.
 *     This is done by connecting every trackingParticle and SimCluster that originated
 *     from a particle passed to Geant back to it.
 *     No configuration options needed.
 *     The tracking particles should be selected by one of the standard tracking selectors, e.g.:
 *     https://github.com/cms-sw/cmssw/blob/master/Validation/RecoTrack/python/TrackingParticleSelectionForEfficiency_cfi.py
 *
 *  Step B)
 *     The ideal PFTruthParticle gets split according to detector limitations, e.g. if
 *     a neutral pion splits to 2 photons and leaves to distinct photon showers,
 *     this step would produce 2 photons.
 *
 *     For neutral and charged particles, stray clusters with deltaR larger than <softkillcone>
 *     and tracks that contribute with less than a relative <softkillrelenergy> to
 *     the total particle energy are removed.
 *
 *     This splitting of neutral particles depends on the granularity of the SimClusters
 *     in the respective subdetector.
 *     <add other possible options as configuration parameters>
 *
 *     In case of charged particles, there are multiple options, e.g. when to split off bremsstrahlung
 *     for an electron.
 *
 *
 *
 *
 */

typedef edm::AssociationMap<edm::OneToMany<
    TrackingParticleCollection, SimClusterCollection>> TrackingParticleToSimCluster;

class PFTruthParticleProducer : public edm::global::EDProducer<> {
public:
  explicit PFTruthParticleProducer(const edm::ParameterSet &);
  ~PFTruthParticleProducer() override;

private:
  void produce(edm::StreamID, edm::Event &, const edm::EventSetup &) const override;

  const SimTrack* getRoot(const SimTrack * st,
          const std::vector<SimTrack> & simtracks,
          const std::vector<SimVertex> & simvertices,
          const std::map<int,int>&  trackIdToTrackIdxAsso ) const;

  edm::EDGetTokenT<TrackingParticleCollection> tpCollectionToken_;
  edm::EDGetTokenT<CaloParticleCollection> cpCollectionToken_;
  edm::EDGetTokenT<std::vector<SimVertex> > svCollectionToken_;
  edm::EDGetTokenT<std::vector<SimTrack> > stCollectionToken_;


};

PFTruthParticleProducer::PFTruthParticleProducer(const edm::ParameterSet &pset)
    : tpCollectionToken_(consumes<TrackingParticleCollection>(pset.getParameter<edm::InputTag>("trackingParticles"))),
      cpCollectionToken_(consumes<SimClusterCollection>(pset.getParameter<edm::InputTag>("caloParticles"))),
      svCollectionToken_(consumes<std::vector<SimVertex> >(pset.getParameter<edm::InputTag>("simVertices"))),
      stCollectionToken_(consumes<std::vector<SimTrack> >(pset.getParameter<edm::InputTag>("simTracks")))
{
  produces<PFTruthParticleCollection>();
}

PFTruthParticleProducer::~PFTruthParticleProducer() {}

//
// member functions
//


const SimTrack* PFTruthParticleProducer::getRoot(
        const SimTrack * st,
        const std::vector<SimTrack> & simtracks,
        const std::vector<SimVertex> & simvertices,
        const std::map<int,int>&  trackIdToTrackIdxAsso ) const{

    SimTrack* out=st;
    int vidx = out->vertIndex();//starting point
    while(true){
        if(vidx<0)
            break; //no vertex
        const SimVertex& vertex = simvertices.at(vidx);
        int stid = vertex.parentIndex();//this is Geant track ID, not vector index
        int stidx = trackIdToTrackIdxAsso[stid]; //get vector index
        if(stidx<0)
            break;
        const SimTrack & simtrack = simtracks.at(stidx);
        vidx = simtrack.vertIndex();
        out=&simtrack;
    }
    return out;
}


// ------------ method called to produce the data  ------------
void PFTruthParticleProducer::produce(edm::StreamID, edm::Event &iEvent, const edm::EventSetup &iSetup) const {

  edm::Handle<TrackingParticleCollection> tpCollection;
  iEvent.getByToken(tpCollectionToken_, tpCollection);

  edm::Handle<CaloParticleCollection> cpCollection;
  iEvent.getByToken(cpCollectionToken_, cpCollection);

  edm::Handle<std::vector<SimVertex> > svCollection;
  iEvent.getByToken(svCollectionToken_, svCollection);

  edm::Handle<std::vector<SimTrack> > stCollection;
  iEvent.getByToken(stCollectionToken_, stCollection);

  std::map<int,int>  trackIdToTrackIdxAsso;
  for(size_t i=0;i<stCollection->size();i++){
      trackIdToTrackIdxAsso[stCollection->at(i).trackId()] = i;
  }

  //unrealistic / ideal PF Truth: create one PFParticle form Geant handover aka CaloParticle

  std::vector<size_t> matchedtptocp;
  PFTruthParticleCollection idealPFTruth;

  for(const auto& cp: *cpCollection){

      TrackingParticleRefVector tprefs;
      const SimClusterRefVector& screfs = cp.simClusters();

      bool hascharged=false;
      //match the tracking particle(s)
      for(size_t i=0; i< tpCollection->size(); i++){
          if(std::find(matchedtptocp.begin(),matchedtptocp.end(), i) != matchedtptocp.end())
              continue; //already matched

          TrackingParticleRef tpref(tpCollection, i);

          //go through whole history
          auto stp = getRoot(& tpref->g4Tracks().at(0),
                  *stCollection,*svCollection, trackIdToTrackIdxAsso);


          //match
          if(stp->trackId() == cp.g4Tracks().at(0).trackId()
                  && stp->eventId() == cp.g4Tracks().at(0).eventId()){
              matchedtptocp.push_back(i);
              tprefs.push_back(tpref);
              if(tpref->charge())
                  hascharged=true;
          }
      }
      PFTruthParticle pftp(tprefs,screfs);
      pftp.setP4(cp.p4());
      pftp.setPdgId(cp.pdgId());
      pftp.setCharge(cp.charge());
      int vidx = cp.g4Tracks().at(0).vertIndex();
      auto vertpos = svCollection->at(vidx).position();

      pftp.setVertex(PFTruthParticle::LorentzVectorF(
              vertpos.x(),vertpos.y(),vertpos.z(),vertpos.t()));

      idealPFTruth.push_back(pftp);
  }


  // --- now we have the ideal PF truth, far from realistic.
  // --- this is where the actual "meat" of the implementation starts

  double softkill_relthreshold = 0.001;//just to avoid ambiguities.
  double hardsplit_deltaR = 0.3;


  std::unique_ptr<PFTruthParticleCollection> PFtruth;

  //apply splitting particle by particle
  //this is where the physics happens
  for(const auto & pftp: idealPFTruth){


      //consider the ones without tracking particles // this is not the same as the ones without charge,
      // because e.g. a very early converting photon would have charge 0, but two tracking particles associated
      if(! pftp.trackingParticles().size()) {
          for(const auto& sc: pftp.simClusters()){
              //create PF particle per SimCluster
              //vertex position will remain the initial CP one (timing)
              auto npftp = pftp;
              npftp.trackingParticles().clear();
              npftp.simClusters().clear();
              npftp.addSimCluster(sc);
              //this is now defined to be a neutral PF Particle. Even if the SC happens to be charged
              npftp.setCharge(0);
              npftp.setP4(sc->impactMomentum());//that's all we measure
              npftp.setPdgId(sc->pdgId()); //to be discussed
              PFtruth->push_back(npftp);
          }
          continue;
      }
      //remaining: the ones that have tracking particles associated


      //some more cases, like far away brem...


      //trim
      auto trimmed_pfp = pftp;
      SimClusterRefVector trimmersc;
      for(auto & sc: trimmed_pfp.simClusters()){
          if(sc->impactMomentum().E() > trimmed_pfp.p4().E()){
              trimmersc.push_back(sc);
          }
      }
      trimmed_pfp.setSimClusters(trimmersc);
      //no split
      PFtruth->push_back(trimmed_pfp);
  }




  iEvent.put(std::move(PFtruth), "PFTruthParticles");


}

// define this as a plug-in
DEFINE_FWK_MODULE(PFTruthParticleProducer);

