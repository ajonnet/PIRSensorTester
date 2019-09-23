//
//  utils.hpp
//  PIRSensorTester
//
//  Created by Amit Jain on 19/09/19.
//  Copyright © 2019 ajonnet. All rights reserved.
//

#ifndef utils_hpp
#define utils_hpp

#include <iostream>
#include <iomanip>
#include <vector>

class utils {
public:
    static std::string strForTime(time_t timeVal, std::string frmt="%Y%m%d_%H%M%S");
    static std::string strForTime(struct std::tm timeVal, std::string frmt="%Y%m%d_%H%M%S");
    static bool parseTimeStr(std::string timeStr, time_t &t, std::string frmt="%Y%m%d_%H%M%S");
    static bool parseTimeStr(std::string timeStr, struct std::tm &TM, std::string frmt="%Y%m%d_%H%M%S");
    
    static std::vector<std::string> Split(std::string mystring, std::string delimiter);
    static std::string trim(std::string &line);
};
#endif /* utils_hpp */
