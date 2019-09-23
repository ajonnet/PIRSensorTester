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

class utils {
public:
    static std::string strForTime(time_t timeVal, std::string frmt="%Y%m%d_%H%M%S");
    static std::string strForTime(struct std::tm timeVal, std::string frmt="%Y%m%d_%H%M%S");
    static bool parseTimeStr(std::string timeStr, time_t &t, std::string frmt="%Y%m%d_%H%M%S");
    static bool parseTimeStr(std::string timeStr, struct std::tm &TM, std::string frmt="%Y%m%d_%H%M%S");
};
#endif /* utils_hpp */
