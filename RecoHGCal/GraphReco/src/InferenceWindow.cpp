/*
 * InferenceWindow.cpp
 *
 *  Created on: 26 Sep 2019
 *      Author: jkiesele
 */



#include "../interface/InferenceWindow.h"

InferenceWindow::InferenceWindow(float centerEta, float centerPhi,
        float outerRegionDEta, float outerRegionDPhi, float innerRegionDEta,
        float innerRegionDPhi) :
        WindowBase(centerEta, centerPhi, outerRegionDEta, outerRegionDPhi,
                innerRegionDEta, innerRegionDPhi)
/* more inits */
{
}

InferenceWindow::~InferenceWindow() {

}

//static
std::vector<InferenceWindow> InferenceWindow::createWindows(size_t nSegmentsPhi,
            size_t nSegmentsEta, double minEta, double maxEta, double frameWidthEta,
            double frameWidthPhi){
    return WindowBase::createWindows<InferenceWindow>(nSegmentsPhi,nSegmentsEta,
            minEta,maxEta,frameWidthEta,frameWidthPhi);
}


