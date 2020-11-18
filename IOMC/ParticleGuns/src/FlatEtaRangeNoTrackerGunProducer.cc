#include <ostream>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/RandomNumberGenerator.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"

#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"

#include "DataFormats/Math/interface/Vector3D.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "IOMC/ParticleGuns/interface/FlatEtaRangeNoTrackerGunProducer.h"

#include "CLHEP/Units/GlobalSystemOfUnits.h"
#include "CLHEP/Units/GlobalPhysicalConstants.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/GlobalPhysicalConstants.h"
#include "CLHEP/Units/GlobalSystemOfUnits.h"

void edm::FlatEtaRangeNoTrackerGunProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;

  desc.add<std::vector<int>>("particleIDs");
  desc.add<int>("nParticles", 1);
  desc.add<bool>("exactShoot", true);
  desc.add<bool>("randomShoot", false);
  desc.add<double>("eMin", 1.);
  desc.add<double>("eMax", 100.);
  desc.add<double>("etaMin", 1.5);
  desc.add<double>("etaMax", 3.0);
  desc.add<double>("phiMin", 0.);
  desc.add<double>("phiMax", 2 * pi);
  desc.addUntracked<bool>("debug", false);
  desc.add<double>("timeSmearInPs",0.);
  desc.add<double>("momSmear",0.);
  desc.add<double>("minDistDR",0.01);

  descriptions.add("FlatEtaRangeNoTrackerGunProducer", desc);
}

edm::FlatEtaRangeNoTrackerGunProducer::FlatEtaRangeNoTrackerGunProducer(const edm::ParameterSet& params)
    : particleIDs_(params.getParameter<std::vector<int>>("particleIDs")),
      nParticles_(params.getParameter<int>("nParticles")),
      exactShoot_(params.getParameter<bool>("exactShoot")),
      randomShoot_(params.getParameter<bool>("randomShoot")),
      eMin_(params.getParameter<double>("eMin")),
      eMax_(params.getParameter<double>("eMax")),
      etaMin_(params.getParameter<double>("etaMin")),
      etaMax_(params.getParameter<double>("etaMax")),
      phiMin_(params.getParameter<double>("phiMin")),
      phiMax_(params.getParameter<double>("phiMax")),
      debug_(params.getUntrackedParameter<bool>("debug")),
      genEvent_(nullptr),
      timeSmear_(params.getParameter<double>("timeSmearInPs")*1e-12),
      momSmear_(params.getParameter<double>("momSmear")),
      minDistDR_(params.getParameter<double>("minDistDR")){
  produces<edm::HepMCProduct>("unsmeared");
  produces<GenEventInfoProduct>();
  produces<GenRunInfoProduct, edm::Transition::EndRun>();
}

edm::FlatEtaRangeNoTrackerGunProducer::~FlatEtaRangeNoTrackerGunProducer() {}

void edm::FlatEtaRangeNoTrackerGunProducer::beginRun(const edm::Run& run, const edm::EventSetup& setup) {
  setup.getData(pdgTable_);
  prop_.setEventSetup(setup);
}

void edm::FlatEtaRangeNoTrackerGunProducer::endRun(const edm::Run& run, const edm::EventSetup& setup) {}

void edm::FlatEtaRangeNoTrackerGunProducer::produce(edm::Event& event, const edm::EventSetup& setup) {
  edm::Service<edm::RandomNumberGenerator> rng;
  CLHEP::HepRandomEngine* engine = &(rng->getEngine(event.streamID()));

  if (debug_) {
    LogDebug("FlatEtaRangeNoTrackerGunProducer") << " : Begin New Event Generation" << std::endl;
  }

  // create a new event to fill
  genEvent_ = new HepMC::GenEvent();


  // determine the number of particles to shoot
  int n = 0;
  if (exactShoot_) {
    n = (int)particleIDs_.size();
  } else if (randomShoot_) {
    n = CLHEP::RandFlat::shoot(engine, 1, nParticles_ + 1);
  } else {
    n = nParticles_;
  }

  int particle_counter=0;
  std::vector<math::XYZPoint> previous_impacts;

  // shoot particles
    for (int i = 0; i < 2 * n; i++) { //n for positive and n for negative eta
        // create a random deltaR

        // obtain kinematics
        int id = particleIDs_[exactShoot_ ? particle_counter : CLHEP::RandFlat::shoot(engine, 0, particleIDs_.size())];
        particle_counter++;
        if(particle_counter>n)
            particle_counter=0;

        //generate initial momentum
        const HepPDT::ParticleData* pData = pdgTable_->particle(HepPDT::ParticleID(abs(id)));
        double eta = CLHEP::RandFlat::shoot(engine, etaMin_, etaMax_);
        if (i < n)
            eta *= -1;
        double phi = CLHEP::RandFlat::shoot(engine, phiMin_, phiMax_);
        double e = CLHEP::RandFlat::shoot(engine, eMin_, eMax_);
        double m = pData->mass().value();
        double p = sqrt(e * e - m * m);
        math::XYZVector pVec = p
                * math::XYZVector(cos(phi), sin(phi), sinh(eta)).unit();
        math::XYZTLorentzVectorF momentum(pVec.x(), pVec.y(), pVec.z(), e);

        //initialize original vertex
        math::XYZTLorentzVectorF vertexPos(0,0,0,0);
        if(timeSmear_>0){
            double t = CLHEP::RandFlat::shoot(engine, -timeSmear_, timeSmear_);
            vertexPos.SetE(t);
        }

        if(debug_)
            LogDebug("FlatEtaRangeNoTrackerGunProducer") << " : Initial vertex position " << vertexPos << std::endl;
        //propagate
        prop_.propagate(vertexPos,momentum,(pData->charge() > 0) ? 1 : ((pData->charge() < 0) ? -1 : 0));

        //just move a tad (1mm) towards beamspot
        if(vertexPos.z()>0)
            vertexPos.SetXYZT(vertexPos.x(),vertexPos.y(),vertexPos.z()-0.1,vertexPos.t());
        else
            vertexPos.SetXYZT(vertexPos.x(),vertexPos.y(),vertexPos.z()+0.1,vertexPos.t());

        if(minDistDR_>0){
            bool next=false;
            for(const auto& previ: previous_impacts){
                math::XYZPoint this_im;
                if(reco::deltaR(previ.eta(),previ.phi(),vertexPos.eta(),vertexPos.phi())  < minDistDR_){
                    next=true;
                    break;
                }
            }
            if(next){
                i--;
                if(debug_)
                    LogDebug("FlatEtaRangeNoTrackerGunProducer") << " : skipping too close particle" << std::endl;
                continue;
            }
            previous_impacts.push_back(math::XYZPoint(vertexPos.x(),vertexPos.y(),vertexPos.z()));
        }

        if(momSmear_>0){
            //TBI
        }
        //convert all to HEPMC

        HepMC::GenVertex* vtx = new HepMC::GenVertex(HepMC::FourVector(
                vertexPos.x()*cm,vertexPos.y()*cm, vertexPos.z()*cm,vertexPos.t()*s*c_light
                ));

        HepMC::GenParticle* particle = new HepMC::GenParticle(
                HepMC::FourVector(momentum.x(), momentum.y(),
                        momentum.z(), momentum.E()),
                 id, 1);
        particle->suggest_barcode(i + 1);

        // add the particle to the vertex and the vertex to the event
        vtx->add_particle_out(particle);
        genEvent_->add_vertex(vtx);

        if (debug_) {
            vtx->print();
            particle->print();
        }
    }

  // fill event attributes
  genEvent_->set_event_number(event.id().event());
  genEvent_->set_signal_process_id(20);

  if (debug_) {
    genEvent_->print();
  }

  // store outputs
  std::unique_ptr<HepMCProduct> BProduct(new HepMCProduct());
  BProduct->addHepMCData(genEvent_);
  event.put(std::move(BProduct), "unsmeared");
  std::unique_ptr<GenEventInfoProduct> genEventInfo(new GenEventInfoProduct(genEvent_));
  event.put(std::move(genEventInfo));

  if (debug_) {
    LogDebug("FlatEtaRangeNoTrackerGunProducer") << " : Event Generation Done " << std::endl;
  }
}

void edm::FlatEtaRangeNoTrackerGunProducer::endRunProduce(edm::Run& run, const edm::EventSetup& setup) {
  std::unique_ptr<GenRunInfoProduct> genRunInfo(new GenRunInfoProduct());
  run.put(std::move(genRunInfo));
}
