//
//  utils.cpp
//  PIRSensorTester
//
//  Created by Amit Jain on 19/09/19.
//  Copyright © 2019 ajonnet. All rights reserved.
//

#include "utils.hpp"

using namespace std;

string utils::strForTime(time_t timeVal, string frmt) {
    struct tm * timeinfo;
    timeinfo = localtime(&timeVal);
    
    //Generate File Name
    string timeStr = utils::strForTime(*timeinfo, frmt);
    return timeStr;
}

string utils::strForTime(struct std::tm timeVal, string frmt) {
    
    //Generate File Name
    char buf[80];
    strftime(buf, sizeof(buf), frmt.c_str(), &timeVal);
    string timeStr = string(buf);
    return timeStr;
}

cv::Mat utils::imgWithText(std::string msg, cv::Mat img, cv::Scalar color, double _fontScale) {
    
    int baseline=0;
    double fontScale = _fontScale;
    int thickness = 1;
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    
    cv::Size text_size = cv::getTextSize(msg,fontFace,fontScale,thickness,&baseline);
    cv::Point textPos = cv::Point(2,img.rows - text_size.height);
    cv::putText(img, msg, textPos, fontFace, fontScale, color, 1);
    return img;
}
