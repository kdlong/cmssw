/*
    
  "testFiles/DrellYan_LO_MGMLMv233_2016_weightInfo.txt";
  "testFiles/DrellYan_LO_MGMLMv242_2017_weightInfo.txt";
  
  
  "testFiles/W1jet_260_weight_info.txt";
  "testFiles/WJ_MiNNLO_weightinfo.txt";
  "testFiles/WJ_NNLOPS_weightinfo.txt";  
  "testFiles/WZVBS_2017_weightInfo.txt";
  "testFiles/WZVBS_aQGCFM_weightInfo.txt";
  "testFiles/WZVBS_aQGC_weightInfo.txt";
  "testFiles/WZVBS_private_weightInfo.txt";
  "testFiles/ZJ_MiNNLO_weightinfo.txt";
  "testFiles/ZZTo4L_powheg_2016_weightInfo.txt";
  "testFiles/ZZTo4L_powheg_2017_weightInfo.txt";
  "testFiles/susyweights.txt";


 "testFiles/DrellYan_NLO_MGFXFXv242_2017_weightInfo.txt";



 "testFiles/VVV_2017_weight_info.txt"; // possibly bad file?
*/

void lhe_test() {
    
    vector<string> files = {//"testFiles/DrellYan_LO_MGMLMv233_2016_weightInfo.txt",
                            "testFiles/DrellYan_LO_MGMLMv242_2017_weightInfo.txt",
                            // "testFiles/W1jet_260_weight_info.txt",
                            // "testFiles/WJ_MiNNLO_weightinfo.txt",
                            // "testFiles/WJ_NNLOPS_weightinfo.txt",
                            // "testFiles/WZVBS_2017_weightInfo.txt",
                            // "testFiles/WZVBS_aQGCFM_weightInfo.txt",
                            // "testFiles/WZVBS_aQGC_weightInfo.txt",
                            // "testFiles/WZVBS_private_weightInfo.txt",
                            // "testFiles/ZJ_MiNNLO_weightinfo.txt",
                            // "testFiles/ZZTo4L_powheg_2016_weightInfo.txt",
                            // "testFiles/ZZTo4L_powheg_2017_weightInfo.txt",
                            // "testFiles/susyweights.txt",
                            // "testFiles/DrellYan_NLO_MGFXFXv242_2017_weightInfo.txt"
    };
    for(auto infile: files) {
        test::Test_LHEWeightHelper a;
        std::cout << "-------------------------------------------\n";
        std::cout << infile << "\n";
        a.add_file(infile);
        a.weight_.parseWeights();
    }
    
}


/* Tested
   "testFiles/DrellYan_NLO_MGFXFXv233_2016_weightInfo.txt";
*/
