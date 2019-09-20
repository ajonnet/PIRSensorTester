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
#include <opencv2/opencv.hpp>

class utils {
public:
    static std::string strForTime(time_t timeVal, std::string frmt="%Y%m%d_%H%M%S");
    static std::string strForTime(struct std::tm timeVal, std::string frmt="%Y%m%d_%H%M%S");
    
    static cv::Mat imgWithText(std::string msg, cv::Mat img, cv::Scalar color = cv::Scalar(0,200,200), double fontScale = 0.4);
};
#endif /* utils_hpp */
