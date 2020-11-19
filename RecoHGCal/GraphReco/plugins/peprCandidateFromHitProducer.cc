/*
 * CMSSW plugin that performs a Window-based inference of networks using RecHits and produces PF candidates.
 *
 * Authors: Gerrit Van Onsem <Gerrit.Van.Onsem@cern.ch>
 *          Marcel Rieger <marcel.rieger@cern.ch>
 */

#include <memory>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/ParameterSet/interface/FileInPath.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/HGCRecHit/interface/HGCRecHitCollections.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"

#include "RecoLocalCalo/HGCalRecAlgos/interface/RecHitTools.h"

#include "RecoHGCal/GraphReco/interface/InferenceWindow.h"

#include "DataFormats/Common/interface/View.h"

#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h" 
#include "DataFormats/Candidate/interface/LeafCandidate.h"

#include "DataFormats/HGCalReco/interface/Trackster.h"

using namespace ticl;

// macros for simplified logs
// message logger disabled for the moment
// #define INFO edm::LogInfo("peprCandidateFromHitProducer")
// #define WARNING edm::LogWarning("peprCandidateFromHitProducer")
// #define ERROR edm::LogError("peprCandidateFromHitProducer")
#define INFO std::cout << "peprCandidateFromHitProducer INFO   : "
#define WARNING std::cout << "peprCandidateFromHitProducer WARNING: "
#define ERROR std::cout << "peprCandidateFromHitProducer ERROR  : "


class peprCandidateFromHitProducer: public edm::stream::EDProducer<> {
 public:
    explicit peprCandidateFromHitProducer(const edm::ParameterSet&);
    ~peprCandidateFromHitProducer();

 private:
    void beginStream(edm::StreamID);
    void endStream();
    void produce(edm::Event&, const edm::EventSetup&) override;

    void fillWindows(const edm::Event&);
    void writeInputArrays(const std::vector<std::vector<float> >&);
    void readOutputArrays(std::vector<std::vector<float> >&);
    pid_t start_background(std::string);

    // options
    std::vector<edm::InputTag> recHitCollections_;

    // tokens
    std::vector<edm::EDGetTokenT<HGCRecHitCollection> > recHitTokens_;  
    edm::EDGetTokenT<edm::View<reco::Track>> tracksToken_;

    std::string tritonScript_;
    std::string inpipeName_;
    std::string outpipeName_;
    std::string containerPIDName_;

    // rechit tools
    hgcal::RecHitTools recHitTools_;

    double minCandEnergy_;

    // windows
    std::vector<InferenceWindow> windows_;

    double minEta_;
    double maxEta_;
    double etaFrameWidth_;
    double phiFrameWidth_;
    size_t nEtaSegments_;
    size_t nPhiSegments_;

};


peprCandidateFromHitProducer::peprCandidateFromHitProducer(const edm::ParameterSet& config) :
        recHitCollections_(config.getParameter<std::vector<edm::InputTag> >("recHitCollections")), 
        tracksToken_(consumes<edm::View<reco::Track>>(config.getParameter<edm::InputTag>("tracks"))),
        tritonScript_(config.getParameter<edm::FileInPath>("tritonScript").fullPath()), 
        inpipeName_(config.getParameter<std::string>("inpipeName")),
        outpipeName_(config.getParameter<std::string>("outpipeName")),
        minCandEnergy_(config.getParameter<double>("minCandEnergy")),
        //FIXME: actually these are all not needed if windows are created in the constructor!
        minEta_(config.getParameter<double>("minEta")),
        maxEta_(config.getParameter<double>("maxEta"))
        {

    // get tokens
    for (edm::InputTag& recHitCollection : recHitCollections_) {
        recHitTokens_.push_back(consumes<HGCRecHitCollection>(recHitCollection));
    }
   
    // window size and overlap in phi and eta
    etaFrameWidth_ = 0.2;
    phiFrameWidth_ = 0.2;
    nEtaSegments_ = 1;
    nPhiSegments_ = 1;


    produces<reco::PFCandidateCollection>();
    //produces<std::vector<Trackster>>();

    int pid = getpid();
    char * mypid = (char*)malloc(6);
    sprintf(mypid, "%d", pid);
    //std::cout << "Process ID of producer: " << mypid << std::endl;
    inpipeName_ = std::string(mypid) + "_" + inpipeName_;
    outpipeName_ = std::string(mypid) + "_" + outpipeName_;
    containerPIDName_ = "containerpid";
    //for local testing (e.g. when one does not know the pid in advance)
    //inpipeName_ = "TEST_" + inpipeName_;
    //outpipeName_ = "TEST_" + outpipeName_;


    //launch script(s) to work with Triton clients
    //https://github.com/cms-pepr/HGCalML/tree/master/triton
    
    //within the client script, the container is also started in the background and its process ID is stored for later kill command in destructor
    std::string clientcommand = tritonScript_ + " " + inpipeName_ + " " + containerPIDName_ + " &";
    system(clientcommand.data());
    //std::cout << "  Forward client command executed" << std::endl;

}

peprCandidateFromHitProducer::~peprCandidateFromHitProducer() {

    std::string containerpid_location="/dev/shm/" + containerPIDName_;
    int containerpid;
    std::ifstream containerpidfile;
    containerpidfile.open(containerpid_location);
    containerpidfile >> containerpid;
    //std::cout << "Process ID of triton container being killed: " << containerpid << std::endl;
    system(("kill -9 " + std::to_string(containerpid)).data());
}


void peprCandidateFromHitProducer::beginStream(edm::StreamID streamId) {
    windows_ = InferenceWindow::createWindows(nPhiSegments_,nEtaSegments_,minEta_,maxEta_,etaFrameWidth_,phiFrameWidth_);

    // FIXME, make configurable?
    for(auto& w: windows_)
        w.setMode(WindowBase::useRechits);
}

void peprCandidateFromHitProducer::endStream() {

    //windows_.clear();
}

void peprCandidateFromHitProducer::produce(edm::Event& event, const edm::EventSetup& setup) {
    // fill rechits into windows
    fillWindows(event);

    //vector (from multiple windows) of vector of ~ OC candidates
    std::vector< std::vector<std::vector<float> > > windowoutputs; 
    
    // run the evaluation per window
    std::vector<std::vector<float> >  hitFeatures;
    for (auto & window : windows_) {
        //std::cout << "New window " << std::endl;

        hitFeatures = window.getHitFeatures();
        //std::cout << "   hitFeatures size = " << hitFeatures.size() << std::endl;
        //std::cout << "      number of features per hit = " << hitFeatures[0].size() << std::endl;

        writeInputArrays(hitFeatures);
 
        //inner vector is regressed energy, 2D position, and time (total 4 elements)
        std::vector<std::vector<float> >  candidates;
        readOutputArrays(candidates); 

        windowoutputs.push_back( candidates );

        //break; //if quickly testing just one window
    }


    // making candidate collection
    auto pfcandidates = std::make_unique<reco::PFCandidateCollection>();
    //std::cout << "Creating PF candidates " << std::endl;

    for (size_t i=0; i<windowoutputs.size(); i++) {

        //loop over windows
        //std::cout << " Window " << i << std::endl;

        float E=-9999., X = -9999., Y=-9999., Z=-9999.;

        // loop over particles reconstructed in the current window
        for(size_t j=0;j<windowoutputs[i].size();j++) {

            //std::cout << " particle " << j << std::endl;

            //const auto abs_pdg_id = -9999;
            const auto charge = 0; // FIXME
                     
            //inner index as filled in readOutputArrays
            E = windowoutputs[i][j][0];

            //temporary lower threshold on energy of candidates
            if(E < minCandEnergy_) continue;

            X = windowoutputs[i][j][1];
            Y = windowoutputs[i][j][2];
            Z = windowoutputs[i][j][3];

            //block inspired by calcP4 method in TICL TracksterP4FromEnergySum plugin
            //below assuming (0,0,0) as vertex
            //starts from 'position (X,Y,Z)'
            math::XYZVector direction(X, Y, Z);
            direction = direction.Unit();
            direction *= E;
            math::XYZTLorentzVector cartesian(direction.X(), direction.Y(), direction.Z(), E);
            //// Convert px, py, pz, E vector to CMS standard pt/eta/phi/m vector
            reco::Candidate::LorentzVector p4(cartesian);

            // //Alternative calculation as check
            // float R = std::sqrt(X*X + Y*Y + Z*Z);
            // float px = E * X / R;
            // float py = E * Y / R;
            // float pz = E * Z / R;
            // std::cout << "  (px,py,pz) = " << "(" << px << "," << py << "," << pz << ")" << std::endl;

            const auto& four_mom = p4;
            //std::cout << "    PF particle energy " << four_mom.E() << ", Px = " << four_mom.Px() << ", Py = " << four_mom.Py() << ", Pz = " << four_mom.Pz() << std::endl;
            reco::PFCandidate::ParticleType part_type = reco::PFCandidate::X;
            pfcandidates->emplace_back(charge, four_mom, part_type);

        }

    }



    event.put(std::move(pfcandidates));

    //std::cout << "Results produced and put in event" << std::endl;


    // clear all windows
    for (auto& window : windows_) {
        window.clear();
    }

    //std::cout << "Windows cleared" << std::endl;
}



void peprCandidateFromHitProducer::fillWindows(const edm::Event& event) {

    if (!windows_.size()) {
        throw cms::Exception("NoWindows") << "no windows initialized";
    }
    
    //std::cout << "Number of windows = " << windows_.size() << std::endl;
    
    // copied block from window ntupler code
    // get rechits, get positions and merge collections
    std::vector<HGCRecHitWithPos> allrechits;
    for (auto & token : recHitTokens_) {
       for (const auto& rh : event.get(token)) {
           HGCRecHitWithPos rhp = { const_cast<HGCRecHit*>(&rh), recHitTools_.getPosition(rh.detid()) };
           allrechits.push_back(rhp);
       }
    }
    // sort according to the energy
    std::sort(allrechits.begin(), allrechits.end(), 
        [](const HGCRecHitWithPos& rh1, const HGCRecHitWithPos& rh2) 
            { return rh1.hit->energy() > rh2.hit->energy();});

    
    // fills a vector of the specified size with zeroes (entries will be 0 if rechit is not filled, and 1 if it is filled)
    std::vector<size_t> filledrechits(allrechits.size(),0);


    for (auto & window : windows_) {
        //fill rechits in this window
        for(size_t it=0;it<allrechits.size();it++) {
           if(filledrechits.at(it)>3) continue;
           if(window.maybeAddRecHit(allrechits.at(it)))
               filledrechits.at(it)++;
        }

        window.fillFeatureArrays();
    }


}


void peprCandidateFromHitProducer::writeInputArrays(const std::vector<std::vector<float> >& hitFeatures) {

    std::string inpipeRAM = "/dev/shm/" + inpipeName_; //write in RAM
    //std::string inpipeRAM = inpipeName_; //do not write in RAM (local testing)

    //wait until pipe is open
    std::cout << "Checking if pipe is open..." << std::endl;
    bool pipe_open = false;
    while(!pipe_open) {
        struct stat buf;
        if (stat(inpipeRAM.c_str(), &buf) != -1)
        {
            std::cout << "   Pipe open --> proceed" << std::endl;
            pipe_open = true;
        }
    }

    std::ofstream inputArrayStream;
    inputArrayStream.open(inpipeRAM.c_str(), std::ofstream::out | std::ofstream::app);
    inputArrayStream.flush();
    //std::cout << "     inpipeRAM = " << inpipeRAM << std::endl;

    inputArrayStream << std::setprecision(std::numeric_limits<float>::digits10 + 1) << std::scientific;
    for (size_t i=0; i<hitFeatures.size(); i++) {

        for (size_t j=0; j<hitFeatures[i].size(); j++) {
            inputArrayStream << hitFeatures[i][j] << " ";
        }
        inputArrayStream << "\n";
    }

    inputArrayStream.close();
}


void peprCandidateFromHitProducer::readOutputArrays(std::vector<std::vector<float> >& candidates) {

    std::string outpipeRAM = "/dev/shm/" + outpipeName_; //read from RAM
    //std::string outpipeRAM = outpipeName_; //do not read from RAM (local testing)

    std::ifstream outputArrayStream;
    //outputArrayStream.open(outpipeRAM.c_str(), std::ofstream::out | std::ofstream::app);
    outputArrayStream.open(outpipeRAM.c_str());
    //std::cout << "     outpipeRAM = " << outpipeRAM << std::endl;

    std::string line;
    if(outputArrayStream.is_open()) {
        //std::cout << "Output array file opened!" << std::endl;
        while ( true )   
        {
            getline (outputArrayStream,line);
            //std::cout << line << '\n';

            if(line.empty()){
                break;
            }

            // //split line according to spaces
            std::istringstream iss(line);
            std::vector<std::string> values(std::istream_iterator<std::string>{iss},
                                 std::istream_iterator<std::string>());

            std::string energy_str, X_str, Y_str, Z_str, Xrel_str, Yrel_str; //T_str;
            float E = -9999., X = -9999., Y = -9999., Z = -9999.; //T = -9999.;

            if(values.size() == 16 || values.size() == 17) {

                //input rechit position
                X_str = values[5];
                Y_str = values[6];
                Z_str = values[7];

                energy_str = values[10];
                //values are relative to input rechit position
                Xrel_str = values[11];
                Yrel_str = values[12];
                //FIXME: is T also relative to T of rechit? Not used for now
                //T_str = values[13]; 

                E = std::stof(energy_str);
                X = std::stof(X_str) + std::stof(Xrel_str);
                Y = std::stof(Y_str) + std::stof(Yrel_str);

                //positive Z of rechit means Z = 320 cm (as regressed X,Y is measured on HGCAL surface)
                if ( std::stof(Z_str) > 0 ) Z = 320.;  //in cm
                else Z = -320.;   //in cm
                    
                //std::cout << "    Storing values from the model output" << std::endl;
                std::vector<float> candidate = {E, X, Y, Z };
                candidates.push_back( candidate );
            }
            else {
                if (values.size() == 2) {
                    //std::cout << "    this is the line with shapes; ignore for now" << std::endl;
                }
                else {
                    std::cout << "    Please check model output to retrieve desired regressed properties" << std::endl;
                }
            }

        }
        //std::cout << "Closing output array stream" << std::endl;
        outputArrayStream.close();
    }


}


DEFINE_FWK_MODULE(peprCandidateFromHitProducer);
