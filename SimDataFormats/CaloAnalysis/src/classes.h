#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/Common/interface/AssociationMapHelpers.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/CaloParticle.h"
#include "SimDataFormats/CaloAnalysis/interface/CaloParticleFwd.h"
#include "SimDataFormats/CaloAnalysis/interface/SimCluster.h"
#include "SimDataFormats/CaloAnalysis/interface/SimClusterFwd.h"

#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/AssociationMap.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/OneToManyWithQuality.h"
#include "DataFormats/Common/interface/OneToManyWithQualityGeneric.h"
#include "DataFormats/Common/interface/View.h"
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

    edm::helpers::KeyVal<edm::RefProd<std::vector<TrackingParticle> >,edm::RefProd<std::vector<SimCluster> > > am0;
    edm::helpers::KeyVal<edm::Ref<std::vector<TrackingParticle>,TrackingParticle,edm::refhelper::FindUsingAdvance<std::vector<TrackingParticle>,TrackingParticle> >,std::vector<std::pair<edm::Ref<std::vector<SimCluster>,SimCluster,edm::refhelper::FindUsingAdvance<std::vector<SimCluster>,SimCluster> >,float> > > amf1;
    edm::AssociationMap<edm::OneToManyWithQuality<std::vector<TrackingParticle>,std::vector<SimCluster>,float,unsigned int> > amf2;
    edm::Wrapper<edm::AssociationMap<edm::OneToManyWithQuality<std::vector<TrackingParticle>,std::vector<SimCluster>,float,unsigned int> > > amf3;
    std::map<unsigned int,edm::helpers::KeyVal<edm::Ref<std::vector<TrackingParticle>,TrackingParticle,edm::refhelper::FindUsingAdvance<std::vector<TrackingParticle>,TrackingParticle> >,std::vector<std::pair<edm::Ref<std::vector<SimCluster>,SimCluster,edm::refhelper::FindUsingAdvance<std::vector<SimCluster>,SimCluster> >,float> > > > amf4;
    edm::helpers::KeyVal<edm::Ref<std::vector<TrackingParticle>,TrackingParticle,edm::refhelper::FindUsingAdvance<std::vector<TrackingParticle>,TrackingParticle> >,std::vector<std::pair<edm::Ref<std::vector<SimCluster>,SimCluster,edm::refhelper::FindUsingAdvance<std::vector<SimCluster>,SimCluster> >,int> > > am1;
    edm::AssociationMap<edm::OneToManyWithQuality<std::vector<TrackingParticle>,std::vector<SimCluster>,int,unsigned int> > am2;
    edm::Wrapper<edm::AssociationMap<edm::OneToManyWithQuality<std::vector<TrackingParticle>,std::vector<SimCluster>,int,unsigned int> > > am3;
    std::map<unsigned int,edm::helpers::KeyVal<edm::Ref<std::vector<TrackingParticle>,TrackingParticle,edm::refhelper::FindUsingAdvance<std::vector<TrackingParticle>,TrackingParticle> >,std::vector<std::pair<edm::Ref<std::vector<SimCluster>,SimCluster,edm::refhelper::FindUsingAdvance<std::vector<SimCluster>,SimCluster> >,int> > > > am4;

    edm::helpers::KeyVal<edm::RefProd<std::vector<SimCluster> >,edm::RefProd<std::vector<TrackingParticle> > > ma0;
    edm::helpers::KeyVal<edm::Ref<std::vector<SimCluster>,SimCluster,edm::refhelper::FindUsingAdvance<std::vector<SimCluster>,SimCluster> >,std::vector<std::pair<edm::Ref<std::vector<TrackingParticle>,TrackingParticle,edm::refhelper::FindUsingAdvance<std::vector<TrackingParticle>,TrackingParticle> >,int> > > ma1;
    edm::AssociationMap<edm::OneToManyWithQuality<std::vector<SimCluster>,std::vector<TrackingParticle>,int,unsigned int> > ma2;

    edm::helpers::KeyVal<edm::RefProd<std::vector<TrackingParticle> >,edm::RefProd<std::vector<SimCluster> > > a1;
    edm::helpers::KeyVal<edm::Ref<std::vector<TrackingParticle>,TrackingParticle,edm::refhelper::FindUsingAdvance<std::vector<TrackingParticle>,TrackingParticle> >,std::vector<std::pair<edm::Ref<std::vector<SimCluster>,SimCluster,edm::refhelper::FindUsingAdvance<std::vector<SimCluster>,SimCluster> >,float> > > a2;
    std::map<unsigned int,edm::helpers::KeyVal<edm::Ref<std::vector<TrackingParticle>,TrackingParticle,edm::refhelper::FindUsingAdvance<std::vector<TrackingParticle>,TrackingParticle> >,std::vector<std::pair<edm::Ref<std::vector<SimCluster>,SimCluster,edm::refhelper::FindUsingAdvance<std::vector<SimCluster>,SimCluster> >,float> > > > a3;

    edm::Wrapper<edm::AssociationMap<edm::OneToManyWithQuality<std::vector<SimCluster>,std::vector<TrackingParticle>,int,unsigned int> > > ma3;
    std::map<unsigned int,edm::helpers::KeyVal<edm::Ref<std::vector<SimCluster>,SimCluster,edm::refhelper::FindUsingAdvance<std::vector<SimCluster>,SimCluster> >,std::vector<std::pair<edm::Ref<std::vector<TrackingParticle>,TrackingParticle,edm::refhelper::FindUsingAdvance<std::vector<TrackingParticle>,TrackingParticle> >,int> > > > ma4;
    edm::Wrapper<edm::Association<std::vector<TrackingParticle> > > wasso;
    edm::Association<std::vector<TrackingParticle> >  asso;

    std::vector<std::pair<edm::Ref<std::vector<TrackingParticle>,TrackingParticle,edm::refhelper::FindUsingAdvance<std::vector<TrackingParticle>,TrackingParticle> >,int> > ma5;
    std::pair<edm::Ref<std::vector<TrackingParticle>,TrackingParticle,edm::refhelper::FindUsingAdvance<std::vector<TrackingParticle>,TrackingParticle> >,int> ma6;
  }  // namespace CaloAnalysis
}  // namespace SimDataFormats
