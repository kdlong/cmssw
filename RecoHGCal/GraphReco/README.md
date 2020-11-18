# Running the pepr PF candidate producer for HGCAL

This example demonstrates how to run the particle reconstruction in the HGCAL subdetector via inference of graph neural networks. 

## Setup

Install CMSSW:
```
export SCRAM_ARCH="slc7_amd64_gcc820"
cmsrel CMSSW_11_1_0_pre7
cd CMSSW_11_1_0_pre7/src
cmsenv
scram b -j 8
```

Install custom packages: 
```
git cms-init
git cms-merge-topic cms-pepr:pepr_CMSSW_11_1_0_pre7
scram b -j 8
```

This will check out the necessary packages. To use the PEPR reconstruction, you only need to add the producer to the RECO sequence:
```
process.load(“RecoHGCal/GraphReco/python/peprCandidateFromHitProducer_cfi”)
```
This will provide 
```
process.peprCandidateFromHitProducer
```
and produce ```recoPFCandidates_peprCandidateFromHitProducer__RECO```


## Generate events and more information

For testing, also scripts to produce events are provided.

First we produce GEN-SIM-DIGI (GSD) events, in this example by shooting particles (e.g. photons) 
in a certain energy range towards the HGCAL subdetector via the `FlatEtaRangeGunProducer`.
```
cd RecoHGCal/GraphReco/test
cmsRun GSD_GUN.py seed=1 outputFile="file:1_GSD.root" maxEvents=5
```
Once the GSD events are produced, we can run the reconstruction step: 
```
cmsRun RECO_pf.py inputFiles="file://1_GSD.root" outputFile="file:1_RECO.root" outputFileDQM="file:1_DQM.root" maxEvents=5
```
A dedicated **EDProducer module**, the `peprCandidateFromHitProducer` located 
in the [RecoHGCAL/GraphReco](.) package, 
produces PF candidates straight from rechit information, in this example via the [Object Condensation](https://arxiv.org/abs/2002.03605) method. 

The inference of trained graph neural network models is done by sending the rechit information per endcap to a custom Triton server, evaluating the model, 
and retrieving the regressed energy and position of clustered particle candidates. 
These candidates are subsequently turned into a PFcandidate collection named `recoPFCandidates_peprCandidateFromHitProducer__RECO`. Particle and charge identification as well as track-cluster matching are work in progress and not included yet. 

**Note:** it may take some time for the first event in the reconstruction to start. The messages printed during that time will be:

```
INFO:    Convert SIF file to sandbox...
...
Checking if pipe is open...
```
The speed of communication with the client and launching it will drastically improve with a next CMSSW update, where the new version of the client will be included directly in CMSSW. 
Moreover, also the inference time on each event will improve by orders of magnitude once dedicated Triton GPU servers are used. Right now, with only a CPU triton server, it is not advised to process full events, this will take multiple minutes.


The **sequence** of the producer module is as follows:
* In the constructor of the producer, the Triton client is started.
* The producer checks for open pipes (set up to communicate with the Triton server) and will wait until the pipes are open to send the rechit data to the server.
* In case the pipes are open before the producer reaches the check, the client will wait until the rechit data is passed from the producer.
* The inference itself is done on the Triton server via the trained model that is stored there, and the results are passed back to the module where a collection of reconstructed particle candidates is created.
* The Triton client is automatically closed in the destructor of the producer

### Adding the peprCandidateFromHitProducer to your favorite RECO config

The `peprCandidateFromHitProducer` can be loaded from the [config fragment](python/peprCandidateFromHitProducer_cfi.py) and added to a RECO sequence in the usual way. Add the following line to your python config

```
process.load("RecoHGCal/GraphReco/python/peprCandidateFromHitProducer_cfi")
```

this adds the producer to the process, you should then schedule it by adding it to a scheduled sequence, for example:

```
process.reconstruction_step += process.peprCandidateFromHitProducer
```

It's not expected that you would need to change arguments from their default values, but 
this can be done as for other EDProducer modules, for example

```
process.peprCandidateFromHitProducer.tracks = "globalMuons"  # Not a very good idea, but possible
```
