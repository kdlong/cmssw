#include <cstdio>
#include <memory>
#include <vector>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"

#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/FileInPath.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenWeightInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

#include "GeneratorInterface/LHEInterface/interface/LHERunInfo.h"
#include "GeneratorInterface/LHEInterface/interface/LHEEvent.h"
#include "GeneratorInterface/Core/interface/LHEWeightHelper.h"

#include "FWCore/ServiceRegistry/interface/Service.h"

class LHEWeightProductProducer : public edm::one::EDProducer<edm::BeginLuminosityBlockProducer,
                                                                edm::one::WatchRuns> {
public:
  explicit LHEWeightProductProducer(const edm::ParameterSet& iConfig);
  ~LHEWeightProductProducer() override;
  
private:
  gen::LHEWeightHelper weightHelper_;
  std::string lheLabel_;
  edm::EDGetTokenT<LHERunInfoProduct> lheRunInfoToken_;
  edm::EDGetTokenT<LHEEventProduct> lheEventToken_;
  const edm::EDGetTokenT<GenWeightInfoProduct> lheWeightInfoToken_;
  bool foundWeightProduct_;

  void produce(edm::Event&, const edm::EventSetup&) override;
  void beginLuminosityBlockProduce(edm::LuminosityBlock& lumi, edm::EventSetup const& es) override;
  void beginRun(edm::Run const& run, edm::EventSetup const& es) override;
  void endRun(edm::Run const& run, edm::EventSetup const& es) override;

};

// TODO: Accept a vector of strings (source, externalLHEProducer) exit if neither are found
LHEWeightProductProducer::LHEWeightProductProducer(const edm::ParameterSet& iConfig) :
    lheLabel_(iConfig.getParameter<std::string>("lheSourceLabel")),
    lheRunInfoToken_(consumes<LHERunInfoProduct, edm::InRun>(lheLabel_)),
    lheEventToken_(consumes<LHEEventProduct>(lheLabel_)),
    lheWeightInfoToken_(consumes<GenWeightInfoProduct, edm::InLumi>(lheLabel_))
{
  produces<GenWeightProduct>();
  produces<GenWeightInfoProduct, edm::Transition::BeginLuminosityBlock>();
}


LHEWeightProductProducer::~LHEWeightProductProducer()
{
}


// ------------ method called to produce the data  ------------
void
LHEWeightProductProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  if (foundWeightProduct_)
      return;
  
  edm::Handle<LHEEventProduct> lheEventInfo;
  iEvent.getByToken(lheEventToken_, lheEventInfo);
  // Read weights from LHEEventProduct
  auto weightProduct = weightHelper_.weightProduct(lheEventInfo->weights(), lheEventInfo->originalXWGTUP());
  iEvent.put(std::move(weightProduct));
}

void 
LHEWeightProductProducer::beginRun(edm::Run const& run, edm::EventSetup const& es) {
    edm::Handle<LHERunInfoProduct> lheRunInfoHandle;
    run.getByLabel(lheLabel_, lheRunInfoHandle);

    typedef std::vector<LHERunInfoProduct::Header>::const_iterator header_cit;
    LHERunInfoProduct::Header headerWeightInfo;
    for (header_cit iter=lheRunInfoHandle->headers_begin(); iter!=lheRunInfoHandle->headers_end(); iter++) { 
        if (iter->tag() == "initrwgt")
            headerWeightInfo = *iter;
    }

	//weightHelper_.parseWeightGroupsFromHeader(headerWeightInfo.lines());
}

void
LHEWeightProductProducer::endRun(edm::Run const& run, edm::EventSetup const& es) { }

void
LHEWeightProductProducer::beginLuminosityBlockProduce(edm::LuminosityBlock& lumi, edm::EventSetup const& es) {
    edm::Handle<GenWeightInfoProduct> lheWeightInfoHandle;
    lumi.getByToken(lheWeightInfoToken_, lheWeightInfoHandle);
    if (lheWeightInfoHandle.isValid()) {
        foundWeightProduct_ = true;
        return;
    }

    auto weightInfoProduct = std::make_unique<GenWeightInfoProduct>();
    for (auto& weightGroup : weightHelper_.weightGroups()) {
        weightInfoProduct->addWeightGroupInfo(weightGroup.clone());
    }
    lumi.put(std::move(weightInfoProduct));
}

DEFINE_FWK_MODULE(LHEWeightProductProducer);

