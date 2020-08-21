#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/Common/interface/AssociationMapHelpers.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/CaloParticle.h"
#include "SimDataFormats/CaloAnalysis/interface/CaloParticleFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"

namespace SimDataFormats {
  namespace CaloAnalysis {
    SimCluster sc;
    SimClusterCollection vsc;
    edm::Wrapper<SimClusterCollection> wvsc;

    SimClusterRef scr;
    SimClusterRefVector scrv;
    SimClusterRefProd scrp;
    SimClusterContainer scc;

    SimClusterHistory sch;
    edm::Wrapper<SimClusterHistory> wsch;

    CaloParticle cp;
    CaloParticleCollection vcp;
    edm::Wrapper<CaloParticleCollection> wvcp;

    CaloParticleRef cpr;
    CaloParticleRefVector cprv;
    CaloParticleRefProd cprp;
    CaloParticleContainer cpc;
    edm::helpers::KeyVal<edm::RefProd<std::vector<TrackingParticle> >,
                                               edm::RefProd<std::vector<SimCluster> > > testy;
    //std::pair<edm::Ref<std::vector<SimCluster>,SimCluster,edm::refhelper::FindUsingAdvance<std::vector<SimCluster>,SimCluster> >,double> camarche;
    edm::AssociationMap<edm::OneToManyWithQuality<std::vector<TrackingParticle>,std::vector<SimCluster>,double,unsigned int>> test1;
    edm::Wrapper<edm::AssociationMap<edm::OneToManyWithQuality<std::vector<TrackingParticle>,std::vector<SimCluster>,double,unsigned int>>> test;
    edm::Ref<std::vector<TrackingParticle>,TrackingParticle,edm::refhelper::FindUsingAdvance<std::vector<TrackingParticle>,TrackingParticle> > bleh1;
    std::vector<std::pair<edm::Ref<std::vector<SimCluster>,SimCluster,edm::refhelper::FindUsingAdvance<std::vector<SimCluster>,SimCluster> >,double> > bleh2;
  }  // namespace CaloAnalysis
}  // namespace SimDataFormats
