from DataFormats.FWLite import Events,Handle,Runs,Lumis
import ROOT
sources = ["externalLHEProducer"]
#sources = ["testLHEWeights", "testGenWeights"]

#for filename in ["HIG-RunIIFall18wmLHEGS-00509.root"," HIG-RunIIFall18wmLHEGS-00509_ordered.root","HIG-RunIIFall18wmLHEGS-00509_unordered.root"]:
for filename in ["HIG-RunIIFall18wmLHEGS-00509.root"]:
#for filename in ["test.root"]:
    for source in sources:
        lumis = Lumis(filename)
        lumi = lumis.__iter__().next()
        weightInfoHandle = Handle("GenWeightInfoProduct")
        lumi.getByLabel(source, weightInfoHandle)
        weightInfoProd = weightInfoHandle.product()

        events = Events(filename)
        event = events.__iter__().next()
        weightHandle = Handle("GenWeightProduct")
        event.getByLabel(source, weightHandle)
        weightInfo = weightHandle.product()
        print weightInfoProd.allWeightGroupsInfo(), len(weightInfoProd.allWeightGroupsInfo())
        for j, weights in enumerate(weightInfo.weights()):
            print "-"*10, "Looking at entry", j, "length is", len(weights),"-"*10
            matching = weightInfoProd.orderedWeightGroupInfo(j)
            print "Group is", matching, "name is", matching.name(), "well formed?", matching.isWellFormed()
            print "Group description", matching.description()
            print type(matching.weightType()), matching.weightType()
            if matching.weightType() == 's':
                for var in [(x, y) for x in ["05", "1", "2"] for y in ["05", "1", "2"]]:
                    name = "muR%smuF%sIndex" % (var[0], var[1]) if not (var[0] == "1" and var[1] == "1") else "centralIndex"
                    print name, getattr(matching, name)()
            elif matching.weightType() == 'P':
                print "uncertaintyType", "Hessian" if matching.uncertaintyType() == ROOT.gen.kHessianUnc else "MC"
                print "contains LHAPDFIds", len(matching.lhapdfIdsContained()), [i for i in matching.lhapdfIdsContained()],
                print "Has alphas? ", matching.hasAlphasVariations()
            print "Weights length?", len(weights), "Contained ids lenths?", len(matching.containedIds())
            print "-"*80
            for i,weight in enumerate(weights):
                print i, weight
                info = matching.weightMetaInfo(i)
                print "   ID, localIndex, globalIndex, label, set:", info.id, info.localIndex, info.globalIndex, info.label, matching.name()
            print "-"*80



