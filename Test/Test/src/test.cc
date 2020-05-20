#include "Test/Test/interface/test.h"

namespace test {
    Test_LHEWeightHelper::Test_LHEWeightHelper() {
        
    }

    void Test_LHEWeightHelper::add_file(std::string infileName) {
	std::ifstream myfile;
	myfile.open(infileName);
	std::vector<std::string> lines;
	std::string line;
        while(getline(myfile, line)) {
	    lines.push_back(line);
	}
	weight_.setHeaderLines(lines);
	myfile.close();
    }

    
}

