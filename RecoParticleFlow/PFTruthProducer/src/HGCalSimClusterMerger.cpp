
#include "../interface/HGCalSimClusterMerger.h"
#include "../interface/SimClusterMerger.h"
#include "Geometry/HGCalGeometry/interface/HGCalGeometry.h"
#include "DataFormats/HcalDetId/interface/HcalSubdetector.h"

//DEBUG
#include <fstream>



//create the properties
using SCVertex=HitMergeVertex;// RadiusMergeVertex, SCGausMergeVertex; OldRadiusMergeVertex
using SCWrapper=SimClusterMergeWrapper;


/*
 * DEBUG PART
 */

#include "TGraph.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TLatex.h"
#include "TStyle.h"
#include <chrono>


void drawVertices(const std::vector<SCVertex>& result, bool onlytext=false, bool merged=false){
    int count=1;
    TGraph * g = 0;
    for(const auto v : result){
        if(!v.valid())
            continue;
        std::vector<float>  grpx,grpy;

        float meanx=0,meany=0,totale=0;


        for(const auto sc: v.objects()){
            if(!merged){
                grpx.push_back(sc->x());
                grpy.push_back(sc->y());
            }
            meanx+=sc->x()*sc->e();
            meany+=sc->y()*sc->e();
            totale+=sc->e();

            TEllipse *el1 = new TEllipse(sc->x(),sc->y(),sc->r(),sc->r());
            //el1->SetLineColorAlpha(count,0.8);
            el1->SetLineWidth(2);
            el1->SetFillColorAlpha(0,0);
            if(!onlytext)
                el1->Draw("same");
        }
        meanx/=totale;
        meany/=totale;

        if(merged){
            grpx.push_back(meanx);
            grpy.push_back(meany);
        }

        TLatex * l = new TLatex( meanx,meany, v.name().data());
        l->SetTextSize(l->GetTextSize()/6.);
        l->SetTextColor(kWhite);
       //no text l->Draw("same");

        g = new TGraph(grpx.size(),&grpx.at(0),&grpy.at(0));

        g->SetMarkerStyle(5);
        //g->SetMarkerColor(count);
        g->Draw("P");

        count++;
    }
}

float getrange(const std::vector<SCWrapper>& scwappers){
    float minmax=0;
    for(const auto& sc: scwappers){
        float x = fabs(sc.pos().x());
        float y = fabs(sc.pos().y());
        if(x>minmax)
            minmax=x;
        if(y>minmax)
            minmax=y;
    }
    return minmax;
}


void drawTotal(const std::vector<SCWrapper>& scwappers){
    float range=getrange(scwappers)*1.05;//cm in both dir
    int nbins =1200;
    float increment = (2*range)/(float)nbins;

    gStyle->SetOptStat(0);
    TH2D * energy = new TH2D("","",nbins,-range,range,nbins,-range,range);
    for(float i=-range; i<range ; i+=increment){
        for(float j=-range; j<range ; j+=increment){
            float val=0;
            for(const auto& sc: scwappers){
                val+= sc.gaus(i,j);
            }
            energy->Fill(i+increment/2.,j+increment/2.,val);
        }
    }
    gStyle->SetNumberContours(256);
    gStyle->SetPalette(kBird);
    energy->Draw("colz");
}


void drawContours(const std::vector<SCWrapper>& scwappers, const std::vector<SCVertex>& result){
    float range=getrange(scwappers)*1.05;
    int nbins =1200;
    float increment = (2*range)/(float)nbins;

    std::vector<std::vector<float> > totale;
    std::vector<std::vector<std::vector<float> > > fracs;

    std::vector<float> xys;
    for(float i=-range; i<range ; i+=increment){
        xys.push_back(i);
    }

    std::vector<std::vector<int> > maxs;

    for(const auto i:xys){
        std::vector<int> maxi;
        for(const auto j:xys){
            float val=0;
            for(const auto& sc: scwappers){
                val+= sc.gaus(i,j);
            }
            float maxijval = 0;
            int maxij=0;
            int ccres=0;
            for(const auto r: result){
                float ratio = r.energy(i,j)/val;
                if(ratio>maxijval){
                    maxijval=ratio;
                    maxij = ccres;
                }
                ccres++;
            }
            maxi.push_back(maxij);
        }
        maxs.push_back(maxi);
    }


    //now contours, as a graph
    std::vector<float> xs,ys;
    int previousint = -1;
    for(size_t i=0;i<xys.size();i++){
        for(size_t j=0;j<xys.size();j++){
            if(previousint!=maxs.at(i).at(j)){
                if(previousint>=0){
                    xs.push_back(xys.at(i));
                    ys.push_back(xys.at(j));
                }
                previousint=maxs.at(i).at(j);
            }
        }
    }

    TGraph * g = new TGraph(xs.size(), &xs.at(0), &ys.at(0));
    g->Draw("P");


}

template<class T>
void splitByZ(const std::vector<T>& in, std::vector<T>& pos, std::vector<T>& neg){
    for(const auto & scw: in){
        if(scw.z()>0)
            pos.push_back(scw);
        else
            neg.push_back(scw);
    }
}

template<>
void splitByZ<SCVertex*>(const std::vector<SCVertex*>& in,
        std::vector<SCVertex*>& pos, std::vector<SCVertex*>& neg){
    for(const auto & scw: in){
        if(!scw->valid())
            continue;
        if(scw->z()>0)
            pos.push_back(scw);
        else
            neg.push_back(scw);
    }
}

/*
 * END DEBUG PART
 */



HGCalSimClusterMerger::HGCalSimClusterMerger(const HGCRecHitCollection& rechits,
           const hgcal::RecHitTools * rechittools,
           const SimHistoryTool * hist
    ):rechits_(&rechits),rechittools_(rechittools),histtool_(hist){
        createHitMap();

        cNLayers_=3;
        cSearchRadius_=3.;
        cClusterRadiusScale_=1.2;
        cMergeRadiusScale_=1.;
        cEContainment_=0.68;

}

std::vector<SimCluster> HGCalSimClusterMerger::merge(const std::vector<const SimCluster*>& scs,
        float isHighEfracThreshold, float connectThreshold, float expandWidth,
        std::vector<std::vector<size_t> >& idxs,
        bool debugplots) const{


    //create the wrappers


    HitMergeVertex::connectThreshold=connectThreshold;
    HitMergeVertex::singleClusterEfrac=isHighEfracThreshold;


    std::cout << "merging " << scs.size() << std::endl;//DEBUG

    std::vector<SCWrapper> scwappers;

    std::vector<float> debugradii, debugenergies;

    size_t idx=0;
    for(const auto& sc: scs){
        float e=0;//fraction weighted energy in hits
        float etot=0;
        // for isolated shower, both are (almost) the same
        float firsthitr=0;
        float r = calcCircle(sc,e,etot,firsthitr) * (1.+expandWidth);
        //etot as closest to center total hit energy
        if(r<0.){
            r=1e-3;
            e=0;
            etot=1e-6;
        }

        scwappers.push_back(SCWrapper(sc,e,etot,firsthitr,idx));

        debugradii.push_back(r);
        debugenergies.push_back(e);

        idx++;
    }

    //since we're using a gaussian approx here, also total energies need to be adapted
    //N^2 loop, but rather straight forwards, could be binned
    for(auto& scw : scwappers){
        float otherenergy=0;
        for(const auto& rhscw: scwappers){
            //explicitly allow self-reference
            otherenergy+=rhscw.gaus(scw.pos());//fast enough to just evaluate
        }
        scw.setEtot(otherenergy);
    }

    MergeGraph<SCVertex > graph;

    //const SimClusterMergeWrapper* t,
  //  const std::vector<RecHit> & allhits,
  //  const std::vector<std::pair<DetId,float> >& hafs
    //add the vertices

    //just a duplicate for the moment
    //TBI performance remove duplicate
    std::vector<HGCRecHit> allhits;
    for(size_t i=0;i<rechits_->size();i++)
        allhits.push_back((*rechits_)[i]);


    auto start = std::chrono::high_resolution_clock::now();//DEBUG

    for(const auto& scw: scwappers){
        auto hafs = scw.SC()->hits_and_fractions();
        graph.addVertex( SCVertex(&scw, allhits, hafs) );
    }

    graph.createEdges();

    auto debugpremerge = graph.vertices();


    graph.merge();


    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "took "<< duration.count()/1000. <<" milliseconds for "<< scwappers.size()<< " showers." << std::endl;

    //collect the result
    std::vector<SimCluster> out;
    idxs.clear();
    //merge done. get result
    auto result = graph.vertices();
    for(const auto& vert: result){
        if(!vert.valid())
            continue;
        std::vector<size_t> tmpidxs;
        std::vector<const SimCluster* > tomerge;
        for(const auto scw: vert.objects()){
            tmpidxs.push_back(scw->idx());
            tomerge.push_back(scw->SC());
        }
        out.push_back(histtool_->createMergedSimCluster(tomerge));
        idxs.push_back(tmpidxs);
    }

    std::cout << "merged from " << scwappers.size() << " to " << out.size() << std::endl;//DEBUG

   // return out;

    if(!debugplots)
        return out;
    static int counter=-1;
    if(true || counter>2)
        return out;//only a few debug plots

    std::cout << "prepare debug plots..." << std::endl;
    //select by endcap
    std::vector<SCWrapper> posscwrapper, negscwrapper;
    std::vector<SCVertex> posv,negv, preposv, prenegv;

    splitByZ(scwappers,posscwrapper,negscwrapper);
    splitByZ(debugpremerge, preposv,prenegv);
    splitByZ(result, posv,negv);

    std::cout << "debug plots" << std::endl;
    counter++;
    //validation stuff
    TCanvas cv;
    drawTotal(posscwrapper);
    drawVertices(preposv,false);
    cv.Print((std::to_string(counter)+"_pos_pre.pdf").data());

    drawTotal(posscwrapper);
    drawVertices(posv,true,true);
    drawContours(posscwrapper,posv);
    cv.Print((std::to_string(counter)+"_pos_post.pdf").data());

    //validation stuff
    drawTotal(negscwrapper);
    drawVertices(prenegv,false);
    cv.Print((std::to_string(counter)+"_neg_pre.pdf").data());

    drawTotal(negscwrapper);
    drawVertices(negv,true,true);
    drawContours(negscwrapper,negv);
    cv.Print((std::to_string(counter)+"_neg_post.pdf").data());

    std::cout << "debug plots done" << std::endl;

    return out;

    //DEBUG OUTPUT, dumb json
    std::ofstream f("file"+std::to_string(counter)+".dat");


    f << "[";

    int nm=idxs.size();
    for(const auto& ii:idxs){
        f << "[";
        int nc=ii.size();
        for(const auto& i:ii){
            const auto& pos = scs.at(i)->g4Tracks().at(0).getPositionAtBoundary();
            f << "[" <<  pos.x() <<", " << pos.y() << ", " << pos.z() << ", " << debugradii.at(i) << ", "<< debugenergies.at(i)<< "]";
            if(nc>1)
                f<<", ";
            nc--;
        }
        if(nm>1)
            f << "],";
        else
            f << "]";
        nm--;
    }

    f << "]";
    f.close();
    //
    return out;
}


double HGCalSimClusterMerger::calcCircle(const SimCluster* sc, float& assignedEnSum, float& regionEnSum, float& firsthitradius)const{
    auto hafs = sc->hits_and_fractions();

    // restrict to simclusters with at least one hit in the HGCAL
    if(!isHGCal(*sc))
        return -1;


    auto bpos4v= sc->g4Tracks().at(0).getPositionAtBoundary();
    LocalVector boundaryPos(bpos4v.x(),bpos4v.y(),bpos4v.z());

    LocalVector axis(sc->impactMomentum().x(),sc->impactMomentum().y(),sc->impactMomentum().z());
    axis=axis.unit();
    //new way: simply distance to boundary crossing as cClusterRadiusScale_ and within that cEContainment_

    std::vector<float> hitDistToBoundary;
    std::vector<float> hitDistanceToAxis;

    assignedEnSum=0;
    std::vector<HitStruct> scHits;
    for(const auto haf:hafs){
        const HGCRecHit* rh = getHit(haf.first);
        bool ishgcal = isHGCal(haf.first);
        if(!rh || !ishgcal) continue;

        auto pos = getHitPosVec(rh->detid());

        float dist = (pos-boundaryPos).mag();
        if(dist > cMergeRadiusScale_)
            continue;

        float radius = getHitRadius(haf.first);
        scHits.emplace_back(haf.first, rh, haf.second, radius);

        hitDistToBoundary.push_back(dist);

        auto rel_hit_pos = pos - boundaryPos;
        auto projection_along_axis = rel_hit_pos.dot(axis) * axis;
        float dist_to_axis = (rel_hit_pos-projection_along_axis).mag();

        hitDistanceToAxis.push_back(dist_to_axis);

        assignedEnSum+=rh->energy() * haf.second;
    }

    if(scHits.size()<1){
        return -1;
        firsthitradius=1;
    }

    auto ldsort = argsort(hitDistToBoundary);//hitDistanceToAxis);//hitDistToBoundary);//hitDistanceToAxis);
    apply_argsort_in_place(scHits,ldsort);
    apply_argsort_in_place(hitDistanceToAxis,ldsort);
    apply_argsort_in_place(hitDistToBoundary,ldsort);//just in case

    firsthitradius=scHits.at(0).radius;

    float tmpensum=0;
    float currentradius=0;
    int lasti=0;
    for(size_t i=0;i<scHits.size();i++){
        const auto& h=scHits.at(i);
        tmpensum += h.energy();
        if(tmpensum/assignedEnSum > cEContainment_){
            break;
        }
        //for all other hits, create width energy weighted
        if(currentradius<hitDistanceToAxis.at(i))
            currentradius=hitDistanceToAxis.at(i);
        lasti=i;
    }

    if(currentradius < scHits.at(0).radius)
        currentradius = scHits.at(0).radius;


    //now we have a width, let's recaluclate energy sums under gaussian assumption
    assignedEnSum=0;
    regionEnSum=0;
    for(size_t i=0;i<(size_t)lasti;i++){
        const float& dist = hitDistanceToAxis.at(i);
        float mult = exp(-dist*dist/(2.*currentradius*currentradius));
        const auto& h=scHits.at(i);
        assignedEnSum += mult*h.energy();
        regionEnSum += mult*h.energy()/(h.fraction+1e-9);
    }

    return currentradius;


    /*
    for(const auto& h: scHits){

        assignedEnSum += h.energy();
        regionEnSum += h.energy()/(h.fraction+1e-9);



    }


    // fill information for the hits belonging to the simcluster
    // and determine first layer while at it
    std::vector<HitStruct> scHits;
    std::vector<int> layershit;
    for(const auto haf:hafs){
        const HGCRecHit* rh = getHit(haf.first);
        bool ishgcal = isHGCal(haf.first);
        if(!rh || !ishgcal) continue;

        int layer = rechittools_->getLayer(haf.first);
        if(std::find(layershit.begin(),layershit.end(),layer)==layershit.end())
            layershit.push_back(layer);
        float radius = getHitRadius(haf.first);
        scHits.emplace_back(haf.first, rh, haf.second, radius);

    }

    if(scHits.size()<1)
        return -1;

    std::sort(layershit.begin(),layershit.end());

    int lastlayer=0;
    if(layershit.size()>=(size_t)cNLayers_)
        lastlayer = layershit.at(cNLayers_-1);
    else if(layershit.size())
        lastlayer = layershit.at(layershit.size()-1);

*/
    //create the reference point and axis


    // store only the hits relevant to calculate the radii
    // also determine their distance to the shower axis
    // and their deposited energy sum

    /*
     *
     * check what happens if you sort by z distance to boundary first.
     * Layer assignment could be broken
     *
     */
    /*
    std::vector<const HitStruct*> assignedHits;
    std::vector<float> hitDistanceToAxis;
    assignedEnSum = 0;
    regionEnSum = 0;
    for(const auto& h: scHits){
       // if((int)rechittools_->getLayer(h.id) > lastlayer)
       //     continue;

        assignedEnSum += h.energy();
        regionEnSum += h.energy()/(h.fraction+1e-9);

        auto rel_hit_pos = getHitPosVec(h.id) - boundaryPos;
        auto projection_along_axis = rel_hit_pos.dot(axis) * axis;
        float dist_to_axis = (rel_hit_pos-projection_along_axis).mag();

        assignedHits.push_back(&h);
        hitDistanceToAxis.push_back(dist_to_axis);

    }

    if(assignedHits.size()<1)
        return -1;

    auto ldsort = argsort(hitDistanceToAxis);//hitDistanceToAxis);
    apply_argsort_in_place(assignedHits,ldsort);
    apply_argsort_in_place(hitDistanceToAxis,ldsort);

    auto centralHit = assignedHits.at(0);

    auto centralHitPos = getHitPosVec(centralHit->id);
    if(hitDistanceToAxis.at(0) > cSearchRadius_ * centralHit->radius)
        return centralHit->radius; //not dense hit

 //   LocalVector centralHitPos = getHitPosVec(centralHit->id);

    double ensum=0;
    double currentradius=0;
    float hitradius = 1e-6;

    //determine region energy sum using Gaussian weighting


    for (size_t i = 0; i < assignedHits.size(); i++) {
        auto hit = assignedHits.at(i);

        //same as hitDistanceToAxis.at(i);// FIXME
        auto hit_pos = getHitPosVec(hit->id) - centralHitPos;//centralHitPos;//now w.r.t. central hit
        auto projection_along_axis = hit_pos.dot(axis) * axis;
        float dist = (hit_pos-projection_along_axis).mag();

        hitradius = hit->radius;
        double rsum = currentradius + cClusterRadiusScale_ * hitradius;
        double reldist = dist / (rsum+1e-6);

        if(reldist>1.)
            break;// done

        ensum += hit->energy();
        if(ensum/assignedEnSum > cEContainment_){
            break;
        }

        if(dist > currentradius)
            currentradius=dist;
    }

    currentradius+=hitradius; //add the last hit radius
    if(currentradius>10.)
        std::cout << "large radius " << currentradius << std::endl;//DEBUG
    return currentradius;
    */

}

bool HGCalSimClusterMerger::isHGCal(const SimCluster& cluster)const{
    for (const auto& hitsAndEnergies : cluster.hits_and_fractions()) {
        const DetId id = hitsAndEnergies.first;
        if(isHGCal(id))
            return true;
    }
    return false;
}

bool HGCalSimClusterMerger::isHGCal(DetId id)const{
    return id.det() == DetId::HGCalEE
            || id.det() == DetId::HGCalHSi
            || id.det() == DetId::HGCalHSc
            || (id.det() == DetId::Forward && id.subdetId() != static_cast<int>(HFNose))
            || (id.det() == DetId::Hcal && id.subdetId() == HcalSubdetector::HcalEndcap);
}

float HGCalSimClusterMerger::getHitRadius(DetId id)const{
    if(rechittools_->isSilicon(id)){
        return rechittools_->getRadiusToSide(id);
    }
    else if(rechittools_->isScintillator(id)){
        auto hit_pos = getHitPosVec(id);
        auto etaphi = rechittools_->getScintDEtaDPhi(id);
        float maxdetadphi = std::max(etaphi.first,etaphi.second);
        return 0.5 * hit_pos.perp() * maxdetadphi * sqrt(2.);
    }
    return 0;
}


void HGCalSimClusterMerger::createHitMap(){
  hitmap_.clear();
  for (size_t i = 0; i < rechits_->size(); i++)
    hitmap_[(*rechits_)[i].detid()] = i;
}



const HGCRecHit* HGCalSimClusterMerger::getHit(DetId id) const {
  auto it = hitmap_.find(id);
  if (it == hitmap_.end())
      return 0;
  return &(*rechits_)[it->second];
}

