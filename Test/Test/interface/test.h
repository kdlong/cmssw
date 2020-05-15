#include "GeneratorInterface/Core/interface/LHEWeightHelper.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace test {
    class Test_LHEWeightHelper {
    public:
	Test_LHEWeightHelper();

	void add_file(std::string infileName);
	gen::LHEWeightHelper weight_;
	
    };

}
