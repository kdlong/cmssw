// system include files
#include <memory>

#include "ResidualGlobalCorrectionMakerBase.h"

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
#include "DataFormats/SiStripDetId/interface/SiStripDetId.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "DataFormats/SiPixelDetId/interface/PXFDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"
// #include "DataFormats/SiStripDetId/interface/TIDDetId.h"
// #include "DataFormats/SiStripDetId/interface/TIBDetId.h"
// #include "DataFormats/SiStripDetId/interface/TOBDetId.h"
// #include "DataFormats/SiStripDetId/interface/TECDetId.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/MeasurementPoint.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/CommonTopologies/interface/TkRadialStripTopology.h"
#include "DataFormats/Math/interface/approx_log.h"
#include "DataFormats/Math/interface/AlgebraicROOTObjects.h"
#include "TrackingTools/AnalyticalJacobians/interface/AnalyticalCurvilinearJacobian.h"
#include "TrackingTools/AnalyticalJacobians/interface/JacobianLocalToCurvilinear.h"
#include "TrackingTools/AnalyticalJacobians/interface/JacobianCurvilinearToLocal.h"
#include "TrackingTools/AnalyticalJacobians/interface/JacobianCurvilinearToCartesian.h"
#include "TrackingTools/AnalyticalJacobians/interface/JacobianCartesianToCurvilinear.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/ErrorFrameTransformer.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalPropagator.h"
#include "TrackingTools/MaterialEffects/interface/PropagatorWithMaterial.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/TrajectoryParametrization/interface/CurvilinearTrajectoryParameters.h"
#include "TrackingTools/KalmanUpdators/interface/KFSwitching1DUpdator.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHitBuilder.h"
#include "TrackingTools/Records/interface/TransientRecHitRecord.h" 
#include "RecoTracker/TransientTrackingRecHit/interface/TkTransientTrackingRecHitBuilder.h"
#include "TrackingTools/TrackFitters/interface/TrajectoryStateCombiner.h"
#include "TrackingTools/PatternTools/interface/TrajTrackAssociation.h"
#include "DataFormats/TrackerRecHit2D/interface/TkCloner.h"
#include "DataFormats/TrackingRecHit/interface/KfComponentsHolder.h"
#include "DataFormats/Math/interface/invertPosDefMatrix.h"
#include "DataFormats/Math/interface/ProjectMatrix.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h" 

#include "Geometry/TrackerGeometryBuilder/interface/StripGeomDetUnit.h"
#include "Geometry/CommonDetUnit/interface/GluedGeomDet.h"
#include "DataFormats/TrackerRecHit2D/interface/TrackerSingleRecHit.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "TrackingTools/PatternTools/interface/TSCPBuilderNoMaterial.h"
#include "TrackingTools/PatternTools/interface/TSCBLBuilderWithPropagator.h"
#include "DataFormats/TrackingRecHit/interface/InvalidTrackingRecHit.h"
#include "DataFormats/GeometrySurface/interface/Cylinder.h"
#include "TrackingTools/GeomPropagators/interface/HelixBarrelPlaneCrossingByCircle.h"
#include "TrackingTools/GeomPropagators/interface/HelixArbitraryPlaneCrossing.h"
#include "CondFormats/Alignment/interface/Alignments.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "RecoLocalTracker/SiStripClusterizer/interface/SiStripClusterInfo.h"
#include "DataFormats/MuonReco/interface/Muon.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "Alignment/CommonAlignment/interface/Utilities.h"




// #include "../interface/OffsetMagneticField.h"
// #include "../interface/ParmInfo.h"


#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "functions.h"
#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <unsupported/Eigen/AutoDiff>
#include<Eigen/StdVector>
#include <iostream>
#include <functional>


//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
ResidualGlobalCorrectionMakerBase::ResidualGlobalCorrectionMakerBase(const edm::ParameterSet &iConfig)

{
  //now do what ever initialization is needed
//   inputTraj_ = consumes<std::vector<Trajectory>>(edm::InputTag("TrackRefitter"));
//   inputTrack_ = consumes<TrajTrackAssociationCollection>(edm::InputTag("TrackRefitter"));
//   inputTrack_ = consumes<reco::TrackCollection>(edm::InputTag("TrackRefitter"));
//   inputIndices_ = consumes<std::vector<int> >(edm::InputTag("TrackRefitter"));
  
  
  inputTrackOrig_ = consumes<reco::TrackCollection>(edm::InputTag(iConfig.getParameter<edm::InputTag>("src")));

  
  fitFromGenParms_ = iConfig.getParameter<bool>("fitFromGenParms");
  fillTrackTree_ = iConfig.getParameter<bool>("fillTrackTree");
  fillGrads_ = iConfig.getParameter<bool>("fillGrads");
  fillRunTree_ = iConfig.getParameter<bool>("fillRunTree");
  doGen_ = iConfig.getParameter<bool>("doGen");
  doSim_ = iConfig.getParameter<bool>("doSim");
  bsConstraint_ = iConfig.getParameter<bool>("bsConstraint");
  applyHitQuality_ = iConfig.getParameter<bool>("applyHitQuality");
  doMuons_ = iConfig.getParameter<bool>("doMuons");
  corFile_ = iConfig.getParameter<std::string>("corFile");

  inputBs_ = consumes<reco::BeamSpot>(edm::InputTag("offlineBeamSpot"));

  
  if (doGen_) {
//     GenParticlesToken_ = consumes<std::vector<reco::GenParticle>>(edm::InputTag("genParticles"));
//     GenParticlesToken_ = consumes<edm::View<reco::Candidate>>(edm::InputTag("genParticles"));
    GenParticlesToken_ = consumes<edm::View<reco::Candidate>>(iConfig.getParameter<edm::InputTag>("genParticles"));
    genEventInfoToken_ = consumes<GenEventInfoProduct>(edm::InputTag("generator"));
  }
  
  if (doSim_) {
    genParticlesBarcodeToken_ = consumes<std::vector<int>>(edm::InputTag("genParticles"));

//     inputSimHits_ = consumes<std::vector<PSimHit>>(edm::InputTag("g4SimHits","TrackerHitsTECLowTof"));
    std::vector<std::string> labels;
    labels.push_back("TrackerHitsPixelBarrelLowTof");
    labels.push_back("TrackerHitsPixelEndcapLowTof");
    labels.push_back("TrackerHitsTECLowTof");
    labels.push_back("TrackerHitsTIBLowTof");
    labels.push_back("TrackerHitsTIDLowTof");
    labels.push_back("TrackerHitsTOBLowTof");
    
    for (const std::string& label : labels) {
      inputSimHits_.push_back(consumes<std::vector<PSimHit>>(edm::InputTag("g4SimHits", label)));
    }
    
    inputSimTracks_ = consumes<std::vector<SimTrack>>(edm::InputTag("g4SimHits"));
  }
  
  if (doMuons_) {
//     inputMuons_ = consumes<reco::MuonCollection>(edm::InputTag(iConfig.getParameter<edm::InputTag>("muons")));
    inputMuons_ = consumes<edm::View<reco::Muon>>(edm::InputTag(iConfig.getParameter<edm::InputTag>("muons")));
  }
  
  inputGeometry_ = consumes<int>(edm::InputTag("geopro"));
  
  debugprintout_ = false;

  doMuonAssoc_ = iConfig.getParameter<bool>("doMuonAssoc");
  if (doMuonAssoc_) {
    inputMuonAssoc_ = consumes<edm::Association<std::vector<pat::Muon>>>(iConfig.getParameter<edm::InputTag>("src"));
  }


//   fout = new TFile("trackTreeGrads.root", "RECREATE");
//   fout = new TFile("trackTreeGradsdebug.root", "RECREATE");
//   fout = new TFile("trackTreeGrads.root", "RECREATE");
  //TODO this needs a newer root version
//   fout->SetCompressionAlgorithm(ROOT::kLZ4);
//   fout->SetCompressionLevel(3);
  
//   edm::Service<TgFileService> fs;
  
//   tree = new TTree("tree", "tree");
  


}

ResidualGlobalCorrectionMakerBase::~ResidualGlobalCorrectionMakerBase()
{

  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
}

//
// member functions
//


// ------------ method called once each job just before starting event loop  ------------
void ResidualGlobalCorrectionMakerBase::beginStream(edm::StreamID streamid)
{
  if (fillTrackTree_ || fillRunTree_) {
    std::stringstream filenamestream;
    filenamestream << "globalcor_" << streamid.value() << ".root";
    fout = new TFile(filenamestream.str().c_str(), "RECREATE");
  }
  
//   runtree = new TTree("runtree","");
//   gradtree = fs->make<TTree>("gradtree","");
//   hesstree = fs->make<TTree>("hesstree","");
  
  
  if (fillTrackTree_) {
    tree = new TTree("tree","");
    const int basketSize = 4*1024*1024;
    tree->SetAutoFlush(0);
    
    tree->Branch("nParms", &nParms, basketSize);
//     tree->Branch("globalidxv", globalidxv.data(), "globalidxv[nParms]/i", basketSize);
    tree->Branch("globalidxv", &globalidxvfinal, basketSize);
    
    if (fillGrads_) {
      tree->Branch("gradv", gradv.data(), "gradv[nParms]/F", basketSize);
      tree->Branch("nSym", &nSym, basketSize);
      tree->Branch("hesspackedv", hesspackedv.data(), "hesspackedv[nSym]/F", basketSize);
      
      tree->Branch("gradmax", &gradmax);
      tree->Branch("hessmax", &hessmax);
    }
    
    tree->Branch("run", &run);
    tree->Branch("lumi", &lumi);
    tree->Branch("event", &event);
    
    tree->Branch("edmval", &edmval);
    tree->Branch("deltachisqval", &deltachisqval);
    tree->Branch("niter", &niter);
    
    tree->Branch("chisqval", &chisqval);
    tree->Branch("ndof", &ndof);

    tree->Branch("genweight", &genweight);
    
    nParms = 0.;

    
  }

}

// ------------ method called once each job just after ending the event loop  ------------
void ResidualGlobalCorrectionMakerBase::endStream()
{
  if (fout != nullptr) {

    fout->cd();

  //   TTree *gradtree = new TTree("gradtree","");
  //   unsigned int idx;
  //   double gradval;
  //   gradtree->Branch("idx",&idx);
  //   gradtree->Branch("gradval",&gradval);
  //   for (unsigned int i=0; i<gradagg.size(); ++i) {
  //     idx = i;
  //     gradval = gradagg[i];
  //     gradtree->Fill();
  //   }
  //
  //   TTree *hesstree = new TTree("hesstree","");
  //   unsigned int iidx;
  //   unsigned int jidx;
  //   double hessval;
  //   hesstree->Branch("iidx",&iidx);
  //   hesstree->Branch("jidx",&jidx);
  //   hesstree->Branch("hessval",&hessval);
  //
  //   for (auto const& item : hessaggsparse) {
  //     iidx = item.first.first;
  //     jidx = item.first.second;
  //     hessval = item.second;
  //     hesstree->Fill();
  //   }

    fout->Write();
    fout->Close();
  }
}

// ------------ method called when starting to processes a run  ------------

void 
ResidualGlobalCorrectionMakerBase::beginRun(edm::Run const& run, edm::EventSetup const& es)
{
  
  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  
  edm::ESHandle<TrackerGeometry> globalGeometryIdeal;
  es.get<TrackerDigiGeometryRecord>().get("idealForDigi", globalGeometryIdeal);
  
  edm::ESHandle<TrackerTopology> trackerTopology;
  es.get<TrackerTopologyRcd>().get(trackerTopology);
  
//   edm::ESHandle<Propagator> thePropagator;
//   es.get<TrackingComponentsRecord>().get("RungeKuttaTrackerPropagator", thePropagator);
//   const MagneticField* field = thePropagator->magneticField();

  edm::ESHandle<MagneticField> magfield;
  es.get<IdealMagneticFieldRecord>().get(magfield);
  auto field = magfield.product();
  
  std::set<std::pair<int, DetId> > parmset;
  
  

  
  for (const GeomDet* det : globalGeometry->detUnits()) {
    if (!det) {
      continue;
    }
    if (GeomDetEnumerators::isTracker(det->subDetector())) {
      
//       std::cout << "detid: " << det->geographicalId().rawId() << std::endl;
      
//       std::cout << "detid: " << det->geographicalId().rawId() << " subdet: " << det->subDetector() << " isStereo: " << trackerTopology->isStereo(det->geographicalId()) << " isRphi: " << trackerTopology->isRPhi(det->geographicalId()) << " glued: " << trackerTopology->glued(det->geographicalId()) << " stack: " << trackerTopology->stack(det->geographicalId()) << " upper: " << trackerTopology->upper(det->geographicalId()) << " lower: " << trackerTopology->lower(det->geographicalId()) << " partner: " << trackerTopology->partnerDetId(det->geographicalId()).rawId() <<" xi: " << det->surface().mediumProperties().xi() << std::endl;

      const bool ispixel = GeomDetEnumerators::isTrackerPixel(det->subDetector());
      const bool isendcap = GeomDetEnumerators::isEndcap(det->subDetector());

      const uint32_t gluedid = trackerTopology->glued(det->geographicalId());
      const bool isglued = gluedid != 0;
      const DetId parmdetid = isglued ? DetId(gluedid) : det->geographicalId();
      
//       const uint32_t gluedid = trackerTopology->glued(det->geographicalId());
//       const bool isglued = gluedid != 0;
// //       const bool align2d = ispixel || isglued || isendcap;      
//       const DetId parmdetid = isglued ? DetId(gluedid) : det->geographicalId();
      
//       const bool align2d = ispixel || isendcap;
//       const bool align2d = true;
      const bool align2d = ispixel;
//       const bool align2d = false;
//       const bool align2d = isendcap && !ispixel;

      
      //always have parameters for local x alignment, in-plane rotation, bfield, and e-loss
      parmset.emplace(0, det->geographicalId());
//       parmset.emplace(1, det->geographicalId());
//       parmset.emplace(2, det->geographicalId());
//       parmset.emplace(3, det->geographicalId());
//       parmset.emplace(4, det->geographicalId());
      parmset.emplace(5, det->geographicalId());
      
      if (align2d) {
        //local y alignment parameters only for pixels for now
        parmset.emplace(1, det->geographicalId());
      }
      // bfield and material parameters are associated to glued detids where applicable
      parmset.emplace(6, parmdetid);
      parmset.emplace(7, parmdetid);
    }
  }
  
//   const unsigned int netabins = 48;
//   const unsigned int nphibins = 36;
//   if (hetaphi == nullptr) {
//     hetaphi = new TH2D("hetaphi", "", netabins, -2.4, 2.4, nphibins, -M_PI, M_PI);
//   }
//   
//   for (unsigned int ibin = 0; ibin < (netabins + 2); ++ibin) {
//     for (unsigned int jbin = 1; jbin < (nphibins + 1); ++jbin) {
//       const unsigned int bin = hetaphi->GetBin(ibin, jbin);
//       parmset.emplace(8, bin);
//     }
//   }
  
  if (detidparms.empty()) {

    
    detidparms.clear();
    detidparmsrev.clear();
    
    TTree *runtree = nullptr;

    unsigned int iidx;
    int parmtype;
    unsigned int rawdetid;
    int subdet;
    int layer;
    int stereo;
    float x;
    float y;
    float z;
    float eta;
    float phi;
    float rho;
    float xi;
    float bx;
    float by;
    float bz;
    float bradial;
    float baxial;
    float b0;
    float b0trivial;
    float dx;
    float dy;
    float dz;
    float dtheta;



  //   assert(0);
    if (fillRunTree_) {
    //   TFile *runfout = new TFile("trackTreeGradsParmInfo.root", "RECREATE");
      fout->cd();
      runtree = new TTree("runtree", "");

      runtree->Branch("iidx", &iidx);
      runtree->Branch("parmtype", &parmtype);
      runtree->Branch("rawdetid", &rawdetid);
      runtree->Branch("subdet", &subdet);
      runtree->Branch("layer", &layer);
      runtree->Branch("stereo", &stereo);
      runtree->Branch("x", &x);
      runtree->Branch("y", &y);
      runtree->Branch("z", &z);
      runtree->Branch("eta", &eta);
      runtree->Branch("phi", &phi);
      runtree->Branch("rho", &rho);
      runtree->Branch("xi", &xi);
      runtree->Branch("bx", &bx);
      runtree->Branch("by", &by);
      runtree->Branch("bz", &bz);
      runtree->Branch("bradial", &bradial);
      runtree->Branch("baxial", &baxial);
      runtree->Branch("b0", &b0);
      runtree->Branch("b0trivial", &b0trivial);
      runtree->Branch("dx", &dx);
      runtree->Branch("dy", &dy);
      runtree->Branch("dz", &dz);
      runtree->Branch("dtheta", &dtheta);

    }
    
    unsigned int globalidx = 0;
    for (const auto& key: parmset) {
//       std::cout << "parmtype = " << key.first << " detid = " << key.second.rawId() << std::endl;
      
      //fill info
      iidx = globalidx;
      parmtype = key.first;
      
      if (parmtype < 8) {
      
        const DetId& detid = key.second;
        const GeomDet* det = globalGeometry->idToDet(detid);
        
        const Surface &surface = det->surface();
        
        const Surface &surfaceIdeal = globalGeometryIdeal->idToDet(detid)->surface();
        
        const GloballyPositioned<double> surfaceD = surfaceToDouble(surface);
        const GloballyPositioned<double> surfaceDIdeal = surfaceToDouble(surfaceIdeal);
        
        const Vector3DBase<double, GlobalTag> dpos = surfaceDIdeal.position() - surfaceD.position();
        
        dx = dpos.x();
        dy = dpos.y();
        dz = dpos.z();
        
        const Vector3DBase<double, LocalTag> localx(1., 0., 0.);
        const Vector3DBase<double, GlobalTag> xglob = surfaceD.toGlobal(localx);
        const Vector3DBase<double, GlobalTag> xglobIdeal = surfaceDIdeal.toGlobal(localx);
        
        const double dtheta = std::acos(xglob.dot(xglobIdeal));
        
        
    //     if (detid.rawId() == 302122272) {
    //       std::cout << "width: " << det->surface().bounds().width() << std::endl;
    //       std::cout << "length: " << det->surface().bounds().length() << std::endl;
    //       std::cout << "thickness: " << det->surface().bounds().thickness() << std::endl;
    //     }
        
        layer = 0;
        stereo = 0;
    //     int subdet = det->subDetector();
    //     float eta = det->surface().position().eta();

        if (det->subDetector() == GeomDetEnumerators::PixelBarrel)
        {
          PXBDetId detid(det->geographicalId());
          layer = detid.layer();
        }
        else if (det->subDetector() == GeomDetEnumerators::PixelEndcap)
        {
          PXFDetId detid(det->geographicalId());
          layer = -1 * (detid.side() == 1) * detid.disk() + (detid.side() == 2) * detid.disk();
        }
        else if (det->subDetector() == GeomDetEnumerators::TIB)
        {
    //       TIBDetId detid(det->geographicalId());
    //       layer = detid.layer();
          layer = trackerTopology->tibLayer(det->geographicalId());
          stereo = trackerTopology->isStereo(det->geographicalId());
        }
        else if (det->subDetector() == GeomDetEnumerators::TOB)
        {
    //       TOBDetId detid(det->geographicalId());
    //       layer = detid.layer();
          layer = trackerTopology->tobLayer(det->geographicalId());
          stereo = trackerTopology->isStereo(det->geographicalId());
        }
        else if (det->subDetector() == GeomDetEnumerators::TID)
        {
          unsigned int side = trackerTopology->tidSide(detid);
          unsigned int wheel = trackerTopology->tidWheel(detid);
          layer = -1 * (side == 1) * wheel + (side == 2) * wheel;
          stereo = trackerTopology->isStereo(det->geographicalId());

        }
        else if (det->subDetector() == GeomDetEnumerators::TEC)
        {
    //       TECDetId detid(det->geographicalId());
    //       layer = -1 * (detid.side() == 1) * detid.wheel() + (detid.side() == 2) * detid.wheel();
          unsigned int side = trackerTopology->tecSide(detid);
          unsigned int wheel = trackerTopology->tecWheel(detid);
          layer = -1 * (side == 1) * wheel + (side == 2) * wheel;
          stereo = trackerTopology->isStereo(det->geographicalId());
        }
        
    //     ParmInfo parminfo;
    //     parminfo.parmtype = key.first;
    //     parminfo.subdet = det->subDetector();
    //     parminfo.layer = layer;
    //     parminfo.x = det->surface().position().x();
    //     parminfo.y = det->surface().position().y();
    //     parminfo.z = det->surface().position().z();
    //     parminfo.eta = det->surface().position().eta();
    //     parminfo.phi = det->surface().position().phi();
    //     parminfo.rho = det->surface().position().perp();


        rawdetid = detid;
        subdet = det->subDetector();
        //layer already set above
        x = det->surface().position().x();
        y = det->surface().position().y();
        z = det->surface().position().z();
        eta = det->surface().position().eta();
        phi = det->surface().position().phi();
        rho = det->surface().position().perp();
        xi = det->surface().mediumProperties().xi();
        
        auto const bfieldval = field->inTesla(det->surface().position());
        bx = bfieldval.x();
        by = bfieldval.y();
        bz = bfieldval.z();
        
        const GlobalVector posv(det->surface().position().x(), det->surface().position().y(), det->surface().position().z());
        
        const GlobalVector posxy(det->surface().position().x(), det->surface().position().y(), 0.);
        bradial = bfieldval.dot(posxy)/posxy.mag();
        
        const GlobalVector rphi = posxy.cross(GlobalVector(0.,0.,1.));
        baxial = bfieldval.dot(rphi)/rphi.mag();
        
        const GlobalVector b0dir = posv.cross(rphi)/posv.mag()/rphi.mag();
        
        b0 = bfieldval.dot(b0dir)/b0dir.mag();
        
        const GlobalVector btrivial(0., 0., 3.8);
        b0trivial = btrivial.dot(b0dir)/b0dir.mag();
        
        detidlayermap[detid] = {{ subdet, layer, stereo }};
      }
      else {
        rawdetid = -99;
        subdet = -99;
        layer = -99;
        stereo = -99;
        x = -99.;
        y = -99.;
        z = -99.;
        rho = -99.;
        xi = -99.;
        bx = -99.;
        by = -99.;
        bz = -99.;
        
        const unsigned int etaphiidx = key.second;
        int etabin;
        int phibin;
        int kbin;
        
        hetaphi->GetBinXYZ(etaphiidx, etabin, phibin, kbin);
        eta = hetaphi->GetXaxis()->GetBinCenter(etabin);
        phi = hetaphi->GetYaxis()->GetBinCenter(phibin);
        
      }

      //fill map
      detidparms.emplace(key, globalidx);
      detidparmsrev.emplace_back(key);
      
      globalidx++;
      
      if (fillRunTree_) {
        runtree->Fill();
      }
    }
    
  //   runfout->Write();
  //   runfout->Close();
    
    unsigned int nglobal = detidparms.size();
  //   std::sort(detidparms.begin(), detidparms.end());
    std::cout << "nglobalparms = " << detidparms.size() << std::endl;
    
    //initialize gradient
    if (!gradagg.size()) {
      gradagg.resize(nglobal, 0.);
    }
    
    // load corrections from previous iteration if applicable
    corparms_.assign(parmset.size(), 0.);
    
    if (!corFile_.empty()) {
      TFile *corfile = TFile::Open(corFile_.c_str());
      TTree *cortree = (TTree*)corfile->Get("parmtree");
      
      unsigned int idx;
      float val;
      
//       cortree->SetBranchAddress("idx", &idx);
      cortree->SetBranchAddress("x", &val);
      
      const unsigned int nparms = cortree->GetEntries();
      assert(nparms == parmset.size());
      for (unsigned int iparm = 0; iparm < nparms; ++iparm) {
        cortree->GetEntry(iparm);
        corparms_[iparm] = val;
//         corparms_[idx] = val;
      }
    }
  
  }
  
  surfacemap_.clear();
  surfacemapD_.clear();
  // fill map of modified surfaces with results of previous iteration if applicable
  for (const GeomDet* det : globalGeometry->detUnits()) {
    if (!det) {
      continue;
    }
    if (GeomDetEnumerators::isTracker(det->subDetector())) {
      
      const bool ispixel = GeomDetEnumerators::isTrackerPixel(det->subDetector());
      const bool isendcap = GeomDetEnumerators::isEndcap(det->subDetector());
      
      const uint32_t gluedid = trackerTopology->glued(det->geographicalId());
      const bool isglued = gluedid != 0;
      const DetId parmdetid = isglued ? DetId(gluedid) : det->geographicalId();
      const GeomDet* parmDet = isglued ? globalGeometry->idToDet(parmdetid) : det;
      
      const double xifraction = isglued ? det->surface().mediumProperties().xi()/parmDet->surface().mediumProperties().xi() : 1.;
      
//       const bool align2d = ispixel;
//       const bool align2d = false;

      const bool align2d = detidparms.count(std::make_pair(1, det->geographicalId()));

      

      
      const unsigned int dxidx = detidparms.at(std::make_pair(0, det->geographicalId()));
      const unsigned int dthetaidx = detidparms.at(std::make_pair(5, det->geographicalId()));
//       const unsigned int dbidx = detidparms.at(std::make_pair(6, parmdetid));
      const unsigned int dxiidx = detidparms.at(std::make_pair(7, parmdetid));
      
      // n.b. sign is flipped for alignment terms
      const float dx = -corparms_[dxidx];
//       const float dy = corparms_[dyidx];
      const float dtheta = -corparms_[dthetaidx];
//       const float db = corparms_[dbdx];
      const float dxi = corparms_[dxiidx];
      
      float dy = 0.;
      if (align2d) {
        const unsigned int dyidx = detidparms.at(std::make_pair(1, det->geographicalId())); 
        dy = -corparms_[dyidx];
      }
      
      const Surface &surface = det->surface();
//       Point3DBase<double, GlobalTag> pos = surface.position();
//       
//       //re-orthogonalize
//       Matrix<double, 3, 3> rot;
//       rot << surface.rotation().xx(), surface.rotation().xy(), surface.rotation().xz(),
//               surface.rotation().yx(), surface.rotation().yy(), surface.rotation().yz(),
//               surface.rotation().zx(), surface.rotation().zy(), surface.rotation().zz();
//               
// //       std::cout << "rot check pre:" << std::endl;
// //       std::cout << rot.transpose()*rot << std::endl;
//             
//       for (unsigned int i = 0; i < 3; ++i) {
//         for (unsigned int j = 0; j < i; ++j) {
//           rot.row(i) = (rot.row(i) - (rot.row(i)*rot.row(j).transpose())[0]/rot.row(j).squaredNorm()*rot.row(j)).eval();
//         }
//       }
//       for (unsigned int i = 0; i < 3; ++i) {
//           rot.row(i) = (rot.row(i)/rot.row(i).norm()).eval();
//       }
//       
// //       std::cout << "rot check post:" << std::endl;
// //       std::cout << rot.transpose()*rot << std::endl;
//       
//       const TkRotation<double> tkrot(rot(0,0), rot(0,1), rot(0,2),
//                                      rot(1,0), rot(1,1), rot(1,2),
//                                      rot(2,0), rot(2,1), rot(2,2));
//       
//       
//       GloballyPositioned<double> surfaceD(pos, tkrot);
      
      GloballyPositioned<double> surfaceD = surfaceToDouble(surface);
      
      
      ReferenceCountingPointer<Plane> plane = Plane::build(det->surface());
//       std::shared_ptr<Plane> plane = std::make_shared<Plane>(det->surface());
      
      //move, rotate and modify material
      plane->move(plane->toGlobal(LocalVector(dx, dy, 0.)));
      plane->rotate(Surface::RotationType(Surface::RotationType::BasicVector(plane->toGlobal(LocalVector(0.,0.,1.))), dtheta));
      
      surfaceD.move(surfaceD.toGlobal(Vector3DBase<double, LocalTag>(dx, dy, 0.)));
      surfaceD.rotate(TkRotation<double>(TkRotation<double>::BasicVector(surfaceD.toGlobal(Vector3DBase<double, LocalTag>(0.,0.,1.))), dtheta));
      
//       Matrix<double, 3, 3> rot2;
//       rot2 << surfaceD.rotation().xx(), surfaceD.rotation().xy(), surfaceD.rotation().xz(),
//               surfaceD.rotation().yx(), surfaceD.rotation().yy(), surfaceD.rotation().yz(),
//               surfaceD.rotation().zx(), surfaceD.rotation().zy(), surfaceD.rotation().zz();
//       
//       std::cout << "rot2 check:" << std::endl;
//       std::cout << rot2.transpose()*rot2 << std::endl;
              
//       if (plane->mediumProperties().xi() + xifraction*dxi < 0.05*plane->mediumProperties().xi()) {
//         std::cout << "xi value clipped!" << std::endl;
//       }
      
      const float radLen = plane->mediumProperties().radLen();
      const float xi = std::max(plane->mediumProperties().xi() + xifraction*dxi, 0.05*plane->mediumProperties().xi());
//       const float xi = 0.05*plane->mediumProperties().xi();
      const MediumProperties mp(radLen, xi);
//       plane->setMediumProperties(mp);
      
//       printf("in beginRun, detid = %u, parmdetid = %u, oldp = %p, newp = %p\n", det->geographicalId().rawId(), parmdetid.rawId(), &det->surface(), &(*plane));
//       std::cout << "oldxi = " << det->surface().mediumProperties().xi() << " newxi = " << plane->mediumProperties().xi() << " dxi = " << dxi << " xifraction = " << xifraction << std::endl;
      
//       surfacemap_[det->geographicalId()] = plane;
      
      surfacemapD_[det->geographicalId()] = surfaceD;
      

    }
    
  }
    
  
}


GloballyPositioned<double> ResidualGlobalCorrectionMakerBase::surfaceToDouble(const Surface &surface) const {
  Point3DBase<double, GlobalTag> pos = surface.position();
  //re-orthogonalize
  Matrix<double, 3, 3> rot;
  rot << surface.rotation().xx(), surface.rotation().xy(), surface.rotation().xz(),
          surface.rotation().yx(), surface.rotation().yy(), surface.rotation().yz(),
          surface.rotation().zx(), surface.rotation().zy(), surface.rotation().zz();
          
//       std::cout << "rot check pre:" << std::endl;
//       std::cout << rot.transpose()*rot << std::endl;
        
  for (unsigned int i = 0; i < 3; ++i) {
    for (unsigned int j = 0; j < i; ++j) {
      rot.row(i) = (rot.row(i) - (rot.row(i)*rot.row(j).transpose())[0]/rot.row(j).squaredNorm()*rot.row(j)).eval();
    }
  }
  for (unsigned int i = 0; i < 3; ++i) {
      rot.row(i) = (rot.row(i)/rot.row(i).norm()).eval();
  }
  
//       std::cout << "rot check post:" << std::endl;
//       std::cout << rot.transpose()*rot << std::endl;
  
  const TkRotation<double> tkrot(rot(0,0), rot(0,1), rot(0,2),
                                  rot(1,0), rot(1,1), rot(1,2),
                                  rot(2,0), rot(2,1), rot(2,2));
  
  
  return GloballyPositioned<double>(pos, tkrot);
}

// ------------ method called when ending the processing of a run  ------------
/*
void 
HitAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
HitAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
HitAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void ResidualGlobalCorrectionMakerBase::fillDescriptions(edm::ConfigurationDescriptions &descriptions)
{
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

Matrix<double, 5, 1> ResidualGlobalCorrectionMakerBase::bfieldJacobian(const GlobalTrajectoryParameters& globalSource,
                                                             const GlobalTrajectoryParameters& globalDest,
                                                             const double& s,
                                                             const GlobalVector& bfield) const {
 
  //analytic jacobian wrt magnitude of magnetic field
  //TODO should we parameterize with respect to z-component instead?
  //extending derivation from CMS NOTE 2006/001
  const Vector3d b(bfield.x(),
                      bfield.y(),
                      bfield.z());
  const double magb = b.norm();
  const Vector3d h = b.normalized();

  const Vector3d p0(globalSource.momentum().x(),
                      globalSource.momentum().y(),
                      globalSource.momentum().z());
  const Vector3d p1(globalDest.momentum().x(),
                      globalDest.momentum().y(),
                      globalDest.momentum().z());
  const Vector3d M0(globalSource.position().x(),
                    globalSource.position().y(),
                    globalSource.position().z());
  const Vector3d T0 = p0.normalized();
  const Vector3d T = p1.normalized();
  const double p = p0.norm();
  const double q = globalSource.charge();
  const double qop = q/p;

  const Vector3d N0alpha = h.cross(T0);
  const double alpha = N0alpha.norm();
  const Vector3d N0 = N0alpha.normalized();
  const double gamma = h.transpose()*T;
  const Vector3d Z(0.,0.,1.);
  const Vector3d U = Z.cross(T).normalized();
  const Vector3d V = T.cross(U);

  //this is printed from sympy.printing.cxxcode together with sympy.cse for automatic substitution of common expressions
  auto const xf0 = h;
  auto const xf1 = xf0[2];
  auto const xf2 = magb*qop;
  auto const xf3 = s*xf2;
  auto const xf4 = std::cos(xf3);
  auto const xf5 = 1 - xf4;
  auto const xf6 = gamma*xf5;
  auto const xf7 = T0;
  auto const xf8 = xf7[2];
  auto const xf9 = xf4*xf8;
  auto const xf10 = N0;
  auto const xf11 = xf10[2];
  auto const xf12 = std::sin(xf3);
  auto const xf13 = alpha*xf12;
  auto const xf14 = xf11*xf13;
  auto const xf15 = -xf14 + xf9;
  auto const xf16 = xf1*xf6 + xf15;
  auto const xf17 = std::pow(qop, -2);
  auto const xf18 = xf0[0];
  auto const xf19 = xf7[0];
  auto const xf20 = xf19*xf4;
  auto const xf21 = xf10[0];
  auto const xf22 = xf13*xf21;
  auto const xf23 = xf20 - xf22;
  auto const xf24 = xf18*xf6 + xf23;
  auto const xf25 = std::pow(xf24, 2);
  auto const xf26 = xf0[1];
  auto const xf27 = xf7[1];
  auto const xf28 = xf27*xf4;
  auto const xf29 = xf10[1];
  auto const xf30 = xf13*xf29;
  auto const xf31 = xf28 - xf30;
  auto const xf32 = xf26*xf6 + xf31;
  auto const xf33 = std::pow(xf32, 2);
  auto const xf34 = xf17*xf25 + xf17*xf33;
  auto const xf35 = 1.0/xf34;
  auto const xf36 = xf17*xf35;
  auto const xf37 = 1.0/(std::pow(xf16, 2)*xf36 + 1);
  auto const xf38 = qop*s;
  auto const xf39 = xf12*xf8;
  auto const xf40 = xf12*xf38;
  auto const xf41 = gamma*xf1;
  auto const xf42 = xf38*xf4;
  auto const xf43 = alpha*xf11;
  auto const xf44 = 1.0/std::fabs(qop);
  auto const xf45 = std::pow(xf34, -1.0/2.0);
  auto const xf46 = xf44*xf45;
  auto const xf47 = xf19*xf40;
  auto const xf48 = alpha*xf21;
  auto const xf49 = xf42*xf48;
  auto const xf50 = gamma*xf40;
  auto const xf51 = xf18*xf50;
  auto const xf52 = (1.0/2.0)*xf17;
  auto const xf53 = xf24*xf52;
  auto const xf54 = xf27*xf40;
  auto const xf55 = alpha*xf29;
  auto const xf56 = xf42*xf55;
  auto const xf57 = xf26*xf50;
  auto const xf58 = xf32*xf52;
  auto const xf59 = xf16*xf44/std::pow(xf34, 3.0/2.0);
  auto const xf60 = 1.0/magb;
  auto const xf61 = s*xf4;
  auto const xf62 = xf60*xf61;
  auto const xf63 = 1.0/qop;
  auto const xf64 = xf63/std::pow(magb, 2);
  auto const xf65 = xf12*xf64;
  auto const xf66 = xf62 - xf65;
  auto const xf67 = -gamma*xf62 + gamma*xf65;
  auto const xf68 = -xf12*xf3 + xf5;
  auto const xf69 = xf64*xf68;
  auto const xf70 = -xf16*(xf1*xf67 + xf43*xf69 + xf66*xf8) - xf24*(xf18*xf67 + xf19*xf66 + xf48*xf69) - xf32*(xf26*xf67 + xf27*xf66 + xf55*xf69);
  auto const xf71 = xf12*xf2;
  auto const xf72 = xf2*xf4;
  auto const xf73 = xf19*xf71;
  auto const xf74 = xf48*xf72;
  auto const xf75 = gamma*xf71;
  auto const xf76 = xf18*xf75;
  auto const xf77 = xf27*xf71;
  auto const xf78 = xf55*xf72;
  auto const xf79 = xf26*xf75;
  auto const xf80 = xf37*(xf46*(-xf2*xf39 + xf41*xf71 - xf43*xf72) + xf59*(-xf53*(-2*xf73 - 2*xf74 + 2*xf76) - xf58*(-2*xf77 - 2*xf78 + 2*xf79)));
  auto const xf81 = xf24*xf36;
  auto const xf82 = xf32*xf36;
  auto const xf83 = xf81*(-xf77 - xf78 + xf79) - xf82*(-xf73 - xf74 + xf76);
  auto const xf84 = U;
  auto const xf85 = xf84[0];
  auto const xf86 = s*xf60;
  auto const xf87 = xf5*xf64;
  auto const xf88 = gamma*xf18;
  auto const xf89 = xf12 - xf3;
  auto const xf90 = xf64*xf89;
  auto const xf91 = xf60*xf63;
  auto const xf92 = xf91*(-xf38 + xf42);
  auto const xf93 = -xf19*xf65 + xf20*xf86 - xf22*xf86 + xf48*xf87 + xf88*xf90 - xf88*xf92;
  auto const xf94 = xf84[1];
  auto const xf95 = gamma*xf26;
  auto const xf96 = -xf27*xf65 + xf28*xf86 - xf30*xf86 + xf55*xf87 + xf90*xf95 - xf92*xf95;
  auto const xf97 = xf84[2];
  auto const xf98 = -xf14*xf86 + xf41*xf90 - xf41*xf92 + xf43*xf87 - xf65*xf8 + xf86*xf9;
  auto const xf99 = xf91*(-xf2 + xf72);
  auto const xf100 = xf23 - xf88*xf99;
  auto const xf101 = xf31 - xf95*xf99;
  auto const xf102 = xf15 - xf41*xf99;
  auto const xf103 = xf100*xf85 + xf101*xf94 + xf102*xf97;
  auto const xf104 = V;
  auto const xf105 = xf104[0];
  auto const xf106 = xf104[1];
  auto const xf107 = xf104[2];
  auto const xf108 = xf100*xf105 + xf101*xf106 + xf102*xf107;
  auto const xf109 = xf17*(((qop) > 0) - ((qop) < 0));
  auto const xf110 = magb*s;
  auto const xf111 = xf110*xf12;
  auto const xf112 = xf110*xf4;
  auto const xf113 = std::pow(qop, -3);
  auto const xf114 = xf111*xf19;
  auto const xf115 = xf112*xf48;
  auto const xf116 = xf111*xf88;
  auto const xf117 = xf111*xf27;
  auto const xf118 = xf112*xf55;
  auto const xf119 = xf111*xf95;
  auto const xf120 = xf61*xf63;
  auto const xf121 = xf17*xf60;
  auto const xf122 = xf12*xf121;
  auto const xf123 = xf120 - xf122;
  auto const xf124 = -gamma*xf120 + gamma*xf122;
  auto const xf125 = xf121*xf68;
  auto const xf126 = -xf16*(xf1*xf124 + xf123*xf8 + xf125*xf43) - xf24*(xf123*xf19 + xf124*xf18 + xf125*xf48) - xf32*(xf123*xf27 + xf124*xf26 + xf125*xf55);
  auto const xf127 = xf35*xf44;
  auto const xf128 = s*xf63;
  auto const xf129 = xf121*xf5;
  auto const xf130 = xf121*xf89;
  auto const xf131 = xf91*(-xf110 + xf112);
  auto const xf132 = -xf122*xf19 + xf128*xf20 - xf128*xf22 + xf129*xf48 + xf130*xf88 - xf131*xf88;
  auto const xf133 = -xf122*xf27 + xf128*xf28 - xf128*xf30 + xf129*xf55 + xf130*xf95 - xf131*xf95;
  auto const xf134 = -xf122*xf8 - xf128*xf14 + xf128*xf9 + xf129*xf43 + xf130*xf41 - xf131*xf41;
  auto const dlamdB = xf37*(xf46*(-xf38*xf39 + xf40*xf41 - xf42*xf43) + xf59*(-xf53*(-2*xf47 - 2*xf49 + 2*xf51) - xf58*(-2*xf54 - 2*xf56 + 2*xf57))) + xf70*xf80;
  auto const dphidB = xf70*xf83 + xf81*(-xf54 - xf56 + xf57) - xf82*(-xf47 - xf49 + xf51);
  auto const dxtdB = xf103*xf70 + xf85*xf93 + xf94*xf96 + xf97*xf98;
  auto const dytdB = xf105*xf93 + xf106*xf96 + xf107*xf98 + xf108*xf70;
  auto const dlamdqop = xf126*xf80 + xf37*(-xf109*xf16*xf45 + xf46*(-xf110*xf39 + xf111*xf41 - xf112*xf43) + xf59*(xf113*xf25 + xf113*xf33 - xf53*(-2*xf114 - 2*xf115 + 2*xf116) - xf58*(-2*xf117 - 2*xf118 + 2*xf119)));
  auto const dphidqop = xf126*xf83 + xf127*xf24*(-xf109*xf32 + xf44*(-xf117 - xf118 + xf119)) - xf127*xf32*(-xf109*xf24 + xf44*(-xf114 - xf115 + xf116));
  auto const dxtdqop = xf103*xf126 + xf132*xf85 + xf133*xf94 + xf134*xf97;
  auto const dytdqop = xf105*xf132 + xf106*xf133 + xf107*xf134 + xf108*xf126;


  Matrix<double, 5, 1> dF;
  dF[0] = 0.;
  dF[1] = dlamdB;
  dF[2] = dphidB;
  dF[3] = dxtdB;
  dF[4] = dytdB;
  
//   convert to tesla
  dF *= 2.99792458e-3;
  
  Matrix<double, 5, 1> Fqop;
  Fqop[0] = 1.;
  Fqop[1] = dlamdqop;
  Fqop[2] = dphidqop;
  Fqop[3] = dxtdqop;
  Fqop[4] = dytdqop;
// //   
//   std::cout << "Fqop from sympy:" << std::endl;
//   std::cout << Fqop << std::endl;
  
  return dF;
}

Matrix<double, 5, 6> ResidualGlobalCorrectionMakerBase::curvtransportJacobian(const GlobalTrajectoryParameters& globalSource,
                                                             const GlobalTrajectoryParameters& globalDest,
                                                             const double& s,
                                                             const GlobalVector& bfield) const {
   
  Matrix<double, 5, 6> res;

  const double qop0 = globalSource.signedInverseMomentum();
  const double limit = 5.;
  const double cutCriterion = std::abs(s * qop0);
  
//   std::cout << "cutCriterion " << cutCriterion << std::endl;
//   if (cutCriterion > limit) {
  if (true) {
//   if (false) {
    
    // large s limit, use CMSSW calculation which
    // explicitly uses final position and momentum from propagation
    
//     std::cout << "computing jacobian from cmssw" << std::endl;
    AnalyticalCurvilinearJacobian curv2curv;
    curv2curv.computeFullJacobian(globalSource, globalDest.position(), globalDest.momentum(), bfield, s);
    const AlgebraicMatrix55& curv2curvjac = curv2curv.jacobian();
    const Matrix<double, 5, 5> F = Map<const Matrix<double, 5, 5, RowMajor>>(curv2curvjac.Array());
    
    res.topLeftCorner<5,5>() = F;
    res(0,5) = 0.;
    res.block<4, 1>(1,5) = qop0/bfield.mag()*F.block<4, 1>(1, 0);
  }
  else {
    // small s limit, use sympy-based calculation which
    // uses final position and momentum implicitly from path length + transport equations
    
//     std::cout << "computing jacobian from sympy" << std::endl;
    
    const double M0x = globalSource.position().x();
    const double M0y = globalSource.position().y();
    const double M0z = globalSource.position().z();
    
    const double p0 = globalSource.momentum().mag();
    const GlobalVector W0 = globalSource.momentum()/p0;
    const double W0x = W0.x();
    const double W0y = W0.y();
    const double W0z = W0.z();
    
    const double B = bfield.mag();
    const GlobalVector H = bfield/B;
    const double hx = H.x();
    const double hy = H.y();
    const double hz = H.z();
    
      
    const double x0 = B*s;
    const double x1 = qop0*x0;
    const double x2 = std::cos(x1);
    const double x3 = std::pow(W0z, 2);
    const double x4 = std::pow(W0x, 2);
    const double x5 = std::pow(W0y, 2);
    const double x6 = x4 + x5;
    const double x7 = 1.0/x6;
    const double x8 = std::pow(x3*x7 + 1, -1.0/2.0);
    const double x9 = x2*x8;
    const double x10 = std::sqrt(x6);
    const double x11 = 1.0/x10;
    const double x12 = W0z*x11;
    const double x13 = x12*x9;
    const double x14 = W0y*hx;
    const double x15 = x11*x14;
    const double x16 = W0x*hy;
    const double x17 = x11*x16;
    const double x18 = x15 - x17;
    const double x19 = std::sin(x1);
    const double x20 = x19*x8;
    const double x21 = x18*x20;
    const double x22 = x2 - 1;
    const double x23 = hx*x8;
    const double x24 = W0x*x11;
    const double x25 = hy*x8;
    const double x26 = W0y*x11;
    const double x27 = hz*x8;
    const double x28 = x12*x27 + x23*x24 + x25*x26;
    const double x29 = x22*x28;
    const double x30 = -hz*x29 + x13 - x21;
    const double x31 = x26*x9;
    const double x32 = x24*x27;
    const double x33 = x12*x23 - x32;
    const double x34 = x19*x33;
    const double x35 = hy*x29;
    const double x36 = x31 + x34 - x35;
    const double x37 = x24*x9;
    const double x38 = x12*x25;
    const double x39 = x26*x27;
    const double x40 = x38 - x39;
    const double x41 = x19*x40;
    const double x42 = -hx*x29 + x37 - x41;
    const double x43 = std::pow(x36, 2) + std::pow(x42, 2);
    const double x44 = 1.0/x43;
    const double x45 = 1.0/(std::pow(x30, 2)*x44 + 1);
    const double x46 = x12*x20;
    const double x47 = -x15 + x17;
    const double x48 = x47*x9;
    const double x49 = hz*x28;
    const double x50 = x19*x49;
    const double x51 = std::pow(x43, -1.0/2.0);
    const double x52 = x20*x26;
    const double x53 = x0*x52;
    const double x54 = x0*x2;
    const double x55 = x33*x54;
    const double x56 = hy*x28;
    const double x57 = x0*x19;
    const double x58 = x56*x57;
    const double x59 = (1.0/2.0)*x36;
    const double x60 = x20*x24;
    const double x61 = x0*x60;
    const double x62 = -x38 + x39;
    const double x63 = x54*x62;
    const double x64 = hx*x28;
    const double x65 = x57*x64;
    const double x66 = (1.0/2.0)*x42;
    const double x67 = x30/std::pow(x43, 3.0/2.0);
    const double x68 = x22*x8;
    const double x69 = x18*x68;
    const double x70 = B*qop0;
    const double x71 = W0z*x24;
    const double x72 = W0z*x26;
    const double x73 = -M0x*x71 - M0y*x72 + M0z*(x11*x4 + x11*x5);
    const double x74 = x10*x73;
    const double x75 = x1 - x19;
    const double x76 = x46 + x49*x75 + x69 + x70*x74;
    const double x77 = 1.0/B;
    const double x78 = x77/std::pow(qop0, 2);
    const double x79 = x76*x78;
    const double x80 = x0 - x54;
    const double x81 = 1.0/qop0;
    const double x82 = x77*x81;
    const double x83 = x82*(B*x74 + x0*x13 - x0*x21 + x49*x80);
    const double x84 = -M0x*x26 + M0y*x24;
    const double x85 = W0z*x73;
    const double x86 = W0x*x84 - W0y*x85;
    const double x87 = B*x86;
    const double x88 = qop0*x87 + x10*(-x22*x33 + x52 + x56*x75);
    const double x89 = x11*x78;
    const double x90 = x88*x89;
    const double x91 = x10*(x0*x31 + x0*x34 + x56*x80) + x87;
    const double x92 = x11*x82;
    const double x93 = x91*x92;
    const double x94 = W0x*x85 + W0y*x84;
    const double x95 = B*x94;
    const double x96 = -qop0*x95 + x10*(x22*x40 + x60 + x64*x75);
    const double x97 = x89*x96;
    const double x98 = x10*(x0*x37 - x0*x41 + x64*x80) - x95;
    const double x99 = x92*x98;
    const double x100 = -x30*(-x79 + x83) - x36*(-x90 + x93) - x42*(-x97 + x99);
    const double x101 = x52*x70;
    const double x102 = x2*x70;
    const double x103 = x102*x33;
    const double x104 = x19*x70;
    const double x105 = x104*x56;
    const double x106 = x60*x70;
    const double x107 = x102*x62;
    const double x108 = x104*x64;
    const double x109 = x45*(x51*(-x46*x70 + x48*x70 + x50*x70) + x67*(-x59*(-2*x101 + 2*x103 + 2*x105) - x66*(-2*x106 + 2*x107 + 2*x108)));
    const double x110 = W0z*x7;
    const double x111 = W0x*x110;
    const double x112 = W0y*x110;
    const double x113 = -x111*x23 - x112*x25 + x27;
    const double x114 = hz*x113;
    const double x115 = x112*x9;
    const double x116 = x111*x27 + x23;
    const double x117 = x116*x19;
    const double x118 = x113*x22;
    const double x119 = hy*x118;
    const double x120 = x111*x9;
    const double x121 = x112*x27;
    const double x122 = x19*(-x121 - x25);
    const double x123 = hx*x118;
    const double x124 = x114*x75 - x12*x69 + x20;
    const double x125 = x30*x82;
    const double x126 = x113*x75;
    const double x127 = hx*x126 - x111*x20 + x22*(x121 + x25);
    const double x128 = x42*x82;
    const double x129 = 1 - x2;
    const double x130 = hy*x126 - x112*x20 + x116*x129;
    const double x131 = x36*x82;
    const double x132 = -x124*x125 - x127*x128 - x130*x131;
    const double x133 = W0x*hx;
    const double x134 = x11*x133;
    const double x135 = W0y*hy;
    const double x136 = x11*x135;
    const double x137 = -x23*x26 + x24*x25;
    const double x138 = hz*x137;
    const double x139 = x19*x39;
    const double x140 = x137*x22;
    const double x141 = hy*x140;
    const double x142 = x19*x32;
    const double x143 = hx*x140;
    const double x144 = x138*x75 + x68*(x134 + x136);
    const double x145 = x137*x75;
    const double x146 = hy*x145 + x129*x39 + x60;
    const double x147 = hx*x145 - x22*x32 - x52;
    const double x148 = -x125*x144 - x128*x147 - x131*x146;
    const double x149 = -x24*x36 + x26*x42;
    const double x150 = -x10*x30 + x36*x72 + x42*x71;
    const double x151 = qop0*s;
    const double x152 = x151*x52;
    const double x153 = x151*x2;
    const double x154 = x153*x33;
    const double x155 = x151*x19;
    const double x156 = x155*x56;
    const double x157 = x151*x60;
    const double x158 = x153*x62;
    const double x159 = x155*x64;
    const double x160 = x81/std::pow(B, 2);
    const double x161 = x160*x76;
    const double x162 = x151 - x153;
    const double x163 = x82*(qop0*x74 + x13*x151 - x151*x21 + x162*x49);
    const double x164 = x11*x160;
    const double x165 = x164*x88;
    const double x166 = qop0*x86 + x10*(x151*x31 + x151*x34 + x162*x56);
    const double x167 = x166*x92;
    const double x168 = x164*x96;
    const double x169 = -qop0*x94 + x10*(x151*x37 - x151*x41 + x162*x64);
    const double x170 = x169*x92;
    const double x171 = -x30*(-x161 + x163) - x36*(-x165 + x167) - x42*(-x168 + x170);
    const double x172 = x42*x44;
    const double x173 = -x31;
    const double x174 = x44*(x173 - x34 + x35);
    const double x175 = x172*(-x101 + x103 + x105) + x174*(-x106 + x107 + x108);
    const double x176 = x22*(W0z*hz + x133 + x135);
    const double x177 = W0y*x2 - hy*x176 + x19*(-W0x*hz + W0z*hx);
    const double x178 = x3 + x6;
    const double x179 = 1.0/x178;
    const double x180 = W0x*x2 - hx*x176 + x19*(W0y*hz - W0z*hy);
    const double x181 = x179*std::pow(x180, 2);
    const double x182 = std::pow(x177, 2)*x179;
    const double x183 = std::pow(x181 + x182, -1.0/2.0);
    const double x184 = x183/std::sqrt(x178*x7);
    const double x185 = x184*x7;
    const double x186 = x177*x185;
    const double x187 = x186*x96;
    const double x188 = x180*x185;
    const double x189 = x188*x88;
    const double x190 = x188*x82;
    const double x191 = x186*x82;
    const double x192 = -x102 + x70;
    const double x193 = x192*x56 + x31*x70 + x34*x70;
    const double x194 = x180*x184;
    const double x195 = x194*x92;
    const double x196 = x192*x64 + x37*x70 - x41*x70;
    const double x197 = x177*x184;
    const double x198 = x197*x92;
    const double x199 = x193*x195 - x196*x198;
    const double x200 = x179*x183*(W0z*x2 - hz*x176 + x19*(-x14 + x16));
    const double x201 = x180*x200;
    const double x202 = x177*x200;
    const double x203 = x181*x183 + x182*x183;
    const double x204 = x202*x82;
    const double x205 = x201*x82;
    const double x206 = x203*x82;
    const double x207 = -x193*x204 - x196*x205 + x206*(x13*x70 + x192*x49 - x21*x70);
    const double dqopdqop0 = 1;
    const double dqopdlam0 = 0;
    const double dqopdphi0 = 0;
    const double dqopdxt0 = 0;
    const double dqopdyt0 = 0;
    const double dqopdB = 0;
    const double dlamdqop0 = x100*x109 + x45*(x51*(-x0*x46 + x0*x48 + x0*x50) + x67*(-x59*(-2*x53 + 2*x55 + 2*x58) - x66*(-2*x61 + 2*x63 + 2*x65)));
    const double dlamdlam0 = x109*x132 + x45*(x51*(-x114*x22 - x46*x47 + x9) + x67*(-x59*(-2*x115 + 2*x117 - 2*x119) - x66*(-2*x120 + 2*x122 - 2*x123)));
    const double dlamdphi0 = x109*x148 + x45*(x51*(-x138*x22 + x20*(-x134 - x136)) + x67*(-x59*(2*x139 - 2*x141 + 2*x37) - x66*(2*x142 - 2*x143 - 2*x31)));
    const double dlamdxt0 = x109*x149;
    const double dlamdyt0 = x109*x150;
    const double dlamdB = x109*x171 + x45*(x51*(-x151*x46 + x151*x48 + x151*x50) + x67*(-x59*(-2*x152 + 2*x154 + 2*x156) - x66*(-2*x157 + 2*x158 + 2*x159)));
    const double dphidqop0 = x100*x175 + x172*(-x53 + x55 + x58) + x174*(-x61 + x63 + x65);
    const double dphidlam0 = x132*x175 + x172*(-x115 + x117 - x119) + x174*(-x120 + x122 - x123);
    const double dphidphi0 = x148*x175 + x172*(x139 - x141 + x37) + x174*(x142 - x143 + x173);
    const double dphidxt0 = x149*x175;
    const double dphidyt0 = x150*x175;
    const double dphidB = x171*x175 + x172*(-x152 + x154 + x156) + x174*(-x157 + x158 + x159);
    const double dxtdqop0 = x100*x199 + x187*x78 - x189*x78 + x190*x91 - x191*x98;
    const double dxtdlam0 = -x127*x198 + x130*x195 + x132*x199;
    const double dxtdphi0 = x146*x195 - x147*x198 + x148*x199;
    const double dxtdxt0 = W0x*x188 + W0y*x186 + x149*x199;
    const double dxtdyt0 = x111*x197 - x112*x194 + x150*x199;
    const double dxtdB = x160*x187 - x160*x189 + x166*x190 - x169*x191 + x171*x199;
    const double dytdqop0 = x100*x207 + x201*x97 - x201*x99 + x202*x90 - x202*x93 - x203*x79 + x203*x83;
    const double dytdlam0 = x124*x206 - x127*x205 - x130*x204 + x132*x207;
    const double dytdphi0 = x144*x206 - x146*x204 - x147*x205 + x148*x207;
    const double dytdxt0 = x149*x207 + x201*x26 - x202*x24;
    const double dytdyt0 = x10*x203 + x150*x207 + x201*x71 + x202*x72;
    const double dytdB = -x161*x203 + x163*x203 + x165*x202 - x167*x202 + x168*x201 - x170*x201 + x171*x207;
    
    res(0,0) = dqopdqop0;
    res(0,1) = dqopdlam0;
    res(0,2) = dqopdphi0;
    res(0,3) = dqopdxt0;
    res(0,4) = dqopdyt0;
    res(0,5) = dqopdB;
    res(1,0) = dlamdqop0;
    res(1,1) = dlamdlam0;
    res(1,2) = dlamdphi0;
    res(1,3) = dlamdxt0;
    res(1,4) = dlamdyt0;
    res(1,5) = dlamdB;
    res(2,0) = dphidqop0;
    res(2,1) = dphidlam0;
    res(2,2) = dphidphi0;
    res(2,3) = dphidxt0;
    res(2,4) = dphidyt0;
    res(2,5) = dphidB;
    res(3,0) = dxtdqop0;
    res(3,1) = dxtdlam0;
    res(3,2) = dxtdphi0;
    res(3,3) = dxtdxt0;
    res(3,4) = dxtdyt0;
    res(3,5) = dxtdB;
    res(4,0) = dytdqop0;
    res(4,1) = dytdlam0;
    res(4,2) = dytdphi0;
    res(4,3) = dytdxt0;
    res(4,4) = dytdyt0;
    res(4,5) = dytdB;
  }

  // convert to tesla for B field gradient
  res.col(5) *= 2.99792458e-3;
  
  return res;
                                                               
}

Matrix<double, 5, 6> ResidualGlobalCorrectionMakerBase::hybrid2curvJacobian(const FreeTrajectoryState &state) const {
  const GlobalTrajectoryParameters &globalSource = state.parameters();
  const GlobalVector &bfield = state.parameters().magneticFieldInInverseGeV();
  
  CurvilinearTrajectoryParameters curvparms(globalSource.position(), globalSource.momentum(), globalSource.charge());

  const double qop0 = globalSource.signedInverseMomentum();
  const double lam0 = curvparms.lambda();
  const double phi0 = curvparms.phi();

  const double x0 = globalSource.position().x();
  const double y0 = globalSource.position().y();
  const double z0 = globalSource.position().z();
  
  const double p0 = globalSource.momentum().mag();
  const GlobalVector W0 = globalSource.momentum()/p0;
  const double W0x = W0.x();
  const double W0y = W0.y();
  const double W0z = W0.z();
  
  const double B = bfield.mag();
  const GlobalVector H = bfield/B;
  const double hx = H.x();
  const double hy = H.y();
  const double hz = H.z();
  
  const double xf0 = std::cos(lam0);
  const double xf1 = std::pow(xf0, 2);
  const double xf2 = std::sqrt(xf1);
  const double xf3 = 2*phi0;
  const double xf4 = std::sin(xf3);
  const double xf5 = std::cos(xf3);
  const double xf6 = B*qop0;
  const double xf7 = (1.0/4.0)*xf6*std::sqrt(2*std::cos(2*lam0) + 2);
  const double xf8 = std::tan(lam0);
  const double xf9 = std::sin(phi0);
  const double xf10 = std::cos(phi0);
  const double xf11 = std::sin(lam0);
  const double xf12 = xf6*(hx*xf10*xf11 + hy*xf11*xf9 - hz*xf0);
  const double xf13 = std::pow(W0x, 2) + std::pow(W0y, 2);
  const double xf14 = std::pow(xf13, -1.0/2.0);
  const double xf15 = W0x*xf9;
  const double xf16 = W0y*xf10;
  const double xf17 = xf1*(xf15 - xf16);
  const double xf18 = W0x*W0z;
  const double xf19 = W0y*W0z;
  const double xf20 = xf0*xf10*xf18 + xf0*xf19*xf9 - xf11*xf13;
  const double xf21 = xf0*xf20;
  const double dqopdqop0 = 1;
  const double dqopdlam0 = 0;
  const double dqopdphi0 = 0;
  const double dqopdx0 = 0;
  const double dqopdy0 = 0;
  const double dqopdz0 = 0;
  const double dlamdqop0 = 0;
  const double dlamdlam0 = xf2/xf0;
  const double dlamdphi0 = 0;
  const double dlamdx0 = xf7*(hx*xf4 - hy*xf5 - hy);
  const double dlamdy0 = xf7*(-hx*xf5 + hx - hy*xf4);
  const double dlamdz0 = xf2*xf6*xf8*(hx*xf9 - hy*xf10);
  const double dphidqop0 = 0;
  const double dphidlam0 = 0;
  const double dphidphi0 = 1;
  const double dphidx0 = -xf10*xf12;
  const double dphidy0 = -xf12*xf9;
  const double dphidz0 = -xf12*xf8;
  const double dxtdqop0 = 0;
  const double dxtdlam0 = 0;
  const double dxtdphi0 = 0;
  const double dxtdx0 = -xf14*(W0y + xf10*xf17);
  const double dxtdy0 = xf14*(W0x - xf17*xf9);
  const double dxtdz0 = xf0*xf11*xf14*(-xf15 + xf16);
  const double dytdqop0 = 0;
  const double dytdlam0 = 0;
  const double dytdphi0 = 0;
  const double dytdx0 = xf14*(xf10*xf21 - xf18);
  const double dytdy0 = xf14*(-xf19 + xf21*xf9);
  const double dytdz0 = xf14*(xf11*xf20 + xf13);
  Matrix<double, 5, 6> res;
  res(0,0) = dqopdqop0;
  res(0,1) = dqopdlam0;
  res(0,2) = dqopdphi0;
  res(0,3) = dqopdx0;
  res(0,4) = dqopdy0;
  res(0,5) = dqopdz0;
  res(1,0) = dlamdqop0;
  res(1,1) = dlamdlam0;
  res(1,2) = dlamdphi0;
  res(1,3) = dlamdx0;
  res(1,4) = dlamdy0;
  res(1,5) = dlamdz0;
  res(2,0) = dphidqop0;
  res(2,1) = dphidlam0;
  res(2,2) = dphidphi0;
  res(2,3) = dphidx0;
  res(2,4) = dphidy0;
  res(2,5) = dphidz0;
  res(3,0) = dxtdqop0;
  res(3,1) = dxtdlam0;
  res(3,2) = dxtdphi0;
  res(3,3) = dxtdx0;
  res(3,4) = dxtdy0;
  res(3,5) = dxtdz0;
  res(4,0) = dytdqop0;
  res(4,1) = dytdlam0;
  res(4,2) = dytdphi0;
  res(4,3) = dytdx0;
  res(4,4) = dytdy0;
  res(4,5) = dytdz0;
  
  return res;

}

Matrix<double, 5, 6> ResidualGlobalCorrectionMakerBase::hybrid2curvJacobianD(const Matrix<double, 7, 1> &state, const MagneticField *field, double dBz) const {
  
  const GlobalPoint pos(state[0], state[1], state[2]);  
  const GlobalVector &bfield = field->inInverseGeV(pos);
  const Matrix<double, 3, 1> Bv(bfield.x(), bfield.y(), double(bfield.z()) + 2.99792458e-3*dBz);
  
  const double q = state[6];
  
  const double qop0 = q/state.segment<3>(3).norm();
  const double lam0 = std::atan(state[5]/std::sqrt(state[3]*state[3] + state[4]*state[4]));
  const double phi0 = std::atan2(state[4], state[3]);
  
  const Matrix<double, 3, 1> W0 = state.segment<3>(3).normalized();
  const double W0x = W0[0];
  const double W0y = W0[1];
  const double W0z = W0[2];  

  const double x0 = state[0];
  const double y0 = state[1];
  const double z0 = state[2];
  
  const double B = Bv.norm();
  const Matrix<double, 3, 1> H = Bv.normalized();
  const double hx = H[0];
  const double hy = H[1];
  const double hz = H[2];
  
  const double xf0 = std::cos(lam0);
  const double xf1 = std::pow(xf0, 2);
  const double xf2 = std::sqrt(xf1);
  const double xf3 = 2*phi0;
  const double xf4 = std::sin(xf3);
  const double xf5 = std::cos(xf3);
  const double xf6 = B*qop0;
  const double xf7 = (1.0/4.0)*xf6*std::sqrt(2*std::cos(2*lam0) + 2);
  const double xf8 = std::tan(lam0);
  const double xf9 = std::sin(phi0);
  const double xf10 = std::cos(phi0);
  const double xf11 = std::sin(lam0);
  const double xf12 = xf6*(hx*xf10*xf11 + hy*xf11*xf9 - hz*xf0);
  const double xf13 = std::pow(W0x, 2) + std::pow(W0y, 2);
  const double xf14 = std::pow(xf13, -1.0/2.0);
  const double xf15 = W0x*xf9;
  const double xf16 = W0y*xf10;
  const double xf17 = xf1*(xf15 - xf16);
  const double xf18 = W0x*W0z;
  const double xf19 = W0y*W0z;
  const double xf20 = xf0*xf10*xf18 + xf0*xf19*xf9 - xf11*xf13;
  const double xf21 = xf0*xf20;
  const double dqopdqop0 = 1;
  const double dqopdlam0 = 0;
  const double dqopdphi0 = 0;
  const double dqopdx0 = 0;
  const double dqopdy0 = 0;
  const double dqopdz0 = 0;
  const double dlamdqop0 = 0;
  const double dlamdlam0 = xf2/xf0;
  const double dlamdphi0 = 0;
  const double dlamdx0 = xf7*(hx*xf4 - hy*xf5 - hy);
  const double dlamdy0 = xf7*(-hx*xf5 + hx - hy*xf4);
  const double dlamdz0 = xf2*xf6*xf8*(hx*xf9 - hy*xf10);
  const double dphidqop0 = 0;
  const double dphidlam0 = 0;
  const double dphidphi0 = 1;
  const double dphidx0 = -xf10*xf12;
  const double dphidy0 = -xf12*xf9;
  const double dphidz0 = -xf12*xf8;
  const double dxtdqop0 = 0;
  const double dxtdlam0 = 0;
  const double dxtdphi0 = 0;
  const double dxtdx0 = -xf14*(W0y + xf10*xf17);
  const double dxtdy0 = xf14*(W0x - xf17*xf9);
  const double dxtdz0 = xf0*xf11*xf14*(-xf15 + xf16);
  const double dytdqop0 = 0;
  const double dytdlam0 = 0;
  const double dytdphi0 = 0;
  const double dytdx0 = xf14*(xf10*xf21 - xf18);
  const double dytdy0 = xf14*(-xf19 + xf21*xf9);
  const double dytdz0 = xf14*(xf11*xf20 + xf13);
  Matrix<double, 5, 6> res;
  res(0,0) = dqopdqop0;
  res(0,1) = dqopdlam0;
  res(0,2) = dqopdphi0;
  res(0,3) = dqopdx0;
  res(0,4) = dqopdy0;
  res(0,5) = dqopdz0;
  res(1,0) = dlamdqop0;
  res(1,1) = dlamdlam0;
  res(1,2) = dlamdphi0;
  res(1,3) = dlamdx0;
  res(1,4) = dlamdy0;
  res(1,5) = dlamdz0;
  res(2,0) = dphidqop0;
  res(2,1) = dphidlam0;
  res(2,2) = dphidphi0;
  res(2,3) = dphidx0;
  res(2,4) = dphidy0;
  res(2,5) = dphidz0;
  res(3,0) = dxtdqop0;
  res(3,1) = dxtdlam0;
  res(3,2) = dxtdphi0;
  res(3,3) = dxtdx0;
  res(3,4) = dxtdy0;
  res(3,5) = dxtdz0;
  res(4,0) = dytdqop0;
  res(4,1) = dytdlam0;
  res(4,2) = dytdphi0;
  res(4,3) = dytdx0;
  res(4,4) = dytdy0;
  res(4,5) = dytdz0;
  
  return res;

}


Matrix<double, 5, 7> ResidualGlobalCorrectionMakerBase::hybrid2localTransportJacobian(const FreeTrajectoryState& start,
                                              const std::pair<TrajectoryStateOnSurface, double>& propresult) const {
                                            
  // hybrid to curvilinear
  Matrix<double, 5, 7> FdF = hybrid2curvTransportJacobian(start, propresult);
  
  // compute curvilinear to local jacobian at destination
  const TrajectoryStateOnSurface &state1 = propresult.first;
  JacobianCurvilinearToLocal curv2local(state1.surface(), state1.localParameters(), *state1.magneticField());
  const AlgebraicMatrix55& curv2localjac = curv2local.jacobian();
  const Matrix<double, 5, 5> H1 = Map<const Matrix<double, 5, 5, RowMajor>>(curv2localjac.Array());
  
  return H1*FdF;
                                                
}

Matrix<double, 5, 7> ResidualGlobalCorrectionMakerBase::hybrid2curvTransportJacobian(const FreeTrajectoryState& start,
                                              const std::pair<TrajectoryStateOnSurface, double>& propresult) const {
        
  const FreeTrajectoryState& end = *propresult.first.freeState();
  const TrajectoryStateOnSurface& proptsos = propresult.first;
  
  const FreeTrajectoryState& state0 = start;
  const FreeTrajectoryState& state1 = end;
  const GlobalVector& h = start.parameters().magneticFieldInInverseGeV();
//   const GlobalVector h = 0.5*(start.parameters().magneticFieldInInverseGeV() + end.parameters().magneticFieldInInverseGeV());
  const double s = propresult.second;
  
  const Matrix<double, 5, 7> FdF = hybrid2curvTransportJacobian(state0.parameters(), state1.parameters(), s, h);
  
//   const Matrix<double, 5, 7> FdFvar = hybrid2curvTransportJacobianVar(state0.parameters(), state1.parameters(), s, h);
  
//   std::cout << "FdF hybrid:" << std::endl;
//   std::cout << FdF << std::endl;
  
//   std::cout << "FdF hybrid var:" << std::endl;
//   std::cout << FdFvar << std::endl;


  return FdF;
//   return FdFvar;
}

Matrix<double, 5, 7> ResidualGlobalCorrectionMakerBase::hybrid2curvTransportJacobian(const GlobalTrajectoryParameters& globalSource,
                                                             const GlobalTrajectoryParameters& globalDest,
                                                             const double& s,
                                                             const GlobalVector& bfield) const {

  const double qop0 = globalSource.signedInverseMomentum();

    
  const double x0 = globalSource.position().x();
  const double y0 = globalSource.position().y();
  const double z0 = globalSource.position().z();
  
  const double p0 = globalSource.momentum().mag();
  const GlobalVector W0 = globalSource.momentum()/p0;
  const double W0x = W0.x();
  const double W0y = W0.y();
  const double W0z = W0.z();
  
  const double B = bfield.mag();
  const GlobalVector H = bfield/B;
  const double hx = H.x();
  const double hy = H.y();
  const double hz = H.z();
      
  const double xf0 = B*s;
  const double xf1 = qop0*xf0;
  const double xf2 = std::cos(xf1);
  const double xf3 = std::pow(W0z, 2);
  const double xf4 = std::pow(W0x, 2) + std::pow(W0y, 2);
  const double xf5 = 1.0/xf4;
  const double xf6 = std::pow(xf3*xf5 + 1, -1.0/2.0);
  const double xf7 = xf2*xf6;
  const double xf8 = std::pow(xf4, -1.0/2.0);
  const double xf9 = W0z*xf8;
  const double xf10 = xf7*xf9;
  const double xf11 = W0y*hx;
  const double xf12 = xf11*xf8;
  const double xf13 = W0x*hy;
  const double xf14 = xf13*xf8;
  const double xf15 = xf12 - xf14;
  const double xf16 = std::sin(xf1);
  const double xf17 = xf16*xf6;
  const double xf18 = xf15*xf17;
  const double xf19 = xf2 - 1;
  const double xf20 = hx*xf6;
  const double xf21 = xf20*xf8;
  const double xf22 = hy*xf6;
  const double xf23 = W0y*xf8;
  const double xf24 = hz*xf6;
  const double xf25 = xf24*xf8;
  const double xf26 = W0x*xf21 + W0z*xf25 + xf22*xf23;
  const double xf27 = xf19*xf26;
  const double xf28 = hz*xf27;
  const double xf29 = xf10 - xf18 - xf28;
  const double xf30 = xf23*xf7;
  const double xf31 = W0x*xf25;
  const double xf32 = W0z*xf21 - xf31;
  const double xf33 = xf16*xf32;
  const double xf34 = hy*xf27;
  const double xf35 = xf30 + xf33 - xf34;
  const double xf36 = W0x*xf8;
  const double xf37 = xf36*xf7;
  const double xf38 = xf22*xf9;
  const double xf39 = W0y*xf25;
  const double xf40 = xf38 - xf39;
  const double xf41 = xf16*xf40;
  const double xf42 = hx*xf27;
  const double xf43 = xf37 - xf41 - xf42;
  const double xf44 = std::pow(xf35, 2) + std::pow(xf43, 2);
  const double xf45 = 1.0/xf44;
  const double xf46 = 1.0/(std::pow(xf29, 2)*xf45 + 1);
  const double xf47 = xf17*xf9;
  const double xf48 = -xf12 + xf14;
  const double xf49 = xf48*xf7;
  const double xf50 = hz*xf26;
  const double xf51 = xf16*xf50;
  const double xf52 = std::pow(xf44, -1.0/2.0);
  const double xf53 = xf17*xf23;
  const double xf54 = xf0*xf53;
  const double xf55 = xf0*xf2;
  const double xf56 = xf32*xf55;
  const double xf57 = hy*xf26;
  const double xf58 = xf0*xf16;
  const double xf59 = xf57*xf58;
  const double xf60 = (1.0/2.0)*xf35;
  const double xf61 = xf17*xf36;
  const double xf62 = xf0*xf61;
  const double xf63 = -xf38 + xf39;
  const double xf64 = xf55*xf63;
  const double xf65 = hx*xf26;
  const double xf66 = xf58*xf65;
  const double xf67 = (1.0/2.0)*xf43;
  const double xf68 = xf29/std::pow(xf44, 3.0/2.0);
  const double xf69 = B*z0;
  const double xf70 = xf19*xf6;
  const double xf71 = xf15*xf70;
  const double xf72 = xf1 - xf16;
  const double xf73 = qop0*xf69 + xf47 + xf50*xf72 + xf71;
  const double xf74 = 1.0/B;
  const double xf75 = xf74/std::pow(qop0, 2);
  const double xf76 = xf73*xf75;
  const double xf77 = xf0 - xf55;
  const double xf78 = 1.0/qop0;
  const double xf79 = xf74*xf78;
  const double xf80 = xf79*(xf0*xf10 - xf0*xf18 + xf50*xf77 + xf69);
  const double xf81 = B*y0;
  const double xf82 = qop0*xf81 - xf19*xf32 + xf53 + xf57*xf72;
  const double xf83 = xf75*xf82;
  const double xf84 = xf79*(xf0*xf30 + xf0*xf33 + xf57*xf77 + xf81);
  const double xf85 = B*x0;
  const double xf86 = qop0*xf85 + xf19*xf40 + xf61 + xf65*xf72;
  const double xf87 = xf75*xf86;
  const double xf88 = xf79*(xf0*xf37 - xf0*xf41 + xf65*xf77 + xf85);
  const double xf89 = -xf29*(-xf76 + xf80) - xf35*(-xf83 + xf84) - xf43*(-xf87 + xf88);
  const double xf90 = B*qop0;
  const double xf91 = xf53*xf90;
  const double xf92 = xf2*xf90;
  const double xf93 = xf32*xf92;
  const double xf94 = xf16*xf90;
  const double xf95 = xf57*xf94;
  const double xf96 = xf61*xf90;
  const double xf97 = xf63*xf92;
  const double xf98 = xf65*xf94;
  const double xf99 = xf46*(xf52*(-xf47*xf90 + xf49*xf90 + xf51*xf90) + xf68*(-xf60*(-2*xf91 + 2*xf93 + 2*xf95) - xf67*(-2*xf96 + 2*xf97 + 2*xf98)));
  const double xf100 = W0z*xf5;
  const double xf101 = W0x*xf100;
  const double xf102 = W0y*xf100;
  const double xf103 = -xf101*xf20 - xf102*xf22 + xf24;
  const double xf104 = hz*xf103;
  const double xf105 = xf102*xf7;
  const double xf106 = xf101*xf24 + xf20;
  const double xf107 = xf106*xf16;
  const double xf108 = xf103*xf19;
  const double xf109 = hy*xf108;
  const double xf110 = xf101*xf7;
  const double xf111 = xf102*xf24;
  const double xf112 = xf16*(-xf111 - xf22);
  const double xf113 = hx*xf108;
  const double xf114 = xf104*xf72 + xf17 - xf71*xf9;
  const double xf115 = xf29*xf79;
  const double xf116 = xf103*xf72;
  const double xf117 = hx*xf116 - xf101*xf17 + xf19*(xf111 + xf22);
  const double xf118 = xf43*xf79;
  const double xf119 = 1 - xf2;
  const double xf120 = hy*xf116 - xf102*xf17 + xf106*xf119;
  const double xf121 = xf35*xf79;
  const double xf122 = -xf114*xf115 - xf117*xf118 - xf120*xf121;
  const double xf123 = W0x*hx;
  const double xf124 = xf123*xf8;
  const double xf125 = W0y*hy;
  const double xf126 = xf125*xf8;
  const double xf127 = -W0y*xf21 + xf22*xf36;
  const double xf128 = hz*xf127;
  const double xf129 = xf16*xf39;
  const double xf130 = xf127*xf19;
  const double xf131 = hy*xf130;
  const double xf132 = xf16*xf31;
  const double xf133 = hx*xf130;
  const double xf134 = xf128*xf72 + xf70*(xf124 + xf126);
  const double xf135 = xf127*xf72;
  const double xf136 = hy*xf135 + xf119*xf39 + xf61;
  const double xf137 = hx*xf135 - xf19*xf31 - xf53;
  const double xf138 = -xf115*xf134 - xf118*xf137 - xf121*xf136;
  const double xf139 = -xf37 + xf41 + xf42;
  const double xf140 = -xf30;
  const double xf141 = xf140 - xf33 + xf34;
  const double xf142 = -xf10 + xf18 + xf28;
  const double xf143 = qop0*s;
  const double xf144 = xf143*xf53;
  const double xf145 = xf143*xf2;
  const double xf146 = xf145*xf32;
  const double xf147 = xf143*xf16;
  const double xf148 = xf147*xf57;
  const double xf149 = xf143*xf61;
  const double xf150 = xf145*xf63;
  const double xf151 = xf147*xf65;
  const double xf152 = xf78/std::pow(B, 2);
  const double xf153 = xf152*xf73;
  const double xf154 = xf143 - xf145;
  const double xf155 = xf79*(qop0*z0 + xf10*xf143 - xf143*xf18 + xf154*xf50);
  const double xf156 = xf152*xf82;
  const double xf157 = xf79*(qop0*y0 + xf143*xf30 + xf143*xf33 + xf154*xf57);
  const double xf158 = xf152*xf86;
  const double xf159 = xf79*(qop0*x0 + xf143*xf37 - xf143*xf41 + xf154*xf65);
  const double xf160 = -xf29*(-xf153 + xf155) - xf35*(-xf156 + xf157) - xf43*(-xf158 + xf159);
  const double xf161 = xf43*xf45;
  const double xf162 = xf141*xf45;
  const double xf163 = xf161*(-xf91 + xf93 + xf95) + xf162*(-xf96 + xf97 + xf98);
  const double xf164 = xf3 + xf4;
  const double xf165 = 1.0/xf164;
  const double xf166 = xf19*(W0z*hz + xf123 + xf125);
  const double xf167 = W0x*xf2 - hx*xf166 + xf16*(W0y*hz - W0z*hy);
  const double xf168 = xf165*std::pow(xf167, 2);
  const double xf169 = W0y*xf2 - hy*xf166 + xf16*(-W0x*hz + W0z*hx);
  const double xf170 = xf165*std::pow(xf169, 2);
  const double xf171 = std::pow(xf168 + xf170, -1.0/2.0);
  const double xf172 = xf169*xf171;
  const double xf173 = xf8/std::sqrt(xf164*xf5);
  const double xf174 = xf172*xf173;
  const double xf175 = xf167*xf171;
  const double xf176 = xf173*xf175;
  const double xf177 = xf90 - xf92;
  const double xf178 = xf177*xf57 + xf30*xf90 + xf33*xf90;
  const double xf179 = xf176*xf79;
  const double xf180 = xf177*xf65 + xf37*xf90 - xf41*xf90;
  const double xf181 = xf174*xf79;
  const double xf182 = xf178*xf179 - xf180*xf181;
  const double xf183 = xf165*(W0z*xf2 - hz*xf166 + xf16*(-xf11 + xf13));
  const double xf184 = xf175*xf183;
  const double xf185 = xf172*xf183;
  const double xf186 = xf168*xf171 + xf170*xf171;
  const double xf187 = xf185*xf79;
  const double xf188 = xf184*xf79;
  const double xf189 = xf186*xf79;
  const double xf190 = -xf178*xf187 - xf180*xf188 + xf189*(xf10*xf90 + xf177*xf50 - xf18*xf90);

  const double dqopdqop0 = 1;
  const double dqopdlam0 = 0;
  const double dqopdphi0 = 0;
  const double dqopdx0 = 0;
  const double dqopdy0 = 0;
  const double dqopdz0 = 0;
  const double dqopdB = 0;
  const double dlamdqop0 = xf46*(xf52*(-xf0*xf47 + xf0*xf49 + xf0*xf51) + xf68*(-xf60*(-2*xf54 + 2*xf56 + 2*xf59) - xf67*(-2*xf62 + 2*xf64 + 2*xf66))) + xf89*xf99;
  const double dlamdlam0 = xf122*xf99 + xf46*(xf52*(-xf104*xf19 - xf47*xf48 + xf7) + xf68*(-xf60*(-2*xf105 + 2*xf107 - 2*xf109) - xf67*(-2*xf110 + 2*xf112 - 2*xf113)));
  const double dlamdphi0 = xf138*xf99 + xf46*(xf52*(-xf128*xf19 + xf17*(-xf124 - xf126)) + xf68*(-xf60*(2*xf129 - 2*xf131 + 2*xf37) - xf67*(2*xf132 - 2*xf133 - 2*xf30)));
  const double dlamdx0 = xf139*xf99;
  const double dlamdy0 = xf141*xf99;
  const double dlamdz0 = xf142*xf99;
  const double dlamdB = xf160*xf99 + xf46*(xf52*(-xf143*xf47 + xf143*xf49 + xf143*xf51) + xf68*(-xf60*(-2*xf144 + 2*xf146 + 2*xf148) - xf67*(-2*xf149 + 2*xf150 + 2*xf151)));
  const double dphidqop0 = xf161*(-xf54 + xf56 + xf59) + xf162*(-xf62 + xf64 + xf66) + xf163*xf89;
  const double dphidlam0 = xf122*xf163 + xf161*(-xf105 + xf107 - xf109) + xf162*(-xf110 + xf112 - xf113);
  const double dphidphi0 = xf138*xf163 + xf161*(xf129 - xf131 + xf37) + xf162*(xf132 - xf133 + xf140);
  const double dphidx0 = xf139*xf163;
  const double dphidy0 = xf141*xf163;
  const double dphidz0 = xf142*xf163;
  const double dphidB = xf160*xf163 + xf161*(-xf144 + xf146 + xf148) + xf162*(-xf149 + xf150 + xf151);
  const double dxtdqop0 = xf174*xf87 - xf174*xf88 - xf176*xf83 + xf176*xf84 + xf182*xf89;
  const double dxtdlam0 = -xf117*xf181 + xf120*xf179 + xf122*xf182;
  const double dxtdphi0 = xf136*xf179 - xf137*xf181 + xf138*xf182;
  const double dxtdx0 = xf139*xf182 - xf174;
  const double dxtdy0 = xf141*xf182 + xf176;
  const double dxtdz0 = xf142*xf182;
  const double dxtdB = -xf156*xf176 + xf157*xf176 + xf158*xf174 - xf159*xf174 + xf160*xf182;
  const double dytdqop0 = xf184*xf87 - xf184*xf88 + xf185*xf83 - xf185*xf84 - xf186*xf76 + xf186*xf80 + xf190*xf89;
  const double dytdlam0 = xf114*xf189 - xf117*xf188 - xf120*xf187 + xf122*xf190;
  const double dytdphi0 = xf134*xf189 - xf136*xf187 - xf137*xf188 + xf138*xf190;
  const double dytdx0 = xf139*xf190 - xf184;
  const double dytdy0 = xf141*xf190 - xf185;
  const double dytdz0 = xf142*xf190 + xf186;
  const double dytdB = -xf153*xf186 + xf155*xf186 + xf156*xf185 - xf157*xf185 + xf158*xf184 - xf159*xf184 + xf160*xf190;

  Matrix<double, 5, 7> res;

  res(0,0) = dqopdqop0;
  res(0,1) = dqopdlam0;
  res(0,2) = dqopdphi0;
  res(0,3) = dqopdx0;
  res(0,4) = dqopdy0;
  res(0,5) = dqopdz0;
  res(0,6) = dqopdB;
  res(1,0) = dlamdqop0;
  res(1,1) = dlamdlam0;
  res(1,2) = dlamdphi0;
  res(1,3) = dlamdx0;
  res(1,4) = dlamdy0;
  res(1,5) = dlamdz0;
  res(1,6) = dlamdB;
  res(2,0) = dphidqop0;
  res(2,1) = dphidlam0;
  res(2,2) = dphidphi0;
  res(2,3) = dphidx0;
  res(2,4) = dphidy0;
  res(2,5) = dphidz0;
  res(2,6) = dphidB;
  res(3,0) = dxtdqop0;
  res(3,1) = dxtdlam0;
  res(3,2) = dxtdphi0;
  res(3,3) = dxtdx0;
  res(3,4) = dxtdy0;
  res(3,5) = dxtdz0;
  res(3,6) = dxtdB;
  res(4,0) = dytdqop0;
  res(4,1) = dytdlam0;
  res(4,2) = dytdphi0;
  res(4,3) = dytdx0;
  res(4,4) = dytdy0;
  res(4,5) = dytdz0;
  res(4,6) = dytdB;

  // convert to tesla for B field gradient
  res.col(6) *= 2.99792458e-3;
  
  return res;
                                                               
}

Matrix<double, 5, 7> ResidualGlobalCorrectionMakerBase::hybrid2curvTransportJacobianVar(const GlobalTrajectoryParameters& globalSource,
                                                             const GlobalTrajectoryParameters& globalDest,
                                                             const double& s,
                                                             const GlobalVector& bfield) const {

  const double qop0 = globalSource.signedInverseMomentum();

    
  const double x0 = globalSource.position().x();
  const double y0 = globalSource.position().y();
  const double z0 = globalSource.position().z();
  
//   const double p0 = globalSource.momentum().mag();
//   const GlobalVector W0 = globalSource.momentum()/p0;
//   const double W0x = W0.x();
//   const double W0y = W0.y();
//   const double W0z = W0.z();
  
  const double lam0 = M_PI_2 - globalSource.momentum().theta();
  const double phi0 = globalSource.momentum().phi();
  
  const double B = bfield.mag();
  const GlobalVector H = bfield/B;
  const double hx = H.x();
  const double hy = H.y();
  const double hz = H.z();
        
  const double xf0 = std::sin(lam0);
  const double xf1 = B*s;
  const double xf2 = qop0*xf1;
  const double xf3 = std::cos(xf2);
  const double xf4 = xf0*xf3;
  const double xf5 = std::sin(phi0);
  const double xf6 = hx*xf5;
  const double xf7 = std::cos(phi0);
  const double xf8 = hy*xf7;
  const double xf9 = xf6 - xf8;
  const double xf10 = std::cos(lam0);
  const double xf11 = std::sin(xf2);
  const double xf12 = xf10*xf11;
  const double xf13 = xf12*xf9;
  const double xf14 = xf3 - 1;
  const double xf15 = hz*xf0;
  const double xf16 = hx*xf10;
  const double xf17 = hy*xf10;
  const double xf18 = xf15 + xf16*xf7 + xf17*xf5;
  const double xf19 = xf14*xf18;
  const double xf20 = hz*xf19;
  const double xf21 = -xf13 - xf20 + xf4;
  const double xf22 = xf10*xf3;
  const double xf23 = xf22*xf5;
  const double xf24 = hx*xf0;
  const double xf25 = hz*xf10;
  const double xf26 = xf25*xf7;
  const double xf27 = xf24 - xf26;
  const double xf28 = xf11*xf27;
  const double xf29 = hy*xf19;
  const double xf30 = xf23 + xf28 - xf29;
  const double xf31 = xf22*xf7;
  const double xf32 = hy*xf0;
  const double xf33 = xf25*xf5;
  const double xf34 = xf32 - xf33;
  const double xf35 = xf11*xf34;
  const double xf36 = hx*xf19;
  const double xf37 = xf31 - xf35 - xf36;
  const double xf38 = std::pow(xf37, 2);
  const double xf39 = std::pow(xf30, 2) + xf38;
  const double xf40 = 1.0/xf39;
  const double xf41 = 1.0/(std::pow(xf21, 2)*xf40 + 1);
  const double xf42 = xf0*xf11;
  const double xf43 = -xf6 + xf8;
  const double xf44 = xf22*xf43;
  const double xf45 = hz*xf18;
  const double xf46 = xf11*xf45;
  const double xf47 = -xf1*xf42 + xf1*xf44 + xf1*xf46;
  const double xf48 = std::pow(xf39, -1.0/2.0);
  const double xf49 = xf12*xf5;
  const double xf50 = xf1*xf49;
  const double xf51 = 2*xf50;
  const double xf52 = xf1*xf3;
  const double xf53 = xf27*xf52;
  const double xf54 = hy*xf18;
  const double xf55 = xf1*xf11;
  const double xf56 = xf54*xf55;
  const double xf57 = 2*xf56;
  const double xf58 = (1.0/2.0)*xf30;
  const double xf59 = xf12*xf7;
  const double xf60 = xf1*xf59;
  const double xf61 = -xf32 + xf33;
  const double xf62 = xf52*xf61;
  const double xf63 = hx*xf18;
  const double xf64 = xf55*xf63;
  const double xf65 = -2*xf60 + 2*xf62 + 2*xf64;
  const double xf66 = (1.0/2.0)*xf37;
  const double xf67 = -xf65*xf66;
  const double xf68 = xf21/std::pow(xf39, 3.0/2.0);
  const double xf69 = B*z0;
  const double xf70 = xf14*xf9;
  const double xf71 = -xf11 + xf2;
  const double xf72 = qop0*xf69 + xf10*xf70 + xf42 + xf45*xf71;
  const double xf73 = 1.0/B;
  const double xf74 = xf73/std::pow(qop0, 2);
  const double xf75 = xf72*xf74;
  const double xf76 = xf1 - xf52;
  const double xf77 = 1.0/qop0;
  const double xf78 = xf73*xf77;
  const double xf79 = xf78*(-xf1*xf13 + xf1*xf4 + xf45*xf76 + xf69);
  const double xf80 = B*y0;
  const double xf81 = qop0*xf80 - xf14*xf27 + xf49 + xf54*xf71;
  const double xf82 = xf74*xf81;
  const double xf83 = xf78*(xf1*xf23 + xf1*xf28 + xf54*xf76 + xf80);
  const double xf84 = B*x0;
  const double xf85 = qop0*xf84 + xf14*xf34 + xf59 + xf63*xf71;
  const double xf86 = xf74*xf85;
  const double xf87 = xf78*(xf1*xf31 - xf1*xf35 + xf63*xf76 + xf84);
  const double xf88 = -xf21*(-xf75 + xf79) - xf30*(-xf82 + xf83) - xf37*(-xf86 + xf87);
  const double xf89 = B*qop0;
  const double xf90 = -xf42*xf89 + xf44*xf89 + xf46*xf89;
  const double xf91 = xf49*xf89;
  const double xf92 = 2*xf91;
  const double xf93 = xf3*xf89;
  const double xf94 = xf27*xf93;
  const double xf95 = xf11*xf89;
  const double xf96 = xf54*xf95;
  const double xf97 = 2*xf96;
  const double xf98 = xf59*xf89;
  const double xf99 = xf61*xf93;
  const double xf100 = xf63*xf95;
  const double xf101 = 2*xf100 - 2*xf98 + 2*xf99;
  const double xf102 = -xf101*xf66;
  const double xf103 = xf41*(xf48*xf90 + xf68*(xf102 - xf58*(-xf92 + 2*xf94 + xf97)));
  const double xf104 = -xf24*xf7 + xf25 - xf32*xf5;
  const double xf105 = hz*xf104;
  const double xf106 = -xf105*xf14 + xf22 - xf42*xf43;
  const double xf107 = xf4*xf5;
  const double xf108 = 2*xf107;
  const double xf109 = xf15*xf7;
  const double xf110 = xf109 + xf16;
  const double xf111 = xf11*xf110;
  const double xf112 = xf104*xf14;
  const double xf113 = hy*xf112;
  const double xf114 = 2*xf113;
  const double xf115 = xf4*xf7;
  const double xf116 = xf15*xf5;
  const double xf117 = xf11*(-xf116 - xf17);
  const double xf118 = hx*xf112;
  const double xf119 = -2*xf115 + 2*xf117 - 2*xf118;
  const double xf120 = -xf119*xf66;
  const double xf121 = -xf0*xf70 + xf105*xf71 + xf12;
  const double xf122 = xf21*xf78;
  const double xf123 = xf104*xf71;
  const double xf124 = hx*xf123 + xf14*(xf116 + xf17) - xf42*xf7;
  const double xf125 = xf37*xf78;
  const double xf126 = 1 - xf3;
  const double xf127 = hy*xf123 + xf110*xf126 - xf42*xf5;
  const double xf128 = xf30*xf78;
  const double xf129 = -xf121*xf122 - xf124*xf125 - xf127*xf128;
  const double xf130 = hx*xf7;
  const double xf131 = hy*xf5;
  const double xf132 = -xf10*xf6 + xf10*xf8;
  const double xf133 = hz*xf132;
  const double xf134 = xf12*(-xf130 - xf131) - xf133*xf14;
  const double xf135 = 2*xf31;
  const double xf136 = xf11*xf33;
  const double xf137 = 2*xf136;
  const double xf138 = xf132*xf14;
  const double xf139 = hy*xf138;
  const double xf140 = 2*xf139;
  const double xf141 = xf11*xf26;
  const double xf142 = hx*xf138;
  const double xf143 = 2*xf141 - 2*xf142 - 2*xf23;
  const double xf144 = -xf143*xf66;
  const double xf145 = xf10*xf14*(xf130 + xf131) + xf133*xf71;
  const double xf146 = xf132*xf71;
  const double xf147 = hy*xf146 + xf126*xf33 + xf59;
  const double xf148 = hx*xf146 - xf14*xf26 - xf49;
  const double xf149 = -xf122*xf145 - xf125*xf148 - xf128*xf147;
  const double xf150 = -xf31;
  const double xf151 = xf150 + xf35 + xf36;
  const double xf152 = -xf23;
  const double xf153 = xf152 - xf28 + xf29;
  const double xf154 = xf13 + xf20 - xf4;
  const double xf155 = qop0*s;
  const double xf156 = -xf155*xf42 + xf155*xf44 + xf155*xf46;
  const double xf157 = xf155*xf49;
  const double xf158 = 2*xf157;
  const double xf159 = xf155*xf3;
  const double xf160 = xf159*xf27;
  const double xf161 = xf11*xf155;
  const double xf162 = xf161*xf54;
  const double xf163 = 2*xf162;
  const double xf164 = xf155*xf59;
  const double xf165 = xf159*xf61;
  const double xf166 = xf161*xf63;
  const double xf167 = -2*xf164 + 2*xf165 + 2*xf166;
  const double xf168 = -xf167*xf66;
  const double xf169 = xf77/std::pow(B, 2);
  const double xf170 = xf169*xf72;
  const double xf171 = xf155 - xf159;
  const double xf172 = xf78*(qop0*z0 - xf13*xf155 + xf155*xf4 + xf171*xf45);
  const double xf173 = xf169*xf81;
  const double xf174 = xf78*(qop0*y0 + xf155*xf23 + xf155*xf28 + xf171*xf54);
  const double xf175 = xf169*xf85;
  const double xf176 = xf78*(qop0*x0 + xf155*xf31 - xf155*xf35 + xf171*xf63);
  const double xf177 = -xf21*(-xf170 + xf172) - xf30*(-xf173 + xf174) - xf37*(-xf175 + xf176);
  const double xf178 = -xf50 + xf53 + xf56;
  const double xf179 = xf37*xf40;
  const double xf180 = -xf60 + xf62 + xf64;
  const double xf181 = xf153*xf40;
  const double xf182 = -xf91 + xf94 + xf96;
  const double xf183 = xf100 - xf98 + xf99;
  const double xf184 = xf179*xf182 + xf181*xf183;
  const double xf185 = -xf107 + xf111 - xf113;
  const double xf186 = -xf115 + xf117 - xf118;
  const double xf187 = xf136 - xf139 + xf31;
  const double xf188 = xf141 - xf142 + xf152;
  const double xf189 = -xf157 + xf160 + xf162;
  const double xf190 = -xf164 + xf165 + xf166;
  const double xf191 = std::pow(xf153, 2) + xf38;
  const double xf192 = std::pow(xf191, -1.0/2.0);
  const double xf193 = xf153*xf192;
  const double xf194 = xf192*xf37;
  const double xf195 = -xf24 + xf26;
  const double xf196 = xf195*xf52;
  const double xf197 = xf196 + xf50 - xf56;
  const double xf198 = xf192*xf78;
  const double xf199 = xf198*xf85;
  const double xf200 = xf198*xf81;
  const double xf201 = std::pow(xf191, -3.0/2.0);
  const double xf202 = (1.0/2.0)*xf153;
  const double xf203 = -xf202*(2*xf196 + xf51 - xf57) + xf67;
  const double xf204 = xf201*xf203;
  const double xf205 = xf78*xf85;
  const double xf206 = xf153*xf205;
  const double xf207 = xf125*xf81;
  const double xf208 = xf89 - xf93;
  const double xf209 = xf208*xf54 + xf23*xf89 + xf28*xf89;
  const double xf210 = xf194*xf78;
  const double xf211 = xf208*xf63 + xf31*xf89 - xf35*xf89;
  const double xf212 = xf193*xf78;
  const double xf213 = xf195*xf93;
  const double xf214 = xf213 + xf91 - xf96;
  const double xf215 = xf102 - xf202*(2*xf213 + xf92 - xf97);
  const double xf216 = xf201*xf215;
  const double xf217 = xf153*xf216;
  const double xf218 = xf183*xf200 + xf199*xf214 + xf205*xf217 + xf207*xf216 + xf209*xf210 + xf211*xf212;
  const double xf219 = xf11*(-xf109 - xf16);
  const double xf220 = xf107 + xf113 + xf219;
  const double xf221 = xf120 - xf202*(xf108 + xf114 + 2*xf219);
  const double xf222 = xf201*xf221;
  const double xf223 = -xf136 + xf139 + xf150;
  const double xf224 = xf144 - xf202*(-xf135 - xf137 + xf140);
  const double xf225 = xf201*xf224;
  const double xf226 = xf159*xf195;
  const double xf227 = xf157 - xf162 + xf226;
  const double xf228 = xf168 - xf202*(xf158 - xf163 + 2*xf226);
  const double xf229 = xf201*xf228;
  const double xf230 = xf194*xf21;
  const double xf231 = xf193*xf21;
  const double xf232 = xf210*xf85;
  const double xf233 = xf212*xf81;
  const double xf234 = xf122*xf192;
  const double xf235 = xf234*xf85;
  const double xf236 = xf234*xf81;
  const double xf237 = xf192*xf38 - xf193*xf30;
  const double xf238 = xf125*xf21*xf85;
  const double xf239 = xf122*xf81;
  const double xf240 = xf153*xf239;
  const double xf241 = xf192*xf30;
  const double xf242 = xf201*xf38;
  const double xf243 = xf153*xf30;
  const double xf244 = xf201*xf243;
  const double xf245 = xf72*xf78;
  const double xf246 = xf231*xf78;
  const double xf247 = xf230*xf78;
  const double xf248 = xf237*xf78;
  const double xf249 = -xf183*xf235 + xf209*xf246 - xf211*xf247 + xf214*xf236 - xf216*xf238 + xf217*xf239 - xf232*xf90 + xf233*xf90 + xf245*(xf101*xf194 - xf182*xf193 - xf214*xf241 + xf215*xf242 - xf216*xf243) + xf248*(-xf13*xf89 + xf208*xf45 + xf4*xf89);
  const double dqopdqop0 = 1;
  const double dqopdlam0 = 0;
  const double dqopdphi0 = 0;
  const double dqopdx0 = 0;
  const double dqopdy0 = 0;
  const double dqopdz0 = 0;
  const double dqopdB = 0;
  const double dlamdqop0 = xf103*xf88 + xf41*(xf47*xf48 + xf68*(-xf58*(-xf51 + 2*xf53 + xf57) + xf67));
  const double dlamdlam0 = xf103*xf129 + xf41*(xf106*xf48 + xf68*(xf120 - xf58*(-xf108 + 2*xf111 - xf114)));
  const double dlamdphi0 = xf103*xf149 + xf41*(xf134*xf48 + xf68*(xf144 - xf58*(xf135 + xf137 - xf140)));
  const double dlamdx0 = xf103*xf151;
  const double dlamdy0 = xf103*xf153;
  const double dlamdz0 = xf103*xf154;
  const double dlamdB = xf103*xf177 + xf41*(xf156*xf48 + xf68*(xf168 - xf58*(-xf158 + 2*xf160 + xf163)));
  const double dphidqop0 = xf178*xf179 + xf180*xf181 + xf184*xf88;
  const double dphidlam0 = xf129*xf184 + xf179*xf185 + xf181*xf186;
  const double dphidphi0 = xf149*xf184 + xf179*xf187 + xf181*xf188;
  const double dphidx0 = xf151*xf184;
  const double dphidy0 = xf153*xf184;
  const double dphidz0 = xf154*xf184;
  const double dphidB = xf177*xf184 + xf179*xf189 + xf181*xf190;
  const double dxtdqop0 = xf180*xf200 - xf193*xf86 + xf193*xf87 - xf194*xf82 + xf194*xf83 + xf197*xf199 + xf204*xf206 + xf204*xf207 + xf218*xf88;
  const double dxtdlam0 = xf124*xf212 + xf127*xf210 + xf129*xf218 + xf186*xf200 + xf199*xf220 + xf206*xf222 + xf207*xf222;
  const double dxtdphi0 = xf147*xf210 + xf148*xf212 + xf149*xf218 + xf188*xf200 + xf199*xf223 + xf206*xf225 + xf207*xf225;
  const double dxtdx0 = xf151*xf218 + xf193;
  const double dxtdy0 = xf153*xf218 + xf194;
  const double dxtdz0 = xf154*xf218;
  const double dxtdB = -xf173*xf194 + xf174*xf194 - xf175*xf193 + xf176*xf193 + xf177*xf218 + xf190*xf200 + xf199*xf227 + xf206*xf229 + xf207*xf229;
  const double dytdqop0 = -xf180*xf235 + xf197*xf236 - xf204*xf238 + xf204*xf240 + xf230*xf86 - xf230*xf87 - xf231*xf82 + xf231*xf83 - xf232*xf47 + xf233*xf47 - xf237*xf75 + xf237*xf79 + xf245*(-xf178*xf193 + xf194*xf65 - xf197*xf241 + xf203*xf242 - xf203*xf244) + xf249*xf88;
  const double dytdlam0 = -xf106*xf232 + xf106*xf233 + xf121*xf248 - xf124*xf247 + xf127*xf246 + xf129*xf249 - xf186*xf235 + xf220*xf236 - xf222*xf238 + xf222*xf240 + xf245*(xf119*xf194 - xf185*xf193 - xf220*xf241 + xf221*xf242 - xf221*xf244);
  const double dytdphi0 = -xf134*xf232 + xf134*xf233 + xf145*xf248 + xf147*xf246 - xf148*xf247 + xf149*xf249 - xf188*xf235 + xf223*xf236 - xf225*xf238 + xf225*xf240 + xf245*(xf143*xf194 - xf187*xf193 - xf223*xf241 + xf224*xf242 - xf224*xf244);
  const double dytdx0 = xf151*xf249 - xf230;
  const double dytdy0 = xf153*xf249 + xf231;
  const double dytdz0 = xf154*xf249 + xf237;
  const double dytdB = -xf156*xf232 + xf156*xf233 - xf170*xf237 + xf172*xf237 - xf173*xf231 + xf174*xf231 + xf175*xf230 - xf176*xf230 + xf177*xf249 - xf190*xf235 + xf227*xf236 - xf229*xf238 + xf229*xf240 + xf245*(xf167*xf194 - xf189*xf193 - xf227*xf241 + xf228*xf242 - xf228*xf244);
  Matrix<double, 5, 7> res;
  res(0,0) = dqopdqop0;
  res(0,1) = dqopdlam0;
  res(0,2) = dqopdphi0;
  res(0,3) = dqopdx0;
  res(0,4) = dqopdy0;
  res(0,5) = dqopdz0;
  res(0,6) = dqopdB;
  res(1,0) = dlamdqop0;
  res(1,1) = dlamdlam0;
  res(1,2) = dlamdphi0;
  res(1,3) = dlamdx0;
  res(1,4) = dlamdy0;
  res(1,5) = dlamdz0;
  res(1,6) = dlamdB;
  res(2,0) = dphidqop0;
  res(2,1) = dphidlam0;
  res(2,2) = dphidphi0;
  res(2,3) = dphidx0;
  res(2,4) = dphidy0;
  res(2,5) = dphidz0;
  res(2,6) = dphidB;
  res(3,0) = dxtdqop0;
  res(3,1) = dxtdlam0;
  res(3,2) = dxtdphi0;
  res(3,3) = dxtdx0;
  res(3,4) = dxtdy0;
  res(3,5) = dxtdz0;
  res(3,6) = dxtdB;
  res(4,0) = dytdqop0;
  res(4,1) = dytdlam0;
  res(4,2) = dytdphi0;
  res(4,3) = dytdx0;
  res(4,4) = dytdy0;
  res(4,5) = dytdz0;
  res(4,6) = dytdB;

  // convert to tesla for B field gradient
  res.col(6) *= 2.99792458e-3;
  
  return res;
                                                               
}


Matrix<double, 5, 6> ResidualGlobalCorrectionMakerBase::localTransportJacobian(const TrajectoryStateOnSurface &start,
                                            const std::pair<TrajectoryStateOnSurface, double> &propresult,
                                            bool doReverse) const {
  
  const TrajectoryStateOnSurface& state0 = doReverse ? propresult.first : start;
  const TrajectoryStateOnSurface& state1 = doReverse ? start : propresult.first;
  const GlobalVector& h = start.globalParameters().magneticFieldInInverseGeV();
  const double s = doReverse ? -propresult.second : propresult.second;
  
  // compute local to curvilinear jacobian at source
  JacobianLocalToCurvilinear local2curv(state0.surface(), state0.localParameters(), *state0.magneticField());
  const AlgebraicMatrix55& local2curvjac = local2curv.jacobian();
  const Matrix<double, 5, 5> H0 = Map<const Matrix<double, 5, 5, RowMajor>>(local2curvjac.Array());
  
  // compute curvilinear to local jacobian at destination
  JacobianCurvilinearToLocal curv2local(state1.surface(), state1.localParameters(), *state1.magneticField());
  const AlgebraicMatrix55& curv2localjac = curv2local.jacobian();
  const Matrix<double, 5, 5> H1 = Map<const Matrix<double, 5, 5, RowMajor>>(curv2localjac.Array());
  
//   // compute transport jacobian wrt curvlinear parameters
//   AnalyticalCurvilinearJacobian curv2curv;
//   curv2curv.computeFullJacobian(state0.globalParameters(), state1.globalParameters().position(), state1.globalParameters().momentum(), h, s);
//   const AlgebraicMatrix55& curv2curvjac = curv2curv.jacobian();
//   const Matrix<double, 5, 5> F = Map<const Matrix<double, 5, 5, RowMajor>>(curv2curvjac.Array());
//   
//   std::cout << "qbp" << std::endl;
//   std::cout << state0.localParameters().qbp() << std::endl;
//   
//   std::cout << "F" << std::endl;
//   std::cout << F << std::endl;
//   
//   // compute transport jacobian wrt B field (magnitude)
//   const Matrix<double, 5, 1> dF = bfieldJacobian(state0.globalParameters(), state1.globalParameters(), s, h);
//   
//   std::cout << "dF" << std::endl;
//   std::cout << dF << std::endl;
//   
//   Matrix<double, 5, 1> dFalt;
//   dFalt[0] = 0.;
//   dFalt.tail<4>() = 2.99792458e-3*state0.localParameters().qbp()/h.mag()*F.block<4, 1>(1, 0);
//   
//   std::cout << "dFalt" << std::endl;
//   std::cout << dFalt << std::endl;
//   
//   const Matrix<double, 5, 6> FdFfull = curvtransportgrad(state0.globalParameters(), state1.globalParameters(), s, h);
//   std::cout << "FdFfull" << std::endl;
//   std::cout << FdFfull << std::endl;  
//   
//   const Matrix<double, 5, 5> FmIcmssw = F - Matrix<double, 5, 5>::Identity();
//   std::cout << "FmIcmssw" << std::endl;
//   std::cout << FmIcmssw << std::endl;  
//   
//   const Matrix<double, 5, 5> FmI = FdFfull.block<5,5>(0,0) - Matrix<double, 5, 5>::Identity();
//   std::cout << "FmI" << std::endl;
//   std::cout << FmI << std::endl;  
  
  const Matrix<double, 5, 6> FdF = curvtransportJacobian(state0.globalParameters(), state1.globalParameters(), s, h);
  const Matrix<double, 5, 5> F = FdF.topLeftCorner<5,5>();
  const Matrix<double, 5, 1> dF = FdF.col(5);
  
  Matrix<double, 5, 6> res;
  res.leftCols<5>() = H1*F*H0;
  res.rightCols<1>() = H1*dF;
  return res;
                                              
}


Matrix<double, 5, 5> ResidualGlobalCorrectionMakerBase::curv2localJacobianAlt(const TrajectoryStateOnSurface &state) const {
  

  const GlobalTrajectoryParameters &globalSource = state.globalParameters();
  const GlobalVector &bfield = state.globalParameters().magneticFieldInInverseGeV();
  
  CurvilinearTrajectoryParameters curvparms(globalSource.position(), globalSource.momentum(), globalSource.charge());

  const double qop0 = curvparms.Qbp();
  const double lam0 = curvparms.lambda();
  const double phi0 = curvparms.phi();
  const double xt0 = curvparms.xT();
  const double yt0 = curvparms.yT();
  
  const double p0 = globalSource.momentum().mag();
  const GlobalVector W0 = globalSource.momentum()/p0;
  const double W0x = W0.x();
  const double W0y = W0.y();
  const double W0z = W0.z();
    
  const LocalVector lx(1.,0.,0.);
  const LocalVector ly(0.,1.,0.);
  const LocalVector lz(0.,0.,1.);
  const LocalPoint l0(0., 0.);
  
  const GlobalVector I1 = state.surface().toGlobal<double>(lz);
  const GlobalVector J1 = state.surface().toGlobal<double>(lx);
  const GlobalVector K1 = state.surface().toGlobal<double>(ly);  
  const GlobalPoint r1 = state.surface().toGlobal<double>(l0);
  
  const double Ix1 = I1.x();
  const double Iy1 = I1.y();
  const double Iz1 = I1.z();
  
  const double Jx1 = J1.x();
  const double Jy1 = J1.y();
  const double Jz1 = J1.z();
  
  const double Kx1 = K1.x();
  const double Ky1 = K1.y();
  const double Kz1 = K1.z();
  
  const double rx1 = r1.x();
  const double ry1 = r1.y();
  const double rz1 = r1.z();
    
  const double B = bfield.mag();
  const GlobalVector H = bfield/B;
  const double hx = H.x();
  const double hy = H.y();
  const double hz = H.z();
      
  const double x0 = std::sin(lam0);
  const double x1 = Iz1*x0;
  const double x2 = std::cos(lam0);
  const double x3 = std::cos(phi0);
  const double x4 = Ix1*x3;
  const double x5 = std::sin(phi0);
  const double x6 = Iy1*x5;
  const double x7 = x1 + x2*x4 + x2*x6;
  const double x8 = std::pow(x7, -2);
  const double x9 = Iz1*Jx1;
  const double x10 = Iz1*Jy1;
  const double x11 = lam0 + phi0;
  const double x12 = -phi0;
  const double x13 = lam0 + x12;
  const double x14 = std::pow(Ix1*std::cos(x11) + Ix1*std::cos(x13) + Iy1*std::sin(x11) - Iy1*std::sin(x13) + 2*x1, -2);
  const double x15 = 2*Ix1;
  const double x16 = Jy1*x15;
  const double x17 = 2*Iy1;
  const double x18 = Jx1*x17;
  const double x19 = 2*lam0;
  const double x20 = std::cos(x19);
  const double x21 = phi0 + x19;
  const double x22 = std::cos(x21);
  const double x23 = std::sin(x21);
  const double x24 = Ix1*Jz1;
  const double x25 = Iy1*Jz1;
  const double x26 = x12 + x19;
  const double x27 = std::cos(x26);
  const double x28 = std::sin(x26);
  const double x29 = Ix1*W0y - Iy1*W0x;
  const double x30 = hz*x2;
  const double x31 = hx*x0 - x3*x30;
  const double x32 = hy*x0 - x30*x5;
  const double x33 = x2*(hx*x5 - hy*x3);
  const double x34 = x2*x3;
  const double x35 = x2*x5;
  const double x36 = Jx1*x34 + Jy1*x35 + Jz1*x0;
  const double x37 = -Ix1*x32 + Iy1*x31 - Iz1*x33;
  const double x38 = std::pow(W0x, 2) + std::pow(W0y, 2);
  const double x39 = std::pow(x38, -1.0/2.0);
  const double x40 = B*qop0*x39/std::pow(x7, 3);
  const double x41 = x40*(-x36*x37 + x7*(-Jx1*x32 + Jy1*x31 - Jz1*x33));
  const double x42 = Ix1*W0x*W0z + Iy1*W0y*W0z - Iz1*x38;
  const double x43 = Iz1*Kx1;
  const double x44 = Iz1*Ky1;
  const double x45 = Ky1*x15;
  const double x46 = Kx1*x17;
  const double x47 = Ix1*Kz1;
  const double x48 = Iy1*Kz1;
  const double x49 = Kx1*x34 + Ky1*x35 + Kz1*x0;
  const double x50 = x40*(-x37*x49 + x7*(-Kx1*x32 + Ky1*x31 - Kz1*x33));
  const double x51 = x39/x7;
  const double x52 = x38*x7;
  const double x53 = W0z*x7;
  const double dqopdqop0 = 1;
  const double dqopdlam0 = 0;
  const double dqopdphi0 = 0;
  const double dqopdxt0 = 0;
  const double dqopdyt0 = 0;
  const double ddxdzdqop0 = 0;
  const double ddxdzdlam0 = x8*(Jz1*x4 + Jz1*x6 - x10*x5 - x3*x9);
  const double ddxdzdphi0 = x14*(x10*x23 + x10*x28 + x16*x20 + x16 - x18*x20 - x18 - x22*x24 + x22*x9 - x23*x25 + x24*x27 - x25*x28 - x27*x9);
  const double ddxdzdxt0 = x29*x41;
  const double ddxdzdyt0 = x41*x42;
  const double ddydzdqop0 = 0;
  const double ddydzdlam0 = x8*(Kz1*x4 + Kz1*x6 - x3*x43 - x44*x5);
  const double ddydzdphi0 = x14*(x20*x45 - x20*x46 + x22*x43 - x22*x47 + x23*x44 - x23*x48 - x27*x43 + x27*x47 + x28*x44 - x28*x48 + x45 - x46);
  const double ddydzdxt0 = x29*x50;
  const double ddydzdyt0 = x42*x50;
  const double dxdqop0 = 0;
  const double dxdlam0 = 0;
  const double dxdphi0 = 0;
  const double dxdxt0 = x51*(x29*x36 + x7*(-Jx1*W0y + Jy1*W0x));
  const double dxdyt0 = x51*(Jz1*x52 + x36*x42 - x53*(Jx1*W0x + Jy1*W0y));
  const double dydqop0 = 0;
  const double dydlam0 = 0;
  const double dydphi0 = 0;
  const double dydxt0 = x51*(x29*x49 + x7*(-Kx1*W0y + Ky1*W0x));
  const double dydyt0 = x51*(Kz1*x52 + x42*x49 - x53*(Kx1*W0x + Ky1*W0y));
  Matrix<double, 5, 5> res;
  res(0,0) = dqopdqop0;
  res(0,1) = dqopdlam0;
  res(0,2) = dqopdphi0;
  res(0,3) = dqopdxt0;
  res(0,4) = dqopdyt0;
  res(1,0) = ddxdzdqop0;
  res(1,1) = ddxdzdlam0;
  res(1,2) = ddxdzdphi0;
  res(1,3) = ddxdzdxt0;
  res(1,4) = ddxdzdyt0;
  res(2,0) = ddydzdqop0;
  res(2,1) = ddydzdlam0;
  res(2,2) = ddydzdphi0;
  res(2,3) = ddydzdxt0;
  res(2,4) = ddydzdyt0;
  res(3,0) = dxdqop0;
  res(3,1) = dxdlam0;
  res(3,2) = dxdphi0;
  res(3,3) = dxdxt0;
  res(3,4) = dxdyt0;
  res(4,0) = dydqop0;
  res(4,1) = dydlam0;
  res(4,2) = dydphi0;
  res(4,3) = dydxt0;
  res(4,4) = dydyt0;

  
  return res;
                                              
}

Matrix<double, 5, 5> ResidualGlobalCorrectionMakerBase::curv2localJacobianAlteloss(const TrajectoryStateOnSurface &state, double dEdx, double mass) const {
  
  const GlobalTrajectoryParameters &globalSource = state.globalParameters();
  const GlobalVector &bfield = globalSource.magneticFieldInInverseGeV();
  
  const Surface &surface = state.surface();
  
  CurvilinearTrajectoryParameters curvparms(globalSource.position(), globalSource.momentum(), globalSource.charge());

  const double qop0 = curvparms.Qbp();
  const double lam0 = curvparms.lambda();
  const double phi0 = curvparms.phi();
  const double xt0 = curvparms.xT();
  const double yt0 = curvparms.yT();
  
  const double q = globalSource.charge();
  
  const double p0 = globalSource.momentum().mag();
  const GlobalVector W0 = globalSource.momentum()/p0;
  const double W0x = W0.x();
  const double W0y = W0.y();
  const double W0z = W0.z();
    
  const LocalVector lx(1.,0.,0.);
  const LocalVector ly(0.,1.,0.);
  const LocalVector lz(0.,0.,1.);
  const LocalPoint l0(0., 0.);
  
  const GlobalVector I1 = surface.toGlobal<double>(lz);
  const GlobalVector J1 = surface.toGlobal<double>(lx);
  const GlobalVector K1 = surface.toGlobal<double>(ly);  
  const GlobalPoint r1 = surface.toGlobal<double>(l0);
  
  const double Ix1 = I1.x();
  const double Iy1 = I1.y();
  const double Iz1 = I1.z();
  
  const double Jx1 = J1.x();
  const double Jy1 = J1.y();
  const double Jz1 = J1.z();
  
  const double Kx1 = K1.x();
  const double Ky1 = K1.y();
  const double Kz1 = K1.z();
  
  const double rx1 = r1.x();
  const double ry1 = r1.y();
  const double rz1 = r1.z();
    
  const double B = bfield.mag();
  const GlobalVector H = bfield/B;
  const double hx = H.x();
  const double hy = H.y();
  const double hz = H.z();
        
  const double x0 = std::pow(q, 2);
  const double x1 = std::pow(x0, -3.0/2.0);
  const double x2 = Iy1*W0x;
  const double x3 = Ix1*W0y;
  const double x4 = std::pow(qop0, 2);
  const double x5 = std::pow(W0x, 2) + std::pow(W0y, 2);
  const double x6 = std::pow(x5, -1.0/2.0);
  const double x7 = std::sin(lam0);
  const double x8 = Iz1*x7;
  const double x9 = std::cos(lam0);
  const double x10 = std::cos(phi0);
  const double x11 = Ix1*x10;
  const double x12 = std::sin(phi0);
  const double x13 = Iy1*x12;
  const double x14 = x11*x9 + x13*x9 + x8;
  const double x15 = x6/x14;
  const double x16 = dEdx*q*x1*x15*x4*std::sqrt(std::pow(mass, 2)*x4 + x0);
  const double x17 = Ix1*W0x*W0z + Iy1*W0y*W0z - Iz1*x5;
  const double x18 = std::pow(x14, -2);
  const double x19 = Iz1*Jx1;
  const double x20 = Iz1*Jy1;
  const double x21 = lam0 + phi0;
  const double x22 = -phi0;
  const double x23 = lam0 + x22;
  const double x24 = std::pow(Ix1*std::cos(x21) + Ix1*std::cos(x23) + Iy1*std::sin(x21) - Iy1*std::sin(x23) + 2*x8, -2);
  const double x25 = 2*Ix1;
  const double x26 = Jy1*x25;
  const double x27 = 2*Iy1;
  const double x28 = Jx1*x27;
  const double x29 = 2*lam0;
  const double x30 = std::cos(x29);
  const double x31 = phi0 + x29;
  const double x32 = std::cos(x31);
  const double x33 = std::sin(x31);
  const double x34 = Ix1*Jz1;
  const double x35 = Iy1*Jz1;
  const double x36 = x22 + x29;
  const double x37 = std::cos(x36);
  const double x38 = std::sin(x36);
  const double x39 = -x2 + x3;
  const double x40 = hz*x9;
  const double x41 = hx*x7 - x10*x40;
  const double x42 = hy*x7 - x12*x40;
  const double x43 = x9*(hx*x12 - hy*x10);
  const double x44 = x10*x9;
  const double x45 = x12*x9;
  const double x46 = Jx1*x44 + Jy1*x45 + Jz1*x7;
  const double x47 = -Ix1*x42 + Iy1*x41 - Iz1*x43;
  const double x48 = B*qop0*x6/std::pow(x14, 3);
  const double x49 = x48*(x14*(-Jx1*x42 + Jy1*x41 - Jz1*x43) - x46*x47);
  const double x50 = Iz1*Kx1;
  const double x51 = Iz1*Ky1;
  const double x52 = Ky1*x25;
  const double x53 = Kx1*x27;
  const double x54 = Ix1*Kz1;
  const double x55 = Iy1*Kz1;
  const double x56 = Kx1*x44 + Ky1*x45 + Kz1*x7;
  const double x57 = x48*(x14*(-Kx1*x42 + Ky1*x41 - Kz1*x43) - x47*x56);
  const double x58 = x14*x5;
  const double x59 = W0z*x14;
  const double dqopdqop0 = std::pow(q, 3)*x1*std::fabs(qop0)/qop0;
  const double dqopdlam0 = 0;
  const double dqopdphi0 = 0;
  const double dqopdxt0 = x16*(x2 - x3);
  const double dqopdyt0 = -x16*x17;
  const double ddxdzdqop0 = 0;
  const double ddxdzdlam0 = x18*(Jz1*x11 + Jz1*x13 - x10*x19 - x12*x20);
  const double ddxdzdphi0 = x24*(x19*x32 - x19*x37 + x20*x33 + x20*x38 + x26*x30 + x26 - x28*x30 - x28 - x32*x34 - x33*x35 + x34*x37 - x35*x38);
  const double ddxdzdxt0 = x39*x49;
  const double ddxdzdyt0 = x17*x49;
  const double ddydzdqop0 = 0;
  const double ddydzdlam0 = x18*(Kz1*x11 + Kz1*x13 - x10*x50 - x12*x51);
  const double ddydzdphi0 = x24*(x30*x52 - x30*x53 + x32*x50 - x32*x54 + x33*x51 - x33*x55 - x37*x50 + x37*x54 + x38*x51 - x38*x55 + x52 - x53);
  const double ddydzdxt0 = x39*x57;
  const double ddydzdyt0 = x17*x57;
  const double dxdqop0 = 0;
  const double dxdlam0 = 0;
  const double dxdphi0 = 0;
  const double dxdxt0 = x15*(x14*(-Jx1*W0y + Jy1*W0x) + x39*x46);
  const double dxdyt0 = x15*(Jz1*x58 + x17*x46 - x59*(Jx1*W0x + Jy1*W0y));
  const double dydqop0 = 0;
  const double dydlam0 = 0;
  const double dydphi0 = 0;
  const double dydxt0 = x15*(x14*(-Kx1*W0y + Ky1*W0x) + x39*x56);
  const double dydyt0 = x15*(Kz1*x58 + x17*x56 - x59*(Kx1*W0x + Ky1*W0y));
  Matrix<double, 5, 5> res;
  res(0,0) = dqopdqop0;
  res(0,1) = dqopdlam0;
  res(0,2) = dqopdphi0;
  res(0,3) = dqopdxt0;
  res(0,4) = dqopdyt0;
  res(1,0) = ddxdzdqop0;
  res(1,1) = ddxdzdlam0;
  res(1,2) = ddxdzdphi0;
  res(1,3) = ddxdzdxt0;
  res(1,4) = ddxdzdyt0;
  res(2,0) = ddydzdqop0;
  res(2,1) = ddydzdlam0;
  res(2,2) = ddydzdphi0;
  res(2,3) = ddydzdxt0;
  res(2,4) = ddydzdyt0;
  res(3,0) = dxdqop0;
  res(3,1) = dxdlam0;
  res(3,2) = dxdphi0;
  res(3,3) = dxdxt0;
  res(3,4) = dxdyt0;
  res(4,0) = dydqop0;
  res(4,1) = dydlam0;
  res(4,2) = dydphi0;
  res(4,3) = dydxt0;
  res(4,4) = dydyt0;

  
  return res;
                                              
}

// Point3DBase<double, GlobalTag> ResidualGlobalCorrectionMakerBase::toGlobal(const Surface &surface, const Point3DBase<double, LocalTag> &lp) const {
//   Point3DBase<double, GlobalTag> position = surface.position();
//   TkRotation<double> rotation = surface.rotation();
//   
//   return Point3DBase<double, GlobalTag>(rotation.multiplyInverse(lp.basicVector()) + position.basicVector());
// 
// }
// 
// Vector3DBase<double, GlobalTag> ResidualGlobalCorrectionMakerBase::toGlobal(const Surface &surface, const Vector3DBase<double, LocalTag> &lv) const {
//   TkRotation<double> rotation = surface.rotation();
//   
//   return Vector3DBase<double, GlobalTag>(rotation.multiplyInverse(lv.basicVector()));
// }
// 
// Point3DBase<double, LocalTag> ResidualGlobalCorrectionMakerBase::toLocal(const Surface &surface, const Point3DBase<double, GlobalTag> &gp) const {
//   Point3DBase<double, GlobalTag> position = surface.position();
//   TkRotation<double> rotation = surface.rotation();
//   
//   return Point3DBase<double, LocalTag>(rotation*(gp.basicVector() - position.basicVector()));
// 
// }
// 
// Vector3DBase<double, LocalTag> ResidualGlobalCorrectionMakerBase::toLocal(const Surface &surface, const Vector3DBase<double, GlobalTag> &gv) const {
//   TkRotation<double> rotation = surface.rotation();
//   
//   return Vector3DBase<double, LocalTag>(rotation*(gv.basicVector()));
// 
// }

Matrix<double, 5, 5> ResidualGlobalCorrectionMakerBase::curv2localJacobianAltelossD(const Matrix<double, 7, 1> &state, const MagneticField *field, const GloballyPositioned<double> &surface, double dEdx, double mass, double dBz) const {
  
  
  const GlobalPoint pos(state[0], state[1], state[2]);  
  const GlobalVector &bfield = field->inInverseGeV(pos);
  const Matrix<double, 3, 1> Bv(bfield.x(), bfield.y(), double(bfield.z()) + 2.99792458e-3*dBz);
  
  const double q = state[6];
  
  const double qop0 = q/state.segment<3>(3).norm();
  const double lam0 = std::atan(state[5]/std::sqrt(state[3]*state[3] + state[4]*state[4]));
  const double phi0 = std::atan2(state[4], state[3]);
  
  const Matrix<double, 3, 1> W0 = state.segment<3>(3).normalized();
  const double W0x = W0[0];
  const double W0y = W0[1];
  const double W0z = W0[2];
    
  const Vector3DBase<double, LocalTag> lx(1.,0.,0.);
  const Vector3DBase<double, LocalTag> ly(0.,1.,0.);
  const Vector3DBase<double, LocalTag> lz(0.,0.,1.);
  const Point3DBase<double, LocalTag> l0(0., 0., 0.);
  
//   const Vector3DBase<double, GlobalTag> I1 = surface.toGlobal<double>(lz);
//   const Vector3DBase<double, GlobalTag> J1 = surface.toGlobal<double>(lx);
//   const Vector3DBase<double, GlobalTag> K1 = surface.toGlobal<double>(ly);  
//   const Point3DBase<double, GlobalTag> r1 = surface.toGlobal<double>(l0);
  
  const Vector3DBase<double, GlobalTag> I1 = surface.toGlobal(lz);
  const Vector3DBase<double, GlobalTag> J1 = surface.toGlobal(lx);
  const Vector3DBase<double, GlobalTag> K1 = surface.toGlobal(ly);  
  const Point3DBase<double, GlobalTag> r1 = surface.toGlobal(l0);
  
//   const Vector3DBase<double, GlobalTag> I1 = toGlobal(surface, lz);
//   const Vector3DBase<double, GlobalTag> J1 = toGlobal(surface, lx);
//   const Vector3DBase<double, GlobalTag> K1 = toGlobal(surface, ly);  
//   const Point3DBase<double, GlobalTag> r1 = toGlobal(surface, l0);
  
  const double Ix1 = I1.x();
  const double Iy1 = I1.y();
  const double Iz1 = I1.z();
  
  const double Jx1 = J1.x();
  const double Jy1 = J1.y();
  const double Jz1 = J1.z();
  
  const double Kx1 = K1.x();
  const double Ky1 = K1.y();
  const double Kz1 = K1.z();
  
  const double rx1 = r1.x();
  const double ry1 = r1.y();
  const double rz1 = r1.z();
    
  const double B = Bv.norm();
  const Matrix<double, 3, 1> H = Bv.normalized();
  const double hx = H[0];
  const double hy = H[1];
  const double hz = H[2];
        
  const double x0 = std::pow(q, 2);
  const double x1 = std::pow(x0, -3.0/2.0);
  const double x2 = Iy1*W0x;
  const double x3 = Ix1*W0y;
  const double x4 = std::pow(qop0, 2);
  const double x5 = std::pow(W0x, 2) + std::pow(W0y, 2);
  const double x6 = std::pow(x5, -1.0/2.0);
  const double x7 = std::sin(lam0);
  const double x8 = Iz1*x7;
  const double x9 = std::cos(lam0);
  const double x10 = std::cos(phi0);
  const double x11 = Ix1*x10;
  const double x12 = std::sin(phi0);
  const double x13 = Iy1*x12;
  const double x14 = x11*x9 + x13*x9 + x8;
  const double x15 = x6/x14;
  const double x16 = dEdx*q*x1*x15*x4*std::sqrt(std::pow(mass, 2)*x4 + x0);
  const double x17 = Ix1*W0x*W0z + Iy1*W0y*W0z - Iz1*x5;
  const double x18 = std::pow(x14, -2);
  const double x19 = Iz1*Jx1;
  const double x20 = Iz1*Jy1;
  const double x21 = lam0 + phi0;
  const double x22 = -phi0;
  const double x23 = lam0 + x22;
  const double x24 = std::pow(Ix1*std::cos(x21) + Ix1*std::cos(x23) + Iy1*std::sin(x21) - Iy1*std::sin(x23) + 2*x8, -2);
  const double x25 = 2*Ix1;
  const double x26 = Jy1*x25;
  const double x27 = 2*Iy1;
  const double x28 = Jx1*x27;
  const double x29 = 2*lam0;
  const double x30 = std::cos(x29);
  const double x31 = phi0 + x29;
  const double x32 = std::cos(x31);
  const double x33 = std::sin(x31);
  const double x34 = Ix1*Jz1;
  const double x35 = Iy1*Jz1;
  const double x36 = x22 + x29;
  const double x37 = std::cos(x36);
  const double x38 = std::sin(x36);
  const double x39 = -x2 + x3;
  const double x40 = hz*x9;
  const double x41 = hx*x7 - x10*x40;
  const double x42 = hy*x7 - x12*x40;
  const double x43 = x9*(hx*x12 - hy*x10);
  const double x44 = x10*x9;
  const double x45 = x12*x9;
  const double x46 = Jx1*x44 + Jy1*x45 + Jz1*x7;
  const double x47 = -Ix1*x42 + Iy1*x41 - Iz1*x43;
  const double x48 = B*qop0*x6/std::pow(x14, 3);
  const double x49 = x48*(x14*(-Jx1*x42 + Jy1*x41 - Jz1*x43) - x46*x47);
  const double x50 = Iz1*Kx1;
  const double x51 = Iz1*Ky1;
  const double x52 = Ky1*x25;
  const double x53 = Kx1*x27;
  const double x54 = Ix1*Kz1;
  const double x55 = Iy1*Kz1;
  const double x56 = Kx1*x44 + Ky1*x45 + Kz1*x7;
  const double x57 = x48*(x14*(-Kx1*x42 + Ky1*x41 - Kz1*x43) - x47*x56);
  const double x58 = x14*x5;
  const double x59 = W0z*x14;
  const double dqopdqop0 = std::pow(q, 3)*x1*std::fabs(qop0)/qop0;
  const double dqopdlam0 = 0;
  const double dqopdphi0 = 0;
  const double dqopdxt0 = x16*(x2 - x3);
  const double dqopdyt0 = -x16*x17;
  const double ddxdzdqop0 = 0;
  const double ddxdzdlam0 = x18*(Jz1*x11 + Jz1*x13 - x10*x19 - x12*x20);
  const double ddxdzdphi0 = x24*(x19*x32 - x19*x37 + x20*x33 + x20*x38 + x26*x30 + x26 - x28*x30 - x28 - x32*x34 - x33*x35 + x34*x37 - x35*x38);
  const double ddxdzdxt0 = x39*x49;
  const double ddxdzdyt0 = x17*x49;
  const double ddydzdqop0 = 0;
  const double ddydzdlam0 = x18*(Kz1*x11 + Kz1*x13 - x10*x50 - x12*x51);
  const double ddydzdphi0 = x24*(x30*x52 - x30*x53 + x32*x50 - x32*x54 + x33*x51 - x33*x55 - x37*x50 + x37*x54 + x38*x51 - x38*x55 + x52 - x53);
  const double ddydzdxt0 = x39*x57;
  const double ddydzdyt0 = x17*x57;
  const double dxdqop0 = 0;
  const double dxdlam0 = 0;
  const double dxdphi0 = 0;
  const double dxdxt0 = x15*(x14*(-Jx1*W0y + Jy1*W0x) + x39*x46);
  const double dxdyt0 = x15*(Jz1*x58 + x17*x46 - x59*(Jx1*W0x + Jy1*W0y));
  const double dydqop0 = 0;
  const double dydlam0 = 0;
  const double dydphi0 = 0;
  const double dydxt0 = x15*(x14*(-Kx1*W0y + Ky1*W0x) + x39*x56);
  const double dydyt0 = x15*(Kz1*x58 + x17*x56 - x59*(Kx1*W0x + Ky1*W0y));
  Matrix<double, 5, 5> res;
  res(0,0) = dqopdqop0;
  res(0,1) = dqopdlam0;
  res(0,2) = dqopdphi0;
  res(0,3) = dqopdxt0;
  res(0,4) = dqopdyt0;
  res(1,0) = ddxdzdqop0;
  res(1,1) = ddxdzdlam0;
  res(1,2) = ddxdzdphi0;
  res(1,3) = ddxdzdxt0;
  res(1,4) = ddxdzdyt0;
  res(2,0) = ddydzdqop0;
  res(2,1) = ddydzdlam0;
  res(2,2) = ddydzdphi0;
  res(2,3) = ddydzdxt0;
  res(2,4) = ddydzdyt0;
  res(3,0) = dxdqop0;
  res(3,1) = dxdlam0;
  res(3,2) = dxdphi0;
  res(3,3) = dxdxt0;
  res(3,4) = dxdyt0;
  res(4,0) = dydqop0;
  res(4,1) = dydlam0;
  res(4,2) = dydphi0;
  res(4,3) = dydxt0;
  res(4,4) = dydyt0;

  
  return res;
                                              
}

Matrix<double, 6, 5> ResidualGlobalCorrectionMakerBase::curv2cartJacobianAlt(const FreeTrajectoryState &state) const {
  
  const GlobalTrajectoryParameters &globalSource = state.parameters();
  
  CurvilinearTrajectoryParameters curvparms(globalSource.position(), globalSource.momentum(), globalSource.charge());

  const double qop0 = curvparms.Qbp();
  const double lam0 = curvparms.lambda();
  const double phi0 = curvparms.phi();
  const double xt0 = curvparms.xT();
  const double yt0 = curvparms.yT();
  
  const double p0 = globalSource.momentum().mag();
  const GlobalVector W0 = globalSource.momentum()/p0;
  const double W0x = W0.x();
  const double W0y = W0.y();
  const double W0z = W0.z();
    
  const double x0 = std::sqrt(std::pow(W0x, 2) + std::pow(W0y, 2));
  const double x1 = 1.0/x0;
  const double x2 = W0y*x1;
  const double x3 = W0x*x1;
  const double x4 = 1.0/qop0;
  const double x5 = std::cos(phi0);
  const double x6 = 1.0/std::fabs(qop0);
  const double x7 = x6*std::cos(lam0);
  const double x8 = x5*x7;
  const double x9 = x6*std::sin(lam0);
  const double x10 = std::sin(phi0);
  const double x11 = x10*x7;
  const double dxdqop0 = 0;
  const double dxdlam0 = 0;
  const double dxdphi0 = 0;
  const double dxdxt0 = -x2;
  const double dxdyt0 = -W0z*x3;
  const double dydqop0 = 0;
  const double dydlam0 = 0;
  const double dydphi0 = 0;
  const double dydxt0 = x3;
  const double dydyt0 = -W0z*x2;
  const double dzdqop0 = 0;
  const double dzdlam0 = 0;
  const double dzdphi0 = 0;
  const double dzdxt0 = 0;
  const double dzdyt0 = x0;
  const double dpxdqop0 = -x4*x8;
  const double dpxdlam0 = -x5*x9;
  const double dpxdphi0 = -x11;
  const double dpxdxt0 = 0;
  const double dpxdyt0 = 0;
  const double dpydqop0 = -x11*x4;
  const double dpydlam0 = -x10*x9;
  const double dpydphi0 = x8;
  const double dpydxt0 = 0;
  const double dpydyt0 = 0;
  const double dpzdqop0 = -x4*x9;
  const double dpzdlam0 = x7;
  const double dpzdphi0 = 0;
  const double dpzdxt0 = 0;
  const double dpzdyt0 = 0;
  Matrix<double, 6, 5> res;
  res(0,0) = dxdqop0;
  res(0,1) = dxdlam0;
  res(0,2) = dxdphi0;
  res(0,3) = dxdxt0;
  res(0,4) = dxdyt0;
  res(1,0) = dydqop0;
  res(1,1) = dydlam0;
  res(1,2) = dydphi0;
  res(1,3) = dydxt0;
  res(1,4) = dydyt0;
  res(2,0) = dzdqop0;
  res(2,1) = dzdlam0;
  res(2,2) = dzdphi0;
  res(2,3) = dzdxt0;
  res(2,4) = dzdyt0;
  res(3,0) = dpxdqop0;
  res(3,1) = dpxdlam0;
  res(3,2) = dpxdphi0;
  res(3,3) = dpxdxt0;
  res(3,4) = dpxdyt0;
  res(4,0) = dpydqop0;
  res(4,1) = dpydlam0;
  res(4,2) = dpydphi0;
  res(4,3) = dpydxt0;
  res(4,4) = dpydyt0;
  res(5,0) = dpzdqop0;
  res(5,1) = dpzdlam0;
  res(5,2) = dpzdphi0;
  res(5,3) = dpzdxt0;
  res(5,4) = dpzdyt0;
  
  return res;

  
}

Matrix<double, 6, 5> ResidualGlobalCorrectionMakerBase::curv2cartJacobianAltD(const Matrix<double, 7, 1> &state) const {
  
  const double charge = state[6];
  
  const double qop0 = charge/state.segment<3>(3).norm();
  const double lam0 = std::atan(state[5]/std::sqrt(state[3]*state[3] + state[4]*state[4]));
  const double phi0 = std::atan2(state[4], state[3]);
  
  const Matrix<double, 3, 1> W0 = state.segment<3>(3).normalized();
  const double W0x = W0[0];
  const double W0y = W0[1];
  const double W0z = W0[2];
    
  const double x0 = std::sqrt(std::pow(W0x, 2) + std::pow(W0y, 2));
  const double x1 = 1.0/x0;
  const double x2 = W0y*x1;
  const double x3 = W0x*x1;
  const double x4 = 1.0/qop0;
  const double x5 = std::cos(phi0);
  const double x6 = 1.0/std::fabs(qop0);
  const double x7 = x6*std::cos(lam0);
  const double x8 = x5*x7;
  const double x9 = x6*std::sin(lam0);
  const double x10 = std::sin(phi0);
  const double x11 = x10*x7;
  const double dxdqop0 = 0;
  const double dxdlam0 = 0;
  const double dxdphi0 = 0;
  const double dxdxt0 = -x2;
  const double dxdyt0 = -W0z*x3;
  const double dydqop0 = 0;
  const double dydlam0 = 0;
  const double dydphi0 = 0;
  const double dydxt0 = x3;
  const double dydyt0 = -W0z*x2;
  const double dzdqop0 = 0;
  const double dzdlam0 = 0;
  const double dzdphi0 = 0;
  const double dzdxt0 = 0;
  const double dzdyt0 = x0;
  const double dpxdqop0 = -x4*x8;
  const double dpxdlam0 = -x5*x9;
  const double dpxdphi0 = -x11;
  const double dpxdxt0 = 0;
  const double dpxdyt0 = 0;
  const double dpydqop0 = -x11*x4;
  const double dpydlam0 = -x10*x9;
  const double dpydphi0 = x8;
  const double dpydxt0 = 0;
  const double dpydyt0 = 0;
  const double dpzdqop0 = -x4*x9;
  const double dpzdlam0 = x7;
  const double dpzdphi0 = 0;
  const double dpzdxt0 = 0;
  const double dpzdyt0 = 0;
  Matrix<double, 6, 5> res;
  res(0,0) = dxdqop0;
  res(0,1) = dxdlam0;
  res(0,2) = dxdphi0;
  res(0,3) = dxdxt0;
  res(0,4) = dxdyt0;
  res(1,0) = dydqop0;
  res(1,1) = dydlam0;
  res(1,2) = dydphi0;
  res(1,3) = dydxt0;
  res(1,4) = dydyt0;
  res(2,0) = dzdqop0;
  res(2,1) = dzdlam0;
  res(2,2) = dzdphi0;
  res(2,3) = dzdxt0;
  res(2,4) = dzdyt0;
  res(3,0) = dpxdqop0;
  res(3,1) = dpxdlam0;
  res(3,2) = dpxdphi0;
  res(3,3) = dpxdxt0;
  res(3,4) = dpxdyt0;
  res(4,0) = dpydqop0;
  res(4,1) = dpydlam0;
  res(4,2) = dpydphi0;
  res(4,3) = dpydxt0;
  res(4,4) = dpydyt0;
  res(5,0) = dpzdqop0;
  res(5,1) = dpzdlam0;
  res(5,2) = dpzdphi0;
  res(5,3) = dpzdxt0;
  res(5,4) = dpzdyt0;
  
  return res;

  
}

Matrix<double, 5, 6> ResidualGlobalCorrectionMakerBase::localTransportJacobianAlt(const TrajectoryStateOnSurface &start,
                                            const std::pair<TrajectoryStateOnSurface, double> &propresult,
                                            bool doReverse) const {
  
  const TrajectoryStateOnSurface& state0 = doReverse ? propresult.first : start;
  const TrajectoryStateOnSurface& state1 = doReverse ? start : propresult.first;
  const GlobalVector& bfield = start.globalParameters().magneticFieldInInverseGeV();
  const double s = doReverse ? -propresult.second : propresult.second;
  
  const double qop0 = state0.localParameters().qbp();
  const double dxdz0 = state0.localParameters().dxdz();
  const double dydz0 = state0.localParameters().dydz();
  const double x0 = state0.localParameters().position().x();
  const double y0 = state0.localParameters().position().y();
  
  const double localpzsign = state0.localParameters().pzSign();
  
  const LocalVector lx(1.,0.,0.);
  const LocalVector ly(0.,1.,0.);
  const LocalVector lz(0.,0.,1.);
  const LocalPoint l0(0., 0.);
  
  const GlobalVector I0 = state0.surface().toGlobal(lz);
  const GlobalVector J0 = state0.surface().toGlobal(lx);
  const GlobalVector K0 = state0.surface().toGlobal(ly);  
  const GlobalPoint r0 = state0.surface().toGlobal(l0);
  
  const double Ix0 = I0.x();
  const double Iy0 = I0.y();
  const double Iz0 = I0.z();
  
  const double Jx0 = J0.x();
  const double Jy0 = J0.y();
  const double Jz0 = J0.z();
  
  const double Kx0 = K0.x();
  const double Ky0 = K0.y();
  const double Kz0 = K0.z();
  
  const double rx0 = r0.x();
  const double ry0 = r0.y();
  const double rz0 = r0.z();
  
  const GlobalVector I1 = state1.surface().toGlobal(lz);
  const GlobalVector J1 = state1.surface().toGlobal(lx);
  const GlobalVector K1 = state1.surface().toGlobal(ly);  
  const GlobalPoint r1 = state1.surface().toGlobal(l0);
  
  const double Ix1 = I1.x();
  const double Iy1 = I1.y();
  const double Iz1 = I1.z();
  
  const double Jx1 = J1.x();
  const double Jy1 = J1.y();
  const double Jz1 = J1.z();
  
  const double Kx1 = K1.x();
  const double Ky1 = K1.y();
  const double Kz1 = K1.z();
  
  const double rx1 = r1.x();
  const double ry1 = r1.y();
  const double rz1 = r1.z();
    
  const double B = bfield.mag();
  const GlobalVector H = bfield/B;
  const double hx = H.x();
  const double hy = H.y();
  const double hz = H.z();
    
  const double x1 = B*s;
  const double x2 = qop0*x1;
  const double x3 = std::cos(x2);
  const double x4 = Ix0 + Jx0*dxdz0 + Kx0*dydz0;
  const double x5 = x3*x4;
  const double x6 = std::sin(x2);
  const double x7 = Iy0 + Jy0*dxdz0 + Ky0*dydz0;
  const double x8 = hz*x7;
  const double x9 = Iz0 + Jz0*dxdz0 + Kz0*dydz0;
  const double x10 = hy*x9;
  const double x11 = -x10 + x8;
  const double x12 = x3 - 1;
  const double x13 = hx*x4 + hy*x7 + hz*x9;
  const double x14 = x12*x13;
  const double x15 = -hx*x14 + x11*x6 + x5;
  const double x16 = std::pow(dxdz0, 2) + std::pow(dydz0, 2) + 1;
  const double x17 = std::sqrt(x16);
  const double x18 = 1.0/x17;
  const double x19 = localpzsign*x18;
  const double x20 = x15*x19;
  const double x21 = x3*x7;
  const double x22 = hx*x9 - hz*x4;
  const double x23 = x22*x6;
  const double x24 = -hy*x14 + x21 + x23;
  const double x25 = x19*x24;
  const double x26 = x3*x9;
  const double x27 = hy*x4;
  const double x28 = hx*x7;
  const double x29 = x27 - x28;
  const double x30 = -hz*x14 + x26 + x29*x6;
  const double x31 = x19*x30;
  const double x32 = Ix1*x20 + Iy1*x25 + Iz1*x31;
  const double x33 = 1.0/x32;
  const double x34 = x1*x6;
  const double x35 = x1*x3;
  const double x36 = x13*x34;
  const double x37 = hx*x36 + x11*x35 - x34*x4;
  const double x38 = Jx1*x19;
  const double x39 = hy*x36 + x22*x35 - x34*x7;
  const double x40 = Jy1*x19;
  const double x41 = hz*x36 + x29*x35 - x34*x9;
  const double x42 = Jz1*x19;
  const double x43 = Ix1*x19;
  const double x44 = Iy1*x19;
  const double x45 = Iz1*x19;
  const double x46 = -x37*x43 - x39*x44 - x41*x45;
  const double x47 = std::pow(x32, -2);
  const double x48 = x47*(Jx1*x20 + Jy1*x25 + Jz1*x31);
  const double x49 = localpzsign*x6;
  const double x50 = Jx0*x0 + Kx0*y0 + rx0;
  const double x51 = B*qop0;
  const double x52 = x17*x51;
  const double x53 = x10 - x8;
  const double x54 = localpzsign*x12;
  const double x55 = x2 - x6;
  const double x56 = localpzsign*x13;
  const double x57 = hx*x56;
  const double x58 = x4*x49 + x50*x52 + x53*x54 + x55*x57;
  const double x59 = 1.0/B;
  const double x60 = x18*x59;
  const double x61 = x60/std::pow(qop0, 2);
  const double x62 = localpzsign*x1;
  const double x63 = B*x17;
  const double x64 = x1*x49;
  const double x65 = x1 - x35;
  const double x66 = 1.0/qop0;
  const double x67 = x60*x66;
  const double x68 = -x58*x61 + x67*(x5*x62 + x50*x63 - x53*x64 + x57*x65);
  const double x69 = Jy0*x0 + Ky0*y0 + ry0;
  const double x70 = hy*x56;
  const double x71 = -x22*x54 + x49*x7 + x52*x69 + x55*x70;
  const double x72 = -x61*x71 + x67*(x21*x62 + x23*x62 + x63*x69 + x65*x70);
  const double x73 = Jz0*x0 + Kz0*y0 + rz0;
  const double x74 = -x27 + x28;
  const double x75 = hz*x56;
  const double x76 = x49*x9 + x52*x73 + x54*x74 + x55*x75;
  const double x77 = -x61*x76 + x67*(x26*x62 + x63*x73 - x64*x74 + x65*x75);
  const double x78 = -Ix1*x68 - Iy1*x72 - Iz1*x77;
  const double x79 = x51*x6;
  const double x80 = x3*x51;
  const double x81 = x13*x79;
  const double x82 = hx*x81 + x11*x80 - x4*x79;
  const double x83 = hy*x81 + x22*x80 - x7*x79;
  const double x84 = hz*x81 + x29*x80 - x79*x9;
  const double x85 = -x43*x82 - x44*x83 - x45*x84;
  const double x86 = x33*(x33*(x38*x82 + x40*x83 + x42*x84) + x48*x85);
  const double x87 = Jy0*hz;
  const double x88 = Jz0*hy;
  const double x89 = Jx0*hx + Jy0*hy + Jz0*hz;
  const double x90 = x12*x89;
  const double x91 = Jx0*x3 - hx*x90 + x6*(x87 - x88);
  const double x92 = -Jx0*hz + Jz0*hx;
  const double x93 = Jy0*x3 - hy*x90 + x6*x92;
  const double x94 = Jx0*hy;
  const double x95 = Jy0*hx;
  const double x96 = Jz0*x3 - hz*x90 + x6*(x94 - x95);
  const double x97 = std::pow(x16, -3.0/2.0);
  const double x98 = dxdz0*x97;
  const double x99 = localpzsign*x98;
  const double x100 = x15*x99;
  const double x101 = x24*x99;
  const double x102 = x30*x99;
  const double x103 = Ix1*x100 + Iy1*x101 + Iz1*x102 - x43*x91 - x44*x93 - x45*x96;
  const double x104 = x18*x51;
  const double x105 = dxdz0*x104;
  const double x106 = localpzsign*x55;
  const double x107 = x106*x89;
  const double x108 = x58*x66;
  const double x109 = x59*x98;
  const double x110 = -x108*x109 + x67*(Jx0*x49 + hx*x107 + x105*x50 + x54*(-x87 + x88));
  const double x111 = x109*x66;
  const double x112 = -x111*x76 + x67*(Jz0*x49 + hz*x107 + x105*x73 + x54*(-x94 + x95));
  const double x113 = -x111*x71 + x67*(Jy0*x49 + hy*x107 + x105*x69 - x54*x92);
  const double x114 = -Ix1*x110 - Iy1*x113 - Iz1*x112;
  const double x115 = Ky0*hz;
  const double x116 = Kz0*hy;
  const double x117 = Kx0*hx + Ky0*hy + Kz0*hz;
  const double x118 = x117*x12;
  const double x119 = Kx0*x3 - hx*x118 + x6*(x115 - x116);
  const double x120 = -Kx0*hz + Kz0*hx;
  const double x121 = Ky0*x3 - hy*x118 + x120*x6;
  const double x122 = Kx0*hy;
  const double x123 = Ky0*hx;
  const double x124 = Kz0*x3 - hz*x118 + x6*(x122 - x123);
  const double x125 = dydz0*x97;
  const double x126 = localpzsign*x125;
  const double x127 = x126*x15;
  const double x128 = x126*x24;
  const double x129 = x126*x30;
  const double x130 = Ix1*x127 + Iy1*x128 + Iz1*x129 - x119*x43 - x121*x44 - x124*x45;
  const double x131 = dydz0*x104;
  const double x132 = x106*x117;
  const double x133 = x125*x59;
  const double x134 = -x108*x133 + x67*(Kx0*x49 + hx*x132 + x131*x50 + x54*(-x115 + x116));
  const double x135 = x133*x66;
  const double x136 = -x135*x76 + x67*(Kz0*x49 + hz*x132 + x131*x73 + x54*(-x122 + x123));
  const double x137 = -x135*x71 + x67*(Ky0*x49 + hy*x132 - x120*x54 + x131*x69);
  const double x138 = -Ix1*x134 - Iy1*x137 - Iz1*x136;
  const double x139 = -Ix1*Jx0 - Iy1*Jy0 - Iz1*Jz0;
  const double x140 = -Ix1*Kx0 - Iy1*Ky0 - Iz1*Kz0;
  const double x141 = qop0*s;
  const double x142 = x141*x6;
  const double x143 = x141*x3;
  const double x144 = x13*x142;
  const double x145 = hx*x144 + x11*x143 - x142*x4;
  const double x146 = hy*x144 - x142*x7 + x143*x22;
  const double x147 = hz*x144 - x142*x9 + x143*x29;
  const double x148 = -x145*x43 - x146*x44 - x147*x45;
  const double x149 = x18/std::pow(B, 2);
  const double x150 = localpzsign*x141;
  const double x151 = qop0*x17;
  const double x152 = x141*x49;
  const double x153 = x141 - x143;
  const double x154 = -x108*x149 + x67*(x150*x5 + x151*x50 - x152*x53 + x153*x57);
  const double x155 = x149*x66;
  const double x156 = -x155*x71 + x67*(x150*x21 + x150*x23 + x151*x69 + x153*x70);
  const double x157 = -x155*x76 + x67*(x150*x26 + x151*x73 - x152*x74 + x153*x75);
  const double x158 = -Ix1*x154 - Iy1*x156 - Iz1*x157;
  const double x159 = Kx1*x19;
  const double x160 = Ky1*x19;
  const double x161 = Kz1*x19;
  const double x162 = x47*(Kx1*x20 + Ky1*x25 + Kz1*x31);
  const double x163 = x33*(x162*x85 + x33*(x159*x82 + x160*x83 + x161*x84));
  const double x164 = localpzsign*x51;
  const double x165 = x51 - x80;
  const double x166 = x67*(x164*x21 + x164*x23 + x165*x70);
  const double x167 = x49*x51;
  const double x168 = x67*(x164*x5 + x165*x57 - x167*x53);
  const double x169 = x67*(x164*x26 + x165*x75 - x167*x74);
  const double x170 = x33*(Jx1*x168 + Jy1*x166 + Jz1*x169);
  const double x171 = x33*(Kx1*x168 + Ky1*x166 + Kz1*x169);
  const double dqopdqop0 = 1;
  const double dqopddxdz0 = 0;
  const double dqopddydz0 = 0;
  const double dqopdx0 = 0;
  const double dqopdy0 = 0;
  const double dqopdB = 0;
  const double ddxdzdqop0 = x33*(x37*x38 + x39*x40 + x41*x42) + x46*x48 + x78*x86;
  const double ddxdzddxdz0 = x103*x48 + x114*x86 + x33*(-Jx1*x100 - Jy1*x101 - Jz1*x102 + x38*x91 + x40*x93 + x42*x96);
  const double ddxdzddydz0 = x130*x48 + x138*x86 + x33*(-Jx1*x127 - Jy1*x128 - Jz1*x129 + x119*x38 + x121*x40 + x124*x42);
  const double ddxdzdx0 = x139*x86;
  const double ddxdzdy0 = x140*x86;
  const double ddxdzdB = x148*x48 + x158*x86 + x33*(x145*x38 + x146*x40 + x147*x42);
  const double ddydzdqop0 = x162*x46 + x163*x78 + x33*(x159*x37 + x160*x39 + x161*x41);
  const double ddydzddxdz0 = x103*x162 + x114*x163 + x33*(-Kx1*x100 - Ky1*x101 - Kz1*x102 + x159*x91 + x160*x93 + x161*x96);
  const double ddydzddydz0 = x130*x162 + x138*x163 + x33*(-Kx1*x127 - Ky1*x128 - Kz1*x129 + x119*x159 + x121*x160 + x124*x161);
  const double ddydzdx0 = x139*x163;
  const double ddydzdy0 = x140*x163;
  const double ddydzdB = x148*x162 + x158*x163 + x33*(x145*x159 + x146*x160 + x147*x161);
  const double dxdqop0 = Jx1*x68 + Jy1*x72 + Jz1*x77 + x170*x78;
  const double dxddxdz0 = Jx1*x110 + Jy1*x113 + Jz1*x112 + x114*x170;
  const double dxddydz0 = Jx1*x134 + Jy1*x137 + Jz1*x136 + x138*x170;
  const double dxdx0 = Jx0*Jx1 + Jy0*Jy1 + Jz0*Jz1 + x139*x170;
  const double dxdy0 = Jx1*Kx0 + Jy1*Ky0 + Jz1*Kz0 + x140*x170;
  const double dxdB = Jx1*x154 + Jy1*x156 + Jz1*x157 + x158*x170;
  const double dydqop0 = Kx1*x68 + Ky1*x72 + Kz1*x77 + x171*x78;
  const double dyddxdz0 = Kx1*x110 + Ky1*x113 + Kz1*x112 + x114*x171;
  const double dyddydz0 = Kx1*x134 + Ky1*x137 + Kz1*x136 + x138*x171;
  const double dydx0 = Jx0*Kx1 + Jy0*Ky1 + Jz0*Kz1 + x139*x171;
  const double dydy0 = Kx0*Kx1 + Ky0*Ky1 + Kz0*Kz1 + x140*x171;
  const double dydB = Kx1*x154 + Ky1*x156 + Kz1*x157 + x158*x171;
  Matrix<double, 5, 6> res;
  res(0,0) = dqopdqop0;
  res(0,1) = dqopddxdz0;
  res(0,2) = dqopddydz0;
  res(0,3) = dqopdx0;
  res(0,4) = dqopdy0;
  res(0,5) = dqopdB;
  res(1,0) = ddxdzdqop0;
  res(1,1) = ddxdzddxdz0;
  res(1,2) = ddxdzddydz0;
  res(1,3) = ddxdzdx0;
  res(1,4) = ddxdzdy0;
  res(1,5) = ddxdzdB;
  res(2,0) = ddydzdqop0;
  res(2,1) = ddydzddxdz0;
  res(2,2) = ddydzddydz0;
  res(2,3) = ddydzdx0;
  res(2,4) = ddydzdy0;
  res(2,5) = ddydzdB;
  res(3,0) = dxdqop0;
  res(3,1) = dxddxdz0;
  res(3,2) = dxddydz0;
  res(3,3) = dxdx0;
  res(3,4) = dxdy0;
  res(3,5) = dxdB;
  res(4,0) = dydqop0;
  res(4,1) = dyddxdz0;
  res(4,2) = dyddydz0;
  res(4,3) = dydx0;
  res(4,4) = dydy0;
  res(4,5) = dydB;
  
  res.col(5) *= 2.99792458e-3;
  
  return res;
                                              
}

Matrix<double, 5, 6> ResidualGlobalCorrectionMakerBase::curv2localTransportJacobian(const FreeTrajectoryState& start,
                                              const std::pair<TrajectoryStateOnSurface, double>& propresult,
                                              bool doReverse) const {
        
  const FreeTrajectoryState& end = *propresult.first.freeState();
  const TrajectoryStateOnSurface& proptsos = propresult.first;
  
  const FreeTrajectoryState& state0 = doReverse ? end : start;
  const FreeTrajectoryState& state1 = doReverse ? start : end;
  const GlobalVector& h = start.parameters().magneticFieldInInverseGeV();
  const double s = doReverse ? -propresult.second : propresult.second;
  
//   // compute transport jacobian wrt curvlinear parameters
//   AnalyticalCurvilinearJacobian curv2curv;
//   curv2curv.computeFullJacobian(state0.parameters(), state1.parameters().position(), state1.parameters().momentum(), h, s);
//   const AlgebraicMatrix55& curv2curvjac = curv2curv.jacobian();
//   const Matrix<double, 5, 5> F = Map<const Matrix<double, 5, 5, RowMajor>>(curv2curvjac.Array());
//   
//   // compute transport jacobian wrt B field (magnitude)
//   const Matrix<double, 5, 1> dF = bfieldJacobian(state0.parameters(), state1.parameters(), s, h);
  
  const Matrix<double, 5, 6> FdF = curvtransportJacobian(state0.parameters(), state1.parameters(), s, h);
  const Matrix<double, 5, 5> F = FdF.topLeftCorner<5,5>();
  const Matrix<double, 5, 1> dF = FdF.col(5);
  
  Matrix<double, 5, 6> res;
  
  if (doReverse) {
    // compute local to curvilinear jacobian at source
    JacobianLocalToCurvilinear local2curv(proptsos.surface(), proptsos.localParameters(), *proptsos.magneticField());
    const AlgebraicMatrix55& local2curvjac = local2curv.jacobian();
    const Matrix<double, 5, 5> H0 = Map<const Matrix<double, 5, 5, RowMajor>>(local2curvjac.Array());
    
    res.leftCols<5>() = F*H0;
    res.rightCols<1>() = dF;
  }
  else {
    // compute curvilinear to local jacobian at destination
    JacobianCurvilinearToLocal curv2local(proptsos.surface(), proptsos.localParameters(), *proptsos.magneticField());
    const AlgebraicMatrix55& curv2localjac = curv2local.jacobian();
    const Matrix<double, 5, 5> H1 = Map<const Matrix<double, 5, 5, RowMajor>>(curv2localjac.Array());
    
    res.leftCols<5>() = H1*F;
    res.rightCols<1>() = H1*dF;
  }
  
  return res;
                                                
}

Matrix<double, 5, 6> ResidualGlobalCorrectionMakerBase::curv2curvTransportJacobian(const FreeTrajectoryState& start,
                                              const std::pair<TrajectoryStateOnSurface, double>& propresult,
                                              bool doReverse) const {
        
  const FreeTrajectoryState& end = *propresult.first.freeState();
  const TrajectoryStateOnSurface& proptsos = propresult.first;
  
  const FreeTrajectoryState& state0 = doReverse ? end : start;
  const FreeTrajectoryState& state1 = doReverse ? start : end;
  const GlobalVector& h = start.parameters().magneticFieldInInverseGeV();
//   const GlobalVector h = 0.5*(start.parameters().magneticFieldInInverseGeV() + end.parameters().magneticFieldInInverseGeV());
  const double s = doReverse ? -propresult.second : propresult.second;
  
//   // compute transport jacobian wrt curvlinear parameters
//   AnalyticalCurvilinearJacobian curv2curv;
//   curv2curv.computeFullJacobian(state0.parameters(), state1.parameters().position(), state1.parameters().momentum(), h, s);
//   const AlgebraicMatrix55& curv2curvjac = curv2curv.jacobian();
//   const Matrix<double, 5, 5> F = Map<const Matrix<double, 5, 5, RowMajor>>(curv2curvjac.Array());
//   
//   // compute transport jacobian wrt B field (magnitude)
//   const Matrix<double, 5, 1> dF = bfieldJacobian(state0.parameters(), state1.parameters(), s, h);
  
  const Matrix<double, 5, 6> FdF = curvtransportJacobian(state0.parameters(), state1.parameters(), s, h);

  return FdF;
}

AlgebraicVector5 ResidualGlobalCorrectionMakerBase::localMSConvolution(const TrajectoryStateOnSurface& tsos, const MaterialEffectsUpdator& updator) const {
  
  const LocalVector lx(1.,0.,0.);
  const LocalVector ly(0.,1.,0.);
  const GlobalVector J = tsos.surface().toGlobal(lx);
  const GlobalVector K = tsos.surface().toGlobal(ly);
    
  const double Jx = J.x();
  const double Jy = J.y();
  const double Jz = J.z();
  
  const double Kx = K.x();
  const double Ky = K.y();
  const double Kz = K.z();
  
  const Matrix<double, 3, 1> p0(tsos.globalMomentum().x(), tsos.globalMomentum().y(), tsos.globalMomentum().z());
  const Matrix<double, 3, 1> W0 = p0.normalized();
  const Matrix<double, 3, 1> zhat(0., 0., 1.);
  
  const Matrix<double, 3, 1> U0 = zhat.cross(W0).normalized();
  const Matrix<double, 3, 1> V0 = W0.cross(U0);
    
  const double Ux = U0[0];
  const double Uy = U0[1];
  
  const double Vx = V0[0];
  const double Vy = V0[1];
  const double Vz = V0[2];
  
  const double Wx = W0.x();
  const double Wy = W0.y();
  const double Wz = W0.z();
  
  const double x0 = std::pow(Wx, 2) + std::pow(Wy, 2) + std::pow(Wz, 2);
  const double x1 = std::pow(x0, -1.0/2.0);
  const double x2 = Ux*Wx + Uy*Wy;
  const double x3 = 1.0/x0;
  const double x4 = 2*x3;
  const double x5 = x2*x4;
  const double x6 = 3*x3;
  const double x7 = std::pow(Ux, 2) + std::pow(Uy, 2) - std::pow(x2, 2)*x6;
  const double x8 = x3*(Jx*Wx + Jy*Wy + Jz*Wz);
  const double x9 = Vx*Wx + Vy*Wy;
  const double x10 = x4*x9;
  const double x11 = std::pow(Vx, 2) + std::pow(Vy, 2) - x6*std::pow(x9, 2);
  const double x12 = x3*(Kx*Wx + Ky*Wy + Kz*Wz);
  const double d2dxdzdthetau2 = x1*(-x5*(Jx*Ux + Jy*Uy) - x7*x8);
  const double d2dxdzdthetav2 = x1*(-x10*(Jx*Vx + Jy*Vy) - x11*x8);
  const double d2dydzdthetau2 = x1*(-x12*x7 - x5*(Kx*Ux + Ky*Uy));
  const double d2dydzdthetav2 = x1*(-x10*(Kx*Vx + Ky*Vy) - x11*x12);

  
  
  const Surface& surface = tsos.surface();
  //
  //
  // Now get information on medium
  //
  const MediumProperties& mp = surface.mediumProperties();

  // Momentum vector
  LocalVector d = tsos.localMomentum();
  float p2 = d.mag2();
  d *= 1.f / sqrt(p2);
  float xf = 1.f / std::abs(d.z());  // increase of path due to angle of incidence
  // calculate general physics things
  constexpr float amscon = 1.8496e-4;  // (13.6MeV)**2
  const float m2 = updator.mass() * updator.mass();    // use mass hypothesis from constructor
  float e2 = p2 + m2;
  float beta2 = p2 / e2;
  // calculate the multiple scattering angle
  float radLen = mp.radLen() * xf;  // effective rad. length
  float sigt2 = 0.;                 // sigma(alpha)**2

  // Calculated rms scattering angle squared.
  float fact = 1.f + 0.038f * unsafe_logf<2>(radLen);
  fact *= fact;
  float a = fact / (beta2 * p2);
  sigt2 = amscon * radLen * a;
  
  const double sigma2 = sigt2;
  
  AlgebraicVector5 res;
  res[0] = 0.;
  res[1] = 0.5*d2dxdzdthetau2*sigma2 + 0.5*d2dxdzdthetav2*sigma2;
  res[2] = 0.5*d2dydzdthetau2*sigma2 + 0.5*d2dydzdthetav2*sigma2;
  res[3] = 0.;
  res[4] = 0.;

  return res;
}

// Matrix<double, 5, 6> ResidualGlobalCorrectionMakerBase::materialEffectsJacobian(const TrajectoryStateOnSurface& tsos, const MaterialEffectsUpdator& updator) {
//   
//   //jacobian of local parameters with respect to initial local parameters and material parameter xi
//   //n.b this is the jacobian in LOCAL parameters (so E multiplies to the left of H s.t the total projection is E*Hprop*F)
//   
//   const double m2 = pow(updator.mass(), 2);  // use mass hypothesis from constructor
//   constexpr double emass = 0.511e-3;
//   constexpr double poti = 16.e-9 * 10.75;                 // = 16 eV * Z**0.9, for Si Z=14
//   const double eplasma = 28.816e-9 * sqrt(2.33 * 0.498);  // 28.816 eV * sqrt(rho*(Z/A)) for Si
//   const double qop = tsos.localParameters().qbp();
//   const double dxdz = tsos.localParameters().dxdz();
//   const double dydz = tsos.localParameters().dydz();
//   const double xi = tsos.surface().mediumProperties().xi();
//   const double scale = 26.;
// 
//   //this is printed from sympy.printing.cxxcode together with sympy.cse for automatic substitution of common expressions
//   const double x0 = (((qop) > 0) - ((qop) < 0));
//   const double x1 = std::pow(qop, 2);
//   const double x2 = 1.0/(m2*x1 + 1);
//   const double x3 = std::pow(x2, -1.0/2.0);
//   const double x4 = std::sqrt(std::pow(dxdz, 2) + std::pow(dydz, 2) + 1);
//   const double x5 = scale*x3*x4;
//   const double x6 = x5*xi;
//   const double x7 = x0/std::pow(-x6 + x0/qop, 2);
//   const double x8 = scale*x3*x7*xi/x4;
//   const double res_0 = x7*(m2*qop*x2*x6 + x0/x1);
//   const double res_1 = dxdz*x8;
//   const double res_2 = dydz*x8;
//   const double res_3 = x5*x7;
//   
//   Matrix<double, 5, 6> EdE = Matrix<double, 5, 6>::Zero();
//   //jacobian of q/p wrt local state parameters
//   EdE(0,0) = res_0;
//   EdE(0,1) = res_1;
//   EdE(0,2) = res_2;
//   EdE(1,1) = 1.;
//   EdE(2,2) = 1.;
//   EdE(3,3) = 1.;
//   EdE(4,4) = 1.;
//   //derivative of q/p wrt xi
//   EdE(0,5) = res_3;
//   
//   return EdE;
// }


// Matrix<double, 5, 6> ResidualGlobalCorrectionMakerBase::materialEffectsJacobian(const TrajectoryStateOnSurface& tsos, const MaterialEffectsUpdator& updator) {
//   
//   //jacobian of local parameters with respect to initial local parameters and material parameter xi
//   //n.b this is the jacobian in LOCAL parameters (so E multiplies to the left of H s.t the total projection is E*Hprop*F)
//   
//   const double m2 = pow(updator.mass(), 2);  // use mass hypothesis from constructor
//   constexpr double emass = 0.511e-3;
//   constexpr double poti = 16.e-9 * 10.75;                 // = 16 eV * Z**0.9, for Si Z=14
//   const double eplasma = 28.816e-9 * sqrt(2.33 * 0.498);  // 28.816 eV * sqrt(rho*(Z/A)) for Si
//   const double qop = tsos.localParameters().qbp();
//   const double dxdz = tsos.localParameters().dxdz();
//   const double dydz = tsos.localParameters().dydz();
//   const double xi = tsos.surface().mediumProperties().xi();
// 
//   //this is printed from sympy.printing.cxxcode together with sympy.cse for automatic substitution of common expressions
//   const double x0 = (((qop) > 0) - ((qop) < 0));
//   const double x1 = std::pow(qop, 2);
//   const double x2 = 1.0/x1;
//   const double x3 = m2*x1 + 1;
//   const double x4 = 1.0/x3;
//   const double x5 = 1.0/poti;
//   const double x6 = std::sqrt(std::pow(dxdz, 2) + std::pow(dydz, 2) + 1);
//   const double x7 = x3*xi;
//   const double x8 = -2*std::log(eplasma*x5) + std::log(x5*x6*x7) + std::log(2.0*emass*x2/(m2*std::pow(poti, 2))) + 1.2;
//   const double x9 = x3*x8 - 1;
//   const double x10 = std::pow(x4, -1.0/2.0);
//   const double x11 = x10*x6;
//   const double x12 = x11*x9;
//   const double x13 = x12*xi;
//   const double x14 = 1.0/qop;
//   const double x15 = 2*m2*qop;
//   const double x16 = x0/std::pow(x0*x14 - x13, 2);
//   const double x17 = x10/x6;
//   const double x18 = dxdz*x17;
//   const double x19 = x9*xi;
//   const double x20 = dydz*x17;
//   const double res_0 = x16*(m2*qop*x13*x4 + x0*x2 + x11*xi*(x15*x8 + x3*(-2.0*x14 + x15*x4)));
//   const double res_1 = x16*(x18*x19 + x18*x7);
//   const double res_2 = x16*(x19*x20 + x20*x7);
//   const double res_3 = x16*(x11*x3 + x12);
// 
//   
//   Matrix<double, 5, 6> EdE = Matrix<double, 5, 6>::Zero();
//   //jacobian of q/p wrt local state parameters
//   EdE(0,0) = res_0;
//   EdE(0,1) = res_1;
//   EdE(0,2) = res_2;
//   EdE(1,1) = 1.;
//   EdE(2,2) = 1.;
//   EdE(3,3) = 1.;
//   EdE(4,4) = 1.;
//   //derivative of q/p wrt xi
//   EdE(0,5) = res_3;
//   
//   return EdE;
// }

Matrix<double, 5, 6> ResidualGlobalCorrectionMakerBase::materialEffectsJacobian(const TrajectoryStateOnSurface& tsos, const MaterialEffectsUpdator& updator) {
  
  //jacobian of local parameters with respect to initial local parameters and material parameter xi
  //n.b this is the jacobian in LOCAL parameters (so E multiplies to the left of H s.t the total projection is E*Hprop*F)
  
  const double m2 = pow(updator.mass(), 2);  // use mass hypothesis from constructor
  constexpr double emass = 0.511e-3;
  constexpr double poti = 16.e-9 * 10.75;                 // = 16 eV * Z**0.9, for Si Z=14
  const double eplasma = 28.816e-9 * sqrt(2.33 * 0.498);  // 28.816 eV * sqrt(rho*(Z/A)) for Si
  const double qop = tsos.localParameters().qbp();
  const double dxdz = tsos.localParameters().dxdz();
  const double dydz = tsos.localParameters().dydz();
  const double xi = tsos.surface().mediumProperties().xi();

  //this is printed from sympy.printing.cxxcode together with sympy.cse for automatic substitution of common expressions
  const double x0 = (((qop) > 0) - ((qop) < 0));
  const double x1 = std::pow(qop, 2);
  const double x2 = 1.0/x1;
  const double x3 = x0*x2;
  const double x4 = m2*x1 + 1;
  const double x5 = 1.0/x4;
  const double x6 = std::pow(poti, 2);
  const double x7 = 1.0/x6;
  const double x8 = std::fabs(qop);
  const double x9 = std::pow(emass, 2);
  const double x10 = x8*x9;
  const double x11 = std::sqrt(x4);
  const double x12 = 2*emass;
  const double x13 = m2*x8 + x10 + x11*x12;
  const double x14 = 1.0/x13;
  const double x15 = x10*x14*x7;
  const double x16 = 4*x2;
  const double x17 = 2*std::log(eplasma/poti) - std::log(x15*x16) - 1;
  const double x18 = x17*x4 + 2;
  const double x19 = std::pow(x5, -1.0/2.0);
  const double x20 = std::sqrt(std::pow(dxdz, 2) + std::pow(dydz, 2) + 1);
  const double x21 = x19*x20*xi;
  const double x22 = x18*x21;
  const double x23 = m2*qop;
  const double x24 = x0/std::pow(x22 + x0/qop, 2);
  const double x25 = x18*x19*x24;
  const double x26 = x25*xi/x20;
  const double res_0 = x24*(-x21*(-1.0/4.0*x1*x13*x4*x6*(x10*x16*x7*(-m2*x0 - x0*x9 - x12*x23/x11)/std::pow(x13, 2) + 4*x14*x3*x7*x9 - 8*x15/std::pow(qop, 3))/(x8*x9) + 2*x17*x23) - x22*x23*x5 + x3);
  const double res_1 = -dxdz*x26;
  const double res_2 = -dydz*x26;
  const double res_3 = -x20*x25;
  
  Matrix<double, 5, 6> EdE = Matrix<double, 5, 6>::Zero();
  //jacobian of q/p wrt local state parameters
  EdE(0,0) = res_0;
  EdE(0,1) = res_1;
  EdE(0,2) = res_2;
  EdE(1,1) = 1.;
  EdE(2,2) = 1.;
  EdE(3,3) = 1.;
  EdE(4,4) = 1.;
  //derivative of q/p wrt xi
  EdE(0,5) = res_3;
  
  return EdE;
}

Matrix<double, 5, 6> ResidualGlobalCorrectionMakerBase::materialEffectsJacobianVar(const TrajectoryStateOnSurface& tsos, const MaterialEffectsUpdator& updator) {
  
  //jacobian of local parameters with respect to initial local parameters and material parameter xi
  //n.b this is the jacobian in LOCAL parameters (so E multiplies to the left of H s.t the total projection is E*Hprop*F)
  
  const double m2 = pow(updator.mass(), 2);  // use mass hypothesis from constructor
  constexpr double emass = 0.511e-3;
  constexpr double poti = 16.e-9 * 10.75;                 // = 16 eV * Z**0.9, for Si Z=14
  const double eplasma = 28.816e-9 * sqrt(2.33 * 0.498);  // 28.816 eV * sqrt(rho*(Z/A)) for Si
  const double qop = tsos.localParameters().qbp();
  const double dxdz = tsos.localParameters().dxdz();
  const double dydz = tsos.localParameters().dydz();
  const double xi = tsos.surface().mediumProperties().xi();

  //this is printed from sympy.printing.cxxcode together with sympy.cse for automatic substitution of common expressions
  const double x0 = std::pow(qop, 2);
  const double x1 = 1.0/x0;
  const double x2 = (((qop) > 0) - ((qop) < 0));
  const double x3 = m2*x0 + 1;
  const double x4 = std::fabs(qop);
  const double x5 = std::pow(emass, 2)*x4;
  const double x6 = (x3*(2*std::log(eplasma/poti) - std::log(4*x1*x5/(std::pow(poti, 2)*(2*emass*std::sqrt(x3) + m2*x4 + x5))) - 1) + 2)*std::sqrt(std::pow(dxdz, 2) + std::pow(dydz, 2) + 1)/std::sqrt(1.0/x3);
  const double x7 = std::pow(x6*xi + x2/qop, -2);
  const double res_0 = x1*x7;
  const double res_1 = 0;
  const double res_2 = 0;
  const double res_3 = -x2*x6*x7;
  
  Matrix<double, 5, 6> EdE = Matrix<double, 5, 6>::Zero();
  //jacobian of q/p wrt local state parameters
  EdE(0,0) = res_0;
  EdE(0,1) = res_1;
  EdE(0,2) = res_2;
  EdE(1,1) = 1.;
  EdE(2,2) = 1.;
  EdE(3,3) = 1.;
  EdE(4,4) = 1.;
  //derivative of q/p wrt xi
  EdE(0,5) = res_3;
  
  return EdE;
}


std::array<Matrix<double, 5, 5>, 5> ResidualGlobalCorrectionMakerBase::processNoiseJacobians(const TrajectoryStateOnSurface& tsos, const MaterialEffectsUpdator& updator) const {
  
  //this is the variation of the process noise matrix with respect to the relevant local parameters (qop, dxdz, dydz) and the material parameters xi and radlen
  
  const double m2 = pow(updator.mass(), 2);  // use mass hypothesis from constructor
  constexpr double emass = 0.511e-3;
  constexpr double logfact = 0.038;
  constexpr double amscon = 1.8496e-4;  // (13.6MeV)**2
  const double qop = tsos.localParameters().qbp();
  const double dxdz = tsos.localParameters().dxdz();
  const double dydz = tsos.localParameters().dydz();
  const double signpz = tsos.localParameters().pzSign();
  const double xi = tsos.surface().mediumProperties().xi();
  const double radLen = tsos.surface().mediumProperties().radLen();
    
  const double x0 = std::pow(qop, 5);
  const double x1 = std::pow(qop, 2);
  const double x2 = 1.0/x1;
  const double x3 = m2 + x2;
  const double x4 = 1.0/x3;
  const double x5 = x2*x4;
  const double x6 = 1 - 1.0/2.0*x5;
  const double x7 = std::pow(dxdz, 2);
  const double x8 = std::pow(dydz, 2);
  const double x9 = x7 + x8;
  const double x10 = x9 + 1;
  const double x11 = std::sqrt(x10);
  const double x12 = x11*xi;
  const double x13 = x12*x6;
  const double x14 = std::pow(x3, 2);
  const double x15 = std::pow(emass, 2);
  const double x16 = 1.0/m2;
  const double x17 = std::sqrt(x3);
  const double x18 = emass*x16;
  const double x19 = 2*x18;
  const double x20 = x15*x16 + x17*x19 + 1;
  const double x21 = 1.0/x20;
  const double x22 = x19*x21;
  const double x23 = x14*x22;
  const double x24 = x18*x21;
  const double x25 = 8*x24;
  const double x26 = std::fabs(qop);
  const double x27 = (((qop) > 0) - ((qop) < 0));
  const double x28 = 1.0/x27;
  const double x29 = x28/x26;
  const double x30 = x13*x29;
  const double x31 = std::pow(qop, 4);
  const double x32 = x3*x31;
  const double x33 = 14*x24;
  const double x34 = std::pow(x3, 3.0/2.0);
  const double x35 = std::pow(qop, 3);
  const double x36 = 1.0/x14;
  const double x37 = std::pow(qop, 7)*x23*x29;
  const double x38 = x37*x6;
  const double x39 = x38*xi/x11;
  const double x40 = std::pow(signpz, 2);
  const double x41 = x40/x10;
  const double x42 = x41*x7;
  const double x43 = x41*x8;
  const double x44 = x42 + x43;
  const double x45 = 1.0/x44;
  const double x46 = 1.0*x45;
  const double x47 = std::pow(x10, 2);
  const double x48 = 1.0/x47;
  const double x49 = std::pow(signpz, 4);
  const double x50 = x48*x49;
  const double x51 = x46*x50;
  const double x52 = x42*x46 + x51*x8;
  const double x53 = amscon*radLen;
  const double x54 = 1.0/x49;
  const double x55 = std::pow(x10, 5.0/2.0)*x54;
  const double x56 = x53*x55;
  const double x57 = x52*x56;
  const double x58 = radLen*x11;
  const double x59 = x29*x58;
  const double x60 = logfact*std::log(qop*x59) + 1;
  const double x61 = std::pow(x60, 2);
  const double x62 = x3*x35;
  const double x63 = 1.0*x62;
  const double x64 = x61*x63;
  const double x65 = x29*x61;
  const double x66 = x57*x65;
  const double x67 = 2.0*x1;
  const double x68 = amscon*x52;
  const double x69 = logfact*x60;
  const double x70 = x54*x69;
  const double x71 = 1.0/qop;
  const double x72 = 2.0*x3;
  const double x73 = x31*x72*(-x58*x71 + x59);
  const double x74 = x47*x70*x73;
  const double x75 = std::pow(x10, 3.0/2.0);
  const double x76 = x53*x75;
  const double x77 = dxdz*x76;
  const double x78 = x52*x77;
  const double x79 = x0*x29*x72;
  const double x80 = x70*x79;
  const double x81 = x3*x65;
  const double x82 = x0*x81;
  const double x83 = 5.0*x54*x82;
  const double x84 = 2.0*x45;
  const double x85 = dxdz*x84;
  const double x86 = std::pow(dxdz, 3);
  const double x87 = x40*x48;
  const double x88 = x84*x87;
  const double x89 = dxdz*x8;
  const double x90 = 4.0*x45*x49/std::pow(x10, 3);
  const double x91 = 2*x41;
  const double x92 = 2*x87;
  const double x93 = 1.0/std::pow(x44, 2);
  const double x94 = x93*(-dxdz*x91 + x86*x92 + x89*x92);
  const double x95 = x50*x8;
  const double x96 = 1.0*x82;
  const double x97 = x56*x96;
  const double x98 = dydz*x76;
  const double x99 = x80*x98;
  const double x100 = x83*x98;
  const double x101 = dydz*x84;
  const double x102 = std::pow(dydz, 3);
  const double x103 = dydz*x7;
  const double x104 = x93*(-dydz*x91 + x102*x92 + x103*x92);
  const double x105 = x55*x68;
  const double x106 = x69*x79;
  const double x107 = x26*x27;
  const double x108 = x107*x61;
  const double x109 = 1.0/x40;
  const double x110 = dxdz*dydz*x109;
  const double x111 = x110*x76;
  const double x112 = x1*x3;
  const double x113 = x112*x61;
  const double x114 = amscon*x110;
  const double x115 = x108*x63;
  const double x116 = x109*x115;
  const double x117 = amscon*x109*x58;
  const double x118 = x103*x117;
  const double x119 = x107*x60;
  const double x120 = 2.0*x62;
  const double x121 = logfact*x119*x120;
  const double x122 = 3.0*x108*x62;
  const double x123 = x117*x89;
  const double x124 = x114*x75;
  const double x125 = x43*x46 + x51*x7;
  const double x126 = x125*x56;
  const double x127 = amscon*x125;
  const double x128 = x125*x77;
  const double x129 = x50*x7;
  const double x130 = x127*x55;
  const double x131 = -x5;
  const double x132 = x131 + 1;
  const double x133 = qop*x3;
  const double x134 = x131 + 2;
  const double x135 = x132*x29;
  const double x136 = x134*x14;
  const double x137 = x112*x29;
  const double x138 = x136*x29;
  const double x139 = x134*x29;
  const double x140 = x1*x139;
  const double x141 = qop*x22;
  const double x142 = 4*x24;
  const double x143 = 4.0*x61;
  const double x144 = logfact*(x29 - x71);
  const double x145 = x144*x60;
  const double x146 = -8.0*qop*x145;
  const double x147 = 20.0*x62;
  const double x148 = 4.0*x112;
  const double x149 = x144*(-x119*x2 + x144 + x60*x71);
  const double x150 = x56*(-14.0*qop*x65 - 8.0*x113 + x143 + x145*x147 - x145*x148*x26*x28 + x146 + x147*x65 + x149*x79)/x9;
  const double x151 = 6.0*x108;
  const double delosdqop = std::pow(qop, 6)*x14*x30*x33 - x0*x13*x23 + x12*x37*(x4/x35 - x36/x0) - x25*x30*x32 + 4*x15*x30*x31*x34/(std::pow(m2, 2)*std::pow(x20, 2));
  const double delosddxdz = dxdz*x39;
  const double delosddydz = dydz*x39;
  const double delosdxi = x11*x38;
  const double delosdradLen = 0;
  const double dmsxxdqop = 5.0*x32*x66 - x57*x64 - x66*x67 + x68*x74;
  const double dmsxxddxdz = x78*x80 + x78*x83 + x97*(x41*x85 + x42*x94 - x86*x88 - x89*x90 + x94*x95);
  const double dmsxxddydz = x100*x52 + x52*x99 + x97*(x101*x50 - x102*x90 - x103*x88 + x104*x42 + x104*x95);
  const double dmsxxdxi = 0;
  const double dmsxxdradLen = x105*x106 + x105*x96;
  const double dmsxydqop = x10*x114*x69*x73 + 3.0*x107*x111*x113 - 2.0*x108*x111 + x111*x64;
  const double dmsxyddxdz = x116*x98 + x118*x121 + x118*x122;
  const double dmsxyddydz = x116*x77 + x121*x123 + x122*x123;
  const double dmsxydxi = 0;
  const double dmsxydradLen = x115*x124 + x121*x124;
  const double dmsyydqop = 5.0*x126*x31*x81 - x126*x64 - x126*x65*x67 + x127*x74;
  const double dmsyyddxdz = x128*x80 + x128*x83 + x97*(x129*x94 + x43*x94 + x50*x85 - x8*x85*x87 - x86*x90);
  const double dmsyyddydz = x100*x125 + x125*x99 + x97*(x101*x41 - x102*x88 - x103*x90 + x104*x129 + x104*x43);
  const double dmsyydxi = 0;
  const double dmsyydradLen = x106*x130 + x130*x96;
  const double d2elosdqop2 = x12*x141*(x1*x138*x24*(x142*x5 + x2/x34 - 3/x17) + 14*x112*x135 - 2*x132*x133 + 4*x133*x134 - 28*x134*x137 - x134*x141*x34 + x135*x142*x17 - 8*x135 - 6*x136*x35 - x137*(-7*x5 + 3 + 4*x36/x31) + 21*x138*x31 - x139*x17*x25 + x140*x33*x34 + 2*x140*(3*m2 + 5*x2));
  const double d2msxxdqop2 = x150*(x43 + x7);
  const double d2msxydqop2 = x111*(x107*x120*x149 + 6.0*x113 + x119*x144*x148 + x133*x151 - x143 + 12.0*x145*x62 + x146 - x151*x71);
  const double d2msyydqop2 = x150*(x42 + x8);
  
  std::array<Matrix<double, 5, 5>, 5> res;
  
  Matrix<double, 5, 5> &dQdqop = res[0];
  dQdqop = Matrix<double, 5, 5>::Zero();
  dQdqop(0,0) = delosdqop;
  dQdqop(1,1) = dmsxxdqop;
  dQdqop(1,2) = dmsxydqop;
  dQdqop(2,1) = dmsxydqop;
  dQdqop(2,2) = dmsyydqop;
  
//   std::cout << "dQdqop" << std::endl;
//   std::cout << dQdqop << std::endl;
  
//   Matrix<double, 5, 5> &d2Qdqop2 = res[1];
//   d2Qdqop2 = Matrix<double, 5, 5>::Zero();
//   d2Qdqop2(0,0) = d2elosdqop2;
//   d2Qdqop2(1,1) = d2msxxdqop2;
//   d2Qdqop2(1,2) = d2msxydqop2;
//   d2Qdqop2(2,1) = d2msxydqop2;
//   d2Qdqop2(2,2) = d2msyydqop2;
  
  Matrix<double, 5, 5> &dQddxdz = res[1];
  dQddxdz = Matrix<double, 5, 5>::Zero();
//   dQddxdz(0,0) = delosddxdz;
//   dQddxdz(1,1) = dmsxxddxdz;
//   dQddxdz(1,2) = dmsxyddxdz;
//   dQddxdz(2,1) = dmsxyddxdz;
//   dQddxdz(2,2) = dmsyyddxdz;
  
  Matrix<double, 5, 5> &dQddydz = res[2];
  dQddydz = Matrix<double, 5, 5>::Zero();
//   dQddydz(0,0) = delosddydz;
//   dQddydz(1,1) = dmsxxddydz;
//   dQddydz(1,2) = dmsxyddydz;
//   dQddydz(2,1) = dmsxyddydz;
//   dQddydz(2,2) = dmsyyddydz;
  
  Matrix<double, 5, 5> &dQdxi = res[3];
  dQdxi = Matrix<double, 5, 5>::Zero();
  dQdxi(0,0) = delosdxi;
  dQdxi(1,1) = dmsxxdxi;
  dQdxi(1,2) = dmsxydxi;
  dQdxi(2,1) = dmsxydxi;
  dQdxi(2,2) = dmsyydxi;
  
  Matrix<double, 5, 5> &dQdradLen = res[4];
  dQdradLen = Matrix<double, 5, 5>::Zero();
  dQdradLen(0,0) = delosdradLen;
  dQdradLen(1,1) = dmsxxdradLen;
  dQdradLen(1,2) = dmsxydradLen;
  dQdradLen(2,1) = dmsxydradLen;
  dQdradLen(2,2) = dmsyydradLen;
  
  return res;
}

Matrix<double, 2, 1> ResidualGlobalCorrectionMakerBase::localPositionConvolution(const TrajectoryStateOnSurface& tsos, const Matrix<double, 5, 5> &curvcov) const {
  
  // curvilinear parameters
  const CurvilinearTrajectoryParameters curv(tsos.globalPosition(), tsos.globalMomentum(), tsos.charge());
  const double qop = curv.Qbp();
  const double lam = curv.lambda();
  const double phi = curv.phi();
//   const double xt = curv.xT();
//   const double yt = curv.yT();
  const double xt = 0.;
  const double yt = 0.;
  
  const Matrix<double, 3, 1> p0(tsos.globalMomentum().x(), tsos.globalMomentum().y(), tsos.globalMomentum().z());
  const Matrix<double, 3, 1> W0 = p0.normalized();
  const Matrix<double, 3, 1> zhat(0., 0., 1.);
  
  const Matrix<double, 3, 1> U0 = zhat.cross(W0).normalized();
  const Matrix<double, 3, 1> V0 = W0.cross(U0);
  
//   std::cout << "U0" << std::endl;
//   std::cout << U0 << std::endl;
//   std::cout << "V0" << std::endl;
//   std::cout << V0 << std::endl;
  
//   const Matrix<double, 3, 1> x0alt = xt*U0 + yt*V0;
//   std::cout << "global pos" << std::endl;
//   std::cout << tsos.globalPosition() << std::endl;
//   std::cout << "x0alt" << std::endl;
//   std::cout << x0alt << std::endl;
//   std::cout << "xt" << std::endl;
//   std::cout << xt << std::endl;
//   std::cout << "yt" << std::endl;
//   std::cout << yt << std::endl;
  
  const LocalVector lx(1.,0.,0.);
  const LocalVector ly(0.,1.,0.);
  const LocalVector lz(0.,0.,1.);
  const GlobalVector I = tsos.surface().toGlobal(lz);
  const GlobalVector J = tsos.surface().toGlobal(lx);
  const GlobalVector K = tsos.surface().toGlobal(ly);
  
  const LocalPoint l0(0., 0.);
  const GlobalPoint r = tsos.surface().toGlobal(l0);
  
  const double Ux = U0[0];
  const double Uy = U0[1];
//   const double Uz = U0[2];
  
  const double Vx = V0[0];
  const double Vy = V0[1];
  const double Vz = V0[2];
  
  const double Ix = I.x();
  const double Iy = I.y();
  const double Iz = I.z();
  
  const double Jx = J.x();
  const double Jy = J.y();
  const double Jz = J.z();
  
  const double Kx = K.x();
  const double Ky = K.y();
  const double Kz = K.z();
  
  const double rx = r.x();
  const double ry = r.y();
  const double rz = r.z();
  
  const double pos0x = tsos.globalPosition().x();
  const double pos0y = tsos.globalPosition().y();
  const double pos0z = tsos.globalPosition().z();
  
  //sympy stuff goes here
  const double x0 = std::sin(lam);
  const double x1 = Iz*x0;
  const double x2 = std::cos(lam);
  const double x3 = std::cos(phi);
  const double x4 = Ix*x3;
  const double x5 = x2*x4;
  const double x6 = std::sin(phi);
  const double x7 = Iy*x6;
  const double x8 = x2*x7;
  const double x9 = x5 + x8;
  const double x10 = x1 + x9;
  const double x11 = 1.0/x10;
  const double x12 = Ix*rx;
  const double x13 = Iy*ry;
  const double x14 = Iz*rz;
  const double x15 = Ix*pos0x;
  const double x16 = Iy*pos0y;
  const double x17 = Iz*pos0z;
  const double x18 = Ux*xt;
  const double x19 = Ix*x18;
  const double x20 = Vx*yt;
  const double x21 = Ix*x20;
  const double x22 = Uy*xt;
  const double x23 = Iy*x22;
  const double x24 = Vy*yt;
  const double x25 = Iy*x24;
  const double x26 = Vz*yt;
  const double x27 = Iz*x26;
  const double x28 = x12 + x13 + x14 - x15 - x16 - x17 - x19 - x21 - x23 - x25 - x27;
  const double x29 = pos0z + x26;
  const double x30 = Iz*x2;
  const double x31 = x0*x4;
  const double x32 = x0*x7;
  const double x33 = x30 - x31 - x32;
  const double x34 = x2*x28;
  const double x35 = x29*x33 + x34;
  const double x36 = Jz*x11;
  const double x37 = pos0x + x18 + x20;
  const double x38 = x0*x28;
  const double x39 = -x3*x38 + x33*x37;
  const double x40 = Jx*x11;
  const double x41 = pos0y + x22 + x24;
  const double x42 = x33*x41 - x38*x6;
  const double x43 = Jy*x11;
  const double x44 = std::pow(x10, -2);
  const double x45 = -x30 + x31 + x32;
  const double x46 = x44*x45;
  const double x47 = x10*x29 + x38;
  const double x48 = Jz*x47;
  const double x49 = x3*x34;
  const double x50 = x10*x37 + x49;
  const double x51 = Jx*x50;
  const double x52 = x34*x6;
  const double x53 = x10*x41 + x52;
  const double x54 = Jy*x46;
  const double x55 = -x5 - x8;
  const double x56 = -x1 + x55;
  const double x57 = -x12 - x13 - x14 + x15 + x16 + x17 + x19 + x21 + x23 + x25 + x27;
  const double x58 = x2*x57;
  const double x59 = x3*x58;
  const double x60 = x37*x56 + x59;
  const double x61 = x41*x56 + x58*x6;
  const double x62 = x29*x56 - x38;
  const double x63 = Jz*x46;
  const double x64 = 2*x35;
  const double x65 = Jx*x46;
  const double x66 = 2*x39;
  const double x67 = 2*x42;
  const double x68 = std::pow(x10, -3);
  const double x69 = x45*x68;
  const double x70 = x69*(-2*x30 + 2*x31 + 2*x32);
  const double x71 = Jy*x53;
  const double x72 = x0*x6;
  const double x73 = Ix*x72;
  const double x74 = x0*x3;
  const double x75 = Iy*x74;
  const double x76 = x73 - x75;
  const double x77 = x29*x36;
  const double x78 = x2*x3;
  const double x79 = Iy*x78;
  const double x80 = x2*x6;
  const double x81 = Ix*x80;
  const double x82 = x79 - x81;
  const double x83 = x29*x82;
  const double x84 = x41*x76 + x57*x74;
  const double x85 = x37*x76 - x57*x72;
  const double x86 = x44*(-x73 + x75);
  const double x87 = -x79 + x81;
  const double x88 = x44*x87;
  const double x89 = Jz*x88;
  const double x90 = x41*x82 + x49;
  const double x91 = -x52;
  const double x92 = x37*x82 + x91;
  const double x93 = Jx*x88;
  const double x94 = Jy*x88;
  const double x95 = -2*x79 + 2*x81;
  const double x96 = x69*x95;
  const double x97 = Ix*Ux;
  const double x98 = Iy*Uy;
  const double x99 = -x97 - x98;
  const double x100 = x36*x99;
  const double x101 = x0*x99;
  const double x102 = x97 + x98;
  const double x103 = Ux*x33 + x102*x74;
  const double x104 = Uy*x33 + x102*x72;
  const double x105 = x78*x99;
  const double x106 = Ux*x10 + x105;
  const double x107 = Uy*x10 + x80*x99;
  const double x108 = Ix*Vx;
  const double x109 = Iy*Vy;
  const double x110 = Iz*Vz;
  const double x111 = x108 + x109 + x110;
  const double x112 = Vx*x33 + x111*x74;
  const double x113 = Vy*x33 + x111*x72;
  const double x114 = -x108 - x109 - x110;
  const double x115 = x114*x2;
  const double x116 = Vz*x33 + x115;
  const double x117 = Vz*x10 + x0*x114;
  const double x118 = x117*x46;
  const double x119 = x115*x3;
  const double x120 = Vx*x10 + x119;
  const double x121 = Vy*x10 + x115*x6;
  const double x122 = 2*x83;
  const double x123 = x37*x55 + x59;
  const double x124 = x41*x55 + x91;
  const double x125 = x44*x9;
  const double x126 = 2*x90;
  const double x127 = 2*x92;
  const double x128 = x68*x87*x95;
  const double x129 = Ux*x82 + x102*x80;
  const double x130 = Uy*x82 + x105;
  const double x131 = Vz*x82;
  const double x132 = Vx*x82 + x111*x80;
  const double x133 = Vy*x82 + x119;
  const double x134 = Kz*x11;
  const double x135 = Kx*x11;
  const double x136 = Ky*x11;
  const double x137 = Kz*x47;
  const double x138 = Kx*x46;
  const double x139 = Ky*x46;
  const double x140 = Kz*x46;
  const double x141 = Kx*x50;
  const double x142 = Ky*x53;
  const double x143 = x134*x29;
  const double x144 = Kz*x88;
  const double x145 = Kx*x88;
  const double x146 = Ky*x88;
  const double x147 = x134*x99;
  const double shat = x11*x28;
  const double dvdqop = 0;
  const double d2vdqopdqop = 0;
  const double d2vdqopdlam = 0;
  const double d2vdqopdphi = 0;
  const double d2vdqopdxt = 0;
  const double d2vdqopdyt = 0;
  const double dvdlam = x35*x36 + x39*x40 + x42*x43 + x46*x48 + x46*x51 + x53*x54;
  const double d2vdlamdlam = x36*x47 + x36*x62 + x40*x50 + x40*x60 + x43*x53 + x43*x61 + x48*x70 + x51*x70 + x54*x67 + x63*x64 + x65*x66 + x70*x71;
  const double d2vdlamdphi = x35*x89 + x39*x93 + x40*x85 + x42*x94 + x43*x84 + x48*x86 + x48*x96 + x51*x86 + x51*x96 + x54*x90 + x63*x83 + x65*x92 + x71*x86 + x71*x96 + x76*x77;
  const double d2vdlamdxt = x100*x2 + x101*x63 + x103*x40 + x104*x43 + x106*x65 + x107*x54;
  const double d2vdlamdyt = Jz*x118 + x112*x40 + x113*x43 + x116*x36 + x120*x65 + x121*x54;
  const double dvdphi = x40*x92 + x43*x90 + x48*x88 + x51*x88 + x53*x94 + x77*x82;
  const double d2vdphidphi = x122*x89 + x123*x40 + x124*x43 + x125*x48 + x125*x51 + x125*x71 + x126*x94 + x127*x93 + x128*x48 + x128*x51 + x128*x71 + x55*x77;
  const double d2vdphidxt = x101*x89 + x106*x93 + x107*x94 + x129*x40 + x130*x43;
  const double d2vdphidyt = x117*x89 + x120*x93 + x121*x94 + x131*x36 + x132*x40 + x133*x43;
  const double dvdxt = x0*x100 + x106*x40 + x107*x43;
  const double d2vdxtdxt = 0;
  const double d2vdxtdyt = 0;
  const double dvdyt = x117*x36 + x120*x40 + x121*x43;
  const double d2vdytdyt = 0;
  const double dwdqop = 0;
  const double d2wdqopdqop = 0;
  const double d2wdqopdlam = 0;
  const double d2wdqopdphi = 0;
  const double d2wdqopdxt = 0;
  const double d2wdqopdyt = 0;
  const double dwdlam = x134*x35 + x135*x39 + x136*x42 + x137*x46 + x138*x50 + x139*x53;
  const double d2wdlamdlam = x134*x47 + x134*x62 + x135*x50 + x135*x60 + x136*x53 + x136*x61 + x137*x70 + x138*x66 + x139*x67 + x140*x64 + x141*x70 + x142*x70;
  const double d2wdlamdphi = x135*x85 + x136*x84 + x137*x86 + x137*x96 + x138*x92 + x139*x90 + x140*x83 + x141*x86 + x141*x96 + x142*x86 + x142*x96 + x143*x76 + x144*x35 + x145*x39 + x146*x42;
  const double d2wdlamdxt = x101*x140 + x103*x135 + x104*x136 + x106*x138 + x107*x139 + x147*x2;
  const double d2wdlamdyt = Kz*x118 + x112*x135 + x113*x136 + x116*x134 + x120*x138 + x121*x139;
  const double dwdphi = x135*x92 + x136*x90 + x137*x88 + x143*x82 + x145*x50 + x146*x53;
  const double d2wdphidphi = x122*x144 + x123*x135 + x124*x136 + x125*x137 + x125*x141 + x125*x142 + x126*x146 + x127*x145 + x128*x137 + x128*x141 + x128*x142 + x143*x55;
  const double d2wdphidxt = x101*x144 + x106*x145 + x107*x146 + x129*x135 + x130*x136;
  const double d2wdphidyt = x117*x144 + x120*x145 + x121*x146 + x131*x134 + x132*x135 + x133*x136;
  const double dwdxt = x0*x147 + x106*x135 + x107*x136;
  const double d2wdxtdxt = 0;
  const double d2wdxtdyt = 0;
  const double dwdyt = x117*x134 + x120*x135 + x121*x136;
  const double d2wdytdyt = 0;
  Matrix<double, 5, 5> d2vdx2;
  d2vdx2(0, 0) = d2vdqopdqop;
  d2vdx2(0, 1) = d2vdqopdlam;
  d2vdx2(0, 2) = d2vdqopdphi;
  d2vdx2(0, 3) = d2vdqopdxt;
  d2vdx2(0, 4) = d2vdqopdyt;
  d2vdx2(1, 0) = d2vdqopdlam;
  d2vdx2(1, 1) = d2vdlamdlam;
  d2vdx2(1, 2) = d2vdlamdphi;
  d2vdx2(1, 3) = d2vdlamdxt;
  d2vdx2(1, 4) = d2vdlamdyt;
  d2vdx2(2, 0) = d2vdqopdphi;
  d2vdx2(2, 1) = d2vdlamdphi;
  d2vdx2(2, 2) = d2vdphidphi;
  d2vdx2(2, 3) = d2vdphidxt;
  d2vdx2(2, 4) = d2vdphidyt;
  d2vdx2(3, 0) = d2vdqopdxt;
  d2vdx2(3, 1) = d2vdlamdxt;
  d2vdx2(3, 2) = d2vdphidxt;
  d2vdx2(3, 3) = d2vdxtdxt;
  d2vdx2(3, 4) = d2vdxtdyt;
  d2vdx2(4, 0) = d2vdqopdyt;
  d2vdx2(4, 1) = d2vdlamdyt;
  d2vdx2(4, 2) = d2vdphidyt;
  d2vdx2(4, 3) = d2vdxtdyt;
  d2vdx2(4, 4) = d2vdytdyt;
  Matrix<double, 5, 5> d2wdx2;
  d2wdx2(0, 0) = d2wdqopdqop;
  d2wdx2(0, 1) = d2wdqopdlam;
  d2wdx2(0, 2) = d2wdqopdphi;
  d2wdx2(0, 3) = d2wdqopdxt;
  d2wdx2(0, 4) = d2wdqopdyt;
  d2wdx2(1, 0) = d2wdqopdlam;
  d2wdx2(1, 1) = d2wdlamdlam;
  d2wdx2(1, 2) = d2wdlamdphi;
  d2wdx2(1, 3) = d2wdlamdxt;
  d2wdx2(1, 4) = d2wdlamdyt;
  d2wdx2(2, 0) = d2wdqopdphi;
  d2wdx2(2, 1) = d2wdlamdphi;
  d2wdx2(2, 2) = d2wdphidphi;
  d2wdx2(2, 3) = d2wdphidxt;
  d2wdx2(2, 4) = d2wdphidyt;
  d2wdx2(3, 0) = d2wdqopdxt;
  d2wdx2(3, 1) = d2wdlamdxt;
  d2wdx2(3, 2) = d2wdphidxt;
  d2wdx2(3, 3) = d2wdxtdxt;
  d2wdx2(3, 4) = d2wdxtdyt;
  d2wdx2(4, 0) = d2wdqopdyt;
  d2wdx2(4, 1) = d2wdlamdyt;
  d2wdx2(4, 2) = d2wdphidyt;
  d2wdx2(4, 3) = d2wdxtdyt;
  d2wdx2(4, 4) = d2wdytdyt;
  Matrix<double, 5, 1> dvdx;
  dvdx[0] = dvdqop;
  dvdx[1] = dvdlam;
  dvdx[2] = dvdphi;
  dvdx[3] = dvdxt;
  dvdx[4] = dvdyt;
  Matrix<double, 5, 1> dwdx;
  dwdx[0] = dwdqop;
  dwdx[1] = dwdlam;
  dwdx[2] = dwdphi;
  dwdx[3] = dwdxt;
  dwdx[4] = dwdyt;



//   std::cout << "dvdx" << std::endl;
//   std::cout << dvdx << std::endl;
//   
//   std::cout << "dwdx" << std::endl;
//   std::cout << dwdx << std::endl;
//   
//   std::cout << "d2vdx2" << std::endl;
//   std::cout << d2vdx2 << std::endl;
// 
//   std::cout << "d2wdx2" << std::endl;
//   std::cout << d2wdx2 << std::endl;
  
//   std::cout << "shat" << std::endl;
//   std::cout << shat << std::endl;
  
  // covariance matrix in curvilinear parameters
//   const AlgebraicMatrix55 curvcovsmat = tsos.curvilinearError().matrix();
  
  // map to eigen data structure
//   const Map<const Matrix<double, 5, 5, RowMajor>> curvcov(curvcovsmat.Array());
  
//   std::cout << "curvcov" << std::endl;
//   std::cout << curvcov << std::endl;
  
  // compute eigendecomposition
  SelfAdjointEigenSolver<Matrix<double, 5, 5>> es;
  es.compute(curvcov);
  
  // cov = VDV^(-1)
//   auto const& sqrtD = es.eigenvalues().cwiseSqrt();
  auto const& D = es.eigenvalues();
  auto const& V = es.eigenvectors();
  
  // compute second order correction to local positions
//   Matrix<double, 2, 1> res;
//   res[0] = 0.5*sqrtD.transpose()*V.transpose()*d2vdx2*V*sqrtD;
//   res[1] = 0.5*sqrtD.transpose()*V.transpose()*d2wdx2*V*sqrtD;
  
  Matrix<double, 2, 1> res = Matrix<double, 2, 1>::Zero();
  for (unsigned int i=0; i<5; ++i) {
    res[0] += 0.5*D[i]*V.col(i).transpose()*d2vdx2*V.col(i);
    res[1] += 0.5*D[i]*V.col(i).transpose()*d2wdx2*V.col(i);
  }
  
//   res[0] = d2vdx2*curvc
  
  return res;
  
}


Matrix<double, 2, 1> ResidualGlobalCorrectionMakerBase::localPositionConvolutionD(const Matrix<double, 7, 1>& state, const Matrix<double, 5, 5> &curvcov, const GloballyPositioned<double> &surface) const {

  const double q = state[6];

  const double qop = q/state.segment<3>(3).norm();
  const double lam = std::atan(state[5]/std::sqrt(state[3]*state[3] + state[4]*state[4]));
  const double phi = std::atan2(state[4], state[3]);
  const double xt = 0.;
  const double yt = 0.;

  // curvilinear parameters
//   const CurvilinearTrajectoryParameters curv(tsos.globalPosition(), tsos.globalMomentum(), tsos.charge());
//   const double qop = curv.Qbp();
//   const double lam = curv.lambda();
//   const double phi = curv.phi();
//   const double xt = curv.xT();
//   const double yt = curv.yT();
//   const double xt = 0.;
//   const double yt = 0.;

  const Matrix<double, 3, 1> p0(state[3], state[4], state[5]);
  const Matrix<double, 3, 1> W0 = p0.normalized();
  const Matrix<double, 3, 1> zhat(0., 0., 1.);

  const Matrix<double, 3, 1> U0 = zhat.cross(W0).normalized();
  const Matrix<double, 3, 1> V0 = W0.cross(U0);

//   std::cout << "U0" << std::endl;
//   std::cout << U0 << std::endl;
//   std::cout << "V0" << std::endl;
//   std::cout << V0 << std::endl;

//   const Matrix<double, 3, 1> x0alt = xt*U0 + yt*V0;
//   std::cout << "global pos" << std::endl;
//   std::cout << tsos.globalPosition() << std::endl;
//   std::cout << "x0alt" << std::endl;
//   std::cout << x0alt << std::endl;
//   std::cout << "xt" << std::endl;
//   std::cout << xt << std::endl;
//   std::cout << "yt" << std::endl;
//   std::cout << yt << std::endl;

  const Vector3DBase<double, LocalTag> lx(1.,0.,0.);
  const Vector3DBase<double, LocalTag> ly(0.,1.,0.);
  const Vector3DBase<double, LocalTag> lz(0.,0.,1.);
  const Vector3DBase<double, GlobalTag> I = surface.toGlobal(lz);
  const Vector3DBase<double, GlobalTag> J = surface.toGlobal(lx);
  const Vector3DBase<double, GlobalTag> K = surface.toGlobal(ly);

  const Point3DBase<double, LocalTag> l0(0., 0.);
  const Point3DBase<double, GlobalTag> r = surface.toGlobal(l0);

  const double Ux = U0[0];
  const double Uy = U0[1];
//   const double Uz = U0[2];

  const double Vx = V0[0];
  const double Vy = V0[1];
  const double Vz = V0[2];

  const double Ix = I.x();
  const double Iy = I.y();
  const double Iz = I.z();

  const double Jx = J.x();
  const double Jy = J.y();
  const double Jz = J.z();

  const double Kx = K.x();
  const double Ky = K.y();
  const double Kz = K.z();

  const double rx = r.x();
  const double ry = r.y();
  const double rz = r.z();

  const double pos0x = state[0];
  const double pos0y = state[1];
  const double pos0z = state[2];

  //sympy stuff goes here
  const double x0 = std::sin(lam);
  const double x1 = Iz*x0;
  const double x2 = std::cos(lam);
  const double x3 = std::cos(phi);
  const double x4 = Ix*x3;
  const double x5 = x2*x4;
  const double x6 = std::sin(phi);
  const double x7 = Iy*x6;
  const double x8 = x2*x7;
  const double x9 = x5 + x8;
  const double x10 = x1 + x9;
  const double x11 = 1.0/x10;
  const double x12 = Ix*rx;
  const double x13 = Iy*ry;
  const double x14 = Iz*rz;
  const double x15 = Ix*pos0x;
  const double x16 = Iy*pos0y;
  const double x17 = Iz*pos0z;
  const double x18 = Ux*xt;
  const double x19 = Ix*x18;
  const double x20 = Vx*yt;
  const double x21 = Ix*x20;
  const double x22 = Uy*xt;
  const double x23 = Iy*x22;
  const double x24 = Vy*yt;
  const double x25 = Iy*x24;
  const double x26 = Vz*yt;
  const double x27 = Iz*x26;
  const double x28 = x12 + x13 + x14 - x15 - x16 - x17 - x19 - x21 - x23 - x25 - x27;
  const double x29 = pos0z + x26;
  const double x30 = Iz*x2;
  const double x31 = x0*x4;
  const double x32 = x0*x7;
  const double x33 = x30 - x31 - x32;
  const double x34 = x2*x28;
  const double x35 = x29*x33 + x34;
  const double x36 = Jz*x11;
  const double x37 = pos0x + x18 + x20;
  const double x38 = x0*x28;
  const double x39 = -x3*x38 + x33*x37;
  const double x40 = Jx*x11;
  const double x41 = pos0y + x22 + x24;
  const double x42 = x33*x41 - x38*x6;
  const double x43 = Jy*x11;
  const double x44 = std::pow(x10, -2);
  const double x45 = -x30 + x31 + x32;
  const double x46 = x44*x45;
  const double x47 = x10*x29 + x38;
  const double x48 = Jz*x47;
  const double x49 = x3*x34;
  const double x50 = x10*x37 + x49;
  const double x51 = Jx*x50;
  const double x52 = x34*x6;
  const double x53 = x10*x41 + x52;
  const double x54 = Jy*x46;
  const double x55 = -x5 - x8;
  const double x56 = -x1 + x55;
  const double x57 = -x12 - x13 - x14 + x15 + x16 + x17 + x19 + x21 + x23 + x25 + x27;
  const double x58 = x2*x57;
  const double x59 = x3*x58;
  const double x60 = x37*x56 + x59;
  const double x61 = x41*x56 + x58*x6;
  const double x62 = x29*x56 - x38;
  const double x63 = Jz*x46;
  const double x64 = 2*x35;
  const double x65 = Jx*x46;
  const double x66 = 2*x39;
  const double x67 = 2*x42;
  const double x68 = std::pow(x10, -3);
  const double x69 = x45*x68;
  const double x70 = x69*(-2*x30 + 2*x31 + 2*x32);
  const double x71 = Jy*x53;
  const double x72 = x0*x6;
  const double x73 = Ix*x72;
  const double x74 = x0*x3;
  const double x75 = Iy*x74;
  const double x76 = x73 - x75;
  const double x77 = x29*x36;
  const double x78 = x2*x3;
  const double x79 = Iy*x78;
  const double x80 = x2*x6;
  const double x81 = Ix*x80;
  const double x82 = x79 - x81;
  const double x83 = x29*x82;
  const double x84 = x41*x76 + x57*x74;
  const double x85 = x37*x76 - x57*x72;
  const double x86 = x44*(-x73 + x75);
  const double x87 = -x79 + x81;
  const double x88 = x44*x87;
  const double x89 = Jz*x88;
  const double x90 = x41*x82 + x49;
  const double x91 = -x52;
  const double x92 = x37*x82 + x91;
  const double x93 = Jx*x88;
  const double x94 = Jy*x88;
  const double x95 = -2*x79 + 2*x81;
  const double x96 = x69*x95;
  const double x97 = Ix*Ux;
  const double x98 = Iy*Uy;
  const double x99 = -x97 - x98;
  const double x100 = x36*x99;
  const double x101 = x0*x99;
  const double x102 = x97 + x98;
  const double x103 = Ux*x33 + x102*x74;
  const double x104 = Uy*x33 + x102*x72;
  const double x105 = x78*x99;
  const double x106 = Ux*x10 + x105;
  const double x107 = Uy*x10 + x80*x99;
  const double x108 = Ix*Vx;
  const double x109 = Iy*Vy;
  const double x110 = Iz*Vz;
  const double x111 = x108 + x109 + x110;
  const double x112 = Vx*x33 + x111*x74;
  const double x113 = Vy*x33 + x111*x72;
  const double x114 = -x108 - x109 - x110;
  const double x115 = x114*x2;
  const double x116 = Vz*x33 + x115;
  const double x117 = Vz*x10 + x0*x114;
  const double x118 = x117*x46;
  const double x119 = x115*x3;
  const double x120 = Vx*x10 + x119;
  const double x121 = Vy*x10 + x115*x6;
  const double x122 = 2*x83;
  const double x123 = x37*x55 + x59;
  const double x124 = x41*x55 + x91;
  const double x125 = x44*x9;
  const double x126 = 2*x90;
  const double x127 = 2*x92;
  const double x128 = x68*x87*x95;
  const double x129 = Ux*x82 + x102*x80;
  const double x130 = Uy*x82 + x105;
  const double x131 = Vz*x82;
  const double x132 = Vx*x82 + x111*x80;
  const double x133 = Vy*x82 + x119;
  const double x134 = Kz*x11;
  const double x135 = Kx*x11;
  const double x136 = Ky*x11;
  const double x137 = Kz*x47;
  const double x138 = Kx*x46;
  const double x139 = Ky*x46;
  const double x140 = Kz*x46;
  const double x141 = Kx*x50;
  const double x142 = Ky*x53;
  const double x143 = x134*x29;
  const double x144 = Kz*x88;
  const double x145 = Kx*x88;
  const double x146 = Ky*x88;
  const double x147 = x134*x99;
  const double shat = x11*x28;
  const double dvdqop = 0;
  const double d2vdqopdqop = 0;
  const double d2vdqopdlam = 0;
  const double d2vdqopdphi = 0;
  const double d2vdqopdxt = 0;
  const double d2vdqopdyt = 0;
  const double dvdlam = x35*x36 + x39*x40 + x42*x43 + x46*x48 + x46*x51 + x53*x54;
  const double d2vdlamdlam = x36*x47 + x36*x62 + x40*x50 + x40*x60 + x43*x53 + x43*x61 + x48*x70 + x51*x70 + x54*x67 + x63*x64 + x65*x66 + x70*x71;
  const double d2vdlamdphi = x35*x89 + x39*x93 + x40*x85 + x42*x94 + x43*x84 + x48*x86 + x48*x96 + x51*x86 + x51*x96 + x54*x90 + x63*x83 + x65*x92 + x71*x86 + x71*x96 + x76*x77;
  const double d2vdlamdxt = x100*x2 + x101*x63 + x103*x40 + x104*x43 + x106*x65 + x107*x54;
  const double d2vdlamdyt = Jz*x118 + x112*x40 + x113*x43 + x116*x36 + x120*x65 + x121*x54;
  const double dvdphi = x40*x92 + x43*x90 + x48*x88 + x51*x88 + x53*x94 + x77*x82;
  const double d2vdphidphi = x122*x89 + x123*x40 + x124*x43 + x125*x48 + x125*x51 + x125*x71 + x126*x94 + x127*x93 + x128*x48 + x128*x51 + x128*x71 + x55*x77;
  const double d2vdphidxt = x101*x89 + x106*x93 + x107*x94 + x129*x40 + x130*x43;
  const double d2vdphidyt = x117*x89 + x120*x93 + x121*x94 + x131*x36 + x132*x40 + x133*x43;
  const double dvdxt = x0*x100 + x106*x40 + x107*x43;
  const double d2vdxtdxt = 0;
  const double d2vdxtdyt = 0;
  const double dvdyt = x117*x36 + x120*x40 + x121*x43;
  const double d2vdytdyt = 0;
  const double dwdqop = 0;
  const double d2wdqopdqop = 0;
  const double d2wdqopdlam = 0;
  const double d2wdqopdphi = 0;
  const double d2wdqopdxt = 0;
  const double d2wdqopdyt = 0;
  const double dwdlam = x134*x35 + x135*x39 + x136*x42 + x137*x46 + x138*x50 + x139*x53;
  const double d2wdlamdlam = x134*x47 + x134*x62 + x135*x50 + x135*x60 + x136*x53 + x136*x61 + x137*x70 + x138*x66 + x139*x67 + x140*x64 + x141*x70 + x142*x70;
  const double d2wdlamdphi = x135*x85 + x136*x84 + x137*x86 + x137*x96 + x138*x92 + x139*x90 + x140*x83 + x141*x86 + x141*x96 + x142*x86 + x142*x96 + x143*x76 + x144*x35 + x145*x39 + x146*x42;
  const double d2wdlamdxt = x101*x140 + x103*x135 + x104*x136 + x106*x138 + x107*x139 + x147*x2;
  const double d2wdlamdyt = Kz*x118 + x112*x135 + x113*x136 + x116*x134 + x120*x138 + x121*x139;
  const double dwdphi = x135*x92 + x136*x90 + x137*x88 + x143*x82 + x145*x50 + x146*x53;
  const double d2wdphidphi = x122*x144 + x123*x135 + x124*x136 + x125*x137 + x125*x141 + x125*x142 + x126*x146 + x127*x145 + x128*x137 + x128*x141 + x128*x142 + x143*x55;
  const double d2wdphidxt = x101*x144 + x106*x145 + x107*x146 + x129*x135 + x130*x136;
  const double d2wdphidyt = x117*x144 + x120*x145 + x121*x146 + x131*x134 + x132*x135 + x133*x136;
  const double dwdxt = x0*x147 + x106*x135 + x107*x136;
  const double d2wdxtdxt = 0;
  const double d2wdxtdyt = 0;
  const double dwdyt = x117*x134 + x120*x135 + x121*x136;
  const double d2wdytdyt = 0;
  Matrix<double, 5, 5> d2vdx2;
  d2vdx2(0, 0) = d2vdqopdqop;
  d2vdx2(0, 1) = d2vdqopdlam;
  d2vdx2(0, 2) = d2vdqopdphi;
  d2vdx2(0, 3) = d2vdqopdxt;
  d2vdx2(0, 4) = d2vdqopdyt;
  d2vdx2(1, 0) = d2vdqopdlam;
  d2vdx2(1, 1) = d2vdlamdlam;
  d2vdx2(1, 2) = d2vdlamdphi;
  d2vdx2(1, 3) = d2vdlamdxt;
  d2vdx2(1, 4) = d2vdlamdyt;
  d2vdx2(2, 0) = d2vdqopdphi;
  d2vdx2(2, 1) = d2vdlamdphi;
  d2vdx2(2, 2) = d2vdphidphi;
  d2vdx2(2, 3) = d2vdphidxt;
  d2vdx2(2, 4) = d2vdphidyt;
  d2vdx2(3, 0) = d2vdqopdxt;
  d2vdx2(3, 1) = d2vdlamdxt;
  d2vdx2(3, 2) = d2vdphidxt;
  d2vdx2(3, 3) = d2vdxtdxt;
  d2vdx2(3, 4) = d2vdxtdyt;
  d2vdx2(4, 0) = d2vdqopdyt;
  d2vdx2(4, 1) = d2vdlamdyt;
  d2vdx2(4, 2) = d2vdphidyt;
  d2vdx2(4, 3) = d2vdxtdyt;
  d2vdx2(4, 4) = d2vdytdyt;
  Matrix<double, 5, 5> d2wdx2;
  d2wdx2(0, 0) = d2wdqopdqop;
  d2wdx2(0, 1) = d2wdqopdlam;
  d2wdx2(0, 2) = d2wdqopdphi;
  d2wdx2(0, 3) = d2wdqopdxt;
  d2wdx2(0, 4) = d2wdqopdyt;
  d2wdx2(1, 0) = d2wdqopdlam;
  d2wdx2(1, 1) = d2wdlamdlam;
  d2wdx2(1, 2) = d2wdlamdphi;
  d2wdx2(1, 3) = d2wdlamdxt;
  d2wdx2(1, 4) = d2wdlamdyt;
  d2wdx2(2, 0) = d2wdqopdphi;
  d2wdx2(2, 1) = d2wdlamdphi;
  d2wdx2(2, 2) = d2wdphidphi;
  d2wdx2(2, 3) = d2wdphidxt;
  d2wdx2(2, 4) = d2wdphidyt;
  d2wdx2(3, 0) = d2wdqopdxt;
  d2wdx2(3, 1) = d2wdlamdxt;
  d2wdx2(3, 2) = d2wdphidxt;
  d2wdx2(3, 3) = d2wdxtdxt;
  d2wdx2(3, 4) = d2wdxtdyt;
  d2wdx2(4, 0) = d2wdqopdyt;
  d2wdx2(4, 1) = d2wdlamdyt;
  d2wdx2(4, 2) = d2wdphidyt;
  d2wdx2(4, 3) = d2wdxtdyt;
  d2wdx2(4, 4) = d2wdytdyt;
  Matrix<double, 5, 1> dvdx;
  dvdx[0] = dvdqop;
  dvdx[1] = dvdlam;
  dvdx[2] = dvdphi;
  dvdx[3] = dvdxt;
  dvdx[4] = dvdyt;
  Matrix<double, 5, 1> dwdx;
  dwdx[0] = dwdqop;
  dwdx[1] = dwdlam;
  dwdx[2] = dwdphi;
  dwdx[3] = dwdxt;
  dwdx[4] = dwdyt;



//   std::cout << "dvdx" << std::endl;
//   std::cout << dvdx << std::endl;
//
//   std::cout << "dwdx" << std::endl;
//   std::cout << dwdx << std::endl;
//
//   std::cout << "d2vdx2" << std::endl;
//   std::cout << d2vdx2 << std::endl;
//
//   std::cout << "d2wdx2" << std::endl;
//   std::cout << d2wdx2 << std::endl;

//   std::cout << "shat" << std::endl;
//   std::cout << shat << std::endl;

  // covariance matrix in curvilinear parameters
//   const AlgebraicMatrix55 curvcovsmat = tsos.curvilinearError().matrix();

  // map to eigen data structure
//   const Map<const Matrix<double, 5, 5, RowMajor>> curvcov(curvcovsmat.Array());

//   std::cout << "curvcov" << std::endl;
//   std::cout << curvcov << std::endl;

  // compute eigendecomposition
  SelfAdjointEigenSolver<Matrix<double, 5, 5>> es;
  es.compute(curvcov);

  // cov = VDV^(-1)
//   auto const& sqrtD = es.eigenvalues().cwiseSqrt();
  auto const& D = es.eigenvalues();
  auto const& V = es.eigenvectors();

  // compute second order correction to local positions
//   Matrix<double, 2, 1> res;
//   res[0] = 0.5*sqrtD.transpose()*V.transpose()*d2vdx2*V*sqrtD;
//   res[1] = 0.5*sqrtD.transpose()*V.transpose()*d2wdx2*V*sqrtD;

  Matrix<double, 2, 1> res = Matrix<double, 2, 1>::Zero();
  for (unsigned int i=0; i<5; ++i) {
    res[0] += 0.5*D[i]*V.col(i).transpose()*d2vdx2*V.col(i);
    res[1] += 0.5*D[i]*V.col(i).transpose()*d2wdx2*V.col(i);
  }

//   res[0] = d2vdx2*curvc

  return res;

}


AlgebraicVector5 ResidualGlobalCorrectionMakerBase::update(const TrajectoryStateOnSurface& tsos, const TrackingRecHit& aRecHit) {
  switch (aRecHit.dimension()) {
    case 1:
      return lupdate<1>(tsos, aRecHit);
    case 2:
      return lupdate<2>(tsos, aRecHit);
    case 3:
      return lupdate<3>(tsos, aRecHit);
    case 4:
      return lupdate<4>(tsos, aRecHit);
    case 5:
      return lupdate<5>(tsos, aRecHit);
  }
  return AlgebraicVector5();
}

#if 0
Matrix<double, 5, 3> ResidualGlobalCorrectionMakerBase::vertexToCurvilinearJacobian(const FreeTrajectoryState &state) const {
  GlobalVector xt = state.momentum();
  GlobalVector yt(-xt.y(), xt.x(), 0.);
  GlobalVector zt = xt.cross(yt);
  const GlobalVector& pvec = state.momentum();
  double pt = pvec.perp(), p = pvec.mag();
  double px = pvec.x(), py = pvec.y(), pz = pvec.z();
  double pt2 = pt * pt, p2 = p * p, p3 = p * p * p;

  xt = xt.unit();
  if (fabs(pt) > 0) {
    yt = yt.unit();
    zt = zt.unit();
  }

  Matrix<double, 3, 3> R;
  R(0, 0) = xt.x();
  R(0, 1) = xt.y();
  R(0, 2) = xt.z();
  R(1, 0) = yt.x();
  R(1, 1) = yt.y();
  R(1, 2) = yt.z();
  R(2, 0) = zt.x();
  R(2, 1) = zt.y();
  R(2, 2) = zt.z();
  
  const Matrix<double, 1, 3> dsdvtx = R.row(0);
  
  const MagneticField &bfield = state.parameters().magneticField();

  const Vector3d b(bfield.x(),
                      bfield.y(),
                      bfield.z());
  const double magb = b.norm();
  const Vector3d h = b.normalized();

  const Vector3d p0(state.momentum().x(),
                      state.momentum().y(),
                      state.momentum().z());
  const Vector3d M0(state.position().x(),
                    state.position().y(),
                    state.position().z());
  const Vector3d T0 = p0.normalized();
  const double p = p0.norm();
  const double q = state.charge();
  const double qop = q/p;

  const Vector3d N0alpha = h.cross(T0);
  const double alpha = N0alpha.norm();
  const Vector3d N0 = N0alpha.normalized();
  const double gamma0 = h.transpose()*T0;
  const Vector3d Z(0.,0.,1.);
  const Vector3d U = Z.cross(T).normalized();
  const Vector3d V = T.cross(U);
  
  
  
  Matrix<double, 5, 3> res = Matrix<double, 5, 3>::Zero();
  
  // d(dxt, dyt) / dvtx
  res.bottomRows<2>() = R.bottomRows<2>();
  
}
#endif

Matrix<double, 6, 6> ResidualGlobalCorrectionMakerBase::cartesianToCartesianJacobian(const FreeTrajectoryState &state) const {
  const GlobalVector &bfield = state.parameters().magneticFieldInInverseGeV();

  const Vector3d b(bfield.x(),
                      bfield.y(),
                      bfield.z());
  const double magb = b.norm();
  const Vector3d h = b.normalized();

  const Vector3d p0(state.momentum().x(),
                      state.momentum().y(),
                      state.momentum().z());
//   const Vector3d M0(state.position().x(),
//                     state.position().y(),
//                     state.position().z());
  const Vector3d T0 = p0.normalized();
  const double p = p0.norm();
  const double q = state.charge();
  const double qop = q/p;

  const Vector3d N0alpha = h.cross(T0);
  const double alpha = N0alpha.norm();
  const Vector3d N0 = N0alpha.normalized();
//   const double gamma0 = h.transpose()*T0;
//   const Vector3d Z(0.,0.,1.);
//   const Vector3d U = Z.cross(T).normalized();
//   const Vector3d V = T.cross(U);
  
  Matrix<double, 6, 6> res = Matrix<double, 6, 6>::Identity();
  
  //dp/dvtx
  res.bottomLeftCorner<3, 3>() = -alpha*magb*qop*N0*p0.transpose();
  
  res = Matrix<double, 6, 6>::Zero();
  
  res *= 0.;
//   res *= -1.;
  
  return res;
  
  
  
}


Matrix<double, 1, 6> ResidualGlobalCorrectionMakerBase::massJacobianAlt(const FreeTrajectoryState &state0, const FreeTrajectoryState &state1, double dmass) const {
  
  const double mp = dmass;
  
  CurvilinearTrajectoryParameters curvparms0(state0.position(), state0.momentum(), state0.charge());

  const double qop0 = curvparms0.Qbp();
  const double lam0 = curvparms0.lambda();
  const double phi0 = curvparms0.phi();

  CurvilinearTrajectoryParameters curvparms1(state1.position(), state1.momentum(), state1.charge());

  const double qop1 = curvparms1.Qbp();
  const double lam1 = curvparms1.lambda();
  const double phi1 = curvparms1.phi();

  const double xf0 = std::pow(mp, 2);
  const double xf1 = std::sin(lam0);
  const double xf2 = std::sin(lam1);
  const double xf3 = 1.0/std::fabs(qop0);
  const double xf4 = 1.0/std::fabs(qop1);
  const double xf5 = xf3*xf4;
  const double xf6 = xf2*xf5;
  const double xf7 = std::sin(phi0);
  const double xf8 = std::sin(phi1);
  const double xf9 = xf7*xf8;
  const double xf10 = std::cos(lam0);
  const double xf11 = std::cos(lam1);
  const double xf12 = xf11*xf5;
  const double xf13 = xf10*xf12;
  const double xf14 = 2*xf13;
  const double xf15 = std::cos(phi0);
  const double xf16 = std::cos(phi1);
  const double xf17 = xf15*xf16;
  const double xf18 = std::pow(qop0, -2);
  const double xf19 = std::sqrt(xf0 + xf18);
  const double xf20 = std::pow(qop1, -2);
  const double xf21 = std::sqrt(xf0 + xf20);
  const double xf22 = std::sqrt(2*xf0 - 2*xf1*xf6 - xf14*xf17 - xf14*xf9 + 2*xf19*xf21);
  const double xf23 = xf1*xf2;
  const double xf24 = xf18*xf4*(((qop0) > 0) - ((qop0) < 0));
  const double xf25 = xf10*xf11;
  const double xf26 = xf24*xf25;
  const double xf27 = 1.0/xf22;
  const double xf28 = xf10*xf6;
  const double xf29 = xf1*xf12;
  const double xf30 = xf13*xf16*xf7;
  const double xf31 = xf13*xf15*xf8;
  const double xf32 = xf20*xf3*(((qop1) > 0) - ((qop1) < 0));
  const double xf33 = xf25*xf32;
  const double m = xf22;
  const double dmdqop0 = xf27*(xf17*xf26 + xf23*xf24 + xf26*xf9 - xf21/(std::pow(qop0, 3)*xf19));
  const double dmdlam0 = xf27*(xf17*xf29 - xf28 + xf29*xf9);
  const double dmdphi0 = xf27*(xf30 - xf31);
  const double dmdqop1 = xf27*(xf17*xf33 + xf23*xf32 + xf33*xf9 - xf19/(std::pow(qop1, 3)*xf21));
  const double dmdlam1 = xf27*(xf17*xf28 + xf28*xf9 - xf29);
  const double dmdphi1 = xf27*(-xf30 + xf31);
  Matrix<double, 1, 6> res;
  res(0,0) = dmdqop0;
  res(0,1) = dmdlam0;
  res(0,2) = dmdphi0;
  res(0,3) = dmdqop1;
  res(0,4) = dmdlam1;
  res(0,5) = dmdphi1;
  
//   std::cout << "massJacobianAlt m = " << m << std::endl;




  
  return res;
}

Matrix<double, 1, 6> ResidualGlobalCorrectionMakerBase::massJacobianAltD(const Matrix<double, 7, 1> &state0, const Matrix<double, 7, 1> &state1, double dmass) const {
  
  const double mp = dmass;
  
  const double qop0 = state0[6]/state0.segment<3>(3).norm();
  const double lam0 = std::atan(state0[5]/std::sqrt(state0[3]*state0[3] + state0[4]*state0[4]));
  const double phi0 = std::atan2(state0[4], state0[3]);
  
  const double qop1 = state1[6]/state1.segment<3>(3).norm();
  const double lam1 = std::atan(state1[5]/std::sqrt(state1[3]*state1[3] + state1[4]*state1[4]));
  const double phi1 = std::atan2(state1[4], state1[3]);

  const double xf0 = std::pow(mp, 2);
  const double xf1 = std::sin(lam0);
  const double xf2 = std::sin(lam1);
  const double xf3 = 1.0/std::fabs(qop0);
  const double xf4 = 1.0/std::fabs(qop1);
  const double xf5 = xf3*xf4;
  const double xf6 = xf2*xf5;
  const double xf7 = std::sin(phi0);
  const double xf8 = std::sin(phi1);
  const double xf9 = xf7*xf8;
  const double xf10 = std::cos(lam0);
  const double xf11 = std::cos(lam1);
  const double xf12 = xf11*xf5;
  const double xf13 = xf10*xf12;
  const double xf14 = 2*xf13;
  const double xf15 = std::cos(phi0);
  const double xf16 = std::cos(phi1);
  const double xf17 = xf15*xf16;
  const double xf18 = std::pow(qop0, -2);
  const double xf19 = std::sqrt(xf0 + xf18);
  const double xf20 = std::pow(qop1, -2);
  const double xf21 = std::sqrt(xf0 + xf20);
  const double xf22 = std::sqrt(2*xf0 - 2*xf1*xf6 - xf14*xf17 - xf14*xf9 + 2*xf19*xf21);
  const double xf23 = xf1*xf2;
  const double xf24 = xf18*xf4*(((qop0) > 0) - ((qop0) < 0));
  const double xf25 = xf10*xf11;
  const double xf26 = xf24*xf25;
  const double xf27 = 1.0/xf22;
  const double xf28 = xf10*xf6;
  const double xf29 = xf1*xf12;
  const double xf30 = xf13*xf16*xf7;
  const double xf31 = xf13*xf15*xf8;
  const double xf32 = xf20*xf3*(((qop1) > 0) - ((qop1) < 0));
  const double xf33 = xf25*xf32;
  const double m = xf22;
  const double dmdqop0 = xf27*(xf17*xf26 + xf23*xf24 + xf26*xf9 - xf21/(std::pow(qop0, 3)*xf19));
  const double dmdlam0 = xf27*(xf17*xf29 - xf28 + xf29*xf9);
  const double dmdphi0 = xf27*(xf30 - xf31);
  const double dmdqop1 = xf27*(xf17*xf33 + xf23*xf32 + xf33*xf9 - xf19/(std::pow(qop1, 3)*xf21));
  const double dmdlam1 = xf27*(xf17*xf28 + xf28*xf9 - xf29);
  const double dmdphi1 = xf27*(-xf30 + xf31);
  Matrix<double, 1, 6> res;
  res(0,0) = dmdqop0;
  res(0,1) = dmdlam0;
  res(0,2) = dmdphi0;
  res(0,3) = dmdqop1;
  res(0,4) = dmdlam1;
  res(0,5) = dmdphi1;
  
//   std::cout << "massJacobianAlt m = " << m << std::endl;




  
  return res;
}

Matrix<double, 1, 6> ResidualGlobalCorrectionMakerBase::massJacobianInvSq(const FreeTrajectoryState &state0, const FreeTrajectoryState &state1, double dmass) const {
  
  const double mp = dmass;
  
  CurvilinearTrajectoryParameters curvparms0(state0.position(), state0.momentum(), state0.charge());

  const double qop0 = curvparms0.Qbp();
  const double lam0 = curvparms0.lambda();
  const double phi0 = curvparms0.phi();

  CurvilinearTrajectoryParameters curvparms1(state1.position(), state1.momentum(), state1.charge());

  const double qop1 = curvparms1.Qbp();
  const double lam1 = curvparms1.lambda();
  const double phi1 = curvparms1.phi();

  const double xf0 = std::sin(lam0);
  const double xf1 = std::sin(lam1);
  const double xf2 = xf0*xf1;
  const double xf3 = std::pow(qop0, -2);
  const double xf4 = 2/std::fabs(qop1);
  const double xf5 = xf3*xf4*(((qop0) > 0) - ((qop0) < 0));
  const double xf6 = std::sin(phi0);
  const double xf7 = std::sin(phi1);
  const double xf8 = xf6*xf7;
  const double xf9 = std::cos(lam0);
  const double xf10 = std::cos(lam1);
  const double xf11 = xf10*xf9;
  const double xf12 = xf11*xf5;
  const double xf13 = std::cos(phi0);
  const double xf14 = std::cos(phi1);
  const double xf15 = xf13*xf14;
  const double xf16 = std::pow(mp, 2);
  const double xf17 = std::sqrt(xf16 + xf3);
  const double xf18 = std::pow(qop1, -2);
  const double xf19 = std::sqrt(xf16 + xf18);
  const double xf20 = 2*xf19;
  const double xf21 = 1.0/std::fabs(qop0);
  const double xf22 = xf21*xf4;
  const double xf23 = xf1*xf22;
  const double xf24 = xf10*xf22;
  const double xf25 = xf24*xf9;
  const double xf26 = 1.0/std::pow(-xf0*xf23 - xf15*xf25 + 2*xf16 + xf17*xf20 - xf25*xf8, 2);
  const double xf27 = xf23*xf9;
  const double xf28 = xf0*xf24;
  const double xf29 = xf14*xf25*xf6;
  const double xf30 = xf13*xf25*xf7;
  const double xf31 = 2*xf18*xf21*(((qop1) > 0) - ((qop1) < 0));
  const double xf32 = xf11*xf31;
  const double dminvsqdqop0 = xf26*(-xf12*xf15 - xf12*xf8 - xf2*xf5 + xf20/(std::pow(qop0, 3)*xf17));
  const double dminvsqdlam0 = xf26*(-xf15*xf28 + xf27 - xf28*xf8);
  const double dminvsqdphi0 = xf26*(-xf29 + xf30);
  const double dminvsqdqop1 = xf26*(-xf15*xf32 - xf2*xf31 - xf32*xf8 + 2*xf17/(std::pow(qop1, 3)*xf19));
  const double dminvsqdlam1 = xf26*(-xf15*xf27 - xf27*xf8 + xf28);
  const double dminvsqdphi1 = xf26*(xf29 - xf30);
  Matrix<double, 1, 6> res;
  res(0,0) = dminvsqdqop0;
  res(0,1) = dminvsqdlam0;
  res(0,2) = dminvsqdphi0;
  res(0,3) = dminvsqdqop1;
  res(0,4) = dminvsqdlam1;
  res(0,5) = dminvsqdphi1;

  
//   std::cout << "massJacobianAlt m = " << m << std::endl;




  
  return res;
}


Matrix<double, 1, 6> ResidualGlobalCorrectionMakerBase::mrJacobian(const FreeTrajectoryState &state0, const FreeTrajectoryState &state1, double dmass) const {
  
  const double mp = dmass;
  
  CurvilinearTrajectoryParameters curvparms0(state0.position(), state0.momentum(), state0.charge());

  const double qop0 = curvparms0.Qbp();
  const double lam0 = curvparms0.lambda();
  const double phi0 = curvparms0.phi();

  CurvilinearTrajectoryParameters curvparms1(state1.position(), state1.momentum(), state1.charge());

  const double qop1 = curvparms1.Qbp();
  const double lam1 = curvparms1.lambda();
  const double phi1 = curvparms1.phi();

  const double xf0 = std::sin(lam0);
  const double xf1 = std::sin(lam1);
  const double xf2 = xf0*xf1;
  const double xf3 = std::pow(qop0, -2);
  const double xf4 = 2/std::fabs(qop1);
  const double xf5 = xf3*xf4*(((qop0) > 0) - ((qop0) < 0));
  const double xf6 = std::sin(phi0);
  const double xf7 = std::sin(phi1);
  const double xf8 = xf6*xf7;
  const double xf9 = std::cos(lam0);
  const double xf10 = std::cos(lam1);
  const double xf11 = xf10*xf9;
  const double xf12 = xf11*xf5;
  const double xf13 = std::cos(phi0);
  const double xf14 = std::cos(phi1);
  const double xf15 = xf13*xf14;
  const double xf16 = std::pow(mp, 2);
  const double xf17 = std::sqrt(xf16 + xf3);
  const double xf18 = std::pow(qop1, -2);
  const double xf19 = std::sqrt(xf16 + xf18);
  const double xf20 = 2*xf19;
  const double xf21 = 1.0/std::fabs(qop0);
  const double xf22 = xf21*xf4;
  const double xf23 = xf1*xf22;
  const double xf24 = xf10*xf22;
  const double xf25 = xf24*xf9;
  const double xf26 = std::pow(-xf0*xf23 - xf15*xf25 + 2*xf16 + xf17*xf20 - xf25*xf8, -2);
  const double xf27 = xf23*xf9;
  const double xf28 = xf0*xf24;
  const double xf29 = xf14*xf25*xf6;
  const double xf30 = xf13*xf25*xf7;
  const double xf31 = 2*xf18*xf21*(((qop1) > 0) - ((qop1) < 0));
  const double xf32 = xf11*xf31;
  const double dmrdqop0 = xf26*(-xf12*xf15 - xf12*xf8 - xf2*xf5 + xf20/(std::pow(qop0, 3)*xf17));
  const double dmrdlam0 = xf26*(-xf15*xf28 + xf27 - xf28*xf8);
  const double dmrdphi0 = xf26*(-xf29 + xf30);
  const double dmrdqop1 = xf26*(-xf15*xf32 - xf2*xf31 - xf32*xf8 + 2*xf17/(std::pow(qop1, 3)*xf19));
  const double dmrdlam1 = xf26*(-xf15*xf27 - xf27*xf8 + xf28);
  const double dmrdphi1 = xf26*(xf29 - xf30);
  Matrix<double, 1, 6> res;
  res(0,0) = dmrdqop0;
  res(0,1) = dmrdlam0;
  res(0,2) = dmrdphi0;
  res(0,3) = dmrdqop1;
  res(0,4) = dmrdlam1;
  res(0,5) = dmrdphi1;

  
//   std::cout << "massJacobianAlt m = " << m << std::endl;




  
  return res;
}

Matrix<double, 5, 1> ResidualGlobalCorrectionMakerBase::elossAdHocJacobian(const FreeTrajectoryState &state, double mass) const {
  
  const double qop0 = state.signedInverseMomentum();
  const double q = state.charge();
  
  const double eloss = 0.01; // 10 MeV
  
  const double x0 = std::pow(qop0, 2);
  const double x1 = std::pow(q, 2);
  const double dqopdde = -eloss*q*x0*std::sqrt(std::pow(mass, 2) + x1/x0)*std::fabs(qop0)/std::pow(x1, 3.0/2.0);
  const double dlamdde = 0;
  const double dphidde = 0;
  const double dxtdde = 0;
  const double dytdde = 0;
  Eigen::Matrix<double, 5, 1> res;
  res(0,0) = dqopdde;
  res(1,0) = dlamdde;
  res(2,0) = dphidde;
  res(3,0) = dxtdde;
  res(4,0) = dytdde;

  return res;

}

Matrix<double, 5, 1> ResidualGlobalCorrectionMakerBase::elossAdHocJacobianD(const Matrix<double, 7, 1> &state, double mass) const {
  
  const double q = state[6];
  const double qop0 = q/state.segment<3>(3).norm();
  
  const double eloss = 0.01; // 10 MeV
  
  const double x0 = std::pow(qop0, 2);
  const double x1 = std::pow(q, 2);
  const double dqopdde = -eloss*q*x0*std::sqrt(std::pow(mass, 2) + x1/x0)*std::fabs(qop0)/std::pow(x1, 3.0/2.0);
  const double dlamdde = 0;
  const double dphidde = 0;
  const double dxtdde = 0;
  const double dytdde = 0;
  Eigen::Matrix<double, 5, 1> res;
  res(0,0) = dqopdde;
  res(1,0) = dlamdde;
  res(2,0) = dphidde;
  res(3,0) = dxtdde;
  res(4,0) = dytdde;

  return res;

}

//define this as a plug-in
// DEFINE_FWK_MODULE(ResidualGlobalCorrectionMakerBase);
